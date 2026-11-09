export module Vaev.Engine:layout.values;

import Karm.Gfx;
import Karm.Math;
import Karm.Logger;

import :style.values;
import :layout.formating;

namespace Vaev::Layout {

// MARK: Resolve during layout -------------------------------------------------

export bool isPurePercentage(Style::PxPercent const& calc) {
    // FIXME: If the calc is a non-linear tree, it should check inside if it contains percentages.
    return calc.is<Style::Percent>() or calc.is<Style::LinearPxPercent>();
}

export Au resolve(Style::Px const& value) {
    return Au::fromFloatNearest(value.value());
}

export Vec2Au resolve(Math::Vec2<Style::Px> const& value) {
    return {resolve(value.x), resolve(value.y)};
}

export Style::Px toPixel(Au value) {
    return Style::Px{value.cast<f64>()};
}

export Math::Vec2<Style::Px> toPixel(Vec2Au value) {
    return {toPixel(value.x), toPixel(value.y)};
}

export Opt<Au> resolve(Style::PxPercent const& value, Opt<Au> relative) {
    return value.visit(
        [](Style::Px const& px) -> Opt<Au> {
            return Some(Au{px.value()});
        },
        [&](Style::Percent const& percentage) -> Opt<Au> {
            if (auto const [r] = relative)
                return Some(Au{r.cast<f64>() * (percentage.value() / 100.)});
            return NONE;
        },
        [&](Style::LinearPxPercent const& lin) -> Opt<Au> {
            if (auto const [r] = relative)
                return Some(Au{lin.px.value() + r.cast<f64>() * (lin.percentage.value() / 100.)});
            return NONE;
        },
        [&](Karm::Box<Style::Calc> const& calc) -> Opt<Au> {
            auto simplified = Style::simplify(
                *calc,
                NONE,
                Some(Style::NumericBaseType::LENGTH),
                relative.map([](Au r) {
                    return r.cast<f64>();
                })
            );

            if (auto n = simplified.is<Style::UnitValue>(); n and n->unit() == Style::UnitValue::Unit::PX)
                return Some(Au{n->value()});
            return NONE;
        }
    );
}

export Au resolve(Style::PxPercent const& value, Au relative) {
    if (auto const [au] = resolve(value, Opt<Au>{Some(relative)}))
        return au;
    panic("calc didn't simplify to a single value of expected unit");
}

export Au resolve(Union<Style::Keywords::Auto, Style::PxPercent> const& value, Au relative) {
    if (value.is<Style::Keywords::Auto>())
        return 0_au;
    return resolve(value.expect<Style::PxPercent>(), relative);
}

} // namespace Vaev::Layout
