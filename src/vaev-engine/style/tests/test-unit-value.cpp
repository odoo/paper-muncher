#include <karm/test>

import Vaev.Engine;
import Karm.Math;
import Karm.Diag;

using namespace Karm;
using namespace Karm::Literals;
using namespace Karm::Math::Literals;
using namespace Vaev::Literals;

namespace Vaev::Style::Tests {

static_assert(Meta::Equatable<Css::PureTime, Css::PureTime> and Meta::Comparable<Css::PureTime, Css::PureTime>);
static_assert(Meta::Equatable<Css::PureLength, Css::PureLength> and Meta::Comparable<Css::PureLength, Css::PureLength>);
static_assert(not Meta::Equatable<Css::PureTime, Css::PureAngle> and not Meta::Comparable<Css::PureTime, Css::PureAngle>);
static_assert(not Meta::Equatable<Css::PureLength, Css::PurePercentage> and not Meta::Comparable<Css::PureLength, Css::PurePercentage>);
static_assert(not Meta::Equatable<Css::PureFlex, Css::PurePercentage> and not Meta::Comparable<Css::PureFlex, Css::PurePercentage>);

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
    auto length = try$(_parse<Css::PureLength>("1in"));
    assertEq$(length.unitValue().to(UnitValue::Unit::PX), 96.0);

    auto angle = try$(_parse<Css::PureAngle>("1turn"));
    assertEq$(angle.toDegree(), 360.0);

    auto resolution = try$(_parse<Css::PureResolution>("192dpi"));
    assertEq$(resolution.toDppx(), 2.0);

    auto time = try$(_parse<Css::PureTime>("1500ms"));
    assertEq$(time.unitValue().to(UnitValue::Unit::S), 1.5);

    auto frequency = try$(_parse<Css::PureFrequency>("2kHz"));
    assertEq$(frequency.unitValue().to(UnitValue::Unit::HZ), 2000.0);

    auto percent = try$(_parse<Css::PurePercentage>("50%"));
    assertEq$(percent.value(), 50.0);

    auto flex = try$(_parse<Css::PureFlex>("2fr"));
    assertEq$(flex.unitValue().to(UnitValue::Unit::FR), 2.0);

    auto number = try$(_parse<UnitValue>("2"));
    assertEq$(number.baseType(), NONE);
    assertEq$(try$(_parse<UnitValue>("2%")).baseType(), NumericBaseType::PERCENT);

    return Ok();
}

test$("vaev-css-unit-value-parsing") {
    assertEq$(try$(_parse<Css::PureLength>("2PX")).unitValue().to(UnitValue::Unit::PX), 2.0);

    assert$(_parse<Css::PureLength>("2em"));

    assertEq$(try$(_parse<Css::PureLength>("0")), Css::PureLength::fromPx(0));

    assert$(not _parse<Css::PureLength>("10deg"));
    assert$(not _parse<Css::PureAngle>("10px"));
    assert$(not _parse<Css::PureFlex>("10%"));

    assertEq$(try$(_parse<Css::PureLength>("2em")), (Css::PureLength{UnitValue{2, UnitValue::Unit::EM}}));
    assertNe$(try$(_parse<Css::PureLength>("1in")), try$(_parse<Css::PureLength>("96px")));
    assertEq$(try$(_parse<Css::PureLength>("1in")).unitValue().to(UnitValue::Unit::PX), 96.0);
    assertGt$(try$(_parse<Css::PureLength>("1in")), try$(_parse<Css::PureLength>("95px")));
    assertEq$(try$(_parse<Css::PureResolution>("96dpi")).toDppx(), 1.0);

    return Ok();
}

test$("vaev-css-unit-value-serialization") {
    assertEq$(try$(_serialize(try$(_parse<Css::PureLength>("2em")))), "2em"s);
    assertEq$(try$(_serialize(try$(_parse<Css::PureLength>("3PX")))), "3px"s);
    assertEq$(try$(_serialize(try$(_parse<Css::PureAngle>("90deg")))), "90deg"s);
    assertEq$(try$(_serialize(try$(_parse<Css::PurePercentage>("25%")))), "25%"s);
    assertEq$(try$(_serialize(try$(_parse<Css::PureFlex>("1fr")))), "1fr"s);

    assertEq$(try$(_serialize(try$(_parse<Css::PureLength>("0.1234567em")))), "0.123457em"s);
    assertEq$(try$(_serialize(try$(_parse<Css::PureLength>("3.5PX")))), "3.5px"s);
    assertEq$(try$(_serialize(try$(_parse<Css::PureAngle>("-0.900deg")))), "-0.9deg"s);
    assertEq$(try$(_serialize(try$(_parse<Css::PurePercentage>("0.0%")))), "0%"s);
    assertEq$(try$(_serialize(try$(_parse<Css::PureFlex>("-0fr")))), "0fr"s);

    return Ok();
}

} // namespace Vaev::Style::Tests
