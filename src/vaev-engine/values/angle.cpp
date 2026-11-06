module;

#include <karm/macros>

export module Vaev.Engine:values.angle;

import Karm.Core;

import :css;
import :values.base;
import :values.unitValue;

using namespace Karm;

namespace Vaev::Css {

// 7.1. MARK: Angle Units: the <angle> type and deg, grad, rad, turn units
// https://drafts.csswg.org/css-values/#angles

export using Rad = Distinct<f64, struct _RadTag>;

} // namespace Vaev::Css

namespace Vaev::Style {

Css::Rad toComputed(Css::PureAngle const& value, [[maybe_unused]] auto const& ctx = NONE) {
    return Css::Rad{value.toRadian()};
}

} // namespace Vaev::Style

