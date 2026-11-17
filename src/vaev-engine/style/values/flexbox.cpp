export module Vaev.Engine:style.values.flexbox;

import Karm.Core;

import :css.values.flexbox;
import :style.values.keywords;
import :style.values.numeric;

using namespace Karm;

// https://drafts.csswg.org/css-flexbox-1/

namespace Vaev::Style {

// https://drafts.csswg.org/css-flexbox-1/#flex-direction-property
export using FlexDirection = Css::FlexDirection;

// https://drafts.csswg.org/css-flexbox-1/#flex-wrap-property
export using FlexWrap = Css::FlexWrap;

export using FlexBasis = Union<
    Keywords::Auto,
    Keywords::Content,
    Keywords::MinContent,
    Keywords::MaxContent,
    Keywords::FitContent,
    PxPercent>;

} // namespace Vaev::Style
