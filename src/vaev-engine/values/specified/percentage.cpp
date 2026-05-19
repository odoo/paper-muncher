export module Vaev.Engine:values.specified.percentage;

import :values.specified.base;
import :values.common.percentage;

using namespace Karm;

namespace Vaev::Experimental {

// Note: This only implements ValueParseable since the computed value of percents is context dependent.
export template <>
struct ValueTraits<Percentage> {
    static Res<Percentage> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c.peek() == Css::Token::PERCENTAGE) {
            Io::SScan scan = c->token.data.str();
            c.next();
            return Ok(Percentage{Io::atof(scan).unwrapOr(0.0)});
        }

        return Error::invalidData("expected percentage");
    }
};

} // namespace Vaev::Experimental

export template <>
struct Karm::Io::Repr<Vaev::Experimental::Percentage> {
    static void repr(Io::Emit& e, Vaev::Experimental::Percentage const& v) {
        e("{}%", v.value());
    }
};
