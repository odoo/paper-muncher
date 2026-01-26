module;

#include <karm/macros>

export module Vaev.Engine:props.outline;

import Karm.Core;
import :props.base;
import :css.parser;
import :style.specified;

using namespace Karm;

namespace Vaev::Style {

// MARK: Outline --------------------------------------------------------------

// https://drafts.csswg.org/css-ui/#outline-width
export struct OutlineWidthProperty : Property {
    struct Registration : Property::Registration {
        Symbol name() const override {
            return Properties::OUTLINE_WIDTH;
        }

        Rc<Property> initial() const override {
            return makeRc<OutlineWidthProperty>(self(), LineWidth{Keywords::MEDIUM});
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<OutlineWidthProperty>(self(), c.outline->width);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<OutlineWidthProperty>(self(), try$(parseValue<LineWidth>(c))));
        }
    };

    LineWidth _value;

    OutlineWidthProperty(Rc<Property::Registration> registration, LineWidth value)
        : Property(registration), _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.outline.cow().width = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://drafts.csswg.org/css-ui/#outline-style
export struct OutlineStyleProperty : Property {
    using Value = Union<Keywords::Auto, Gfx::BorderStyle>;

    struct Registration : Property::Registration {
        Symbol name() const override {
            return Properties::OUTLINE_STYLE;
        }

        Rc<Property> initial() const override {
            return makeRc<OutlineStyleProperty>(self(), Value{Gfx::BorderStyle::NONE});
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<OutlineStyleProperty>(self(), c.outline->style);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<OutlineStyleProperty>(self(), try$(parseValue<Value>(c))));
        }
    };

    Value _value;

    OutlineStyleProperty(Rc<Property::Registration> registration, Value value)
        : Property(registration), _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.outline.cow().style = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://drafts.csswg.org/css-ui/#outline-color
export struct OutlineColorProperty : Property {
    using Value = Union<Keywords::Auto, Color>;

    struct Registration : Property::Registration {
        Symbol name() const override {
            return Properties::OUTLINE_COLOR;
        }

        Rc<Property> initial() const override {
            return makeRc<OutlineColorProperty>(self(), Value{Keywords::AUTO});
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<OutlineColorProperty>(self(), c.outline->color);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<OutlineColorProperty>(self(), try$(parseValue<Value>(c))));
        }
    };

    Value _value;

    OutlineColorProperty(Rc<Property::Registration> registration, Value value)
        : Property(registration), _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.outline.cow().color = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://drafts.csswg.org/css-ui/#outline-offset
export struct OutlineOffsetProperty : Property {
    struct Registration : Property::Registration {
        Symbol name() const override {
            return Properties::OUTLINE_OFFSET;
        }

        Rc<Property> initial() const override {
            return makeRc<OutlineOffsetProperty>(self(), CalcValue<Length>{0_au});
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<OutlineOffsetProperty>(self(), c.outline->offset);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<OutlineOffsetProperty>(self(), try$(parseValue<CalcValue<Length>>(c))));
        }
    };

    CalcValue<Length> _value;

    OutlineOffsetProperty(Rc<Property::Registration> registration, CalcValue<Length> value)
        : Property(registration), _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.outline.cow().offset = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://drafts.csswg.org/css-ui/#outline
export struct OutlineProperty : Property {
    struct Registration : Property::Registration {
        Symbol name() const override {
            return Properties::OUTLINE;
        }

        Flags<Options> flags() const override {
            return {SHORTHAND_PROPERTY};
        }

        Rc<Property> initial() const override {
            return makeRc<OutlineProperty>(self(), Outline{});
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<OutlineProperty>(self(), *c.outline);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            Outline value;
            bool styleSet = false;
            while (not c.ended()) {
                auto width = parseValue<CalcValue<Length>>(c);
                if (width) {
                    value.width = width.unwrap();
                    continue;
                }

                auto color = parseValue<Color>(c);
                if (color) {
                    value.color = color.unwrap();
                    continue;
                }

                auto style = parseValue<Gfx::BorderStyle>(c);
                if (style) {
                    value.style = style.unwrap();
                    styleSet = true;
                    continue;
                }

                if (c.skip(Css::Token::ident("auto"))) {
                    if (not styleSet)
                        value.style = Keywords::AUTO;
                    value.color = Keywords::AUTO;
                    continue;
                }

                break;
            }

            return Ok(makeRc<OutlineProperty>(self(), value));
        }
    };

    Outline _value;

    OutlineProperty(Rc<Property::Registration> registration, Outline value)
        : Property(registration), _value(value) {}

    Vec<Rc<Property>> expandShorthand(PropertyRegistry& registry, SpecifiedValues const&, SpecifiedValues&) const override {
        return {
            makeRc<OutlineWidthProperty>(registry.resolveRegistration(Properties::OUTLINE_WIDTH, {}).unwrap(), _value.width),
            makeRc<OutlineStyleProperty>(registry.resolveRegistration(Properties::OUTLINE_STYLE, {}).unwrap(), _value.style),
            makeRc<OutlineColorProperty>(registry.resolveRegistration(Properties::OUTLINE_COLOR, {}).unwrap(), _value.color),
        };
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

} // namespace Vaev::Style
