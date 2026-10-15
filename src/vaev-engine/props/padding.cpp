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
            return makeRc<PaddingTopProperty>(self(), Calc<Length, Percent>{Length{}});
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<PaddingTopProperty>(self(), specified<Calc<Length, Percent>>(c.padding->top));
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<PaddingTopProperty>(self(), try$(parseValue<Calc<Length, Percent>>(c))));
        }
    };

    Calc<Length, Percent> _value;

    PaddingTopProperty(Rc<Property::Registration> registration, Calc<Length, Percent> value)
        : Property(registration), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.padding.cow().top = compute(_value, cx);
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
            return makeRc<PaddingRightProperty>(self(), Calc<Length, Percent>{Length{}});
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<PaddingRightProperty>(self(), specified<Calc<Length, Percent>>(c.padding->end));
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<PaddingRightProperty>(self(), try$(parseValue<Calc<Length, Percent>>(c))));
        }
    };

    Calc<Length, Percent> _value;

    PaddingRightProperty(Rc<Property::Registration> registration, Calc<Length, Percent> value)
        : Property(registration), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.padding.cow().end = compute(_value, cx);
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
            return makeRc<PaddingBottomProperty>(self(), Calc<Length, Percent>{Length{}});
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<PaddingBottomProperty>(self(), specified<Calc<Length, Percent>>(c.padding->bottom));
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<PaddingBottomProperty>(self(), try$(parseValue<Calc<Length, Percent>>(c))));
        }
    };

    Calc<Length, Percent> _value;

    PaddingBottomProperty(Rc<Property::Registration> registration, Calc<Length, Percent> value)
        : Property(registration), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.padding.cow().bottom = compute(_value, cx);
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
            return makeRc<PaddingLeftProperty>(self(), Calc<Length, Percent>{Length{}});
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<PaddingLeftProperty>(self(), specified<Calc<Length, Percent>>(c.padding->start));
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<PaddingLeftProperty>(self(), try$(parseValue<Calc<Length, Percent>>(c))));
        }
    };

    Calc<Length, Percent> _value;

    PaddingLeftProperty(Rc<Property::Registration> registration, Calc<Length, Percent> value)
        : Property(registration), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.padding.cow().start = compute(_value, cx);
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
            return makeRc<PaddingInlineStartProperty>(self(), Calc<Length, Percent>{Length{}});
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<PaddingInlineStartProperty>(self(), specified<Calc<Length, Percent>>(c.padding->start));
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<PaddingInlineStartProperty>(self(), try$(parseValue<Calc<Length, Percent>>(c))));
        }
    };

    Calc<Length, Percent> _value;

    PaddingInlineStartProperty(Rc<Property::Registration> registration, Calc<Length, Percent> value)
        : Property(registration), _value(value) {}

    // FIXME: Take writing mode into account
    Vec<Rc<Property>> expand(RegisteredPropertySet& registry, ComputedValues const&, ComputedValues&) const override {
        return {
            makeRc<PaddingLeftProperty>(registry.resolveRegistration(Properties::PADDING_LEFT, {}).expect(), _value),
        };
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

export struct PaddingInlineEndProperty : Property {
    struct Registration : Property::Registration {
        Registration() : Property::Registration(Properties::PADDING_INLINE_END, LOGICAL) {}

        Rc<Property> initial() const override {
            return makeRc<PaddingInlineEndProperty>(self(), Calc<Length, Percent>{Length{}});
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<PaddingInlineEndProperty>(self(), specified<Calc<Length, Percent>>(c.padding->end));
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<PaddingInlineEndProperty>(self(), try$(parseValue<Calc<Length, Percent>>(c))));
        }
    };

    Calc<Length, Percent> _value;

    PaddingInlineEndProperty(Rc<Property::Registration> registration, Calc<Length, Percent> value)
        : Property(registration), _value(value) {}

    // FIXME: Take writing mode into account
    Vec<Rc<Property>> expand(RegisteredPropertySet& registry, ComputedValues const&, ComputedValues&) const override {
        return {
            makeRc<PaddingRightProperty>(registry.resolveRegistration(Properties::PADDING_RIGHT, {}).expect(), _value),
        };
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

export struct PaddingProperty : Property {
    struct Registration : Property::Registration {
        Registration()
            : Property::Registration(Properties::PADDING, SHORTHAND) {}

        Rc<Property> initial() const override {
            return makeRc<PaddingProperty>(self(), Math::Insets<Calc<Length, Percent>>{Length{}});
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<PaddingProperty>(self(), specified<Padding>(*c.padding));
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<PaddingProperty>(self(), try$(parseValue<Math::Insets<Calc<Length, Percent>>>(c))));
        }
    };

    Math::Insets<Calc<Length, Percent>> _value;

    PaddingProperty(Rc<Property::Registration> registration, Math::Insets<Calc<Length, Percent>> value)
        : Property(registration), _value(value) {}

    Vec<Rc<Property>> expand(RegisteredPropertySet& registry, ComputedValues const&, ComputedValues&) const override {
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
