module;

#include <karm/macros>

export module Vaev.Engine:props.transform;

import Karm.Core;
import :props.base;
import :css.parser;
import :style.computed;
import :style.values.keywords;

using namespace Karm;
using namespace Karm::Literals;

namespace Vaev::Style {

// MARK: Transform -------------------------------------------------------------
// https://drafts.csswg.org/css-transforms/#transform-property

// https://drafts.csswg.org/css-transforms/#propdef-transform-origin
export struct TransformOriginProperty : Property {
    using Value = Css::TransformOrigin;

    struct Registration : Property::Registration {
        Registration()
            : Property::Registration(Properties::TRANSFORM_ORIGIN, PRESENTATION_ATTRIBUTE) {}

        InitialValue initialValue() const override {
            return "50% 50%"s;
        }

        // https://compat.spec.whatwg.org/#css-simple-aliases
        SmallVec<Symbol, 1> legacyNameAlias() const override {
            return {"-webkit-transform-origin"_sym};
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<TransformOriginProperty>(
                self(),
                Value{
                    .xOffset = specifyValue<Css::LengthPercentage>(c.transform->origin.x),
                    .yOffset = specifyValue<Css::LengthPercentage>(c.transform->origin.y),
                }
            );
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<TransformOriginProperty>(self(), try$(parseValue<Value>(c))));
        }
    };

    Value _value;

    TransformOriginProperty(Rc<Property::Registration> registration, Value value)
        : Property(registration), _value(std::move(value)) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.transform.cow().origin = computeValue(_value, cx);
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return Css::serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://drafts.csswg.org/css-transforms/#propdef-transform-box
export struct TransformBoxProperty : Property {
    using Value = Css::TransformBox;

    struct Registration : Property::Registration {
        Registration() : Property::Registration(Properties::TRANSFORM_BOX) {}

        InitialValue initialValue() const override {
            return "view-box"s;
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<TransformBoxProperty>(self(), c.transform->box);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<TransformBoxProperty>(self(), try$(parseValue<Value>(c))));
        }
    };

    Value _value;

    TransformBoxProperty(Rc<Property::Registration> registration, Value value)
        : Property(registration), _value(std::move(value)) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.transform.cow().box = _value;
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return Css::serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://drafts.csswg.org/css-transforms/#propdef-transform
export struct TransformProperty : Property {
    using Value = Union<Keywords::None, Css::TransformList>;

    struct Registration : Property::Registration {
        Registration()
            : Property::Registration(Properties::TRANSFORM, PRESENTATION_ATTRIBUTE) {}

        InitialValue initialValue() const override {
            return "none"s;
        }

        // https://compat.spec.whatwg.org/#css-simple-aliases
        SmallVec<Symbol, 1> legacyNameAlias() const override {
            return {"-webkit-transform"_sym};
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<TransformProperty>(self(), specifyValue<Value>(c.transform->transform));
        }

        // https://www.w3.org/TR/SVG11/coords.html#TransformAttribute
        // rotate(<rotate-angle> <cx> <cy>) is equivalent to
        // translate(<cx>, <cy>) rotate(<rotate-angle>) translate(-<cx>, -<cy>).
        static Opt<Css::Content> _expandSvgRotate(Css::Sst const& c) {
            if (c.prefix != Css::Token::function("rotate("))
                return NONE;

            Cursor<Css::Sst> content = c.content;
            eatWhitespace(content);

            auto angle = parseValue<f64>(content);
            if (not angle)
                return NONE;

            skipOmmitableComma(content);

            auto cx = parseValue<f64>(content);
            if (not cx)
                return NONE;

            skipOmmitableComma(content);

            auto cy = parseValue<f64>(content);
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

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            if (c.ended())
                return Error::invalidData("unexpected end of input");

            if (c.skip(Css::Token::ident("none")))
                return Ok(makeRc<TransformProperty>(self(), Value{Keywords::NONE}));

            Css::TransformList functions;
            while (not c.ended() and c->type == Css::Sst::FUNC) {
                functions.pushBack(try$(parseValue<Css::TransformFunction>(c)));
                eatWhitespace(c);
            }

            return Ok(makeRc<TransformProperty>(self(), Value{std::move(functions)}));
        }
    };

    Value _value;

    TransformProperty(Rc<Property::Registration> registration, Value value)
        : Property(registration), _value(std::move(value)) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.transform.cow().transform = computeValue(_value, cx);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

} // namespace Vaev::Style
