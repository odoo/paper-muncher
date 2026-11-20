module;

#include <karm/macros>

export module Vaev.Engine:props.table;

import Karm.Core;
import :props.base;
import :css.parser;
import :style.computed;
import :style.values.keywords;

using namespace Karm;
using namespace Karm::Literals;

namespace Vaev::Style {

// https://www.w3.org/TR/CSS21/tables.html#propdef-table-layout
export struct TableLayoutProperty : Property {
    using Value = Css::TableLayout;

    struct Registration : Property::Registration {
        Registration() : Property::Registration(Properties::TABLE_LAYOUT) {}

        InitialValue initialValue() const override {
            return "auto"s;
        }

        Rc<Property> load(ComputedValues const& s) const override {
            return makeRc<TableLayoutProperty>(self(), s.tableLayout);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<TableLayoutProperty>(self(), try$(parseValue<Value>(c))));
        }
    };

    Value _value;

    TableLayoutProperty(Rc<Property::Registration> registration, Value value)
        : Property(registration), _value(std::move(value)) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.tableLayout = _value;
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return Css::serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/CSS21/tables.html#caption-position
export struct CaptionSideProperty : Property {
    using Value = Css::CaptionSide;

    struct Registration : Property::Registration {
        Registration()
            : Property::Registration(Properties::CAPTION_SIDE, {INHERITED, BULK_INHERITED}) {}

        InitialValue initialValue() const override {
            return "top"s;
        }

        Rc<Property> load(ComputedValues const& s) const override {
            return makeRc<CaptionSideProperty>(self(), s.table->captionSide);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<CaptionSideProperty>(self(), try$(parseValue<Value>(c))));
        }
    };

    Value _value;

    CaptionSideProperty(Rc<Property::Registration> registration, Value value)
        : Property(registration), _value(std::move(value)) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.table.cow().captionSide = _value;
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return Css::serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

} // namespace Vaev::Style
