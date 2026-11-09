module;

#include <karm/macros>

export module Vaev.Engine:props.flex;

import Karm.Core;
import :props.base;
import :css.parser;
import :style.computed;
import :style.values.keywords;

using namespace Karm;
using namespace Karm::Literals;

namespace Vaev::Style {

// MARK: Flex ------------------------------------------------------------------

// https://www.w3.org/TR/css-flexbox-1/#flex-basis-property
export struct FlexBasisProperty : Property {
    using Value = Union<
        Keywords::Auto,
        Keywords::Content,
        Keywords::MinContent,
        Keywords::MaxContent,
        Keywords::FitContent,
        Css::LengthPercentage>;

    struct Registration : Property::Registration {
        Registration() : Property::Registration(Properties::FLEX_BASIS) {}

        InitialValue initialValue() const override {
            return "auto"s;
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<FlexBasisProperty>(self(), toSpecified(c.flex->basis));
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<FlexBasisProperty>(self(), try$(parseValue<Value>(c))));
        }
    };

    Value _value;

    FlexBasisProperty(Rc<Property::Registration> registration, Value value)
        : Property(registration), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.flex.cow().basis = toComputed(_value, cx);
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return Css::serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/css-flexbox-1/#propdef-flex-direction
export struct FlexDirectionProperty : Property {
    using Value = Css::FlexDirection;

    struct Registration : Property::Registration {
        Registration() : Property::Registration(Properties::FLEX_DIRECTION) {}

        InitialValue initialValue() const override {
            return "row"s;
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<FlexDirectionProperty>(self(), c.flex->direction);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<FlexDirectionProperty>(self(), try$(parseValue<Value>(c))));
        }
    };

    Value _value;

    FlexDirectionProperty(Rc<Property::Registration> registration, Value value)
        : Property(registration), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.flex.cow().direction = _value;
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return Css::serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/css-flexbox-1/#flex-grow-property
export struct FlexGrowProperty : Property {
    using Value = Css::Number;

    struct Registration : Property::Registration {
        Registration() : Property::Registration(Properties::FLEX_GROW) {}

        InitialValue initialValue() const override {
            return "0"s;
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<FlexGrowProperty>(self(), toSpecified(c.flex->grow));
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<FlexGrowProperty>(self(), try$(parseValue<Value>(c))));
        }
    };

    Value _value;

    FlexGrowProperty(Rc<Property::Registration> registration, Value value)
        : Property(registration), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.flex.cow().grow = toComputed(_value);
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return Css::serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/css-flexbox-1/#propdef-flex-shrink
export struct FlexShrinkProperty : Property {
    using Value = Css::Number;

    struct Registration : Property::Registration {
        Registration() : Property::Registration(Properties::FLEX_SHRINK) {}

        InitialValue initialValue() const override {
            return "1"s;
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<FlexShrinkProperty>(self(), toSpecified(c.flex->shrink));
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<FlexShrinkProperty>(self(), try$(parseValue<Value>(c))));
        }
    };

    Value _value;

    FlexShrinkProperty(Rc<Property::Registration> registration, Value value)
        : Property(registration), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.flex.cow().shrink = toComputed(_value);
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return Css::serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/css-flexbox-1/#propdef-flex-wrap
export struct FlexWrapProperty : Property {
    using Value = Css::FlexWrap;

    struct Registration : Property::Registration {
        Registration() : Property::Registration(Properties::FLEX_WRAP) {}

        InitialValue initialValue() const override {
            return "nowrap"s;
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<FlexWrapProperty>(self(), c.flex->wrap);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<FlexWrapProperty>(self(), try$(parseValue<Value>(c))));
        }
    };

    Value _value;

    FlexWrapProperty(Rc<Property::Registration> registration, Value value)
        : Property(registration), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.flex.cow().wrap = _value;
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return Css::serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/css-flexbox-1/#propdef-flex-flow
export struct FlexFlowProperty : Property {
    using Value = Tuple<Css::FlexDirection, Css::FlexWrap>;

    struct Registration : Property::Registration {
        Registration()
            : Property::Registration(Properties::FLEX_FLOW, SHORTHAND) {}

        InitialValue initialValue() const override {
            return "row nowrap"s;
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<FlexFlowProperty>(self(), Tuple{c.flex->direction, c.flex->wrap});
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            if (c.ended())
                return Error::invalidData("unexpected end of input");

            auto value = Value{Keywords::ROW, Keywords::NOWRAP};

            auto direction = parseValue<Css::FlexDirection>(c);
            if (direction) {
                value.v0 = direction.expect();

                auto wrap = parseValue<Css::FlexWrap>(c);
                if (wrap)
                    value.v1 = wrap.expect();
            } else {
                auto wrap = parseValue<Css::FlexWrap>(c);
                if (not wrap)
                    return Error::invalidData("expected flex direction or wrap");
                value.v1 = wrap.expect();

                direction = parseValue<Css::FlexDirection>(c);
                if (direction)
                    value.v0 = direction.expect();
            }

            return Ok(makeRc<FlexFlowProperty>(self(), value));
        }
    };

    Value _value;

    FlexFlowProperty(Rc<Property::Registration> registration, Value value)
        : Property(registration), _value(value) {}

    Longhands expand(RegisteredPropertySet& registry, ComputedValues const&, ComputedValues&) const override {
        return {
            makeRc<FlexDirectionProperty>(registry.resolveRegistration(Properties::FLEX_DIRECTION, {}).expect(), _value.v0),
            makeRc<FlexWrapProperty>(registry.resolveRegistration(Properties::FLEX_WRAP, {}).expect(), _value.v1),
        };
    }

    void repr(Io::Emit& e) const override {
        e("{} {}", _value.v0, _value.v1);
    }
};

// https://www.w3.org/TR/css-flexbox-1/#propdef-flex
export struct FlexProperty : Property {
    struct Value {
        FlexBasisProperty::Value flexBasis = Keywords::AUTO;
        Css::Number flexGrow, flexShrink;
    };

    struct Registration : Property::Registration {
        Registration()
            : Property::Registration(Properties::FLEX, SHORTHAND) {}

        InitialValue initialValue() const override {
            return "0 1 auto"s;
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<FlexProperty>(
                self(),
                Value{
                    toSpecified(c.flex->basis),
                    toSpecified(c.flex->grow),
                    toSpecified(c.flex->shrink),
            });
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            if (c.ended())
                return Error::invalidData("unexpected end of input");

            Value value{Keywords::AUTO, Css::number(0), Css::number(1)};

            if (c.skip(Css::Token::ident("none"))) {
                value = {Keywords::AUTO, Css::number(0), Css::number(0)};
                return Ok(makeRc<FlexProperty>(self(), value));
            } else if (c.skip(Css::Token::ident("initial"))) {
                value = {Keywords::AUTO, Css::number(0), Css::number(1)};
                return Ok(makeRc<FlexProperty>(self(), value));
            }

            // default values if these parameters are omitted
            value.flexGrow = value.flexShrink = Css::number(1);
            value.flexBasis = Css::LengthPercentage{Css::px(0)};

            auto parseGrowShrink = [](Cursor<Css::Sst>& c, Value& value) -> Res<> {
                auto grow = parseValue<Css::Number>(c);
                if (not grow)
                    return Error::invalidData("expected flex item grow");

                value.flexGrow = grow.expect();

                auto shrink = parseValue<Css::Number>(c);
                if (shrink)
                    value.flexShrink = shrink.expect();

                return Ok();
            };

            auto parsedGrowAndMaybeShrink = parseGrowShrink(c, value);
            if (parsedGrowAndMaybeShrink) {
                auto basis = parseValue<FlexBasisProperty::Value>(c);
                if (basis)
                    value.flexBasis = basis.expect();
            } else {
                auto basis = parseValue<FlexBasisProperty::Value>(c);
                if (basis)
                    value.flexBasis = basis.expect();
                else
                    return Error::invalidData("expected flex item grow or basis");

                auto parsedGrowAndMaybeShrink = parseGrowShrink(c, value);
            }
            return Ok(makeRc<FlexProperty>(self(), value));
        }
    };

    Value _value;

    FlexProperty(Rc<Property::Registration> registration, Value value)
        : Property(registration), _value(value) {}

    Longhands expand(RegisteredPropertySet& registry, ComputedValues const&, ComputedValues&) const override {
        return {
            makeRc<FlexBasisProperty>(registry.resolveRegistration(Properties::FLEX_BASIS, {}).expect(), _value.flexBasis),
            makeRc<FlexGrowProperty>(registry.resolveRegistration(Properties::FLEX_GROW, {}).expect(), _value.flexGrow),
            makeRc<FlexShrinkProperty>(registry.resolveRegistration(Properties::FLEX_SHRINK, {}).expect(), _value.flexShrink),
        };
    }

    void repr(Io::Emit& e) const override {
        e("{} {} {}", _value.flexGrow, _value.flexShrink, _value.flexBasis);
    }
};

} // namespace Vaev::Style
