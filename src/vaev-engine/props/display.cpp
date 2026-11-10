module;

#include <karm/macros>

export module Vaev.Engine:props.display;

import Karm.Core;
import :props.base;
import :css.parser;
import :style.computed;
import :style.values.keywords;

using namespace Karm;
using namespace Karm::Literals;

namespace Vaev::Style {

// https://www.w3.org/TR/CSS22/visuren.html#propdef-display
export struct DisplayProperty : Property {
    struct Registration : Property::Registration {
        Registration() : Property::Registration(Properties::DISPLAY) {}

        InitialValue initial() const override {
            return "inline"s;
        }

        Rc<Property> load(ComputedValues const& s) const override {
            return makeRc<DisplayProperty>(self(), s.display);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<DisplayProperty>(self(), try$(parseValue<Css::Display>(c))));
        }
    };

    Css::Display _value;

    DisplayProperty(Rc<Property::Registration> registration, Css::Display value)
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

        InitialValue initial() const override {
            return "normal"s;
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<ContentProperty>(self(), c.content);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            // https://www.w3.org/TR/css-content-3/
            // https://www.w3.org/TR/css-gcpm-3/#funcdef-element
            auto value = [&] -> Res<Css::ContentValue> {
                if (c.ended())
                    return Error::invalidData("unexpected end of input");

                if (c.skip(Css::Token::ident("normal")))
                    return Ok(Keywords::NORMAL);
                else if (c.skip(Css::Token::ident("open-quote")))
                    return Ok(String{"“"});
                else if (c.skip(Css::Token::ident("close-quote")))
                    return Ok(String{"”"});
                else if (c->prefix == Css::Token::function("element(")) {
                    Cursor<Css::Sst> cur = c->content;
                    auto element = try$(parseValue<Css::ElementFunc>(cur));
                    c.next();
                    return Ok(element);
                } else if (c->prefix == Css::Token::function("counter(")) {
                    return parseValue<Css::CounterFunc>(c);
                } else if (c->prefix == Css::Token::function("counters(")) {
                    return parseValue<Css::CountersFunc>(c);
                } else if (c->type == Css::Sst::TOKEN and c->token.type == Css::Token::STRING) {
                    return Ok(try$(parseValue<String>(c)));
                } else
                    return Error::invalidData("expected content");
            }();
            return Ok(makeRc<ContentProperty>(self(), try$(value)));
        }
    };

    Css::ContentValue _value;

    ContentProperty(Rc<Property::Registration> registration, Css::ContentValue value)
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

        InitialValue initial() const override {
            return "0"s;
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<OrderProperty>(self(), c.order);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<OrderProperty>(self(), try$(parseValue<Css::Integer>(c))));
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
