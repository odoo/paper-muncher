module;

#include <karm-core/macros.h>

export module Vaev.Engine:props.breaks;

import Karm.Core;
import :props.base;
import :css.parser;
import :style.specified;

using namespace Karm;

namespace Vaev::Style {

// MARK: Breaks ----------------------------------------------------------------

// https://www.w3.org/TR/css-break-3/#propdef-break-after
export struct BreakAfterProperty : Property {
    struct Registration : Property::Registration {
        Symbol name() const override {
            return Properties::BREAK_AFTER;
        }

        Rc<Property> initial() const override {
            return makeRc<BreakAfterProperty>(self(), BreakBetween::AUTO);
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<BreakAfterProperty>(self(), c.break_->after);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<BreakAfterProperty>(self(), try$(parseValue<BreakBetween>(c))));
        }
    };

    BreakBetween _value;

    BreakAfterProperty(Rc<Property::Registration> registration, BreakBetween value)
        : Property(registration), _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.break_.cow().after = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/css-break-3/#propdef-break-before
export struct BreakBeforeProperty : Property {
    struct Registration : Property::Registration {
        Symbol name() const override {
            return Properties::BREAK_BEFORE;
        }

        Rc<Property> initial() const override {
            return makeRc<BreakBeforeProperty>(self(), BreakBetween::AUTO);
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<BreakBeforeProperty>(self(), c.break_->before);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<BreakBeforeProperty>(self(), try$(parseValue<BreakBetween>(c))));
        }
    };

    BreakBetween _value;

    BreakBeforeProperty(Rc<Property::Registration> registration, BreakBetween value)
        : Property(registration), _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.break_.cow().before = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/css-break-3/#break-within
export struct BreakInsideProperty : Property {
    struct Registration : Property::Registration {
        Symbol name() const override {
            return Properties::BREAK_INSIDE;
        }

        Rc<Property> initial() const override {
            return makeRc<BreakInsideProperty>(self(), BreakInside::AUTO);
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<BreakInsideProperty>(self(), c.break_->inside);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<BreakInsideProperty>(self(), try$(parseValue<BreakInside>(c))));
        }
    };

    BreakInside _value;

    BreakInsideProperty(Rc<Property::Registration> registration, BreakInside value)
        : Property(registration), _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.break_.cow().inside = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

} // namespace Vaev::Style
