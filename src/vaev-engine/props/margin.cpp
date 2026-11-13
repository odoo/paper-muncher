module;

#include <karm/macros>

export module Vaev.Engine:props.margin;

import Karm.Core;
import :props.base;
import :css.parser;
import :style.computed;

using namespace Karm;

namespace Vaev::Style {

// https://drafts.csswg.org/css-box-4/#propdef-margin-top
export struct MarginTopProperty : Property {
    static constexpr auto ID = LonghandId::MARGIN_TOP;

    using Value = Union<Keywords::Auto, Calc<PercentOr<Length>>>;

    static Rc<Property> initial() {
        return makeRc<MarginTopProperty>(Calc<PercentOr<Length>>(Length{}));
    }

    static Rc<Property> load(ComputedValues const& c) {
        return makeRc<MarginTopProperty>(c.margin->top);
    }

    static Res<Rc<Property>> parse(Cursor<Css::Sst>& c, ParsingMode) {
        return Ok(makeRc<MarginTopProperty>(try$(parseValue<Value>(c))));
    }

    struct Registration : LonghandRegistration {
        Registration() : LonghandRegistration(Properties::MARGIN_TOP) {}
    };

    Value _value;

    MarginTopProperty(Value value)
        : Property(ID), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.margin.cow().top = _value;
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://drafts.csswg.org/css-box-4/#propdef-margin-right
export struct MarginRightProperty : Property {
    static constexpr auto ID = LonghandId::MARGIN_RIGHT;

    using Value = MarginTopProperty::Value;

    static Rc<Property> initial() {
        return makeRc<MarginRightProperty>(Calc<PercentOr<Length>>(Length{}));
    }

    static Rc<Property> load(ComputedValues const& c) {
        return makeRc<MarginRightProperty>(c.margin->end);
    }

    static Res<Rc<Property>> parse(Cursor<Css::Sst>& c, ParsingMode) {
        return Ok(makeRc<MarginRightProperty>(try$(parseValue<Value>(c))));
    }

    struct Registration : LonghandRegistration {
        Registration() : LonghandRegistration(Properties::MARGIN_RIGHT) {}
    };

    Value _value;

    MarginRightProperty(Value value)
        : Property(ID), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.margin.cow().end = _value;
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://drafts.csswg.org/css-box-4/#propdef-margin-bottom
export struct MarginBottomProperty : Property {
    static constexpr auto ID = LonghandId::MARGIN_BOTTOM;

    using Value = MarginTopProperty::Value;

    static Rc<Property> initial() {
        return makeRc<MarginBottomProperty>(Calc<PercentOr<Length>>(Length{}));
    }

    static Rc<Property> load(ComputedValues const& c) {
        return makeRc<MarginBottomProperty>(c.margin->bottom);
    }

    static Res<Rc<Property>> parse(Cursor<Css::Sst>& c, ParsingMode) {
        return Ok(makeRc<MarginBottomProperty>(try$(parseValue<Value>(c))));
    }

    struct Registration : LonghandRegistration {
        Registration() : LonghandRegistration(Properties::MARGIN_BOTTOM) {}
    };

    Value _value;

    MarginBottomProperty(Value value)
        : Property(ID), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.margin.cow().bottom = _value;
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://drafts.csswg.org/css-box-4/#propdef-margin-left
export struct MarginLeftProperty : Property {
    static constexpr auto ID = LonghandId::MARGIN_LEFT;

    using Value = MarginTopProperty::Value;

    static Rc<Property> initial() {
        return makeRc<MarginLeftProperty>(Calc<PercentOr<Length>>(Length{}));
    }

    static Rc<Property> load(ComputedValues const& c) {
        return makeRc<MarginLeftProperty>(c.margin->start);
    }

    static Res<Rc<Property>> parse(Cursor<Css::Sst>& c, ParsingMode) {
        return Ok(makeRc<MarginLeftProperty>(try$(parseValue<Value>(c))));
    }

    struct Registration : LonghandRegistration {
        Registration() : LonghandRegistration(Properties::MARGIN_LEFT) {}
    };

    Value _value;

    MarginLeftProperty(Value value)
        : Property(ID), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.margin.cow().start = _value;
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://drafts.csswg.org/css-box-4/#margin-shorthand
export struct MarginShorthand {
    struct Registration : ShorthandRegistration {
        Slice<Symbol> longhands() const override {
            static Array LONGHANDS = {
                Properties::FONT_STYLE,
                Properties::FONT_WEIGHT,
                Properties::FONT_WIDTH,
                Properties::FONT_SIZE,
                Properties::LINE_HEIGHT,
                Properties::FONT_FAMILY,
            };
            return LONGHANDS;
        }

        Res<> parse(Cursor<Css::Sst>& c, Vec<Rc<Property>>& stylesheet) const override {
            auto first = try$(parseValue<MarginTopProperty::Value>(c));

            auto second = parseValue<MarginTopProperty::Value>(c);

            // If there is only one component value, it applies to all sides.
            if (not second) {
                stylesheet.pushBack(makeRc<MarginTopProperty>(first));
                stylesheet.pushBack(makeRc<MarginBottomProperty>(first));
                stylesheet.pushBack(makeRc<MarginLeftProperty>(first));
                stylesheet.pushBack(makeRc<MarginRightProperty>(std::move(first)));
                return Ok();
            }

            auto third = parseValue<MarginTopProperty::Value>(c);

            // If there are two values, the top and bottom margins are set to the first value and
            // the right and left margins are set to the second.
            if (not third) {
                stylesheet.pushBack(makeRc<MarginTopProperty>(first));
                stylesheet.pushBack(makeRc<MarginBottomProperty>(std::move(first)));
                stylesheet.pushBack(makeRc<MarginLeftProperty>(second.expect()));
                stylesheet.pushBack(makeRc<MarginRightProperty>(second.take()));
                return Ok();
            }

            auto fourth = parseValue<MarginTopProperty::Value>(c);

            // If there are three values, the top is set to the first value, the left and right are set to the second,
            // and the bottom is set to the third.
            if (not fourth) {
                stylesheet.pushBack(makeRc<MarginTopProperty>(std::move(first)));
                stylesheet.pushBack(makeRc<MarginRightProperty>(second.expect()));
                stylesheet.pushBack(makeRc<MarginBottomProperty>(third.take()));
                stylesheet.pushBack(makeRc<MarginLeftProperty>(second.take()));
                return Ok();
            }

            // If there are four values they apply to the top, right, bottom, and left, respectively.
            stylesheet.pushBack(makeRc<MarginTopProperty>(std::move(first)));
            stylesheet.pushBack(makeRc<MarginRightProperty>(fourth.take()));
            stylesheet.pushBack(makeRc<MarginBottomProperty>(second.take()));
            stylesheet.pushBack(makeRc<MarginLeftProperty>(third.take()));

            return Ok();
        }
    };
};

// // https://drafts.csswg.org/css-box-4/#margin-shorthand
// export struct MarginProperty : Property {
//     using Value = Math::Insets<MarginTopProperty::Value>;
//
//     struct Registration : Property::Registration {
//         Registration()
//             : Property::Registration(Properties::MARGIN, SHORTHAND) {}
//
//         Rc<Property> initial() const override {
//             return makeRc<MarginProperty>(self(), Value{Calc<PercentOr<Length>>(Length{})});
//         }
//
//         Rc<Property> load(ComputedValues const& c) const override {
//             return makeRc<MarginProperty>(self(), *c.margin);
//         }
//
//         Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
//             return Ok(makeRc<MarginProperty>(self(), try$(parseValue<Value>(c))));
//         }
//     };
//
//     Value _value;
//
//     MarginProperty(Rc<Property::Registration> registration, Value value)
//         : Property(registration), _value(value) {}
//
//     Longhands expand(RegisteredPropertySet& registry, ComputedValues const&, ComputedValues&) const override {
//         return {
//             makeRc<MarginTopProperty>(registry.resolveRegistration(Properties::MARGIN_TOP, {}).expect(), _value.top),
//             makeRc<MarginBottomProperty>(registry.resolveRegistration(Properties::MARGIN_BOTTOM, {}).expect(), _value.bottom),
//             makeRc<MarginLeftProperty>(registry.resolveRegistration(Properties::MARGIN_LEFT, {}).expect(), _value.start),
//             makeRc<MarginRightProperty>(registry.resolveRegistration(Properties::MARGIN_RIGHT, {}).expect(), _value.end),
//         };
//     }
//
//     void repr(Io::Emit& e) const override {
//         e("{}", _value);
//     }
// };

// https://drafts.csswg.org/css-logical/#propdef-margin-inline-start
export struct MarginInlineStartProperty : Property {
    static constexpr auto ID = LonghandId::MARGIN_INLINE_START;

    using Value = MarginTopProperty::Value;

    static Rc<Property> initial() {
        return makeRc<MarginInlineStartProperty>(Calc<PercentOr<Length>>(Length{}));
    }

    static Rc<Property> load(ComputedValues const& c) {
        return makeRc<MarginInlineStartProperty>(c.margin->start);
    }

    static Res<Rc<Property>> parse(Cursor<Css::Sst>& c, ParsingMode) {
        return Ok(makeRc<MarginInlineStartProperty>(try$(parseValue<Value>(c))));
    }

    struct Registration : LonghandRegistration {
        Registration() : LonghandRegistration(Properties::MARGIN_INLINE_START) {}
    };

    Value _value;

    MarginInlineStartProperty(Value value)
        : Property(ID), _value(value) {}

    // FIXME: Take writing mode into account
    // FIXME: Cascade together with the paired physical property, the declaration with
    //        the higher priority must win, not the one applied last.
    //        https://drafts.csswg.org/css-logical-1/#logical-property-group
    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.margin.cow().start = _value;
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://drafts.csswg.org/css-logical/#propdef-margin-inline-end
export struct MarginInlineEndProperty : Property {
    static constexpr auto ID = LonghandId::MARGIN_INLINE_END;

    using Value = MarginTopProperty::Value;

    static Rc<Property> initial() {
        return makeRc<MarginInlineEndProperty>(Calc<PercentOr<Length>>(Length{}));
    }

    static Rc<Property> load(ComputedValues const& c) {
        return makeRc<MarginInlineEndProperty>(c.margin->end);
    }

    static Res<Rc<Property>> parse(Cursor<Css::Sst>& c, ParsingMode) {
        return Ok(makeRc<MarginInlineEndProperty>(try$(parseValue<Value>(c))));
    }

    struct Registration : LonghandRegistration {
        Registration() : LonghandRegistration(Properties::MARGIN_INLINE_END) {}
    };

    Value _value;

    MarginInlineEndProperty(Value value)
        : Property(ID), _value(value) {}

    // FIXME: Take writing mode into account
    // FIXME: Cascade together with the paired physical property, the declaration with
    //        the higher priority must win, not the one applied last.
    //        https://drafts.csswg.org/css-logical-1/#logical-property-group
    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.margin.cow().end = _value;
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// // https://drafts.csswg.org/css-logical/#propdef-margin-inline
// export struct MarginInlineProperty : Property {
//     using Value = Math::Insets<MarginTopProperty::Value>;
//
//     struct Registration : Property::Registration {
//         Registration() : Property::Registration(Properties::MARGIN_INLINE, LOGICAL) {}
//
//         Rc<Property> initial() const override {
//             return makeRc<MarginInlineProperty>(self(), Value{Calc<PercentOr<Length>>(Length{})});
//         }
//
//         Rc<Property> load(ComputedValues const& c) const override {
//             return makeRc<MarginInlineProperty>(self(), Math::Insets{c.margin->start, c.margin->end});
//         }
//
//         Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
//             return Ok(makeRc<MarginInlineProperty>(self(), try$(parseValue<Value>(c))));
//         }
//     };
//
//     Value _value;
//
//     MarginInlineProperty(Rc<Property::Registration> registration, Value value)
//         : Property(registration), _value(value) {}
//
//     // FIXME: Take writing mode into account
//     Longhands expand(RegisteredPropertySet& registry, ComputedValues const&, ComputedValues&) const override {
//         return {
//             makeRc<MarginLeftProperty>(registry.resolveRegistration(Properties::MARGIN_LEFT, {}).expect(), _value.start),
//             makeRc<MarginRightProperty>(registry.resolveRegistration(Properties::MARGIN_RIGHT, {}).expect(), _value.end),
//         };
//     }
//
//     void repr(Io::Emit& e) const override {
//         e("{}", _value);
//     }
// };

// https://drafts.csswg.org/css-logical/#propdef-margin-block-start
export struct MarginBlockStartProperty : Property {
    static constexpr auto ID = LonghandId::MARGIN_BLOCK_START;

    using Value = MarginTopProperty::Value;

    static Rc<Property> initial() {
        return makeRc<MarginBlockStartProperty>(Calc<PercentOr<Length>>(Length{}));
    }

    static Rc<Property> load(ComputedValues const& c) {
        return makeRc<MarginBlockStartProperty>(c.margin->top);
    }

    static Res<Rc<Property>> parse(Cursor<Css::Sst>& c, ParsingMode) {
        return Ok(makeRc<MarginBlockStartProperty>(try$(parseValue<Value>(c))));
    }

    struct Registration : LonghandRegistration {
        Registration() : LonghandRegistration(Properties::MARGIN_BLOCK_START) {}
    };

    Value _value;

    MarginBlockStartProperty(Value value)
        : Property(ID), _value(value) {}

    // FIXME: Take writing mode into account
    // FIXME: Cascade together with the paired physical property, the declaration with
    //        the higher priority must win, not the one applied last.
    //        https://drafts.csswg.org/css-logical-1/#logical-property-group
    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.margin.cow().top = _value;
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://drafts.csswg.org/css-logical/#propdef-margin-block-end
export struct MarginBlockEndProperty : Property {
    static constexpr auto ID = LonghandId::MARGIN_BLOCK_END;

    using Value = MarginTopProperty::Value;

    static Rc<Property> initial() {
        return makeRc<MarginBlockEndProperty>(Calc<PercentOr<Length>>(Length{}));
    }

    static Rc<Property> load(ComputedValues const& c) {
        return makeRc<MarginBlockEndProperty>(c.margin->bottom);
    }

    static Res<Rc<Property>> parse(Cursor<Css::Sst>& c, ParsingMode) {
        return Ok(makeRc<MarginBlockEndProperty>(try$(parseValue<Value>(c))));
    }

    struct Registration : LonghandRegistration {
        Registration() : LonghandRegistration(Properties::MARGIN_BLOCK_END) {}
    };

    Union<Keywords::Auto, Calc<PercentOr<Length>>> _value;

    MarginBlockEndProperty(Value value)
        : Property(ID), _value(value) {}

    // FIXME: Take writing mode into account
    // FIXME: Cascade together with the paired physical property, the declaration with
    //        the higher priority must win, not the one applied last.
    //        https://drafts.csswg.org/css-logical-1/#logical-property-group
    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.margin.cow().bottom = _value;
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// // https://drafts.csswg.org/css-logical/#propdef-margin-block
// export struct MarginBlockProperty : Property {
//     using Value = Math::Insets<MarginTopProperty::Value>;
//
//     struct Registration : Property::Registration {
//         Registration() : Property::Registration(Properties::MARGIN_BLOCK, LOGICAL) {}
//
//         Rc<Property> initial() const override {
//             return makeRc<MarginBlockProperty>(self(), Value{Calc<PercentOr<Length>>(Length{})});
//         }
//
//         Rc<Property> load(ComputedValues const& c) const override {
//             return makeRc<MarginBlockProperty>(self(), Math::Insets{c.margin->top, c.margin->bottom});
//         }
//
//         Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
//             return Ok(makeRc<MarginBlockProperty>(self(), try$(parseValue<Value>(c))));
//         }
//     };
//
//     Value _value;
//
//     MarginBlockProperty(Rc<Property::Registration> registration, Value value)
//         : Property(registration), _value(value) {}
//
//     // FIXME: Take writing mode into account
//     Longhands expand(RegisteredPropertySet& registry, ComputedValues const&, ComputedValues&) const override {
//         return {
//             makeRc<MarginTopProperty>(registry.resolveRegistration(Properties::MARGIN_TOP, {}).expect(), _value.top),
//             makeRc<MarginBottomProperty>(registry.resolveRegistration(Properties::MARGIN_BOTTOM, {}).expect(), _value.bottom),
//         };
//     }
//
//     void repr(Io::Emit& e) const override {
//         e("{}", _value);
//     }
// };

} // namespace Vaev::Style
