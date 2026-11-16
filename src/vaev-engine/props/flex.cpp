module;

#include <karm/macros>

export module Vaev.Engine:props.flex;

import Karm.Core;
import :props.base;
import :css.parser;
import :style.computed;

using namespace Karm;

namespace Vaev::Style {

// MARK: Flex ------------------------------------------------------------------

// https://www.w3.org/TR/css-flexbox-1/#flex-basis-property
export struct FlexBasisProperty : Property {
    static constexpr auto ID = LonghandId::FLEX_BASIS;

    static Rc<Property> initial() {
        return makeRc<FlexBasisProperty>(Keywords::AUTO);
    }

    static Rc<Property> load(ComputedValues const& c) {
        return makeRc<FlexBasisProperty>(c.flex->basis);
    }

    static Res<Rc<Property>> parse(Cursor<Css::Sst>& c, ParsingMode) {
        return Ok(makeRc<FlexBasisProperty>(try$(parseValue<FlexBasis>(c))));
    }

    FlexBasis _value;

    FlexBasisProperty(FlexBasis value)
        : Property(ID), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.flex.cow().basis = _value;
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/css-flexbox-1/#propdef-flex-direction
export struct FlexDirectionProperty : Property {
    static constexpr auto ID = LonghandId::FLEX_DIRECTION;

    static Rc<Property> initial() {
        return makeRc<FlexDirectionProperty>(FlexDirection::ROW);
    }

    static Rc<Property> load(ComputedValues const& c) {
        return makeRc<FlexDirectionProperty>(c.flex->direction);
    }

    static Res<Rc<Property>> parse(Cursor<Css::Sst>& c, ParsingMode) {
        return Ok(makeRc<FlexDirectionProperty>(try$(parseValue<FlexDirection>(c))));
    }

    FlexDirection _value;

    FlexDirectionProperty(FlexDirection value)
        : Property(ID), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.flex.cow().direction = _value;
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/css-flexbox-1/#flex-grow-property
export struct FlexGrowProperty : Property {
    static constexpr auto ID = LonghandId::FLEX_GROW;

    static Rc<Property> initial() {
        return makeRc<FlexGrowProperty>(Number{0});
    }

    static Rc<Property> load(ComputedValues const& c) {
        return makeRc<FlexGrowProperty>(c.flex->grow);
    }

    static Res<Rc<Property>> parse(Cursor<Css::Sst>& c, ParsingMode) {
        return Ok(makeRc<FlexGrowProperty>(try$(parseValue<Number>(c))));
    }

    Number _value;

    FlexGrowProperty(Number value)
        : Property(ID), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.flex.cow().grow = _value;
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/css-flexbox-1/#propdef-flex-shrink
export struct FlexShrinkProperty : Property {
    static constexpr auto ID = LonghandId::FLEX_SHRINK;

    static Rc<Property> initial() {
        return makeRc<FlexShrinkProperty>(Number{1});
    }

    static Rc<Property> load(ComputedValues const& c) {
        return makeRc<FlexShrinkProperty>(c.flex->shrink);
    }

    static Res<Rc<Property>> parse(Cursor<Css::Sst>& c, ParsingMode) {
        return Ok(makeRc<FlexShrinkProperty>(try$(parseValue<Number>(c))));
    }

    Number _value;

    FlexShrinkProperty(Number value)
        : Property(ID), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.flex.cow().shrink = _value;
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/css-flexbox-1/#propdef-flex-wrap
export struct FlexWrapProperty : Property {
    static constexpr auto ID = LonghandId::FLEX_WRAP;

    static Rc<Property> initial() {
        return makeRc<FlexWrapProperty>(FlexWrap::NOWRAP);
    }

    static Rc<Property> load(ComputedValues const& c) {
        return makeRc<FlexWrapProperty>(c.flex->wrap);
    }

    static Res<Rc<Property>> parse(Cursor<Css::Sst>& c, ParsingMode) {
        return Ok(makeRc<FlexWrapProperty>(try$(parseValue<FlexWrap>(c))));
    }

    FlexWrap _value;

    FlexWrapProperty(FlexWrap value)
        : Property(ID), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.flex.cow().wrap = _value;
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/css-flexbox-1/#propdef-flex-flow
export struct FlexFlowShorthand {
    static constexpr auto ID = ShorthandId::FLEX_FLOW;

    static constexpr auto LONGHANDS = Array{
        LonghandId::FLEX_DIRECTION,
        LonghandId::FLEX_WRAP,
    };

    static Res<> parse(Cursor<Css::Sst>& c, Vec<Rc<Property>>& stylesheet, ParsingMode) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        // NOTE: Omitted values are set to their initial value.
        FlexDirection direction = FlexDirection::ROW;
        FlexWrap wrap = FlexWrap::NOWRAP;

        if (auto maybeDirection = parseValue<FlexDirection>(c)) {
            direction = maybeDirection.take();
            if (auto maybeWrap = parseValue<FlexWrap>(c))
                wrap = maybeWrap.take();
        } else {
            wrap = try$(parseValue<FlexWrap>(c));
            if (auto maybeDirection = parseValue<FlexDirection>(c))
                direction = maybeDirection.take();
        }

        stylesheet.pushBack(makeRc<FlexDirectionProperty>(direction));
        stylesheet.pushBack(makeRc<FlexWrapProperty>(wrap));
        return Ok();
    }
};

// https://www.w3.org/TR/css-flexbox-1/#propdef-flex
export struct FlexShorthand {
    static constexpr auto ID = ShorthandId::FLEX;

    static constexpr auto LONGHANDS = Array{
        LonghandId::FLEX_GROW,
        LonghandId::FLEX_SHRINK,
        LonghandId::FLEX_BASIS,
    };

    static void _push(Vec<Rc<Property>>& stylesheet, Number grow, Number shrink, FlexBasis basis) {
        stylesheet.pushBack(makeRc<FlexGrowProperty>(grow));
        stylesheet.pushBack(makeRc<FlexShrinkProperty>(shrink));
        stylesheet.pushBack(makeRc<FlexBasisProperty>(std::move(basis)));
    }

    // <'flex-grow'> <'flex-shrink'>?
    static Res<> _parseGrowShrink(Cursor<Css::Sst>& c, Number& grow, Number& shrink) {
        grow = try$(parseValue<Number>(c));
        if (auto maybeShrink = parseValue<Number>(c))
            shrink = maybeShrink.take();
        return Ok();
    }

    // none | [ <'flex-grow'> <'flex-shrink'>? || <'flex-basis'> ]
    static Res<> parse(Cursor<Css::Sst>& c, Vec<Rc<Property>>& stylesheet, ParsingMode) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        // The keyword none expands to 0 0 auto.
        if (c.skip(Css::Token::ident("none"))) {
            _push(stylesheet, 0, 0, Keywords::AUTO);
            return Ok();
        }

        // When omitted from the flex shorthand, flex-grow and flex-shrink
        // are set to 1, and flex-basis to 0.
        Number grow = 1;
        Number shrink = 1;
        FlexBasis basis = Calc<PercentOr<Length>>(Length{});

        if (_parseGrowShrink(c, grow, shrink)) {
            if (auto maybeBasis = parseValue<FlexBasis>(c))
                basis = maybeBasis.take();
        } else {
            basis = try$(parseValue<FlexBasis>(c));
            (void)_parseGrowShrink(c, grow, shrink);
        }

        _push(stylesheet, grow, shrink, std::move(basis));
        return Ok();
    }
};

} // namespace Vaev::Style
