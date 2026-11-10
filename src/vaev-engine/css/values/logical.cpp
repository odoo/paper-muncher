module;

#include <karm/macros>

export module Vaev.Engine:css.values.logical;

import Karm.Core;

import :css.parser;
import :css.values.base;
import :css.values.keywords;
import :css.values.numeric;

using namespace Karm;

// https://drafts.csswg.org/css-logical-1/

namespace Vaev::Css {

// Value of the logical margin shorthands: <'margin-top'>{1,2}
// The first value is the start side, the second (defaulting to the first) is the end side.
// https://drafts.csswg.org/css-logical/#propdef-margin-block
// https://drafts.csswg.org/css-logical/#propdef-margin-inline
export struct LogicalMargins {
    Union<Keywords::Auto, LengthPercentage> start;
    Union<Keywords::Auto, LengthPercentage> end;

    void repr(Io::Emit& e) const {
        e("({} {})", start, end);
    }
};

export template <>
struct ValueParser<LogicalMargins> {
    static Res<LogicalMargins> parse(Cursor<Sst>& c) {
        auto start = try$(parseValue<Union<Keywords::Auto, LengthPercentage>>(c));
        auto end = parseValue<Union<Keywords::Auto, LengthPercentage>>(c);
        if (not end)
            return Ok(LogicalMargins{start, start});
        return Ok(LogicalMargins{start, end.take()});
    }
};

} // namespace Vaev::Css
