export module Vaev.Engine:values.specified.percentageMix;

import :values.specified.angle;
import :values.specified.base;
import :values.specified.length;
import :values.specified.percentage;

using namespace Karm;
using namespace Vaev::Experimental::Literals;

namespace Vaev::Experimental {

// FIXME: All mixed percentage types should allow calc()

export using LengthPercentage = Union<Length, Percentage>;

export template <>
struct ValueTraits<LengthPercentage> {
    using ComputedType = Union<Px, Percentage>;

    static ComputedType compute(LengthPercentage const& lengthPercentage, ComputationContext const& ctx, Style::ComputedValues const& computedValues) {
        return lengthPercentage.visit(
            [&](Length const& length) -> ComputedType {
                return computeValue(length, ctx, computedValues);
            },
            [](Percentage const& percentage) -> ComputedType {
                if (percentage.value() == 0.0) {
                    return 0_px;
                }

                return percentage;
            }
        );
    }

    static LengthPercentage fromComputed(ComputedType const& computed) {
        return computed.visit(
            [&](Px const& px) -> LengthPercentage {
                return valueFromComputed<Length>(px);
            },
            [](Percentage const& percentage) -> LengthPercentage {
                return percentage;
            }
        );
    }

    static Res<LengthPercentage> parse(Cursor<Css::Sst>& c) {
        return parseOneOf<LengthPercentage>(c);
    }
};

using AnglePercentage = Union<Angle, Percentage>;

export template <>
struct ValueTraits<AnglePercentage> {
    using ComputedType = Union<Degree, Percentage>;

    static ComputedType compute(AnglePercentage const& anglePercentage, ComputationContext const& ctx, Style::ComputedValues const& computedValues) {
        return anglePercentage.visit(
            [&](Angle const& angle) -> ComputedType {
                return computeValue(angle, ctx, computedValues);
            },
            [](Percentage const& percentage) -> ComputedType {
                return percentage;
            }
        );
    }

    static AnglePercentage fromComputed(ComputedType const& computed) {
        return computed.visit(
            [&](Degree const& deg) -> AnglePercentage {
                return valueFromComputed<Angle>(deg);
            },
            [](Percentage const& percentage) -> AnglePercentage {
                return percentage;
            }
        );
    }

    static Res<LengthPercentage> parse(Cursor<Css::Sst>& c) {
        return parseOneOf<LengthPercentage>(c);
    }
};

} // namespace Vaev::Experimental

