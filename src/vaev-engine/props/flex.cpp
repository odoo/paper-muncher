module;

#include <karm-core/macros.h>

export module Vaev.Engine:props.flex;

import Karm.Core;
import :props.base;
import :css.parser;
import :style.specified;

using namespace Karm;

namespace Vaev::Style {

// MARK: Flex ------------------------------------------------------------------

// https://www.w3.org/TR/css-flexbox-1/#flex-basis-property
export struct FlexBasisProperty : Property {
    struct Registration : Property::Registration {
        Symbol name() const override {
            return Properties::FLEX_BASIS;
        }

        Rc<Property> initial() const override {
            return makeRc<FlexBasisProperty>(self(), Keywords::AUTO);
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<FlexBasisProperty>(self(), c.flex->basis);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<FlexBasisProperty>(self(), try$(parseValue<FlexBasis>(c))));
        }
    };

    FlexBasis _value;

    FlexBasisProperty(Rc<Property::Registration> registration, FlexBasis value)
        : Property(registration), _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.flex.cow().basis = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/css-flexbox-1/#propdef-flex-direction
export struct FlexDirectionProperty : Property {
    struct Registration : Property::Registration {
        Symbol name() const override {
            return Properties::FLEX_DIRECTION;
        }

        Rc<Property> initial() const override {
            return makeRc<FlexDirectionProperty>(self(), FlexDirection::ROW);
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<FlexDirectionProperty>(self(), c.flex->direction);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<FlexDirectionProperty>(self(), try$(parseValue<FlexDirection>(c))));
        }
    };

    FlexDirection _value;

    FlexDirectionProperty(Rc<Property::Registration> registration, FlexDirection value)
        : Property(registration), _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.flex.cow().direction = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/css-flexbox-1/#flex-grow-property
export struct FlexGrowProperty : Property {
    struct Registration : Property::Registration {
        Symbol name() const override {
            return Properties::FLEX_GROW;
        }

        Rc<Property> initial() const override {
            return makeRc<FlexGrowProperty>(self(), Number{0});
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<FlexGrowProperty>(self(), c.flex->grow);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<FlexGrowProperty>(self(), try$(parseValue<Number>(c))));
        }
    };

    Number _value;

    FlexGrowProperty(Rc<Property::Registration> registration, Number value)
        : Property(registration), _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.flex.cow().grow = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/css-flexbox-1/#propdef-flex-shrink
export struct FlexShrinkProperty : Property {
    struct Registration : Property::Registration {
        Symbol name() const override {
            return Properties::FLEX_SHRINK;
        }

        Rc<Property> initial() const override {
            return makeRc<FlexShrinkProperty>(self(), Number{1});
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<FlexShrinkProperty>(self(), c.flex->shrink);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<FlexShrinkProperty>(self(), try$(parseValue<Number>(c))));
        }
    };

    Number _value;

    FlexShrinkProperty(Rc<Property::Registration> registration, Number value)
        : Property(registration), _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.flex.cow().shrink = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/css-flexbox-1/#propdef-flex-wrap
export struct FlexWrapProperty : Property {
    struct Registration : Property::Registration {
        Symbol name() const override {
            return Properties::FLEX_WRAP;
        }

        Rc<Property> initial() const override {
            return makeRc<FlexWrapProperty>(self(), FlexWrap::NOWRAP);
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<FlexWrapProperty>(self(), c.flex->wrap);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<FlexWrapProperty>(self(), try$(parseValue<FlexWrap>(c))));
        }
    };

    FlexWrap _value;

    FlexWrapProperty(Rc<Property::Registration> registration, FlexWrap value)
        : Property(registration), _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.flex.cow().wrap = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/css-flexbox-1/#propdef-flex-flow
export struct FlexFlowProperty : Property {
    struct Registration : Property::Registration {
        Symbol name() const override {
            return Properties::FLEX_FLOW;
        }

        Flags<Options> flags() const override {
            return {SHORTHAND_PROPERTY};
        }

        Rc<Property> initial() const override {
            return makeRc<FlexFlowProperty>(self(), Tuple{FlexDirection::ROW, FlexWrap::NOWRAP});
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<FlexFlowProperty>(self(), Tuple{c.flex->direction, c.flex->wrap});
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            if (c.ended())
                return Error::invalidData("unexpected end of input");

            Tuple value{FlexDirection::ROW, FlexWrap::NOWRAP};

            auto direction = parseValue<FlexDirection>(c);
            if (direction) {
                value.v0 = direction.unwrap();

                auto wrap = parseValue<FlexWrap>(c);
                if (wrap)
                    value.v1 = wrap.unwrap();
            } else {
                auto wrap = parseValue<FlexWrap>(c);
                if (not wrap)
                    return Error::invalidData("expected flex direction or wrap");
                value.v1 = wrap.unwrap();

                direction = parseValue<FlexDirection>(c);
                if (direction)
                    value.v0 = direction.unwrap();
            }

            return Ok(makeRc<FlexFlowProperty>(self(), value));
        }
    };

    Tuple<FlexDirection, FlexWrap> _value;

    FlexFlowProperty(Rc<Property::Registration> registration, Tuple<FlexDirection, FlexWrap> value)
        : Property(registration), _value(value) {}

    Vec<Rc<Property>> expandShorthand(PropertyRegistry& registry, SpecifiedValues const&, SpecifiedValues&) const override {
        return {
            makeRc<FlexDirectionProperty>(registry.resolveRegistration(Properties::FLEX_DIRECTION, {}).unwrap(), _value.v0),
            makeRc<FlexWrapProperty>(registry.resolveRegistration(Properties::FLEX_WRAP, {}).unwrap(), _value.v1),
        };
    }

    void repr(Io::Emit& e) const override {
        e("{} {}", _value.v0, _value.v1);
    }
};

// https://www.w3.org/TR/css-flexbox-1/#propdef-flex
export struct FlexProperty : Property {
    struct Registration : Property::Registration {
        Symbol name() const override {
            return Properties::FLEX;
        }

        Flags<Options> flags() const override {
            return {SHORTHAND_PROPERTY};
        }

        Rc<Property> initial() const override {
            return makeRc<FlexProperty>(self(), FlexItemProps{Keywords::AUTO, 0, 1});
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<FlexProperty>(self(), FlexItemProps{c.flex->basis, c.flex->grow, c.flex->shrink});
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            if (c.ended())
                return Error::invalidData("unexpected end of input");

            FlexItemProps value{Keywords::AUTO, 0, 1};

            if (c.skip(Css::Token::ident("none"))) {
                value = {Keywords::AUTO, 0, 0};
                return Ok(makeRc<FlexProperty>(self(), value));
            } else if (c.skip(Css::Token::ident("initial"))) {
                value = {Keywords::AUTO, 0, 1};
                return Ok(makeRc<FlexProperty>(self(), value));
            }

            // default values if these parameters are omitted
            value.flexGrow = value.flexShrink = 1;
            value.flexBasis = CalcValue<PercentOr<Length>>(Length{});

            auto parseGrowShrink = [](Cursor<Css::Sst>& c, FlexItemProps& value) -> Res<> {
                auto grow = parseValue<Number>(c);
                if (not grow)
                    return Error::invalidData("expected flex item grow");

                value.flexGrow = grow.unwrap();

                auto shrink = parseValue<Number>(c);
                if (shrink)
                    value.flexShrink = shrink.unwrap();

                return Ok();
            };

            auto parsedGrowAndMaybeShrink = parseGrowShrink(c, value);
            if (parsedGrowAndMaybeShrink) {
                auto basis = parseValue<FlexBasis>(c);
                if (basis)
                    value.flexBasis = basis.unwrap();
            } else {
                auto basis = parseValue<FlexBasis>(c);
                if (basis)
                    value.flexBasis = basis.unwrap();
                else
                    return Error::invalidData("expected flex item grow or basis");

                auto parsedGrowAndMaybeShrink = parseGrowShrink(c, value);
            }
            return Ok(makeRc<FlexProperty>(self(), value));
        }
    };

    FlexItemProps _value;

    FlexProperty(Rc<Property::Registration> registration, FlexItemProps value)
        : Property(registration), _value(value) {}

    Vec<Rc<Property>> expandShorthand(PropertyRegistry& registry, SpecifiedValues const&, SpecifiedValues&) const override {
        return {
            makeRc<FlexBasisProperty>(registry.resolveRegistration(Properties::FLEX_BASIS, {}).unwrap(), _value.flexBasis),
            makeRc<FlexGrowProperty>(registry.resolveRegistration(Properties::FLEX_GROW, {}).unwrap(), _value.flexGrow),
            makeRc<FlexShrinkProperty>(registry.resolveRegistration(Properties::FLEX_SHRINK, {}).unwrap(), _value.flexShrink),
        };
    }

    void repr(Io::Emit& e) const override {
        e("{} {} {}", _value.flexGrow, _value.flexShrink, _value.flexBasis);
    }
};

} // namespace Vaev::Style
