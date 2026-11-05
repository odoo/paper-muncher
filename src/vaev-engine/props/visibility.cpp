module;

#include <karm/macros>

export module Vaev.Engine:props.visibility;

import Karm.Core;
import :props.base;
import :css.parser;
import :style.computed;

using namespace Karm;

namespace Vaev::Style {

// https://drafts.csswg.org/css-display/#visibility
export struct VisibilityProperty : Property {
    struct Registration : Property::Registration {
        Registration() : Property::Registration(Properties::VISIBILITY) {}

        Rc<Property> initial() const override {
            return makeRc<VisibilityProperty>(self(), Visibility::VISIBLE);
        }

        Rc<Property> load(ComputedValues const& c) const override {
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

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.visibility = _value;
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/css-color-4/#propdef-opacity
export struct OpacityProperty : Property {
    using Value = Union<Number, Percentage>;

    struct Registration : Property::Registration {
        Registration() : Property::Registration(Properties::OPACITY) {}

        Rc<Property> initial() const override {
            return makeRc<OpacityProperty>(self(), Number::fromCanonical(1));
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<OpacityProperty>(self(), Number::fromCanonical(c.opacity));
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            // NOTE: A literal <percentage> is equivalent to the <number> it maps to,
            //       only math functions keep their percentages.
            if (not c.ended() and c.peek() == Css::Token::PERCENTAGE) {
                auto percentage = try$(parseValue<Percentage>(c));
                return Ok(makeRc<OpacityProperty>(self(), Number::fromCanonical(resolve(percentage).value() / 100.)));
            }

            return Ok(makeRc<OpacityProperty>(self(), try$(parseValue<Value>(c))));
        }
    };

    Union<Number, Percentage> _value;

    OpacityProperty(Rc<Property::Registration> registration, Value const& value)
        : Property(registration), _value(std::move(value)) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        auto specifiedNumber = _value.visit(
            [](Number const& n) {
                return resolve(n);
            },
            [](Percentage const& percent) {
                // Percentages:	map to the range [0,1]
                return resolve(percent, 1);
            }
        );

        // Computed value: specified number, clamped to the range [0,1]
        c.opacity = clamp(specifiedNumber, 0, 1);
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://drafts.csswg.org/css-masking/#the-clip-path
export struct ClipPathProperty : Property {
    using Value = Union</* Url, */ BasicShape, Keywords::None>;

    struct Registration : Property::Registration {
        Registration() : Property::Registration(Properties::CLIP_PATH) {}

        Rc<Property> initial() const override {
            return makeRc<ClipPathProperty>(self(), Keywords::NONE);
        }

        Rc<Property> load(ComputedValues const& c) const override {
            if (auto const& [clip] = *c.clip)
                return makeRc<ClipPathProperty>(self(), toSpecified(clip));
            return makeRc<ClipPathProperty>(self(), Keywords::NONE);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<ClipPathProperty>(self(), try$(parseValue<Value>(c))));
        }
    };

    Value _value;

    ClipPathProperty(Rc<Property::Registration> registration, Value value)
        : Property(registration), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        if (auto clipShape = _value.is<BasicShape>())
            c.clip.cow() = Some(resolve(*clipShape, cx));
        else
            c.clip.cow() = NONE;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

} // namespace Vaev::Style
