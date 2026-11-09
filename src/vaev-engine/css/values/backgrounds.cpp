module;

#include <karm/macros>

export module Vaev.Engine:css.values.backgrounds;

import Karm.Gfx;
import Karm.Math;
import Karm.Core;

import :css.lexer;
import :css.parser;
import :css.values.base;
import :css.values.color;
import :css.values.numeric;
import :css.values.images;
import :css.values.keywords;
import :css.values.primitives;

using namespace Karm;

namespace Vaev::Css {

// https://drafts.csswg.org/css-backgrounds/#typedef-line-width
export using LineWidth = Union<
    Keywords::Thin,
    Keywords::Medium,
    Keywords::Thick,
    Length>;

// MARK: Border Style ----------------------------------------------------------
// https://www.w3.org/TR/CSS22/box.html#border-style-properties

export template <>
struct ValueParser<Gfx::BorderStyle> {
    static Res<Gfx::BorderStyle> parse(Cursor<Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of property");

        if (c.skip(Token::ident("none"))) {
            return Ok(Gfx::BorderStyle::NONE);
        } else if (c.skip(Token::ident("solid"))) {
            return Ok(Gfx::BorderStyle::SOLID);
        } else if (c.skip(Token::ident("dashed"))) {
            return Ok(Gfx::BorderStyle::DASHED);
        } else if (c.skip(Token::ident("dotted"))) {
            return Ok(Gfx::BorderStyle::DOTTED);
        } else if (c.skip(Token::ident("hidden"))) {
            return Ok(Gfx::BorderStyle::HIDDEN);
        } else if (c.skip(Token::ident("double"))) {
            return Ok(Gfx::BorderStyle::DOUBLE);
        } else if (c.skip(Token::ident("groove"))) {
            return Ok(Gfx::BorderStyle::GROOVE);
        } else if (c.skip(Token::ident("ridge"))) {
            return Ok(Gfx::BorderStyle::RIDGE);
        } else if (c.skip(Token::ident("outset"))) {
            return Ok(Gfx::BorderStyle::OUTSET);
        } else {
            return Error::invalidData("unknown border-style");
        }
    }
};

// MARK: Border ----------------------------------------------------------------

// Value of the border shorthands.
// https://www.w3.org/TR/css-backgrounds-3/#border-shorthands
export struct Border {
    LineWidth width = Keywords::MEDIUM;
    Gfx::BorderStyle style = Gfx::BorderStyle::NONE;
    Color color = Keywords::CURRENT_COLOR;

    void repr(Io::Emit& e) const {
        e("(border {} {} {})", width, style, color);
    }
};

export template <>
struct ValueParser<Border> {
    static Res<Border> parse(Cursor<Sst>& c) {
        // https://drafts.csswg.org/css-backgrounds/#border-shorthands
        // <line-width> || <line-style> || <color>, each at most once.
        Border border;
        bool widthSet = false;
        bool styleSet = false;
        bool colorSet = false;
        while (not c.ended()) {
            eatWhitespace(c);

            auto width = parseValue<LineWidth>(c);
            if (width) {
                if (widthSet)
                    return Error::invalidData("duplicated border width");
                border.width = width.expect();
                widthSet = true;
                continue;
            }

            auto color = parseValue<Color>(c);
            if (color) {
                if (colorSet)
                    return Error::invalidData("duplicated border color");
                border.color = color.expect();
                colorSet = true;
                continue;
            }

            auto style = parseValue<Gfx::BorderStyle>(c);
            if (style) {
                if (styleSet)
                    return Error::invalidData("duplicated border style");
                border.style = style.expect();
                styleSet = true;
                continue;
            }

            break;
        }

        return Ok(border);
    }
};

export template <typename T>
struct ValueParser<Math::Radii<T>> {
    static Res<Math::Radii<T>> parse(Cursor<Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        eatWhitespace(c);
        auto value1 = parseValue<T>(c);
        if (not value1)
            return parsePostSlash(c, NONE);

        eatWhitespace(c);
        auto value2 = parseValue<T>(c);
        if (not value2)
            return parsePostSlash(c, Some(Math::Radii<T>{value1.take()}));

        eatWhitespace(c);
        auto value3 = parseValue<T>(c);
        if (not value3)
            return parsePostSlash(c, Some(Math::Radii<T>{value1.take(), value2.take()}));

        eatWhitespace(c);
        auto value4 = parseValue<T>(c);
        if (not value4) {
            auto topEndAndBottomStart = value2.take();
            return parsePostSlash(c, Some(Math::Radii<T>{value1.take(), topEndAndBottomStart, value3.take(), topEndAndBottomStart}));
        }

        return parsePostSlash(c, Some(Math::Radii<T>{value1.take(), value2.take(), value3.take(), value4.take()}));
    }

    static Res<Math::Radii<T>> parsePostSlash(Cursor<Sst>& c, Opt<Math::Radii<T>> maybeRadii) {
        // if parse a /
        // 1 value-- > border all(a, d, e, h)
        // 2 values-- > 1 = top - start + bottom - end 2 = the others
        // 3 values-- > 1 = top - start, 2 = top - end + bottom - start, 3 = bottom - end
        // 4 values-- > 1 = top - start, 2 = top - end 3 = bottom - end, 4 = bottom - start
        eatWhitespace(c);

        if (c.ended() or c.peek().token.data != "/"s)
            return maybeRadii.okOr(Error::invalidData("expected border radius"));

        auto radii = maybeRadii.unwrapOr(Math::Radii<T>{T{px(0)}});
        c.next();
        eatWhitespace(c);
        auto value1 = parseValue<T>(c);
        if (not value1) {
            return Ok(std::move(radii));
        }

        eatWhitespace(c);
        auto value2 = parseValue<T>(c);
        if (not value2) {
            radii.a = value1.expect();
            radii.d = value1.expect();
            radii.e = value1.expect();
            radii.h = value1.expect();
            return Ok(std::move(radii));
        }

        eatWhitespace(c);
        auto value3 = parseValue<T>(c);
        if (not value3) {
            radii.a = value1.expect();
            radii.d = value2.expect();
            radii.e = value1.expect();
            radii.h = value2.expect();
            return Ok(std::move(radii));
        }

        eatWhitespace(c);
        auto value4 = parseValue<T>(c);
        if (not value4) {
            radii.a = value1.expect();
            radii.d = value2.expect();
            radii.e = value3.expect();
            radii.h = value2.expect();

            return Ok(std::move(radii));
        }

        radii.a = value1.expect();
        radii.d = value2.expect();
        radii.e = value3.expect();
        radii.h = value4.expect();

        return Ok(std::move(radii));
    }
};

} // namespace Vaev::Css

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
          horizontal(percent(0)),
          verticalAnchor(Keywords::TOP),
          vertical(percent(0)) {
    }

    BackgroundPosition(LengthPercentage horizontal, LengthPercentage vertical)
        : horizontalAnchor(Keywords::LEFT), horizontal(horizontal), verticalAnchor(Keywords::TOP), vertical(vertical) {
    }

    BackgroundPosition(HorizontalAnchor horizontalAnchor, LengthPercentage horizontal, VerticalAnchor verticalAnchor, LengthPercentage vertical)
        : horizontalAnchor(horizontalAnchor), horizontal(horizontal), verticalAnchor(verticalAnchor), vertical(vertical) {
    }

    static BackgroundPosition center() {
        return {Keywords::CENTER, percent(0), Keywords::CENTER, percent(0)};
    }

    void repr(Io::Emit& e) const {
        e("({} {}", horizontalAnchor, horizontal);
        e(" ");
        e("{} {})", verticalAnchor, vertical);
    }
};

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
            LengthPercentage hValue = percent(0);
            BackgroundPosition::VerticalAnchor vAnchor = Keywords::TOP;
            LengthPercentage vValue = percent(0);

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
        LengthPercentage hValue = percent(0);
        bool hSet = false;

        BackgroundPosition::VerticalAnchor vAnchor = Keywords::TOP;
        LengthPercentage vValue = percent(0);

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
