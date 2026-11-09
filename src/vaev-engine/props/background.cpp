module;

#include <karm/macros>

export module Vaev.Engine:props.background;

import Karm.Core;
import :props.base;
import :css.parser;
import :style.computed;
import :css.values.base;
import :css.values.primitives;
import :css.values.backgrounds;
import :style.values.backgrounds;
import :style.values.keywords;

using namespace Karm;

namespace Vaev::Style {

// MARK: Background Color ------------------------------------------------------

// https://www.w3.org/TR/CSS22/colors.html#propdef-background-color
export struct BackgroundColorProperty : Property {
    struct Registration : Property::Registration {
        Registration() : Property::Registration(Properties::BACKGROUND_COLOR) {}

        Rc<Property> initial() const override {
            return makeRc<BackgroundColorProperty>(self(), Gfx::ALPHA);
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<BackgroundColorProperty>(self(), toSpecified(c.backgrounds->color));
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<BackgroundColorProperty>(self(), try$(parseValue<Css::Color>(c))));
        }
    };

    Css::Color _value;

    BackgroundColorProperty(Rc<Property::Registration> registration, Css::Color value)
        : Property(registration), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.backgrounds.cow().color = toComputed(_value, c.color);
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// MARK: Background Image ------------------------------------------------------

// https://www.w3.org/TR/CSS22/colors.html#propdef-background-attachment
export struct BackgroundAttachmentProperty : Property {
    struct Registration : Property::Registration {
        Registration() : Property::Registration(Properties::BACKGROUND_ATTACHMENT) {}

        Rc<Property> initial() const override {
            return makeRc<BackgroundAttachmentProperty>(self(), Vec<Css::BackgroundAttachment>{Keywords::SCROLL});
        }

        Rc<Property> load(ComputedValues const& c) const override {
            Vec<Css::BackgroundAttachment> layers;
            for (auto const& l : c.backgrounds->layers)
                layers.pushBack(l.attachment);
            return makeRc<BackgroundAttachmentProperty>(self(), std::move(layers));
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {

            Vec<Css::BackgroundAttachment> value;
            while (not c.ended()) {
                value.pushBack(try$(parseValue<Css::BackgroundAttachment>(c)));
                eatWhitespace(c);
            }

            return Ok(makeRc<BackgroundAttachmentProperty>(self(), std::move(value)));
        }
    };

    Vec<Css::BackgroundAttachment> _value;

    BackgroundAttachmentProperty(Rc<Property::Registration> registration, Vec<Css::BackgroundAttachment> value)
        : Property(registration), _value(std::move(value)) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
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
        Registration() : Property::Registration(Properties::BACKGROUND_IMAGE) {}

        Rc<Property> initial() const override {
            return makeRc<BackgroundImageProperty>(self(), Vec<Css::Image>{});
        }

        Rc<Property> load(ComputedValues const&) const override {
            return makeRc<BackgroundImageProperty>(self(), Vec<Css::Image>{});
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>&) const override {
            // TODO
            return Ok(makeRc<BackgroundImageProperty>(self(), Vec<Css::Image>{}));
        }
    };

    Vec<Css::Image> _value;

    BackgroundImageProperty(Rc<Property::Registration> registration, Vec<Css::Image> value)
        : Property(registration), _value(std::move(value)) {}

    void apply(ComputedValues const&, ComputedValues&, [[maybe_unused]] ComputationContext const& cx) const override {
        // TODO
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/CSS22/colors.html#propdef-background-position
export struct BackgroundPositionProperty : Property {
    struct Registration : Property::Registration {
        Registration() : Property::Registration(Properties::BACKGROUND_POSITION) {}

        Rc<Property> initial() const override {
            return makeRc<BackgroundPositionProperty>(self(), Vec<Css::BackgroundPosition>{});
        }

        Rc<Property> load(ComputedValues const&) const override {
            return makeRc<BackgroundPositionProperty>(self(), Vec<Css::BackgroundPosition>{});
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>&) const override {
            // TODO
            return Ok(makeRc<BackgroundPositionProperty>(self(), Vec<Css::BackgroundPosition>{}));
        }
    };

    Vec<Css::BackgroundPosition> _value;

    BackgroundPositionProperty(Rc<Property::Registration> registration, Vec<Css::BackgroundPosition> value)
        : Property(registration), _value(std::move(value)) {}

    void apply(ComputedValues const&, ComputedValues&, [[maybe_unused]] ComputationContext const& cx) const override {
        // TODO
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/CSS22/colors.html#propdef-background-repeat
export struct BackgroundRepeatProperty : Property {
    struct Registration : Property::Registration {
        Registration() : Property::Registration(Properties::BACKGROUND_REPEAT) {}

        Rc<Property> initial() const override {
            return makeRc<BackgroundRepeatProperty>(self(), Vec<Css::BackgroundRepeat>{Css::BackgroundRepeat::REPEAT});
        }

        Rc<Property> load(ComputedValues const&) const override {
            // TODO
            return makeRc<BackgroundRepeatProperty>(self(), Vec<Css::BackgroundRepeat>{Css::BackgroundRepeat::REPEAT});
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>&) const override {
            // TODO
            return Ok(makeRc<BackgroundRepeatProperty>(self(), Vec<Css::BackgroundRepeat>{}));
        }
    };

    Vec<Css::BackgroundRepeat> _value;

    BackgroundRepeatProperty(Rc<Property::Registration> registration, Vec<Css::BackgroundRepeat> value)
        : Property(registration),
          _value(std::move(value)) {}

    void apply(ComputedValues const&, ComputedValues&, [[maybe_unused]] ComputationContext const& cx) const override {
        // TODO
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/CSS22/colors.html#x10
export struct BackgroundProperty : Property {
    struct Value {
        Css::Color color = Gfx::ALPHA;
        Vec<Css::BackgroundLayer> layers = {};

        void repr(Io::Emit& e) const {
            e("(background");
            e(" color={}", color);
            e(" layers={}", layers);
            e(")");
        }
    };

    struct Registration : Property::Registration {
        Registration()
            : Property::Registration(Properties::BACKGROUND, SHORTHAND) {}

        Rc<Property> initial() const override {
            return makeRc<BackgroundProperty>(self(), Value{Gfx::ALPHA});
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<BackgroundProperty>(self(), [&] {
                Value value{toSpecified(c.backgrounds->color), {}};
                for (auto const& layer : c.backgrounds->layers)
                    value.layers.pushBack(toSpecified(layer));
                return value;
            }());
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            Value value;
            value.color = try$(parseValue<Css::Color>(c));
            return Ok(makeRc<BackgroundProperty>(self(), std::move(value)));
        }
    };

    Value _value;

    BackgroundProperty(Rc<Property::Registration> registration, Value value)
        : Property(registration), _value(std::move(value)) {}

    Longhands expand(RegisteredPropertySet& registry, ComputedValues const&, ComputedValues&) const override {
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
        Registration()
            : Property::Registration(Properties::COLOR, INHERITED) {}

        Rc<Property> initial() const override {
            return makeRc<ColorProperty>(self(), Css::BLACK);
        }

        void inherit(ComputedValues const& parent, ComputedValues& child) const override {
            child.color = parent.color;
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<ColorProperty>(self(), c.color);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<ColorProperty>(self(), try$(parseValue<Css::Color>(c))));
        }
    };

    Css::Color _value;

    ColorProperty(Rc<Property::Registration> registration, Css::Color value)
        : Property(registration), _value(value) {}

    void apply(ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        // In the color property, the used value of currentcolor is the resolved inherited value.
        c.color = toUsed(_value, parent.color);
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

} // namespace Vaev::Style
