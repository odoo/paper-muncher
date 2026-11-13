module;

#include <karm/macros>

export module Vaev.Engine:props.display;

import Karm.Core;
import :props.base;
import :css.parser;
import :style.computed;

using namespace Karm;

namespace Vaev::Style {

// https://www.w3.org/TR/CSS22/visuren.html#propdef-display
export struct DisplayProperty : Property {
    static constexpr auto ID = LonghandId::DISPLAY;

    static Rc<Property> initial() {
        return makeRc<DisplayProperty>(Display{Display::FLOW, Display::INLINE});
    }

    static Rc<Property> load(ComputedValues const& s) {
        return makeRc<DisplayProperty>(s.display);
    }

    static Res<Rc<Property>> parse(Cursor<Css::Sst>& c, ParsingMode) {
        return Ok(makeRc<DisplayProperty>(try$(parseValue<Display>(c))));
    }

    struct Registration : LonghandRegistration {
        Registration() : LonghandRegistration(Properties::DISPLAY) {}
    };

    Display _value;

    DisplayProperty(Display value)
        : Property(ID), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.display = _value;
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/css-gcpm-3/
// https://drafts.csswg.org/css-content/#content-property
export struct ContentProperty : Property {
    static constexpr auto ID = LonghandId::CONTENT;

    static Rc<Property> initial() {
        return makeRc<ContentProperty>(Keywords::NORMAL);
    }

    static Rc<Property> load(ComputedValues const& c) {
        return makeRc<ContentProperty>(c.content);
    }

    static Res<Rc<Property>> parse(Cursor<Css::Sst>& c, ParsingMode) {
        return Ok(makeRc<ContentProperty>(try$(parseValue<Content>(c))));
    }

    struct Registration : LonghandRegistration {
        Registration() : LonghandRegistration(Properties::CONTENT) {}
    };

    Content _value;

    ContentProperty(Content value)
        : Property(ID), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.content = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/css-display-3/#order-property
export struct OrderProperty : Property {
    static constexpr auto ID = LonghandId::ORDER;

    static Rc<Property> initial() {
        return makeRc<OrderProperty>(Integer{0});
    }

    static Rc<Property> load(ComputedValues const& c) {
        return makeRc<OrderProperty>(c.order);
    }

    static Res<Rc<Property>> parse(Cursor<Css::Sst>& c, ParsingMode) {
        return Ok(makeRc<OrderProperty>(try$(parseValue<Integer>(c))));
    }

    struct Registration : LonghandRegistration {
        Registration() : LonghandRegistration(Properties::ORDER) {}
    };

    Integer _value;

    OrderProperty(Integer value)
        : Property(ID), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.order = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

} // namespace Vaev::Style
