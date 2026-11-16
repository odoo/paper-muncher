module;

#include <karm/macros>

export module Vaev.Engine:props.fonts;

import Karm.Core;
import :props.base;
import :props.baseline;
import :css.parser;
import :style.computed;

using namespace Karm;
using namespace Karm::Literals;

namespace Vaev::Style {

// MARK: Fonts -----------------------------------------------------------------

// https://www.w3.org/TR/css-fonts-4/#font-family-prop
export struct FontFamilyProperty : Property {
    static constexpr auto ID = LonghandId::FONT_FAMILY;
    static constexpr Flags<Options> FLAGS = {INHERITED, BULK_INHERITED};
    static constexpr auto PHASE = ComputationPhase::FONT;

    static Rc<Property> initial() {
        return makeRc<FontFamilyProperty>(Vec<FontFamily>{"sans-serif"_sym});
    }

    static Rc<Property> load(ComputedValues const& c) {
        return makeRc<FontFamilyProperty>(c.font->families);
    }

    static Res<Rc<Property>> parse(Cursor<Css::Sst>& c, ParsingMode) {
        Vec<FontFamily> value{};
        eatWhitespace(c);
        if (c.ended())
            return Error::invalidData("expected font family");

        while (not c.ended()) {
            value.pushBack(try$(parseValue<FontFamily>(c)));

            eatWhitespace(c);
            c.skip(Css::Token::COMMA);
            eatWhitespace(c);
        }
        return Ok(makeRc<FontFamilyProperty>(std::move(value)));
    }

    static void inherit(ComputedValues const& parent, ComputedValues& child) {
        child.font.cow().families = parent.font->families;
    }

    Vec<FontFamily> _value;

    FontFamilyProperty(Vec<FontFamily> value)
        : Property(ID), _value(std::move(value)) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.font.cow().families = _value;
    }

    Res<> serialize(Io::TextWriter& w) const override {
        bool first = true;

        for (auto const& family : _value) {
            if (first) {
                first = false;
            } else {
                try$(w.writeStr(", "s));
            }

            try$(w.writeStr(family.name.str()));
        }

        return Ok();
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/css-fonts-4/#font-weight-prop
export struct FontWeightProperty : Property {
    static constexpr auto ID = LonghandId::FONT_WEIGHT;
    static constexpr Flags<Options> FLAGS = {INHERITED, BULK_INHERITED};
    static constexpr auto PHASE = ComputationPhase::FONT;

    static Rc<Property> initial() {
        return makeRc<FontWeightProperty>(FontWeight{Gfx::FontWeight::REGULAR});
    }

    static Rc<Property> load(ComputedValues const& c) {
        return makeRc<FontWeightProperty>(c.font->weight);
    }

    static Res<Rc<Property>> parse(Cursor<Css::Sst>& c, ParsingMode) {
        return Ok(makeRc<FontWeightProperty>(try$(parseValue<FontWeight>(c))));
    }

    static void inherit(ComputedValues const& parent, ComputedValues& child) {
        child.font.cow().weight = parent.font->weight;
    }

    FontWeight _value;

    FontWeightProperty(FontWeight value)
        : Property(ID), _value(value) {}

    void apply(ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.font.cow().weight = _value.resolve(parent.font->weight);
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/css-fonts-4/#font-width-prop
export struct FontWidthProperty : Property {
    static constexpr auto ID = LonghandId::FONT_WIDTH;
    static constexpr Flags<Options> FLAGS = {INHERITED, BULK_INHERITED};
    // https://drafts.csswg.org/css-fonts/#font-stretch-prop
    static constexpr Array LEGACY_NAME_ALIASES = {"font-stretch"s};
    static constexpr auto PHASE = ComputationPhase::FONT;

    static Rc<Property> initial() {
        return makeRc<FontWidthProperty>(FontWidth::NORMAL);
    }

    static Rc<Property> load(ComputedValues const& c) {
        return makeRc<FontWidthProperty>(c.font->width);
    }

    static Res<Rc<Property>> parse(Cursor<Css::Sst>& c, ParsingMode) {
        return Ok(makeRc<FontWidthProperty>(try$(parseValue<FontWidth>(c))));
    }

    static void inherit(ComputedValues const& parent, ComputedValues& child) {
        child.font.cow().width = parent.font->width;
    }

    FontWidth _value;

    FontWidthProperty(FontWidth value)
        : Property(ID), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.font.cow().width = _value;
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/css-fonts-4/#font-style-prop
export struct FontStyleProperty : Property {
    static constexpr auto ID = LonghandId::FONT_STYLE;
    static constexpr Flags<Options> FLAGS = {INHERITED, BULK_INHERITED};
    static constexpr auto PHASE = ComputationPhase::FONT;

    static Rc<Property> initial() {
        return makeRc<FontStyleProperty>(FontStyle::NORMAL);
    }

    static Rc<Property> load(ComputedValues const& c) {
        return makeRc<FontStyleProperty>(c.font->style);
    }

    static Res<Rc<Property>> parse(Cursor<Css::Sst>& c, ParsingMode) {
        return Ok(makeRc<FontStyleProperty>(try$(parseValue<FontStyle>(c))));
    }

    static void inherit(ComputedValues const& parent, ComputedValues& child) {
        child.font.cow().style = parent.font->style;
    }

    FontStyle _value;

    FontStyleProperty(FontStyle value)
        : Property(ID), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.font.cow().style = _value;
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/css-fonts-4/#font-size-prop
export struct FontSizeProperty : Property {
    static constexpr auto ID = LonghandId::FONT_SIZE;
    static constexpr Flags<Options> FLAGS = {INHERITED, BULK_INHERITED};
    static constexpr auto PHASE = ComputationPhase::FONT;

    static Rc<Property> initial() {
        return makeRc<FontSizeProperty>(Keywords::MEDIUM);
    }

    static Rc<Property> load(ComputedValues const& c) {
        return makeRc<FontSizeProperty>(Length{c.font->size});
    }

    static Res<Rc<Property>> parse(Cursor<Css::Sst>& c, ParsingMode) {
        return Ok(makeRc<FontSizeProperty>(try$(parseValue<FontSize>(c))));
    }

    static void inherit(ComputedValues const& parent, ComputedValues& child) {
        child.font.cow().size = parent.font->size;
    }

    FontSize _value;

    FontSizeProperty(FontSize value)
        : Property(ID), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.font.cow().size = resolve(_value, cx);
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/css-fonts-4/#font-prop
export struct FontShorthand {
    static constexpr auto ID = ShorthandId::FONT;

    // FIXME: The font shorthand also resets font-variant, font-size-adjust,
    //        font-kerning, font-feature-settings, font-optical-sizing,
    //        font-variation-settings and font-language-override,
    //        which are not implemented yet.
    static constexpr auto LONGHANDS = Array{
        LonghandId::FONT_STYLE,
        LonghandId::FONT_WEIGHT,
        LonghandId::FONT_WIDTH,
        LonghandId::FONT_SIZE,
        LonghandId::LINE_HEIGHT,
        LonghandId::FONT_FAMILY,
    };

    // [ [ <'font-style'> || <font-variant-css2> || <'font-weight'> || <font-width-css3> ]?
    //   <'font-size'> [ / <'line-height'> ]? <'font-family'># ] | <system-family-name>
    static Res<> parse(Cursor<Css::Sst>& c, Vec<Rc<Property>>& stylesheet, ParsingMode mode) {
        // TODO: <system-family-name>
        Opt<FontStyle> style;
        Opt<FontWeight> weight;
        Opt<FontWidth> width;
        bool variant = false;

        while (true) {
            eatWhitespace(c);
            if (c.ended())
                return Error::invalidData("expected font-size");

            if (not style) {
                if (auto maybeStyle = parseValue<FontStyle>(c)) {
                    style = Some(maybeStyle.take());
                    continue;
                }
            }

            if (not weight) {
                if (auto maybeWeight = parseValue<FontWeight>(c)) {
                    weight = Some(maybeWeight.take());
                    continue;
                }
            }

            // <font-width-css3> only accepts keywords, a percentage here is the font-size.
            if (not width and c.peek() == Css::Token::IDENT) {
                if (auto maybeWidth = parseValue<FontWidth>(c)) {
                    width = Some(maybeWidth.take());
                    continue;
                }
            }

            // <font-variant-css2> = normal | small-caps
            // NOTE: Tried last, so that normal is first given to the other properties.
            // FIXME: font-variant is not implemented, small-caps is ignored.
            if (not variant and (c.skip(Css::Token::ident("normal")) or c.skip(Css::Token::ident("small-caps")))) {
                variant = true;
                continue;
            }

            break;
        }

        auto size = try$(parseValue<FontSize>(c));

        Opt<Rc<Property>> lineHeight;
        eatWhitespace(c);
        if (c.skip(Css::Token::delim("/"))) {
            eatWhitespace(c);
            lineHeight = Some(try$(LineHeightProperty::parse(c, mode)));
        }

        auto family = try$(FontFamilyProperty::parse(c, mode));

        // NOTE: Omitted values are reset to their initial value.
        if (style)
            stylesheet.pushBack(makeRc<FontStyleProperty>(*style));
        else
            stylesheet.pushBack(initialProperty(LonghandId::FONT_STYLE));

        if (weight)
            stylesheet.pushBack(makeRc<FontWeightProperty>(*weight));
        else
            stylesheet.pushBack(initialProperty(LonghandId::FONT_WEIGHT));

        if (width)
            stylesheet.pushBack(makeRc<FontWidthProperty>(*width));
        else
            stylesheet.pushBack(initialProperty(LonghandId::FONT_WIDTH));

        stylesheet.pushBack(makeRc<FontSizeProperty>(std::move(size)));

        if (lineHeight)
            stylesheet.pushBack(*lineHeight);
        else
            stylesheet.pushBack(initialProperty(LonghandId::LINE_HEIGHT));

        stylesheet.pushBack(family);

        return Ok();
    }
};

} // namespace Vaev::Style
