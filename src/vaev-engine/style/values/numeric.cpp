export module Vaev.Engine:style.values.numeric;

import Karm.Core;

import :css.values.calc;
import :css.values.numeric;
import :css.values.unitValue;
import :style.values.calc;
import :style.values.unitValue;
import :style.values.base;

using namespace Karm;

namespace Vaev::Style {

// MARK: Number ---------------------------------------------------------------
// https://drafts.csswg.org/css-values/#numbers

export template <>
struct ValueComputer<Css::Number> {
    static f64 compute(Css::Number const& value) {
        auto simplified = simplify(value.calcTree(), NONE, NONE);

        if (auto n = simplified.is<UnitValue>(); n and n->unit() == UnitValue::Unit::NONE) {
            return n->value();
        } else {
            panic("calc didn't simplify to a single value of expected unit");
        }
    }

    static Css::Number specify(f64 value) {
        return Css::number(value);
    }
};

// MARK: Percentage ------------------------------------------------------------
// https://drafts.csswg.org/css-values/#percentages

export using Percent = Distinct<f64, struct _PercentTag>;

export template <>
struct ValueComputer<Css::Percentage> {
    static Percent compute(Css::Percentage const& value) {
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

    static f64 compute(Css::Percentage const& value, f64 percentBasis) {
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

    static Css::Percentage specify(Percent const& value) {
        return Css::percent(value.value());
    }
};

// MARK: Length ---------------------------------------------------------------
// https://drafts.csswg.org/css-values/#lengths

// FIXME: Make Pixel and Percentage backed by f32.
export using Px = Distinct<f64, struct _PixelTag>;

export template <>
struct ValueComputer<Css::LegacyLength> {
    static Px compute(Css::LegacyLength const& value, LengthContext auto const& ctx) {
        return Px{simplify(value.unitValue(), ctx).value()};
    }
};

export template <>
struct ValueComputer<Css::Length> {
    static Px compute(Css::Length const& value, LengthContext auto const& ctx) {
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

    static Opt<Px> compute(Css::Length const& value) {
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

    static Css::Length specify(Px const& px) {
        return Css::Length{UNCHECKED, UnitValue{px.value(), UnitValue::Unit::PX}};
    }
};

// MARK: Angle ----------------------------------------------------------------
// https://drafts.csswg.org/css-values/#angles

export using Deg = Distinct<f64, struct _DegTag>;

export template <>
struct ValueComputer<Css::Angle> {
    static Deg compute(Css::Angle const& value) {
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

    static Css::Angle specify(Deg const& angle) {
        return Css::Angle{UNCHECKED, UnitValue{angle.value(), UnitValue::Unit::DEG}};
    }
};

// MARK: Resolution -----------------------------------------------------------
// https://drafts.csswg.org/css-values/#resolution

export using Dppx = Distinct<f64, struct _DppxTag>;

export template <>
struct ValueComputer<Css::Resolution> {
    static Dppx compute(Css::Resolution const& value) {
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
};

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
export template <>
struct ValueComputer<Css::LengthPercentage> {
    static PxPercent compute(Css::LengthPercentage const& value, LengthContext auto const& ctx) {
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

    static Px compute(Css::LengthPercentage const& value, LengthContext auto const& ctx, f64 percentBasis) {
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

    static Css::LengthPercentage specify(PxPercent const& value) {
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
};

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
