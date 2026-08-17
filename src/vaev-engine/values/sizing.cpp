module;

#include <karm/macros>

export module Vaev.Engine:values.sizing;

import Karm.Core;

import :css;
import :values.base;
import :values.calc;
import :values.keywords;
import :values.length;
import :values.percent;
import :values.writing;

using namespace Karm;
using namespace Karm::Math::Literals;

namespace Vaev {

// https://www.w3.org/TR/css-sizing-3/#box-sizing
export enum struct BoxSizing : u8 {
    CONTENT_BOX,
    BORDER_BOX,
    _LEN,
};

// MARK: FitContent
// https://drafts.csswg.org/css-sizing-3/#funcdef-width-fit-content
export template <typename Dim>
struct FitContent {
    Calc<Length, Percent> value = Length{0_au};

    void repr(Io::Emit& e) const {
        e("(fit-content {})", value);
    }
};

export template <>
struct ValueTraits<FitContent<Length>> {
    static Res<FitContent<Length>> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c->prefix == Css::Token::function("fit-content(")) {
            FitContent<Length> result;
            Cursor<Css::Sst> scan = c->content;
            result.value = try$(parseValue<Calc<Length, Percent>>(scan));
            c.next();
            return Ok(result);
        }
        return Error::invalidData("invalid fit-content");
    }
};

// https://www.w3.org/TR/css-sizing-3/#propdef-width
// https://www.w3.org/TR/css-sizing-3/#propdef-height
export template <typename Dim>
using Size = Union<Keywords::Auto, Calc<Dim, Percent>, Keywords::MinContent, Keywords::MaxContent, FitContent<Dim>>;
export template <typename Dim>
using MaxSize = Union<Keywords::None, Calc<Length, Percent>, Keywords::MinContent, Keywords::MaxContent, FitContent<Dim>>;

export struct SizingProps {
    Size<Au> width = Keywords::AUTO, height = Keywords::AUTO;
    Size<Au> minWidth = Keywords::AUTO, minHeight = Keywords::AUTO;
    MaxSize<Au> maxWidth = Keywords::NONE, maxHeight = Keywords::NONE;

    Size<Au>& size(Axis axis) {
        return axis == Axis::HORIZONTAL ? width : height;
    }

    Size<Au> const size(Axis axis) const {
        return axis == Axis::HORIZONTAL ? width : height;
    }

    Size<Au>& minSize(Axis axis) {
        return axis == Axis::HORIZONTAL ? minWidth : minHeight;
    }

    Size<Au> const minSize(Axis axis) const {
        return axis == Axis::HORIZONTAL ? minWidth : minHeight;
    }

    MaxSize<Au>& maxSize(Axis axis) {
        return axis == Axis::HORIZONTAL ? maxWidth : maxHeight;
    }

    MaxSize<Au> const maxSize(Axis axis) const {
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
