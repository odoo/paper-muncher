#pragma once

namespace marK {
struct Backtrace;
} // namespace Karm

namespace marK::_Embed {

// MARK: Backtrace -------------------------------------------------------------

Backtrace captureBacktrace();

Backtrace forceCaptureBacktrace();

// MARK: Locks -----------------------------------------------------------------

void relaxe();

void enterCritical();

void leaveCritical();

} // namespace Karm::_Embed
