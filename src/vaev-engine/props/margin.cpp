module;

#include <karm/macros>

export module Vaev.Engine:props.margin;

import Karm.Core;
import :props.base;
import :css.parser;
import :style.computed;

using namespace Karm;

namespace Vaev::Style {

// https://drafts.csswg.org/css-box-4/#propdef-margin-top
export struct MarginTopProperty : Property {
    static constexpr auto ID = LonghandId::MARGIN_TOP;

    using Value = Union<Keywords::Auto, Calc<PercentOr<Length>>>;

    static Rc<Property> initial() {
        return makeRc<MarginTopProperty>(Calc<PercentOr<Length>>(Length{}));
    }

    static Rc<Property> load(ComputedValues const& c) {
        return makeRc<MarginTopProperty>(c.margin->top);
    }

    static Res<Rc<Property>> parse(Cursor<Css::Sst>& c, ParsingMode) {
        return Ok(makeRc<MarginTopProperty>(try$(parseValue<Value>(c))));
    }

    Value _value;

    MarginTopProperty(Value value)
        : Property(ID), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.margin.cow().top = _value;
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://drafts.csswg.org/css-box-4/#propdef-margin-right
export struct MarginRightProperty : Property {
    static constexpr auto ID = LonghandId::MARGIN_RIGHT;

    using Value = MarginTopProperty::Value;

    static Rc<Property> initial() {
        return makeRc<MarginRightProperty>(Calc<PercentOr<Length>>(Length{}));
    }

    static Rc<Property> load(ComputedValues const& c) {
        return makeRc<MarginRightProperty>(c.margin->end);
    }

    static Res<Rc<Property>> parse(Cursor<Css::Sst>& c, ParsingMode) {
        return Ok(makeRc<MarginRightProperty>(try$(parseValue<Value>(c))));
    }

    Value _value;

    MarginRightProperty(Value value)
        : Property(ID), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.margin.cow().end = _value;
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://drafts.csswg.org/css-box-4/#propdef-margin-bottom
export struct MarginBottomProperty : Property {
    static constexpr auto ID = LonghandId::MARGIN_BOTTOM;

    using Value = MarginTopProperty::Value;

    static Rc<Property> initial() {
        return makeRc<MarginBottomProperty>(Calc<PercentOr<Length>>(Length{}));
    }

    static Rc<Property> load(ComputedValues const& c) {
        return makeRc<MarginBottomProperty>(c.margin->bottom);
    }

    static Res<Rc<Property>> parse(Cursor<Css::Sst>& c, ParsingMode) {
        return Ok(makeRc<MarginBottomProperty>(try$(parseValue<Value>(c))));
    }

    Value _value;

    MarginBottomProperty(Value value)
        : Property(ID), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.margin.cow().bottom = _value;
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://drafts.csswg.org/css-box-4/#propdef-margin-left
export struct MarginLeftProperty : Property {
    static constexpr auto ID = LonghandId::MARGIN_LEFT;

    using Value = MarginTopProperty::Value;

    static Rc<Property> initial() {
        return makeRc<MarginLeftProperty>(Calc<PercentOr<Length>>(Length{}));
    }

    static Rc<Property> load(ComputedValues const& c) {
        return makeRc<MarginLeftProperty>(c.margin->start);
    }

    static Res<Rc<Property>> parse(Cursor<Css::Sst>& c, ParsingMode) {
        return Ok(makeRc<MarginLeftProperty>(try$(parseValue<Value>(c))));
    }

    Value _value;

    MarginLeftProperty(Value value)
        : Property(ID), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.margin.cow().start = _value;
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://drafts.csswg.org/css-box-4/#margin-shorthand
// https://drafts.csswg.org/css-box-4/#margin-shorthand
export struct MarginShorthand {
    static constexpr auto ID = ShorthandId::MARGIN;

    static constexpr Array LONGHANDS = {
        LonghandId::MARGIN_TOP,
        LonghandId::MARGIN_RIGHT,
        LonghandId::MARGIN_BOTTOM,
        LonghandId::MARGIN_LEFT,
    };

    // <'margin-top'>{1,4}
    static Res<> parse(Cursor<Css::Sst>& c, Vec<Rc<Property>>& stylesheet, ParsingMode) {
        auto value = try$(parseValue<Math::Insets<MarginTopProperty::Value>>(c));
        stylesheet.pushBack(makeRc<MarginTopProperty>(value.top));
        stylesheet.pushBack(makeRc<MarginRightProperty>(value.end));
        stylesheet.pushBack(makeRc<MarginBottomProperty>(value.bottom));
        stylesheet.pushBack(makeRc<MarginLeftProperty>(value.start));
        return Ok();
    }
};

// https://drafts.csswg.org/css-logical/#propdef-margin-inline-start
export struct MarginInlineStartProperty : Property {
    static constexpr auto ID = LonghandId::MARGIN_INLINE_START;

    using Value = MarginTopProperty::Value;

    static Rc<Property> initial() {
        return makeRc<MarginInlineStartProperty>(Calc<PercentOr<Length>>(Length{}));
    }

    static Rc<Property> load(ComputedValues const& c) {
        return makeRc<MarginInlineStartProperty>(c.margin->start);
    }

    static Res<Rc<Property>> parse(Cursor<Css::Sst>& c, ParsingMode) {
        return Ok(makeRc<MarginInlineStartProperty>(try$(parseValue<Value>(c))));
    }

    Value _value;

    MarginInlineStartProperty(Value value)
        : Property(ID), _value(value) {}

    // FIXME: Take writing mode into account
    // FIXME: Cascade together with the paired physical property, the declaration with
    //        the higher priority must win, not the one applied last.
    //        https://drafts.csswg.org/css-logical-1/#logical-property-group
    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.margin.cow().start = _value;
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://drafts.csswg.org/css-logical/#propdef-margin-inline-end
export struct MarginInlineEndProperty : Property {
    static constexpr auto ID = LonghandId::MARGIN_INLINE_END;

    using Value = MarginTopProperty::Value;

    static Rc<Property> initial() {
        return makeRc<MarginInlineEndProperty>(Calc<PercentOr<Length>>(Length{}));
    }

    static Rc<Property> load(ComputedValues const& c) {
        return makeRc<MarginInlineEndProperty>(c.margin->end);
    }

    static Res<Rc<Property>> parse(Cursor<Css::Sst>& c, ParsingMode) {
        return Ok(makeRc<MarginInlineEndProperty>(try$(parseValue<Value>(c))));
    }

    Value _value;

    MarginInlineEndProperty(Value value)
        : Property(ID), _value(value) {}

    // FIXME: Take writing mode into account
    // FIXME: Cascade together with the paired physical property, the declaration with
    //        the higher priority must win, not the one applied last.
    //        https://drafts.csswg.org/css-logical-1/#logical-property-group
    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.margin.cow().end = _value;
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://drafts.csswg.org/css-logical/#propdef-margin-inline
export struct MarginInlineShorthand {
    static constexpr auto ID = ShorthandId::MARGIN_INLINE;

    static constexpr Array LONGHANDS = {
        LonghandId::MARGIN_INLINE_START,
        LonghandId::MARGIN_INLINE_END,
    };

    // <'margin-top'>{1,2}
    static Res<> parse(Cursor<Css::Sst>& c, Vec<Rc<Property>>& stylesheet, ParsingMode) {
        auto start = try$(parseValue<MarginTopProperty::Value>(c));

        // If the second value is omitted, it is set to the same as the first.
        auto end = parseValue<MarginTopProperty::Value>(c).unwrapOr(start);

        stylesheet.pushBack(makeRc<MarginInlineStartProperty>(std::move(start)));
        stylesheet.pushBack(makeRc<MarginInlineEndProperty>(std::move(end)));
        return Ok();
    }
};

// https://drafts.csswg.org/css-logical/#propdef-margin-block-start
export struct MarginBlockStartProperty : Property {
    static constexpr auto ID = LonghandId::MARGIN_BLOCK_START;

    using Value = MarginTopProperty::Value;

    static Rc<Property> initial() {
        return makeRc<MarginBlockStartProperty>(Calc<PercentOr<Length>>(Length{}));
    }

    static Rc<Property> load(ComputedValues const& c) {
        return makeRc<MarginBlockStartProperty>(c.margin->top);
    }

    static Res<Rc<Property>> parse(Cursor<Css::Sst>& c, ParsingMode) {
        return Ok(makeRc<MarginBlockStartProperty>(try$(parseValue<Value>(c))));
    }

    Value _value;

    MarginBlockStartProperty(Value value)
        : Property(ID), _value(value) {}

    // FIXME: Take writing mode into account
    // FIXME: Cascade together with the paired physical property, the declaration with
    //        the higher priority must win, not the one applied last.
    //        https://drafts.csswg.org/css-logical-1/#logical-property-group
    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.margin.cow().top = _value;
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://drafts.csswg.org/css-logical/#propdef-margin-block-end
export struct MarginBlockEndProperty : Property {
    static constexpr auto ID = LonghandId::MARGIN_BLOCK_END;

    using Value = MarginTopProperty::Value;

    static Rc<Property> initial() {
        return makeRc<MarginBlockEndProperty>(Calc<PercentOr<Length>>(Length{}));
    }

    static Rc<Property> load(ComputedValues const& c) {
        return makeRc<MarginBlockEndProperty>(c.margin->bottom);
    }

    static Res<Rc<Property>> parse(Cursor<Css::Sst>& c, ParsingMode) {
        return Ok(makeRc<MarginBlockEndProperty>(try$(parseValue<Value>(c))));
    }

    Union<Keywords::Auto, Calc<PercentOr<Length>>> _value;

    MarginBlockEndProperty(Value value)
        : Property(ID), _value(value) {}

    // FIXME: Take writing mode into account
    // FIXME: Cascade together with the paired physical property, the declaration with
    //        the higher priority must win, not the one applied last.
    //        https://drafts.csswg.org/css-logical-1/#logical-property-group
    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.margin.cow().bottom = _value;
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://drafts.csswg.org/css-logical/#propdef-margin-block
export struct MarginBlockShorthand {
    static constexpr auto ID = ShorthandId::MARGIN_BLOCK;

    static constexpr Array LONGHANDS = {
        LonghandId::MARGIN_BLOCK_START,
        LonghandId::MARGIN_BLOCK_END,
    };

    // <'margin-top'>{1,2}
    static Res<> parse(Cursor<Css::Sst>& c, Vec<Rc<Property>>& stylesheet, ParsingMode) {
        auto start = try$(parseValue<MarginTopProperty::Value>(c));

        // If the second value is omitted, it is set to the same as the first.
        auto end = parseValue<MarginTopProperty::Value>(c).unwrapOr(start);

        stylesheet.pushBack(makeRc<MarginBlockStartProperty>(std::move(start)));
        stylesheet.pushBack(makeRc<MarginBlockEndProperty>(std::move(end)));
        return Ok();
    }
};

} // namespace Vaev::Style
