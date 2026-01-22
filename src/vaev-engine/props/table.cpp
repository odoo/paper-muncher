module;

#include <karm-core/macros.h>

export module Vaev.Engine:props.table;

import Karm.Core;
import :props.base;
import :css.parser;
import :style.specified;

using namespace Karm;

namespace Vaev::Style {

// https://www.w3.org/TR/CSS21/tables.html#propdef-table-layout
export struct TableLayoutProperty : Property {
    struct Registration : Property::Registration {
        Symbol name() const override {
            return Properties::TABLE_LAYOUT;
        }

        Rc<Property> initial() const override {
            return makeRc<TableLayoutProperty>(self(), TableLayout::AUTO);
        }

        Rc<Property> load(SpecifiedValues const& s) const override {
            return makeRc<TableLayoutProperty>(self(), s.table->tableLayout);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<TableLayoutProperty>(self(), try$(parseValue<TableLayout>(c))));
        }
    };

    TableLayout _value;

    TableLayoutProperty(Rc<Property::Registration> registration, TableLayout value)
        : Property(registration), _value(value) {}

    void apply(SpecifiedValues& s) const override {
        s.table.cow().tableLayout = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/CSS21/tables.html#caption-position
export struct CaptionSideProperty : Property {
    struct Registration : Property::Registration {
        Symbol name() const override {
            return Properties::CAPTION_SIDE;
        }

        Flags<Options> flags() const override {
            return {INHERITED};
        }

        Rc<Property> initial() const override {
            return makeRc<CaptionSideProperty>(self(), CaptionSide::TOP);
        }

        Rc<Property> load(SpecifiedValues const& s) const override {
            return makeRc<CaptionSideProperty>(self(), s.table->captionSide);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<CaptionSideProperty>(self(), try$(parseValue<CaptionSide>(c))));
        }
    };

    CaptionSide _value;

    CaptionSideProperty(Rc<Property::Registration> registration, CaptionSide value)
        : Property(registration), _value(value) {}

    void apply(SpecifiedValues& s) const override {
        s.table.cow().captionSide = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

} // namespace Vaev::Style
