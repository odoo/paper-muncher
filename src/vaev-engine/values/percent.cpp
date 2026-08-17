export module Vaev.Engine:values.percent;

import Karm.Core;

import :css;
import :values.base;
import :values.primitives;

using namespace Karm;

namespace Vaev {

// MARK: Percentage ------------------------------------------------------------
// https://drafts.csswg.org/css-values/#percentages

export using Percent = Distinct<f64, struct _PercentTag>;

export template <>
struct ValueTraits<Percent> {
    static Res<Percent> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c.peek() == Css::Token::PERCENTAGE) {
            Io::SScan scan = c->token.data.str();
            c.next();
            return Ok(Percent{Io::atof(scan).unwrapOr(0.0)});
        }

        return Error::invalidData("expected percentage");
    }
};

template <typename T>
auto resolve(Percent const& value, auto const&, T relative) {
    return T{relative.template cast<f64>() * (value.value() / 100.)};
}

} // namespace Vaev

export template <>
struct Karm::Io::Repr<Vaev::Percent> {
    static void repr(Io::Emit& e, Vaev::Percent const& v) {
        e("{}%", v.value());
    }
};
