#pragma once

#include <karm-io/emit.h>

#include "calc.h"
#include "length.h"
#include "percent.h"
#include "vaev-base/keywords.h"
#include "writing.h"

namespace Vaev {

// https://www.w3.org/TR/css-sizing-3/#box-sizing
enum struct BoxSizing : u8 {
    CONTENT_BOX,
    BORDER_BOX,
};

struct FitContent {
    CalcValue<PercentOr<Length>> value = CalcValue<PercentOr<Length>>(Length{});

    void repr(Io::Emit& e) const {
        e("(fit-content {})", value);
    }
};

// https://www.w3.org/TR/css-sizing-3/#propdef-width
// https://www.w3.org/TR/css-sizing-3/#propdef-height
using Size = FlatUnion<Keywords::Auto, CalcValue<PercentOr<Length>>, Keywords::MinContent, Keywords::MaxContent, FitContent>;
using MaxSize = FlatUnion<Keywords::None, CalcValue<PercentOr<Length>>, Keywords::MinContent, Keywords::MaxContent, FitContent>;

struct SizingProps {
    Size width = Keywords::Auto{}, height = Keywords::Auto{};
    Size minWidth = Keywords::Auto{}, minHeight = Keywords::Auto{};
    MaxSize maxWidth = Keywords::None{}, maxHeight = Keywords::None{};

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
