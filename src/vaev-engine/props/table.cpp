module;

#include <karm/macros>

export module Vaev.Engine:props.table;

import Karm.Core;
import :props.base;
import :css.parser;
import :style.computed;

using namespace Karm;

namespace Vaev::Style {

// https://www.w3.org/TR/CSS21/tables.html#propdef-table-layout
export struct TableLayoutProperty : Property {
    static constexpr auto ID = LonghandId::TABLE_LAYOUT;

    static Rc<Property> initial() {
        return makeRc<TableLayoutProperty>(TableLayout::AUTO);
    }

    static Rc<Property> load(ComputedValues const& s) {
        return makeRc<TableLayoutProperty>(s.tableLayout);
    }

    static Res<Rc<Property>> parse(Cursor<Css::Sst>& c, ParsingMode) {
        return Ok(makeRc<TableLayoutProperty>(try$(parseValue<TableLayout>(c))));
    }

    struct Registration : LonghandRegistration {
        Registration() : LonghandRegistration(Properties::TABLE_LAYOUT) {}
    };

    TableLayout _value;

    TableLayoutProperty(TableLayout value)
        : Property(ID), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.tableLayout = _value;
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/CSS21/tables.html#caption-position
export struct CaptionSideProperty : Property {
    static constexpr auto ID = LonghandId::CAPTION_SIDE;
    static constexpr Flags<Options> FLAGS = {INHERITED, BULK_INHERITED};

    static Rc<Property> initial() {
        return makeRc<CaptionSideProperty>(CaptionSide::TOP);
    }

    static Rc<Property> load(ComputedValues const& s) {
        return makeRc<CaptionSideProperty>(s.table->captionSide);
    }

    static Res<Rc<Property>> parse(Cursor<Css::Sst>& c, ParsingMode) {
        return Ok(makeRc<CaptionSideProperty>(try$(parseValue<CaptionSide>(c))));
    }

    struct Registration : LonghandRegistration {
        Registration()
            : LonghandRegistration(Properties::CAPTION_SIDE, FLAGS) {}
    };

    CaptionSide _value;

    CaptionSideProperty(CaptionSide value)
        : Property(ID), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.table.cow().captionSide = _value;
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

} // namespace Vaev::Style
