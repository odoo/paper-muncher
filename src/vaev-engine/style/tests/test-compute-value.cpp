#include <karm/test>

import Vaev.Engine;
import Karm.Math;
import Karm.Diag;

using namespace Karm;
using namespace Karm::Math::Literals;

namespace Vaev::Style::Tests {

static ComputationContext _computationContext() {
    ComputationContext cx{};
    cx.fontSize = 16;
    cx.rootFontSize = 10;
    cx.viewportLargeWidth = 1000;
    return cx;
}

template <typename T>
static Res<T> _parse(Str input) {
    auto lex = Css::Lexer{input};
    auto diags = Diag::Collector::ignore();
    auto [val, _] = consumeDeclarationValue(lex, diags);
    Cursor<Css::Sst> sst{val};
    return parseValue<T>(sst);
}

test$("vaev-css-compute-length-percentage") {
    using ComputedCalc = Calc<Au, Percent>;

    auto testCase = [&](Str input, ComputedCalc expected) -> Res<> {
        auto value = try$(_parse<Calc<Length, Percent>>(input));
        auto computed = compute(value, _computationContext());
        assertEq$(Io::format("{}", computed), Io::format("{}", expected));
        return Ok();
    };

    try$(testCase("10px", ComputedCalc{10_au}));
    try$(testCase("50%", ComputedCalc{Percent{50}}));
    try$(testCase("2em", ComputedCalc{32_au}));
    try$(testCase("2rem", ComputedCalc{20_au}));
    try$(testCase("10vw", ComputedCalc{100_au}));

    // Operands that don't depend on the percentage basis are simplified away
    try$(testCase("calc(10px + 2em)", ComputedCalc{42_au}));
    try$(testCase("calc(2 * 1rem)", ComputedCalc{20_au}));
    try$(testCase("calc(1em / 2)", ComputedCalc{8_au}));

    // Percentages are kept until layout
    try$(testCase(
        "calc(50% + 2em)",
        ComputedCalc{
            CalcOp::ADD,
            Percent{50},
            32_au,
        }
    ));

    return Ok();
}

test$("vaev-css-resolve-computed-length-percentage") {
    auto testCase = [&](Str input, Au relative, Au expected) -> Res<> {
        auto value = try$(_parse<Calc<Length, Percent>>(input));
        auto computed = compute(value, _computationContext());
        assertEq$(resolve(computed, relative), expected);
        return Ok();
    };

    try$(testCase("2em", 200_au, 32_au));
    try$(testCase("50%", 200_au, 100_au));
    try$(testCase("calc(50% + 2em)", 200_au, 132_au));
    try$(testCase("calc(100% - 1rem)", 200_au, 190_au));

    return Ok();
}

test$("vaev-css-specified-from-computed") {
    using SpecifiedCalc = Calc<Length, Percent>;

    auto testCase = [&](Str input, SpecifiedCalc expected) -> Res<> {
        auto value = try$(_parse<SpecifiedCalc>(input));
        auto computed = compute(value, _computationContext());
        auto roundTrip = specified<SpecifiedCalc>(computed);
        assertEq$(Io::format("{}", roundTrip), Io::format("{}", expected));
        return Ok();
    };

    try$(testCase("2em", SpecifiedCalc{Length{32_au}}));
    try$(testCase(
        "calc(50% + 2em)",
        SpecifiedCalc{
            CalcOp::ADD,
            Percent{50},
            Length{32_au},
        }
    ));

    return Ok();
}

test$("vaev-css-compute-border-radius-calc") {
    using Radii = Math::Radii<Calc<Length, Percent>>;

    auto value = try$(_parse<Radii>("calc(1em + 2px) 50%"));
    auto computed = compute(value, _computationContext());
    Computed<Radii> expected = {Calc<Au, Percent>{18_au}, Calc<Au, Percent>{Percent{50}}};
    assertEq$(Io::format("{}", computed), Io::format("{}", expected));
    return Ok();
}

test$("vaev-css-compute-transform") {
    auto testCase = [&](Str input, Computed<Transform> expected) -> Res<> {
        auto value = try$(_parse<Transform>(input));
        auto computed = compute(value, _computationContext());
        assertEq$(Io::format("{}", computed), Io::format("{}", expected));
        assertEq$(Io::format("{}", specified<Transform>(computed)), Io::format("{}", specified<Transform>(expected)));
        return Ok();
    };

    try$(testCase("none", Keywords::NONE));
    try$(testCase(
        "translate(2em, 10%) scale(2)",
        Computed<TransformList>{
            Computed<TranslateTransform>{32_au, Percent{10}},
            ScaleTransform{2.0, 2.0},
        }
    ));

    return Ok();
}

test$("vaev-css-compute-transform-origin") {
    auto value = try$(_parse<TransformOrigin>("2em 50%"));
    auto computed = compute(value, _computationContext());
    Computed<TransformOrigin> expected = {
        .xOffset = Calc<Au, Percent>{32_au},
        .yOffset = Calc<Au, Percent>{Percent{50}},
    };
    assertEq$(Io::format("{}", computed), Io::format("{}", expected));
    return Ok();
}

test$("vaev-css-compute-line-height") {
    auto testCase = [&](Str input, Computed<LineHeight> expected) -> Res<> {
        auto value = try$(_parse<LineHeight>(input));
        auto computed = compute(value, _computationContext());
        assertEq$(Io::format("{}", computed), Io::format("{}", expected));
        return Ok();
    };

    try$(testCase("normal", Keywords::NORMAL));
    try$(testCase("1.5", Number{1.5}));
    try$(testCase("150%", 24_au));
    try$(testCase("2em", 32_au));

    return Ok();
}

test$("vaev-css-calc-type-checking") {
    using LengthPercentage = Calc<Length, Percent>;

    auto valid = [&](Str input) -> Res<> {
        assert$(_parse<LengthPercentage>(input));
        return Ok();
    };

    auto invalid = [&](Str input) -> Res<> {
        assert$(not _parse<LengthPercentage>(input));
        return Ok();
    };

    try$(valid("calc(10px + 2em)"));
    try$(valid("calc(50% - 10px)"));
    try$(valid("calc(2 * 10px)"));
    try$(valid("calc(10px / 2)"));

    try$(invalid("calc(10px * 10px)"));
    try$(invalid("calc(10px / 2px)"));
    try$(invalid("calc(10px + 2)"));
    try$(invalid("calc(10px + 5deg)"));
    try$(invalid("calc(2 * 3)"));

    // Percentages are only accepted where they are expected
    assert$(not _parse<Calc<Length>>("calc(50% + 10px)"));

    return Ok();
}

test$("vaev-css-dimension-units") {
    auto length = try$(_parse<AbsoluteLength>("1in"));
    assertEq$(resolve(length), 96_au);

    auto angle = try$(_parse<Angle>("1turn"));
    assertEq$(angle.toDegree(), 360.0);

    auto resolution = try$(_parse<Resolution>("192dpi"));
    assertEq$(resolution.toDppx(), 2.0);

    // Units are case insensitive
    assertEq$(try$(_parse<AbsoluteLength>("2PX")).pixels().value(), 2.0);

    // Relative lengths aren't absolute lengths
    assert$(not _parse<AbsoluteLength>("2em"));

    return Ok();
}

} // namespace Vaev::Style::Tests
