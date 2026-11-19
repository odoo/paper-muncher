module;

#include <karm/macros>

export module Vaev.Engine:props.positioned;

import Karm.Core;
import :props.base;
import :css.parser;
import :style.computed;
import :style.values.keywords;
import :style.values.primitives;

using namespace Karm;
using namespace Karm::Literals;

namespace Vaev::Style {

// MARK: Positioning -----------------------------------------------------------

// https://www.w3.org/TR/CSS22/visuren.html#positioning-scheme
export struct PositionProperty : Property {
    using Value = Css::PositionScheme;

    struct Registration : Property::Registration {
        Registration() : Property::Registration(Properties::POSITION) {}

        InitialValue initialValue() const override {
            return "static"s;
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<PositionProperty>(self(), c.position);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<PositionProperty>(self(), try$(parseValue<Value>(c))));
        }
    };

    Value _value;

    PositionProperty(Rc<Property::Registration> registration, Value value)
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
    using Value = Css::BoxInset;

    struct Registration : Property::Registration {
        Registration() : Property::Registration(Properties::TOP) {}

        InitialValue initialValue() const override {
            return "auto"s;
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<TopProperty>(self(), specifyValue<Value>(c.insets->top));
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<TopProperty>(self(), try$(parseValue<Value>(c))));
        }
    };

    Value _value;

    TopProperty(Rc<Property::Registration> registration, Value value)
        : Property(registration), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.insets.cow().top = computeValue(_value, cx);
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return Css::serializeValue(w, _value);
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

        InitialValue initialValue() const override {
            return "auto"s;
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<RightProperty>(self(), specifyValue<Value>(c.insets->end));
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<RightProperty>(self(), try$(parseValue<Value>(c))));
        }
    };

    Value _value;

    RightProperty(Rc<Property::Registration> registration, Value value)
        : Property(registration), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.insets.cow().end = computeValue(_value, cx);
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return Css::serializeValue(w, _value);
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

        InitialValue initialValue() const override {
            return "auto"s;
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<BottomProperty>(self(), specifyValue<Value>(c.insets->bottom));
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<BottomProperty>(self(), try$(parseValue<Value>(c))));
        }
    };

    Value _value;

    BottomProperty(Rc<Property::Registration> registration, Value value)
        : Property(registration), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.insets.cow().bottom = computeValue(_value, cx);
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return Css::serializeValue(w, _value);
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

        InitialValue initialValue() const override {
            return "auto"s;
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<LeftProperty>(self(), specifyValue<Value>(c.insets->start));
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<LeftProperty>(self(), try$(parseValue<Value>(c))));
        }
    };

    Value _value;

    LeftProperty(Rc<Property::Registration> registration, Value value)
        : Property(registration), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.insets.cow().start = computeValue(_value, cx);
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return Css::serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://drafts.csswg.org/css2/#z-index
export struct ZIndexProperty : Property {
    using Value = Css::ZIndex;

    struct Registration : Property::Registration {
        Registration() : Property::Registration(Properties::Z_INDEX) {}

        InitialValue initialValue() const override {
            return "auto"s;
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<ZIndexProperty>(self(), c.zIndex);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<ZIndexProperty>(self(), try$(parseValue<Value>(c))));
        }
    };

    Value _value;

    ZIndexProperty(Rc<Property::Registration> registration, Value value)
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
