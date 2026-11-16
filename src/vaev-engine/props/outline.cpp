module;

#include <karm/macros>

export module Vaev.Engine:props.outline;

import Karm.Core;
import :props.base;
import :css.parser;
import :style.computed;

using namespace Karm;

namespace Vaev::Style {

// MARK: Outline --------------------------------------------------------------

// https://drafts.csswg.org/css-ui/#outline-width
export struct OutlineWidthProperty : Property {
    static constexpr auto ID = LonghandId::OUTLINE_WIDTH;

    static Rc<Property> initial() {
        return makeRc<OutlineWidthProperty>(LineWidth{Keywords::MEDIUM});
    }

    static Rc<Property> load(ComputedValues const& c) {
        return makeRc<OutlineWidthProperty>(c.outline->width);
    }

    static Res<Rc<Property>> parse(Cursor<Css::Sst>& c, ParsingMode) {
        return Ok(makeRc<OutlineWidthProperty>(try$(parseValue<LineWidth>(c))));
    }

    LineWidth _value;

    OutlineWidthProperty(LineWidth value)
        : Property(ID), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.outline.cow().width = _value;
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://drafts.csswg.org/css-ui/#outline-style
export struct OutlineStyleProperty : Property {
    static constexpr auto ID = LonghandId::OUTLINE_STYLE;

    using Value = Union<Keywords::Auto, Gfx::BorderStyle>;

    static Rc<Property> initial() {
        return makeRc<OutlineStyleProperty>(Value{Gfx::BorderStyle::NONE});
    }

    static Rc<Property> load(ComputedValues const& c) {
        return makeRc<OutlineStyleProperty>(c.outline->style);
    }

    static Res<Rc<Property>> parse(Cursor<Css::Sst>& c, ParsingMode) {
        return Ok(makeRc<OutlineStyleProperty>(try$(parseValue<Value>(c))));
    }

    Value _value;

    OutlineStyleProperty(Value value)
        : Property(ID), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.outline.cow().style = _value;
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://drafts.csswg.org/css-ui/#outline-color
export struct OutlineColorProperty : Property {
    static constexpr auto ID = LonghandId::OUTLINE_COLOR;
    // Must be applied after 'outline-style'.
    static constexpr auto PHASE = ComputationPhase::LATE;

    using Value = Union<Keywords::Auto, Color>;

    static Rc<Property> initial() {
        return makeRc<OutlineColorProperty>(Value{Keywords::AUTO});
    }

    static Rc<Property> load(ComputedValues const& c) {
        return makeRc<OutlineColorProperty>(Color{c.outline->color});
    }

    static Res<Rc<Property>> parse(Cursor<Css::Sst>& c, ParsingMode) {
        return Ok(makeRc<OutlineColorProperty>(try$(parseValue<Value>(c))));
    }

    Value _value;

    OutlineColorProperty(Value value)
        : Property(ID), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.outline.cow().color = _value.visit(
            // https://drafts.csswg.org/css-ui/#valdef-outline-color-auto
            [&](Keywords::Auto) {
                // When outline-style is auto, outline-color: auto computes to auto and represents the accent color.
                // Otherwise, outline-color: auto computes to currentColor.
                return c.outline->style == Keywords::AUTO
                           ? resolve(Color{SystemColor::ACCENT_COLOR}, c.color)
                           : c.color;
            },
            [&](Color color) {
                return resolve(color, c.color);
            }
        );
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://drafts.csswg.org/css-ui/#outline-offset
export struct OutlineOffsetProperty : Property {
    static constexpr auto ID = LonghandId::OUTLINE_OFFSET;

    static Rc<Property> initial() {
        return makeRc<OutlineOffsetProperty>(Calc<Length>{0_au});
    }

    static Rc<Property> load(ComputedValues const& c) {
        return makeRc<OutlineOffsetProperty>(c.outline->offset);
    }

    static Res<Rc<Property>> parse(Cursor<Css::Sst>& c, ParsingMode) {
        return Ok(makeRc<OutlineOffsetProperty>(try$(parseValue<Calc<Length>>(c))));
    }

    Calc<Length> _value;

    OutlineOffsetProperty(Calc<Length> value)
        : Property(ID), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.outline.cow().offset = _value;
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://drafts.csswg.org/css-ui/#outline
export struct OutlineShorthand {
    static constexpr auto ID = ShorthandId::OUTLINE;

    static constexpr Array LONGHANDS = {
        LonghandId::OUTLINE_WIDTH,
        LonghandId::OUTLINE_STYLE,
        LonghandId::OUTLINE_COLOR,
    };

    // <'outline-width'> || <'outline-style'> || <'outline-color'>
    static Res<> parse(Cursor<Css::Sst>& c, Vec<Rc<Property>>& stylesheet, ParsingMode) {
        Opt<LineWidth> width;
        Opt<OutlineStyleProperty::Value> style;
        Opt<OutlineColorProperty::Value> color;

        // NOTE: auto is valid for both outline-style and outline-color,
        //       so it is only assigned once the other components are known:
        //       it goes to whichever of them is not set yet.
        usize autos = 0;
        while (true) {
            eatWhitespace(c);
            if (c.ended())
                break;

            if (c.skip(Css::Token::ident("auto"))) {
                autos++;
                continue;
            }

            if (not width) {
                if (auto maybeWidth = parseValue<LineWidth>(c)) {
                    width = Some(maybeWidth.take());
                    continue;
                }
            }

            if (not style) {
                if (auto maybeStyle = parseValue<OutlineStyleProperty::Value>(c)) {
                    style = Some(maybeStyle.take());
                    continue;
                }
            }

            if (not color) {
                if (auto maybeColor = parseValue<OutlineColorProperty::Value>(c)) {
                    color = Some(maybeColor.take());
                    continue;
                }
            }

            break;
        }

        if (autos and not style) {
            style = Some(OutlineStyleProperty::Value{Keywords::AUTO});
            autos--;
        }

        if (autos and not color) {
            color = Some(OutlineColorProperty::Value{Keywords::AUTO});
            autos--;
        }

        if (autos)
            return Error::invalidData("too many auto in outline");

        if (not width and not style and not color)
            return Error::invalidData("expected outline-width, outline-style or outline-color");

        // NOTE: Omitted values are reset to their initial value.
        if (width)
            stylesheet.pushBack(makeRc<OutlineWidthProperty>(*width));
        else
            stylesheet.pushBack(initialProperty(LonghandId::OUTLINE_WIDTH));

        if (style)
            stylesheet.pushBack(makeRc<OutlineStyleProperty>(*style));
        else
            stylesheet.pushBack(initialProperty(LonghandId::OUTLINE_STYLE));

        if (color)
            stylesheet.pushBack(makeRc<OutlineColorProperty>(*color));
        else
            stylesheet.pushBack(initialProperty(LonghandId::OUTLINE_COLOR));

        return Ok();
    }
};

} // namespace Vaev::Style
