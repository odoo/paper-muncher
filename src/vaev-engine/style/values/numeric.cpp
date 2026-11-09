module;

#include <karm/macros>

export module Vaev.Engine:style.values.numeric;

import Karm.Core;

import :css.values.calc;
import :css.values.unitValue;
import :values.calc;
import :values.unitValue;

using namespace Karm;

// Computed values of the CSS numeric data types.
// https://drafts.csswg.org/css-values/#numeric-types

namespace Vaev::Style {

// MARK: Number ---------------------------------------------------------------
// https://drafts.csswg.org/css-values/#numbers

export f64 toComputed(Css::Number const& value) {
    auto simplified = simplify(value.calcTree(), NONE, NONE);

    if (auto n = simplified.is<UnitValue>(); n and n->unit() == UnitValue::Unit::NONE) {
        return n->value();
    } else {
        panic("calc didn't simplify to a single value of expected unit");
    }
}

// MARK: Percentage ------------------------------------------------------------
// https://drafts.csswg.org/css-values/#percentages

export using Percent = Distinct<f64, struct _PercentTag>;

export Percent toComputed(Css::Percentage const& value) {
    auto simplified = simplify(
        value.calcTree(),
        NONE,
        Some(NumericBaseType::PERCENT),
        NONE
    );

    if (auto n = simplified.is<UnitValue>(); n and n->unit() == UnitValue::Unit::PERCENT) {
        return Percent{n->value()};
    } else {
        panic("calc didn't simplify to a single value of expected unit");
    }
}

export f64 toComputed(Css::Percentage const& value, f64 percentBasis) {
    auto simplified = simplify(
        value.calcTree(),
        NONE,
        NONE,
        Some(percentBasis)
    );

    if (auto n = simplified.is<UnitValue>(); n and n->unit() == UnitValue::Unit::NONE) {
        return n->value();
    } else {
        panic("calc didn't simplify to a single value of expected unit");
    }
}

// MARK: Length ---------------------------------------------------------------
// https://drafts.csswg.org/css-values/#lengths

// FIXME: Make Pixel and Percentage backed by f32.
export using Pixel = Distinct<f64, struct _PixelTag>;

export Pixel toComputed(Css::PureLength const& value, LengthContext auto const& ctx) {
    return Pixel{value.unitValue().canonical(ctx).value()};
}

export Pixel toComputed(Css::Length const& value, LengthContext auto const& ctx) {
    auto simplified = simplify(
        value.calcTree(),
        ctx,
        Some(NumericBaseType::LENGTH),
        NONE
    );

    if (auto n = simplified.template is<UnitValue>(); n and n->unit() == UnitValue::Unit::PX) {
        return Pixel{n->value()};
    } else {
        panic("calc didn't simplify to a single value of expected unit");
    }
}

export Css::Length toSpecified(Pixel const& px) {
    return Css::Length{UNCHECKED, UnitValue{px.value(), UnitValue::Unit::PX}};
}

// MARK: Angle ----------------------------------------------------------------
// https://drafts.csswg.org/css-values/#angles

export using Deg = Distinct<f64, struct _DegTag>;

export Deg toComputed(Css::PureAngle const& value) {
    return Deg{value.unitValue().to(UnitValue::Unit::DEG)};
}

export Deg toComputed(Css::Angle const& value) {
    auto simplified = simplify(
        value.calcTree(),
        NONE,
        Some(NumericBaseType::ANGLE),
        NONE
    );

    if (auto n = simplified.is<UnitValue>(); n and n->unit() == UnitValue::Unit::DEG) {
        return Deg{n->value()};
    } else {
        panic("calc didn't simplify to a single value of expected unit");
    }
}

export Css::Angle toSpecified(Deg const& angle) {
    return Css::Angle{UNCHECKED, UnitValue{angle.value(), UnitValue::Unit::DEG}};
}

// MARK: Resolution -----------------------------------------------------------
// https://drafts.csswg.org/css-values/#resolution

export using Dppx = Distinct<f64, struct _DppxTag>;

export Dppx toComputed(Css::PureResolution const& value) {
    return Dppx{value.unitValue().to(UnitValue::Unit::DPPX)};
}

// MARK: Length-Percentage ----------------------------------------------------
// https://drafts.csswg.org/css-values/#typedef-length-percentage

export struct LinearPixelPercentage {
    Pixel px;
    Percent percentage;

    void repr(Io::Emit& e) const {
        e("({} + {})", px, percentage);
    }
};

export using PixelPercentage = Union<Pixel, Percent, LinearPixelPercentage, Box<Calc>>;

// https://drafts.csswg.org/css-values-4/#calc-computed-value
export PixelPercentage toComputed(Css::LengthPercentage const& value, LengthContext auto const& ctx) {
    auto simplified = simplify(
        value.calcTree(),
        ctx,
        Some(NumericBaseType::LENGTH),
        NONE
    );

    if (auto [numeric] = simplified.template is<UnitValue>()) {
        if (numeric.unit() == UnitValue::Unit::PX) {
            return Pixel{numeric.value()};
        }

        if (numeric.unit() == UnitValue::Unit::PERCENT) {
            return Percent{numeric.value()};
        }
    }

    // NOTE: Children are sorted by simplification, so the percentage comes first.
    if (auto sum = simplified.template is<SumNode>(); sum and sum->children.len() == 2) {
        auto maybePercent = sum->children[0].template is<UnitValue>();
        auto maybePx = sum->children[1].template is<UnitValue>();
        if (maybePercent and maybePx and maybePercent->unit() == UnitValue::Unit::PERCENT and maybePx->unit() == UnitValue::Unit::PX) {
            return LinearPixelPercentage{Pixel{maybePx->value()}, Percent{maybePercent->value()}};
        }
    }

    return makeBox<Calc>(std::move(simplified));
}

export Css::LengthPercentage toSpecified(PixelPercentage const& value) {
    return value.visit(
        [](Pixel const& px) -> Css::LengthPercentage {
            return Css::LengthPercentage{UNCHECKED, UnitValue{px.value(), UnitValue::Unit::PX}};
        },
        [](Percent const& percentage) -> Css::LengthPercentage {
            return Css::LengthPercentage{UNCHECKED, UnitValue{percentage.value(), UnitValue::Unit::PERCENT}};
        },
        [](LinearPixelPercentage const& lin) -> Css::LengthPercentage {
            return Css::LengthPercentage{
                UNCHECKED,
                SumNode{
                    .children = {
                        UnitValue{lin.percentage.value(), UnitValue::Unit::PERCENT},
                        UnitValue{lin.px.value(), UnitValue::Unit::PX},
                    },
                }
            };
        },
        [](Box<Calc> const& calc) -> Css::LengthPercentage {
            return Css::LengthPercentage{UNCHECKED, *calc};
        }
    );
}

// For properties whose percentages resolve at computed-value time (e.g. line-height against font-size).
export Pixel toComputed(Css::LengthPercentage const& value, LengthContext auto const& ctx, f64 percentBasis) {
    auto simplified = simplify(
        value.calcTree(),
        ctx,
        Some(NumericBaseType::LENGTH),
        Some(percentBasis)
    );

    if (auto n = simplified.template is<UnitValue>(); n and n->unit() == UnitValue::Unit::PX) {
        return Pixel{n->value()};
    } else {
        panic("calc didn't simplify to a single value of expected unit");
    }
}

} // namespace Vaev::Style

namespace Vaev::Literals {

export constexpr Style::Pixel operator""_px(unsigned long long val) {
    return Style::Pixel{static_cast<f64>(val)};
}

export constexpr Style::Pixel operator""_px(long double val) {
    return Style::Pixel{static_cast<f64>(val)};
}

export constexpr Style::Deg operator""_deg(unsigned long long val) {
    return Style::Deg{static_cast<f64>(val)};
}

export constexpr Style::Deg operator""_deg(long double val) {
    return Style::Deg{static_cast<f64>(val)};
}

export constexpr Style::Dppx operator""_dppx(unsigned long long val) {
    return Style::Dppx{static_cast<f64>(val)};
}

export constexpr Style::Dppx operator""_dppx(long double val) {
    return Style::Dppx{static_cast<f64>(val)};
}

} // namespace Vaev::Literals

export template <>
struct Karm::Io::Formatter<Vaev::Style::Pixel> {
    Res<> format(Io::TextWriter& writer, Vaev::Style::Pixel value) {
        return Io::format(writer, "{}px", value.value());
    }
};

export template <>
struct Karm::Io::Formatter<Vaev::Style::Percent> {
    Res<> format(Io::TextWriter& writer, Vaev::Style::Percent value) {
        return Io::format(writer, "{}%", value.value());
    }
};

export template <>
struct Karm::Io::Formatter<Vaev::Style::Deg> {
    Res<> format(Io::TextWriter& writer, Vaev::Style::Deg value) {
        return Io::format(writer, "{}deg", value.value());
    }
};

export template <>
struct Karm::Io::Formatter<Vaev::Style::Dppx> {
    Res<> format(Io::TextWriter& writer, Vaev::Style::Dppx value) {
        return Io::format(writer, "{}dppx", value.value());
    }
};
