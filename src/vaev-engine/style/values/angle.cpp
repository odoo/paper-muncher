module;

#include <karm/macros>

export module Vaev.Engine:style.values.angle;

import Karm.Core;

import :css.values.unitValue;

using namespace Karm;

namespace Vaev::Style {

// 7.1. MARK: Angle Units: the <angle> type and deg, grad, rad, turn units
// https://drafts.csswg.org/css-values/#angles

export using Rad = Distinct<f64, struct _RadTag>;

Rad toComputed(Css::PureAngle const& value, [[maybe_unused]] auto const& ctx = NONE) {
    return Rad{value.toRadian()};
}

} // namespace Vaev::Style
