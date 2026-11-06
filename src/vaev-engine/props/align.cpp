module;

#include <karm/macros>

export module Vaev.Engine:props.align;

import Karm.Core;
import :props.base;
import :css.parser;
import :style.computed;

using namespace Karm;

namespace Vaev::Style {

// MARK: Align -----------------------------------------------------------------
// https://drafts.csswg.org/css-align-3

// https://drafts.csswg.org/css-align-3/#propdef-align-content
export struct AlignContentProperty : Property {
    struct Registration : Property::Registration {
        Registration() : Property::Registration(Properties::ALIGN_CONTENT) {}

        Rc<Property> initial() const override {
            return makeRc<AlignContentProperty>(self(), Align::Keywords::STRETCH);
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<AlignContentProperty>(self(), c.aligns.alignContent);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<AlignContentProperty>(self(), try$(parseValue<Align>(c))));
        }
    };

    Align _value;

    AlignContentProperty(Rc<Property::Registration> registration, Align value)
        : Property(registration), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.aligns.alignContent = _value;
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return Css::serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://drafts.csswg.org/css-align-3/#propdef-justify-content
export struct JustifyContentProperty : Property {
    struct Registration : Property::Registration {
        Registration() : Property::Registration(Properties::JUSTIFY_CONTENT) {}

        Rc<Property> initial() const override {
            return makeRc<JustifyContentProperty>(self(), Align::Keywords::FLEX_START);
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<JustifyContentProperty>(self(), c.aligns.justifyContent);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<JustifyContentProperty>(self(), try$(parseValue<Align>(c))));
        }
    };

    Align _value;

    JustifyContentProperty(Rc<Property::Registration> registration, Align value)
        : Property(registration), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.aligns.justifyContent = _value;
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return Css::serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://drafts.csswg.org/css-align-3/#propdef-justify-self
export struct JustifySelfProperty : Property {
    struct Registration : Property::Registration {
        Registration() : Property::Registration(Properties::JUSTIFY_SELF) {}

        Rc<Property> initial() const override {
            return makeRc<JustifySelfProperty>(self(), Align::Keywords::AUTO);
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<JustifySelfProperty>(self(), c.aligns.justifySelf);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<JustifySelfProperty>(self(), try$(parseValue<Align>(c))));
        }
    };

    Align _value;

    JustifySelfProperty(Rc<Property::Registration> registration, Align value)
        : Property(registration), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.aligns.justifySelf = _value;
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return Css::serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://drafts.csswg.org/css-align-3/#propdef-align-self
export struct AlignSelfProperty : Property {
    struct Registration : Property::Registration {
        Registration() : Property::Registration(Properties::ALIGN_SELF) {}

        Rc<Property> initial() const override {
            return makeRc<AlignSelfProperty>(self(), Align::Keywords::AUTO);
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<AlignSelfProperty>(self(), c.aligns.alignSelf);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<AlignSelfProperty>(self(), try$(parseValue<Align>(c))));
        }
    };

    Align _value;

    AlignSelfProperty(Rc<Property::Registration> registration, Align value)
        : Property(registration), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.aligns.alignSelf = _value;
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return Css::serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://drafts.csswg.org/css-align-3/#propdef-justify-items
export struct JustifyItemsProperty : Property {
    struct Registration : Property::Registration {
        Registration() : Property::Registration(Properties::JUSTIFY_ITEMS) {}

        Rc<Property> initial() const override {
            return makeRc<JustifyItemsProperty>(self(), Align::LEGACY);
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<JustifyItemsProperty>(self(), c.aligns.justifyItems);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<JustifyItemsProperty>(self(), try$(parseValue<Align>(c))));
        }
    };

    Align _value;

    JustifyItemsProperty(Rc<Property::Registration> registration, Align value)
        : Property(registration), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.aligns.justifyItems = _value;
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return Css::serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://drafts.csswg.org/css-align-3/#propdef-align-items
export struct AlignItemsProperty : Property {
    struct Registration : Property::Registration {
        Registration() : Property::Registration(Properties::ALIGN_ITEMS) {}

        Rc<Property> initial() const override {
            return makeRc<AlignItemsProperty>(self(), Align::Keywords::STRETCH);
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<AlignItemsProperty>(self(), c.aligns.alignItems);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<AlignItemsProperty>(self(), try$(parseValue<Align>(c))));
        }
    };

    Align _value;

    AlignItemsProperty(Rc<Property::Registration> registration, Align value)
        : Property(registration), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.aligns.alignItems = _value;
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return Css::serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://drafts.csswg.org/css-align-3/#column-row-gap
export struct RowGapProperty : Property {
    struct Registration : Property::Registration {
        Registration() : Property::Registration(Properties::ROW_GAP) {}

        // https://drafts.csswg.org/css-align-3/#gap-legacy
        SmallVec<Symbol, 1> legacyNameAlias() const override {
            return {"grid-row-gap"_sym};
        }

        Rc<Property> initial() const override {
            return makeRc<RowGapProperty>(self(), Keywords::NORMAL);
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<RowGapProperty>(self(), toSpecified(c.gaps->row));
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<RowGapProperty>(self(), try$(parseValue<Css::Gap>(c))));
        }
    };

    Css::Gap _value;

    RowGapProperty(Rc<Property::Registration> registration, Css::Gap value)
        : Property(registration), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.gaps.cow().row = toComputed(_value, cx);
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
    struct Registration : Property::Registration {
        Registration() : Property::Registration(Properties::COLUMN_GAP) {}

        // https://drafts.csswg.org/css-align-3/#gap-legacy
        SmallVec<Symbol, 1> legacyNameAlias() const override {
            return {"grid-column-gap"_sym};
        }

        Rc<Property> initial() const override {
            return makeRc<ColumnGapProperty>(self(), Keywords::NORMAL);
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<ColumnGapProperty>(self(), toSpecified(c.gaps->col));
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<ColumnGapProperty>(self(), try$(parseValue<Css::Gap>(c))));
        }
    };

    Css::Gap _value;

    ColumnGapProperty(Rc<Property::Registration> registration, Css::Gap value)
        : Property(registration), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.gaps.cow().col = toComputed(_value, cx);
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

export struct GapProperty : Property {
    // <'row-gap'> <'column-gap'>?
    struct Value {
        Css::Gap row = Keywords::NORMAL;
        Css::Gap col = Keywords::NORMAL;

        void repr(Io::Emit& e) const {
            e("(gaps {} {})", row, col);
        }
    };

    struct Registration : Property::Registration {
        Registration() : Property::Registration(Properties::GAP) {}

        // https://drafts.csswg.org/css-align-3/#gap-legacy
        SmallVec<Symbol, 1> legacyNameAlias() const override {
            return {"grid-gap"_sym};
        }

        Rc<Property> initial() const override {
            return makeRc<GapProperty>(
                self(),
                Value{
                    Keywords::NORMAL,
                    Keywords::NORMAL,
                }
            );
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<GapProperty>(self(), Value{toSpecified(c.gaps->row), toSpecified(c.gaps->col)});
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            auto rowGap = try$(parseValue<Css::Gap>(c));
            eatWhitespace(c);
            auto colGap = parseValue<Css::Gap>(c);

            if (not colGap)
                return Ok(makeRc<GapProperty>(self(), Value{rowGap, rowGap}));
            return Ok(makeRc<GapProperty>(self(), Value{rowGap, colGap.take()}));
        }
    };

    Value _value;

    GapProperty(Rc<Property::Registration> registration, Value value)
        : Property(registration), _value(value) {}

    Longhands expand(RegisteredPropertySet& registry, ComputedValues const&, ComputedValues&) const override {
        return {
            makeRc<RowGapProperty>(registry.resolveRegistration(Properties::ROW_GAP, {}).expect(), _value.row),
            makeRc<ColumnGapProperty>(registry.resolveRegistration(Properties::COLUMN_GAP, {}).expect(), _value.col),
        };
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

} // namespace Vaev::Style
