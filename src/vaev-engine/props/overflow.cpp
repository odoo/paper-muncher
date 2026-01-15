module;

#include <karm-core/macros.h>

export module Vaev.Engine:props.overflow;

import Karm.Core;
import :props.base;
import :css.parser;
import :style.specified;

using namespace Karm;

namespace Vaev::Style {

// MARK: Overflow --------------------------------------------------------------

// https://www.w3.org/TR/css-overflow/#overflow-control
export struct OverflowXProperty : Property {
    struct Registration : Property::Registration {
        Symbol name() const override {
            return Properties::OVERFLOW_X;
        }

        Rc<Property> initial() const override {
            return makeRc<OverflowXProperty>(self(), Overflow::VISIBLE);
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<OverflowXProperty>(self(), c.overflows.x);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<OverflowXProperty>(self(), try$(parseValue<Overflow>(c))));
        }
    };

    Overflow _value;

    OverflowXProperty(Rc<Property::Registration> registration, Overflow value)
        : Property(registration), _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.overflows.x = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/css-overflow/#overflow-control
export struct OverflowYProperty : Property {
    struct Registration : Property::Registration {
        Symbol name() const override {
            return Properties::OVERFLOW_Y;
        }

        Rc<Property> initial() const override {
            return makeRc<OverflowYProperty>(self(), Overflow::VISIBLE);
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<OverflowYProperty>(self(), c.overflows.y);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<OverflowYProperty>(self(), try$(parseValue<Overflow>(c))));
        }
    };

    Overflow _value;

    OverflowYProperty(Rc<Property::Registration> registration, Overflow value)
        : Property(registration), _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.overflows.y = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/css-overflow/#overflow-block
export struct OverflowBlockProperty : Property {
    struct Registration : Property::Registration {
        Symbol name() const override {
            return Properties::OVERFLOW_BLOCK;
        }

        Rc<Property> initial() const override {
            return makeRc<OverflowBlockProperty>(self(), Overflow::VISIBLE);
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<OverflowBlockProperty>(self(), c.overflows.block);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<OverflowBlockProperty>(self(), try$(parseValue<Overflow>(c))));
        }
    };

    Overflow _value;

    OverflowBlockProperty(Rc<Property::Registration> registration, Overflow value)
        : Property(registration), _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.overflows.block = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/css-overflow/#overflow-inline
export struct OverflowInlineProperty : Property {
    struct Registration : Property::Registration {
        Symbol name() const override {
            return Properties::OVERFLOW_INLINE;
        }

        Rc<Property> initial() const override {
            return makeRc<OverflowInlineProperty>(self(), Overflow::VISIBLE);
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<OverflowInlineProperty>(self(), c.overflows.inline_);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<OverflowInlineProperty>(self(), try$(parseValue<Overflow>(c))));
        }
    };

    Overflow _value;

    OverflowInlineProperty(Rc<Property::Registration> registration, Overflow value)
        : Property(registration), _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.overflows.inline_ = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/css-overflow-3/#propdef-overflow
export struct OverflowProperty : Property {
    struct Registration : Property::Registration {
        Symbol name() const override {
            return Properties::OVERFLOW;
        }

        Flags<Options> flags() const override {
            return {SHORTHAND_PROPERTY};
        }

        Rc<Property> initial() const override {
            return makeRc<OverflowProperty>(self(), Pair{Overflow::VISIBLE, Overflow::VISIBLE});
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<OverflowProperty>(self(), Pair{c.overflows.x, c.overflows.y});
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            eatWhitespace(c);
            if (c.ended())
                return Error::invalidData("unexpected end of input");

            Pair<Overflow> value;
            value.v0 = try$(parseValue<Overflow>(c));

            eatWhitespace(c);
            if (c.ended()) {
                value.v1 = value.v0;
            } else {
                value.v1 = try$(parseValue<Overflow>(c));
            }

            return Ok(makeRc<OverflowProperty>(self(), value));
        }
    };

    Pair<Overflow> _value;

    OverflowProperty(Rc<Property::Registration> registration, Pair<Overflow> value)
        : Property(registration), _value(value) {}

    Vec<Rc<Property>> expandShorthand(PropertyRegistry& registry, SpecifiedValues const&, SpecifiedValues&) const override {
        return {
            makeRc<OverflowXProperty>(registry.resolveRegistration(Properties::OVERFLOW_X, {}).unwrap(), _value.v0),
            makeRc<OverflowYProperty>(registry.resolveRegistration(Properties::OVERFLOW_Y, {}).unwrap(), _value.v1),
        };
    }

    void repr(Io::Emit& e) const override {
        e("{} {}", _value.v0, _value.v1);
    }
};

} // namespace Vaev::Style
