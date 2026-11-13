module;

#include <karm/macros>

export module Vaev.Engine:props.align;

import Karm.Core;
import :props.base;
import :css.parser;
import :style.computed;

using namespace Karm;
using namespace Karm::Literals;

namespace Vaev::Style {

// MARK: Align -----------------------------------------------------------------
// https://drafts.csswg.org/css-align-3

// https://drafts.csswg.org/css-align-3/#propdef-align-content
export struct AlignContentProperty : Property {
    static constexpr auto ID = LonghandId::ALIGN_CONTENT;

    static Rc<Property> initial() {
        return makeRc<AlignContentProperty>(Align::Keywords::STRETCH);
    }

    static Rc<Property> load(ComputedValues const& c) {
        return makeRc<AlignContentProperty>(c.aligns.alignContent);
    }

    static Res<Rc<Property>> parse(Cursor<Css::Sst>& c, ParsingMode) {
        return Ok(makeRc<AlignContentProperty>(try$(parseValue<Align>(c))));
    }

    struct Registration : LonghandRegistration {
        Registration() : LonghandRegistration(Properties::ALIGN_CONTENT) {}
    };

    Align _value;

    AlignContentProperty(Align value)
        : Property(ID), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.aligns.alignContent = _value;
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://drafts.csswg.org/css-align-3/#propdef-justify-content
export struct JustifyContentProperty : Property {
    static constexpr auto ID = LonghandId::JUSTIFY_CONTENT;

    static Rc<Property> initial() {
        return makeRc<JustifyContentProperty>(Align::Keywords::FLEX_START);
    }

    static Rc<Property> load(ComputedValues const& c) {
        return makeRc<JustifyContentProperty>(c.aligns.justifyContent);
    }

    static Res<Rc<Property>> parse(Cursor<Css::Sst>& c, ParsingMode) {
        return Ok(makeRc<JustifyContentProperty>(try$(parseValue<Align>(c))));
    }

    struct Registration : LonghandRegistration {
        Registration() : LonghandRegistration(Properties::JUSTIFY_CONTENT) {}
    };

    Align _value;

    JustifyContentProperty(Align value)
        : Property(ID), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.aligns.justifyContent = _value;
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://drafts.csswg.org/css-align-3/#propdef-justify-self
export struct JustifySelfProperty : Property {
    static constexpr auto ID = LonghandId::JUSTIFY_SELF;

    static Rc<Property> initial() {
        return makeRc<JustifySelfProperty>(Align::Keywords::AUTO);
    }

    static Rc<Property> load(ComputedValues const& c) {
        return makeRc<JustifySelfProperty>(c.aligns.justifySelf);
    }

    static Res<Rc<Property>> parse(Cursor<Css::Sst>& c, ParsingMode) {
        return Ok(makeRc<JustifySelfProperty>(try$(parseValue<Align>(c))));
    }

    struct Registration : LonghandRegistration {
        Registration() : LonghandRegistration(Properties::JUSTIFY_SELF) {}
    };

    Align _value;

    JustifySelfProperty(Align value)
        : Property(ID), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.aligns.justifySelf = _value;
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://drafts.csswg.org/css-align-3/#propdef-align-self
export struct AlignSelfProperty : Property {
    static constexpr auto ID = LonghandId::ALIGN_SELF;

    static Rc<Property> initial() {
        return makeRc<AlignSelfProperty>(Align::Keywords::AUTO);
    }

    static Rc<Property> load(ComputedValues const& c) {
        return makeRc<AlignSelfProperty>(c.aligns.alignSelf);
    }

    static Res<Rc<Property>> parse(Cursor<Css::Sst>& c, ParsingMode) {
        return Ok(makeRc<AlignSelfProperty>(try$(parseValue<Align>(c))));
    }

    struct Registration : LonghandRegistration {
        Registration() : LonghandRegistration(Properties::ALIGN_SELF) {}
    };

    Align _value;

    AlignSelfProperty(Align value)
        : Property(ID), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.aligns.alignSelf = _value;
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://drafts.csswg.org/css-align-3/#propdef-justify-items
export struct JustifyItemsProperty : Property {
    static constexpr auto ID = LonghandId::JUSTIFY_ITEMS;

    static Rc<Property> initial() {
        return makeRc<JustifyItemsProperty>(Align::LEGACY);
    }

    static Rc<Property> load(ComputedValues const& c) {
        return makeRc<JustifyItemsProperty>(c.aligns.justifyItems);
    }

    static Res<Rc<Property>> parse(Cursor<Css::Sst>& c, ParsingMode) {
        return Ok(makeRc<JustifyItemsProperty>(try$(parseValue<Align>(c))));
    }

    struct Registration : LonghandRegistration {
        Registration() : LonghandRegistration(Properties::JUSTIFY_ITEMS) {}
    };

    Align _value;

    JustifyItemsProperty(Align value)
        : Property(ID), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.aligns.justifyItems = _value;
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://drafts.csswg.org/css-align-3/#propdef-align-items
export struct AlignItemsProperty : Property {
    static constexpr auto ID = LonghandId::ALIGN_ITEMS;

    static Rc<Property> initial() {
        return makeRc<AlignItemsProperty>(Align::Keywords::STRETCH);
    }

    static Rc<Property> load(ComputedValues const& c) {
        return makeRc<AlignItemsProperty>(c.aligns.alignItems);
    }

    static Res<Rc<Property>> parse(Cursor<Css::Sst>& c, ParsingMode) {
        return Ok(makeRc<AlignItemsProperty>(try$(parseValue<Align>(c))));
    }

    struct Registration : LonghandRegistration {
        Registration() : LonghandRegistration(Properties::ALIGN_ITEMS) {}
    };

    Align _value;

    AlignItemsProperty(Align value)
        : Property(ID), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.aligns.alignItems = _value;
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://drafts.csswg.org/css-align-3/#column-row-gap
export struct RowGapProperty : Property {
    static constexpr auto ID = LonghandId::ROW_GAP;
    // https://drafts.csswg.org/css-align-3/#gap-legacy
    static constexpr Array LEGACY_NAME_ALIASES = {"grid-row-gap"s};

    static Rc<Property> initial() {
        return makeRc<RowGapProperty>(Keywords::NORMAL);
    }

    static Rc<Property> load(ComputedValues const& c) {
        return makeRc<RowGapProperty>(c.gaps->row);
    }

    static Res<Rc<Property>> parse(Cursor<Css::Sst>& c, ParsingMode) {
        return Ok(makeRc<RowGapProperty>(try$(parseValue<Gap>(c))));
    }

    struct Registration : LonghandRegistration {
        Registration() : LonghandRegistration(Properties::ROW_GAP) {}
    };

    Gap _value;

    RowGapProperty(Gap value)
        : Property(ID), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.gaps.cow().row = _value;
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://drafts.csswg.org/css-align-3/#column-row-gap
export struct ColumnGapProperty : Property {
    static constexpr auto ID = LonghandId::COLUMN_GAP;
    // https://drafts.csswg.org/css-align-3/#gap-legacy
    static constexpr Array LEGACY_NAME_ALIASES = {"grid-column-gap"s};

    static Rc<Property> initial() {
        return makeRc<ColumnGapProperty>(Keywords::NORMAL);
    }

    static Rc<Property> load(ComputedValues const& c) {
        return makeRc<ColumnGapProperty>(c.gaps->col);
    }

    static Res<Rc<Property>> parse(Cursor<Css::Sst>& c, ParsingMode) {
        return Ok(makeRc<ColumnGapProperty>(try$(parseValue<Gap>(c))));
    }

    struct Registration : LonghandRegistration {
        Registration() : LonghandRegistration(Properties::COLUMN_GAP) {}
    };

    Gap _value;

    ColumnGapProperty(Gap value)
        : Property(ID), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.gaps.cow().col = _value;
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// export struct GapProperty : Property {
//     struct Registration : LonghandRegistration {
//         Registration() : LonghandRegistration(Properties::GAP) {}
//
//         // https://drafts.csswg.org/css-align-3/#gap-legacy
//         SmallVec<Symbol, 1> legacyNameAlias() const override {
//             return {"grid-gap"_sym};
//         }
//
//         Rc<Property> initial() const override {
//             return makeRc<GapProperty>(Gaps{
//                     Keywords::NORMAL,
//                     Keywords::NORMAL,
//                 }
//             );
//         }
//
//         Rc<Property> load(ComputedValues const& c) const override {
//             return makeRc<GapProperty>(*c.gaps);
//         }
//
//         Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
//             auto rowGap = try$(parseValue<Gap>(c));
//             eatWhitespace(c);
//             auto colGap = parseValue<Gap>(c);
//
//             if (not colGap)
//                 return Ok(makeRc<GapProperty>(Gaps{rowGap, rowGap}));
//             return Ok(makeRc<GapProperty>(Gaps{rowGap, colGap.take()}));
//         }
//     };
//
//     Gaps _value;
//
//     GapProperty(Gaps value)
//         : Property(registrationOf<GapProperty>()), _value(value) {}
//
//     Longhands expand(RegisteredPropertySet& registry, ComputedValues const&, ComputedValues&) const override {
//         return {
//             makeRc<RowGapProperty>(registry.resolveRegistration(Properties::MARGIN_TOP, {}).expect(), _value.row),
//             makeRc<ColumnGapProperty>(registry.resolveRegistration(Properties::MARGIN_TOP, {}).expect(), _value.col),
//         };
//     }
//
//     void repr(Io::Emit& e) const override {
//         e("{}", _value);
//     }
// };

} // namespace Vaev::Style
