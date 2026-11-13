module;

#include <karm/macros>

export module Vaev.Engine:props.flex;

import Karm.Core;
import :props.base;
import :css.parser;
import :style.computed;

using namespace Karm;

namespace Vaev::Style {

// MARK: Flex ------------------------------------------------------------------

// https://www.w3.org/TR/css-flexbox-1/#flex-basis-property
export struct FlexBasisProperty : Property {
    static constexpr auto ID = LonghandId::FLEX_BASIS;

    static Rc<Property> initial() {
        return makeRc<FlexBasisProperty>(Keywords::AUTO);
    }

    static Rc<Property> load(ComputedValues const& c) {
        return makeRc<FlexBasisProperty>(c.flex->basis);
    }

    static Res<Rc<Property>> parse(Cursor<Css::Sst>& c, ParsingMode) {
        return Ok(makeRc<FlexBasisProperty>(try$(parseValue<FlexBasis>(c))));
    }

    struct Registration : LonghandRegistration {
        Registration() : LonghandRegistration(Properties::FLEX_BASIS) {}
    };

    FlexBasis _value;

    FlexBasisProperty(FlexBasis value)
        : Property(ID), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.flex.cow().basis = _value;
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/css-flexbox-1/#propdef-flex-direction
export struct FlexDirectionProperty : Property {
    static constexpr auto ID = LonghandId::FLEX_DIRECTION;

    static Rc<Property> initial() {
        return makeRc<FlexDirectionProperty>(FlexDirection::ROW);
    }

    static Rc<Property> load(ComputedValues const& c) {
        return makeRc<FlexDirectionProperty>(c.flex->direction);
    }

    static Res<Rc<Property>> parse(Cursor<Css::Sst>& c, ParsingMode) {
        return Ok(makeRc<FlexDirectionProperty>(try$(parseValue<FlexDirection>(c))));
    }

    struct Registration : LonghandRegistration {
        Registration() : LonghandRegistration(Properties::FLEX_DIRECTION) {}
    };

    FlexDirection _value;

    FlexDirectionProperty(FlexDirection value)
        : Property(ID), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.flex.cow().direction = _value;
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/css-flexbox-1/#flex-grow-property
export struct FlexGrowProperty : Property {
    static constexpr auto ID = LonghandId::FLEX_GROW;

    static Rc<Property> initial() {
        return makeRc<FlexGrowProperty>(Number{0});
    }

    static Rc<Property> load(ComputedValues const& c) {
        return makeRc<FlexGrowProperty>(c.flex->grow);
    }

    static Res<Rc<Property>> parse(Cursor<Css::Sst>& c, ParsingMode) {
        return Ok(makeRc<FlexGrowProperty>(try$(parseValue<Number>(c))));
    }

    struct Registration : LonghandRegistration {
        Registration() : LonghandRegistration(Properties::FLEX_GROW) {}
    };

    Number _value;

    FlexGrowProperty(Number value)
        : Property(ID), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.flex.cow().grow = _value;
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/css-flexbox-1/#propdef-flex-shrink
export struct FlexShrinkProperty : Property {
    static constexpr auto ID = LonghandId::FLEX_SHRINK;

    static Rc<Property> initial() {
        return makeRc<FlexShrinkProperty>(Number{1});
    }

    static Rc<Property> load(ComputedValues const& c) {
        return makeRc<FlexShrinkProperty>(c.flex->shrink);
    }

    static Res<Rc<Property>> parse(Cursor<Css::Sst>& c, ParsingMode) {
        return Ok(makeRc<FlexShrinkProperty>(try$(parseValue<Number>(c))));
    }

    struct Registration : LonghandRegistration {
        Registration() : LonghandRegistration(Properties::FLEX_SHRINK) {}
    };

    Number _value;

    FlexShrinkProperty(Number value)
        : Property(ID), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.flex.cow().shrink = _value;
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/css-flexbox-1/#propdef-flex-wrap
export struct FlexWrapProperty : Property {
    static constexpr auto ID = LonghandId::FLEX_WRAP;

    static Rc<Property> initial() {
        return makeRc<FlexWrapProperty>(FlexWrap::NOWRAP);
    }

    static Rc<Property> load(ComputedValues const& c) {
        return makeRc<FlexWrapProperty>(c.flex->wrap);
    }

    static Res<Rc<Property>> parse(Cursor<Css::Sst>& c, ParsingMode) {
        return Ok(makeRc<FlexWrapProperty>(try$(parseValue<FlexWrap>(c))));
    }

    struct Registration : LonghandRegistration {
        Registration() : LonghandRegistration(Properties::FLEX_WRAP) {}
    };

    FlexWrap _value;

    FlexWrapProperty(FlexWrap value)
        : Property(ID), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.flex.cow().wrap = _value;
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// // https://www.w3.org/TR/css-flexbox-1/#propdef-flex-flow
// export struct FlexFlowProperty : Property {
//     struct Registration : Property::Registration {
//         Registration()
//             : Property::Registration(Properties::FLEX_FLOW, SHORTHAND) {}
//
//         Rc<Property> initial() const override {
//             return makeRc<FlexFlowProperty>(self(), Tuple{FlexDirection::ROW, FlexWrap::NOWRAP});
//         }
//
//         Rc<Property> load(ComputedValues const& c) const override {
//             return makeRc<FlexFlowProperty>(self(), Tuple{c.flex->direction, c.flex->wrap});
//         }
//
//         Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
//             if (c.ended())
//                 return Error::invalidData("unexpected end of input");
//
//             Tuple value{FlexDirection::ROW, FlexWrap::NOWRAP};
//
//             auto direction = parseValue<FlexDirection>(c);
//             if (direction) {
//                 value.v0 = direction.expect();
//
//                 auto wrap = parseValue<FlexWrap>(c);
//                 if (wrap)
//                     value.v1 = wrap.expect();
//             } else {
//                 auto wrap = parseValue<FlexWrap>(c);
//                 if (not wrap)
//                     return Error::invalidData("expected flex direction or wrap");
//                 value.v1 = wrap.expect();
//
//                 direction = parseValue<FlexDirection>(c);
//                 if (direction)
//                     value.v0 = direction.expect();
//             }
//
//             return Ok(makeRc<FlexFlowProperty>(self(), value));
//         }
//     };
//
//     Tuple<FlexDirection, FlexWrap> _value;
//
//     FlexFlowProperty(Rc<Property::Registration> registration, Tuple<FlexDirection, FlexWrap> value)
//         : Property(registration), _value(value) {}
//
//     Longhands expand(RegisteredPropertySet& registry, ComputedValues const&, ComputedValues&) const override {
//         return {
//             makeRc<FlexDirectionProperty>(registry.resolveRegistration(Properties::FLEX_DIRECTION, {}).expect(), _value.v0),
//             makeRc<FlexWrapProperty>(registry.resolveRegistration(Properties::FLEX_WRAP, {}).expect(), _value.v1),
//         };
//     }
//
//     void repr(Io::Emit& e) const override {
//         e("{} {}", _value.v0, _value.v1);
//     }
// };

// // https://www.w3.org/TR/css-flexbox-1/#propdef-flex
// export struct FlexProperty : Property {
//     struct Registration : Property::Registration {
//         Registration()
//             : Property::Registration(Properties::FLEX, SHORTHAND) {}
//
//         Rc<Property> initial() const override {
//             return makeRc<FlexProperty>(self(), FlexItemProps{Keywords::AUTO, 0, 1});
//         }
//
//         Rc<Property> load(ComputedValues const& c) const override {
//             return makeRc<FlexProperty>(self(), FlexItemProps{c.flex->basis, c.flex->grow, c.flex->shrink});
//         }
//
//         Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
//             if (c.ended())
//                 return Error::invalidData("unexpected end of input");
//
//             FlexItemProps value{Keywords::AUTO, 0, 1};
//
//             if (c.skip(Css::Token::ident("none"))) {
//                 value = {Keywords::AUTO, 0, 0};
//                 return Ok(makeRc<FlexProperty>(self(), value));
//             } else if (c.skip(Css::Token::ident("initial"))) {
//                 value = {Keywords::AUTO, 0, 1};
//                 return Ok(makeRc<FlexProperty>(self(), value));
//             }
//
//             // default values if these parameters are omitted
//             value.flexGrow = value.flexShrink = 1;
//             value.flexBasis = Calc<PercentOr<Length>>(Length{});
//
//             auto parseGrowShrink = [](Cursor<Css::Sst>& c, FlexItemProps& value) -> Res<> {
//                 auto grow = parseValue<Number>(c);
//                 if (not grow)
//                     return Error::invalidData("expected flex item grow");
//
//                 value.flexGrow = grow.expect();
//
//                 auto shrink = parseValue<Number>(c);
//                 if (shrink)
//                     value.flexShrink = shrink.expect();
//
//                 return Ok();
//             };
//
//             auto parsedGrowAndMaybeShrink = parseGrowShrink(c, value);
//             if (parsedGrowAndMaybeShrink) {
//                 auto basis = parseValue<FlexBasis>(c);
//                 if (basis)
//                     value.flexBasis = basis.expect();
//             } else {
//                 auto basis = parseValue<FlexBasis>(c);
//                 if (basis)
//                     value.flexBasis = basis.expect();
//                 else
//                     return Error::invalidData("expected flex item grow or basis");
//
//                 auto parsedGrowAndMaybeShrink = parseGrowShrink(c, value);
//             }
//             return Ok(makeRc<FlexProperty>(self(), value));
//         }
//     };
//
//     FlexItemProps _value;
//
//     FlexProperty(Rc<Property::Registration> registration, FlexItemProps value)
//         : Property(registration), _value(value) {}
//
//     Longhands expand(RegisteredPropertySet& registry, ComputedValues const&, ComputedValues&) const override {
//         return {
//             makeRc<FlexBasisProperty>(registry.resolveRegistration(Properties::FLEX_BASIS, {}).expect(), _value.flexBasis),
//             makeRc<FlexGrowProperty>(registry.resolveRegistration(Properties::FLEX_GROW, {}).expect(), _value.flexGrow),
//             makeRc<FlexShrinkProperty>(registry.resolveRegistration(Properties::FLEX_SHRINK, {}).expect(), _value.flexShrink),
//         };
//     }
//
//     void repr(Io::Emit& e) const override {
//         e("{} {} {}", _value.flexGrow, _value.flexShrink, _value.flexBasis);
//     }
// };

} // namespace Vaev::Style
