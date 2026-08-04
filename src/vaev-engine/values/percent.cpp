export module Vaev.Engine:values.percent;

import Karm.Core;

import :css;
import :values.base;
import :values.primitives;
import :values.resolved;

using namespace Karm;

namespace Vaev {

// MARK: Percentage ------------------------------------------------------------
// https://drafts.csswg.org/css-values/#percentages

export using Percent = Distinct<f64, struct _PercentTag>;

export template <>
struct ValueParser<Percent> {
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

export template <typename T>
using PercentOr = Union<Percent, T>;

template <typename T>
struct PercentRelative {
    T percentRelative;
};

template <typename T>
PercentRelative(T) -> PercentRelative<T>;

export template <typename T>
concept PercentContext = requires(T t) {
    { t.percentRelative }
};

export template <typename T, PercentContext Cx, typename R = Resolved<T, Cx>>
R resolve(PercentOr<T> const& value, Cx auto& cx) {
    if (auto v = value.template is<Percent>())
        return R{cx.percentRelative.template cast<f64>() * ((*v).value() / 100.)};
    return resolve(value.template unwrap<T>(), cx);
}

export template <typename T, typename Cx, typename R = PercentOr<Resolved<T, Cx>>>
R resolve(PercentOr<T> const& value, Cx auto& cx) {
    if (auto v = value.template is<Percent>())
        return R{*v};
    return R{resolve(value.template unwrap<T>(), cx)};
}

} // namespace Vaev

export template <>
struct Karm::Io::Repr<Vaev::Percent> {
    static void repr(Io::Emit& e, Vaev::Percent const& v) {
        e("{}%", v.value());
    }
};
