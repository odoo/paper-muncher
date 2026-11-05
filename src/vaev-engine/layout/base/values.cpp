export module Vaev.Engine:layout.values;

import Karm.Gfx;
import Karm.Math;
import Karm.Logger;

import :values;
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

export Au resolve(Style::PixelPercentage const& value, Au relative) {
    return Style::resolve(value, relative);
}

export Au resolve(Union<Keywords::Auto, Style::PixelPercentage> const& value, Au relative) {
    if (value.is<Keywords::Auto>())
        return 0_au;
    return Style::resolve(value.expect<Style::PixelPercentage>(), relative);
}

export Opt<Au> resolve(Style::PixelPercentage const& value, Opt<Au> relative) {
    return Style::resolve(value, relative);
}

} // namespace Vaev::Layout
