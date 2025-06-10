#pragma once

#include "length.h"
#include "percent.h"
#include "width.h"

namespace Vaev {

// MARK: Table Layout ----------------------------------------------------------
// https://www.w3.org/TR/CSS21/tables.html#propdef-table-layout

enum struct TableLayout {
    AUTO,
    FIXED,

    _LEN
};

template <>
struct ValueParser<TableLayout> {
    static Res<TableLayout> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c.skip(Css::Token::ident("auto"))) {
            return Ok(TableLayout::AUTO);
        } else if (c.skip(Css::Token::ident("fixed"))) {
            return Ok(TableLayout::FIXED);
        }

        return Error::invalidData("expected table layout value");
    }
};

// MARK: Caption Side ----------------------------------------------------------
// https://www.w3.org/TR/CSS21/tables.html#caption-position

enum struct CaptionSide {
    TOP,
    BOTTOM,

    _LEN
};

template <>
struct ValueParser<CaptionSide> {
    static Res<CaptionSide> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c.skip(Css::Token::ident("top"))) {
            return Ok(CaptionSide::TOP);
        } else if (c.skip(Css::Token::ident("bottom"))) {
            return Ok(CaptionSide::BOTTOM);
        }

        return Error::invalidData("expected caption side value");
    }
};

// MARK: Border Collapse -------------------------------------------------------
// https://www.w3.org/TR/CSS22/tables.html#propdef-border-collapse

enum struct BorderCollapse {
    SEPARATE,
    COLLAPSE,
};

template <>
struct ValueParser<BorderCollapse> {
    static Res<BorderCollapse> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c.skip(Css::Token::ident("collapse"))) {
            return Ok(BorderCollapse::COLLAPSE);
        } else if (c.skip(Css::Token::ident("separate"))) {
            return Ok(BorderCollapse::SEPARATE);
        }

        return Error::invalidData("expected border collapse value");
    }
};

// MARK: Border Spacing --------------------------------------------------------
// https://www.w3.org/TR/CSS22/tables.html#propdef-border-spacing

struct BorderSpacing {
    Length horizontal, vertical;

    void repr(Io::Emit& e) const {
        e("({}, {})", horizontal, vertical);
    }
};

template <>
struct ValueParser<BorderSpacing> {
    static Res<BorderSpacing> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        auto firstLength = parseValue<Length>(c);

        if (not firstLength)
            return Error::invalidData("expected length parameter for border-spacing");

        auto secondLength = parseValue<Length>(c);

        if (secondLength) {
            return Ok(BorderSpacing{firstLength.unwrap(), secondLength.unwrap()});
        } else {
            return Ok(BorderSpacing{firstLength.unwrap(), firstLength.unwrap()});
        }

        return Error::invalidData("expected border spacing value");
    }
};

struct TableProps {
    TableLayout tableLayout = TableLayout::AUTO;
    CaptionSide captionSide = CaptionSide::TOP;
    BorderSpacing spacing = {0_au, 0_au};
    BorderCollapse collapse = BorderCollapse::SEPARATE;

    void repr(Io::Emit& e) const {
        e("(table");
        e(" tableLayout={}", tableLayout);
        e(" captionSide={}", captionSide);
        e(" spacing={}", spacing);
        e(" collapse={}", collapse);
        e(")");
    }
};

} // namespace Vaev
