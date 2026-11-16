module;

#include <karm/macros>

export module Vaev.Engine:props.padding;

import Karm.Core;
import :props.base;
import :css.parser;
import :style.computed;

using namespace Karm;

namespace Vaev::Style {

// MARK: Padding ---------------------------------------------------------------

// https://www.w3.org/TR/css-box-3/#propdef-padding

export struct PaddingTopProperty : Property {
    static constexpr auto ID = LonghandId::PADDING_TOP;

    static Rc<Property> initial() {
        return makeRc<PaddingTopProperty>(Calc<PercentOr<Length>>{Length{}});
    }

    static Rc<Property> load(ComputedValues const& c) {
        return makeRc<PaddingTopProperty>(c.padding->top);
    }

    static Res<Rc<Property>> parse(Cursor<Css::Sst>& c, ParsingMode) {
        return Ok(makeRc<PaddingTopProperty>(try$(parseValue<Calc<PercentOr<Length>>>(c))));
    }

    Calc<PercentOr<Length>> _value;

    PaddingTopProperty(Calc<PercentOr<Length>> value)
        : Property(ID), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.padding.cow().top = _value;
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

export struct PaddingRightProperty : Property {
    static constexpr auto ID = LonghandId::PADDING_RIGHT;

    static Rc<Property> initial() {
        return makeRc<PaddingRightProperty>(Calc<PercentOr<Length>>{Length{}});
    }

    static Rc<Property> load(ComputedValues const& c) {
        return makeRc<PaddingRightProperty>(c.padding->end);
    }

    static Res<Rc<Property>> parse(Cursor<Css::Sst>& c, ParsingMode) {
        return Ok(makeRc<PaddingRightProperty>(try$(parseValue<Calc<PercentOr<Length>>>(c))));
    }

    Calc<PercentOr<Length>> _value;

    PaddingRightProperty(Calc<PercentOr<Length>> value)
        : Property(ID), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.padding.cow().end = _value;
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

export struct PaddingBottomProperty : Property {
    static constexpr auto ID = LonghandId::PADDING_BOTTOM;

    static Rc<Property> initial() {
        return makeRc<PaddingBottomProperty>(Calc<PercentOr<Length>>{Length{}});
    }

    static Rc<Property> load(ComputedValues const& c) {
        return makeRc<PaddingBottomProperty>(c.padding->bottom);
    }

    static Res<Rc<Property>> parse(Cursor<Css::Sst>& c, ParsingMode) {
        return Ok(makeRc<PaddingBottomProperty>(try$(parseValue<Calc<PercentOr<Length>>>(c))));
    }

    Calc<PercentOr<Length>> _value;

    PaddingBottomProperty(Calc<PercentOr<Length>> value)
        : Property(ID), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.padding.cow().bottom = _value;
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

export struct PaddingLeftProperty : Property {
    static constexpr auto ID = LonghandId::PADDING_LEFT;

    static Rc<Property> initial() {
        return makeRc<PaddingLeftProperty>(Calc<PercentOr<Length>>{Length{}});
    }

    static Rc<Property> load(ComputedValues const& c) {
        return makeRc<PaddingLeftProperty>(c.padding->start);
    }

    static Res<Rc<Property>> parse(Cursor<Css::Sst>& c, ParsingMode) {
        return Ok(makeRc<PaddingLeftProperty>(try$(parseValue<Calc<PercentOr<Length>>>(c))));
    }

    Calc<PercentOr<Length>> _value;

    PaddingLeftProperty(Calc<PercentOr<Length>> value)
        : Property(ID), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.padding.cow().start = _value;
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

export struct PaddingInlineStartProperty : Property {
    static constexpr auto ID = LonghandId::PADDING_INLINE_START;

    static Rc<Property> initial() {
        return makeRc<PaddingInlineStartProperty>(Calc<PercentOr<Length>>{Length{}});
    }

    static Rc<Property> load(ComputedValues const& c) {
        return makeRc<PaddingInlineStartProperty>(c.padding->start);
    }

    static Res<Rc<Property>> parse(Cursor<Css::Sst>& c, ParsingMode) {
        return Ok(makeRc<PaddingInlineStartProperty>(try$(parseValue<Calc<PercentOr<Length>>>(c))));
    }

    Calc<PercentOr<Length>> _value;

    PaddingInlineStartProperty(Calc<PercentOr<Length>> value)
        : Property(ID), _value(value) {}

    // FIXME: Take writing mode into account
    // FIXME: Cascade together with the paired physical property, the declaration with
    //        the higher priority must win, not the one applied last.
    //        https://drafts.csswg.org/css-logical-1/#logical-property-group
    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.padding.cow().start = _value;
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

export struct PaddingInlineEndProperty : Property {
    static constexpr auto ID = LonghandId::PADDING_INLINE_END;

    static Rc<Property> initial() {
        return makeRc<PaddingInlineEndProperty>(Calc<PercentOr<Length>>{Length{}});
    }

    static Rc<Property> load(ComputedValues const& c) {
        return makeRc<PaddingInlineEndProperty>(c.padding->end);
    }

    static Res<Rc<Property>> parse(Cursor<Css::Sst>& c, ParsingMode) {
        return Ok(makeRc<PaddingInlineEndProperty>(try$(parseValue<Calc<PercentOr<Length>>>(c))));
    }

    Calc<PercentOr<Length>> _value;

    PaddingInlineEndProperty(Calc<PercentOr<Length>> value)
        : Property(ID), _value(value) {}

    // FIXME: Take writing mode into account
    // FIXME: Cascade together with the paired physical property, the declaration with
    //        the higher priority must win, not the one applied last.
    //        https://drafts.csswg.org/css-logical-1/#logical-property-group
    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.padding.cow().end = _value;
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://drafts.csswg.org/css-box-4/#padding-shorthand
export struct PaddingShorthand {
    static constexpr auto ID = ShorthandId::PADDING;

    static constexpr Array LONGHANDS = {
        LonghandId::PADDING_TOP,
        LonghandId::PADDING_RIGHT,
        LonghandId::PADDING_BOTTOM,
        LonghandId::PADDING_LEFT,
    };

    // <'padding-top'>{1,4}
    static Res<> parse(Cursor<Css::Sst>& c, Vec<Rc<Property>>& stylesheet, ParsingMode) {
        auto value = try$(parseValue<Math::Insets<Calc<PercentOr<Length>>>>(c));
        stylesheet.pushBack(makeRc<PaddingTopProperty>(value.top));
        stylesheet.pushBack(makeRc<PaddingRightProperty>(value.end));
        stylesheet.pushBack(makeRc<PaddingBottomProperty>(value.bottom));
        stylesheet.pushBack(makeRc<PaddingLeftProperty>(value.start));
        return Ok();
    }
};

} // namespace Vaev::Style
