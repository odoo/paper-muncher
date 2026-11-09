export module Vaev.Engine:style.values.sizing;

import Karm.Core;

import :css.values.sizing;
import :style.values.keywords;
import :style.values.numeric;

using namespace Karm;

// https://drafts.csswg.org/css-sizing-3/

namespace Vaev::Style {

// https://www.w3.org/TR/css-sizing-3/#box-sizing
export using BoxSizing = Css::BoxSizing;

export using Size = Union<Keywords::Auto, PxPercent, Keywords::Stretch, Keywords::MinContent, Keywords::MaxContent, Keywords::FitContent>;

export using MaxSize = Union<Keywords::None, PxPercent, Keywords::Stretch, Keywords::MinContent, Keywords::MaxContent, Keywords::FitContent>;

} // namespace Vaev::Style
