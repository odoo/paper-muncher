module;

#include <karm/macros>

export module Vaev.Engine:props.page;

import Karm.Core;

import :props.base;
import :css.parser;
import :style.specified;
import :values.page;

using namespace Karm;

namespace Vaev::Style {

// MARK: Padding ---------------------------------------------------------------

// https://drafts.csswg.org/css-page/#page-size-prop
export struct PageSizeProperty : Property {
    struct Registration : Property::Registration {
        Symbol name() const override {
            return Properties::SIZE;
        }

        Rc<Property> initial() const override {
            return makeRc<PageSizeProperty>(self(), PageSize(Keywords::AUTO));
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<PageSizeProperty>(self(), c.page->size);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<PageSizeProperty>(self(), try$(parseValue<PageSize>(c))));
        }
    };

    PageSize _value;

    PageSizeProperty(Rc<Property::Registration> registration, PageSize value)
        : Property(registration), _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.page.cow().size = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

} // namespace Vaev::Style
