#pragma once

#include "request.h"
#include "response.h"

namespace Karm::Net::Http {

struct Service {
    virtual ~Service() = default;
    virtual Async::Task<> handleAsync(Rc<Request>, Rc<Response::Writer>) = 0;
};

struct Server {
    static Rc<Server> simple(Rc<Service> srv);

    Rc<Service> _srv;
    virtual ~Server() = default;
    virtual Async::Task<> serveAsync() = 0;
};

// MARK: Serverless ------------------------------------------------------------

Async::Task<> servAsync(Rc<Service> srv);

} // namespace Karm::Net::Http
