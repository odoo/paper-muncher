module;

#include <karm-async/task.h>
#include <karm-base/rc.h>
#include <karm-logger/logger.h>
#include <karm-mime/url.h>
#include <karm-sys/lookup.h>
#include <karm-sys/socket.h>

export module Karm.Http:client;

import Karm.Aio;
import :request;
import :response;

namespace Karm::Http {

export struct Client {
    static Rc<Client> simple() {
        struct SimpleClient : public Client {
            Async::Task<Rc<Response>> doAsync(Rc<Request> request) override {
                auto& url = request->url;
                logDebug("sending {} request to {}...", request->method, url);
                auto ips = co_trya$(Sys::lookupAsync(url.host));
                auto port = url.port.unwrapOr(80);
                Sys::SocketAddr addr{first(ips), (u16)port};
                logDebug("connecting to {}", addr);
                auto conn = co_try$(Sys::TcpConnection::connect(addr));
                logDebug("connected", addr);

                Io::StringWriter req;
                co_try$(request->unparse(req));

                co_trya$(conn.writeAsync(req.bytes()));

                if (auto body = request->body) {
                    co_trya$(Aio::copyAsync(**body, conn));
                }

                auto response = makeRc<Response>();

                co_return Ok(response);
            }
        };

        return makeRc<SimpleClient>();
    }

    virtual ~Client() = default;

    virtual Async::Task<Rc<Response>> doAsync(Rc<Request> request) = 0;

    Async::Task<Rc<Response>> getAsync(Mime::Url url) {
        auto req = makeRc<Request>();
        req->method = Method::GET;
        req->url = url;
        req->version = Version{1, 1};
        return doAsync(req);
    }

    Async::Task<Rc<Response>> headAsync(Mime::Url url) {
        auto req = makeRc<Request>();
        req->method = Method::HEAD;
        req->url = url;
        req->version = Version{1, 1};
        return doAsync(req);
    }

    Async::Task<Rc<Response>> postAsync(Mime::Url url, Rc<Body> body) {
        auto req = makeRc<Request>();
        req->method = Method::POST;
        req->url = url;
        req->version = Version{1, 1};
        req->body = body;
        return doAsync(req);
    }
};

// MARK: Clientless ------------------------------------------------------------

export Async::Task<Rc<Response>> getAsync(Mime::Url url) {
    return Client::simple()->getAsync(url);
}

export Async::Task<Rc<Response>> headAsync(Mime::Url url) {
    return Client::simple()->headAsync(url);
}

export Async::Task<Rc<Response>> postAsync(Mime::Url url, Rc<Body> body) {
    return Client::simple()->postAsync(url, body);
}

export Async::Task<Rc<Response>> doAsync(Rc<Request> request) {
    return Client::simple()->doAsync(request);
}

} // namespace Karm::Http
