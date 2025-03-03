#pragma once

#include "request.h"
#include "response.h"

namespace Karm::Net::Http {

struct Client {
    static Rc<Client> simple();

    virtual ~Client() = default;

    virtual Async::Task<Rc<Response>> doAsync(Rc<Request> request) = 0;

    Async::Task<Rc<Response>> getAsync(Mime::Url url);

    Async::Task<Rc<Response>> headAsync(Mime::Url url);

    Async::Task<Rc<Response>> postAsync(Mime::Url url, Rc<Body> body);
};

// MARK: Clientless ------------------------------------------------------------

Async::Task<Rc<Response>> getAsync(Mime::Url url);

Async::Task<Rc<Response>> headAsync(Mime::Url url);

Async::Task<Rc<Response>> postAsync(Mime::Url url, Rc<Body> body);

} // namespace Karm::Net::Http
