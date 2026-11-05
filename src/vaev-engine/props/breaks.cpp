module;

#include <karm/macros>

export module Vaev.Engine:props.breaks;

import Karm.Core;
import :props.base;
import :css.parser;
import :style.computed;

using namespace Karm;

namespace Vaev::Style {

// MARK: Breaks ----------------------------------------------------------------

// https://www.w3.org/TR/css-break-3/#propdef-break-after
export struct BreakAfterProperty : Property {
    struct Registration : Property::Registration {
        Registration() : Property::Registration(Properties::BREAK_AFTER) {}

        Rc<Property> initial() const override {
            return makeRc<BreakAfterProperty>(self(), Keywords::AUTO);
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<BreakAfterProperty>(self(), c.break_->after);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<BreakAfterProperty>(self(), try$(parseValue<BreakBetween>(c))));
        }
    };

    BreakBetween _value;

    BreakAfterProperty(Rc<Property::Registration> registration, BreakBetween value)
        : Property(registration), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.break_.cow().after = _value;
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/css-break-3/#propdef-break-before
export struct BreakBeforeProperty : Property {
    struct Registration : Property::Registration {
        Registration() : Property::Registration(Properties::BREAK_BEFORE) {}

        Rc<Property> initial() const override {
            return makeRc<BreakBeforeProperty>(self(), Keywords::AUTO);
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<BreakBeforeProperty>(self(), c.break_->before);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<BreakBeforeProperty>(self(), try$(parseValue<BreakBetween>(c))));
        }
    };

    BreakBetween _value;

    BreakBeforeProperty(Rc<Property::Registration> registration, BreakBetween value)
        : Property(registration), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.break_.cow().before = _value;
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/css-break-3/#break-within
export struct BreakInsideProperty : Property {
    struct Registration : Property::Registration {
        Registration() : Property::Registration(Properties::BREAK_INSIDE) {}

        Rc<Property> initial() const override {
            return makeRc<BreakInsideProperty>(self(), Keywords::AUTO);
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<BreakInsideProperty>(self(), c.break_->inside);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<BreakInsideProperty>(self(), try$(parseValue<BreakInside>(c))));
        }
    };

    BreakInside _value;

    BreakInsideProperty(Rc<Property::Registration> registration, BreakInside value)
        : Property(registration), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.break_.cow().inside = _value;
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// MARK: Page Break Aliases --------------------------------------------------

// https://www.w3.org/TR/css-break-3/#page-break-properties
static Res<BreakBetween> _parseLegacyPageBreak(Cursor<Css::Sst>& c) {
    if (c.skip(Css::Token::ident("auto")))
        return Ok(Keywords::AUTO);
    if (c.skip(Css::Token::ident("always")))
        return Ok(Keywords::PAGE);
    if (c.skip(Css::Token::ident("avoid")))
        return Ok(Keywords::AVOID);
    if (c.skip(Css::Token::ident("left")))
        return Ok(Keywords::LEFT);
    if (c.skip(Css::Token::ident("right")))
        return Ok(Keywords::RIGHT);
    return Error::invalidData("expected auto, always, avoid, left or right");
}

static void _reprLegacyPageBreak(Io::Emit& e, BreakBetween value) {
    if (value == Keywords::PAGE)
        e("always");
    else
        e("{}", value);
}

// https://www.w3.org/TR/css-break-3/#page-break-properties
export struct PageBreakAfterProperty : Property {
    struct Registration : Property::Registration {
        Registration() : Property::Registration(Properties::PAGE_BREAK_AFTER, SHORTHAND) {}

        Rc<Property> initial() const override {
            return makeRc<PageBreakAfterProperty>(self(), Keywords::AUTO);
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<PageBreakAfterProperty>(self(), c.break_->after);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<PageBreakAfterProperty>(self(), try$(_parseLegacyPageBreak(c))));
        }
    };

    BreakBetween _value;

    PageBreakAfterProperty(Rc<Property::Registration> registration, BreakBetween value)
        : Property(registration), _value(value) {}

    Longhands expand(RegisteredPropertySet& registry, ComputedValues const&, ComputedValues&) const override {
        return {
            makeRc<BreakAfterProperty>(registry.resolveRegistration(Properties::BREAK_AFTER, {}).expect(), _value),
        };
    }

    void repr(Io::Emit& e) const override {
        _reprLegacyPageBreak(e, _value);
    }
};

// https://www.w3.org/TR/css-break-3/#page-break-properties
export struct PageBreakBeforeProperty : Property {
    struct Registration : Property::Registration {
        Registration() : Property::Registration(Properties::PAGE_BREAK_BEFORE, SHORTHAND) {}

        Rc<Property> initial() const override {
            return makeRc<PageBreakBeforeProperty>(self(), Keywords::AUTO);
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<PageBreakBeforeProperty>(self(), c.break_->before);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<PageBreakBeforeProperty>(self(), try$(_parseLegacyPageBreak(c))));
        }
    };

    BreakBetween _value;

    PageBreakBeforeProperty(Rc<Property::Registration> registration, BreakBetween value)
        : Property(registration), _value(value) {}

    Longhands expand(RegisteredPropertySet& registry, ComputedValues const&, ComputedValues&) const override {
        return {
            makeRc<BreakBeforeProperty>(registry.resolveRegistration(Properties::BREAK_BEFORE, {}).expect(), _value),
        };
    }

    void repr(Io::Emit& e) const override {
        _reprLegacyPageBreak(e, _value);
    }
};

// https://www.w3.org/TR/css-break-3/#page-break-properties
export struct PageBreakInsideProperty : Property {
    struct Registration : Property::Registration {
        Registration() : Property::Registration(Properties::PAGE_BREAK_INSIDE, SHORTHAND) {}

        Rc<Property> initial() const override {
            return makeRc<PageBreakInsideProperty>(self(), Keywords::AUTO);
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<PageBreakInsideProperty>(self(), c.break_->inside);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            if (c.skip(Css::Token::ident("auto")))
                return Ok(makeRc<PageBreakInsideProperty>(self(), Keywords::AUTO));
            if (c.skip(Css::Token::ident("avoid")))
                return Ok(makeRc<PageBreakInsideProperty>(self(), Keywords::AVOID));
            return Error::invalidData("expected auto or avoid");
        }
    };

    BreakInside _value;

    PageBreakInsideProperty(Rc<Property::Registration> registration, BreakInside value)
        : Property(registration), _value(value) {}

    Longhands expand(RegisteredPropertySet& registry, ComputedValues const&, ComputedValues&) const override {
        return {
            makeRc<BreakInsideProperty>(registry.resolveRegistration(Properties::BREAK_INSIDE, {}).expect(), _value),
        };
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

} // namespace Vaev::Style
