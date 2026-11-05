#include <karm/test>

import Vaev.Engine;
import Karm.Math;
import Karm.Diag;

using namespace Karm;
using namespace Karm::Literals;
using namespace Karm::Math::Literals;

namespace Vaev::Style::Tests {

static_assert(Meta::Equatable<PureTime, PureTime> and Meta::Comparable<PureTime, PureTime>);
static_assert(Meta::Equatable<PureAbsoluteLength, PureLength> and Meta::Comparable<PureAbsoluteLength, PureLength>);
static_assert(not Meta::Equatable<PureTime, PureAngle> and not Meta::Comparable<PureTime, PureAngle>);
static_assert(not Meta::Equatable<PureLength, PurePercentage> and not Meta::Comparable<PureLength, PurePercentage>);
static_assert(not Meta::Equatable<PureFlex, PurePercentage> and not Meta::Comparable<PureFlex, PurePercentage>);

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
    auto length = try$(_parse<PureAbsoluteLength>("1in"));
    assertEq$(resolve(length), 96_au);

    auto angle = try$(_parse<PureAngle>("1turn"));
    assertEq$(angle.toDegree(), 360.0);

    auto resolution = try$(_parse<PureResolution>("192dpi"));
    assertEq$(resolution.toDppx(), 2.0);

    auto time = try$(_parse<PureTime>("1500ms"));
    assertEq$(time.toSecond(), 1.5);

    auto frequency = try$(_parse<PureFrequency>("2kHz"));
    assertEq$(frequency.toHz(), 2000.0);

    auto percent = try$(_parse<PurePercentage>("50%"));
    assertEq$(percent.value(), 50.0);

    auto flex = try$(_parse<PureFlex>("2fr"));
    assertEq$(flex.toFr(), 2.0);

    auto number = try$(_parse<UnitValue>("2"));
    assertEq$(number.baseType(), NONE);
    assertEq$(try$(_parse<UnitValue>("2%")).baseType(), NumericBaseType::PERCENT);

    return Ok();
}

test$("vaev-css-unit-value-parsing") {
    assertEq$(try$(_parse<PureAbsoluteLength>("2PX")).pixels().value(), 2.0);

    assert$(not _parse<PureAbsoluteLength>("2em"));
    assert$(_parse<PureLength>("2em"));

    assertEq$(try$(_parse<PureLength>("0")), PureLength{0_au});

    assert$(not _parse<PureLength>("10deg"));
    assert$(not _parse<PureAngle>("10px"));
    assert$(not _parse<PureFlex>("10%"));

    assertEq$(try$(_parse<PureLength>("2em")), PureLength::fromEm(2));
    assertNe$(try$(_parse<PureAbsoluteLength>("1in")), try$(_parse<PureAbsoluteLength>("96px")));
    assertEq$(try$(_parse<PureAbsoluteLength>("1in")).pixels().value(), 96.0);
    assertGt$(try$(_parse<PureAbsoluteLength>("1in")), try$(_parse<PureAbsoluteLength>("95px")));
    assertEq$(try$(_parse<PureResolution>("96dpi")).toDppx(), 1.0);

    return Ok();
}

test$("vaev-css-unit-value-serialization") {
    assertEq$(try$(_serialize(try$(_parse<PureLength>("2em")))), "2em"s);
    assertEq$(try$(_serialize(try$(_parse<PureLength>("3PX")))), "3px"s);
    assertEq$(try$(_serialize(try$(_parse<PureAngle>("90deg")))), "90deg"s);
    assertEq$(try$(_serialize(try$(_parse<PurePercentage>("25%")))), "25%"s);
    assertEq$(try$(_serialize(try$(_parse<PureFlex>("1fr")))), "1fr"s);

    assertEq$(try$(_serialize(try$(_parse<PureLength>("0.1234567em")))), "0.123457em"s);
    assertEq$(try$(_serialize(try$(_parse<PureLength>("3.5PX")))), "3.5px"s);
    assertEq$(try$(_serialize(try$(_parse<PureAngle>("-0.900deg")))), "-0.9deg"s);
    assertEq$(try$(_serialize(try$(_parse<PurePercentage>("0.0%")))), "0%"s);
    assertEq$(try$(_serialize(try$(_parse<PureFlex>("-0fr")))), "0fr"s);

    return Ok();
}

} // namespace Vaev::Style::Tests
