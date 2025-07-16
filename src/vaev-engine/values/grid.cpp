module;

#include <karm-base/distinct.h>
#include <karm-base/symbol.h>
#include <karm-base/union.h>
#include <karm-io/aton.h>
#include <karm-math/fixed.h>

export module Vaev.Engine:values.grid;

import :values.primitives;
import :values.keywords;
import :values.percent;
import :values.length;
import :values.calc;
import :values.sizing;

namespace Vaev {

struct GridTrackList;

// MARK: Track List ------------------------------------------------------------
// https://www.w3.org/TR/css-grid-1/#track-list

// https://www.w3.org/TR/css-grid-1/#fr-unit
using GridFlex = Distinct<f64, struct _GridFlexTag>;

template <>
struct ValueParser<GridFlex> {
    static Res<GridFlex> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c.peek() != Css::Token::DIMENSION)
            return Error::invalidData("expected dimension");

        Io::SScan scan = c->token.data.str();
        auto value = Io::atof(scan, {.allowExp = false}).unwrapOr(0.0);

        auto unit = scan.remStr();
        if (unit != "fr")
            return Error::invalidData("expected flexible length");

        c.next();
        return Ok(GridFlex{value});
    }
};

// https://www.w3.org/TR/css-grid-1/#typedef-track-breadth
using GridBreath = Union<
    CalcValue<PercentOr<Length>>,
    GridFlex,
    Keywords::MinContent,
    Keywords::MaxContent,
    Keywords::Auto>;

// https://www.w3.org/TR/css-grid-1/#funcdef-grid-template-columns-minmax
struct GridMinMax {
    GridBreath min;
    GridBreath max;
};

template <>
struct ValueParser<GridMinMax> {
    static Res<GridMinMax> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (*c != Css::Token::function("minmax("))
            return Error::invalidData("expected minmax() function");

        Cursor<Css::Sst> content = c->content;

        eatWhitespace(content);
        auto min = try$(parseValue<GridBreath>(content));
        skipOmmitableComma(content);
        auto max = try$(parseValue<GridBreath>(content));
        eatWhitespace(content);

        if (not content.ended())
            return Error::invalidData("unexpected token at the end of minmax()");

        c.next();
        return Ok(GridMinMax{std::move(min), std::move(max)});
    }
};

// https://www.w3.org/TR/css-grid-1/#repeat-notation
// https://www.w3.org/TR/css-grid-1/#repeat-syntax

using GridRepeatNumber = Union<
    Integer,
    Keywords::AutoFill,
    Keywords::AutoFit>;

struct GridRepeat {
    GridRepeatNumber number;
    Box<GridTrackList> tracks;
};

// https://www.w3.org/TR/css-grid-1/#typedef-track-size
// <track-size>          = <track-breadth> | minmax( <inflexible-breadth> , <track-breadth> ) | fit-content( <length-percentage [0,∞ ]> )
// <fixed-size>          = <fixed-breadth> | minmax( <fixed-breadth> , <track-breadth> ) | minmax( <inflexible-breadth> , <fixed-breadth> )
using GridTrackSize = FlatUnion<
    GridBreath,
    GridMinMax,
    FitContent>;

// <track-list>          = [ <line-names>? [ <track-size> | <track-repeat> ] ]+ <line-names>?
// <auto-track-list>     = [ <line-names>? [ <fixed-size> | <fixed-repeat> ] ]* <line-names>? <auto-repeat>
//                         [ <line-names>? [ <fixed-size> | <fixed-repeat> ] ]* <line-names>?
// <explicit-track-list> = [ <line-names>? <track-size> ]+ <line-names>?

struct GridLineNames {
    Vec<CustomIdent> names;
};

struct GridTrackList {
    struct Line {
        Vec<CustomIdent> names;
        Union<GridTrackSize, GridRepeat> size;
    };

    Vec<Track> tracks;
    GridRepeat autoRepeat;
};

Res<Symbol> _parseLineNames() {}

template <>
struct ValueParser<GridTrackList> {
    static Res<GridTrackList> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");
    }
};

template <>
struct ValueParser<GridRepeat> {
    static Res<GridRepeat> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (*c != Css::Token::function("repeat("))
            return Error::invalidData("expected repeat() function");

        Cursor<Css::Sst> content = c->content;
        eatWhitespace(content);
        auto number = try$(parseValue<GridRepeatNumber>(content));
        skipOmmitableComma(content);
        auto tracks = try$(parseValue<GridTrackList>(content));
        eatWhitespace(content);

        if (not content.ended())
            return Error::invalidData("unexpected token at the end of repeat()");

        c.next();
        return Ok(GridRepeat{std::move(number), std::move(tracks)});
    }
};

// https://www.w3.org/TR/css-grid-1/#grid-auto-flow-property
export struct GridAutoFlow {
    enum struct PlaceItem {
        ROW,
        COLUMN,
    };

    using enum PlaceItem;

    PlaceItem place;
    bool dense = false;

    GridAutoFlow(PlaceItem place)
        : place(place) {}

    GridAutoFlow(PlaceItem place, bool dense)
        : place(place), dense(dense) {}

    bool sparse() const {
        return not dense;
    }
};

template <>
struct ValueParser<GridAutoFlow> {
    static Res<GridAutoFlow> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        Opt<GridAutoFlow::PlaceItem> place;
        bool dense = false;

        while (true) {
            if (not place and c.skip(Css::Token::ident("row")))
                place = GridAutoFlow::ROW;
            else if (not place and c.skip(Css::Token::ident("column")))
                place = GridAutoFlow::COLUMN;
            else if (not dense and c.skip(Css::Token::ident("dense")))
                dense = true;
            else
                break;
        }

        if (not place and not dense)
            return Error::invalidData("expected place and/or dense");

        return Ok(GridAutoFlow{place.unwrapOr(GridAutoFlow::ROW), dense});
    }
};

// MARK: Implicit/Explicit Grid ------------------------------------------------

export struct GridAreaRow {
    Vec<Symbol> cols;
};

// https://www.w3.org/TR/css-grid-1/#valdef-grid-template-areas-string
static Res<GridAreaRow> _parseAreaRow(Str str) {
    Io::SScan s{str};
    GridAreaRow row;

    s.eat(Css::RE_WHITESPACE);
    while (auto col = s.token(Css::RE_IDENTIFIER)) {
        s.eat(Css::RE_WHITESPACE);
        row.cols.pushBack(Symbol::from(col));
    }

    if (not s.ended())
        return Error::invalidData("unexpected codepoint in grid area row");

    return Ok(std::move(row));
}

using GridAreas = Vec<GridAreaRow>;

template <>
struct ValueParser<GridAreas> {
    static Res<GridAreas> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c.skip(Css::Token::ident("none")))
            return Ok();

        GridAreas res;
        while (not c.ended() and *c == Css::Token::STRING) {
            res.pushBack(try$(_parseAreaRow(c->token.data)));
            c.next();
            eatWhitespace(c);
        }

        if (not res.len())
            return Error::invalidData("expected at least one grid area");

        return Ok(std::move(res));
    }
};

export struct ExplicitGridProps {
    Union<Keywords::None, GridAreas> areas;
    Union<Keywords::None, GridTrackList> rows, columns;
};

export struct ImplicitGridProps {
    GridAutoFlow flow;
    Vec<GridTrackSize> rows, columns;
};

// MARK: Grid Line -------------------------------------------------------------

// https://www.w3.org/TR/css-grid-1/#typedef-grid-row-start-grid-line
struct GridLine {
    enum struct Type {
        AUTO,
        // https://www.w3.org/TR/css-grid-1/#grid-placement-int
        NTH,
        // https://www.w3.org/TR/css-grid-1/#grid-placement-span-int
        SPAN,
    };
    using enum Type;

    Type type;
    Integer span = 0;
    Opt<CustomIdent> area = NONE;

    GridLine(Type type = GridLine::AUTO)
        : type(type) {}
};

template <>
struct ValueParser<GridLine> {
    static Res<GridLine> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (parseValue<Keywords::None>(c))
            return Ok(GridLine::AUTO);

        Opt<Integer> integer;
        Opt<CustomIdent> customIdent;
        bool span = false;

        while (true) {
            eatWhitespace(c);
            if (not span and c.skip(Css::Token::ident("span"))) {
                span = true;
            } else if (not integer) {
                integer = try$(parseValue<Integer>(c));
            } else if (not customIdent) {
                customIdent = try$(parseValue<CustomIdent>(c));
            } else
                break;
        }

        GridLine line;
        if (span) {
            // https://www.w3.org/TR/css-grid-1/#grid-placement-span-int
            if (integer.unwrapOr(1) <= 0)
                return Error::invalidData("negative or zero span are invalid");
            line.type = GridLine::SPAN;
            line.span = integer.unwrapOr(1);
            line.area = customIdent;
        } else {
            // https://www.w3.org/TR/css-grid-1/#grid-placement-int
            if (integer)
                return Error::invalidData("nth is required");
            if (integer.unwrapOr(0) <= 0)
                return Error::invalidData("zero nth is invalid");
            line.type = GridLine::NTH;
            line.span = integer.unwrapOr(0);
            line.area = customIdent;
        }

        return Ok(std::move(line));
    }
};

// MARK: Grid ------------------------------------------------------------------

// https://www.w3.org/TR/css-grid-1/#propdef-grid-area
export struct GridArea {
    // NOTE: This has to be kept in the same order as the property grid-area
    GridLine rowStart = GridLine::AUTO;
    GridLine columnStart = GridLine::AUTO;
    GridLine rowEnd = GridLine::AUTO;
    GridLine columnEnd = GridLine::AUTO;

    GridLine& operator[](usize i) {
        return MutSlice{&rowStart, 4}[i];
    }
};

static_assert(sizeof(GridArea) == sizeof(GridLine[4]));

export struct GridProps {
    ExplicitGridProps explict;
    ImplicitGridProps implicit;
    GridArea area;
};

} // namespace Vaev
