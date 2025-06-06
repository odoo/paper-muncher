#pragma once

#include <karm-text/font.h>

namespace Vaev::Style {

// https://www.w3.org/TR/css-cascade/#computed
struct ComputedValues {
    Rc<Text::Fontface> fontFace;
};

} // namespace Vaev::Style