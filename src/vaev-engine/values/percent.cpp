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

export template <typename T>
using PercentOr = Union<Percent, T>;

// FIXME: Refactor during calc refactor to look something like this:
//        LengthPercentage = Length | Percent | Calc
export template <typename T>
struct ValueTraits<PercentOr<T>> {
    // https://drafts.csswg.org/css-values-4/#combine-mixed
    using ComputedType = Union<Percent, Computed<T>>;

    static Res<PercentOr<T>> parse(Cursor<Css::Sst>& c) {
        return parseOneOf<PercentOr<T>>(c);
    }

    static ComputedType compute(PercentOr<T> const& percentOr, ComputationContext const& ctx) {
        return percentOr.visit([&](auto&& v) -> ComputedType {
            return computeValue(v, ctx);
        });
    }

    static PercentOr<T> fromComputed(ComputedType const& computed) {
        return computed.visit(Visitor{
            [](Computed<T> const& val) -> PercentOr<T> {
                return valueFromComputed<T>(val);
            },
            [](Percent const& percent) -> PercentOr<T> {
                return valueFromComputed<Percent>(percent);
            }
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
