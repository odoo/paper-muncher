#pragma once

#include <karm-io/emit.h>

#include "calc.h"
#include "keywords.h"
#include "length.h"
#include "percent.h"
#include "writing.h"

namespace Vaev {

// https://www.w3.org/TR/css-sizing-3/#box-sizing
enum struct BoxSizing : u8 {
    CONTENT_BOX,
    BORDER_BOX,
};

// MARK: FitContent
// https://drafts.csswg.org/css-sizing-3/#preferred-size-properties

struct FitContent {
    CalcValue<PercentOr<Length>> value = {Length{}};

    void repr(Io::Emit& e) const {
        e("(fit-content {})", value);
    }
};

template <>
struct ValueParser<FitContent> {
    static Res<FitContent> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c->prefix == Css::Token::function("fit-content(")) {
            FitContent result;
            Cursor<Css::Sst> scan = c->content;
            result.value = try$(parseValue<PercentOr<Length>>(scan));
            c.next();
            return Ok(result);
        }
        return Error::invalidData("invalid fit-content");
    }
};

// https://www.w3.org/TR/css-sizing-3/#propdef-width
// https://www.w3.org/TR/css-sizing-3/#propdef-height
using Size = FlatUnion<Keywords::Auto, CalcValue<PercentOr<Length>>, Keywords::MinContent, Keywords::MaxContent, FitContent>;
using MaxSize = FlatUnion<Keywords::None, CalcValue<PercentOr<Length>>, Keywords::MinContent, Keywords::MaxContent, FitContent>;

struct SizingProps {
    Size width = Keywords::AUTO, height = Keywords::AUTO;
    Size minWidth = Keywords::AUTO, minHeight = Keywords::AUTO;
    MaxSize maxWidth = Keywords::NONE, maxHeight = Keywords::NONE;

    Size& size(Axis axis) {
        return axis == Axis::HORIZONTAL ? width : height;
    }

    Size const size(Axis axis) const {
        return axis == Axis::HORIZONTAL ? width : height;
    }

    Size& minSize(Axis axis) {
        return axis == Axis::HORIZONTAL ? minWidth : minHeight;
    }

    Size const minSize(Axis axis) const {
        return axis == Axis::HORIZONTAL ? minWidth : minHeight;
    }

    MaxSize& maxSize(Axis axis) {
        return axis == Axis::HORIZONTAL ? maxWidth : maxHeight;
    }

    MaxSize const maxSize(Axis axis) const {
        return axis == Axis::HORIZONTAL ? maxWidth : maxHeight;
    }

    void repr(Io::Emit& e) const {
        e("(sizing");
        e(" width={}", width);
        e(" height={}", height);
        e(" minWidth={}", minWidth);
        e(" minHeight={}", minHeight);
        e(" maxWidth={}", maxWidth);
        e(" maxHeight={}", maxHeight);
        e(")");
    }
};

} // namespace Vaev
