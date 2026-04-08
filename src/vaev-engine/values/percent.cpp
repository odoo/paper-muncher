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
struct ValueTraits<Percent> : DefaultValueTraits<Percent> {
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

export template <>
struct ComputedValueTraits<Percent> {
    using Resolved = Au;

    static Resolved resolve(Percent const& percent, Opt<Au> relative) {
        if (not relative)
            return 0_au;

        return *relative * percent.value();
    }
};

export template <typename T>
using PercentOr = Union<Percent, T>;

export template <typename T>
struct ComputedValueTraits<PercentOr<T>> {
    using Resolved = Au;

    static Resolved resolve(PercentOr<T> const& percentOr, Opt<Au> relative) {
        return percentOr.visit([&](auto&& v) -> Resolved {
            return resolveValue(v, relative);
        });
    }
};

export template <typename T>
struct _Resolved<PercentOr<T>> {
    using Type = Resolved<T>;
};

} // namespace Vaev

export template <>
struct Karm::Io::Repr<Vaev::Percent> {
    static void repr(Io::Emit& e, Vaev::Percent const& v) {
        e("{}%", v.value());
    }
};
