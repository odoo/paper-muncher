#pragma once

#include <karm-io/text.h>

namespace marK::Logger::_Embed {

void loggerLock();

void loggerUnlock();

Io::TextWriter& loggerOut();

} // namespace Karm::Logger::_Embed
