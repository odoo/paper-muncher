module;

#include <karm/macros>

export module Vaev.Engine:props.margin;

import Karm.Core;
import :props.base;
import :css.parser;
import :style.computed;

using namespace Karm;

namespace Vaev::Style {

// https://drafts.csswg.org/css-box-4/#propdef-margin-top
export struct MarginTopProperty : Property {
    using Value = Union<Keywords::Auto, Calc<Length, Percent>>;

    struct Registration : Property::Registration {
        Symbol name() const override {
            return Properties::MARGIN_TOP;
        }

        Rc<Property> initial() const override {
            return makeRc<MarginTopProperty>(self(), Calc<Length, Percent>(Length{}));
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<MarginTopProperty>(self(), c.margin->top);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<MarginTopProperty>(self(), try$(parseValue<Value>(c))));
        }
    };

    Value _value;

    MarginTopProperty(Rc<Property::Registration> registration, Value value)
        : Property(registration), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.margin.cow().top = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://drafts.csswg.org/css-box-4/#propdef-margin-right
export struct MarginRightProperty : Property {
    using Value = MarginTopProperty::Value;

    struct Registration : Property::Registration {
        Symbol name() const override {
            return Properties::MARGIN_RIGHT;
        }

        Rc<Property> initial() const override {
            return makeRc<MarginRightProperty>(self(), Calc<Length, Percent>(Length{}));
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<MarginRightProperty>(self(), c.margin->end);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<MarginRightProperty>(self(), try$(parseValue<Value>(c))));
        }
    };

    Value _value;

    MarginRightProperty(Rc<Property::Registration> registration, Value value)
        : Property(registration), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.margin.cow().end = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://drafts.csswg.org/css-box-4/#propdef-margin-bottom
export struct MarginBottomProperty : Property {
    using Value = MarginTopProperty::Value;

    struct Registration : Property::Registration {
        Symbol name() const override {
            return Properties::MARGIN_BOTTOM;
        }

        Rc<Property> initial() const override {
            return makeRc<MarginBottomProperty>(self(), Calc<Length, Percent>(Length{}));
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<MarginBottomProperty>(self(), c.margin->bottom);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<MarginBottomProperty>(self(), try$(parseValue<Value>(c))));
        }
    };

    Value _value;

    MarginBottomProperty(Rc<Property::Registration> registration, Value value)
        : Property(registration), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.margin.cow().bottom = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://drafts.csswg.org/css-box-4/#propdef-margin-left
export struct MarginLeftProperty : Property {
    using Value = MarginTopProperty::Value;

    struct Registration : Property::Registration {
        Symbol name() const override {
            return Properties::MARGIN_LEFT;
        }

        Rc<Property> initial() const override {
            return makeRc<MarginLeftProperty>(self(), Calc<Length, Percent>(Length{}));
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<MarginLeftProperty>(self(), c.margin->start);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<MarginLeftProperty>(self(), try$(parseValue<Value>(c))));
        }
    };

    Value _value;

    MarginLeftProperty(Rc<Property::Registration> registration, Value value)
        : Property(registration), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.margin.cow().start = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://drafts.csswg.org/css-box-4/#margin-shorthand
export struct MarginProperty : Property {
    using Value = Math::Insets<MarginTopProperty::Value>;

    struct Registration : Property::Registration {
        Symbol name() const override {
            return Properties::MARGIN;
        }

        Flags<Options> flags() const override {
            return {SHORTHAND_PROPERTY};
        }

        Rc<Property> initial() const override {
            return makeRc<MarginProperty>(self(), Value{Calc<Length, Percent>(Length{})});
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<MarginProperty>(self(), *c.margin);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<MarginProperty>(self(), try$(parseValue<Value>(c))));
        }
    };

    Value _value;

    MarginProperty(Rc<Property::Registration> registration, Value value)
        : Property(registration), _value(value) {}

    Vec<Rc<Property>> expandShorthand(RegisteredPropertySet& registry, ComputedValues const&, ComputedValues&) const override {
        return {
            makeRc<MarginTopProperty>(registry.resolveRegistration(Properties::MARGIN_TOP, {}).unwrap(), _value.top),
            makeRc<MarginBottomProperty>(registry.resolveRegistration(Properties::MARGIN_BOTTOM, {}).unwrap(), _value.bottom),
            makeRc<MarginLeftProperty>(registry.resolveRegistration(Properties::MARGIN_LEFT, {}).unwrap(), _value.start),
            makeRc<MarginRightProperty>(registry.resolveRegistration(Properties::MARGIN_RIGHT, {}).unwrap(), _value.end),
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
        Symbol name() const override {
            return Properties::MARGIN_INLINE_START;
        }

        Rc<Property> initial() const override {
            return makeRc<MarginInlineStartProperty>(self(), Calc<Length, Percent>(Length{}));
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<MarginInlineStartProperty>(self(), c.margin->start);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<MarginInlineStartProperty>(self(), try$(parseValue<Value>(c))));
        }
    };

    Value _value;

    MarginInlineStartProperty(Rc<Property::Registration> registration, Value value)
        : Property(registration), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        // FIXME: Take writing mode into account
        c.margin.cow().start = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://drafts.csswg.org/css-logical/#propdef-margin-inline-end
export struct MarginInlineEndProperty : Property {
    using Value = MarginTopProperty::Value;

    struct Registration : Property::Registration {
        Symbol name() const override {
            return Properties::MARGIN_INLINE_END;
        }

        Rc<Property> initial() const override {
            return makeRc<MarginInlineEndProperty>(self(), Calc<Length, Percent>(Length{}));
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<MarginInlineEndProperty>(self(), c.margin->end);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<MarginInlineEndProperty>(self(), try$(parseValue<Value>(c))));
        }
    };

    Value _value;

    MarginInlineEndProperty(Rc<Property::Registration> registration, Value value)
        : Property(registration), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        // FIXME: Take writing mode into account
        c.margin.cow().end = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://drafts.csswg.org/css-logical/#propdef-margin-inline
export struct MarginInlineProperty : Property {
    using Value = Math::Insets<MarginTopProperty::Value>;

    struct Registration : Property::Registration {
        Symbol name() const override {
            return Properties::MARGIN_INLINE;
        }

        Rc<Property> initial() const override {
            return makeRc<MarginInlineProperty>(self(), Value{Calc<Length, Percent>(Length{})});
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<MarginInlineProperty>(self(), Math::Insets{c.margin->start, c.margin->end});
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<MarginInlineProperty>(self(), try$(parseValue<Value>(c))));
        }
    };

    Value _value;

    MarginInlineProperty(Rc<Property::Registration> registration, Value value)
        : Property(registration), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        // FIXME: Take writing mode into account
        c.margin.cow().start = _value.start;
        c.margin.cow().end = _value.end;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://drafts.csswg.org/css-logical/#propdef-margin-block-start
export struct MarginBlockStartProperty : Property {
    using Value = MarginTopProperty::Value;

    struct Registration : Property::Registration {
        Symbol name() const override {
            return Properties::MARGIN_BLOCK_START;
        }

        Rc<Property> initial() const override {
            return makeRc<MarginBlockStartProperty>(self(), Calc<Length, Percent>(Length{}));
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<MarginBlockStartProperty>(self(), c.margin->top);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<MarginBlockStartProperty>(self(), try$(parseValue<Value>(c))));
        }
    };

    Value _value;

    MarginBlockStartProperty(Rc<Property::Registration> registration, Value value)
        : Property(registration), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        // FIXME: Take writing mode into account
        c.margin.cow().top = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://drafts.csswg.org/css-logical/#propdef-margin-block-end
export struct MarginBlockEndProperty : Property {
    using Value = MarginTopProperty::Value;

    struct Registration : Property::Registration {
        Symbol name() const override {
            return Properties::MARGIN_BLOCK_END;
        }

        Rc<Property> initial() const override {
            return makeRc<MarginBlockEndProperty>(self(), Calc<Length, Percent>(Length{}));
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<MarginBlockEndProperty>(self(), c.margin->bottom);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<MarginBlockEndProperty>(self(), try$(parseValue<Value>(c))));
        }
    };

    Union<Keywords::Auto, Calc<Length, Percent>> _value;

    MarginBlockEndProperty(Rc<Property::Registration> registration, Value value)
        : Property(registration), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        // FIXME: Take writing mode into account
        c.margin.cow().bottom = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://drafts.csswg.org/css-logical/#propdef-margin-block
export struct MarginBlockProperty : Property {
    using Value = Math::Insets<MarginTopProperty::Value>;

    struct Registration : Property::Registration {
        Symbol name() const override {
            return Properties::MARGIN_BLOCK;
        }

        Rc<Property> initial() const override {
            return makeRc<MarginBlockProperty>(self(), Value{Calc<Length, Percent>(Length{})});
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<MarginBlockProperty>(self(), Math::Insets{c.margin->top, c.margin->bottom});
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<MarginBlockProperty>(self(), try$(parseValue<Value>(c))));
        }
    };

    Value _value;

    MarginBlockProperty(Rc<Property::Registration> registration, Value value)
        : Property(registration), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        // FIXME: Take writing mode into account
        c.margin.cow().top = _value.top;
        c.margin.cow().bottom = _value.bottom;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

} // namespace Vaev::Style
