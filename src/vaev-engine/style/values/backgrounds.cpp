module;

#include <karm/macros>

export module Vaev.Engine:style.values.backgrounds;

import Karm.Core;
import Karm.Math;

import :css.values.backgrounds;
import :css.values.calc;
import :css.values.images;
import :style.values.numeric;
import :values.calc;
import :values.unitValue;

using namespace Vaev::Literals;
using namespace Karm;

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

export Math::Radii<PixelPercentage> toComputed(Math::Radii<Css::LengthPercentage> const& value, LengthContext auto const& ctx) {
    return {
        toComputed(value.a, ctx),
        toComputed(value.b, ctx),
        toComputed(value.c, ctx),
        toComputed(value.d, ctx),
        toComputed(value.e, ctx),
        toComputed(value.f, ctx),
        toComputed(value.g, ctx),
        toComputed(value.h, ctx),
    };
}

export Math::Radii<Css::LengthPercentage> toSpecified(Math::Radii<PixelPercentage> const& value) {
    return {
        toSpecified(value.a),
        toSpecified(value.b),
        toSpecified(value.c),
        toSpecified(value.d),
        toSpecified(value.e),
        toSpecified(value.f),
        toSpecified(value.g),
        toSpecified(value.h),
    };
}

// https://drafts.csswg.org/css-values/#position
// The computed value is a pair of offsets from the top left origin.

export struct Position {
    PixelPercentage x;
    PixelPercentage y;

    static Position center() {
        return {Percent{50}, Percent{50}};
    }
};

export Position toComputed(Css::BackgroundPosition const& value, LengthContext auto const& ctx) {
    auto offset = [&](bool isCenter, bool isFromEnd, Css::LengthPercentage const& length) -> PixelPercentage {
        if (isCenter)
            return Percent{50};

        if (isFromEnd) {
            // calc(100% - <length-percentage>)
            return toComputed(
                Css::LengthPercentage{
                    UNCHECKED,
                    SumNode{
                        .children = {
                            UnitValue{100, UnitValue::Unit::PERCENT},
                            NegateNode{makeBox<Calc>(length.calcTree())},
                        },
                    },
                },
                ctx
            );
        }

        return toComputed(length, ctx);
    };

    return {
        offset(
            value.horizontalAnchor.is<Keywords::Center>().has(),
            value.horizontalAnchor.is<Keywords::Right>().has(),
            value.horizontal
        ),
        offset(
            value.verticalAnchor.is<Keywords::Center>().has(),
            value.verticalAnchor.is<Keywords::Bottom>().has(),
            value.vertical
        ),
    };
}

export Css::BackgroundPosition toSpecified(Position const& value) {
    return {toSpecified(value.x), toSpecified(value.y)};
}

export struct BackgroundLayer {
    Opt<Css::Image> image;
    Css::BackgroundAttachment attachment = Keywords::FIXED;
    Position position = {Percent{0}, Percent{0}};
    Css::BackgroundRepeat repeat;
};

export Css::BackgroundLayer toSpecified(BackgroundLayer const& value) {
    return {value.image, value.attachment, toSpecified(value.position), value.repeat};
}

} // namespace Vaev::Style
