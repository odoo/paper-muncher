module;

#include <karm/macros>

export module Vaev.Engine:props.list;

import :props.base;

namespace Vaev::Style {

// https://www.w3.org/TR/css-lists-3/#propdef-list-style-image
struct ListStyleImageProperty : Property {
    static constexpr auto ID = LonghandId::LIST_STYLE_IMAGE;
    static constexpr Flags<Options> FLAGS = {INHERITED, BULK_INHERITED};

    static Rc<Property> initial() {
        return makeRc<ListStyleImageProperty>(Keywords::NONE);
    }

    static Rc<Property> load(ComputedValues const& c) {
        return makeRc<ListStyleImageProperty>(c.list->image);
    }

    static Res<Rc<Property>> parse(Cursor<Css::Sst>& c, ParsingMode) {
        auto value = try$(parseValue<ListImage>(c));
        return Ok(makeRc<ListStyleImageProperty>(value));
    }

    static void inherit(ComputedValues const& parent, ComputedValues& child) {
        child.list.cow().image = parent.list->image;
    }

    ListImage _value;

    ListStyleImageProperty(ListImage value)
        : Property(ID), _value(value) {}

    void apply(ComputedValues const&, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.list.cow().image = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/css-lists-3/#propdef-list-style-type
struct ListStyleTypeProperty : Property {
    static constexpr auto ID = LonghandId::LIST_STYLE_TYPE;
    static constexpr Flags<Options> FLAGS = {INHERITED, BULK_INHERITED};

    static Rc<Property> initial() {
        return makeRc<ListStyleTypeProperty>(CustomIdent{"disc"_sym});
    }

    static Rc<Property> load(ComputedValues const& c) {
        return makeRc<ListStyleTypeProperty>(c.list->type);
    }

    static Res<Rc<Property>> parse(Cursor<Css::Sst>& c, ParsingMode) {
        auto value = try$(parseValue<ListType>(c));
        return Ok(makeRc<ListStyleTypeProperty>(value));
    }

    static void inherit(ComputedValues const& parent, ComputedValues& child) {
        child.list.cow().type = parent.list->type;
    }

    ListType _value;

    ListStyleTypeProperty(ListType value)
        : Property(ID), _value(value) {}

    void apply(ComputedValues const&, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.list.cow().type = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/css-lists-3/#list-style-position-property
struct ListStylePositionProperty : Property {
    static constexpr auto ID = LonghandId::LIST_STYLE_POSITION;
    static constexpr Flags<Options> FLAGS = {INHERITED, BULK_INHERITED};

    static Rc<Property> initial() {
        return makeRc<ListStylePositionProperty>(Keywords::OUTSIDE);
    }

    static Rc<Property> load(ComputedValues const& c) {
        return makeRc<ListStylePositionProperty>(c.list->position);
    }

    static Res<Rc<Property>> parse(Cursor<Css::Sst>& c, ParsingMode) {
        auto value = try$(parseValue<ListPosition>(c));
        return Ok(makeRc<ListStylePositionProperty>(value));
    }

    static void inherit(ComputedValues const& parent, ComputedValues& child) {
        child.list.cow().position = parent.list->position;
    }

    ListPosition _value;

    ListStylePositionProperty(ListPosition value)
        : Property(ID), _value(value) {}

    void apply(ComputedValues const&, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.list.cow().position = _value;
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/css-lists-3/#propdef-list-style
export struct ListStyleShorthand {
    static constexpr auto ID = ShorthandId::LIST_STYLE;

    static constexpr Array LONGHANDS = {
        LonghandId::LIST_STYLE_POSITION,
        LonghandId::LIST_STYLE_IMAGE,
        LonghandId::LIST_STYLE_TYPE,
    };

    // <'list-style-position'> || <'list-style-image'> || <'list-style-type'>
    static Res<> parse(Cursor<Css::Sst>& c, Vec<Rc<Property>>& stylesheet, ParsingMode) {
        Opt<ListPosition> position;
        Opt<ListImage> image;
        Opt<ListType> type;
        usize nones = 0;

        while (true) {
            eatWhitespace(c);
            if (c.ended())
                break;

            // none is valid for both list-style-image and list-style-type,
            // it is applied to whichever of the two isn't otherwise set.
            if (c.skip(Css::Token::ident("none"))) {
                nones++;
                continue;
            }

            if (not position) {
                if (auto maybePosition = parseValue<ListPosition>(c)) {
                    position = Some(maybePosition.take());
                    continue;
                }
            }

            if (not image) {
                if (auto maybeImage = parseValue<ListImage>(c)) {
                    image = Some(maybeImage.take());
                    continue;
                }
            }

            // NOTE: Tried last, as <counter-style> accepts any identifier.
            if (not type) {
                if (auto maybeType = parseValue<ListType>(c)) {
                    type = Some(maybeType.take());
                    continue;
                }
            }

            break;
        }

        if (not position and not image and not type and nones == 0)
            return Error::invalidData("expected list-style-position, list-style-image or list-style-type");

        if (not image and nones > 0) {
            image = Some<ListImage>(Keywords::NONE);
            nones--;
        }

        if (not type and nones > 0) {
            type = Some<ListType>(Keywords::NONE);
            nones--;
        }

        if (nones > 0)
            return Error::invalidData("too many none values");

        // NOTE: Omitted values are reset to their initial value.
        if (position)
            stylesheet.pushBack(makeRc<ListStylePositionProperty>(*position));
        else
            stylesheet.pushBack(initialProperty(LonghandId::LIST_STYLE_POSITION));

        if (image)
            stylesheet.pushBack(makeRc<ListStyleImageProperty>(*image));
        else
            stylesheet.pushBack(initialProperty(LonghandId::LIST_STYLE_IMAGE));

        if (type)
            stylesheet.pushBack(makeRc<ListStyleTypeProperty>(*type));
        else
            stylesheet.pushBack(initialProperty(LonghandId::LIST_STYLE_TYPE));

        return Ok();
    }
};

// https://www.w3.org/TR/css-lists-3/#propdef-marker-side
struct MarkerSideProperty : Property {
    static constexpr auto ID = LonghandId::MARKER_SIDE;
    static constexpr Flags<Options> FLAGS = {INHERITED, BULK_INHERITED};

    static Rc<Property> initial() {
        return makeRc<MarkerSideProperty>(Keywords::MATCH_SELF);
    }

    static Rc<Property> load(ComputedValues const& c) {
        return makeRc<MarkerSideProperty>(c.list->markerSide);
    }

    static Res<Rc<Property>> parse(Cursor<Css::Sst>& c, ParsingMode) {
        auto value = try$(parseValue<MarkerSide>(c));
        return Ok(makeRc<MarkerSideProperty>(value));
    }

    static void inherit(ComputedValues const& parent, ComputedValues& child) {
        child.list.cow().markerSide = parent.list->markerSide;
    }

    MarkerSide _value;

    MarkerSideProperty(MarkerSide value)
        : Property(ID), _value(value) {}

    void apply(ComputedValues const&, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.list.cow().markerSide = _value;
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

} // namespace Vaev::Style
