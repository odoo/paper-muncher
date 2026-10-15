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
import :values.resolved;
import :values.writing;

using namespace Karm;
using namespace Karm::Math::Literals;

namespace Vaev {

// https://www.w3.org/TR/css-sizing-3/#box-sizing
export using BoxSizing = Union<Keywords::ContentBox, Keywords::BorderBox>;

// https://www.w3.org/TR/css-sizing-3/#propdef-width
// https://www.w3.org/TR/css-sizing-3/#propdef-height
export using Size = Union<Keywords::Auto, Calc<Length, Percent>, Keywords::Stretch, Keywords::MinContent, Keywords::MaxContent, Keywords::FitContent>;
export using MaxSize = Union<Keywords::None, Calc<Length, Percent>, Keywords::Stretch, Keywords::MinContent, Keywords::MaxContent, Keywords::FitContent>;

export struct SizingProps {
    Computed<Size> width = Keywords::AUTO, height = Keywords::AUTO;
    Computed<Size> minWidth = Keywords::AUTO, minHeight = Keywords::AUTO;
    Computed<MaxSize> maxWidth = Keywords::NONE, maxHeight = Keywords::NONE;

    Computed<Size>& size(Axis axis) {
        return axis == Axis::HORIZONTAL ? width : height;
    }

    Computed<Size> const size(Axis axis) const {
        return axis == Axis::HORIZONTAL ? width : height;
    }

    Computed<Size>& minSize(Axis axis) {
        return axis == Axis::HORIZONTAL ? minWidth : minHeight;
    }

    Computed<Size> const minSize(Axis axis) const {
        return axis == Axis::HORIZONTAL ? minWidth : minHeight;
    }

    Computed<MaxSize>& maxSize(Axis axis) {
        return axis == Axis::HORIZONTAL ? maxWidth : maxHeight;
    }

    Computed<MaxSize> const maxSize(Axis axis) const {
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
