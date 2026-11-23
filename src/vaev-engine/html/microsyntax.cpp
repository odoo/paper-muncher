module;

#include <karm/macros>

export module Vaev.Engine:html.microsyntax;

import Karm.Core;

using namespace Karm;

namespace Vaev::Html {

// https://infra.spec.whatwg.org/#ascii-whitespace
static constexpr auto ASCII_WHITESPACE =
    Re::single(0x0009) |
    Re::single(0x000A) |
    Re::single(0x000C) |
    Re::single(0x000D) |
    Re::single(0x0020);

// https://html.spec.whatwg.org/#rules-for-parsing-integers
export Res<isize> parseInteger(Str value) {
    auto scan = Io::SScan{value};

    scan.skip(ASCII_WHITESPACE);

    if (scan.ended())
        return Error::invalidData("unexpected end of input");

    return Io::atoi(scan).okOr(Error::invalidData("invalid integer"));
}

// https://html.spec.whatwg.org/#non-negative-integers
export Res<usize> parseNonNegativeInteger(Str value) {
    auto scan = Io::SScan{value};

    scan.skip(ASCII_WHITESPACE);

    if (scan.ended())
        return Error::invalidData("unexpected end of input");

    return Io::atou(scan).okOr(Error::invalidData("invalid integer"));
}

// https://html.spec.whatwg.org/#floating-point-numbers
export Res<f64> parseFloatingPointNumber(Str value) {
    auto scan = Io::SScan{value};

    scan.skip(ASCII_WHITESPACE);

    if (scan.ended())
        return Error::invalidData("unexpected end of input");

    return Io::atof(scan, {.allowExp = true}).okOr(Error::invalidData("invalid float"));
}

using Length = Distinct<f64, struct _LengthTag>;
using Percentage = Distinct<f64, struct _PercentgeTag>;
using Dimension = Union<Length, Percentage>;

// https://html.spec.whatwg.org/#percentages-and-dimensions
export Res<Dimension> parseDimension(Str value) {
    auto scan = Io::SScan{value};

    scan.skip(ASCII_WHITESPACE);

    if (scan.ended())
        return Error::invalidData("unexpected end of input");

    auto v = try$(Io::atof(scan, {.allowExp = false}).okOr(Error::invalidData("invalid float")));

    if (v < 0.0)
        return Error::invalidData("negative dimension");

    if (scan.skip('%'))
        return Ok(Percentage{v});

    return Ok(Length{v});
}

// https://html.spec.whatwg.org/#nonzero-percentages-and-lengths
export Res<Dimension> parseNonZeroDimension(Str value) {
    auto dimension = try$(parseDimension(value));
    return dimension.visit(
        [](Length const& length) -> Res<Dimension> {
            if (length.value() == 0.0)
                return Error::invalidData("zero non-negative dimension");

            return Ok(length);
        },
        [](Percentage const& percentage) -> Res<Dimension> {
            if (percentage.value() == 0.0)
                return Error::invalidData("zero non-negative dimension");

            return Ok(percentage);
        }
    );
}

} // namespace Vaev::Html
