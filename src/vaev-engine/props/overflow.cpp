module;

#include <karm/macros>

export module Vaev.Engine:props.overflow;

import Karm.Core;
import :props.base;
import :css.parser;
import :style.computed;
import :style.values.keywords;

using namespace Karm;
using namespace Karm::Literals;

namespace Vaev::Style {

// MARK: Overflow --------------------------------------------------------------

// https://www.w3.org/TR/css-overflow/#overflow-control
export struct OverflowXProperty : Property {
    struct Registration : Property::Registration {
        Registration() : Property::Registration(Properties::OVERFLOW_X) {}

        InitialValue initial() const override {
            return "visible"s;
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<OverflowXProperty>(self(), c.overflows.x);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<OverflowXProperty>(self(), try$(parseValue<Css::Overflow>(c))));
        }
    };

    Css::Overflow _value;

    OverflowXProperty(Rc<Property::Registration> registration, Css::Overflow value)
        : Property(registration), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.overflows.x = _value;
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return Css::serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/css-overflow/#overflow-control
export struct OverflowYProperty : Property {
    struct Registration : Property::Registration {
        Registration() : Property::Registration(Properties::OVERFLOW_Y) {}

        InitialValue initial() const override {
            return "visible"s;
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<OverflowYProperty>(self(), c.overflows.y);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<OverflowYProperty>(self(), try$(parseValue<Css::Overflow>(c))));
        }
    };

    Css::Overflow _value;

    OverflowYProperty(Rc<Property::Registration> registration, Css::Overflow value)
        : Property(registration), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.overflows.y = _value;
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return Css::serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/css-overflow/#overflow-block
export struct OverflowBlockProperty : Property {
    struct Registration : Property::Registration {
        Registration() : Property::Registration(Properties::OVERFLOW_BLOCK) {}

        InitialValue initial() const override {
            return "visible"s;
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<OverflowBlockProperty>(self(), c.overflows.block);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<OverflowBlockProperty>(self(), try$(parseValue<Css::Overflow>(c))));
        }
    };

    Css::Overflow _value;

    OverflowBlockProperty(Rc<Property::Registration> registration, Css::Overflow value)
        : Property(registration), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.overflows.block = _value;
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return Css::serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/css-overflow/#overflow-inline
export struct OverflowInlineProperty : Property {
    struct Registration : Property::Registration {
        Registration() : Property::Registration(Properties::OVERFLOW_INLINE) {}

        InitialValue initial() const override {
            return "visible"s;
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<OverflowInlineProperty>(self(), c.overflows.inline_);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<OverflowInlineProperty>(self(), try$(parseValue<Css::Overflow>(c))));
        }
    };

    Css::Overflow _value;

    OverflowInlineProperty(Rc<Property::Registration> registration, Css::Overflow value)
        : Property(registration), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.overflows.inline_ = _value;
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return Css::serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/css-overflow-3/#propdef-overflow
export struct OverflowProperty : Property {
    struct Registration : Property::Registration {
        Registration()
            : Property::Registration(Properties::OVERFLOW, SHORTHAND) {}

        InitialValue initial() const override {
            return "visible"s;
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<OverflowProperty>(self(), Pair{c.overflows.x, c.overflows.y});
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            eatWhitespace(c);
            if (c.ended())
                return Error::invalidData("unexpected end of input");

            Pair<Css::Overflow> value{Keywords::VISIBLE, Keywords::VISIBLE};
            value.v0 = try$(parseValue<Css::Overflow>(c));

            eatWhitespace(c);
            if (c.ended()) {
                value.v1 = value.v0;
            } else {
                value.v1 = try$(parseValue<Css::Overflow>(c));
            }

            return Ok(makeRc<OverflowProperty>(self(), value));
        }
    };

    Pair<Css::Overflow> _value;

    OverflowProperty(Rc<Property::Registration> registration, Pair<Css::Overflow> value)
        : Property(registration), _value(value) {}

    Longhands expand(RegisteredPropertySet& registry, ComputedValues const&, ComputedValues&) const override {
        return {
            makeRc<OverflowXProperty>(registry.resolveRegistration(Properties::OVERFLOW_X, {}).expect(), _value.v0),
            makeRc<OverflowYProperty>(registry.resolveRegistration(Properties::OVERFLOW_Y, {}).expect(), _value.v1),
        };
    }

    void repr(Io::Emit& e) const override {
        e("{} {}", _value.v0, _value.v1);
    }
};

} // namespace Vaev::Style
