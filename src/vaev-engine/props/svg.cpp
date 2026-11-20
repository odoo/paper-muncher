module;

#include <karm/macros>

export module Vaev.Engine:props.svg;

import Karm.Core;
import :props.base;
import :css.parser;
import :style.computed;

using namespace Karm;
using namespace Karm::Literals;

namespace Vaev::Style {

// MARK: SVG ----------------------------------------------------------------

// https://svgwg.org/svg2-draft/geometry.html#XProperty
export struct SvgXProperty : Property {
    using Value = Css::LengthPercentage;

    struct Registration : Property::Registration {
        Registration()
            : Property::Registration(Properties::X, PRESENTATION_ATTRIBUTE) {}

        InitialValue initialValue() const override {
            return "0"s;
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<SvgXProperty>(self(), specifyValue<Value>(c.svg->x));
        }

        Res<Rc<Property>> parsePresentationAttribute(Str style) override {
            return Property::Registration::parsePresentationAttribute(Io::format("{}px", style));
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<SvgXProperty>(self(), try$(parseValue<Value>(c))));
        }
    };

    Value _value;

    SvgXProperty(Rc<Property::Registration> registration, Value value)
        : Property(registration), _value(std::move(value)) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.svg.cow().x = computeValue(_value, cx);
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return Css::serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://svgwg.org/svg2-draft/geometry.html#YProperty
export struct SvgYProperty : Property {
    using Value = Css::LengthPercentage;

    struct Registration : Property::Registration {
        Registration()
            : Property::Registration(Properties::Y, PRESENTATION_ATTRIBUTE) {}

        InitialValue initialValue() const override {
            return "0"s;
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<SvgYProperty>(self(), specifyValue<Value>(c.svg->y));
        }

        Res<Rc<Property>> parsePresentationAttribute(Str style) override {
            return Property::Registration::parsePresentationAttribute(Io::format("{}px", style));
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<SvgYProperty>(self(), try$(parseValue<Value>(c))));
        }
    };

    Value _value;

    SvgYProperty(Rc<Property::Registration> registration, Value value)
        : Property(registration), _value(std::move(value)) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.svg.cow().y = computeValue(_value, cx);
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return Css::serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://svgwg.org/svg2-draft/geometry.html#CXProperty
export struct SvgCXProperty : Property {
    using Value = Css::LengthPercentage;

    struct Registration : Property::Registration {
        Registration()
            : Property::Registration(Properties::CX, PRESENTATION_ATTRIBUTE) {}

        InitialValue initialValue() const override {
            return "0"s;
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<SvgCXProperty>(self(), specifyValue<Value>(c.svg->cx));
        }

        Res<Rc<Property>> parsePresentationAttribute(Str style) override {
            return Property::Registration::parsePresentationAttribute(Io::format("{}px", style));
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<SvgCXProperty>(self(), try$(parseValue<Value>(c))));
        }
    };

    Value _value;

    SvgCXProperty(Rc<Property::Registration> registration, Value value)
        : Property(registration), _value(std::move(value)) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.svg.cow().cx = computeValue(_value, cx);
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return Css::serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://svgwg.org/svg2-draft/geometry.html#CYProperty
export struct SvgCYProperty : Property {
    using Value = Css::LengthPercentage;

    struct Registration : Property::Registration {
        Registration()
            : Property::Registration(Properties::CY, PRESENTATION_ATTRIBUTE) {}

        InitialValue initialValue() const override {
            return "0"s;
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<SvgCYProperty>(self(), specifyValue<Value>(c.svg->cy));
        }

        Res<Rc<Property>> parsePresentationAttribute(Str style) override {
            return Property::Registration::parsePresentationAttribute(Io::format("{}px", style));
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<SvgCYProperty>(self(), try$(parseValue<Value>(c))));
        }
    };

    Value _value;

    SvgCYProperty(Rc<Property::Registration> registration, Value value)
        : Property(registration), _value(std::move(value)) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.svg.cow().cy = computeValue(_value, cx);
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return Css::serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://svgwg.org/svg2-draft/geometry.html#RProperty
export struct SvgRProperty : Property {
    using Value = Css::LengthPercentage;

    struct Registration : Property::Registration {
        Registration()
            : Property::Registration(Properties::R, PRESENTATION_ATTRIBUTE) {}

        InitialValue initialValue() const override {
            return "0"s;
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<SvgRProperty>(self(), specifyValue<Value>(c.svg->r));
        }

        Res<Rc<Property>> parsePresentationAttribute(Str style) override {
            return Property::Registration::parsePresentationAttribute(Io::format("{}px", style));
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<SvgRProperty>(self(), try$(parseValue<Value>(c))));
        }
    };

    Value _value;

    SvgRProperty(Rc<Property::Registration> registration, Value value)
        : Property(registration), _value(std::move(value)) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.svg.cow().r = computeValue(_value, cx);
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return Css::serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://svgwg.org/svg2-draft/geometry.html#RxProperty
export struct SvgRxProperty : Property {
    using Value = Union<Keywords::Auto, Css::LengthPercentage>;

    struct Registration : Property::Registration {
        Registration()
            : Property::Registration(Properties::RX, PRESENTATION_ATTRIBUTE) {}

        InitialValue initialValue() const override {
            return "auto"s;
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<SvgRxProperty>(self(), specifyValue<Value>(c.svg->rx));
        }

        Res<Rc<Property>> parsePresentationAttribute(Str style) override {
            if (auto property = Property::Registration::parsePresentationAttribute(style))
                return property;
            return Property::Registration::parsePresentationAttribute(Io::format("{}px", style));
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<SvgRxProperty>(self(), try$(parseValue<Value>(c))));
        }
    };

    Value _value;

    SvgRxProperty(Rc<Property::Registration> registration, Value value)
        : Property(registration), _value(std::move(value)) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.svg.cow().rx = computeValue(_value, cx);
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return Css::serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://svgwg.org/svg2-draft/geometry.html#RyProperty
export struct SvgRyProperty : Property {
    using Value = Union<Keywords::Auto, Css::LengthPercentage>;

    struct Registration : Property::Registration {
        Registration()
            : Property::Registration(Properties::RY, PRESENTATION_ATTRIBUTE) {}

        InitialValue initialValue() const override {
            return "auto"s;
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<SvgRyProperty>(self(), specifyValue<Value>(c.svg->ry));
        }

        Res<Rc<Property>> parsePresentationAttribute(Str style) override {
            if (auto property = Property::Registration::parsePresentationAttribute(style))
                return property;
            return Property::Registration::parsePresentationAttribute(Io::format("{}px", style));
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<SvgRyProperty>(self(), try$(parseValue<Value>(c))));
        }
    };

    Value _value;

    SvgRyProperty(Rc<Property::Registration> registration, Value value)
        : Property(registration), _value(std::move(value)) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.svg.cow().ry = computeValue(_value, cx);
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return Css::serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://svgwg.org/svg2-draft/painting.html#FillProperty
export struct SvgFillProperty : Property {
    using Value = Css::SvgPaint;

    struct Registration : Property::Registration {
        Registration()
            : Property::Registration(Properties::FILL, {PRESENTATION_ATTRIBUTE, INHERITED, BULK_INHERITED}) {}

        InitialValue initialValue() const override {
            return "black"s;
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<SvgFillProperty>(self(), specifyValue<Value>(c.svgPaint->fill));
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<SvgFillProperty>(self(), try$(parseValue<Value>(c))));
        }
    };

    Value _value;

    SvgFillProperty(Rc<Property::Registration> registration, Value value)
        : Property(registration), _value(std::move(value)) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.svgPaint.cow().fill = computeValue(_value, c.color);
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return Css::serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://svgwg.org/svg2-draft/paths.html#TheDProperty
export struct SvgDProperty : Property {
    using Value = Union<String, None>;

    struct Registration : Property::Registration {
        Registration()
            : Property::Registration(Properties::D, PRESENTATION_ATTRIBUTE) {}

        InitialValue initialValue() const override {
            return "none"s;
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<SvgDProperty>(self(), c.svg->d);
        }

        static String _wrapPathAsCSSStyle(Str style) {
            StringBuilder sb;
            sb.append("path(\""s);
            for (auto r : iterRunes(style)) {
                if (r == '\n')
                    continue;
                sb.append(r);
            }
            sb.append("\")"s);
            return sb.take();
        }

        Res<Rc<Property>> parsePresentationAttribute(Str style) override {
            auto fixedStyle = _wrapPathAsCSSStyle(style);
            Css::Lexer lex{fixedStyle};
            Diag::Collector diags = Diag::Collector::ignore();
            auto [sst, _] = Css::consumeDeclarationValue(lex, diags);
            Cursor<Css::Sst> content = sst;
            return parse(content);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            eatWhitespace(c);
            if (c.skip(Css::Token::ident("none")))
                return Ok(makeRc<SvgDProperty>(self(), Union<String, None>{NONE}));

            if (c.peek() != Css::Sst::FUNC or c.peek().prefix != Css::Token::function("path(")) {
                return Error::invalidData("expected path function");
            }

            auto pathFunc = c.next();
            Cursor<Css::Sst> scanPath{pathFunc.content};
            return Ok(makeRc<SvgDProperty>(self(), try$(parseValue<String>(scanPath))));
        }
    };

    Value _value;

    SvgDProperty(Rc<Property::Registration> registration, Value value)
        : Property(registration), _value(std::move(value)) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.svg.cow().d = _value;
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return _value.visit(
            [&](String const& path) -> Res<> {
                // https://drafts.csswg.org/css-shapes-1/#basic-shape-serialization
                try$(w.writeStr("path("s));
                try$(Css::serializeValue(w, path));
                return w.writeRune(')');
            },
            [&](None) -> Res<> {
                // https://svgwg.org/svg2-draft/paths.html#TheDProperty
                return w.writeStr("none"s);
            }
        );
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://svgwg.org/svg2-draft/coords.html#ViewBoxAttribute
export struct SvgViewBoxProperty : Property {
    using Value = Opt<Css::SvgViewBox>;

    struct Registration : Property::Registration {
        Registration()
            : Property::Registration(Properties::VIEWBOX, PRESENTATION_ATTRIBUTE) {}

        InitialValue initialValue() const override {
            return "none"s;
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<SvgViewBoxProperty>(self(), c.svg->viewBox);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            if (c.skip(Css::Token::ident("none")))
                return Ok(makeRc<SvgViewBoxProperty>(self(), Opt<Css::SvgViewBox>{NONE}));

            Css::SvgViewBox viewBox;

            viewBox.minX = try$(parseValue<f64>(c));

            c.skip(Css::Token::COMMA);
            viewBox.minY = try$(parseValue<f64>(c));

            c.skip(Css::Token::COMMA);
            viewBox.width = try$(parseValue<f64>(c));

            c.skip(Css::Token::COMMA);
            viewBox.height = try$(parseValue<f64>(c));

            return Ok(makeRc<SvgViewBoxProperty>(self(), Some(std::move(viewBox))));
        }
    };

    Value _value;

    SvgViewBoxProperty(Rc<Property::Registration> registration, Value value)
        : Property(registration), _value(std::move(value)) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.svg.cow().viewBox = _value;
    }

    Res<> serialize(Io::TextWriter& w) const override {
        if (auto const& [viewBox] = _value)
            return Css::serializeValue(w, viewBox);

        // NOSPEC: An unspecified viewBox has no serialized form.
        return Ok();
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://svgwg.org/svg2-draft/painting.html#SpecifyingStrokePaint
export struct SvgStrokeProperty : Property {
    using Value = Css::SvgPaint;

    struct Registration : Property::Registration {
        Registration()
            : Property::Registration(Properties::STROKE, {PRESENTATION_ATTRIBUTE, INHERITED, BULK_INHERITED}) {}

        InitialValue initialValue() const override {
            return "none"s;
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<SvgStrokeProperty>(self(), specifyValue<Value>(c.svgPaint->stroke));
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<SvgStrokeProperty>(self(), try$(parseValue<Value>(c))));
        }
    };

    Value _value;

    SvgStrokeProperty(Rc<Property::Registration> registration, Value value)
        : Property(registration), _value(std::move(value)) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.svgPaint.cow().stroke = computeValue(_value, c.color);
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return Css::serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://svgwg.org/svg2-draft/painting.html#StrokeOpacity
export struct SvgStrokeOpacityProperty : Property {
    using Value = Css::OpacityValue;

    struct Registration : Property::Registration {
        Registration()
            : Property::Registration(Properties::STROKE_OPACITY, {INHERITED, BULK_INHERITED}) {}

        InitialValue initialValue() const override {
            return "1"s;
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<SvgStrokeOpacityProperty>(self(), specifyValue<Value>(c.svgPaint->strokeOpacity));
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<SvgStrokeOpacityProperty>(self(), try$(parseValue<Value>(c))));
        }
    };

    Value _value;

    SvgStrokeOpacityProperty(Rc<Property::Registration> registration, Value value)
        : Property(registration), _value(std::move(value)) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.svgPaint.cow().strokeOpacity = computeValue(_value);
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return Css::serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://svgwg.org/svg2-draft/painting.html#FillOpacity
export struct FillOpacityProperty : Property {
    using Value = Css::OpacityValue;

    struct Registration : Property::Registration {
        Registration()
            : Property::Registration(Properties::FILL_OPACITY, {PRESENTATION_ATTRIBUTE, INHERITED, BULK_INHERITED}) {}

        InitialValue initialValue() const override {
            return "1"s;
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<FillOpacityProperty>(self(), specifyValue<Value>(c.svgPaint->fillOpacity));
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<FillOpacityProperty>(self(), try$(parseValue<Value>(c))));
        }
    };

    Value _value;

    FillOpacityProperty(Rc<Property::Registration> registration, Value value)
        : Property(registration), _value(std::move(value)) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.svgPaint.cow().fillOpacity = computeValue(_value);
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return Css::serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://svgwg.org/svg2-draft/painting.html#StrokeWidth
export struct StrokeWidthProperty : Property {
    using Value = Css::LengthPercentage;

    struct Registration : Property::Registration {
        Registration()
            : Property::Registration(Properties::STROKE_WIDTH, {PRESENTATION_ATTRIBUTE, INHERITED, BULK_INHERITED}) {}

        InitialValue initialValue() const override {
            return "1px"s;
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<StrokeWidthProperty>(self(), specifyValue<Value>(c.svgPaint->strokeWidth));
        }

        // https://svgwg.org/svg2-draft/painting.html#StrokeWidth
        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            if (auto number = parseValue<f64>(c))
                return Ok(makeRc<StrokeWidthProperty>(self(), Css::px(number.take())));
            return Ok(makeRc<StrokeWidthProperty>(self(), try$(parseValue<Value>(c))));
        }
    };

    Value _value;

    StrokeWidthProperty(Rc<Property::Registration> registration, Value value)
        : Property(registration), _value(std::move(value)) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.svgPaint.cow().strokeWidth = computeValue(_value, cx);
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return Css::serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

} // namespace Vaev::Style
