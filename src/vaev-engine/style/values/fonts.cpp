module;

#include <karm/macros>

export module Vaev.Engine:style.values.fonts;

import Karm.Core;

import :style.values.numeric;
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

} // namespace Vaev::Style
