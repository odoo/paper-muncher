module;

#include <karm-async/task.h>
#include <karm-base/rc.h>
#include <karm-logger/logger.h>
#include <karm-mime/url.h>
#include <karm-sys/chan.h>
#include <karm-sys/file.h>
#include <karm-sys/lookup.h>
#include <karm-sys/socket.h>

export module Karm.Http:client;

import Karm.Aio;
import :request;
import :response;

namespace Karm::Http {

export struct Client {
    virtual ~Client() = default;

    virtual Async::Task<Rc<Response>> doAsync(Rc<Request> request) = 0;

    Async::Task<Rc<Response>> getAsync(Mime::Url url) {
        auto req = makeRc<Request>();
        req->method = Method::GET;
        req->url = url;
        req->version = Version{1, 1};
        req->header.add("Host", url.host);

        return doAsync(req);
    }

    Async::Task<Rc<Response>> headAsync(Mime::Url url) {
        auto req = makeRc<Request>();
        req->method = Method::HEAD;
        req->url = url;
        req->version = Version{1, 1};
        req->header.add("Host", url.host);

        return doAsync(req);
    }

    Async::Task<Rc<Response>> postAsync(Mime::Url url, Rc<Body> body) {
        auto req = makeRc<Request>();
        req->method = Method::POST;
        req->url = url;
        req->version = Version{1, 1};
        req->body = body;
        req->header.add("Host", url.host);

        return doAsync(req);
    }
};

// MARK: Simple Client ---------------------------------------------------------

static constexpr usize BUF_SIZE = 4096;

struct ContentBody : public Body {
    Buf<Byte> _resumes;
    usize _resumesPos = 0;
    Sys::TcpConnection _conn;
    usize _contentLength;

    ContentBody(Bytes resumes, Sys::TcpConnection conn, usize contentLength)
        : _resumes(resumes),
          _conn(std::move(conn)),
          _contentLength(contentLength - resumes.len()) {
    }

    Async::Task<usize> readAsync(MutBytes buf) override {
        if (_resumesPos < _resumes.len()) {
            usize n = min(buf.len(), _resumes.len() - _resumesPos);
            copy(sub(_resumes, _resumesPos, n), buf);
            _resumesPos += n;
            co_return n;
        }

        if (_contentLength == 0) {
            co_return 0;
        }

        usize n = min(buf.len(), _contentLength);
        n = co_trya$(_conn.readAsync(mutSub(buf, 0, n)));
        _contentLength -= n;
        co_return n;
    }
};

struct ChunkedBody : public Body {
    Buf<Byte> _buf;
    Sys::TcpConnection _conn;

    ChunkedBody(Bytes resumes, Sys::TcpConnection conn)
        : _buf(resumes), _conn(std::move(conn)) {}
};

struct SimpleClient : public Client {
    Async::Task<> _sendRequest(Request& request, Sys::TcpConnection& conn) {
        Io::StringWriter req;
        co_try$(request.unparse(req));
        co_trya$(conn.writeAsync(req.bytes()));

        if (auto body = request.body)
            co_trya$(Aio::copyAsync(**body, conn));

        co_return Ok();
    }

    Async::Task<Rc<Response>> _recvResponse(Sys::TcpConnection& conn) {
        Array<u8, BUF_SIZE> buf = {};
        Io::BufReader reader = sub(buf, 0, co_trya$(conn.readAsync(buf)));
        auto response = co_try$(Response::read(reader));

        if (auto contentLength = response.header.contentLength()) {
            response.body = makeRc<ContentBody>(reader.bytes(), std::move(conn), contentLength.unwrap());
        } else if (auto transferEncoding = response.header.get("Transfer-Encoding"s)) {
            logWarn("Transfer-Encoding: {} not supported", transferEncoding);
        } else {
            // NOTE: When there is no content length, and no transfer encoding,
            //       we read until the server closes the socket.
            response.body = makeRc<ContentBody>(reader.bytes(), std::move(conn), Limits<usize>::MAX);
        }

        co_return Ok(makeRc<Response>(std::move(response)));
    }

    Async::Task<Rc<Response>> doAsync(Rc<Request> request) override {
        auto& url = request->url;
        if (url.scheme != "http")
            co_return Error::invalidInput("unsupported scheme");

        logDebug("{} {}", request->method, url);

        auto ips = co_trya$(Sys::lookupAsync(url.host));
        auto port = url.port.unwrapOr(80);
        Sys::SocketAddr addr{first(ips), (u16)port};
        auto conn = co_try$(Sys::TcpConnection::connect(addr));
        co_trya$(_sendRequest(*request, conn));
        co_return co_trya$(_recvResponse(conn));
    }
};

export Rc<Client> simpleClient() {
    return makeRc<SimpleClient>();
}

// MARK: Local -----------------------------------------------------------------

struct LocalClient : public Client {
    Res<Rc<Body>> _load(Mime::Url url) {
        if (try$(Sys::isFile(url)))
            return Ok(Body::from(try$(Sys::File::open(url))));

        auto dir = try$(Sys::Dir::open(url));
        Io::StringWriter sw;
        Io::Emit e{sw};
        e("<html><body><h1>Index of {}</h1><ul>", url.path);
        for (auto& diren : dir.entries()) {
            if (diren.hidden())
                continue;
            e("<li><a href=\"{}\">{}</a></li>", url.join(diren.name), diren.name);
        }
        e("</ul></body></html>");
        return Ok(Body::from(sw.take()));
    }

    Async::Task<Rc<Response>> doAsync(Rc<Request> request) override {
        auto response = makeRc<Response>();

        response->code = Code::OK;
        response->header.add("Content-Type", "text/html");

        if (request->method == Method::GET)
            response->body = co_try$(_load(request->url));

        co_return Ok(response);
    }
};

export Rc<Client> localClient() {
    return makeRc<LocalClient>();
}

// MARK: Fallback --------------------------------------------------------------

struct FallbackClient : public Client {
    Vec<Rc<Client>> _clients;

    FallbackClient(Vec<Rc<Client>> clients) : _clients(std::move(clients)) {}

    Async::Task<Rc<Response>> doAsync(Rc<Request> request) override {
        for (auto& client : _clients) {
            auto res = co_await client->doAsync(request);
            if (res)
                co_return res.unwrap();
        }

        co_return Error::notFound("no client could handle the request");
    }
};

export Rc<Client> fallbackClient(Vec<Rc<Client>> clients) {
    return makeRc<FallbackClient>(std::move(clients));
}

// MARK: Clientless ------------------------------------------------------------

export Async::Task<Rc<Response>> getAsync(Mime::Url url) {
    return simpleClient()->getAsync(url);
}

export Async::Task<Rc<Response>> headAsync(Mime::Url url) {
    return simpleClient()->headAsync(url);
}

export Async::Task<Rc<Response>> postAsync(Mime::Url url, Rc<Body> body) {
    return simpleClient()->postAsync(url, body);
}

export Async::Task<Rc<Response>> doAsync(Rc<Request> request) {
    return simpleClient()->doAsync(request);
}

} // namespace Karm::Http
