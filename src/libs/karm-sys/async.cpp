#include <karm-logger/logger.h>

#include "_embed.h"
#include "async.h"

namespace marK::Sys {

Sched& globalSched() {
    return _Embed::globalSched();
}

} // namespace marK::Sys
