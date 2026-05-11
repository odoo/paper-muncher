module;

#include <karm/macros>

export module Vaev.Engine:values.lineWidth;

import Karm.Math;

import :css;
import :values.base;
import :values.calc;
import :values.keywords;
import :values.length;

using namespace Karm::Math::Literals;

namespace Vaev {

// MARK: Line Width -------------------------------------------------------------
// https://drafts.csswg.org/css-backgrounds/#typedef-line-width

// FIXME: Calc is missing
export using LineWidth = Union<
    Keywords::Thin,
    Keywords::Medium,
    Keywords::Thick,
    Length>;

export template <>
struct ValueTraits<LineWidth> {
    using ComputedType = Px;

    static Res<LineWidth> parse(Cursor<Css::Sst>& c) {
        return parseOneOf<LineWidth>(c);
    }

    static ComputedType compute(LineWidth const& lineWidth, ComputationContext const& ctx) {
        return lineWidth.visit(Visitor{
            [](Keywords::Thin) {
                return Px(1);
            },
            [](Keywords::Medium) {
                return Px(3);
            },
            [](Keywords::Thick) {
                return Px(5);
            },
            [&](Length const& length) {
                return computeValue(length, ctx).snappedAsBorderWidth();
            }
        });
    }

    static LineWidth fromComputed(ComputedType const& computed) {
        return Length(computed.value(), Length::PX);
    }
};

} // namespace Vaev
