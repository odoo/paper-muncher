module;

#include <karm/macros>

export module Vaev.Engine:css.values.flex;

import Karm.Core;

import :css.parser;
import :css.values.calc;
import :css.values.keywords;
import :values.keywords;

using namespace Karm;

namespace Vaev::Css {

// MARK: Flexible Length -------------------------------------------------------
// https://drafts.csswg.org/css-grid/#typedef-flex

// MARK: FlexDirection ---------------------------------------------------------
// https://drafts.csswg.org/css-flexbox-1/#flex-direction-property

// MARK: FlexWrap --------------------------------------------------------------
// https://drafts.csswg.org/css-flexbox-1/#flex-wrap-property

export using FlexBasis = Union<
    Keywords::Auto,
    Keywords::Content,
    Keywords::MinContent,
    Keywords::MaxContent,
    Keywords::FitContent,
    LengthPercentage>;

} // namespace Vaev::Css
