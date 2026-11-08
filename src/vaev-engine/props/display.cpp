module;

#include <karm/macros>

export module Vaev.Engine:props.display;

import Karm.Core;
import :props.base;
import :css.parser;
import :style.computed;

using namespace Karm;

namespace Vaev::Css {

export template <>
struct ValueParser<Style::ContentValue> {
    // https://www.w3.org/TR/css-content-3/
    // https://www.w3.org/TR/css-gcpm-3/#funcdef-element
    static Res<Style::ContentValue> parse(Cursor<Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c.skip(Token::ident("normal")))
            return Ok(Keywords::NORMAL);
        else if (c.skip(Token::ident("open-quote")))
            return Ok(String{"“"});
        else if (c.skip(Token::ident("close-quote")))
            return Ok(String{"”"});
        else if (c->prefix == Token::function("element(")) {
            Cursor<Sst> cur = c->content;
            auto element = try$(parseValue<ElementFunc>(cur));
            c.next();
            return Ok(element);
        } else if (c->prefix == Token::function("counter(")) {
            return parseValue<CounterFunc>(c);
        } else if (c->prefix == Token::function("counters(")) {
            return parseValue<CountersFunc>(c);
        } else if (c->type == Sst::TOKEN and c->token.type == Token::STRING) {
            return Ok(try$(parseValue<String>(c)));
        } else
            return Error::invalidData("expected content");
    }
};

} // namespace Vaev::Css

namespace Vaev::Style {

// https://www.w3.org/TR/CSS22/visuren.html#propdef-display
export struct DisplayProperty : Property {
    struct Registration : Property::Registration {
        Registration() : Property::Registration(Properties::DISPLAY) {}

        Rc<Property> initial() const override {
            return makeRc<DisplayProperty>(self(), Display{Display::FLOW, Display::INLINE});
        }

        Rc<Property> load(ComputedValues const& s) const override {
            return makeRc<DisplayProperty>(self(), s.display);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<DisplayProperty>(self(), try$(parseValue<Display>(c))));
        }
    };

    Display _value;

    DisplayProperty(Rc<Property::Registration> registration, Display value)
        : Property(registration), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.display = _value;
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return Css::serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/css-gcpm-3/
// https://drafts.csswg.org/css-content/#content-property
export struct ContentProperty : Property {
    struct Registration : Property::Registration {
        Registration() : Property::Registration(Properties::CONTENT) {}

        Rc<Property> initial() const override {
            return makeRc<ContentProperty>(self(), Keywords::NORMAL);
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<ContentProperty>(self(), c.content);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<ContentProperty>(self(), try$(parseValue<ContentValue>(c))));
        }
    };

    ContentValue _value;

    ContentProperty(Rc<Property::Registration> registration, ContentValue value)
        : Property(registration), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.content = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/css-display-3/#order-property
export struct OrderProperty : Property {
    struct Registration : Property::Registration {
        Registration() : Property::Registration(Properties::ORDER) {}

        Rc<Property> initial() const override {
            return makeRc<OrderProperty>(self(), Integer{0});
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<OrderProperty>(self(), c.order);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<OrderProperty>(self(), try$(parseValue<Integer>(c))));
        }
    };

    Integer _value;

    OrderProperty(Rc<Property::Registration> registration, Integer value)
        : Property(registration), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.order = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

} // namespace Vaev::Style
