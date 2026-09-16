export module Vaev.Idl:base;

import Karm.Core;

using namespace Karm;

namespace Vaev::Idl {

// https://webidl.spec.whatwg.org/#idl-any
export using Any = None; // TODO

// https://webidl.spec.whatwg.org/#idl-DOMString
export using DOMString = String;

// https://webidl.spec.whatwg.org/#idl-ByteString
export using BytesString = String;

// https://webidl.spec.whatwg.org/#idl-USVString
export using USVString = String;

} // namespace Vaev::Idl
