module;

#include <karm/macros>

export module Vaev.Engine:props.transform;

import Karm.Core;
import :props.base;
import :css.parser;
import :style.specified;

using namespace Karm;

namespace Vaev::Style {

// MARK: Transform -------------------------------------------------------------
// https://drafts.csswg.org/css-transforms/#transform-property

// https://drafts.csswg.org/css-transforms/#transform-origin-property
export struct TransformOriginProperty : Property {
    struct Registration : Property::Registration {
        Symbol name() const override {
            return Properties::TRANSFORM_ORIGIN;
        }

        Flags<Options> flags() const override {
            return {PRESENTATION_ATTRIBUTE};
        }

        Rc<Property> initial() const override {
            return makeRc<TransformOriginProperty>(
                self(),
                TransformOrigin{
                    .xOffset = CalcValue<PercentOr<Length>>{Percent{50}},
                    .yOffset = CalcValue<PercentOr<Length>>{Percent{50}},
                }
            );
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<TransformOriginProperty>(self(), c.transform->origin);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<TransformOriginProperty>(self(), try$(parseValue<TransformOrigin>(c))));
        }
    };

    TransformOrigin _value;

    TransformOriginProperty(Rc<Property::Registration> registration, TransformOrigin value)
        : Property(registration), _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.transform.cow().origin = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://drafts.csswg.org/css-transforms/#transform-box
export struct TransformBoxProperty : Property {
    struct Registration : Property::Registration {
        Symbol name() const override {
            return Properties::TRANSFORM_BOX;
        }

        Rc<Property> initial() const override {
            return makeRc<TransformBoxProperty>(self(), TransformBox{Keywords::VIEW_BOX});
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<TransformBoxProperty>(self(), c.transform->box);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<TransformBoxProperty>(self(), try$(parseValue<TransformBox>(c))));
        }
    };

    TransformBox _value;

    TransformBoxProperty(Rc<Property::Registration> registration, TransformBox value)
        : Property(registration), _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.transform.cow().box = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://drafts.csswg.org/css-transforms/#propdef-transform
export struct TransformProperty : Property {
    struct Registration : Property::Registration {
        Symbol name() const override {
            return Properties::TRANSFORM;
        }

        Flags<Options> flags() const override {
            return {PRESENTATION_ATTRIBUTE};
        }

        Rc<Property> initial() const override {
            return makeRc<TransformProperty>(self(), Transform{Keywords::NONE});
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<TransformProperty>(self(), c.transform->transform);
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
            auto [sst, _] = Css::consumeDeclarationValue(lex);
            _fixTransformNumberToDimensions(sst);
            Cursor<Css::Sst> content{sst};
            return parse(content);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<TransformProperty>(self(), try$(parseValue<Transform>(c))));
        }
    };

    Transform _value;

    TransformProperty(Rc<Property::Registration> registration, Transform value)
        : Property(registration), _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.transform.cow().transform = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

} // namespace Vaev::Style
