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

export enum struct FlexDirection {
    ROW,
    ROW_REVERSE,
    COLUMN,
    COLUMN_REVERSE,

    _LEN,
};

export template <>
struct ValueParser<FlexDirection> {
    static Res<FlexDirection> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c.skip(Css::Token::ident("row")))
            return Ok(FlexDirection::ROW);
        else if (c.skip(Css::Token::ident("row-reverse")))
            return Ok(FlexDirection::ROW_REVERSE);
        else if (c.skip(Css::Token::ident("column")))
            return Ok(FlexDirection::COLUMN);
        else if (c.skip(Css::Token::ident("column-reverse")))
            return Ok(FlexDirection::COLUMN_REVERSE);
        else
            return Error::invalidData("expected flex direction");
    }
};

// MARK: FlexWrap --------------------------------------------------------------
// https://drafts.csswg.org/css-flexbox-1/#flex-wrap-property

export enum struct FlexWrap {
    NOWRAP,
    WRAP,
    WRAP_REVERSE,

    _LEN
};

export template <>
struct ValueParser<FlexWrap> {
    static Res<FlexWrap> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c.skip(Css::Token::ident("nowrap")))
            return Ok(FlexWrap::NOWRAP);
        else if (c.skip(Css::Token::ident("wrap")))
            return Ok(FlexWrap::WRAP);
        else if (c.skip(Css::Token::ident("wrap-reverse")))
            return Ok(FlexWrap::WRAP_REVERSE);
        else
            return Error::invalidData("expected flex wrap");
    }
};

export using FlexBasis = Union<
    Keywords::Auto,
    Keywords::Content,
    Keywords::MinContent,
    Keywords::MaxContent,
    Keywords::FitContent,
    LengthPercentage>;

export using ComputedFlexBasis = Union<
    Keywords::Auto,
    Keywords::Content,
    Keywords::MinContent,
    Keywords::MaxContent,
    Keywords::FitContent,
    PixelPercentage>;

export ComputedFlexBasis resolve(FlexBasis const& value, LengthContext auto const& ctx) {
    return value.visit([&](auto const& v) -> ComputedFlexBasis { return resolve(v, ctx); });
}

export FlexBasis toSpecified(ComputedFlexBasis const& value) {
    return value.visit([](auto const& v) -> FlexBasis { return toSpecified(v); });
}

export struct FlexItemProps {
    FlexBasis flexBasis = Keywords::AUTO;
    f64 flexGrow, flexShrink;

    void repr(Io::Emit& e) const {
        e("({} {} {})", flexBasis, flexGrow, flexShrink);
    }
};

export struct FlexProps {
    // FlexContainer
    FlexDirection direction = FlexDirection::ROW;
    FlexWrap wrap = FlexWrap::NOWRAP;

    // FlexItem
    ComputedFlexBasis basis = Keywords::AUTO;
    f64 grow = 0;
    f64 shrink = 1;

    bool isRowOriented() const {
        return direction == FlexDirection::ROW or
               direction == FlexDirection::ROW_REVERSE;
    }

    bool isReverse() const {
        return direction == FlexDirection::ROW_REVERSE or
               direction == FlexDirection::COLUMN_REVERSE;
    }

    void repr(Io::Emit& e) const {
        e("(flex");
        e(" direction={}", direction);
        e(" wrap={}", wrap);
        e(" basis={}", basis);
        e(" grow={}", grow);
        e(" shrink={}", shrink);
        e(")");
    }
};

} // namespace Vaev
