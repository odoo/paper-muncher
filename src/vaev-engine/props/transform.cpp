module;

#include <karm/macros>

export module Vaev.Engine:props.transform;

import Karm.Core;
import :props.base;
import :css.parser;
import :style.computed;

using namespace Karm;
using namespace Karm::Literals;

namespace Vaev::Style {

// MARK: Transform -------------------------------------------------------------
// https://drafts.csswg.org/css-transforms/#transform-property

// https://drafts.csswg.org/css-transforms/#transform-origin-property
export struct TransformOriginProperty : Property {
    static constexpr auto ID = LonghandId::TRANSFORM_ORIGIN;
    static inline Flags<Options> const FLAGS = {PRESENTATION_ATTRIBUTE};
    // https://compat.spec.whatwg.org/#css-simple-aliases
    static constexpr Array LEGACY_NAME_ALIASES = {"-webkit-transform-origin"s};

    static Rc<Property> initial() {
        return makeRc<TransformOriginProperty>(TransformOrigin{
                .xOffset = Calc<PercentOr<Length>>{Percent{50}},
                .yOffset = Calc<PercentOr<Length>>{Percent{50}},
            }
        );
    }

    static Rc<Property> load(ComputedValues const& c) {
        return makeRc<TransformOriginProperty>(c.transform->origin);
    }

    static Res<Rc<Property>> parse(Cursor<Css::Sst>& c) {
        return Ok(makeRc<TransformOriginProperty>(try$(parseValue<TransformOrigin>(c))));
    }

    struct Registration : LonghandRegistration {
        Registration()
            : LonghandRegistration(Properties::TRANSFORM_ORIGIN, FLAGS) {}
    };

    TransformOrigin _value;

    TransformOriginProperty(TransformOrigin value)
        : Property(registrationOf<TransformOriginProperty>()), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.transform.cow().origin = _value;
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://drafts.csswg.org/css-transforms/#transform-box
export struct TransformBoxProperty : Property {
    static constexpr auto ID = LonghandId::TRANSFORM_BOX;

    static Rc<Property> initial() {
        return makeRc<TransformBoxProperty>(TransformBox{Keywords::VIEW_BOX});
    }

    static Rc<Property> load(ComputedValues const& c) {
        return makeRc<TransformBoxProperty>(c.transform->box);
    }

    static Res<Rc<Property>> parse(Cursor<Css::Sst>& c) {
        return Ok(makeRc<TransformBoxProperty>(try$(parseValue<TransformBox>(c))));
    }

    struct Registration : LonghandRegistration {
        Registration() : LonghandRegistration(Properties::TRANSFORM_BOX) {}
    };

    TransformBox _value;

    TransformBoxProperty(TransformBox value)
        : Property(registrationOf<TransformBoxProperty>()), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.transform.cow().box = _value;
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://drafts.csswg.org/css-transforms/#propdef-transform
export struct TransformProperty : Property {
    static constexpr auto ID = LonghandId::TRANSFORM;
    static inline Flags<Options> const FLAGS = {PRESENTATION_ATTRIBUTE};
    // https://compat.spec.whatwg.org/#css-simple-aliases
    static constexpr Array LEGACY_NAME_ALIASES = {"-webkit-transform"s};

    static Rc<Property> initial() {
        return makeRc<TransformProperty>(Transform{Keywords::NONE});
    }

    static Rc<Property> load(ComputedValues const& c) {
        return makeRc<TransformProperty>(c.transform->transform);
    }

    static Res<Rc<Property>> parse(Cursor<Css::Sst>& c) {
        return Ok(makeRc<TransformProperty>(try$(parseValue<Transform>(c))));
    }

    struct Registration : LonghandRegistration {
        Registration()
            : LonghandRegistration(Properties::TRANSFORM, FLAGS) {}

        // https://www.w3.org/TR/SVG11/coords.html#TransformAttribute
        // rotate(<rotate-angle> <cx> <cy>) is equivalent to
        // translate(<cx>, <cy>) rotate(<rotate-angle>) translate(-<cx>, -<cy>).
        static Opt<Css::Content> _expandSvgRotate(Css::Sst const& c) {
            if (c.prefix != Css::Token::function("rotate("))
                return NONE;

            Cursor<Css::Sst> content = c.content;
            eatWhitespace(content);

            auto angle = parseValue<Number>(content);
            if (not angle)
                return NONE;

            skipOmmitableComma(content);

            auto cx = parseValue<Number>(content);
            if (not cx)
                return NONE;

            skipOmmitableComma(content);

            auto cy = parseValue<Number>(content);
            if (not cy)
                return NONE;

            eatWhitespace(content);
            if (not content.ended())
                return NONE;

            auto expanded = Io::format(
                "translate({:.17}, {:.17}) rotate({:.17}) translate({:.17}, {:.17})",
                cx.expect(), cy.expect(), angle.expect(), -cx.expect(), -cy.expect()
            );
            Css::Lexer lex{expanded};
            auto [res, _] = Css::consumeDeclarationValue(lex, Diag::Collector::ignore());
            return Some(std::move(res));
        }

        static void _expandSvgRotate(Css::Content& sst) {
            Css::Content res;
            for (auto& c : sst) {
                if (auto expanded = _expandSvgRotate(c))
                    res.insertMany(res.len(), *expanded);
                else
                    res.pushBack(c);
            }
            sst = std::move(res);
        }

        static void _fixTransformNumberToDimensions(Vec<Css::Sst>& sst) {
            auto appendSuffix = [](String const& a, Str const& b) {
                StringBuilder sb;
                sb.append(a);
                sb.append(b);
                return sb.take();
            };

            for (auto& c : sst) {
                if (c.prefix == Css::Token::function("translate(")) {
                    for (auto& tc : c.content) {
                        if (tc.token.type != Css::Token::NUMBER)
                            continue;

                        tc.token = {Css::Token::DIMENSION, appendSuffix(tc.token.data, "px"s)};
                    }
                } else if (
                    c.prefix == Css::Token::function("rotate(") or
                    c.prefix == Css::Token::function("skewX(") or
                    c.prefix == Css::Token::function("skewY(")
                ) {
                    for (auto& tc : c.content) {
                        if (tc.token.type != Css::Token::NUMBER)
                            continue;

                        tc.token = {Css::Token::DIMENSION, appendSuffix(tc.token.data, "deg"s)};
                    }
                }
            }
        }

        Res<Rc<Property>> parsePresentationAttribute(Str style) override {
            Css::Lexer lex{style};
            auto diags = Diag::Collector::ignore();
            auto [sst, _] = Css::consumeDeclarationValue(lex, diags);
            _expandSvgRotate(sst);
            _fixTransformNumberToDimensions(sst);
            Cursor<Css::Sst> content{sst};
            return parse(content);
        }
    };

    Transform _value;

    TransformProperty(Transform value)
        : Property(registrationOf<TransformProperty>()), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.transform.cow().transform = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

} // namespace Vaev::Style
