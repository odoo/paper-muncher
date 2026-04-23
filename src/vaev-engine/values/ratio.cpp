module;

#include <karm/macros>

export module Vaev.Engine:values.ratio;

import Karm.Core;
import Karm.Math;

import :values.primitives;

using namespace Karm;

namespace Vaev {

export struct Ratio {
    Number num;
    Number deno;

    Ratio() : num(0.0), deno(1.0) {}

    Ratio(Number num, Number deno = 1.0) : num(num), deno(deno) {}

    auto operator==(Ratio const& other) const {
        return Math::epsilonEq(num / deno, other.num / other.deno);
    }

    auto operator<=>(Ratio const& other) const {
        return num / deno <=> other.num / other.deno;
    }

    void repr(Io::Emit& e) const {
        e("(ratio {} {})", num, deno);
    }
};

export template <>
struct ValueTraits<Ratio> : DefaultValueTraits<Ratio> {
    static Res<Ratio> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (auto num = parseValue<Number>(c)) {
            if (c.skip(Css::Token::delim("/"))) {
                auto deno = try$(parseValue<Number>(c));
                return Ok(Ratio(num.unwrap(), deno));
            }

            return Ok(Ratio(num.unwrap()));
        }

        return Error::invalidData("expected ratio");
    }
};

} // namespace Vaev
