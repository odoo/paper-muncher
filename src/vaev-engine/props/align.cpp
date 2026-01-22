module;

#include <karm-core/macros.h>

export module Vaev.Engine:props.align;

import Karm.Core;
import :props.base;
import :css.parser;
import :style.specified;

using namespace Karm;

namespace Vaev::Style {

// MARK: Align -----------------------------------------------------------------
// https://drafts.csswg.org/css-align-3

// https://drafts.csswg.org/css-align-3/#propdef-align-content
export struct AlignContentProperty : Property {
    struct Registration : Property::Registration {
        Symbol name() const override {
            return Properties::ALIGN_CONTENT;
        }

        Rc<Property> initial() const override {
            return makeRc<AlignContentProperty>(self(), Align::Keywords::STRETCH);
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<AlignContentProperty>(self(), c.aligns.alignContent);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<AlignContentProperty>(self(), try$(parseValue<Align>(c))));
        }
    };

    Align _value;

    AlignContentProperty(Rc<Property::Registration> registration, Align value)
        : Property(registration), _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.aligns.alignContent = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://drafts.csswg.org/css-align-3/#propdef-justify-content
export struct JustifyContentProperty : Property {
    struct Registration : Property::Registration {
        Symbol name() const override {
            return Properties::JUSTIFY_CONTENT;
        }

        Rc<Property> initial() const override {
            return makeRc<JustifyContentProperty>(self(), Align::Keywords::FLEX_START);
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<JustifyContentProperty>(self(), c.aligns.justifyContent);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<JustifyContentProperty>(self(), try$(parseValue<Align>(c))));
        }
    };

    Align _value;

    JustifyContentProperty(Rc<Property::Registration> registration, Align value)
        : Property(registration), _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.aligns.justifyContent = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://drafts.csswg.org/css-align-3/#propdef-justify-self
export struct JustifySelfProperty : Property {
    struct Registration : Property::Registration {
        Symbol name() const override {
            return Properties::JUSTIFY_ITEMS;
        }

        Rc<Property> initial() const override {
            return makeRc<JustifySelfProperty>(self(), Align{});
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<JustifySelfProperty>(self(), c.aligns.justifySelf);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<JustifySelfProperty>(self(), try$(parseValue<Align>(c))));
        }
    };

    Align _value;

    JustifySelfProperty(Rc<Property::Registration> registration, Align value)
        : Property(registration), _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.aligns.justifySelf = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://drafts.csswg.org/css-align-3/#propdef-align-self
export struct AlignSelfProperty : Property {
    struct Registration : Property::Registration {
        Symbol name() const override {
            return Properties::ALIGN_SELF;
        }

        Rc<Property> initial() const override {
            return makeRc<AlignSelfProperty>(self(), Align::Keywords::AUTO);
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<AlignSelfProperty>(self(), c.aligns.alignSelf);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<AlignSelfProperty>(self(), try$(parseValue<Align>(c))));
        }
    };

    Align _value;

    AlignSelfProperty(Rc<Property::Registration> registration, Align value)
        : Property(registration), _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.aligns.alignSelf = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://drafts.csswg.org/css-align-3/#propdef-justify-items
export struct JustifyItemsProperty : Property {
    struct Registration : Property::Registration {
        Symbol name() const override {
            return Properties::JUSTIFY_ITEMS;
        }

        Rc<Property> initial() const override {
            return makeRc<JustifyItemsProperty>(self(), Align{});
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<JustifyItemsProperty>(self(), c.aligns.justifyItems);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<JustifyItemsProperty>(self(), try$(parseValue<Align>(c))));
        }
    };

    Align _value;

    JustifyItemsProperty(Rc<Property::Registration> registration, Align value)
        : Property(registration), _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.aligns.justifyItems = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://drafts.csswg.org/css-align-3/#propdef-align-items
export struct AlignItemsProperty : Property {
    struct Registration : Property::Registration {
        Symbol name() const override {
            return Properties::ALIGN_ITEMS;
        }

        Rc<Property> initial() const override {
            return makeRc<AlignItemsProperty>(self(), Align::Keywords::STRETCH);
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<AlignItemsProperty>(self(), c.aligns.alignItems);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<AlignItemsProperty>(self(), try$(parseValue<Align>(c))));
        }
    };

    Align _value;

    AlignItemsProperty(Rc<Property::Registration> registration, Align value)
        : Property(registration), _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.aligns.alignItems = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://drafts.csswg.org/css-align-3/#column-row-gap
export struct RowGapProperty : Property {
    struct Registration : Property::Registration {
        Symbol name() const override {
            return Properties::ROW_GAP;
        }

        // https://drafts.csswg.org/css-align-3/#gap-legacy
        Vec<Symbol> legacyAlias() const override {
            return {"grid-row-gap"_sym};
        }

        Rc<Property> initial() const override {
            return makeRc<RowGapProperty>(self(), Keywords::NORMAL);
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<RowGapProperty>(self(), c.gaps->row);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<RowGapProperty>(self(), try$(parseValue<Gap>(c))));
        }
    };

    Gap _value;

    RowGapProperty(Rc<Property::Registration> registration, Gap value)
        : Property(registration), _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.gaps.cow().row = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://drafts.csswg.org/css-align-3/#column-row-gap
export struct ColumnGapProperty : Property {
    struct Registration : Property::Registration {
        Symbol name() const override {
            return Properties::COLUMN_GAP;
        }

        // https://drafts.csswg.org/css-align-3/#gap-legacy
        Vec<Symbol> legacyAlias() const override {
            return {"grid-column-gap"_sym};
        }

        Rc<Property> initial() const override {
            return makeRc<ColumnGapProperty>(self(), Keywords::NORMAL);
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<ColumnGapProperty>(self(), c.gaps->col);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<ColumnGapProperty>(self(), try$(parseValue<Gap>(c))));
        }
    };

    Gap _value;

    ColumnGapProperty(Rc<Property::Registration> registration, Gap value)
        : Property(registration), _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.gaps.cow().col = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

export struct GapProperty : Property {
    struct Registration : Property::Registration {
        Symbol name() const override {
            return Properties::GAP;
        }

        // https://drafts.csswg.org/css-align-3/#gap-legacy
        Vec<Symbol> legacyAlias() const override {
            return {"grid-gap"_sym};
        }

        Rc<Property> initial() const override {
            return makeRc<GapProperty>(
                self(),
                Gaps{
                    Keywords::NORMAL,
                    Keywords::NORMAL,
                }
            );
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<GapProperty>(self(), *c.gaps);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            auto rowGap = try$(parseValue<Gap>(c));
            eatWhitespace(c);
            auto colGap = parseValue<Gap>(c);

            if (not colGap)
                return Ok(makeRc<GapProperty>(self(), Gaps{rowGap, rowGap}));
            return Ok(makeRc<GapProperty>(self(), Gaps{rowGap, colGap.take()}));
        }
    };

    Gaps _value;

    GapProperty(Rc<Property::Registration> registration, Gaps value)
        : Property(registration), _value(value) {}

    Vec<Rc<Property>> expandShorthand(PropertyRegistry& registry, SpecifiedValues const&, SpecifiedValues&) const override {
        return {
            makeRc<RowGapProperty>(registry.resolveRegistration(Properties::MARGIN_TOP, {}).unwrap(), _value.row),
            makeRc<ColumnGapProperty>(registry.resolveRegistration(Properties::MARGIN_TOP, {}).unwrap(), _value.col),
        };
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

} // namespace Vaev::Style
