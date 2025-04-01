#include <karm-logger/_embed.h>
#include <karm-sys/chan.h>

namespace marK::Logger::_Embed {

void loggerLock() {}

void loggerUnlock() {}

Io::TextWriter& loggerOut() {
    return Sys::err();
}

} // namespace marK::Logger::_Embed
