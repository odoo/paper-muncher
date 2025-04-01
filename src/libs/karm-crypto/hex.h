#pragma once

#include <karm-io/text.h>

namespace marK::Crypto {

Res<> hexEncode(Bytes bytes, Io::TextWriter& out);

Res<String> hexEncode(Bytes bytes);

} // namespace Karm::Crypto
