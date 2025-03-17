module;

#include <karm-async/task.h>
#include <karm-base/rc.h>
#include <karm-logger/logger.h>
#include <karm-mime/url.h>
#include <karm-sys/chan.h>
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
        } else {
            response.body = NONE;
        }

        co_return Ok(makeRc<Response>(std::move(response)));
    }

    Async::Task<Rc<Response>> doAsync(Rc<Request> request) override {
        auto& url = request->url;
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
