export module Vaev.Fetch:headers;

import Vaev.Idl;
import Vaev.Script;
import Karm.Core;
using namespace Karm;

namespace Vaev::Fetch {

// https://fetch.spec.whatwg.org/#typedefdef-headersinit
export using HeadersInit = Map<Idl::BytesString, Idl::BytesString>;

} // namespace Vaev::Fetch