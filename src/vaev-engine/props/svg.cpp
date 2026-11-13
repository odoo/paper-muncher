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
    static constexpr auto ID = LonghandId::X;
    static inline Flags<Options> const FLAGS = {PRESENTATION_ATTRIBUTE};

    static Rc<Property> initial() {
        return makeRc<SvgXProperty>(PercentOr<Length>{Length{0_au}});
    }

    static Rc<Property> load(ComputedValues const& c) {
        return makeRc<SvgXProperty>(c.svg->x);
    }

    static Res<Rc<Property>> parse(Cursor<Css::Sst>& c) {
        return Ok(makeRc<SvgXProperty>(try$(parseValue<PercentOr<Length>>(c))));
    }

    struct Registration : LonghandRegistration {
        Registration()
            : LonghandRegistration(Properties::X, FLAGS) {}

        Res<Rc<Property>> parsePresentationAttribute(Str style) override {
            return LonghandRegistration::parsePresentationAttribute(Io::format("{}px", style));
        }
    };

    PercentOr<Length> _value;

    SvgXProperty(PercentOr<Length> value)
        : Property(registrationOf<SvgXProperty>()), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.svg.cow().x = _value;
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
    static constexpr auto ID = LonghandId::Y;
    static inline Flags<Options> const FLAGS = {PRESENTATION_ATTRIBUTE};

    static Rc<Property> initial() {
        return makeRc<SvgYProperty>(PercentOr<Length>{Length{0_au}});
    }

    static Rc<Property> load(ComputedValues const& c) {
        return makeRc<SvgYProperty>(c.svg->y);
    }

    static Res<Rc<Property>> parse(Cursor<Css::Sst>& c) {
        return Ok(makeRc<SvgYProperty>(try$(parseValue<PercentOr<Length>>(c))));
    }

    struct Registration : LonghandRegistration {
        Registration()
            : LonghandRegistration(Properties::Y, FLAGS) {}

        Res<Rc<Property>> parsePresentationAttribute(Str style) override {
            return LonghandRegistration::parsePresentationAttribute(Io::format("{}px", style));
        }
    };

    PercentOr<Length> _value;

    SvgYProperty(PercentOr<Length> value)
        : Property(registrationOf<SvgYProperty>()), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.svg.cow().y = _value;
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
    static constexpr auto ID = LonghandId::CX;
    static inline Flags<Options> const FLAGS = {PRESENTATION_ATTRIBUTE};

    static Rc<Property> initial() {
        return makeRc<SvgCXProperty>(PercentOr<Length>{Length{0_au}});
    }

    static Rc<Property> load(ComputedValues const& c) {
        return makeRc<SvgCXProperty>(c.svg->cx);
    }

    static Res<Rc<Property>> parse(Cursor<Css::Sst>& c) {
        return Ok(makeRc<SvgCXProperty>(try$(parseValue<PercentOr<Length>>(c))));
    }

    struct Registration : LonghandRegistration {
        Registration()
            : LonghandRegistration(Properties::CX, FLAGS) {}

        Res<Rc<Property>> parsePresentationAttribute(Str style) override {
            return LonghandRegistration::parsePresentationAttribute(Io::format("{}px", style));
        }
    };

    PercentOr<Length> _value;

    SvgCXProperty(PercentOr<Length> value)
        : Property(registrationOf<SvgCXProperty>()), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.svg.cow().cx = _value;
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
    static constexpr auto ID = LonghandId::CY;
    static inline Flags<Options> const FLAGS = {PRESENTATION_ATTRIBUTE};

    static Rc<Property> initial() {
        return makeRc<SvgCYProperty>(PercentOr<Length>{Length{0_au}});
    }

    static Rc<Property> load(ComputedValues const& c) {
        return makeRc<SvgCYProperty>(c.svg->cy);
    }

    static Res<Rc<Property>> parse(Cursor<Css::Sst>& c) {
        return Ok(makeRc<SvgCYProperty>(try$(parseValue<PercentOr<Length>>(c))));
    }

    struct Registration : LonghandRegistration {
        Registration()
            : LonghandRegistration(Properties::CY, FLAGS) {}

        Res<Rc<Property>> parsePresentationAttribute(Str style) override {
            return LonghandRegistration::parsePresentationAttribute(Io::format("{}px", style));
        }
    };

    PercentOr<Length> _value;

    SvgCYProperty(PercentOr<Length> value)
        : Property(registrationOf<SvgCYProperty>()), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.svg.cow().cy = _value;
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
    static constexpr auto ID = LonghandId::R;
    static inline Flags<Options> const FLAGS = {PRESENTATION_ATTRIBUTE};

    static Rc<Property> initial() {
        return makeRc<SvgRProperty>(PercentOr<Length>{Length{0_au}});
    }

    static Rc<Property> load(ComputedValues const& c) {
        return makeRc<SvgRProperty>(c.svg->r);
    }

    static Res<Rc<Property>> parse(Cursor<Css::Sst>& c) {
        return Ok(makeRc<SvgRProperty>(try$(parseValue<PercentOr<Length>>(c))));
    }

    struct Registration : LonghandRegistration {
        Registration()
            : LonghandRegistration(Properties::R, FLAGS) {}

        Res<Rc<Property>> parsePresentationAttribute(Str style) override {
            return LonghandRegistration::parsePresentationAttribute(Io::format("{}px", style));
        }
    };

    PercentOr<Length> _value;

    SvgRProperty(PercentOr<Length> value)
        : Property(registrationOf<SvgRProperty>()), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.svg.cow().r = _value;
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://svgwg.org/svg2-draft/geometry.html#RxProperty
export struct SvgRxProperty : Property {
    static constexpr auto ID = LonghandId::RX;
    static inline Flags<Options> const FLAGS = {PRESENTATION_ATTRIBUTE};

    static Rc<Property> initial() {
        return makeRc<SvgRxProperty>(Keywords::AUTO);
    }

    static Rc<Property> load(ComputedValues const& c) {
        return makeRc<SvgRxProperty>(c.svg->rx);
    }

    static Res<Rc<Property>> parse(Cursor<Css::Sst>& c) {
        // FIXME: Negative values are invalid and must be ignored.
        return Ok(makeRc<SvgRxProperty>(try$(parseValue<Union<Keywords::Auto, PercentOr<Length>>>(c))));
    }

    struct Registration : LonghandRegistration {
        Registration()
            : LonghandRegistration(Properties::RX, FLAGS) {}

        Res<Rc<Property>> parsePresentationAttribute(Str style) override {
            if (auto property = LonghandRegistration::parsePresentationAttribute(style))
                return property;
            return LonghandRegistration::parsePresentationAttribute(Io::format("{}px", style));
        }
    };

    Union<Keywords::Auto, PercentOr<Length>> _value;

    SvgRxProperty(Union<Keywords::Auto, PercentOr<Length>> value)
        : Property(registrationOf<SvgRxProperty>()), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.svg.cow().rx = _value;
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://svgwg.org/svg2-draft/geometry.html#RyProperty
export struct SvgRyProperty : Property {
    static constexpr auto ID = LonghandId::RY;
    static inline Flags<Options> const FLAGS = {PRESENTATION_ATTRIBUTE};

    static Rc<Property> initial() {
        return makeRc<SvgRyProperty>(Keywords::AUTO);
    }

    static Rc<Property> load(ComputedValues const& c) {
        return makeRc<SvgRyProperty>(c.svg->ry);
    }

    static Res<Rc<Property>> parse(Cursor<Css::Sst>& c) {
        // FIXME: Negative values are invalid and must be ignored.
        return Ok(makeRc<SvgRyProperty>(try$(parseValue<Union<Keywords::Auto, PercentOr<Length>>>(c))));
    }

    struct Registration : LonghandRegistration {
        Registration()
            : LonghandRegistration(Properties::RY, FLAGS) {}

        Res<Rc<Property>> parsePresentationAttribute(Str style) override {
            if (auto property = LonghandRegistration::parsePresentationAttribute(style))
                return property;
            return LonghandRegistration::parsePresentationAttribute(Io::format("{}px", style));
        }
    };

    Union<Keywords::Auto, PercentOr<Length>> _value;

    SvgRyProperty(Union<Keywords::Auto, PercentOr<Length>> value)
        : Property(registrationOf<SvgRyProperty>()), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.svg.cow().ry = _value;
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
    static constexpr auto ID = LonghandId::FILL;
    static inline Flags<Options> const FLAGS = {PRESENTATION_ATTRIBUTE, INHERITED, BULK_INHERITED};

    static Rc<Property> initial() {
        return makeRc<SvgFillProperty>(SvgPaint{Some(Color{Gfx::BLACK})});
    }

    static Rc<Property> load(ComputedValues const& c) {
        return makeRc<SvgFillProperty>(SvgPaint{c.svgPaint->fill});
    }

    static Res<Rc<Property>> parse(Cursor<Css::Sst>& c) {
        return Ok(makeRc<SvgFillProperty>(try$(parseValue<SvgPaint>(c))));
    }

    struct Registration : LonghandRegistration {
        Registration()
            : LonghandRegistration(Properties::FILL, FLAGS) {}
    };

    SvgPaint _value;

    SvgFillProperty(SvgPaint value)
        : Property(registrationOf<SvgFillProperty>()), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.svgPaint.cow().fill = _value;
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
    static constexpr auto ID = LonghandId::D;
    static inline Flags<Options> const FLAGS = {PRESENTATION_ATTRIBUTE};

    static Rc<Property> initial() {
        return makeRc<SvgDProperty>(Union<String, None>{NONE});
    }

    static Rc<Property> load(ComputedValues const& c) {
        return makeRc<SvgDProperty>(c.svg->d);
    }

    static Res<Rc<Property>> parse(Cursor<Css::Sst>& c) {
        eatWhitespace(c);
        if (c.peek() != Css::Sst::FUNC or c.peek().prefix != Css::Token::function("path(")) {
            return Error::invalidData("expected path function");
        }

        auto pathFunc = c.next();
        Cursor<Css::Sst> scanPath{pathFunc.content};
        return Ok(makeRc<SvgDProperty>(try$(parseValue<String>(scanPath))));
    }

    struct Registration : LonghandRegistration {
        Registration()
            : LonghandRegistration(Properties::D, FLAGS) {}

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
    };

    Union<String, None> _value;

    SvgDProperty(Union<String, None> value)
        : Property(registrationOf<SvgDProperty>()), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.svg.cow().d = _value;
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return _value.visit(
            [&](String const& path) -> Res<> {
                // https://drafts.csswg.org/css-shapes-1/#basic-shape-serialization
                try$(w.writeStr("path("s));
                try$(serializeValue(w, path));
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
    static constexpr auto ID = LonghandId::VIEWBOX;
    static inline Flags<Options> const FLAGS = {PRESENTATION_ATTRIBUTE};

    static Rc<Property> initial() {
        return makeRc<SvgViewBoxProperty>(Opt<SvgViewBox>{NONE});
    }

    static Rc<Property> load(ComputedValues const& c) {
        return makeRc<SvgViewBoxProperty>(c.svg->viewBox);
    }

    static Res<Rc<Property>> parse(Cursor<Css::Sst>& c) {
        SvgViewBox viewBox;

        viewBox.minX = try$(parseValue<Number>(c));

        c.skip(Css::Token::COMMA);
        viewBox.minY = try$(parseValue<Number>(c));

        c.skip(Css::Token::COMMA);
        viewBox.width = try$(parseValue<Number>(c));

        c.skip(Css::Token::COMMA);
        viewBox.height = try$(parseValue<Number>(c));

        return Ok(makeRc<SvgViewBoxProperty>(Some(std::move(viewBox))));
    }

    struct Registration : LonghandRegistration {
        Registration()
            : LonghandRegistration(Properties::VIEWBOX, FLAGS) {}
    };

    Opt<SvgViewBox> _value;

    SvgViewBoxProperty(Opt<SvgViewBox> value)
        : Property(registrationOf<SvgViewBoxProperty>()), _value(std::move(value)) {}

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
    static constexpr auto ID = LonghandId::STROKE;
    static inline Flags<Options> const FLAGS = {PRESENTATION_ATTRIBUTE, INHERITED, BULK_INHERITED};

    static Rc<Property> initial() {
        return makeRc<SvgStrokeProperty>(SvgPaint{NONE});
    }

    static Rc<Property> load(ComputedValues const& c) {
        return makeRc<SvgStrokeProperty>(SvgPaint{c.svgPaint->stroke});
    }

    static Res<Rc<Property>> parse(Cursor<Css::Sst>& c) {
        return Ok(makeRc<SvgStrokeProperty>(try$(parseValue<SvgPaint>(c))));
    }

    struct Registration : LonghandRegistration {
        Registration()
            : LonghandRegistration(Properties::STROKE, FLAGS) {}
    };

    SvgPaint _value;

    SvgStrokeProperty(SvgPaint value)
        : Property(registrationOf<SvgStrokeProperty>()), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.svgPaint.cow().stroke = _value;
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
    static constexpr auto ID = LonghandId::STROKE_OPACITY;
    static inline Flags<Options> const FLAGS = {INHERITED, BULK_INHERITED};

    static Rc<Property> initial() {
        return makeRc<SvgStrokeOpacityProperty>(Number{1});
    }

    static Rc<Property> load(ComputedValues const& c) {
        return makeRc<SvgStrokeOpacityProperty>(c.svgPaint->strokeOpacity);
    }

    static Res<Rc<Property>> parse(Cursor<Css::Sst>& c) {
        auto maybePercent = parseValue<Percent>(c);
        if (maybePercent) {
            return Ok(makeRc<SvgStrokeOpacityProperty>(maybePercent.expect().value() / 100));
        } else {
            return Ok(makeRc<SvgStrokeOpacityProperty>(try$(parseValue<Number>(c))));
        }
    }

    struct Registration : LonghandRegistration {
        Registration()
            : LonghandRegistration(Properties::STROKE_OPACITY, FLAGS) {}
    };

    Number _value;

    SvgStrokeOpacityProperty(Number value)
        : Property(registrationOf<SvgStrokeOpacityProperty>()), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.svgPaint.cow().strokeOpacity = _value;
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://svgwg.org/svg2-draft/painting.html#FillOpacity
export struct FillOpacityProperty : Property {
    static constexpr auto ID = LonghandId::FILL_OPACITY;
    static inline Flags<Options> const FLAGS = {PRESENTATION_ATTRIBUTE, INHERITED, BULK_INHERITED};

    static Rc<Property> initial() {
        return makeRc<FillOpacityProperty>(Number{1});
    }

    static Rc<Property> load(ComputedValues const& c) {
        return makeRc<FillOpacityProperty>(c.svgPaint->fillOpacity);
    }

    static Res<Rc<Property>> parse(Cursor<Css::Sst>& c) {
        auto maybePercent = parseValue<Percent>(c);
        if (maybePercent) {
            return Ok(makeRc<FillOpacityProperty>(maybePercent.expect().value() / 100));
        } else {
            return Ok(makeRc<FillOpacityProperty>(try$(parseValue<Number>(c))));
        }
    }

    struct Registration : LonghandRegistration {
        Registration()
            : LonghandRegistration(Properties::FILL_OPACITY, FLAGS) {}
    };

    Number _value;

    FillOpacityProperty(Number value)
        : Property(registrationOf<FillOpacityProperty>()), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.svgPaint.cow().fillOpacity = _value;
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://svgwg.org/svg2-draft/painting.html#StrokeWidth
export struct StrokeWidthProperty : Property {
    static constexpr auto ID = LonghandId::STROKE_WIDTH;
    static inline Flags<Options> const FLAGS = {PRESENTATION_ATTRIBUTE, INHERITED, BULK_INHERITED};

    static Rc<Property> initial() {
        return makeRc<StrokeWidthProperty>(PercentOr<Length>{Length{1_au}});
    }

    static Rc<Property> load(ComputedValues const& c) {
        return makeRc<StrokeWidthProperty>(c.svgPaint->strokeWidth);
    }

    // https://svgwg.org/svg2-draft/painting.html#StrokeWidth
    static Res<Rc<Property>> parse(Cursor<Css::Sst>& c) {
        if (auto number = parseValue<Number>(c))
            return Ok(makeRc<StrokeWidthProperty>(PercentOr<Length>{Length::fromPx(number.take())}));
        return Ok(makeRc<StrokeWidthProperty>(try$(parseValue<PercentOr<Length>>(c))));
    }

    struct Registration : LonghandRegistration {
        Registration()
            : LonghandRegistration(Properties::STROKE_WIDTH, FLAGS) {}
    };

    PercentOr<Length> _value;

    StrokeWidthProperty(PercentOr<Length> value)
        : Property(registrationOf<StrokeWidthProperty>()), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.svgPaint.cow().strokeWidth = _value;
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

} // namespace Vaev::Style
