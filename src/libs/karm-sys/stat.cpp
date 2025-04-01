#include "stat.h"

#include "_embed.h"
#include "proc.h"

namespace marK::Sys {

Res<Stat> stat(Mime::Url const& url) {
    try$(ensureUnrestricted());
    return _Embed::stat(url);
}

} // namespace marK::Sys
