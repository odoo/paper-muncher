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
            return makeRc<PaddingTopProperty>(self(), Calc<PercentOr<Length>>{Length{}});
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<PaddingTopProperty>(self(), c.padding->top);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<PaddingTopProperty>(self(), try$(parseValue<Calc<PercentOr<Length>>>(c))));
        }
    };

    Calc<PercentOr<Length>> _value;

    PaddingTopProperty(Rc<Property::Registration> registration, Calc<PercentOr<Length>> value)
        : Property(registration), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.padding.cow().top = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

export struct PaddingRightProperty : Property {
    struct Registration : Property::Registration {
        Registration() : Property::Registration(Properties::PADDING_RIGHT) {}

        Rc<Property> initial() const override {
            return makeRc<PaddingRightProperty>(self(), Calc<PercentOr<Length>>{Length{}});
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<PaddingRightProperty>(self(), c.padding->end);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<PaddingRightProperty>(self(), try$(parseValue<Calc<PercentOr<Length>>>(c))));
        }
    };

    Calc<PercentOr<Length>> _value;

    PaddingRightProperty(Rc<Property::Registration> registration, Calc<PercentOr<Length>> value)
        : Property(registration), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.padding.cow().end = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

export struct PaddingBottomProperty : Property {
    struct Registration : Property::Registration {
        Registration() : Property::Registration(Properties::PADDING_BOTTOM) {}

        Rc<Property> initial() const override {
            return makeRc<PaddingBottomProperty>(self(), Calc<PercentOr<Length>>{Length{}});
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<PaddingBottomProperty>(self(), c.padding->bottom);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<PaddingBottomProperty>(self(), try$(parseValue<Calc<PercentOr<Length>>>(c))));
        }
    };

    Calc<PercentOr<Length>> _value;

    PaddingBottomProperty(Rc<Property::Registration> registration, Calc<PercentOr<Length>> value)
        : Property(registration), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.padding.cow().bottom = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

export struct PaddingLeftProperty : Property {
    struct Registration : Property::Registration {
        Registration() : Property::Registration(Properties::PADDING_LEFT) {}

        Rc<Property> initial() const override {
            return makeRc<PaddingLeftProperty>(self(), Calc<PercentOr<Length>>{Length{}});
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<PaddingLeftProperty>(self(), c.padding->start);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<PaddingLeftProperty>(self(), try$(parseValue<Calc<PercentOr<Length>>>(c))));
        }
    };

    Calc<PercentOr<Length>> _value;

    PaddingLeftProperty(Rc<Property::Registration> registration, Calc<PercentOr<Length>> value)
        : Property(registration), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.padding.cow().start = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

export struct PaddingInlineStartProperty : Property {
    struct Registration : Property::Registration {
        Registration() : Property::Registration(Properties::PADDING_INLINE_START) {}

        Rc<Property> initial() const override {
            return makeRc<PaddingInlineStartProperty>(self(), Calc<PercentOr<Length>>{Length{}});
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<PaddingInlineStartProperty>(self(), c.padding->start);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<PaddingInlineStartProperty>(self(), try$(parseValue<Calc<PercentOr<Length>>>(c))));
        }
    };

    Calc<PercentOr<Length>> _value;

    PaddingInlineStartProperty(Rc<Property::Registration> registration, Calc<PercentOr<Length>> value)
        : Property(registration), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.padding.cow().start = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

export struct PaddingInlineEndProperty : Property {
    struct Registration : Property::Registration {
        Registration() : Property::Registration(Properties::PADDING_INLINE_END) {}

        Rc<Property> initial() const override {
            return makeRc<PaddingInlineEndProperty>(self(), Calc<PercentOr<Length>>{Length{}});
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<PaddingInlineEndProperty>(self(), c.padding->end);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<PaddingInlineEndProperty>(self(), try$(parseValue<Calc<PercentOr<Length>>>(c))));
        }
    };

    Calc<PercentOr<Length>> _value;

    PaddingInlineEndProperty(Rc<Property::Registration> registration, Calc<PercentOr<Length>> value)
        : Property(registration), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.padding.cow().end = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

export struct PaddingProperty : Property {
    struct Registration : Property::Registration {
        Registration() : Property::Registration(Properties::PADDING) {}

        Flags<Options> flags() const override {
            return {SHORTHAND_PROPERTY};
        }

        Rc<Property> initial() const override {
            return makeRc<PaddingProperty>(self(), Math::Insets<Calc<PercentOr<Length>>>{Length{}});
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<PaddingProperty>(self(), *c.padding);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<PaddingProperty>(self(), try$(parseValue<Math::Insets<Calc<PercentOr<Length>>>>(c))));
        }
    };

    Math::Insets<Calc<PercentOr<Length>>> _value;

    PaddingProperty(Rc<Property::Registration> registration, Math::Insets<Calc<PercentOr<Length>>> value)
        : Property(registration), _value(value) {}

    Vec<Rc<Property>> expandShorthand(RegisteredPropertySet& registry, ComputedValues const&, ComputedValues&) const override {
        return {
            makeRc<PaddingTopProperty>(registry.resolveRegistration(Properties::PADDING_TOP, {}).unwrap(), _value.top),
            makeRc<PaddingRightProperty>(registry.resolveRegistration(Properties::PADDING_RIGHT, {}).unwrap(), _value.end),
            makeRc<PaddingBottomProperty>(registry.resolveRegistration(Properties::PADDING_BOTTOM, {}).unwrap(), _value.bottom),
            makeRc<PaddingLeftProperty>(registry.resolveRegistration(Properties::PADDING_LEFT, {}).unwrap(), _value.start),
        };
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

} // namespace Vaev::Style
