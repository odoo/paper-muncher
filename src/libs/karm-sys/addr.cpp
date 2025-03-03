#include "addr.h"

#include "_embed.h"

namespace Karm::Sys {

// MARK: IP --------------------------------------------------------------------

Async::Task<Vec<Ip>> Ip::lookupAsync(Str host) {
    return _Embed::ipLookupAsync(host);
}

} // namespace Karm::Sys
