export module Vaev.Engine:style.values.position;

import Karm.Core;

import :css.values.position;

using namespace Karm;

// https://drafts.csswg.org/css-position-3/

namespace Vaev::Style {

export using ZIndex = Css::ZIndex;

// https://www.w3.org/TR/CSS22/visuren.html#propdef-position
export using PositionScheme = Css::PositionScheme;

} // namespace Vaev::Style
