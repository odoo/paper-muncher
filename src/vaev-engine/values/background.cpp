module;

#include <karm/macros>

export module Vaev.Engine:values.background;

import Karm.Core;

import :css;
import :values.base;
import :values.calc2;
import :values.color;
import :values.image;
import :values.keywords;
import :values.length;
import :values.percent;
import :values.unitValue;

using namespace Karm;

namespace Vaev::Css {

// https://drafts.csswg.org/css-backgrounds/#background-attachment
using BackgroundAttachment = Union<
    Keywords::Scroll,
    Keywords::Fixed,
    Keywords::Local>;

// MARK: Background Position ---------------------------------------------------
// https://www.w3.org/TR/css-backgrounds-3/#typedef-bg-position
// [
//   case 1 :
//   [ left | center | right | top | bottom | <length-percentage> ]
// |
//   case 2 :
//   [ left | center | right | <length-percentage> ]
//   [ top | center | bottom | <length-percentage> ]
// |
//   case 3 :
//   [ center | [ left | right ] <length-percentage>? ] &&
//   [ center | [ top | bottom ] <length-percentage>? ]
// ]

export struct BackgroundPosition {
    using HorizontalAnchor = Union<Keywords::Left, Keywords::Center, Keywords::Right>;
    using VerticalAnchor = Union<Keywords::Top, Keywords::Center, Keywords::Bottom>;

    HorizontalAnchor horizontalAnchor;
    LengthPercentage horizontal;
    VerticalAnchor verticalAnchor;
    LengthPercentage vertical;

    BackgroundPosition()
        : horizontalAnchor(Keywords::LEFT),
          horizontal(LengthPercentage::fromPercentage(0)),
          verticalAnchor(Keywords::TOP),
          vertical(LengthPercentage::fromPercentage(0)) {
    }

    BackgroundPosition(LengthPercentage horizontal, LengthPercentage vertical)
        : horizontalAnchor(Keywords::LEFT), horizontal(horizontal), verticalAnchor(Keywords::TOP), vertical(vertical) {
    }

    BackgroundPosition(HorizontalAnchor horizontalAnchor, LengthPercentage horizontal, VerticalAnchor verticalAnchor, LengthPercentage vertical)
        : horizontalAnchor(horizontalAnchor), horizontal(horizontal), verticalAnchor(verticalAnchor), vertical(vertical) {
    }

    static BackgroundPosition center() {
        return {Keywords::CENTER, LengthPercentage::fromPercentage(0), Keywords::CENTER, LengthPercentage::fromPercentage(0)};
    }

    void repr(Io::Emit& e) const {
        e("({} {}", horizontalAnchor, horizontal);
        e(" ");
        e("{} {})", verticalAnchor, vertical);
    }
};

} // namespace Vaev::Css

namespace Vaev::Style {

// https://drafts.csswg.org/css-values/#position
// The computed value is a pair of offsets from the top left origin.

export struct Position {
    PixelPercentage x;
    PixelPercentage y;

    static Style::Position center() {
        return {Percent{50}, Percent{50}};
    }
};

export Style::Position resolve(Css::BackgroundPosition const& value, Css::LengthContext auto const& ctx) {
    auto offset = [&](bool isCenter, bool isFromEnd, Css::LengthPercentage const& length) -> PixelPercentage {
        if (isCenter)
            return Percent{50};

        if (isFromEnd) {
            // calc(100% - <length-percentage>)
            return resolve(
                Css::LengthPercentage{
                    UNCHECKED,
                    Css::SumNode{
                        .children = {
                            Css::UnitValue{100, Css::UnitValue::Unit::PERCENT},
                            Css::NegateNode{makeBox<Css::Calc>(length.calcTree())},
                        },
                    },
                },
                ctx
            );
        }

        return resolve(length, ctx);
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

export Css::BackgroundPosition toSpecified(Style::Position const& value) {
    return {toSpecified(value.x), toSpecified(value.y)};
}

} // namespace Vaev::Style

namespace Vaev::Css {

export template <>
struct ValueParser<BackgroundPosition> {
    static Res<BackgroundPosition> parse(Cursor<Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        using Item = Union<
            Keywords::Left,
            Keywords::Center,
            Keywords::Right,
            Keywords::Top,
            Keywords::Center,
            Keywords::Bottom,
            LengthPercentage>;

        InlineVec<Item, 4> items;
        for (int i = 0; i < 4 && not c.ended(); ++i) {
            eatWhitespace(c);
            auto item = parseValue<Item>(c);
            eatWhitespace(c);
            if (item)
                items.pushBack(item.expect());
            else
                break;
        }

        if (not items)
            return Error::invalidData("invalid background position");

        if (items.len() == 1) { // case 1
            if (items[0].is<Keywords::Bottom>() or items[0].is<Keywords::Top>())
                items.pushFront(Keywords::CENTER);
            else
                items.pushBack(Keywords::CENTER);
        }

        if (items.len() == 2) { // case 2
            if (items[0].is<Keywords::Center>() and (items[1].is<Keywords::Left>() or items[1].is<Keywords::Right>()))
                items.pushBack(items.popFront());

            BackgroundPosition::HorizontalAnchor hAnchor = Keywords::LEFT;
            LengthPercentage hValue = LengthPercentage::fromPercentage(0);
            BackgroundPosition::VerticalAnchor vAnchor = Keywords::TOP;
            LengthPercentage vValue = LengthPercentage::fromPercentage(0);

            try$(
                items[0].visit(
                    [&](Meta::Contains<Keywords::Left, Keywords::Right, Keywords::Center> auto& t) -> Res<> {
                        hAnchor = t;
                        return Ok();
                    },
                    [&](LengthPercentage& t) -> Res<> {
                        hValue = t;
                        return Ok();
                    },
                    [](auto&) -> Res<> {
                        return Error::invalidData("invalid horizontal anchor");
                    }
                )
            );

            try$(
                items[1].visit(
                    [&](Meta::Contains<Keywords::Bottom, Keywords::Top, Keywords::Center> auto& t) -> Res<> {
                        vAnchor = t;
                        return Ok();
                    },
                    [&](LengthPercentage& t) -> Res<> {
                        vValue = t;
                        return Ok();
                    },
                    [](auto&) -> Res<> {
                        return Error::invalidData("invalid vertical anchor");
                    }
                )
            );

            return Ok(BackgroundPosition(hAnchor, hValue, vAnchor, vValue));
        }

        // case 3
        BackgroundPosition::HorizontalAnchor hAnchor = Keywords::LEFT;
        LengthPercentage hValue = LengthPercentage::fromPercentage(0);
        bool hSet = false;

        BackgroundPosition::VerticalAnchor vAnchor = Keywords::TOP;
        LengthPercentage vValue = LengthPercentage::fromPercentage(0);

        usize secondPairIndex = 2;

        try$(
            items[0].visit(
                [&](Meta::Contains<Keywords::Left, Keywords::Right> auto& t) -> Res<> {
                    hAnchor = t;
                    hSet = true;

                    if (auto mesure = items[1].is<LengthPercentage>()) {
                        hValue = *mesure;
                    } else {
                        secondPairIndex = 1;
                    }

                    return Ok();
                },
                [&](Keywords::Center& t) -> Res<> {
                    hAnchor = t;
                    hSet = true;

                    secondPairIndex = 1;

                    return Ok();
                },
                [&](Meta::Contains<Keywords::Top, Keywords::Bottom> auto& t) -> Res<> {
                    vAnchor = t;

                    if (auto mesure = items[1].is<LengthPercentage>()) {
                        vValue = *mesure;
                    } else {
                        secondPairIndex = 1;
                    }

                    return Ok();
                },
                [](auto&) -> Res<> {
                    return Error::invalidData("invalid anchor");
                }
            )
        );

        try$(
            items[secondPairIndex].visit(
                [&](Meta::Contains<Keywords::Left, Keywords::Right> auto& t) -> Res<> {
                    if (hSet) {
                        if (hAnchor.is<Keywords::Center>()) { // the first center was aimed at the vertical part so we exchange
                            vAnchor = Keywords::CENTER;
                            vValue = hValue;
                        } else {
                            return Error::invalidData("expected vertical anchor");
                        }
                    }

                    hAnchor = t;

                    if (secondPairIndex + 1 < items.len()) {
                        if (auto mesure = items[secondPairIndex + 1].is<LengthPercentage>())
                            hValue = *mesure;
                        else {
                            return Error::invalidData("expected percent or length");
                        }
                    }

                    return Ok();
                },
                [&](Meta::Contains<Keywords::Top, Keywords::Bottom, Keywords::Center> auto& t) -> Res<> {
                    vAnchor = t;

                    if (secondPairIndex + 1 < items.len()) {
                        if (auto mesure = items[secondPairIndex + 1].is<LengthPercentage>())
                            vValue = *mesure;
                        else {
                            return Error::invalidData("unexpected last value");
                        }
                    }

                    return Ok();
                },
                [](auto&) -> Res<> {
                    return Error::invalidData("invalid anchor");
                }
            )
        );

        return Ok(BackgroundPosition(hAnchor, hValue, vAnchor, vValue));
    }
};

export struct BackgroundRepeat {
    enum _Val : u8 {
        NO = 0,
        X = 1 << 0,
        Y = 1 << 1,
        REPEAT = X | Y,
    };

    u8 _val;

    constexpr BackgroundRepeat(_Val val = REPEAT)
        : _val(static_cast<u8>(val)) {
    }

    void repr(Io::Emit& e) const {
        if (_val == NO) {
            e("no-repeat");
        } else if (_val == REPEAT) {
            e("repeat");
        } else {
            if (_val & X)
                e("repeat-x");
            if (_val & Y)
                e("repeat-y");
        }
    }
};

export struct BackgroundLayer {
    Opt<Image> image;
    BackgroundAttachment attachment = Keywords::FIXED;
    BackgroundPosition position;
    BackgroundRepeat repeat;

    void repr(Io::Emit& e) const {
        e("(background");
        e(" image={}", image);
        e(" attachment={}", attachment);
        e(" position={}", position);
        e(" repeat={}", repeat);
        e(")");
    }
};


} // namespace Vaev::Css

namespace Vaev::Style {

export struct BackgroundLayer {
    Opt<Css::Image> image;
    Css::BackgroundAttachment attachment = Keywords::FIXED;
    Style::Position position = {Percent{0}, Percent{0}};
    Css::BackgroundRepeat repeat;
};

export Css::BackgroundLayer toSpecified(Style::BackgroundLayer const& value) {
    return {value.image, value.attachment, toSpecified(value.position), value.repeat};
}

export struct Backgrounds {
    Style::Color color = Css::TRANSPARENT;
    Vec<Style::BackgroundLayer> layers = {};

    bool imageIsNone() const {
        return iter(layers) |
               All([](auto const& l) {
                   return l.image == NONE;
               });
    }
};

} // namespace Vaev::Style

