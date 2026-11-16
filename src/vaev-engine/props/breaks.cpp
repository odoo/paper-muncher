module;

#include <karm/macros>

export module Vaev.Engine:props.breaks;

import Karm.Core;
import :props.base;
import :css.parser;
import :style.computed;

using namespace Karm;

namespace Vaev::Style {

// MARK: Breaks ----------------------------------------------------------------

// https://www.w3.org/TR/css-break-3/#propdef-break-after
export struct BreakAfterProperty : Property {
    static constexpr auto ID = LonghandId::BREAK_AFTER;

    static Rc<Property> initial() {
        return makeRc<BreakAfterProperty>(BreakBetween::AUTO);
    }

    static Rc<Property> load(ComputedValues const& c) {
        return makeRc<BreakAfterProperty>(c.break_->after);
    }

    static Res<Rc<Property>> parse(Cursor<Css::Sst>& c, ParsingMode) {
        return Ok(makeRc<BreakAfterProperty>(try$(parseValue<BreakBetween>(c))));
    }

    BreakBetween _value;

    BreakAfterProperty(BreakBetween value)
        : Property(ID), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.break_.cow().after = _value;
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/css-break-3/#propdef-break-before
export struct BreakBeforeProperty : Property {
    static constexpr auto ID = LonghandId::BREAK_BEFORE;

    static Rc<Property> initial() {
        return makeRc<BreakBeforeProperty>(BreakBetween::AUTO);
    }

    static Rc<Property> load(ComputedValues const& c) {
        return makeRc<BreakBeforeProperty>(c.break_->before);
    }

    static Res<Rc<Property>> parse(Cursor<Css::Sst>& c, ParsingMode) {
        return Ok(makeRc<BreakBeforeProperty>(try$(parseValue<BreakBetween>(c))));
    }

    BreakBetween _value;

    BreakBeforeProperty(BreakBetween value)
        : Property(ID), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.break_.cow().before = _value;
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/css-break-3/#break-within
export struct BreakInsideProperty : Property {
    static constexpr auto ID = LonghandId::BREAK_INSIDE;

    static Rc<Property> initial() {
        return makeRc<BreakInsideProperty>(BreakInside::AUTO);
    }

    static Rc<Property> load(ComputedValues const& c) {
        return makeRc<BreakInsideProperty>(c.break_->inside);
    }

    static Res<Rc<Property>> parse(Cursor<Css::Sst>& c, ParsingMode) {
        return Ok(makeRc<BreakInsideProperty>(try$(parseValue<BreakInside>(c))));
    }

    BreakInside _value;

    BreakInsideProperty(BreakInside value)
        : Property(ID), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.break_.cow().inside = _value;
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// MARK: Page Break Aliases ----------------------------------------------------

// https://www.w3.org/TR/css-break-3/#page-break-properties
static Res<BreakBetween> _parseLegacyPageBreak(Cursor<Css::Sst>& c) {
    if (c.skip(Css::Token::ident("auto")))
        return Ok(BreakBetween::AUTO);
    if (c.skip(Css::Token::ident("always")))
        return Ok(BreakBetween::PAGE);
    if (c.skip(Css::Token::ident("avoid")))
        return Ok(BreakBetween::AVOID);
    if (c.skip(Css::Token::ident("left")))
        return Ok(BreakBetween::LEFT);
    if (c.skip(Css::Token::ident("right")))
        return Ok(BreakBetween::RIGHT);
    return Error::invalidData("expected auto, always, avoid, left or right");
}

// https://www.w3.org/TR/css-break-3/#page-break-properties
export struct PageBreakAfterShorthand {
    static constexpr auto ID = ShorthandId::PAGE_BREAK_AFTER;

    static constexpr auto LONGHANDS = Array{
        LonghandId::BREAK_AFTER,
    };

    static Res<> parse(Cursor<Css::Sst>& c, Vec<Rc<Property>>& stylesheet, ParsingMode) {
        stylesheet.pushBack(makeRc<BreakAfterProperty>(try$(_parseLegacyPageBreak(c))));
        return Ok();
    }
};

// https://www.w3.org/TR/css-break-3/#page-break-properties
export struct PageBreakBeforeShorthand {
    static constexpr auto ID = ShorthandId::PAGE_BREAK_BEFORE;

    static constexpr auto LONGHANDS = Array{
        LonghandId::BREAK_BEFORE,
    };

    static Res<> parse(Cursor<Css::Sst>& c, Vec<Rc<Property>>& stylesheet, ParsingMode) {
        stylesheet.pushBack(makeRc<BreakBeforeProperty>(try$(_parseLegacyPageBreak(c))));
        return Ok();
    }
};

// https://www.w3.org/TR/css-break-3/#page-break-properties
export struct PageBreakInsideShorthand {
    static constexpr auto ID = ShorthandId::PAGE_BREAK_INSIDE;

    static constexpr auto LONGHANDS = Array{
        LonghandId::BREAK_INSIDE,
    };

    static Res<> parse(Cursor<Css::Sst>& c, Vec<Rc<Property>>& stylesheet, ParsingMode) {
        if (c.skip(Css::Token::ident("auto"))) {
            stylesheet.pushBack(makeRc<BreakInsideProperty>(BreakInside::AUTO));
            return Ok();
        }

        if (c.skip(Css::Token::ident("avoid"))) {
            stylesheet.pushBack(makeRc<BreakInsideProperty>(BreakInside::AVOID));
            return Ok();
        }

        return Error::invalidData("expected auto or avoid");
    }
};

} // namespace Vaev::Style
