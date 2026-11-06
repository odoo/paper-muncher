module;

#include <karm/macros>

export module Vaev.Engine:props.positioned;

import Karm.Core;
import :props.base;
import :css.parser;
import :style.computed;

using namespace Karm;

namespace Vaev::Style {

// MARK: Positioning -----------------------------------------------------------

// https://www.w3.org/TR/CSS22/visuren.html#positioning-scheme
export struct PositionProperty : Property {
    struct Registration : Property::Registration {
        Registration() : Property::Registration(Properties::POSITION) {}

        Rc<Property> initial() const override {
            return makeRc<PositionProperty>(self(), Css::Position{Keywords::STATIC});
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<PositionProperty>(self(), c.position);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<PositionProperty>(self(), try$(parseValue<Css::Position>(c))));
        }
    };

    Css::Position _value;

    PositionProperty(Rc<Property::Registration> registration, Css::Position value)
        : Property(registration), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.position = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/CSS22/visuren.html#propdef-top
export struct TopProperty : Property {
    using Value = Union<Keywords::Auto, Css::LengthPercentage>;

    struct Registration : Property::Registration {
        Registration() : Property::Registration(Properties::TOP) {}

        Rc<Property> initial() const override {
            return makeRc<TopProperty>(self(), Value{Keywords::AUTO});
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<TopProperty>(self(), toSpecified(c.insets->top));
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<TopProperty>(self(), try$(parseValue<Value>(c))));
        }
    };

    Value _value;

    TopProperty(Rc<Property::Registration> registration, Value value)
        : Property(registration), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.insets.cow().top = toComputed(_value, cx);
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/CSS22/visuren.html#propdef-right
export struct RightProperty : Property {
    using Value = TopProperty::Value;

    struct Registration : Property::Registration {
        Registration() : Property::Registration(Properties::RIGHT) {}

        Rc<Property> initial() const override {
            return makeRc<RightProperty>(self(), Value{Keywords::AUTO});
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<RightProperty>(self(), toSpecified(c.insets->end));
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<RightProperty>(self(), try$(parseValue<Value>(c))));
        }
    };

    Value _value;

    RightProperty(Rc<Property::Registration> registration, Value value)
        : Property(registration), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.insets.cow().end = toComputed(_value, cx);
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/CSS22/visuren.html#propdef-bottom
export struct BottomProperty : Property {
    using Value = TopProperty::Value;

    struct Registration : Property::Registration {
        Registration() : Property::Registration(Properties::BOTTOM) {}

        Rc<Property> initial() const override {
            return makeRc<BottomProperty>(self(), Value{Keywords::AUTO});
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<BottomProperty>(self(), toSpecified(c.insets->bottom));
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<BottomProperty>(self(), try$(parseValue<Value>(c))));
        }
    };

    Value _value;

    BottomProperty(Rc<Property::Registration> registration, Value value)
        : Property(registration), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.insets.cow().bottom = toComputed(_value, cx);
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/CSS22/visuren.html#propdef-left
export struct LeftProperty : Property {
    using Value = TopProperty::Value;

    struct Registration : Property::Registration {
        Registration() : Property::Registration(Properties::LEFT) {}

        Rc<Property> initial() const override {
            return makeRc<LeftProperty>(self(), Value{Keywords::AUTO});
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<LeftProperty>(self(), toSpecified(c.insets->start));
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<LeftProperty>(self(), try$(parseValue<Value>(c))));
        }
    };

    Value _value;

    LeftProperty(Rc<Property::Registration> registration, Value value)
        : Property(registration), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.insets.cow().start = toComputed(_value, cx);
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://drafts.csswg.org/css2/#z-index
export struct ZIndexProperty : Property {
    struct Registration : Property::Registration {
        Registration() : Property::Registration(Properties::Z_INDEX) {}

        Rc<Property> initial() const override {
            return makeRc<ZIndexProperty>(self(), Css::ZIndex{Keywords::AUTO});
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<ZIndexProperty>(self(), c.zIndex);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<ZIndexProperty>(self(), try$(parseValue<Css::ZIndex>(c))));
        }
    };

    Css::ZIndex _value;

    ZIndexProperty(Rc<Property::Registration> registration, Css::ZIndex value)
        : Property(registration), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.zIndex = _value;
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return Css::serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

} // namespace Vaev::Style
