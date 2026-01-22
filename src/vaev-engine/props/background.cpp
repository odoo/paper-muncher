module;

#include <karm-core/macros.h>

export module Vaev.Engine:props.background;

import Karm.Core;
import :props.base;
import :css.parser;
import :style.specified;
import :values.base;
import :values.primitives;
import :values.background;

using namespace Karm;

namespace Vaev::Style {

// MARK: Background Color ------------------------------------------------------

// https://www.w3.org/TR/CSS22/colors.html#propdef-background-color
export struct BackgroundColorProperty : Property {
    struct Registration : Property::Registration {
        Symbol name() const override {
            return Properties::BACKGROUND_COLOR;
        }

        Rc<Property> initial() const override {
            return makeRc<BackgroundColorProperty>(self(), TRANSPARENT);
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<BackgroundColorProperty>(self(), c.backgrounds->color);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<BackgroundColorProperty>(self(), try$(parseValue<Color>(c))));
        }
    };

    Color _value;

    BackgroundColorProperty(Rc<Property::Registration> registration, Color value)
        : Property(registration), _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.backgrounds.cow().color = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// MARK: Background Image ------------------------------------------------------

// https://www.w3.org/TR/CSS22/colors.html#propdef-background-attachment
export struct BackgroundAttachmentProperty : Property {
    struct Registration : Property::Registration {
        Symbol name() const override {
            return Properties::BACKGROUND_ATTACHMENT;
        }

        Rc<Property> initial() const override {
            return makeRc<BackgroundAttachmentProperty>(self(), Vec<BackgroundAttachment>{Keywords::SCROLL});
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            Vec<BackgroundAttachment> layers;
            for (auto const& l : c.backgrounds->layers)
                layers.pushBack(l.attachment);
            return makeRc<BackgroundAttachmentProperty>(self(), std::move(layers));
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {

            Vec<BackgroundAttachment> value;
            while (not c.ended()) {
                value.pushBack(try$(parseValue<BackgroundAttachment>(c)));
                eatWhitespace(c);
            }

            return Ok(makeRc<BackgroundAttachmentProperty>(self(), std::move(value)));
        }
    };

    Vec<BackgroundAttachment> _value;

    BackgroundAttachmentProperty(Rc<Property::Registration> registration, Vec<BackgroundAttachment> value)
        : Property(registration), _value(std::move(value)) {}

    void apply(SpecifiedValues& c) const override {
        auto& layers = c.backgrounds.cow().layers;
        layers.resize(max(layers.len(), _value.len()));
        for (usize i = 0; i < _value.len(); ++i)
            layers[i].attachment = _value[i];
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/CSS22/colors.html#propdef-background-image
export struct BackgroundImageProperty : Property {
    struct Registration : Property::Registration {
        Symbol name() const override {
            return Properties::BACKGROUND_IMAGE;
        }

        Rc<Property> initial() const override {
            return makeRc<BackgroundImageProperty>(self(), Vec<Image>{});
        }

        Rc<Property> load(SpecifiedValues const&) const override {
            return makeRc<BackgroundImageProperty>(self(), Vec<Image>{});
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>&) const override {
            // TODO
            return Ok(makeRc<BackgroundImageProperty>(self(), Vec<Image>{}));
        }
    };

    Vec<Image> _value;

    BackgroundImageProperty(Rc<Property::Registration> registration, Vec<Image> value)
        : Property(registration), _value(std::move(value)) {}

    void apply(SpecifiedValues&) const override {
        // TODO
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/CSS22/colors.html#propdef-background-position
export struct BackgroundPositionProperty : Property {
    struct Registration : Property::Registration {
        Symbol name() const override {
            return Properties::BACKGROUND_POSITION;
        }

        Rc<Property> initial() const override {
            return makeRc<BackgroundPositionProperty>(self(), Vec<BackgroundPosition>{});
        }

        Rc<Property> load(SpecifiedValues const&) const override {
            return makeRc<BackgroundPositionProperty>(self(), Vec<BackgroundPosition>{});
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>&) const override {
            // TODO
            return Ok(makeRc<BackgroundPositionProperty>(self(), Vec<BackgroundPosition>{}));
        }
    };

    Vec<BackgroundPosition> _value;

    BackgroundPositionProperty(Rc<Property::Registration> registration, Vec<BackgroundPosition> value)
        : Property(registration), _value(std::move(value)) {}

    void apply(SpecifiedValues&) const override {
        // TODO
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/CSS22/colors.html#propdef-background-repeat
export struct BackgroundRepeatProperty : Property {
    struct Registration : Property::Registration {
        Symbol name() const override {
            return Properties::BACKGROUND_REPEAT;
        }

        Rc<Property> initial() const override {
            return makeRc<BackgroundRepeatProperty>(self(), Vec<BackgroundRepeat>{BackgroundRepeat::REPEAT});
        }

        Rc<Property> load(SpecifiedValues const&) const override {
            return makeRc<BackgroundRepeatProperty>(self(), Vec<BackgroundRepeat>{});
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>&) const override {
            // TODO
            return Ok(makeRc<BackgroundRepeatProperty>(self(), Vec<BackgroundRepeat>{}));
        }
    };

    Vec<BackgroundRepeat> _value;

    BackgroundRepeatProperty(Rc<Property::Registration> registration, Vec<BackgroundRepeat> value)
        : Property(registration),
          _value(std::move(value)) {}

    void apply(SpecifiedValues&) const override {
        // TODO
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/CSS22/colors.html#x10
export struct BackgroundProperty : Property {
    struct Registration : Property::Registration {
        Symbol name() const override {
            return Properties::BACKGROUND;
        }

        Flags<Options> flags() const override {
            return {SHORTHAND_PROPERTY};
        }

        Rc<Property> initial() const override {
            return makeRc<BackgroundProperty>(self(), BackgroundProps{TRANSPARENT});
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<BackgroundProperty>(self(), *c.backgrounds);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            BackgroundProps value;
            value.color = try$(parseValue<Color>(c));
            return Ok(makeRc<BackgroundProperty>(self(), std::move(value)));
        }
    };

    BackgroundProps _value;

    BackgroundProperty(Rc<Property::Registration> registration, BackgroundProps value)
        : Property(registration), _value(std::move(value)) {}

    Vec<Rc<Property>> expandShorthand(PropertyRegistry& registry, SpecifiedValues const&, SpecifiedValues&) const override {
        return {
            makeRc<BackgroundColorProperty>(
                registry.resolveRegistration(Properties::BACKGROUND_COLOR, {}).take(),
                _value.color
            ),
        };
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/CSS22/colors.html#propdef-color
export struct ColorProperty : Property {
    struct Registration : Property::Registration {
        Symbol name() const override {
            return Properties::COLOR;
        }

        Flags<Options> flags() const override {
            return {INHERITED};
        }

        Rc<Property> initial() const override {
            return makeRc<ColorProperty>(self(), BLACK);
        }

        void inherit(SpecifiedValues const& parent, SpecifiedValues& child) override {
            child.color = parent.color;
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<ColorProperty>(self(), c.color);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<ColorProperty>(self(), try$(parseValue<Color>(c))));
        }
    };

    Color _value;

    ColorProperty(Rc<Property::Registration> registration, Color value)
        : Property(registration), _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.color = resolve(_value, Gfx::BLACK);
    }

    void apply(SpecifiedValues const& parent, SpecifiedValues& c) const override {
        c.color = resolve(_value, parent.color);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

} // namespace Vaev::Style
