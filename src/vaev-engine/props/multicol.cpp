module;

#include <karm/macros>

export module Vaev.Engine:props.multicol;

import Karm.Core;
import :props.base;
import :css.parser;
import :style.computed;

using namespace Karm;

namespace Vaev::Style {

// https://drafts.csswg.org/css-multicol-2/#cw
export struct ColumnWidthProperty : Property {
    struct Registration : Property::Registration {
        Registration()
            : Property::Registration(Properties::COLUMN_WIDTH) {}

        Rc<Property> initial() const override {
            return makeRc<ColumnWidthProperty>(self(), Keywords::AUTO);
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<ColumnWidthProperty>(
                self(),
                c.column->width.visit(
                    [](Keywords::Auto) -> Css::ColumnWidth {
                        return Keywords::AUTO;
                    },
                    [](Au width) -> Css::ColumnWidth {
                        return toSpecified(Pixel{width.cast<f64>()});
                    }
                )
            );
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<ColumnWidthProperty>(self(), try$(parseValue<Css::ColumnWidth>(c))));
        }
    };

    Css::ColumnWidth _value;

    ColumnWidthProperty(Rc<Property::Registration> registration, Css::ColumnWidth value)
        : Property(registration), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, ComputationContext const& cx) const override {
        // Computed value: the keyword `auto` or an absolute length
        // FIXME: Clamp negative calc() results to 0.
        c.column.cow().width = _value.visit(
            [](Keywords::Auto) -> Union<Keywords::Auto, Au> {
                return Keywords::AUTO;
            },
            [&](Css::Length const& width) -> Union<Keywords::Auto, Au> {
                return Au::fromFloatNearest(toComputed(width, cx).value());
            }
        );
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://drafts.csswg.org/css-multicol-2/#ch
export struct ColumnHeightProperty : Property {
    struct Registration : Property::Registration {
        Registration()
            : Property::Registration(Properties::COLUMN_HEIGHT) {}

        Rc<Property> initial() const override {
            return makeRc<ColumnHeightProperty>(self(), Keywords::AUTO);
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<ColumnHeightProperty>(
                self(),
                c.column->height.visit(
                    [](Keywords::Auto) -> Css::ColumnHeight {
                        return Keywords::AUTO;
                    },
                    [](Au height) -> Css::ColumnHeight {
                        return toSpecified(Pixel{height.cast<f64>()});
                    }
                )
            );
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<ColumnHeightProperty>(self(), try$(parseValue<Css::ColumnHeight>(c))));
        }
    };

    Css::ColumnHeight _value;

    ColumnHeightProperty(Rc<Property::Registration> registration, Css::ColumnHeight value)
        : Property(registration), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, ComputationContext const& cx) const override {
        // Computed value: the keyword auto or an absolute length
        // FIXME: Clamp negative calc() results to 0.
        c.column.cow().height = _value.visit(
            [](Keywords::Auto) -> Union<Keywords::Auto, Au> {
                return Keywords::AUTO;
            },
            [&](Css::Length const& height) -> Union<Keywords::Auto, Au> {
                return Au::fromFloatNearest(toComputed(height, cx).value());
            }
        );
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://drafts.csswg.org/css-multicol-2/#cc
export struct ColumnCountProperty : Property {
    struct Registration : Property::Registration {
        Registration()
            : Property::Registration(Properties::COLUMN_COUNT) {}

        Rc<Property> initial() const override {
            return makeRc<ColumnCountProperty>(self(), Keywords::AUTO);
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<ColumnCountProperty>(self(), c.column->count);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<ColumnCountProperty>(self(), try$(parseValue<Css::ColumnCount>(c))));
        }
    };

    Css::ColumnCount _value;

    ColumnCountProperty(Rc<Property::Registration> registration, Css::ColumnCount value)
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
    struct Registration : Property::Registration {
        Registration()
            : Property::Registration(Properties::COLUMNS, SHORTHAND) {}

        Rc<Property> initial() const override {
            return makeRc<ColumnsProperty>(self(), Tuple<Css::ColumnWidth, Css::ColumnCount, Css::ColumnHeight>{.v0 = Keywords::AUTO, .v1 = Keywords::AUTO, .v2 = Keywords::AUTO});
        }

        Rc<Property> load(ComputedValues const& c) const override {
            auto width = c.column->width.visit(
                [](Keywords::Auto) -> Css::ColumnWidth {
                    return Keywords::AUTO;
                },
                [](Au width) -> Css::ColumnWidth {
                    return toSpecified(Pixel{width.cast<f64>()});
                }
            );

            auto height = c.column->height.visit(
                [](Keywords::Auto) -> Css::ColumnHeight {
                    return Keywords::AUTO;
                },
                [](Au height) -> Css::ColumnHeight {
                    return toSpecified(Pixel{height.cast<f64>()});
                }
            );

            return makeRc<ColumnsProperty>(self(), Tuple<Css::ColumnWidth, Css::ColumnCount, Css::ColumnHeight>{.v0 = width, .v1 = c.column->count, .v2 = height});
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            if (c.ended())
                return Error::invalidData("unexpected end of input");

            Tuple<Css::ColumnWidth, Css::ColumnCount, Css::ColumnHeight> value{Keywords::AUTO, Keywords::AUTO, Keywords::AUTO};

            int i = 0;
            while (not c.ended() and c.peek() != Css::Token::delim("/")) {
                if (++i > 2)
                    break;

                // auto is ambiguous, as auto is a valid value for both column-width and column-count.
                if (parseValue<Keywords::Auto>(c)) {
                    continue;
                }

                if (value.v0 == Keywords::AUTO) {
                    if (auto width = parseValue<Css::ColumnWidth>(c)) {
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
                if (auto height = parseValue<Css::ColumnHeight>(c))
                    value.v2 = height.take();
                else
                    return Error::invalidData("unexpected token");
            }

            return Ok(makeRc<ColumnsProperty>(self(), value));
        }
    };

    Tuple<Css::ColumnWidth, Css::ColumnCount, Css::ColumnHeight> _value;

    ColumnsProperty(Rc<Property::Registration> registration, Tuple<Css::ColumnWidth, Css::ColumnCount, Css::ColumnHeight> value)
        : Property(registration), _value(value) {}

    // FIXME: This property should also resets the column-wrap property to its initial value.
    Longhands expand(RegisteredPropertySet& registry, ComputedValues const&, ComputedValues&) const override {
        return {
            makeRc<ColumnWidthProperty>(registry.resolveRegistration(Properties::COLUMN_WIDTH, {}).expect(), _value.v0),
            makeRc<ColumnCountProperty>(registry.resolveRegistration(Properties::COLUMN_COUNT, {}).expect(), _value.v1),
            makeRc<ColumnHeightProperty>(registry.resolveRegistration(Properties::COLUMN_HEIGHT, {}).expect(), _value.v2),
        };
    }

    // https://drafts.csswg.org/cssom/#serialize-a-css-value
    Res<> serialize(Io::TextWriter& w) const override {
        // If component values can be omitted or replaced with a shorter representation without
        // changing the meaning of the value, omit/replace them.
        if (_value.v0 == Keywords::AUTO and _value.v1 == Keywords::AUTO)
            try$(serializeValue(w, _value.v0));
        else if (_value.v0 == Keywords::AUTO)
            try$(Css::serializeValue(w, _value.v1));
        else if (_value.v1 == Keywords::AUTO)
            try$(serializeValue(w, _value.v0));
        else {
            // If certain component values can appear in any order without changing the meaning of the value
            // (a pattern typically represented by a double bar || in the value syntax), reorder the component
            // values to use the canonical order of component values as given in the property definition table.
            try$(serializeValue(w, _value.v0));
            try$(w.writeRune(' '));
            try$(Css::serializeValue(w, _value.v1));
        }

        if (_value.v2 != Keywords::AUTO) {
            try$(w.writeStr(" / "s));
            try$(serializeValue(w, _value.v2));
        }

        return Ok();
    }

    void repr(Io::Emit& e) const override {
        e("{} {} {}", _value.v0, _value.v1, _value.v2);
    }
};

} // namespace Vaev::Style
