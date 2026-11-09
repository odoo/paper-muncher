export module Vaev.Engine:style.values.numeric;

import Karm.Core;

import :css.values.calc;
import :css.values.numeric;
import :css.values.unitValue;
import :style.values.calc;
import :style.values.unitValue;

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

export Css::Number toSpecified(f64 value) {
    return Css::number(value);
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

export Css::Percentage toSpecified(Percent const& value) {
    return Css::percent(value.value());
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
export using Px = Distinct<f64, struct _PixelTag>;

export Px toComputed(Css::PureLength const& value, LengthContext auto const& ctx) {
    return Px{simplify(value.unitValue(), ctx).value()};
}

export Px toComputed(Css::Length const& value, LengthContext auto const& ctx) {
    auto simplified = simplify(
        value.calcTree(),
        ctx,
        Some(NumericBaseType::LENGTH),
        NONE
    );

    if (auto n = simplified.template is<UnitValue>(); n and n->unit() == UnitValue::Unit::PX) {
        return Px{n->value()};
    } else {
        panic("calc didn't simplify to a single value of expected unit");
    }
}

export Opt<Px> toComputed(Css::Length const& value) {
    auto simplified = simplify(
        value.calcTree(),
        NONE,
        Some(NumericBaseType::LENGTH),
        NONE
    );

    if (auto n = simplified.is<UnitValue>(); n and n->unit() == UnitValue::Unit::PX)
        return Some(Px{n->value()});

    return NONE;
}

export Css::Length toSpecified(Px const& px) {
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

export Dppx toComputed(Css::Resolution const& value) {
    auto simplified = simplify(
        value.calcTree(),
        NONE,
        Some(NumericBaseType::RESOLUTION),
        NONE
    );

    if (auto n = simplified.is<UnitValue>(); n and n->unit() == UnitValue::Unit::DPPX) {
        return Dppx{n->value()};
    } else {
        panic("calc didn't simplify to a single value of expected unit");
    }
}

// MARK: Length-Percentage ----------------------------------------------------
// https://drafts.csswg.org/css-values/#typedef-length-percentage

export struct LinearPxPercent {
    Px px;
    Percent percentage;

    void repr(Io::Emit& e) const {
        e("({} + {})", px, percentage);
    }
};

export using PxPercent = Union<Px, Percent, LinearPxPercent, Box<Calc>>;

// https://drafts.csswg.org/css-values-4/#calc-computed-value
export PxPercent toComputed(Css::LengthPercentage const& value, LengthContext auto const& ctx) {
    auto simplified = simplify(
        value.calcTree(),
        ctx,
        Some(NumericBaseType::LENGTH),
        NONE
    );

    if (auto [numeric] = simplified.template is<UnitValue>()) {
        if (numeric.unit() == UnitValue::Unit::PX) {
            return Px{numeric.value()};
        }

        if (numeric.unit() == UnitValue::Unit::PERCENT) {
            return Percent{numeric.value()};
        }
    }

    // NOTE: Children are sorted by simplification, so the percentage comes first.
    if (auto sum = simplified.template is<Css::SumNode>(); sum and sum->children.len() == 2) {
        auto maybePercent = sum->children[0].template is<UnitValue>();
        auto maybePx = sum->children[1].template is<UnitValue>();
        if (maybePercent and maybePx and maybePercent->unit() == UnitValue::Unit::PERCENT and maybePx->unit() == UnitValue::Unit::PX) {
            return LinearPxPercent{Px{maybePx->value()}, Percent{maybePercent->value()}};
        }
    }

    return makeBox<Calc>(std::move(simplified));
}

export Css::LengthPercentage toSpecified(PxPercent const& value) {
    return value.visit(
        [](Px const& px) -> Css::LengthPercentage {
            return Css::LengthPercentage{UNCHECKED, UnitValue{px.value(), UnitValue::Unit::PX}};
        },
        [](Percent const& percentage) -> Css::LengthPercentage {
            return Css::LengthPercentage{UNCHECKED, UnitValue{percentage.value(), UnitValue::Unit::PERCENT}};
        },
        [](LinearPxPercent const& lin) -> Css::LengthPercentage {
            return Css::LengthPercentage{
                UNCHECKED,
                Css::SumNode{
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
export Px toComputed(Css::LengthPercentage const& value, LengthContext auto const& ctx, f64 percentBasis) {
    auto simplified = simplify(
        value.calcTree(),
        ctx,
        Some(NumericBaseType::LENGTH),
        Some(percentBasis)
    );

    if (auto n = simplified.template is<UnitValue>(); n and n->unit() == UnitValue::Unit::PX) {
        return Px{n->value()};
    } else {
        panic("calc didn't simplify to a single value of expected unit");
    }
}

} // namespace Vaev::Style

namespace Vaev::Literals {

export constexpr Style::Px operator""_px(unsigned long long val) {
    return Style::Px{static_cast<f64>(val)};
}

export constexpr Style::Px operator""_px(long double val) {
    return Style::Px{static_cast<f64>(val)};
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
struct Karm::Io::Formatter<Vaev::Style::Px> {
    Res<> format(Io::TextWriter& writer, Vaev::Style::Px value) {
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
