module;

#include <karm/macros>

export module Vaev.Engine:style.values.calc2;

import Karm.Core;

import :css.values.calc2;
import :style.values.length;
import :style.values.percent;
import :values.calc2;
import :values.unitValue;

using namespace Karm;
using namespace Vaev::Literals;

namespace Vaev::Style {

// FIXME: Make Pixel and Percentage backed by f32.
export using Rad = Distinct<f64, struct _RadTag>;

export struct LinearPixelPercentage {
    Pixel px;
    Percent percentage;

    void repr(Io::Emit& e) const {
        e("({} + {})", px, percentage);
    }
};

export f64 toComputed(Css::Number const& value) {
    auto simplified = simplify(value.calcTree(), NONE, NONE);

    if (auto n = simplified.is<UnitValue>(); n and n->unit() == UnitValue::Unit::NONE) {
        return n->value();
    } else {
        panic("calc didn't simplify to a single value of expected unit");
    }
}

export Rad toComputed(Css::Angle const& value) {
    auto simplified = simplify(
        value.calcTree(),
        NONE,
        Some(NumericBaseType::ANGLE),
        NONE
    );

    if (auto n = simplified.is<UnitValue>(); n and n->baseType() == NumericBaseType::ANGLE) {
        return Rad{n->to(UnitValue::Unit::RAD)};
    } else {
        panic("calc didn't simplify to a single value of expected unit");
    }
}

export Css::Angle toSpecified(Rad const& angle) {
    return Css::Angle{UNCHECKED, UnitValue{angle.value(), UnitValue::Unit::RAD}};
}

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

} // namespace Vaev::Style
