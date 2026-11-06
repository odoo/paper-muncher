export module Vaev.Engine:values.writing;

import Karm.Core;

import :values.keywords;

using namespace Karm;

namespace Vaev {

// https://drafts.csswg.org/css-writing-modes-4/#inline-axis
// https://drafts.csswg.org/css-writing-modes-4/#block-axis

export struct Axis {
    enum struct _Type {
        HORIZONTAL,
        VERTICAL,

        CROSS = VERTICAL,
        MAIN = HORIZONTAL,

        BLOCK = VERTICAL,
        INLINE = HORIZONTAL,
    };

    using enum _Type;

    _Type value;

    constexpr Axis(_Type value) : value(value) {
    }

    constexpr Axis cross() const {
        return value == HORIZONTAL ? VERTICAL : HORIZONTAL;
    }

    bool operator==(_Type const& value) const {
        return this->value == value;
    }

    bool operator==(Axis const& other) const = default;

    int index() const {
        return value == HORIZONTAL ? 0 : 1;
    }
};

// https://drafts.csswg.org/css-writing-modes-4/#propdef-writing-mode
export using WritingMode = Union<Keywords::HorizontalTb, Keywords::VerticalRl, Keywords::VerticalLr, Keywords::SidewaysRl, Keywords::SidewaysLr>;

// https://drafts.csswg.org/css-writing-modes-4/#propdef-direction
export using Direction = Union<Keywords::Ltr, Keywords::Rtl>;

} // namespace Vaev
