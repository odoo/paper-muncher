module;

#include <karm-gfx/font.h>

export module Vaev.Engine:style.computed;

import Karm.Core;

using namespace Karm;

namespace Vaev::Style {

// https://www.w3.org/TR/css-cascade/#computed
export struct ComputedValues {
    Rc<Gfx::Fontface> fontFace;
};

} // namespace Vaev::Style
