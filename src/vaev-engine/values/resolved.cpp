export module Vaev.Engine:values.resolved;

import Karm.Core;

using namespace Karm;

namespace Vaev {

export template <typename Value, typename Cx>
using Resolved = decltype(resolve(Meta::declval<Value>(), Meta::declval<Cx>()));

} // namespace Vaev
