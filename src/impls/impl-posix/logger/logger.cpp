#include <karm-logger/_embed.h>
#include <stdio.h>

namespace Karm::Logger::_Embed {

void loggerLock() {}

void loggerUnlock() {}

Io::TextWriter& loggerOut() {
    struct LoggerOut : Io::TextEncoderBase<> {
        Res<usize> write(Bytes buf) override {
            if (fwrite(buf.buf(), 1, buf.len(), stdout) < buf.len()) {
                return Error::other("could not write to stdout");
            }
            return Ok(buf.len());
        }
    };

    static LoggerOut out;
    return out;
}

} // namespace Karm::Logger::_Embed
