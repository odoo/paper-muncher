module;

#include <karm/macros>

export module Vaev.Engine:props.multicol;

import Karm.Core;
import :props.base;
import :css.parser;
import :style.computed;
import :style.values.keywords;

using namespace Karm;
using namespace Karm::Literals;

namespace Vaev::Style {

// https://drafts.csswg.org/css-multicol-2/#cw
export struct ColumnWidthProperty : Property {
    using Value = Union<Keywords::Auto, Css::Length>;

    struct Registration : Property::Registration {
        Registration()
            : Property::Registration(Properties::COLUMN_WIDTH) {}

        InitialValue initial() const override {
            return "auto"s;
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<ColumnWidthProperty>(self(), toSpecified(Value{c.column->width}));
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<ColumnWidthProperty>(self(), try$(parseValue<Value>(c))));
        }
    };

    Value _value;

    ColumnWidthProperty(Rc<Property::Registration> registration, Value value)
        : Property(registration), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, ComputationContext const& cx) const override {
        c.column.cow().width = toComputed(_value);
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return Css::serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://drafts.csswg.org/css-multicol-2/#ch
export struct ColumnHeightProperty : Property {
    using Value = Union<Keywords::Auto, Css::Length>;

    struct Registration : Property::Registration {
        Registration()
            : Property::Registration(Properties::COLUMN_HEIGHT) {}

        InitialValue initial() const override {
            return "auto"s;
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<ColumnHeightProperty>(self(), Value{toSpecified(c.column->height)});
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<ColumnHeightProperty>(self(), try$(parseValue<Value>(c))));
        }
    };

    Value _value;

    ColumnHeightProperty(Rc<Property::Registration> registration, Value value)
        : Property(registration), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, ComputationContext const& cx) const override {
        c.column.cow().height = toComputed(_value);
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return Css::serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://drafts.csswg.org/css-multicol-2/#cc
export struct ColumnCountProperty : Property {
    using Value = Css::ColumnCount;

    struct Registration : Property::Registration {
        Registration()
            : Property::Registration(Properties::COLUMN_COUNT) {}

        InitialValue initial() const override {
            return "auto"s;
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<ColumnCountProperty>(self(), c.column->count);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<ColumnCountProperty>(self(), try$(parseValue<Value>(c))));
        }
    };

    Value _value;

    ColumnCountProperty(Rc<Property::Registration> registration, Value value)
        : Property(registration), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.column.cow().count = _value;
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return Css::serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://drafts.csswg.org/css-multicol-2/#columns
export struct ColumnsProperty : Property {
    struct Value {
        ColumnWidthProperty::Value width;
        ColumnCountProperty::Value count;
        ColumnHeightProperty::Value height;
    };

    struct Registration : Property::Registration {
        Registration()
            : Property::Registration(Properties::COLUMNS, SHORTHAND) {}

        InitialValue initial() const override {
            return "auto"s;
        }

        Rc<Property> load(ComputedValues const& c) const override {
            ColumnWidthProperty::Value width = toSpecified(c.column->width);
            ColumnHeightProperty::Value height = toSpecified(c.column->height);
            return makeRc<ColumnsProperty>(self(), Tuple{.v0 = width, .v1 = c.column->count, .v2 = height});
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            if (c.ended())
                return Error::invalidData("unexpected end of input");

            Tuple<ColumnWidthProperty::Value, Css::ColumnCount, ColumnHeightProperty::Value> value{Keywords::AUTO, Keywords::AUTO, Keywords::AUTO};

            int i = 0;
            while (not c.ended() and c.peek() != Css::Token::delim("/")) {
                if (++i > 2)
                    break;

                // auto is ambiguous, as auto is a valid value for both column-width and column-count.
                if (parseValue<Keywords::Auto>(c)) {
                    continue;
                }

                if (value.v0 == Keywords::AUTO) {
                    if (auto width = parseValue<ColumnWidthProperty::Value>(c)) {
                        value.v0 = width.take();
                        continue;
                    }
                }

                if (value.v1 == Keywords::AUTO) {
                    if (auto count = parseValue<Css::ColumnCount>(c)) {
                        value.v1 = count.take();
                        continue;
                    }
                }

                return Error::invalidData("unexpected token");
            }

            if (i == 0)
                return Error::invalidData("at least one column-width or column-count value must be present");

            if (c.skip(Css::Token::delim("/"))) {
                if (auto height = parseValue<ColumnHeightProperty::Value>(c))
                    value.v2 = height.take();
                else
                    return Error::invalidData("unexpected token");
            }

            return Ok(makeRc<ColumnsProperty>(self(), value));
        }
    };

    Value _value;

    ColumnsProperty(Rc<Property::Registration> registration, Value value)
        : Property(registration), _value(value) {}

    // FIXME: This property should also resets the column-wrap property to its initial value.
    Longhands expand(RegisteredPropertySet& registry, ComputedValues const&, ComputedValues&) const override {
        return {
            makeRc<ColumnWidthProperty>(registry.resolveRegistration(Properties::COLUMN_WIDTH, {}).expect(), _value.width),
            makeRc<ColumnCountProperty>(registry.resolveRegistration(Properties::COLUMN_COUNT, {}).expect(), _value.count),
            makeRc<ColumnHeightProperty>(registry.resolveRegistration(Properties::COLUMN_HEIGHT, {}).expect(), _value.height),
        };
    }

    // https://drafts.csswg.org/cssom/#serialize-a-css-value
    Res<> serialize(Io::TextWriter& w) const override {
        // If component values can be omitted or replaced with a shorter representation without
        // changing the meaning of the value, omit/replace them.
        if (_value.width == Keywords::AUTO and _value.width == Keywords::AUTO)
            try$(Css::serializeValue(w, _value.width));
        else if (_value.width == Keywords::AUTO)
            try$(Css::serializeValue(w, _value.count));
        else if (_value.count == Keywords::AUTO)
            try$(Css::serializeValue(w, _value.width));
        else {
            // If certain component values can appear in any order without changing the meaning of the value
            // (a pattern typically represented by a double bar || in the value syntax), reorder the component
            // values to use the canonical order of component values as given in the property definition table.
            try$(Css::serializeValue(w, _value.width));
            try$(w.writeRune(' '));
            try$(Css::serializeValue(w, _value.count));
        }

        if (_value.height != Keywords::AUTO) {
            try$(w.writeStr(" / "s));
            try$(Css::serializeValue(w, _value.height));
        }

        return Ok();
    }

    void repr(Io::Emit& e) const override {
        e("{} {} {}", _value.width, _value.count, _value.height);
    }
};

} // namespace Vaev::Style
