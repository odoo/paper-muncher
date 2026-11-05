export module Vaev.Engine:layout.values;

import Karm.Gfx;
import Karm.Math;
import Karm.Logger;

import :values;
import :layout.formating;

namespace Vaev::Layout {

// MARK: Resolve during layout -------------------------------------------------

export bool isPurePercentage(PixelPercentage const& calc) {
    // FIXME: If the calc is a non-linear tree, it should check inside if it contains percentages.
    return calc.is<Percent>() or calc.is<LinearPixelPercentage>();
}

export Au resolve(Pixel const& value) {
    return Au::fromFloatNearest(value.value());
}

export Au resolve(PixelPercentage const& value, Au relative) {
    return Vaev::resolve(value, relative);
}

export Au resolve(Union<Keywords::Auto, PixelPercentage> const& value, Au relative) {
    if (value.is<Keywords::Auto>())
        return 0_au;
    return Vaev::resolve(value.expect<PixelPercentage>(), relative);
}

export Opt<Au> resolve(PixelPercentage const& value, Opt<Au> relative) {
    return Vaev::resolve(value, relative);
}

} // namespace Vaev::Layout
