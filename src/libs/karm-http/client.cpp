#include "client.h"

namespace Karm::Net::Http {

Async::Task<Rc<Response>> Client::getAsync(Mime::Url url) {
    auto req = makeRc<Request>();
    req->method = Method::GET;
    req->url = url;
    req->version = Version{1, 1};
    return doAsync(req);
}

Async::Task<Rc<Response>> Client::headAsync(Mime::Url url) {
    auto req = makeRc<Request>();
    req->method = Method::HEAD;
    req->url = url;
    req->version = Version{1, 1};
    return doAsync(req);
}

Async::Task<Rc<Response>> Client::postAsync(Mime::Url url, Rc<Body> body) {
    auto req = makeRc<Request>();
    req->method = Method::POST;
    req->url = url;
    req->version = Version{1, 1};
    req->body = body;
    return doAsync(req);
}

// MARK: Clientless ------------------------------------------------------------

Async::Task<Rc<Response>> getAsync(Mime::Url url) {
    return Client::simple()->getAsync(url);
}

Async::Task<Rc<Response>> headAsync(Mime::Url url) {
    return Client::simple()->headAsync(url);
}

Async::Task<Rc<Response>> postAsync(Mime::Url url, Rc<Body> body) {
    return Client::simple()->postAsync(url, body);
}

} // namespace Karm::Http
