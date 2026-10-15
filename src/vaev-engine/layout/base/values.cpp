export module Vaev.Engine:layout.values;

import Karm.Math;
import Karm.Logger;

import :values;
import :layout.formating;

namespace Vaev::Layout {

// MARK: Resolve during layout -------------------------------------------------

export bool isPurePercentage(Calc<Au, Percent> const& calcValue) {
    return calcValue.is<Percent>().has();
}

export Au resolve(Union<Keywords::Auto, Calc<Au, Percent>> const& value, Au relative) {
    if (value.is<Keywords::Auto>())
        return 0_au;
    return resolve(value.expect<Calc<Au, Percent>>(), relative);
}

} // namespace Vaev::Layout
