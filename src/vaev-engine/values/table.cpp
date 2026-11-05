module;

#include <karm/macros>

export module Vaev.Engine:values.table;

import Karm.Core;
import Karm.Math;

import :css;
import :values.base;
import :values.calc2;
import :values.length;
import :values.percent;
import :values.unitValue;
import :values.keywords;

using namespace Karm;
using namespace Karm::Math::Literals;

namespace Vaev {

// MARK: Table Layout ----------------------------------------------------------
// https://www.w3.org/TR/CSS21/tables.html#propdef-table-layout

export using TableLayout = Union<
    Keywords::Auto,
    Keywords::Fixed>;


// MARK: Caption Side ----------------------------------------------------------
// https://www.w3.org/TR/CSS21/tables.html#caption-position

export using CaptionSide = Union<
    Keywords::Top,
    Keywords::Bottom>;


// MARK: Border Collapse -------------------------------------------------------
// https://www.w3.org/TR/CSS22/tables.html#propdef-border-collapse

export using BorderCollapse = Union<
    Keywords::Separate,
    Keywords::Collapse>;


// MARK: Border Spacing --------------------------------------------------------
// https://www.w3.org/TR/CSS22/tables.html#propdef-border-spacing

export struct BorderSpacing {
    Length horizontal, vertical;

    void repr(Io::Emit& e) const {
        e("({}, {})", horizontal, vertical);
    }
};

namespace Style {

export struct BorderSpacing {
    Pixel horizontal = Pixel{0};
    Pixel vertical = Pixel{0};
};

} // namespace Style
export Style::BorderSpacing resolve(BorderSpacing const& value, LengthContext auto const& ctx) {
    return {resolve(value.horizontal, ctx), resolve(value.vertical, ctx)};
}

export BorderSpacing toSpecified(Style::BorderSpacing const& value) {
    return {toSpecified(value.horizontal), toSpecified(value.vertical)};
}

export template <>
struct ValueParser<BorderSpacing> {
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
};

export template <>
struct ValueSerializer<BorderSpacing> {
    // https://drafts.csswg.org/cssom/#serialize-a-css-value
    static Res<> serialize(Io::TextWriter& w, BorderSpacing const& value) {
        // If component values can be omitted or replaced with a shorter representation
        // without changing the meaning of the value, omit/replace them.
        Io::StringWriter horizontal;
        try$(serializeValue(horizontal, value.horizontal));
        Io::StringWriter vertical;
        try$(serializeValue(vertical, value.vertical));

        auto h = horizontal.take();
        auto v = vertical.take();
        try$(w.writeStr(h.str()));

        if (h == v)
            return Ok();

        try$(w.writeRune(' '));
        try$(w.writeStr(v.str()));

        return Ok();
    }
};

} // namespace Vaev
