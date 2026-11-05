module;

#include <karm/macros>

export module Vaev.Engine:values.flex;

import Karm.Core;

import :css;
import :values.base;
import :values.calc2;
import :values.keywords;
import :values.sizing;
import :values.unitValue;

using namespace Karm;

namespace Vaev {

// MARK: Flexible Length -------------------------------------------------------
// https://drafts.csswg.org/css-grid/#typedef-flex


// MARK: FlexDirection ---------------------------------------------------------
// https://drafts.csswg.org/css-flexbox-1/#flex-direction-property

export using FlexDirection = Union<
    Keywords::Row,
    Keywords::RowReverse,
    Keywords::Column,
    Keywords::ColumnReverse>;


// MARK: FlexWrap --------------------------------------------------------------
// https://drafts.csswg.org/css-flexbox-1/#flex-wrap-property

export using FlexWrap = Union<
    Keywords::Nowrap,
    Keywords::Wrap,
    Keywords::WrapReverse>;


export using FlexBasis = Union<
    Keywords::Auto,
    Keywords::Content,
    Keywords::MinContent,
    Keywords::MaxContent,
    Keywords::FitContent,
    LengthPercentage>;

namespace Style {

export using FlexBasis = Union<
    Keywords::Auto,
    Keywords::Content,
    Keywords::MinContent,
    Keywords::MaxContent,
    Keywords::FitContent,
    PixelPercentage>;

} // namespace Style

export Style::FlexBasis resolve(FlexBasis const& value, LengthContext auto const& ctx) {
    return value.visit([&](auto const& v) -> Style::FlexBasis { return resolve(v, ctx); });
}

export FlexBasis toSpecified(Style::FlexBasis const& value) {
    return value.visit([](auto const& v) -> FlexBasis { return toSpecified(v); });
}

namespace Style {

export struct Flex {
    // FlexContainer
    FlexDirection direction = Keywords::ROW;
    FlexWrap wrap = Keywords::NOWRAP;

    // FlexItem
    Style::FlexBasis basis = Keywords::AUTO;
    f64 grow = 0;
    f64 shrink = 1;

    bool isRowOriented() const {
        return direction == Keywords::ROW or
               direction == Keywords::ROW_REVERSE;
    }

    bool isReverse() const {
        return direction == Keywords::ROW_REVERSE or
               direction == Keywords::COLUMN_REVERSE;
    }
};

} // namespace Style
} // namespace Vaev
