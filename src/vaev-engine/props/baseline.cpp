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
    static inline Flags<Options> const FLAGS = {INHERITED};

    static Rc<Property> initial() {
        return makeRc<LineHeightProperty>(Keywords::NORMAL);
    }

    static Rc<Property> load(ComputedValues const& c) {
        return makeRc<LineHeightProperty>(c.inline_->lineHeight);
    }

    static Res<Rc<Property>> parse(Cursor<Css::Sst>& c) {
        return Ok(makeRc<LineHeightProperty>(try$(parseValue<LineHeight>(c))));
    }

    static void inherit(ComputedValues const& parent, ComputedValues& child) {
        child.inline_.cow().lineHeight = parent.inline_->lineHeight;
    }

    struct Registration : LonghandRegistration {
        Registration()
            : LonghandRegistration(Properties::LINE_HEIGHT, FLAGS) {}
    };

    LineHeight _value;

    LineHeightProperty(LineHeight value)
        : Property(registrationOf<LineHeightProperty>()), _value(value) {}

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

    static Res<Rc<Property>> parse(Cursor<Css::Sst>& c) {
        return Ok(makeRc<DominantBaselineProperty>(try$(parseValue<DominantBaseline>(c))));
    }

    struct Registration : LonghandRegistration {
        Registration() : LonghandRegistration(Properties::DOMINANT_BASELINE) {}
    };

    DominantBaseline _value;

    DominantBaselineProperty(DominantBaseline value)
        : Property(registrationOf<DominantBaselineProperty>()), _value(value) {}

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

    static Res<Rc<Property>> parse(Cursor<Css::Sst>& c) {
        return Ok(makeRc<BaselineSourceProperty>(try$(parseValue<BaselineSource>(c))));
    }

    struct Registration : LonghandRegistration {
        Registration() : LonghandRegistration(Properties::BASELINE_SOURCE) {}
    };

    BaselineSource _value;

    BaselineSourceProperty(BaselineSource value)
        : Property(registrationOf<BaselineSourceProperty>()), _value(value) {}

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

    static Res<Rc<Property>> parse(Cursor<Css::Sst>& c) {
        return Ok(makeRc<AlignmentBaselineProperty>(try$(parseValue<AlignmentBaseline>(c))));
    }

    struct Registration : LonghandRegistration {
        Registration() : LonghandRegistration(Properties::ALIGNMENT_BASELINE) {}
    };

    AlignmentBaseline _value;

    AlignmentBaselineProperty(AlignmentBaseline value)
        : Property(registrationOf<AlignmentBaselineProperty>()), _value(value) {}

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

    static Res<Rc<Property>> parse(Cursor<Css::Sst>& c) {
        return Ok(makeRc<BaselineShiftProperty>(try$(parseValue<BaselineShift>(c))));
    }

    struct Registration : LonghandRegistration {
        Registration() : LonghandRegistration(Properties::BASELINE_SHIFT) {}
    };

    BaselineShift _value;

    BaselineShiftProperty(BaselineShift value)
        : Property(registrationOf<BaselineShiftProperty>()), _value(value) {}

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

// // https://drafts.csswg.org/css-inline/#transverse-alignment
// export struct VerticalAlignProperty : Property {
//     struct Value {
//         Opt<BaselineSource> baselineSource = NONE;
//         Opt<AlignmentBaseline> alignmentBaseline = NONE;
//         Opt<BaselineShift> baselineShift = NONE;
//     };
//
//     struct Registration : Property::Registration {
//         Registration()
//             : Property::Registration(Properties::VERTICAL_ALIGN, SHORTHAND) {}
//
//         Rc<Property> initial() const override {
//             return makeRc<VerticalAlignProperty>(self(), Value{.alignmentBaseline = Some(Keywords::BASELINE)});
//         }
//
//         Rc<Property> load(ComputedValues const& c) const override {
//             return makeRc<VerticalAlignProperty>(
//                 self(),
//                 Value{
//                     Some(c.inline_->baselineSource),
//                     Some(c.inline_->alignmentBaseline),
//                     Some(c.inline_->baselineShift),
//                 }
//             );
//         }
//
//         Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
//             if (c.ended())
//                 return Error::invalidData("unexpected end of input");
//
//             auto value = Value{};
//
//             while (not c.ended()) {
//                 auto maybeFirstOrLast = parseValue<Union<Keywords::First, Keywords::Last>>(c);
//                 if (maybeFirstOrLast) {
//                     if (not value.baselineSource) {
//                         value.baselineSource = Some(maybeFirstOrLast.expect().visit(
//                             [](auto& v) -> BaselineSource {
//                                 return v;
//                             }
//                         ));
//                         continue;
//                     }
//
//                     return Error::invalidData("duplicated value type");
//                 }
//
//                 auto maybeAlignmentBaseline = parseValue<AlignmentBaseline>(c);
//                 if (maybeAlignmentBaseline) {
//                     if (not value.alignmentBaseline) {
//                         value.alignmentBaseline = Some(maybeAlignmentBaseline.expect());
//                         continue;
//                     }
//
//                     return Error::invalidData("duplicated value type");
//                 }
//
//                 auto maybeBaselineShift = parseValue<BaselineShift>(c);
//                 if (maybeBaselineShift) {
//                     if (not value.baselineShift) {
//                         value.baselineShift = Some(maybeBaselineShift.expect());
//                         continue;
//                     }
//
//                     return Error::invalidData("duplicated value type");
//                 }
//
//                 return Error::invalidData("unknown value");
//             }
//
//             if (not value.alignmentBaseline and not value.baselineSource and not value.baselineShift) {
//                 return Error::invalidData("missing value");
//             }
//
//             return Ok(makeRc<VerticalAlignProperty>(self(), value));
//         }
//     };
//
//     Value _value;
//
//     VerticalAlignProperty(Rc<Property::Registration> registration, Value value)
//         : Property(registration), _value(value) {}
//
//     Longhands expand(RegisteredPropertySet& registry, ComputedValues const&, ComputedValues&) const override {
//         Longhands shorthands = {};
//
//         if (auto const& [baselineSource] = _value.baselineSource) {
//             shorthands.pushBack(
//                 makeRc<BaselineSourceProperty>(
//                     registry.resolveRegistration(Properties::BASELINE_SOURCE, {}).take(),
//                     baselineSource
//                 )
//             );
//         }
//
//         if (auto const& [alignmentBaseline] = _value.alignmentBaseline) {
//             shorthands.pushBack(
//                 makeRc<AlignmentBaselineProperty>(
//                     registry.resolveRegistration(Properties::ALIGNMENT_BASELINE, {}).take(),
//                     alignmentBaseline
//                 )
//             );
//         }
//
//         if (auto const& [baselineShift] = _value.baselineShift) {
//             shorthands.pushBack(
//                 makeRc<BaselineShiftProperty>(
//                     registry.resolveRegistration(Properties::BASELINE_SHIFT, {}).take(),
//                     baselineShift
//                 )
//             );
//         }
//
//         return shorthands;
//     }
//
//     void repr(Io::Emit& e) const override {
//         e("baseline-source={} alignment-baseline={}", _value.baselineSource, _value.alignmentBaseline);
//     }
// };

} // namespace Vaev::Style
