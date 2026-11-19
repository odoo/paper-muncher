export module Vaev.Engine:style.values.backgrounds;

import Karm.Core;
import Karm.Gfx;
import Karm.Math;

import :css.values.backgrounds;
import :css.values.calc;
import :css.values.numeric;
import :css.values.images;
import :style.values.numeric;
import :css.values.unitValue;
import :style.values.keywords;
import :style.values.color;
import :style.values.base;

using namespace Vaev::Literals;
using namespace Karm;

namespace Vaev::Style {

export template <>
struct ValueComputer<Css::LineWidth> {
    static Px compute(Css::LineWidth const& value, LengthContext auto const& ctx) {
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
                return computeValue(length, ctx);
            }
        );
    }

    static Css::LineWidth specify(Px value) {
        return specifyValue<Css::Length>(value);
    }
};

// https://drafts.csswg.org/css-values/#position
// The computed value is a pair of offsets from the top left origin.

export struct Position {
    PxPercent x;
    PxPercent y;

    static Position center() {
        return {Percent{50}, Percent{50}};
    }

    void repr(Io::Emit& e) const {
        e("(position {} {})", x, y);
    }
};

export template <>
struct ValueComputer<Css::BackgroundPosition> {
    static Position compute(Css::BackgroundPosition const& value, LengthContext auto const& ctx) {
        auto offset = [&](bool isCenter, bool isFromEnd, Css::LengthPercentage const& length) -> PxPercent {
            if (isCenter)
                return Percent{50};

            if (isFromEnd) {
                return computeValue(
                    Css::LengthPercentage{
                        UNCHECKED,
                        Css::SumNode{
                            .children = {
                                UnitValue{100, UnitValue::Unit::PERCENT},
                                Css::NegateNode{makeBox<Calc>(length.calcTree())},
                            },
                        },
                    },
                    ctx
                );
            }

            return computeValue(length, ctx);
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

    static Css::BackgroundPosition specify(Position const& value) {
        return {
            specifyValue<Css::LengthPercentage>(value.x),
            specifyValue<Css::LengthPercentage>(value.y),
        };
    }
};

// https://drafts.csswg.org/css-backgrounds/#typedef-bg-layer
export struct BackgroundLayer {
    Opt<Css::Image> image;
    Css::BackgroundAttachment attachment = Keywords::FIXED;
    Position position = {Percent{0}, Percent{0}};
    Css::BackgroundRepeat repeat;
};

export template <>
struct ValueComputer<Css::BackgroundLayer> {
    static Css::BackgroundLayer specify(BackgroundLayer const& value) {
        return {value.image, value.attachment, specifyValue<Css::BackgroundPosition>(value.position), value.repeat};
    }
};

// https://drafts.csswg.org/css-backgrounds/#border-shorthands
export struct Border {
    Px width = 3_px;
    Gfx::BorderStyle style = Gfx::BorderStyle::NONE;
    Color color = Gfx::BLACK;
};

export template <>
struct ValueComputer<Css::Border> {
    static Css::Border specify(Border const& value) {
        return {specifyValue<Css::LineWidth>(value.width), value.style, specifyValue<Css::Color>(value.color)};
    }
};

} // namespace Vaev::Style
