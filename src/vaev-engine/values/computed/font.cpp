export module Vaev.Engine:values.computed.font;

import Karm.Core;

import :values.common;
import :values.computed.dimensions;

using namespace Karm;
using namespace Vaev::Experimental::Literals;

namespace Vaev::Experimental {

export struct FontProps {
    Vec<Symbol> families = {};
    f64 weight = 0;
    Percentage width = Percentage{0};
    _FontStyle<Degree> style = Keywords::NORMAL;
    Px size = 0_px;
};

} // namespace Vaev::Experimental
