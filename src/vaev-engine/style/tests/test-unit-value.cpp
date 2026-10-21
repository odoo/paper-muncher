#include <karm/test>

import Vaev.Engine;
import Karm.Math;
import Karm.Diag;

using namespace Karm;
using namespace Karm::Literals;
using namespace Karm::Math::Literals;

namespace Vaev::Style::Tests {

static_assert(Meta::Equatable<Time, Time> and Meta::Comparable<Time, Time>);
static_assert(Meta::Equatable<AbsoluteLength, Length> and Meta::Comparable<AbsoluteLength, Length>);
static_assert(not Meta::Equatable<Time, Angle> and not Meta::Comparable<Time, Angle>);
static_assert(not Meta::Equatable<Length, Percent> and not Meta::Comparable<Length, Percent>);
static_assert(not Meta::Equatable<Flex, Percent> and not Meta::Comparable<Flex, Percent>);

template <typename T>
static Res<T> _parse(Str input) {
    auto lex = Css::Lexer{input};
    auto diags = Diag::Collector::ignore();
    auto [val, _] = consumeDeclarationValue(lex, diags);
    Cursor<Css::Sst> sst{val};
    return parseValue<T>(sst);
}

template <typename T>
static Res<String> _serialize(T const& value) {
    Io::StringWriter sw;
    try$(serializeValue(sw, value));
    return Ok(sw.take());
}

test$("vaev-css-unit-value-units") {
    auto length = try$(_parse<AbsoluteLength>("1in"));
    assertEq$(resolve(length), 96_au);

    auto angle = try$(_parse<Angle>("1turn"));
    assertEq$(angle.toDegree(), 360.0);

    auto resolution = try$(_parse<Resolution>("192dpi"));
    assertEq$(resolution.toDppx(), 2.0);

    auto time = try$(_parse<Time>("1500ms"));
    assertEq$(time.toSecond(), 1.5);

    auto frequency = try$(_parse<Frequency>("2kHz"));
    assertEq$(frequency.toHz(), 2000.0);

    auto percent = try$(_parse<Percent>("50%"));
    assertEq$(percent.value(), 50.0);

    auto flex = try$(_parse<Flex>("2fr"));
    assertEq$(flex.toFr(), 2.0);

    auto number = try$(_parse<UnitValue>("2"));
    assertEq$(number.baseType(), NONE);
    assertEq$(try$(_parse<UnitValue>("2%")).baseType(), NumericBaseType::PERCENT);

    return Ok();
}

test$("vaev-css-unit-value-parsing") {
    assertEq$(try$(_parse<AbsoluteLength>("2PX")).pixels().value(), 2.0);

    assert$(not _parse<AbsoluteLength>("2em"));
    assert$(_parse<Length>("2em"));

    assertEq$(try$(_parse<Length>("0")), Length{0_au});

    assert$(not _parse<Length>("10deg"));
    assert$(not _parse<Angle>("10px"));
    assert$(not _parse<Flex>("10%"));

    assertEq$(try$(_parse<Length>("2em")), Length::fromEm(2));
    assertNe$(try$(_parse<AbsoluteLength>("1in")), try$(_parse<AbsoluteLength>("96px")));
    assertEq$(try$(_parse<AbsoluteLength>("1in")).pixels().value(), 96.0);
    assertGt$(try$(_parse<AbsoluteLength>("1in")), try$(_parse<AbsoluteLength>("95px")));
    assertEq$(try$(_parse<Resolution>("96dpi")).toDppx(), 1.0);

    return Ok();
}

test$("vaev-css-unit-value-serialization") {
    assertEq$(try$(_serialize(try$(_parse<Length>("2em")))), "2em"s);
    assertEq$(try$(_serialize(try$(_parse<Length>("3PX")))), "3px"s);
    assertEq$(try$(_serialize(try$(_parse<Angle>("90deg")))), "90deg"s);
    assertEq$(try$(_serialize(try$(_parse<Percent>("25%")))), "25%"s);
    assertEq$(try$(_serialize(try$(_parse<Flex>("1fr")))), "1fr"s);

    assertEq$(try$(_serialize(try$(_parse<Length>("0.1234567em")))), "0.123457em"s);
    assertEq$(try$(_serialize(try$(_parse<Length>("3.5PX")))), "3.5px"s);
    assertEq$(try$(_serialize(try$(_parse<Angle>("-0.900deg")))), "-0.9deg"s);
    assertEq$(try$(_serialize(try$(_parse<Percent>("0.0%")))), "0%"s);
    assertEq$(try$(_serialize(try$(_parse<Flex>("-0fr")))), "0fr"s);

    return Ok();
}

} // namespace Vaev::Style::Tests
