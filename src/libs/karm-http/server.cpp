#include "server.h"

namespace Karm::Net::Http {

// MARK: Serverless ------------------------------------------------------------

Async::Task<> servAsync(Rc<Service> srv) {
    return Server::simple(srv)->serveAsync();
}

} // namespace Karm::Http
