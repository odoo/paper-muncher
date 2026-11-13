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
    static constexpr auto ID = LonghandId::VISIBILITY;

    static Rc<Property> initial() {
        return makeRc<VisibilityProperty>(Visibility::VISIBLE);
    }

    static Rc<Property> load(ComputedValues const& c) {
        return makeRc<VisibilityProperty>(c.visibility);
    }

    static Res<Rc<Property>> parse(Cursor<Css::Sst>& c, ParsingMode) {
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

        return Ok(makeRc<VisibilityProperty>(value));
    }

    struct Registration : LonghandRegistration {
        Registration() : LonghandRegistration(Properties::VISIBILITY) {}
    };

    Visibility _value;

    VisibilityProperty(Visibility value)
        : Property(ID), _value(value) {}

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
    static constexpr auto ID = LonghandId::OPACITY;

    static Rc<Property> initial() {
        return makeRc<OpacityProperty>(Number{1});
    }

    static Rc<Property> load(ComputedValues const& c) {
        return makeRc<OpacityProperty>(c.opacity);
    }

    static Res<Rc<Property>> parse(Cursor<Css::Sst>& c, ParsingMode) {
        auto maybePercent = parseValue<Percent>(c);
        if (maybePercent) {
            return Ok(makeRc<OpacityProperty>(maybePercent.expect().value() / 100));
        }
        return Ok(makeRc<OpacityProperty>(try$(parseValue<Number>(c))));
    }

    struct Registration : LonghandRegistration {
        Registration() : LonghandRegistration(Properties::OPACITY) {}
    };

    Number _value;

    OpacityProperty(Number value)
        : Property(ID), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.opacity = _value;
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
    static constexpr auto ID = LonghandId::CLIP_PATH;

    using Value = Union</* Url, */ BasicShape, Keywords::None>;

    static Rc<Property> initial() {
        return makeRc<ClipPathProperty>(Keywords::NONE);
    }

    static Rc<Property> load(ComputedValues const& c) {
        if (auto const& [clip] = *c.clip)
            return makeRc<ClipPathProperty>(clip);
        return makeRc<ClipPathProperty>(Keywords::NONE);
    }

    static Res<Rc<Property>> parse(Cursor<Css::Sst>& c, ParsingMode) {
        return Ok(makeRc<ClipPathProperty>(try$(parseValue<Value>(c))));
    }

    struct Registration : LonghandRegistration {
        Registration() : LonghandRegistration(Properties::CLIP_PATH) {}
    };

    Value _value;

    ClipPathProperty(Value value)
        : Property(ID), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        if (auto clipShape = _value.is<BasicShape>())
            c.clip.cow() = Some(*clipShape);
        else
            c.clip.cow() = NONE;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

} // namespace Vaev::Style
