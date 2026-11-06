export module Vaev.Engine:values.insets;

import Karm.Core;

import :values.keywords;
import :values.primitives;

using namespace Karm;

namespace Vaev {

export struct RunningPosition {
    CustomIdent customIdent;

    void repr(Io::Emit& e) const {
        e("running '{}'", customIdent);
    }
};

// https://www.w3.org/TR/CSS22/visuren.html#propdef-position
export using Position = Union<Keywords::Static, Keywords::Relative, Keywords::Absolute, Keywords::Fixed, Keywords::Sticky, RunningPosition>;

} // namespace Vaev
