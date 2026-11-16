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
    static constexpr auto ID = LonghandId::COLUMN_WIDTH;

    static Rc<Property> initial() {
        return makeRc<ColumnWidthProperty>(Keywords::AUTO);
    }

    static Rc<Property> load(ComputedValues const& c) {
        return makeRc<ColumnWidthProperty>(c.column->width.visit(
                [](Keywords::Auto) -> ColumnWidth {
                    return Keywords::AUTO;
                },
                [](Au width) -> ColumnWidth {
                    return Calc<Length>{width};
                }
            )
        );
    }

    static Res<Rc<Property>> parse(Cursor<Css::Sst>& c, ParsingMode) {
        return Ok(makeRc<ColumnWidthProperty>(try$(parseValue<ColumnWidth>(c))));
    }

    ColumnWidth _value;

    ColumnWidthProperty(ColumnWidth value)
        : Property(ID), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, ComputationContext const& cx) const override {
        // Computed value: the keyword `auto` or an absolute length
        // FIXME: Clamp negative calc() results to 0.
        c.column.cow().width = _value.visit(
            [](Keywords::Auto) -> Union<Keywords::Auto, Au> {
                return Keywords::AUTO;
            },
            [&](Calc<Length> const& width) -> Union<Keywords::Auto, Au> {
                return resolve(width, cx);
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
    static constexpr auto ID = LonghandId::COLUMN_HEIGHT;

    static Rc<Property> initial() {
        return makeRc<ColumnHeightProperty>(Keywords::AUTO);
    }

    static Rc<Property> load(ComputedValues const& c) {
        return makeRc<ColumnHeightProperty>(c.column->height.visit(
                [](Keywords::Auto) -> ColumnHeight {
                    return Keywords::AUTO;
                },
                [](Au height) -> ColumnHeight {
                    return Calc<Length>{height};
                }
            )
        );
    }

    static Res<Rc<Property>> parse(Cursor<Css::Sst>& c, ParsingMode) {
        return Ok(makeRc<ColumnHeightProperty>(try$(parseValue<ColumnHeight>(c))));
    }

    ColumnHeight _value;

    ColumnHeightProperty(ColumnHeight value)
        : Property(ID), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, ComputationContext const& cx) const override {
        // Computed value: the keyword auto or an absolute length
        // FIXME: Clamp negative calc() results to 0.
        c.column.cow().height = _value.visit(
            [](Keywords::Auto) -> Union<Keywords::Auto, Au> {
                return Keywords::AUTO;
            },
            [&](Calc<Length> const& height) -> Union<Keywords::Auto, Au> {
                return resolve(height, cx);
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
    static constexpr auto ID = LonghandId::COLUMN_COUNT;

    static Rc<Property> initial() {
        return makeRc<ColumnCountProperty>(Keywords::AUTO);
    }

    static Rc<Property> load(ComputedValues const& c) {
        return makeRc<ColumnCountProperty>(c.column->count);
    }

    static Res<Rc<Property>> parse(Cursor<Css::Sst>& c, ParsingMode) {
        return Ok(makeRc<ColumnCountProperty>(try$(parseValue<ColumnCount>(c))));
    }

    ColumnCount _value;

    ColumnCountProperty(ColumnCount value)
        : Property(ID), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.column.cow().count = _value;
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://drafts.csswg.org/css-multicol-2/#columns
export struct ColumnsShorthand {
    static constexpr auto ID = ShorthandId::COLUMNS;

    // FIXME: The columns shorthand also resets column-wrap to its initial value.
    static constexpr Array LONGHANDS = {
        LonghandId::COLUMN_WIDTH,
        LonghandId::COLUMN_COUNT,
        LonghandId::COLUMN_HEIGHT,
    };

    // [ <'column-width'> || <'column-count'> ] [ / <'column-height'> ]?
    static Res<> parse(Cursor<Css::Sst>& c, Vec<Rc<Property>>& stylesheet, ParsingMode) {
        Opt<ColumnWidth> width;
        Opt<ColumnCount> count;

        // NOTE: auto is valid for both column-width and column-count,
        //       it goes to the first one that is not set yet.
        while (true) {
            eatWhitespace(c);
            if (c.ended() or c.peek() == Css::Token::delim("/"))
                break;

            if (not width) {
                if (auto maybeWidth = parseValue<ColumnWidth>(c)) {
                    width = Some(maybeWidth.take());
                    continue;
                }
            }

            if (not count) {
                if (auto maybeCount = parseValue<ColumnCount>(c)) {
                    count = Some(maybeCount.take());
                    continue;
                }
            }

            break;
        }

        if (not width and not count)
            return Error::invalidData("expected column-width or column-count");

        Opt<ColumnHeight> height;
        if (c.skip(Css::Token::delim("/")))
            height = Some(try$(parseValue<ColumnHeight>(c)));

        // NOTE: Omitted values are reset to their initial value, auto.
        stylesheet.pushBack(makeRc<ColumnWidthProperty>(width.unwrapOr(Keywords::AUTO)));
        stylesheet.pushBack(makeRc<ColumnCountProperty>(count.unwrapOr(Keywords::AUTO)));
        stylesheet.pushBack(makeRc<ColumnHeightProperty>(height.unwrapOr(Keywords::AUTO)));
        return Ok();
    }
};

} // namespace Vaev::Style
