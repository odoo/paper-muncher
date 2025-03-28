module;

#include <karm-base/base.h>

export module Vaev.Style:origin;

namespace Vaev::Style {

// https://drafts.csswg.org/css-cascade/#origin
export enum struct Origin {
    // NOTE: The order of these values is important
    USER_AGENT,
    USER,
    AUTHOR,
    INLINE, //< Declarations from style attributes
};

export std::strong_ordering operator<=>(Origin a, Origin b) {
    return static_cast<u8>(a) <=> static_cast<u8>(b);
}

export bool operator==(Origin a, Origin b) {
    return static_cast<u8>(a) == static_cast<u8>(b);
}

} // namespace Vaev::Style
