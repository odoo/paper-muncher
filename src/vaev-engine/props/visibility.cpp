module;

#include <karm-core/macros.h>

export module Vaev.Engine:props.visibility;

import Karm.Core;
import :props.base;
import :css.parser;
import :style.specified;

using namespace Karm;

namespace Vaev::Style {

// https://drafts.csswg.org/css-display/#visibility
export struct VisibilityProperty : Property {
    struct Registration : Property::Registration {
        Symbol name() const override {
            return Properties::VISIBILITY;
        }

        Rc<Property> initial() const override {
            return makeRc<VisibilityProperty>(self(), Visibility::VISIBLE);
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<VisibilityProperty>(self(), c.visibility);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            Visibility value;
            if (c.skip(Css::Token::ident("visible"))) {
                value = Visibility::VISIBLE;
            } else if (c.skip(Css::Token::ident("hidden"))) {
                value = Visibility::HIDDEN;
            } else if (c.skip(Css::Token::ident("collapse"))) {
                value = Visibility::COLLAPSE;
            } else {
                return Error::invalidData("expected visibility");
            }

            return Ok(makeRc<VisibilityProperty>(self(), value));
        }
    };

    Visibility _value;

    VisibilityProperty(Rc<Property::Registration> registration, Visibility value)
        : Property(registration), _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.visibility = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/css-color-4/#propdef-opacity
export struct OpacityProperty : Property {
    struct Registration : Property::Registration {
        Symbol name() const override {
            return Properties::OPACITY;
        }

        Rc<Property> initial() const override {
            return makeRc<OpacityProperty>(self(), Number{1});
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<OpacityProperty>(self(), c.opacity);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            auto maybePercent = parseValue<Percent>(c);
            if (maybePercent) {
                return Ok(makeRc<OpacityProperty>(self(), maybePercent.unwrap().value() / 100));
            }
            return Ok(makeRc<OpacityProperty>(self(), try$(parseValue<Number>(c))));
        }
    };

    Number _value;

    OpacityProperty(Rc<Property::Registration> registration, Number value)
        : Property(registration), _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.opacity = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://drafts.fxtf.org/css-masking/#the-clip-path
export struct ClipPathProperty : Property {
    using Value = Union</* Url, */ BasicShape, Keywords::None>;

    struct Registration : Property::Registration {
        Symbol name() const override {
            return Properties::CLIP_PATH;
        }

        Rc<Property> initial() const override {
            return makeRc<ClipPathProperty>(self(), Keywords::NONE);
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            if (c.clip->has())
                return makeRc<ClipPathProperty>(self(), c.clip->unwrap());
            return makeRc<ClipPathProperty>(self(), Keywords::NONE);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<ClipPathProperty>(self(), try$(parseValue<Value>(c))));
        }
    };

    Value _value;

    ClipPathProperty(Rc<Property::Registration> registration, Value value)
        : Property(registration), _value(value) {}

    void apply(SpecifiedValues& c) const override {
        if (auto clipShape = _value.is<BasicShape>())
            c.clip.cow() = *clipShape;
        else
            c.clip.cow() = NONE;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

} // namespace Vaev::Style
