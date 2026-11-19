module;

#include <karm/macros>

export module Vaev.Engine:props.margin;

import Karm.Core;
import :props.base;
import :css.parser;
import :style.computed;
import :style.values.keywords;

using namespace Karm;
using namespace Karm::Literals;

namespace Vaev::Style {

// https://drafts.csswg.org/css-box-4/#propdef-margin-top
export struct MarginTopProperty : Property {
    using Value = Css::Margin;

    struct Registration : Property::Registration {
        Registration() : Property::Registration(Properties::MARGIN_TOP) {}

        InitialValue initialValue() const override {
            return "0"s;
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<MarginTopProperty>(self(), specifyValue<Value>(c.margin->top));
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<MarginTopProperty>(self(), try$(parseValue<Value>(c))));
        }
    };

    Value _value;

    MarginTopProperty(Rc<Property::Registration> registration, Value value)
        : Property(registration), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.margin.cow().top = computeValue(_value, cx);
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return Css::serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://drafts.csswg.org/css-box-4/#propdef-margin-right
export struct MarginRightProperty : Property {
    using Value = MarginTopProperty::Value;

    struct Registration : Property::Registration {
        Registration() : Property::Registration(Properties::MARGIN_RIGHT) {}

        InitialValue initialValue() const override {
            return "0"s;
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<MarginRightProperty>(self(), specifyValue<Value>(c.margin->end));
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<MarginRightProperty>(self(), try$(parseValue<Value>(c))));
        }
    };

    Value _value;

    MarginRightProperty(Rc<Property::Registration> registration, Value value)
        : Property(registration), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.margin.cow().end = computeValue(_value, cx);
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return Css::serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://drafts.csswg.org/css-box-4/#propdef-margin-bottom
export struct MarginBottomProperty : Property {
    using Value = MarginTopProperty::Value;

    struct Registration : Property::Registration {
        Registration() : Property::Registration(Properties::MARGIN_BOTTOM) {}

        InitialValue initialValue() const override {
            return "0"s;
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<MarginBottomProperty>(self(), specifyValue<Value>(c.margin->bottom));
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<MarginBottomProperty>(self(), try$(parseValue<Value>(c))));
        }
    };

    Value _value;

    MarginBottomProperty(Rc<Property::Registration> registration, Value value)
        : Property(registration), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.margin.cow().bottom = computeValue(_value, cx);
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return Css::serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://drafts.csswg.org/css-box-4/#propdef-margin-left
export struct MarginLeftProperty : Property {
    using Value = MarginTopProperty::Value;

    struct Registration : Property::Registration {
        Registration() : Property::Registration(Properties::MARGIN_LEFT) {}

        InitialValue initialValue() const override {
            return "0"s;
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<MarginLeftProperty>(self(), specifyValue<Value>(c.margin->start));
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<MarginLeftProperty>(self(), try$(parseValue<Value>(c))));
        }
    };

    Value _value;

    MarginLeftProperty(Rc<Property::Registration> registration, Value value)
        : Property(registration), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.margin.cow().start = computeValue(_value, cx);
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return Css::serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://drafts.csswg.org/css-box-4/#margin-shorthand
export struct MarginProperty : Property {
    using Value = Math::Insets<MarginTopProperty::Value>;

    struct Registration : Property::Registration {
        Registration()
            : Property::Registration(Properties::MARGIN, SHORTHAND) {}

        InitialValue initialValue() const override {
            return "0"s;
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<MarginProperty>(self(), specifyValue<Value>(*c.margin));
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<MarginProperty>(self(), try$(parseValue<Value>(c))));
        }
    };

    Value _value;

    MarginProperty(Rc<Property::Registration> registration, Value value)
        : Property(registration), _value(value) {}

    Longhands expand(RegisteredPropertySet& registry, ComputedValues const&, ComputedValues&) const override {
        return {
            makeRc<MarginTopProperty>(registry.resolveRegistration(Properties::MARGIN_TOP, {}).expect(), _value.top),
            makeRc<MarginBottomProperty>(registry.resolveRegistration(Properties::MARGIN_BOTTOM, {}).expect(), _value.bottom),
            makeRc<MarginLeftProperty>(registry.resolveRegistration(Properties::MARGIN_LEFT, {}).expect(), _value.start),
            makeRc<MarginRightProperty>(registry.resolveRegistration(Properties::MARGIN_RIGHT, {}).expect(), _value.end),
        };
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://drafts.csswg.org/css-logical/#propdef-margin-inline-start
export struct MarginInlineStartProperty : Property {
    using Value = MarginTopProperty::Value;

    struct Registration : Property::Registration {
        Registration() : Property::Registration(Properties::MARGIN_INLINE_START, LOGICAL) {}

        InitialValue initialValue() const override {
            return "0"s;
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<MarginInlineStartProperty>(self(), specifyValue<Value>(c.margin->start));
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<MarginInlineStartProperty>(self(), try$(parseValue<Value>(c))));
        }
    };

    Value _value;

    MarginInlineStartProperty(Rc<Property::Registration> registration, Value value)
        : Property(registration), _value(value) {}

    // FIXME: Take writing mode into account
    Longhands expand(RegisteredPropertySet& registry, ComputedValues const&, ComputedValues&) const override {
        return {
            makeRc<MarginLeftProperty>(registry.resolveRegistration(Properties::MARGIN_LEFT, {}).expect(), _value),
        };
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return Css::serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://drafts.csswg.org/css-logical/#propdef-margin-inline-end
export struct MarginInlineEndProperty : Property {
    using Value = MarginTopProperty::Value;

    struct Registration : Property::Registration {
        Registration() : Property::Registration(Properties::MARGIN_INLINE_END, LOGICAL) {}

        InitialValue initialValue() const override {
            return "0"s;
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<MarginInlineEndProperty>(self(), specifyValue<Value>(c.margin->end));
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<MarginInlineEndProperty>(self(), try$(parseValue<Value>(c))));
        }
    };

    Value _value;

    MarginInlineEndProperty(Rc<Property::Registration> registration, Value value)
        : Property(registration), _value(value) {}

    // FIXME: Take writing mode into account
    Longhands expand(RegisteredPropertySet& registry, ComputedValues const&, ComputedValues&) const override {
        return {
            makeRc<MarginRightProperty>(registry.resolveRegistration(Properties::MARGIN_RIGHT, {}).expect(), _value),
        };
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return Css::serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://drafts.csswg.org/css-logical/#propdef-margin-inline
export struct MarginInlineProperty : Property {
    using Value = Pair<MarginTopProperty::Value>;

    struct Registration : Property::Registration {
        Registration() : Property::Registration(Properties::MARGIN_INLINE, LOGICAL) {}

        InitialValue initialValue() const override {
            return "0"s;
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<MarginInlineProperty>(self(), Value{specifyValue<MarginTopProperty::Value>(c.margin->start), specifyValue<MarginTopProperty::Value>(c.margin->end)});
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<MarginInlineProperty>(self(), try$(parseValue<Value>(c))));
        }
    };

    Value _value;

    MarginInlineProperty(Rc<Property::Registration> registration, Value value)
        : Property(registration), _value(value) {}

    // FIXME: Take writing mode into account
    Longhands expand(RegisteredPropertySet& registry, ComputedValues const&, ComputedValues&) const override {
        return {
            makeRc<MarginLeftProperty>(registry.resolveRegistration(Properties::MARGIN_LEFT, {}).expect(), _value.v0),
            makeRc<MarginRightProperty>(registry.resolveRegistration(Properties::MARGIN_RIGHT, {}).expect(), _value.v1),
        };
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://drafts.csswg.org/css-logical/#propdef-margin-block-start
export struct MarginBlockStartProperty : Property {
    using Value = MarginTopProperty::Value;

    struct Registration : Property::Registration {
        Registration() : Property::Registration(Properties::MARGIN_BLOCK_START, LOGICAL) {}

        InitialValue initialValue() const override {
            return "0"s;
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<MarginBlockStartProperty>(self(), specifyValue<Value>(c.margin->top));
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<MarginBlockStartProperty>(self(), try$(parseValue<Value>(c))));
        }
    };

    Value _value;

    MarginBlockStartProperty(Rc<Property::Registration> registration, Value value)
        : Property(registration), _value(value) {}

    // FIXME: Take writing mode into account
    Longhands expand(RegisteredPropertySet& registry, ComputedValues const&, ComputedValues&) const override {
        return {
            makeRc<MarginTopProperty>(registry.resolveRegistration(Properties::MARGIN_TOP, {}).expect(), _value),
        };
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return Css::serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://drafts.csswg.org/css-logical/#propdef-margin-block-end
export struct MarginBlockEndProperty : Property {
    using Value = MarginTopProperty::Value;

    struct Registration : Property::Registration {
        Registration() : Property::Registration(Properties::MARGIN_BLOCK_END, LOGICAL) {}

        InitialValue initialValue() const override {
            return "0"s;
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<MarginBlockEndProperty>(self(), specifyValue<Value>(c.margin->bottom));
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<MarginBlockEndProperty>(self(), try$(parseValue<Value>(c))));
        }
    };

    Value _value;

    MarginBlockEndProperty(Rc<Property::Registration> registration, Value value)
        : Property(registration), _value(value) {}

    // FIXME: Take writing mode into account
    Longhands expand(RegisteredPropertySet& registry, ComputedValues const&, ComputedValues&) const override {
        return {
            makeRc<MarginBottomProperty>(registry.resolveRegistration(Properties::MARGIN_BOTTOM, {}).expect(), _value),
        };
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return Css::serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://drafts.csswg.org/css-logical/#propdef-margin-block
export struct MarginBlockProperty : Property {
    using Value = MarginInlineProperty::Value;

    struct Registration : Property::Registration {
        Registration() : Property::Registration(Properties::MARGIN_BLOCK, LOGICAL) {}

        InitialValue initialValue() const override {
            return "0"s;
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<MarginBlockProperty>(self(), Value{specifyValue<MarginTopProperty::Value>(c.margin->top), specifyValue<MarginTopProperty::Value>(c.margin->bottom)});
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<MarginBlockProperty>(self(), try$(parseValue<Value>(c))));
        }
    };

    Value _value;

    MarginBlockProperty(Rc<Property::Registration> registration, Value value)
        : Property(registration), _value(value) {}

    // FIXME: Take writing mode into account
    Longhands expand(RegisteredPropertySet& registry, ComputedValues const&, ComputedValues&) const override {
        return {
            makeRc<MarginTopProperty>(registry.resolveRegistration(Properties::MARGIN_TOP, {}).expect(), _value.v0),
            makeRc<MarginBottomProperty>(registry.resolveRegistration(Properties::MARGIN_BOTTOM, {}).expect(), _value.v1),
        };
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

} // namespace Vaev::Style
