#include "backtrace.h"

#include "_embed.h"

namespace marK {

Backtrace Backtrace::capture() {
    return _Embed::captureBacktrace();
}

Backtrace Backtrace::forceCapture() {
    return _Embed::forceCaptureBacktrace();
}

} // namespace marK
