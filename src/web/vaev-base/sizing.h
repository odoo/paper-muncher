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
    CalcValue<PercentOr<Length>> value;

    void repr(Io::Emit& e) const {
        e("(fit-content {})", value);
    }
};

// https://www.w3.org/TR/css-sizing-3/#propdef-width
// https://www.w3.org/TR/css-sizing-3/#propdef-height
// auto | <length-percentage [0,∞]> | min-content | max-content | fit-content(<length-percentage [0,∞]>) |
using Size = FlatUnion<Keywords::Auto, CalcValue<PercentOr<Length>>, Keywords::MinContent, Keywords::MaxContent, FitContent>;
using MaxSize = FlatUnion<Keywords::None, CalcValue<PercentOr<Length>>, Keywords::MinContent, Keywords::MaxContent, FitContent>;

// struct Size {
//     enum struct Type : u8 {
//         NONE,
//         AUTO,
//         LENGTH,
//         MIN_CONTENT,
//         MAX_CONTENT,
//         FIT_CONTENT,
//     };

//     using enum Type;

//     Type type;
//     CalcValue<PercentOr<Length>> value;

//     constexpr Size() : type(AUTO), value(Length{}) {
//     }

//     constexpr Size(Type type) : type(type), value(Length{}) {
//     }

//     Size(CalcValue<PercentOr<Length>> value) : type(LENGTH), value(value) {
//     }

//     constexpr Size(Percent value) : type(LENGTH), value(value) {
//     }

//     constexpr Size(Length value) : type(LENGTH), value(value) {
//     }

//     bool operator==(Type type) const {
//         return this->type == type;
//     }

//     void repr(Io::Emit& e) const {
//         switch (type) {
//         case Type::NONE:
//             e("none");
//             break;

//         case Type::AUTO:
//             e("auto");
//             break;

//         case Type::LENGTH:
//             e("{}", value);
//             break;

//         case Type::MIN_CONTENT:
//             e("min-content");
//             break;

//         case Type::MAX_CONTENT:
//             e("max-content");
//             break;

//         case Type::FIT_CONTENT:
//             e("fit-content");
//             break;

//         default:
//             e("unknown");
//             break;
//         }
//     }
// };

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
