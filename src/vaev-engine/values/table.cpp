export module Vaev.Engine:values.table;

import Karm.Core;
import Karm.Math;

import :css;
import :values.base;
import :values.length;
import :values.percent;

using namespace Karm;
using namespace Karm::Math::Literals;

namespace Vaev {

// MARK: Table Layout ----------------------------------------------------------
// https://www.w3.org/TR/CSS21/tables.html#propdef-table-layout

export enum struct TableLayout {
    AUTO,
    FIXED,

    _LEN
};

export template <>
struct ValueTraits<TableLayout> : EnumValueTraits<TableLayout> {
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

export enum struct CaptionSide {
    TOP,
    BOTTOM,

    _LEN
};

export template <>
struct ValueTraits<CaptionSide> : EnumValueTraits<CaptionSide> {
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

export enum struct BorderCollapse {
    SEPARATE,
    COLLAPSE,
};

export template <>
struct ValueTraits<BorderCollapse> : EnumValueTraits<BorderCollapse> {
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

export struct BorderSpacing {
    Length horizontal, vertical;

    void repr(Io::Emit& e) const {
        e("({}, {})", horizontal, vertical);
    }
};

export struct ComputedBorderSpacing {
    Au horizontal, vertical;

    void repr(Io::Emit& e) const {
        e("({}, {})", horizontal, vertical);
    }
};

export template <>
struct ValueTraits<BorderSpacing> {
    using Computed = ComputedBorderSpacing;

    static BorderSpacing specify(ComputedBorderSpacing const& value) {
        return {value.horizontal, value.vertical};
    }

    static Res<BorderSpacing> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        auto firstLength = parseValue<Length>(c);

        if (not firstLength)
            return Error::invalidData("expected length parameter for border-spacing");

        auto secondLength = parseValue<Length>(c);

        if (secondLength) {
            return Ok(BorderSpacing{firstLength.expect(), secondLength.expect()});
        } else {
            return Ok(BorderSpacing{firstLength.expect(), firstLength.expect()});
        }

        return Error::invalidData("expected border spacing value");
    }

    static ComputedBorderSpacing compute(BorderSpacing const& value, LengthContext auto const& ctx) {
        return {Vaev::compute(value.horizontal, ctx), Vaev::compute(value.vertical, ctx)};
    }
};

} // namespace Vaev
