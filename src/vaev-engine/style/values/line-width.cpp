module;

#include <karm/macros>

export module Vaev.Engine:style.values.lineWidth;

import Karm.Core;

import :css.values.calc2;
import :css.values.lineWidth;
import :style.values.calc2;
import :style.values.length;
import :values.unitValue;

using namespace Vaev::Literals;

namespace Vaev::Style {

export Pixel toComputed(Css::LineWidth const& value, LengthContext auto const& ctx) {
    return value.visit(
        [](Keywords::Thin const&) {
            return 1_px;
        },
        [](Keywords::Medium const&) {
            return 3_px;
        },
        [](Keywords::Thick const&) {
            return 5_px;
        },
        [&](Css::Length const& length) {
            return toComputed(length, ctx);
        }
    );
}

} // namespace Vaev::Style
