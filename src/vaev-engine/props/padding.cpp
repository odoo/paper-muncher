module;

#include <karm/macros>

export module Vaev.Engine:props.padding;

import Karm.Core;
import :props.base;
import :css.parser;
import :style.computed;

using namespace Karm;

namespace Vaev::Style {

// MARK: Padding ---------------------------------------------------------------

// https://www.w3.org/TR/css-box-3/#propdef-padding

export struct PaddingTopProperty : Property {
    static constexpr auto ID = LonghandId::PADDING_TOP;

    static Rc<Property> initial() {
        return makeRc<PaddingTopProperty>(Calc<PercentOr<Length>>{Length{}});
    }

    static Rc<Property> load(ComputedValues const& c) {
        return makeRc<PaddingTopProperty>(c.padding->top);
    }

    static Res<Rc<Property>> parse(Cursor<Css::Sst>& c) {
        return Ok(makeRc<PaddingTopProperty>(try$(parseValue<Calc<PercentOr<Length>>>(c))));
    }

    struct Registration : LonghandRegistration {
        Registration() : LonghandRegistration(Properties::PADDING_TOP) {}
    };

    Calc<PercentOr<Length>> _value;

    PaddingTopProperty(Calc<PercentOr<Length>> value)
        : Property(registrationOf<PaddingTopProperty>()), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.padding.cow().top = _value;
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

export struct PaddingRightProperty : Property {
    static constexpr auto ID = LonghandId::PADDING_RIGHT;

    static Rc<Property> initial() {
        return makeRc<PaddingRightProperty>(Calc<PercentOr<Length>>{Length{}});
    }

    static Rc<Property> load(ComputedValues const& c) {
        return makeRc<PaddingRightProperty>(c.padding->end);
    }

    static Res<Rc<Property>> parse(Cursor<Css::Sst>& c) {
        return Ok(makeRc<PaddingRightProperty>(try$(parseValue<Calc<PercentOr<Length>>>(c))));
    }

    struct Registration : LonghandRegistration {
        Registration() : LonghandRegistration(Properties::PADDING_RIGHT) {}
    };

    Calc<PercentOr<Length>> _value;

    PaddingRightProperty(Calc<PercentOr<Length>> value)
        : Property(registrationOf<PaddingRightProperty>()), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.padding.cow().end = _value;
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

export struct PaddingBottomProperty : Property {
    static constexpr auto ID = LonghandId::PADDING_BOTTOM;

    static Rc<Property> initial() {
        return makeRc<PaddingBottomProperty>(Calc<PercentOr<Length>>{Length{}});
    }

    static Rc<Property> load(ComputedValues const& c) {
        return makeRc<PaddingBottomProperty>(c.padding->bottom);
    }

    static Res<Rc<Property>> parse(Cursor<Css::Sst>& c) {
        return Ok(makeRc<PaddingBottomProperty>(try$(parseValue<Calc<PercentOr<Length>>>(c))));
    }

    struct Registration : LonghandRegistration {
        Registration() : LonghandRegistration(Properties::PADDING_BOTTOM) {}
    };

    Calc<PercentOr<Length>> _value;

    PaddingBottomProperty(Calc<PercentOr<Length>> value)
        : Property(registrationOf<PaddingBottomProperty>()), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.padding.cow().bottom = _value;
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

export struct PaddingLeftProperty : Property {
    static constexpr auto ID = LonghandId::PADDING_LEFT;

    static Rc<Property> initial() {
        return makeRc<PaddingLeftProperty>(Calc<PercentOr<Length>>{Length{}});
    }

    static Rc<Property> load(ComputedValues const& c) {
        return makeRc<PaddingLeftProperty>(c.padding->start);
    }

    static Res<Rc<Property>> parse(Cursor<Css::Sst>& c) {
        return Ok(makeRc<PaddingLeftProperty>(try$(parseValue<Calc<PercentOr<Length>>>(c))));
    }

    struct Registration : LonghandRegistration {
        Registration() : LonghandRegistration(Properties::PADDING_LEFT) {}
    };

    Calc<PercentOr<Length>> _value;

    PaddingLeftProperty(Calc<PercentOr<Length>> value)
        : Property(registrationOf<PaddingLeftProperty>()), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.padding.cow().start = _value;
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

export struct PaddingInlineStartProperty : Property {
    static constexpr auto ID = LonghandId::PADDING_INLINE_START;

    static Rc<Property> initial() {
        return makeRc<PaddingInlineStartProperty>(Calc<PercentOr<Length>>{Length{}});
    }

    static Rc<Property> load(ComputedValues const& c) {
        return makeRc<PaddingInlineStartProperty>(c.padding->start);
    }

    static Res<Rc<Property>> parse(Cursor<Css::Sst>& c) {
        return Ok(makeRc<PaddingInlineStartProperty>(try$(parseValue<Calc<PercentOr<Length>>>(c))));
    }

    struct Registration : LonghandRegistration {
        Registration() : LonghandRegistration(Properties::PADDING_INLINE_START) {}
    };

    Calc<PercentOr<Length>> _value;

    PaddingInlineStartProperty(Calc<PercentOr<Length>> value)
        : Property(registrationOf<PaddingInlineStartProperty>()), _value(value) {}

    // FIXME: Take writing mode into account
    // FIXME: Cascade together with the paired physical property, the declaration with
    //        the higher priority must win, not the one applied last.
    //        https://drafts.csswg.org/css-logical-1/#logical-property-group
    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.padding.cow().start = _value;
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

export struct PaddingInlineEndProperty : Property {
    static constexpr auto ID = LonghandId::PADDING_INLINE_END;

    static Rc<Property> initial() {
        return makeRc<PaddingInlineEndProperty>(Calc<PercentOr<Length>>{Length{}});
    }

    static Rc<Property> load(ComputedValues const& c) {
        return makeRc<PaddingInlineEndProperty>(c.padding->end);
    }

    static Res<Rc<Property>> parse(Cursor<Css::Sst>& c) {
        return Ok(makeRc<PaddingInlineEndProperty>(try$(parseValue<Calc<PercentOr<Length>>>(c))));
    }

    struct Registration : LonghandRegistration {
        Registration() : LonghandRegistration(Properties::PADDING_INLINE_END) {}
    };

    Calc<PercentOr<Length>> _value;

    PaddingInlineEndProperty(Calc<PercentOr<Length>> value)
        : Property(registrationOf<PaddingInlineEndProperty>()), _value(value) {}

    // FIXME: Take writing mode into account
    // FIXME: Cascade together with the paired physical property, the declaration with
    //        the higher priority must win, not the one applied last.
    //        https://drafts.csswg.org/css-logical-1/#logical-property-group
    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.padding.cow().end = _value;
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// export struct PaddingProperty : Property {
//     struct Registration : Property::Registration {
//         Registration()
//             : Property::Registration(Properties::PADDING, SHORTHAND) {}
//
//         Rc<Property> initial() const override {
//             return makeRc<PaddingProperty>(self(), Math::Insets<Calc<PercentOr<Length>>>{Length{}});
//         }
//
//         Rc<Property> load(ComputedValues const& c) const override {
//             return makeRc<PaddingProperty>(self(), *c.padding);
//         }
//
//         Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
//             return Ok(makeRc<PaddingProperty>(self(), try$(parseValue<Math::Insets<Calc<PercentOr<Length>>>>(c))));
//         }
//     };
//
//     Math::Insets<Calc<PercentOr<Length>>> _value;
//
//     PaddingProperty(Rc<Property::Registration> registration, Math::Insets<Calc<PercentOr<Length>>> value)
//         : Property(registration), _value(value) {}
//
//     Longhands expand(RegisteredPropertySet& registry, ComputedValues const&, ComputedValues&) const override {
//         return {
//             makeRc<PaddingTopProperty>(registry.resolveRegistration(Properties::PADDING_TOP, {}).expect(), _value.top),
//             makeRc<PaddingRightProperty>(registry.resolveRegistration(Properties::PADDING_RIGHT, {}).expect(), _value.end),
//             makeRc<PaddingBottomProperty>(registry.resolveRegistration(Properties::PADDING_BOTTOM, {}).expect(), _value.bottom),
//             makeRc<PaddingLeftProperty>(registry.resolveRegistration(Properties::PADDING_LEFT, {}).expect(), _value.start),
//         };
//     }
//
//     void repr(Io::Emit& e) const override {
//         e("{}", _value);
//     }
// };

} // namespace Vaev::Style
