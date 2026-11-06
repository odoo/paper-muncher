module;

#include <karm/macros>

export module Vaev.Engine:style.values.font;

import Karm.Core;

import :css.values.font;
import :css.values.unitValue;
import :style.values.length;
import :values.unitValue;

using namespace Vaev::Literals;

namespace Vaev::Style {

export struct FontSizeContextData {
    f64 userFontSize = 16;   /// Font size of the user agent
    f64 parentFontSize = 16; /// Font size of the parent box
};

export template <typename T>
concept FontSizeContext = LengthContext<T> and requires(T t) {
    { t.userFontSize } -> Meta::Convertible<f64>;
    { t.parentFontSize } -> Meta::Convertible<f64>;
};

export Pixel toComputed(Css::FontSize const& value, FontSizeContext auto const& ctx) {
    return value.visit(
        [&](Css::PureLength const& v) {
            return toComputed(v, ctx);
        },
        [&](Css::PurePercentage const& v) {
            return Pixel{ctx.parentFontSize * (v.value() / 100.0)};
        },
        [&](Keywords::XxSmall) {
            return Pixel{ctx.userFontSize * 0.5};
        },
        [&](Keywords::XSmall) {
            return Pixel{ctx.userFontSize * 0.75};
        },
        [&](Keywords::Small) {
            return Pixel{ctx.userFontSize * 0.875};
        },
        [&](Keywords::Medium) {
            return Pixel{ctx.userFontSize};
        },
        [&](Keywords::Large) {
            return Pixel{ctx.userFontSize * 1.125};
        },
        [&](Keywords::XLarge) {
            return Pixel{ctx.userFontSize * 1.25};
        },
        [&](Keywords::XxLarge) {
            return Pixel{ctx.userFontSize * 1.5};
        },
        [&](Keywords::Larger) {
            return Pixel{ctx.parentFontSize * 1.25};
        },
        [&](Keywords::Smaller) {
            return Pixel{ctx.parentFontSize * 0.875};
        }
    );
}

} // namespace Vaev::Style
