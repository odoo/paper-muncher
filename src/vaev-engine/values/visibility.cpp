export module Vaev.Engine:values.visibility;

import Karm.Core;

using namespace Karm;

namespace Vaev {

export enum struct Visibility : u8 {
    VISIBLE,
    HIDDEN,
    COLLAPSE,

    _LEN
};

} // namespace Vaev
