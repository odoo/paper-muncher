export module Vaev.Engine:style.values.primitives;

import Karm.Core;

import :css.values.primitives;

using namespace Karm;

namespace Vaev::Style {

// https://drafts.csswg.org/css-values/#integers
export using Integer = isize;

export Integer toComputed(Integer value) {
    return value;
}

// https://www.w3.org/TR/css-values-4/#identifier-value
export using CustomIdent = Css::CustomIdent;

} // namespace Vaev::Style
