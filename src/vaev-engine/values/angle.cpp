module;

#include <karm/macros>

export module Vaev.Engine:values.angle;

import Karm.Core;

import :css;
import :values.base;
import :values.unitValue;
import :values.resolved;

using namespace Karm;

namespace Vaev {

// 7.1. MARK: Angle Units: the <angle> type and deg, grad, rad, turn units
// https://drafts.csswg.org/css-values/#angles

export using Rad = Distinct<f64, struct _RadTag>;


export template <>
struct _Resolved<PureAngle> {
    using Type = Rad;
};

Rad resolve(PureAngle const& value, [[maybe_unused]] auto const& ctx = NONE) {
    return Rad{value.toRadian()};
}

} // namespace Vaev
