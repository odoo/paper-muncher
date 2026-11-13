module;

#include <karm/macros>

export module Vaev.Engine:props.breaks;

import Karm.Core;
import :props.base;
import :css.parser;
import :style.computed;

using namespace Karm;

namespace Vaev::Style {

// MARK: Breaks ----------------------------------------------------------------

// https://www.w3.org/TR/css-break-3/#propdef-break-after
export struct BreakAfterProperty : Property {
    static constexpr auto ID = LonghandId::BREAK_AFTER;

    static Rc<Property> initial() {
        return makeRc<BreakAfterProperty>(BreakBetween::AUTO);
    }

    static Rc<Property> load(ComputedValues const& c) {
        return makeRc<BreakAfterProperty>(c.break_->after);
    }

    static Res<Rc<Property>> parse(Cursor<Css::Sst>& c) {
        return Ok(makeRc<BreakAfterProperty>(try$(parseValue<BreakBetween>(c))));
    }

    struct Registration : LonghandRegistration {
        Registration() : LonghandRegistration(Properties::BREAK_AFTER) {}
    };

    BreakBetween _value;

    BreakAfterProperty(BreakBetween value)
        : Property(registrationOf<BreakAfterProperty>()), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.break_.cow().after = _value;
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/css-break-3/#propdef-break-before
export struct BreakBeforeProperty : Property {
    static constexpr auto ID = LonghandId::BREAK_BEFORE;

    static Rc<Property> initial() {
        return makeRc<BreakBeforeProperty>(BreakBetween::AUTO);
    }

    static Rc<Property> load(ComputedValues const& c) {
        return makeRc<BreakBeforeProperty>(c.break_->before);
    }

    static Res<Rc<Property>> parse(Cursor<Css::Sst>& c) {
        return Ok(makeRc<BreakBeforeProperty>(try$(parseValue<BreakBetween>(c))));
    }

    struct Registration : LonghandRegistration {
        Registration() : LonghandRegistration(Properties::BREAK_BEFORE) {}
    };

    BreakBetween _value;

    BreakBeforeProperty(BreakBetween value)
        : Property(registrationOf<BreakBeforeProperty>()), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.break_.cow().before = _value;
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/css-break-3/#break-within
export struct BreakInsideProperty : Property {
    static constexpr auto ID = LonghandId::BREAK_INSIDE;

    static Rc<Property> initial() {
        return makeRc<BreakInsideProperty>(BreakInside::AUTO);
    }

    static Rc<Property> load(ComputedValues const& c) {
        return makeRc<BreakInsideProperty>(c.break_->inside);
    }

    static Res<Rc<Property>> parse(Cursor<Css::Sst>& c) {
        return Ok(makeRc<BreakInsideProperty>(try$(parseValue<BreakInside>(c))));
    }

    struct Registration : LonghandRegistration {
        Registration() : LonghandRegistration(Properties::BREAK_INSIDE) {}
    };

    BreakInside _value;

    BreakInsideProperty(BreakInside value)
        : Property(registrationOf<BreakInsideProperty>()), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.break_.cow().inside = _value;
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// MARK: Page Break Aliases --------------------------------------------------
//
// // https://www.w3.org/TR/css-break-3/#page-break-properties
// static Res<BreakBetween> _parseLegacyPageBreak(Cursor<Css::Sst>& c) {
//     if (c.skip(Css::Token::ident("auto")))
//         return Ok(BreakBetween::AUTO);
//     if (c.skip(Css::Token::ident("always")))
//         return Ok(BreakBetween::PAGE);
//     if (c.skip(Css::Token::ident("avoid")))
//         return Ok(BreakBetween::AVOID);
//     if (c.skip(Css::Token::ident("left")))
//         return Ok(BreakBetween::LEFT);
//     if (c.skip(Css::Token::ident("right")))
//         return Ok(BreakBetween::RIGHT);
//     return Error::invalidData("expected auto, always, avoid, left or right");
// }
//
// static void _reprLegacyPageBreak(Io::Emit& e, BreakBetween value) {
//     if (value == BreakBetween::PAGE)
//         e("always");
//     else
//         e("{}", value);
// }

// // https://www.w3.org/TR/css-break-3/#page-break-properties
// export struct PageBreakAfterProperty : Property {
//     struct Registration : Property::Registration {
//         Registration() : Property::Registration(Properties::PAGE_BREAK_AFTER, SHORTHAND) {}
//
//         Rc<Property> initial() const override {
//             return makeRc<PageBreakAfterProperty>(self(), BreakBetween::AUTO);
//         }
//
//         Rc<Property> load(ComputedValues const& c) const override {
//             return makeRc<PageBreakAfterProperty>(self(), c.break_->after);
//         }
//
//         Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
//             return Ok(makeRc<PageBreakAfterProperty>(self(), try$(_parseLegacyPageBreak(c))));
//         }
//     };
//
//     BreakBetween _value;
//
//     PageBreakAfterProperty(Rc<Property::Registration> registration, BreakBetween value)
//         : Property(registration), _value(value) {}
//
//     Longhands expand(RegisteredPropertySet& registry, ComputedValues const&, ComputedValues&) const override {
//         return {
//             makeRc<BreakAfterProperty>(registry.resolveRegistration(Properties::BREAK_AFTER, {}).expect(), _value),
//         };
//     }
//
//     void repr(Io::Emit& e) const override {
//         _reprLegacyPageBreak(e, _value);
//     }
// };

// // https://www.w3.org/TR/css-break-3/#page-break-properties
// export struct PageBreakBeforeProperty : Property {
//     struct Registration : Property::Registration {
//         Registration() : Property::Registration(Properties::PAGE_BREAK_BEFORE, SHORTHAND) {}
//
//         Rc<Property> initial() const override {
//             return makeRc<PageBreakBeforeProperty>(self(), BreakBetween::AUTO);
//         }
//
//         Rc<Property> load(ComputedValues const& c) const override {
//             return makeRc<PageBreakBeforeProperty>(self(), c.break_->before);
//         }
//
//         Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
//             return Ok(makeRc<PageBreakBeforeProperty>(self(), try$(_parseLegacyPageBreak(c))));
//         }
//     };
//
//     BreakBetween _value;
//
//     PageBreakBeforeProperty(Rc<Property::Registration> registration, BreakBetween value)
//         : Property(registration), _value(value) {}
//
//     Longhands expand(RegisteredPropertySet& registry, ComputedValues const&, ComputedValues&) const override {
//         return {
//             makeRc<BreakBeforeProperty>(registry.resolveRegistration(Properties::BREAK_BEFORE, {}).expect(), _value),
//         };
//     }
//
//     void repr(Io::Emit& e) const override {
//         _reprLegacyPageBreak(e, _value);
//     }
// };

// // https://www.w3.org/TR/css-break-3/#page-break-properties
// export struct PageBreakInsideProperty : Property {
//     struct Registration : Property::Registration {
//         Registration() : Property::Registration(Properties::PAGE_BREAK_INSIDE, SHORTHAND) {}
//
//         Rc<Property> initial() const override {
//             return makeRc<PageBreakInsideProperty>(self(), BreakInside::AUTO);
//         }
//
//         Rc<Property> load(ComputedValues const& c) const override {
//             return makeRc<PageBreakInsideProperty>(self(), c.break_->inside);
//         }
//
//         Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
//             if (c.skip(Css::Token::ident("auto")))
//                 return Ok(makeRc<PageBreakInsideProperty>(self(), BreakInside::AUTO));
//             if (c.skip(Css::Token::ident("avoid")))
//                 return Ok(makeRc<PageBreakInsideProperty>(self(), BreakInside::AVOID));
//             return Error::invalidData("expected auto or avoid");
//         }
//     };
//
//     BreakInside _value;
//
//     PageBreakInsideProperty(Rc<Property::Registration> registration, BreakInside value)
//         : Property(registration), _value(value) {}
//
//     Longhands expand(RegisteredPropertySet& registry, ComputedValues const&, ComputedValues&) const override {
//         return {
//             makeRc<BreakInsideProperty>(registry.resolveRegistration(Properties::BREAK_INSIDE, {}).expect(), _value),
//         };
//     }
//
//     void repr(Io::Emit& e) const override {
//         e("{}", _value);
//     }
// };

} // namespace Vaev::Style
