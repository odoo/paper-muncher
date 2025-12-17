module;

#include <karm-core/macros.h>

export module Vaev.Engine:props.svg;

import Karm.Core;
import :props.base;
import :css.parser;
import :style.specified;

using namespace Karm;

namespace Vaev::Style {

// MARK: SVG ----------------------------------------------------------------

// https://svgwg.org/svg2-draft/geometry.html#XProperty
export struct SvgXProperty : Property {
    struct Registration : Property::Registration {
        Symbol name() const override {
            return Properties::X;
        }

        Flags<Options> flags() const override {
            return {PRESENTATION_ATTRIBUTE};
        }

        Rc<Property> initial() const override {
            return makeRc<SvgXProperty>(self(), PercentOr<Length>{Length{0_au}});
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<SvgXProperty>(self(), c.svg->x);
        }

        Res<Rc<Property>> parsePresentationAttribute(Str style) override {
            return Property::Registration::parsePresentationAttribute(Io::format("{}px", style));
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<SvgXProperty>(self(), try$(parseValue<PercentOr<Length>>(c))));
        }
    };

    PercentOr<Length> _value;

    SvgXProperty(Rc<Property::Registration> registration, PercentOr<Length> value)
        : Property(registration), _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.svg.cow().x = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://svgwg.org/svg2-draft/geometry.html#YProperty
export struct SvgYProperty : Property {
    struct Registration : Property::Registration {
        Symbol name() const override {
            return Properties::Y;
        }

        Flags<Options> flags() const override {
            return {PRESENTATION_ATTRIBUTE};
        }

        Rc<Property> initial() const override {
            return makeRc<SvgYProperty>(self(), PercentOr<Length>{Length{0_au}});
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<SvgYProperty>(self(), c.svg->y);
        }

        Res<Rc<Property>> parsePresentationAttribute(Str style) override {
            return Property::Registration::parsePresentationAttribute(Io::format("{}px", style));
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<SvgYProperty>(self(), try$(parseValue<PercentOr<Length>>(c))));
        }
    };

    PercentOr<Length> _value;

    SvgYProperty(Rc<Property::Registration> registration, PercentOr<Length> value)
        : Property(registration), _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.svg.cow().y = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://svgwg.org/svg2-draft/geometry.html#CXProperty
export struct SvgCXProperty : Property {
    struct Registration : Property::Registration {
        Symbol name() const override {
            return Properties::CX;
        }

        Flags<Options> flags() const override {
            return {PRESENTATION_ATTRIBUTE};
        }

        Rc<Property> initial() const override {
            return makeRc<SvgCXProperty>(self(), PercentOr<Length>{Length{0_au}});
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<SvgCXProperty>(self(), c.svg->cx);
        }

        Res<Rc<Property>> parsePresentationAttribute(Str style) override {
            return Property::Registration::parsePresentationAttribute(Io::format("{}px", style));
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<SvgCXProperty>(self(), try$(parseValue<PercentOr<Length>>(c))));
        }
    };

    PercentOr<Length> _value;

    SvgCXProperty(Rc<Property::Registration> registration, PercentOr<Length> value)
        : Property(registration), _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.svg.cow().cx = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://svgwg.org/svg2-draft/geometry.html#CYProperty
export struct SvgCYProperty : Property {
    struct Registration : Property::Registration {
        Symbol name() const override {
            return Properties::CY;
        }

        Flags<Options> flags() const override {
            return {PRESENTATION_ATTRIBUTE};
        }

        Rc<Property> initial() const override {
            return makeRc<SvgCYProperty>(self(), PercentOr<Length>{Length{0_au}});
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<SvgCYProperty>(self(), c.svg->cy);
        }

        Res<Rc<Property>> parsePresentationAttribute(Str style) override {
            return Property::Registration::parsePresentationAttribute(Io::format("{}px", style));
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<SvgCYProperty>(self(), try$(parseValue<PercentOr<Length>>(c))));
        }
    };

    PercentOr<Length> _value;

    SvgCYProperty(Rc<Property::Registration> registration, PercentOr<Length> value)
        : Property(registration), _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.svg.cow().cy = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://svgwg.org/svg2-draft/geometry.html#RProperty
export struct SvgRProperty : Property {
    struct Registration : Property::Registration {
        Symbol name() const override {
            return Properties::R;
        }

        Flags<Options> flags() const override {
            return {PRESENTATION_ATTRIBUTE};
        }

        Rc<Property> initial() const override {
            return makeRc<SvgRProperty>(self(), PercentOr<Length>{Length{0_au}});
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<SvgRProperty>(self(), c.svg->r);
        }

        Res<Rc<Property>> parsePresentationAttribute(Str style) override {
            return Property::Registration::parsePresentationAttribute(Io::format("{}px", style));
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<SvgRProperty>(self(), try$(parseValue<PercentOr<Length>>(c))));
        }
    };

    PercentOr<Length> _value;

    SvgRProperty(Rc<Property::Registration> registration, PercentOr<Length> value)
        : Property(registration), _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.svg.cow().r = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://svgwg.org/svg2-draft/painting.html#FillProperty
export struct SvgFillProperty : Property {
    struct Registration : Property::Registration {
        Symbol name() const override {
            return Properties::FILL;
        }

        Flags<Options> flags() const override {
            return {PRESENTATION_ATTRIBUTE};
        }

        Rc<Property> initial() const override {
            return makeRc<SvgFillProperty>(self(), Paint{Color{Gfx::BLACK}});
        }

        void inherit(SpecifiedValues const& parent, SpecifiedValues& child) override {
            // NOTE: We bail out early if the parent has the default SVG values.
            //       This avoids needlessly writing into the child's style, which would
            //       trigger a copy-on-write of the whole property group for nothing.
            if (parent.svg.defaulted())
                return;
            child.svg.cow().fill = parent.svg->fill;
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<SvgFillProperty>(self(), c.svg->fill);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<SvgFillProperty>(self(), try$(parseValue<Paint>(c))));
        }
    };

    Paint _value;

    SvgFillProperty(Rc<Property::Registration> registration, Paint value)
        : Property(registration), _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.svg.cow().fill = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://svgwg.org/svg2-draft/paths.html#TheDProperty
export struct SvgDProperty : Property {
    struct Registration : Property::Registration {
        Symbol name() const override {
            return Properties::D;
        }

        Flags<Options> flags() const override {
            return {PRESENTATION_ATTRIBUTE};
        }

        Rc<Property> initial() const override {
            return makeRc<SvgDProperty>(self(), Union<String, None>{NONE});
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
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
            auto [sst, _] = Css::consumeDeclarationValue(lex);
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

    void apply(SpecifiedValues& c) const override {
        c.svg.cow().d = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://svgwg.org/svg2-draft/coords.html#ViewBoxAttribute
export struct SvgViewBoxProperty : Property {
    struct Registration : Property::Registration {
        Symbol name() const override {
            return Properties::VIEWBOX;
        }

        Flags<Options> flags() const override {
            return {PRESENTATION_ATTRIBUTE};
        }

        Rc<Property> initial() const override {
            return makeRc<SvgViewBoxProperty>(self(), Opt<ViewBox>{NONE});
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<SvgViewBoxProperty>(self(), c.svg->viewBox);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            ViewBox viewBox;

            viewBox.minX = try$(parseValue<Number>(c));

            c.skip(Css::Token::comma());
            viewBox.minY = try$(parseValue<Number>(c));

            c.skip(Css::Token::comma());
            viewBox.width = try$(parseValue<Number>(c));

            c.skip(Css::Token::comma());
            viewBox.height = try$(parseValue<Number>(c));

            return Ok(makeRc<SvgViewBoxProperty>(self(), Opt<ViewBox>{std::move(viewBox)}));
        }
    };

    Opt<ViewBox> _value;

    SvgViewBoxProperty(Rc<Property::Registration> registration, Opt<ViewBox> value)
        : Property(registration), _value(std::move(value)) {}

    void apply(SpecifiedValues& c) const override {
        c.svg.cow().viewBox = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://svgwg.org/svg2-draft/painting.html#SpecifyingStrokePaint
export struct SvgStrokeProperty : Property {
    struct Registration : Property::Registration {
        Symbol name() const override {
            return Properties::STROKE;
        }

        Flags<Options> flags() const override {
            return {PRESENTATION_ATTRIBUTE};
        }

        Rc<Property> initial() const override {
            return makeRc<SvgStrokeProperty>(self(), Paint{NONE});
        }

        void inherit(SpecifiedValues const& parent, SpecifiedValues& child) override {
            // NOTE: We bail out early if the parent has the default SVG values.
            //       This avoids needlessly writing into the child's style, which would
            //       trigger a copy-on-write of the whole property group for nothing.
            if (parent.svg.defaulted())
                return;
            child.svg.cow().fillOpacity = parent.svg->fillOpacity;
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<SvgStrokeProperty>(self(), c.svg->stroke);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<SvgStrokeProperty>(self(), try$(parseValue<Paint>(c))));
        }
    };

    Paint _value;

    SvgStrokeProperty(Rc<Property::Registration> registration, Paint value)
        : Property(registration), _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.svg.cow().stroke = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://svgwg.org/svg2-draft/painting.html#StrokeOpacity
export struct SvgStrokeOpacityProperty : Property {
    struct Registration : Property::Registration {
        Symbol name() const override {
            return Properties::STROKE_OPACITY;
        }

        Rc<Property> initial() const override {
            return makeRc<SvgStrokeOpacityProperty>(self(), Number{1});
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<SvgStrokeOpacityProperty>(self(), c.svg->strokeOpacity);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            auto maybePercent = parseValue<Percent>(c);
            if (maybePercent) {
                return Ok(makeRc<SvgStrokeOpacityProperty>(self(), maybePercent.unwrap().value() / 100));
            } else {
                return Ok(makeRc<SvgStrokeOpacityProperty>(self(), try$(parseValue<Number>(c))));
            }
        }
    };

    Number _value;

    SvgStrokeOpacityProperty(Rc<Property::Registration> registration, Number value)
        : Property(registration), _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.svg.cow().strokeOpacity = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://svgwg.org/svg2-draft/painting.html#FillOpacity
export struct FillOpacityProperty : Property {
    struct Registration : Property::Registration {
        Symbol name() const override {
            return Properties::FILL_OPACITY;
        }

        Flags<Options> flags() const override {
            return {PRESENTATION_ATTRIBUTE};
        }

        Rc<Property> initial() const override {
            return makeRc<FillOpacityProperty>(self(), Number{1});
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<FillOpacityProperty>(self(), c.svg->fillOpacity);
        }

        void inherit(SpecifiedValues const& parent, SpecifiedValues& child) override {
            // NOTE: We bail out early if the parent has the default SVG values.
            //       This avoids needlessly writing into the child's style, which would
            //       trigger a copy-on-write of the whole property group for nothing.
            if (parent.svg.defaulted())
                return;
            child.svg.cow().fillOpacity = parent.svg->fillOpacity;
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            auto maybePercent = parseValue<Percent>(c);
            if (maybePercent) {
                return Ok(makeRc<FillOpacityProperty>(self(), maybePercent.unwrap().value() / 100));
            } else {
                return Ok(makeRc<FillOpacityProperty>(self(), try$(parseValue<Number>(c))));
            }
        }
    };

    Number _value;

    FillOpacityProperty(Rc<Property::Registration> registration, Number value)
        : Property(registration), _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.svg.cow().fillOpacity = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://svgwg.org/svg2-draft/painting.html#StrokeWidth
export struct StrokeWidthProperty : Property {
    struct Registration : Property::Registration {
        Symbol name() const override {
            return Properties::STROKE_WIDTH;
        }

        Flags<Options> flags() const override {
            return {PRESENTATION_ATTRIBUTE};
        }

        Rc<Property> initial() const override {
            return makeRc<StrokeWidthProperty>(self(), PercentOr<Length>{Length{1_au}});
        }

        void inherit(SpecifiedValues const& parent, SpecifiedValues& child) override {
            // NOTE: We bail out early if the parent has the default SVG values.
            //       This avoids needlessly writing into the child's style, which would
            //       trigger a copy-on-write of the whole property group for nothing.
            if (parent.svg.defaulted())
                return;
            child.svg.cow().strokeWidth = parent.svg->strokeWidth;
        }

        Res<Rc<Property>> parsePresentationAttribute(Str style) override {
            return Property::Registration::parsePresentationAttribute(Io::format("{}px", style));
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<StrokeWidthProperty>(self(), c.svg->strokeWidth);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<StrokeWidthProperty>(self(), try$(parseValue<PercentOr<Length>>(c))));
        }
    };

    PercentOr<Length> _value;

    StrokeWidthProperty(Rc<Property::Registration> registration, PercentOr<Length> value)
        : Property(registration), _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.svg.cow().strokeWidth = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

} // namespace Vaev::Style
