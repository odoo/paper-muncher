module;

#include <karm/macros>

export module Vaev.Engine:props.list;

import :props.base;

namespace Vaev::Style {

// https://www.w3.org/TR/css-lists-3/#propdef-list-style-image
struct ListStyleImageProperty : Property {
    static constexpr auto ID = LonghandId::LIST_STYLE_IMAGE;
    static inline Flags<Options> const FLAGS = {INHERITED, BULK_INHERITED};

    static Rc<Property> initial() {
        return makeRc<ListStyleImageProperty>(Keywords::NONE);
    }

    static Rc<Property> load(ComputedValues const& c) {
        return makeRc<ListStyleImageProperty>(c.list->image);
    }

    static Res<Rc<Property>> parse(Cursor<Css::Sst>& c) {
        auto value = try$(parseValue<ListImage>(c));
        return Ok(makeRc<ListStyleImageProperty>(value));
    }

    static void inherit(ComputedValues const& parent, ComputedValues& child) {
        child.list.cow().image = parent.list->image;
    }

    struct Registration : LonghandRegistration {
        Registration()
            : LonghandRegistration(Properties::LIST_STYLE_IMAGE, FLAGS) {}
    };

    ListImage _value;

    ListStyleImageProperty(ListImage value)
        : Property(registrationOf<ListStyleImageProperty>()), _value(value) {}

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
    static inline Flags<Options> const FLAGS = {INHERITED, BULK_INHERITED};

    static Rc<Property> initial() {
        return makeRc<ListStyleTypeProperty>(CustomIdent{"disc"_sym});
    }

    static Rc<Property> load(ComputedValues const& c) {
        return makeRc<ListStyleTypeProperty>(c.list->type);
    }

    static Res<Rc<Property>> parse(Cursor<Css::Sst>& c) {
        auto value = try$(parseValue<ListType>(c));
        return Ok(makeRc<ListStyleTypeProperty>(value));
    }

    static void inherit(ComputedValues const& parent, ComputedValues& child) {
        child.list.cow().type = parent.list->type;
    }

    struct Registration : LonghandRegistration {
        Registration()
            : LonghandRegistration(Properties::LIST_STYLE_TYPE, FLAGS) {}
    };

    ListType _value;

    ListStyleTypeProperty(ListType value)
        : Property(registrationOf<ListStyleTypeProperty>()), _value(value) {}

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
    static inline Flags<Options> const FLAGS = {INHERITED, BULK_INHERITED};

    static Rc<Property> initial() {
        return makeRc<ListStylePositionProperty>(Keywords::OUTSIDE);
    }

    static Rc<Property> load(ComputedValues const& c) {
        return makeRc<ListStylePositionProperty>(c.list->position);
    }

    static Res<Rc<Property>> parse(Cursor<Css::Sst>& c) {
        auto value = try$(parseValue<ListPosition>(c));
        return Ok(makeRc<ListStylePositionProperty>(value));
    }

    static void inherit(ComputedValues const& parent, ComputedValues& child) {
        child.list.cow().position = parent.list->position;
    }

    struct Registration : LonghandRegistration {
        Registration()
            : LonghandRegistration(Properties::LIST_STYLE_POSITION, FLAGS) {}
    };

    ListPosition _value;

    ListStylePositionProperty(ListPosition value)
        : Property(registrationOf<ListStylePositionProperty>()), _value(value) {}

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

// // https://www.w3.org/TR/css-lists-3/#propdef-list-style
// struct ListStyleProperty : Property {
//     struct Value {
//         ListImage image = Keywords::NONE;
//         ListType type = CustomIdent{"disc"_sym};
//         ListPosition position = Keywords::OUTSIDE;
//
//         void repr(Io::Emit& e) const {
//             e("(list-style {} {} {})", image, type, position);
//         }
//     };
//
//     struct Registration : Property::Registration {
//         Registration()
//             : Property::Registration(Properties::LIST_STYLE, SHORTHAND) {}
//
//         Rc<Property> initial() const override {
//             return makeRc<ListStyleProperty>(self(), Value{});
//         }
//
//         Rc<Property> load(ComputedValues const& c) const override {
//             return makeRc<ListStyleProperty>(
//                 self(),
//                 Value{
//                     c.list->image,
//                     c.list->type,
//                     c.list->position,
//                 }
//             );
//         }
//
//         Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
//             Value value;
//             while (not c.ended()) {
//                 // None is ambiguous, as none is a valid value for both list-style-image and list-style-type.
//                 // FIXME: There is a thing about setting only the one that was
//                 //        not set yet, but we need better parseValue infra for that.
//                 if (parseValue<Keywords::None>(c)) {
//                     value.type = Keywords::NONE;
//                     value.image = Keywords::NONE;
//                     continue;
//                 }
//
//                 if (auto image = parseValue<ListImage>(c)) {
//                     value.image = image.take();
//                     continue;
//                 }
//
//                 if (auto type = parseValue<ListType>(c)) {
//                     value.type = type.take();
//                     continue;
//                 }
//
//                 if (auto position = parseValue<ListPosition>(c)) {
//                     value.position = position.take();
//                     continue;
//                 }
//
//                 return Error::invalidData("unexpected token");
//             }
//             return Ok(makeRc<ListStyleProperty>(self(), std::move(value)));
//         }
//     };
//
//     Value _value;
//
//     ListStyleProperty(Rc<Property::Registration> registration, Value value)
//         : Property(registration), _value(value) {}
//
//     Longhands expand(RegisteredPropertySet& registry, ComputedValues const&, ComputedValues&) const override {
//         return {
//             makeRc<ListStyleImageProperty>(registry.resolveRegistration(Properties::LIST_STYLE_IMAGE, {}).expect(), _value.image),
//             makeRc<ListStyleTypeProperty>(registry.resolveRegistration(Properties::LIST_STYLE_TYPE, {}).expect(), _value.type),
//             makeRc<ListStylePositionProperty>(registry.resolveRegistration(Properties::LIST_STYLE_POSITION, {}).expect(), _value.position),
//         };
//     }
//
//     void repr(Io::Emit& e) const override {
//         e("{}", _value);
//     }
// };

// https://www.w3.org/TR/css-lists-3/#propdef-marker-side
struct MarkerSideProperty : Property {
    static constexpr auto ID = LonghandId::MARKER_SIDE;
    static inline Flags<Options> const FLAGS = {INHERITED, BULK_INHERITED};

    static Rc<Property> initial() {
        return makeRc<MarkerSideProperty>(Keywords::MATCH_SELF);
    }

    static Rc<Property> load(ComputedValues const& c) {
        return makeRc<MarkerSideProperty>(c.list->markerSide);
    }

    static Res<Rc<Property>> parse(Cursor<Css::Sst>& c) {
        auto value = try$(parseValue<MarkerSide>(c));
        return Ok(makeRc<MarkerSideProperty>(value));
    }

    static void inherit(ComputedValues const& parent, ComputedValues& child) {
        child.list.cow().markerSide = parent.list->markerSide;
    }

    struct Registration : LonghandRegistration {
        Registration()
            : LonghandRegistration(Properties::MARKER_SIDE, FLAGS) {}
    };

    MarkerSide _value;

    MarkerSideProperty(MarkerSide value)
        : Property(registrationOf<MarkerSideProperty>()), _value(value) {}

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
