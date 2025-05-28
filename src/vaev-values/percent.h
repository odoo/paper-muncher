#pragma once

#include <karm-base/distinct.h>
#include <karm-io/emit.h>

#include "base.h"
#include "primitives.h"
#include "resolved.h"

namespace Vaev {

// MARK: Percentage ------------------------------------------------------------
// https://drafts.csswg.org/css-values/#percentages

using Percent = Distinct<f64, struct _PercentTag>;

template <>
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

template <typename T>
using PercentOr = Union<Percent, T>;

template <typename T>
struct _Resolved<PercentOr<T>> {
    using Type = Resolved<T>;
};

} // namespace Vaev

template <>
struct Karm::Io::Repr<Vaev::Percent> {
    static void repr(Io::Emit& e, Vaev::Percent const& v) {
        e("{}%", v.value());
    }
};
