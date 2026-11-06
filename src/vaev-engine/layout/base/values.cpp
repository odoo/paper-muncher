export module Vaev.Engine:layout.values;

import Karm.Gfx;
import Karm.Math;
import Karm.Logger;

import :values;
import :css.values;
import :style.values;
import :layout.formating;

namespace Vaev::Layout {

// MARK: Resolve during layout -------------------------------------------------

export bool isPurePercentage(Style::PixelPercentage const& calc) {
    // FIXME: If the calc is a non-linear tree, it should check inside if it contains percentages.
    return calc.is<Style::Percent>() or calc.is<Style::LinearPixelPercentage>();
}

export Au resolve(Style::Pixel const& value) {
    return Au::fromFloatNearest(value.value());
}

export Opt<Au> resolve(Style::PixelPercentage const& value, Opt<Au> relative) {
    return value.visit(
        [](Style::Pixel const& px) -> Opt<Au> {
            return Some(Au{px.value()});
        },
        [&](Style::Percent const& percentage) -> Opt<Au> {
            if (auto const [r] = relative)
                return Some(Au{r.cast<f64>() * (percentage.value() / 100.)});
            return NONE;
        },
        [&](Style::LinearPixelPercentage const& lin) -> Opt<Au> {
            if (auto const [r] = relative)
                return Some(Au{lin.px.value() + r.cast<f64>() * (lin.percentage.value() / 100.)});
            return NONE;
        },
        [&](Karm::Box<Calc> const& calc) -> Opt<Au> {
            auto simplified = simplify(
                *calc,
                NONE,
                Some(NumericBaseType::LENGTH),
                relative.map([](Au r) {
                    return r.cast<f64>();
                })
            );

            if (auto n = simplified.is<UnitValue>(); n and n->unit() == UnitValue::Unit::PX)
                return Some(Au{n->value()});
            return NONE;
        }
    );
}

export Au resolve(Style::PixelPercentage const& value, Au relative) {
    if (auto const [au] = resolve(value, Opt<Au>{Some(relative)}))
        return au;
    panic("calc didn't simplify to a single value of expected unit");
}

export Au resolve(Union<Keywords::Auto, Style::PixelPercentage> const& value, Au relative) {
    if (value.is<Keywords::Auto>())
        return 0_au;
    return resolve(value.expect<Style::PixelPercentage>(), relative);
}

} // namespace Vaev::Layout
