module;

#include <karm/macros>

export module Vaev.Engine:props.baseline;

import Karm.Core;
import :props.base;
import :css.parser;
import :style.computed;

using namespace Karm;

namespace Vaev::Style {

// https://drafts.csswg.org/css-inline/#line-height-property
export struct LineHeightProperty : Property {
    static constexpr auto ID = LonghandId::LINE_HEIGHT;
    static constexpr Flags<Options> FLAGS = {INHERITED};

    static Rc<Property> initial() {
        return makeRc<LineHeightProperty>(Keywords::NORMAL);
    }

    static Rc<Property> load(ComputedValues const& c) {
        return makeRc<LineHeightProperty>(c.inline_->lineHeight);
    }

    static Res<Rc<Property>> parse(Cursor<Css::Sst>& c, ParsingMode) {
        return Ok(makeRc<LineHeightProperty>(try$(parseValue<LineHeight>(c))));
    }

    static void inherit(ComputedValues const& parent, ComputedValues& child) {
        child.inline_.cow().lineHeight = parent.inline_->lineHeight;
    }

    LineHeight _value;

    LineHeightProperty(LineHeight value)
        : Property(ID), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.inline_.cow().lineHeight = _value;
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/css-inline-3/#dominant-baseline-property
export struct DominantBaselineProperty : Property {
    static constexpr auto ID = LonghandId::DOMINANT_BASELINE;

    static Rc<Property> initial() {
        return makeRc<DominantBaselineProperty>(Keywords::AUTO);
    }

    static Rc<Property> load(ComputedValues const& c) {
        return makeRc<DominantBaselineProperty>(c.inline_->dominantBaseline);
    }

    static Res<Rc<Property>> parse(Cursor<Css::Sst>& c, ParsingMode) {
        return Ok(makeRc<DominantBaselineProperty>(try$(parseValue<DominantBaseline>(c))));
    }

    DominantBaseline _value;

    DominantBaselineProperty(DominantBaseline value)
        : Property(ID), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.inline_.cow().dominantBaseline = _value;
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/css-inline-3/#baseline-source
export struct BaselineSourceProperty : Property {
    static constexpr auto ID = LonghandId::BASELINE_SOURCE;

    static Rc<Property> initial() {
        return makeRc<BaselineSourceProperty>(Keywords::AUTO);
    }

    static Rc<Property> load(ComputedValues const& c) {
        return makeRc<BaselineSourceProperty>(c.inline_->baselineSource);
    }

    static Res<Rc<Property>> parse(Cursor<Css::Sst>& c, ParsingMode) {
        return Ok(makeRc<BaselineSourceProperty>(try$(parseValue<BaselineSource>(c))));
    }

    BaselineSource _value;

    BaselineSourceProperty(BaselineSource value)
        : Property(ID), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.inline_.cow().baselineSource = _value;
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/css-inline-3/#alignment-baseline-property
export struct AlignmentBaselineProperty : Property {
    static constexpr auto ID = LonghandId::ALIGNMENT_BASELINE;

    static Rc<Property> initial() {
        return makeRc<AlignmentBaselineProperty>(Keywords::BASELINE);
    }

    static Rc<Property> load(ComputedValues const& c) {
        return makeRc<AlignmentBaselineProperty>(c.inline_->alignmentBaseline);
    }

    static Res<Rc<Property>> parse(Cursor<Css::Sst>& c, ParsingMode) {
        return Ok(makeRc<AlignmentBaselineProperty>(try$(parseValue<AlignmentBaseline>(c))));
    }

    AlignmentBaseline _value;

    AlignmentBaselineProperty(AlignmentBaseline value)
        : Property(ID), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.inline_.cow().alignmentBaseline = _value;
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://drafts.csswg.org/css-inline/#propdef-baseline-shift
export struct BaselineShiftProperty : Property {
    static constexpr auto ID = LonghandId::BASELINE_SHIFT;

    static Rc<Property> initial() {
        return makeRc<BaselineShiftProperty>(Calc<PercentOr<Length>>(Length{}));
    }

    static Rc<Property> load(ComputedValues const& c) {
        return makeRc<BaselineShiftProperty>(c.inline_->baselineShift);
    }

    static Res<Rc<Property>> parse(Cursor<Css::Sst>& c, ParsingMode) {
        return Ok(makeRc<BaselineShiftProperty>(try$(parseValue<BaselineShift>(c))));
    }

    BaselineShift _value;

    BaselineShiftProperty(BaselineShift value)
        : Property(ID), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.inline_.cow().baselineShift = _value;
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://drafts.csswg.org/css-inline/#transverse-alignment
export struct VerticalAlignShorthand {
    static constexpr auto ID = ShorthandId::VERTICAL_ALIGN;

    static constexpr auto LONGHANDS = Array{
        LonghandId::ALIGNMENT_BASELINE,
        LonghandId::BASELINE_SOURCE,
        LonghandId::BASELINE_SHIFT,
    };

    static Res<> parse(Cursor<Css::Sst>& c, Vec<Rc<Property>>& stylesheet, ParsingMode) {
        Opt<BaselineSource> baselineSource;
        Opt<BaselineShift> baselineShift;
        Opt<AlignmentBaseline> alignmentBaseline;

        while (not c.ended()) {
            eatWhitespace(c);

            auto maybeFirstOrLast = parseValue<Union<Keywords::First, Keywords::Last>>(c);
            if (maybeFirstOrLast) {
                if (not baselineSource) {
                    baselineSource = Some(maybeFirstOrLast.expect().visit(
                        [](auto& v) -> BaselineSource {
                            return v;
                        }
                    ));
                    continue;
                }

                return Error::invalidData("duplicated value type");
            }

            auto maybeAlignmentBaseline = parseValue<AlignmentBaseline>(c);
            if (maybeAlignmentBaseline) {
                if (not alignmentBaseline) {
                    alignmentBaseline = Some(maybeAlignmentBaseline.expect());
                    continue;
                }

                return Error::invalidData("duplicated value type");
            }

            auto maybeBaselineShift = parseValue<BaselineShift>(c);
            if (maybeBaselineShift) {
                if (not baselineShift) {
                    baselineShift = Some(maybeBaselineShift.expect());
                    continue;
                }

                return Error::invalidData("duplicated value type");
            }

            return Error::invalidData("unknown value");
        }

        if (not alignmentBaseline and not baselineSource and not baselineShift)
            return Error::invalidData("missing value");

        if (alignmentBaseline)
            stylesheet.pushBack(makeRc<AlignmentBaselineProperty>(*alignmentBaseline));
        else
            stylesheet.pushBack(initialProperty(LonghandId::ALIGNMENT_BASELINE));

        if (baselineSource)
            stylesheet.pushBack(makeRc<BaselineSourceProperty>(*baselineSource));
        else
            stylesheet.pushBack(initialProperty(LonghandId::BASELINE_SOURCE));

        if (baselineShift)
            stylesheet.pushBack(makeRc<BaselineShiftProperty>(*baselineShift));
        else
            stylesheet.pushBack(initialProperty(LonghandId::BASELINE_SHIFT));

        return Ok();
    }
};

} // namespace Vaev::Style
