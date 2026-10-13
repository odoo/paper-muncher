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
export using BoxSizing = Union<Keywords::ContentBox, Keywords::BorderBox>;

// https://www.w3.org/TR/css-sizing-3/#propdef-width
// https://www.w3.org/TR/css-sizing-3/#propdef-height
export using Size = Union<Keywords::Auto, Calc<PercentOr<Length>>, Keywords::Stretch, Keywords::MinContent, Keywords::MaxContent, Keywords::FitContent>;
export using MaxSize = Union<Keywords::None, Calc<PercentOr<Length>>, Keywords::Stretch, Keywords::MinContent, Keywords::MaxContent, Keywords::FitContent>;

export struct SizingProps {
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
