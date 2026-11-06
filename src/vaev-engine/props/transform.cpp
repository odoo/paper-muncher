module;

#include <karm/macros>

export module Vaev.Engine:props.transform;

import Karm.Core;
import :props.base;
import :css.parser;
import :style.computed;

using namespace Karm;

namespace Vaev::Style {

// MARK: Transform -------------------------------------------------------------
// https://drafts.csswg.org/css-transforms/#transform-property

// https://drafts.csswg.org/css-transforms/#transform-origin-property
export struct TransformOriginProperty : Property {
    // https://drafts.csswg.org/css-transforms/#transform-origin-property
    struct Value {
        using OneValue = Union<Keywords::Left, Keywords::Center, Keywords::Right, Keywords::Top, Keywords::Bottom, Css::LengthPercentage>;

        using XOffset = Union<Keywords::Left, Keywords::Center, Keywords::Right, Css::LengthPercentage>;
        using YOffset = Union<Keywords::Top, Keywords::Center, Keywords::Bottom, Css::LengthPercentage>;

        XOffset xOffset;
        YOffset yOffset;

        void repr(Io::Emit& e) const {
            e("{} {}", xOffset, yOffset);
        }
    };

    struct Registration : Property::Registration {
        Registration()
            : Property::Registration(Properties::TRANSFORM_ORIGIN, PRESENTATION_ATTRIBUTE) {}

        // https://compat.spec.whatwg.org/#css-simple-aliases
        SmallVec<Symbol, 1> legacyNameAlias() const override {
            return {"-webkit-transform-origin"_sym};
        }

        Rc<Property> initial() const override {
            return makeRc<TransformOriginProperty>(
                self(),
                Value{
                    .xOffset = Css::LengthPercentage::fromPercentage(50),
                    .yOffset = Css::LengthPercentage::fromPercentage(50),
                }
            );
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<TransformOriginProperty>(
                self(),
                Value{
                    .xOffset = toSpecified(c.transform->origin.x),
                    .yOffset = toSpecified(c.transform->origin.y),
                }
            );
        }

        static Res<Value> _twoValues(Cursor<Css::Sst>& c) {
            auto xoffset = try$(parseValue<Value::XOffset>(c));
            if (c.ended())
                return Error::invalidData("unexpected end of input after x-offset");

            auto yoffset = try$(parseValue<Value::YOffset>(c));

            return Ok(Value{
                .xOffset = std::move(xoffset),
                .yOffset = std::move(yoffset),
            });
        }

        static Res<Value> _parse(Cursor<Css::Sst>& c) {
            if (c.ended())
                return Error::invalidData("unexpected end of input");

            {
                auto rollback = c.rollbackPoint();
                auto maybeTwoValues = _twoValues(c);
                if (maybeTwoValues) {
                    rollback.disarm();
                    return maybeTwoValues;
                }
            }

            auto singleValue = try$(parseValue<Value::OneValue>(c));

            return singleValue.visit(
                [&](Keywords::Center) -> Res<Value> {
                    return Ok(Value{
                        .xOffset = Keywords::CENTER,
                        .yOffset = Keywords::CENTER,
                    });
                },

                [&](Keywords::Left) -> Res<Value> {
                    return Ok(Value{
                        .xOffset = Keywords::LEFT,
                        .yOffset = Keywords::CENTER, // default y-offset
                    });
                },
                [&](Keywords::Right) -> Res<Value> {
                    return Ok(Value{
                        .xOffset = Keywords::RIGHT,
                        .yOffset = Keywords::CENTER, // default y-offset
                    });
                },

                [&](Keywords::Top) -> Res<Value> {
                    return Ok(Value{
                        .xOffset = Keywords::CENTER, // default x-offset
                        .yOffset = Keywords::TOP,
                    });
                },
                [&](Keywords::Bottom) -> Res<Value> {
                    return Ok(Value{
                        .xOffset = Keywords::CENTER, // default x-offset
                        .yOffset = Keywords::BOTTOM,
                    });
                },

                [&](Css::LengthPercentage value) -> Res<Value> {
                    return Ok(Value{
                        .xOffset = std::move(value),
                        .yOffset = Keywords::CENTER, // default y-offset
                    });
                }
            );
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<TransformOriginProperty>(self(), try$(_parse(c))));
        }
    };

    Value _value;

    TransformOriginProperty(Rc<Property::Registration> registration, Value value)
        : Property(registration), _value(value) {}

    // NOTE: The computed value is "for <length> the absolute value, otherwise a percentage",
    //       so keywords are turned into percentages.
    static Style::Position _resolve(Value const& value, Css::LengthContext auto const& ctx) {
        return {
            value.xOffset.visit(
                [](Keywords::Left) -> Style::PixelPercentage {
                    return Style::Percent{0};
                },
                [](Keywords::Center) -> Style::PixelPercentage {
                    return Style::Percent{50};
                },
                [](Keywords::Right) -> Style::PixelPercentage {
                    return Style::Percent{100};
                },
                [&](Css::LengthPercentage const& length) -> Style::PixelPercentage {
                    return toComputed(length, ctx);
                }
            ),
            value.yOffset.visit(
                [](Keywords::Top) -> Style::PixelPercentage {
                    return Style::Percent{0};
                },
                [](Keywords::Center) -> Style::PixelPercentage {
                    return Style::Percent{50};
                },
                [](Keywords::Bottom) -> Style::PixelPercentage {
                    return Style::Percent{100};
                },
                [&](Css::LengthPercentage const& length) -> Style::PixelPercentage {
                    return toComputed(length, ctx);
                }
            ),
        };
    }

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.transform.cow().origin = _resolve(_value, cx);
    }

    // https://drafts.csswg.org/cssom/#serialize-a-css-value
    Res<> serialize(Io::TextWriter& w) const override {
        // If either of the above syntactic translations would be less backwards-compatible, do not perform them.
        try$(serializeValue(w, _value.xOffset));
        try$(w.writeRune(' '));
        try$(serializeValue(w, _value.yOffset));

        return Ok();
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://drafts.csswg.org/css-transforms/#transform-box
export struct TransformBoxProperty : Property {
    struct Registration : Property::Registration {
        Registration() : Property::Registration(Properties::TRANSFORM_BOX) {}

        Rc<Property> initial() const override {
            return makeRc<TransformBoxProperty>(self(), Css::TransformBox{Keywords::VIEW_BOX});
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<TransformBoxProperty>(self(), c.transform->box);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<TransformBoxProperty>(self(), try$(parseValue<Css::TransformBox>(c))));
        }
    };

    Css::TransformBox _value;

    TransformBoxProperty(Rc<Property::Registration> registration, Css::TransformBox value)
        : Property(registration), _value(value) {}

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

    // Computed value: as specified, but with lengths made absolute.
    static Style::Transform _resolve(Value const& value, Css::LengthContext auto const& ctx) {
        return value.visit(
            [](Keywords::None) -> Style::Transform {
                return Keywords::NONE;
            },
            [&](Css::TransformList const& list) -> Style::Transform {
                Style::TransformList result;
                for (auto const& function : list)
                    result.pushBack(toComputed(function, ctx));
                return result;
            }
        );
    }

    static Value _toSpecified(Style::Transform const& value) {
        return value.visit(
            [](Keywords::None) -> Value {
                return Keywords::NONE;
            },
            [](Style::TransformList const& list) -> Value {
                Css::TransformList result;
                for (auto const& function : list)
                    result.pushBack(toSpecified(function));
                return result;
            }
        );
    }

    struct Registration : Property::Registration {
        Registration()
            : Property::Registration(Properties::TRANSFORM, PRESENTATION_ATTRIBUTE) {}

        // https://compat.spec.whatwg.org/#css-simple-aliases
        SmallVec<Symbol, 1> legacyNameAlias() const override {
            return {"-webkit-transform"_sym};
        }

        Rc<Property> initial() const override {
            return makeRc<TransformProperty>(self(), Value{Keywords::NONE});
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<TransformProperty>(self(), _toSpecified(c.transform->transform));
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
        : Property(registration), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.transform.cow().transform = _resolve(_value, cx);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

} // namespace Vaev::Style
