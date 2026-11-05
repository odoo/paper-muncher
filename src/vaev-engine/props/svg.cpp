module;

#include <karm/macros>

export module Vaev.Engine:props.svg;

import Karm.Core;
import :props.base;
import :css.parser;
import :style.computed;

using namespace Karm;

namespace Vaev::Style {

// MARK: SVG ----------------------------------------------------------------

// https://svgwg.org/svg2-draft/geometry.html#XProperty
export struct SvgXProperty : Property {
    struct Registration : Property::Registration {
        Registration()
            : Property::Registration(Properties::X, PRESENTATION_ATTRIBUTE) {}

        Rc<Property> initial() const override {
            return makeRc<SvgXProperty>(self(), Css::LengthPercentage::fromCanonical(0));
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<SvgXProperty>(self(), toSpecified(c.svg->x));
        }

        Res<Rc<Property>> parsePresentationAttribute(Str style) override {
            return Property::Registration::parsePresentationAttribute(Io::format("{}px", style));
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<SvgXProperty>(self(), try$(parseValue<Css::LengthPercentage>(c))));
        }
    };

    Css::LengthPercentage _value;

    SvgXProperty(Rc<Property::Registration> registration, Css::LengthPercentage value)
        : Property(registration), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.svg.cow().x = resolve(_value, cx);
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://svgwg.org/svg2-draft/geometry.html#YProperty
export struct SvgYProperty : Property {
    struct Registration : Property::Registration {
        Registration()
            : Property::Registration(Properties::Y, PRESENTATION_ATTRIBUTE) {}

        Rc<Property> initial() const override {
            return makeRc<SvgYProperty>(self(), Css::LengthPercentage::fromCanonical(0));
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<SvgYProperty>(self(), toSpecified(c.svg->y));
        }

        Res<Rc<Property>> parsePresentationAttribute(Str style) override {
            return Property::Registration::parsePresentationAttribute(Io::format("{}px", style));
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<SvgYProperty>(self(), try$(parseValue<Css::LengthPercentage>(c))));
        }
    };

    Css::LengthPercentage _value;

    SvgYProperty(Rc<Property::Registration> registration, Css::LengthPercentage value)
        : Property(registration), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.svg.cow().y = resolve(_value, cx);
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://svgwg.org/svg2-draft/geometry.html#CXProperty
export struct SvgCXProperty : Property {
    struct Registration : Property::Registration {
        Registration()
            : Property::Registration(Properties::CX, PRESENTATION_ATTRIBUTE) {}

        Rc<Property> initial() const override {
            return makeRc<SvgCXProperty>(self(), Css::LengthPercentage::fromCanonical(0));
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<SvgCXProperty>(self(), toSpecified(c.svg->cx));
        }

        Res<Rc<Property>> parsePresentationAttribute(Str style) override {
            return Property::Registration::parsePresentationAttribute(Io::format("{}px", style));
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<SvgCXProperty>(self(), try$(parseValue<Css::LengthPercentage>(c))));
        }
    };

    Css::LengthPercentage _value;

    SvgCXProperty(Rc<Property::Registration> registration, Css::LengthPercentage value)
        : Property(registration), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.svg.cow().cx = resolve(_value, cx);
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://svgwg.org/svg2-draft/geometry.html#CYProperty
export struct SvgCYProperty : Property {
    struct Registration : Property::Registration {
        Registration()
            : Property::Registration(Properties::CY, PRESENTATION_ATTRIBUTE) {}

        Rc<Property> initial() const override {
            return makeRc<SvgCYProperty>(self(), Css::LengthPercentage::fromCanonical(0));
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<SvgCYProperty>(self(), toSpecified(c.svg->cy));
        }

        Res<Rc<Property>> parsePresentationAttribute(Str style) override {
            return Property::Registration::parsePresentationAttribute(Io::format("{}px", style));
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<SvgCYProperty>(self(), try$(parseValue<Css::LengthPercentage>(c))));
        }
    };

    Css::LengthPercentage _value;

    SvgCYProperty(Rc<Property::Registration> registration, Css::LengthPercentage value)
        : Property(registration), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.svg.cow().cy = resolve(_value, cx);
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://svgwg.org/svg2-draft/geometry.html#RProperty
export struct SvgRProperty : Property {
    struct Registration : Property::Registration {
        Registration()
            : Property::Registration(Properties::R, PRESENTATION_ATTRIBUTE) {}

        Rc<Property> initial() const override {
            return makeRc<SvgRProperty>(self(), Css::LengthPercentage::fromCanonical(0));
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<SvgRProperty>(self(), toSpecified(c.svg->r));
        }

        Res<Rc<Property>> parsePresentationAttribute(Str style) override {
            return Property::Registration::parsePresentationAttribute(Io::format("{}px", style));
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<SvgRProperty>(self(), try$(parseValue<Css::LengthPercentage>(c))));
        }
    };

    Css::LengthPercentage _value;

    SvgRProperty(Rc<Property::Registration> registration, Css::LengthPercentage value)
        : Property(registration), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.svg.cow().r = resolve(_value, cx);
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://svgwg.org/svg2-draft/painting.html#FillProperty
export struct SvgFillProperty : Property {
    struct Registration : Property::Registration {
        Registration()
            : Property::Registration(Properties::FILL, {PRESENTATION_ATTRIBUTE, INHERITED, BULK_INHERITED}) {}

        Rc<Property> initial() const override {
            return makeRc<SvgFillProperty>(self(), Vaev::Css::SvgPaint{Some(Vaev::Css::Color{Gfx::BLACK})});
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<SvgFillProperty>(self(), toSpecified(c.svgPaint->fill));
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<SvgFillProperty>(self(), try$(parseValue<Vaev::Css::SvgPaint>(c))));
        }
    };

    Vaev::Css::SvgPaint _value;

    SvgFillProperty(Rc<Property::Registration> registration, Vaev::Css::SvgPaint value)
        : Property(registration), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.svgPaint.cow().fill = toComputed(_value, c.color);
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://svgwg.org/svg2-draft/paths.html#TheDProperty
export struct SvgDProperty : Property {
    struct Registration : Property::Registration {
        Registration()
            : Property::Registration(Properties::D, PRESENTATION_ATTRIBUTE) {}

        Rc<Property> initial() const override {
            return makeRc<SvgDProperty>(self(), Union<String, None>{NONE});
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
            if (c.peek() != Css::Sst::FUNC or c.peek().prefix != Css::Token::function("path(")) {
                return Error::invalidData("expected path function");
            }

            auto pathFunc = c.next();
            Cursor<Css::Sst> scanPath{pathFunc.content};
            return Ok(makeRc<SvgDProperty>(self(), try$(parseValue<String>(scanPath))));
        }
    };

    Union<String, None> _value;

    SvgDProperty(Rc<Property::Registration> registration, Union<String, None> value)
        : Property(registration), _value(value) {}

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
    struct Registration : Property::Registration {
        Registration()
            : Property::Registration(Properties::VIEWBOX, PRESENTATION_ATTRIBUTE) {}

        Rc<Property> initial() const override {
            return makeRc<SvgViewBoxProperty>(self(), Opt<Css::SvgViewBox>{NONE});
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<SvgViewBoxProperty>(self(), c.svg->viewBox);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
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

    Opt<Css::SvgViewBox> _value;

    SvgViewBoxProperty(Rc<Property::Registration> registration, Opt<Css::SvgViewBox> value)
        : Property(registration), _value(std::move(value)) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.svg.cow().viewBox = _value;
    }

    Res<> serialize(Io::TextWriter& w) const override {
        if (auto const& [viewBox] = _value)
            return serializeValue(w, viewBox);

        // NOSPEC: An unspecified viewBox has no serialized form.
        return Ok();
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://svgwg.org/svg2-draft/painting.html#SpecifyingStrokePaint
export struct SvgStrokeProperty : Property {
    struct Registration : Property::Registration {
        Registration()
            : Property::Registration(Properties::STROKE, {PRESENTATION_ATTRIBUTE, INHERITED, BULK_INHERITED}) {}

        Rc<Property> initial() const override {
            return makeRc<SvgStrokeProperty>(self(), Vaev::Css::SvgPaint{NONE});
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<SvgStrokeProperty>(self(), toSpecified(c.svgPaint->stroke));
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<SvgStrokeProperty>(self(), try$(parseValue<Vaev::Css::SvgPaint>(c))));
        }
    };

    Vaev::Css::SvgPaint _value;

    SvgStrokeProperty(Rc<Property::Registration> registration, Vaev::Css::SvgPaint value)
        : Property(registration), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.svgPaint.cow().stroke = toComputed(_value, c.color);
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://svgwg.org/svg2-draft/painting.html#StrokeOpacity
export struct SvgStrokeOpacityProperty : Property {
    struct Registration : Property::Registration {
        Registration()
            : Property::Registration(Properties::STROKE_OPACITY, {INHERITED, BULK_INHERITED}) {}

        Rc<Property> initial() const override {
            return makeRc<SvgStrokeOpacityProperty>(self(), f64{1});
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<SvgStrokeOpacityProperty>(self(), c.svgPaint->strokeOpacity);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            auto maybePercent = parseValue<Css::PurePercentage>(c);
            if (maybePercent) {
                return Ok(makeRc<SvgStrokeOpacityProperty>(self(), maybePercent.expect().value() / 100));
            } else {
                return Ok(makeRc<SvgStrokeOpacityProperty>(self(), try$(parseValue<f64>(c))));
            }
        }
    };

    f64 _value;

    SvgStrokeOpacityProperty(Rc<Property::Registration> registration, f64 value)
        : Property(registration), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.svgPaint.cow().strokeOpacity = _value;
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
    struct Registration : Property::Registration {
        Registration()
            : Property::Registration(Properties::FILL_OPACITY, {PRESENTATION_ATTRIBUTE, INHERITED, BULK_INHERITED}) {}

        Rc<Property> initial() const override {
            return makeRc<FillOpacityProperty>(self(), f64{1});
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<FillOpacityProperty>(self(), c.svgPaint->fillOpacity);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            auto maybePercent = parseValue<Css::PurePercentage>(c);
            if (maybePercent) {
                return Ok(makeRc<FillOpacityProperty>(self(), maybePercent.expect().value() / 100));
            } else {
                return Ok(makeRc<FillOpacityProperty>(self(), try$(parseValue<f64>(c))));
            }
        }
    };

    f64 _value;

    FillOpacityProperty(Rc<Property::Registration> registration, f64 value)
        : Property(registration), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.svgPaint.cow().fillOpacity = _value;
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
    struct Registration : Property::Registration {
        Registration()
            : Property::Registration(Properties::STROKE_WIDTH, {PRESENTATION_ATTRIBUTE, INHERITED, BULK_INHERITED}) {}

        Rc<Property> initial() const override {
            return makeRc<StrokeWidthProperty>(self(), Css::LengthPercentage::fromCanonical(1));
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<StrokeWidthProperty>(self(), toSpecified(c.svgPaint->strokeWidth));
        }

        // https://svgwg.org/svg2-draft/painting.html#StrokeWidth
        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            if (auto number = parseValue<f64>(c))
                return Ok(makeRc<StrokeWidthProperty>(self(), Css::LengthPercentage::fromCanonical(number.take())));
            return Ok(makeRc<StrokeWidthProperty>(self(), try$(parseValue<Css::LengthPercentage>(c))));
        }
    };

    Css::LengthPercentage _value;

    StrokeWidthProperty(Rc<Property::Registration> registration, Css::LengthPercentage value)
        : Property(registration), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.svgPaint.cow().strokeWidth = resolve(_value, cx);
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

} // namespace Vaev::Style
