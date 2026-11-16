module;

#include <karm/macros>

export module Vaev.Engine:props.overflow;

import Karm.Core;
import :props.base;
import :css.parser;
import :style.computed;

using namespace Karm;

namespace Vaev::Style {

// MARK: Overflow --------------------------------------------------------------

// https://www.w3.org/TR/css-overflow/#overflow-control
export struct OverflowXProperty : Property {
    static constexpr auto ID = LonghandId::OVERFLOW_X;

    static Rc<Property> initial() {
        return makeRc<OverflowXProperty>(Overflow::VISIBLE);
    }

    static Rc<Property> load(ComputedValues const& c) {
        return makeRc<OverflowXProperty>(c.overflows.x);
    }

    static Res<Rc<Property>> parse(Cursor<Css::Sst>& c, ParsingMode) {
        return Ok(makeRc<OverflowXProperty>(try$(parseValue<Overflow>(c))));
    }

    Overflow _value;

    OverflowXProperty(Overflow value)
        : Property(ID), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.overflows.x = _value;
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/css-overflow/#overflow-control
export struct OverflowYProperty : Property {
    static constexpr auto ID = LonghandId::OVERFLOW_Y;

    static Rc<Property> initial() {
        return makeRc<OverflowYProperty>(Overflow::VISIBLE);
    }

    static Rc<Property> load(ComputedValues const& c) {
        return makeRc<OverflowYProperty>(c.overflows.y);
    }

    static Res<Rc<Property>> parse(Cursor<Css::Sst>& c, ParsingMode) {
        return Ok(makeRc<OverflowYProperty>(try$(parseValue<Overflow>(c))));
    }

    Overflow _value;

    OverflowYProperty(Overflow value)
        : Property(ID), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.overflows.y = _value;
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/css-overflow/#overflow-block
export struct OverflowBlockProperty : Property {
    static constexpr auto ID = LonghandId::OVERFLOW_BLOCK;

    static Rc<Property> initial() {
        return makeRc<OverflowBlockProperty>(Overflow::VISIBLE);
    }

    static Rc<Property> load(ComputedValues const& c) {
        return makeRc<OverflowBlockProperty>(c.overflows.block);
    }

    static Res<Rc<Property>> parse(Cursor<Css::Sst>& c, ParsingMode) {
        return Ok(makeRc<OverflowBlockProperty>(try$(parseValue<Overflow>(c))));
    }

    Overflow _value;

    OverflowBlockProperty(Overflow value)
        : Property(ID), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.overflows.block = _value;
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/css-overflow/#overflow-inline
export struct OverflowInlineProperty : Property {
    static constexpr auto ID = LonghandId::OVERFLOW_INLINE;

    static Rc<Property> initial() {
        return makeRc<OverflowInlineProperty>(Overflow::VISIBLE);
    }

    static Rc<Property> load(ComputedValues const& c) {
        return makeRc<OverflowInlineProperty>(c.overflows.inline_);
    }

    static Res<Rc<Property>> parse(Cursor<Css::Sst>& c, ParsingMode) {
        return Ok(makeRc<OverflowInlineProperty>(try$(parseValue<Overflow>(c))));
    }

    Overflow _value;

    OverflowInlineProperty(Overflow value)
        : Property(ID), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.overflows.inline_ = _value;
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/css-overflow-3/#propdef-overflow
export struct OverflowShorthand {
    static constexpr auto ID = ShorthandId::OVERFLOW;

    static constexpr Array LONGHANDS = {
        LonghandId::OVERFLOW_X,
        LonghandId::OVERFLOW_Y,
    };

    // <'overflow-x'>{1,2}
    static Res<> parse(Cursor<Css::Sst>& c, Vec<Rc<Property>>& stylesheet, ParsingMode) {
        auto x = try$(parseValue<Overflow>(c));

        // If the second value is omitted, it is set to the same as the first.
        auto y = parseValue<Overflow>(c).unwrapOr(x);

        stylesheet.pushBack(makeRc<OverflowXProperty>(x));
        stylesheet.pushBack(makeRc<OverflowYProperty>(y));
        return Ok();
    }
};

} // namespace Vaev::Style
