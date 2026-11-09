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
    struct Registration : Property::Registration {
        Registration() : Property::Registration(Properties::PADDING_TOP) {}

        Rc<Property> initial() const override {
            return makeRc<PaddingTopProperty>(self(), Css::px(0));
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<PaddingTopProperty>(self(), toSpecified(c.padding->top));
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<PaddingTopProperty>(self(), try$(parseValue<Css::LengthPercentage>(c))));
        }
    };

    Css::LengthPercentage _value;

    PaddingTopProperty(Rc<Property::Registration> registration, Css::LengthPercentage value)
        : Property(registration), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.padding.cow().top = toComputed(_value, cx);
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

export struct PaddingRightProperty : Property {
    struct Registration : Property::Registration {
        Registration() : Property::Registration(Properties::PADDING_RIGHT) {}

        Rc<Property> initial() const override {
            return makeRc<PaddingRightProperty>(self(), Css::px(0));
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<PaddingRightProperty>(self(), toSpecified(c.padding->end));
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<PaddingRightProperty>(self(), try$(parseValue<Css::LengthPercentage>(c))));
        }
    };

    Css::LengthPercentage _value;

    PaddingRightProperty(Rc<Property::Registration> registration, Css::LengthPercentage value)
        : Property(registration), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.padding.cow().end = toComputed(_value, cx);
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

export struct PaddingBottomProperty : Property {
    struct Registration : Property::Registration {
        Registration() : Property::Registration(Properties::PADDING_BOTTOM) {}

        Rc<Property> initial() const override {
            return makeRc<PaddingBottomProperty>(self(), Css::px(0));
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<PaddingBottomProperty>(self(), toSpecified(c.padding->bottom));
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<PaddingBottomProperty>(self(), try$(parseValue<Css::LengthPercentage>(c))));
        }
    };

    Css::LengthPercentage _value;

    PaddingBottomProperty(Rc<Property::Registration> registration, Css::LengthPercentage value)
        : Property(registration), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.padding.cow().bottom = toComputed(_value, cx);
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

export struct PaddingLeftProperty : Property {
    struct Registration : Property::Registration {
        Registration() : Property::Registration(Properties::PADDING_LEFT) {}

        Rc<Property> initial() const override {
            return makeRc<PaddingLeftProperty>(self(), Css::px(0));
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<PaddingLeftProperty>(self(), toSpecified(c.padding->start));
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<PaddingLeftProperty>(self(), try$(parseValue<Css::LengthPercentage>(c))));
        }
    };

    Css::LengthPercentage _value;

    PaddingLeftProperty(Rc<Property::Registration> registration, Css::LengthPercentage value)
        : Property(registration), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.padding.cow().start = toComputed(_value, cx);
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

export struct PaddingInlineStartProperty : Property {
    struct Registration : Property::Registration {
        Registration() : Property::Registration(Properties::PADDING_INLINE_START, LOGICAL) {}

        Rc<Property> initial() const override {
            return makeRc<PaddingInlineStartProperty>(self(), Css::px(0));
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<PaddingInlineStartProperty>(self(), toSpecified(c.padding->start));
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<PaddingInlineStartProperty>(self(), try$(parseValue<Css::LengthPercentage>(c))));
        }
    };

    Css::LengthPercentage _value;

    PaddingInlineStartProperty(Rc<Property::Registration> registration, Css::LengthPercentage value)
        : Property(registration), _value(value) {}

    // FIXME: Take writing mode into account
    Longhands expand(RegisteredPropertySet& registry, ComputedValues const&, ComputedValues&) const override {
        return {
            makeRc<PaddingLeftProperty>(registry.resolveRegistration(Properties::PADDING_LEFT, {}).expect(), _value),
        };
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

export struct PaddingInlineEndProperty : Property {
    struct Registration : Property::Registration {
        Registration() : Property::Registration(Properties::PADDING_INLINE_END, LOGICAL) {}

        Rc<Property> initial() const override {
            return makeRc<PaddingInlineEndProperty>(self(), Css::px(0));
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<PaddingInlineEndProperty>(self(), toSpecified(c.padding->end));
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<PaddingInlineEndProperty>(self(), try$(parseValue<Css::LengthPercentage>(c))));
        }
    };

    Css::LengthPercentage _value;

    PaddingInlineEndProperty(Rc<Property::Registration> registration, Css::LengthPercentage value)
        : Property(registration), _value(value) {}

    // FIXME: Take writing mode into account
    Longhands expand(RegisteredPropertySet& registry, ComputedValues const&, ComputedValues&) const override {
        return {
            makeRc<PaddingRightProperty>(registry.resolveRegistration(Properties::PADDING_RIGHT, {}).expect(), _value),
        };
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

export struct PaddingProperty : Property {
    using Value = Math::Insets<Css::LengthPercentage>;

    struct Registration : Property::Registration {
        Registration()
            : Property::Registration(Properties::PADDING, SHORTHAND) {}

        Rc<Property> initial() const override {
            return makeRc<PaddingProperty>(self(), Value{Css::px(0)});
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<PaddingProperty>(self(), toSpecified(*c.padding));
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<PaddingProperty>(self(), try$(parseValue<Value>(c))));
        }
    };

    Value _value;

    PaddingProperty(Rc<Property::Registration> registration, Value value)
        : Property(registration), _value(value) {}

    Longhands expand(RegisteredPropertySet& registry, ComputedValues const&, ComputedValues&) const override {
        return {
            makeRc<PaddingTopProperty>(registry.resolveRegistration(Properties::PADDING_TOP, {}).expect(), _value.top),
            makeRc<PaddingRightProperty>(registry.resolveRegistration(Properties::PADDING_RIGHT, {}).expect(), _value.end),
            makeRc<PaddingBottomProperty>(registry.resolveRegistration(Properties::PADDING_BOTTOM, {}).expect(), _value.bottom),
            makeRc<PaddingLeftProperty>(registry.resolveRegistration(Properties::PADDING_LEFT, {}).expect(), _value.start),
        };
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

} // namespace Vaev::Style
