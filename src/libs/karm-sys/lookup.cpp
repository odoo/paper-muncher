#include "_embed.h"
#include "addr.h"

namespace marK::Sys {

Async::Task<Vec<Ip>> lookupAsync(Str host) {
    return _Embed::ipLookupAsync(host);
}

} // namespace marK::Sys
