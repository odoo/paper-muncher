#include <karm/test>

import Vaev.Engine;
import Karm.Diag;
import Karm.Logger;
import Karm.Math;

using namespace Karm;
using namespace Karm::Literals;

namespace Vaev::Css::Tests {

// MARK: Helpers ---------------------------------------------------------------

// Parses `input` as a T, the whole input must be consumed.
template <typename T>
static Res<T> _parse(Str input) {
    auto lex = Lexer{input};
    auto diags = Diag::Collector::ignore();
    auto [val, _] = consumeDeclarationValue(lex, diags);
    Cursor<Sst> sst{val};
    auto res = try$(parseValue<T>(sst));
    eatWhitespace(sst);
    if (not sst.ended())
        return Error::invalidData("trailing tokens");
    return Ok(std::move(res));
}

template <typename T>
static bool _valid(Str input) {
    return bool(_parse<T>(input));
}

template <typename T>
static Res<String> _serialize(T const& value) {
    Io::StringWriter sw;
    try$(serializeValue(sw, value));
    return Ok(sw.take());
}

// Specified value serialization (no simplification).
template <typename T>
static Res<String> _specified(Str input) {
    return _serialize(try$(_parse<T>(input)));
}

// Computed <length>, serialized.
static Res<String> _px(Str input) {
    auto calc = try$(_parse<Length>(input));
    auto cx = Style::RelativeLengthContextData{};
    return _serialize(Style::toSpecified(Style::toComputed(calc, cx)));
}

// Computed <length-percentage>, serialized.
static Res<String> _lp(Str input) {
    auto calc = try$(_parse<LengthPercentage>(input));
    auto cx = Style::RelativeLengthContextData{};
    return _serialize(Style::toSpecified(Style::toComputed(calc, cx)));
}

// Used <length-percentage> against a percentage basis, in px.
static Res<f64> _used(Str input, f64 basis) {
    auto calc = try$(_parse<LengthPercentage>(input));
    auto cx = Style::RelativeLengthContextData{};
    return Ok(Au{Style::resolve(Style::toComputed(calc, cx), Style::Pixel{basis}).value()}.cast<f64>());
}

// Used <length-percentage> against an optional percentage basis, in px.
static Res<Opt<f64>> _usedOpt(Str input, Opt<f64> basis) {
    auto calc = try$(_parse<LengthPercentage>(input));
    auto cx = Style::RelativeLengthContextData{};
    Opt<Style::Pixel> relative = NONE;
    if (basis)
        relative = Some(Style::Pixel{*basis});
    return Ok(Style::resolve(Style::toComputed(calc, cx), relative).map([](Style::Pixel px) {
        return Au{px.value()}.cast<f64>();
    }));
}

// Computed <number>.
static Res<f64> _num(Str input) {
    return Ok(static_cast<f64>(Style::toComputed(try$(_parse<Number>(input)))));
}

// Computed <percentage>.
static Res<f64> _pct(Str input) {
    return Ok(Style::toComputed(try$(_parse<Percentage>(input))).value());
}

static bool _near(f64 a, f64 b) {
    return Math::abs(a - b) < 1e-6;
}

static bool _isNegZero(f64 x) {
    return x == 0 and 1.0 / x < 0;
}

static bool _isPosZero(f64 x) {
    return x == 0 and 1.0 / x > 0;
}

// MARK: Basics ----------------------------------------------------------------

test$("calc-length-basic") {
    assertEq$(try$(_px("1px")), "1px"s);
    assertEq$(try$(_px("0")), "0px"s);
    assertEq$(try$(_px("calc(1px)")), "1px"s);
    assertEq$(try$(_px("min(1px, 2px, 3px)")), "1px"s);
    assertEq$(try$(_px("max(1px, 2px, 3px)")), "3px"s);
    assertEq$(try$(_px("clamp(1px, 0px, 3px)")), "1px"s);
    assertEq$(try$(_px("clamp(1px, 5px, 3px)")), "3px"s);
    assertEq$(try$(_px("clamp(1px, 2px, 3px)")), "2px"s);
    assertEq$(try$(_px("clamp(none, 5px, 3px)")), "3px"s);
    assertEq$(try$(_px("clamp(1px, 0px, none)")), "1px"s);
    assertEq$(try$(_px("clamp(none, 2px, none)")), "2px"s);
    assertEq$(try$(_px("hypot(1px, 2px, 3px)")), "3.741657px"s);
    assertEq$(try$(_px("calc(1px * sin(30deg))")), "0.5px"s);
    assertEq$(try$(_px("calc(1px * asin(1/2) / 1deg)")), "30px"s);
    return Ok();
}

test$("calc-length-percentage-basic") {
    assertEq$(try$(_lp("1px")), "1px"s);
    assertEq$(try$(_lp("1%")), "1%"s);
    assertEq$(try$(_lp("calc(1px)")), "1px"s);
    assertEq$(try$(_lp("calc(1%)")), "1%"s);
    assertEq$(try$(_lp("min(1px, 2%, 3px)")), "min(1px, 2%, 3px)"s);
    assertEq$(try$(_lp("max(1px, 2%, 3px)")), "max(1px, 2%, 3px)"s);
    assertEq$(try$(_lp("clamp(1px, 2%, 3px)")), "clamp(1px, 2%, 3px)"s);
    assertEq$(try$(_lp("clamp(none, 5%, 3px)")), "min(5%, 3px)"s);
    assertEq$(try$(_lp("clamp(1px, 0%, none)")), "max(1px, 0%)"s);
    assertEq$(try$(_lp("clamp(none, 2px, none)")), "2px"s);
    assertEq$(try$(_lp("hypot(1px, 2px, 3%)")), "hypot(1px, 2px, 3%)"s);
    assertEq$(try$(_lp("hypot(1px, 2px, 1px + 3%)")), "hypot(1px, 2px, 3% + 1px)"s);
    return Ok();
}

// MARK: Arithmetic ------------------------------------------------------------

test$("calc-precedence") {
    assertEq$(try$(_px("calc(1px + 2px * 3)")), "7px"s);
    assertEq$(try$(_px("calc(10px - 2px - 3px)")), "5px"s);
    assertEq$(try$(_px("calc(12px / 2 / 3)")), "2px"s);
    assertEq$(try$(_px("calc(2 * (1px + 2px))")), "6px"s);
    assertEq$(try$(_px("calc(1px + 2px * 3 - 1px)")), "6px"s);
    assertEq$(try$(_px("calc(1px - -2px)")), "3px"s);
    return Ok();
}

test$("calc-nesting") {
    assertEq$(try$(_px("calc(calc(calc(1px)))")), "1px"s);
    assertEq$(try$(_px("calc((((1px))))")), "1px"s);
    assertEq$(try$(_px("min( max(1px, 2px) , 3px )")), "2px"s);
    return Ok();
}

test$("calc-unit-conversion") {
    assertEq$(try$(_px("calc(1px + 1in)")), "97px"s);
    assertEq$(try$(_px("calc(1in * 2)")), "192px"s);
    assertEq$(try$(_px("calc(1px / 2px * 3px)")), "1.5px"s);
    return Ok();
}

test$("calc-keywords") {
    assertEq$(try$(_px("calc(pi * 1px)")), "3.141593px"s);
    assertEq$(try$(_px("calc(PI * 1px)")), "3.141593px"s);
    assertEq$(try$(_px("calc(e * 1px)")), "2.718282px"s);
    return Ok();
}

// MARK: Math functions --------------------------------------------------------

test$("calc-exp") {
    assertEq$(try$(_px("calc(1px * exp(0))")), "1px"s);
    return Ok();
}

test$("calc-hypot") {
    assertEq$(try$(_px("hypot(3px, 4px)")), "5px"s);
    assertEq$(try$(_px("hypot(-3px)")), "3px"s);
    return Ok();
}

test$("calc-clamp-min-wins") {
    assertEq$(try$(_px("clamp(10px, 20px, 0px)")), "10px"s);
    return Ok();
}

test$("calc-trig") {
    assertEq$(try$(_px("calc(1px * sin(pi / 2))")), "1px"s);
    assertEq$(try$(_px("calc(1px * sin(0.25turn))")), "1px"s);
    assertEq$(try$(_px("calc(1px * sin(1.5707963267948966rad))")), "1px"s);
    assertEq$(try$(_px("calc(1px * atan(1) / 1deg)")), "45px"s);
    assertEq$(try$(_px("calc(1px * atan2(1px, 1px) / 1deg)")), "45px"s);
    assertEq$(try$(_px("calc(1px * atan2(1, 1) / 1deg)")), "45px"s);
    return Ok();
}

test$("calc-number-functions") {
    assert$(_near(try$(_num("pow(2, 3)")), 8));
    assert$(_near(try$(_num("sqrt(16)")), 4));
    assert$(_near(try$(_num("log(8, 2)")), 3));
    assert$(_near(try$(_num("log(e)")), 1));
    assert$(_near(try$(_num("sin(90deg)")), 1));
    assert$(_near(try$(_num("cos(0.5turn)")), -1));
    return Ok();
}

test$("calc-abs") {
    assertEq$(try$(_px("abs(-1px)")), "1px"s);
    assertEq$(try$(_px("abs(1in)")), "96px"s);
    assertEq$(try$(_px("calc(abs(-2px) * 3)")), "6px"s);
    assert$(_near(try$(_num("abs(-2.5)")), 2.5));
    assertEq$(try$(_num("abs(-infinity)")), Math::INF);
    assert$(Math::isNan(try$(_num("abs(NaN)"))));
    return Ok();
}

test$("calc-abs-percent") {
    assertEq$(try$(_used("abs(-10%)", 100)), 10.0);
    assertEq$(try$(_used("abs(10%)", -100)), 10.0);
    assertEq$(try$(_specified<LengthPercentage>("abs(-10%)")), "abs(-10%)"s);
    return Ok();
}

test$("calc-sign") {
    assert$(_near(try$(_num("sign(-5)")), -1));
    assert$(_near(try$(_num("sign(5)")), 1));
    assert$(_near(try$(_num("sign(0)")), 0));
    assert$(_near(try$(_num("sign(-infinity)")), -1));
    assertEq$(try$(_px("calc(1px * sign(-5px))")), "-1px"s);
    assertEq$(try$(_px("calc(2px * sign(1in))")), "2px"s);
    return Ok();
}

test$("calc-sign-signed-zero-and-nan") {
    assert$(_isNegZero(try$(_num("sign(-1 * 0)"))));
    assert$(_isPosZero(try$(_num("sign(0)"))));
    assert$(Math::isNan(try$(_num("sign(NaN)"))));
    return Ok();
}

test$("calc-sign-is-number") {
    assert$(_valid<Number>("sign(1px)"));
    assert$(_valid<Number>("sign(1deg)"));
    assertNot$(_valid<Length>("sign(1px)"));
    return Ok();
}

test$("calc-sign-percent") {
    // A percentage against a negative basis flips the sign, so it must not be simplified early.
    assertEq$(try$(_used("calc(1px * sign(10%))", -100)), -1.0);
    return Ok();
}

// MARK: round() ---------------------------------------------------------------

test$("calc-round-strategies") {
    assert$(_near(try$(_num("round(10, 10)")), 10));
    assert$(_near(try$(_num("calc(round(100, 10))")), 100));
    assert$(_near(try$(_num("calc(round(up, 101, 10))")), 110));
    assert$(_near(try$(_num("calc(round(down, 106, 10))")), 100));
    assert$(_near(try$(_num("calc(round(to-zero, 105, 10))")), 100));
    assert$(_near(try$(_num("calc(round(to-zero, -105, 10))")), -100));
    assert$(_near(try$(_num("calc(round(-100, 10))")), -100));
    assert$(_near(try$(_num("calc(round(up, -103, 10))")), -100));
    return Ok();
}

test$("calc-round-default-step") {
    assert$(_near(try$(_num("round(1.1)")), 1));
    assert$(_near(try$(_num("round(up, 1.1)")), 2));
    assert$(_near(try$(_num("round(down, -1.1)")), -2));
    assert$(_near(try$(_num("round(to-zero, -1.9)")), -1));
    return Ok();
}

test$("calc-round-nested") {
    assert$(_near(try$(_num("calc(round(round(100,10), 10))")), 100));
    assert$(_near(try$(_num("calc(round(up, round(100,10) + 1,10))")), 110));
    assert$(_near(try$(_num("calc(round(down, round(100,10) + 2 * 3,10))")), 100));
    assert$(_near(try$(_num("calc(round(to-zero,round(100,10) * 2 - 95, 10))")), 100));
    assert$(_near(try$(_num("calc(round(round(100,10)* -1,10))")), -100));
    assert$(_near(try$(_num("calc(round(up, -103 + -103 / -103 - 1,10))")), -100));
    return Ok();
}

test$("calc-round-lengths") {
    assertEq$(try$(_px("round(7px, 5px)")), "5px"s);
    assertEq$(try$(_px("round(10px,6px)")), "12px"s);
    assertEq$(try$(_px("round(10in,6in)")), "1152px"s);
    assertEq$(try$(_px("round(100px, 1in)")), "96px"s);
    assertEq$(try$(_px("round(up, 7px, 5px)")), "10px"s);
    assertEq$(try$(_px("ROUND(DOWN, 7px, 5px)")), "5px"s);
    return Ok();
}

test$("calc-round-percent") {
    assertEq$(try$(_used("round(10%, 1px)", 80)), 8.0);
    assertEq$(try$(_used("round(10%, 5px)", 80)), 10.0);
    assertEq$(try$(_used("round(up, 10%, 3px)", 80)), 9.0);
    return Ok();
}

test$("calc-round-serialization") {
    assertEq$(try$(_specified<LengthPercentage>("round(10%, 1px)")), "round(10%, 1px)"s);
    assertEq$(try$(_specified<LengthPercentage>("round(nearest, 10%, 1px)")), "round(10%, 1px)"s);
    assertEq$(try$(_specified<LengthPercentage>("round(up, 10%, 1px)")), "round(up, 10%, 1px)"s);
    assertEq$(try$(_specified<LengthPercentage>("round(down, 10%, 1px)")), "round(down, 10%, 1px)"s);
    assertEq$(try$(_specified<LengthPercentage>("round(to-zero, 10%, 1px)")), "round(to-zero, 10%, 1px)"s);
    assertEq$(try$(_lp("round(up, 10%, 1px)")), "round(up, 10%, 1px)"s);
    return Ok();
}

// FIXME: round() doesn't follow CSS math semantics yet, ties must go towards +∞.
test$("calc-round-nearest-ties-go-up", XFAIL) {
    assert$(_near(try$(_num("round(2.5)")), 3));
    assert$(_near(try$(_num("round(-2.5)")), -2));
    assert$(_near(try$(_num("round(15, 10)")), 20));
    assert$(_near(try$(_num("round(-15, 10)")), -10));
    return Ok();
}

// FIXME: round() doesn't follow CSS math semantics yet, only the magnitude of B matters.
test$("calc-round-negative-step", XFAIL) {
    assert$(_near(try$(_num("round(up, 101, -10)")), 110));
    assert$(_near(try$(_num("round(down, 101, -10)")), 100));
    return Ok();
}

// FIXME: round() doesn't follow CSS math semantics yet.
test$("calc-round-zero-step-is-nan", XFAIL) {
    assert$(Math::isNan(try$(_num("round(1, 0)"))));
    assert$(Math::isNan(try$(_num("round(infinity, infinity)"))));
    return Ok();
}

// FIXME: round() doesn't follow CSS math semantics yet.
//        Karm's Math::floor/ceil/round also go through isize, which is UB for infinities.
test$("calc-round-infinite-a", XFAIL) {
    assertEq$(try$(_num("round(up, infinity, 4)")), Math::INF);
    assertEq$(try$(_num("round(down, -infinity, 4)")), Math::NEG_INF);
    assertEq$(try$(_num("round(infinity, -4)")), Math::INF);
    return Ok();
}

// FIXME: round() doesn't follow CSS math semantics yet.
test$("calc-round-infinite-b", XFAIL) {
    assertEq$(try$(_num("round(4, infinity)")), 0.0);
    assertEq$(try$(_num("round(to-zero, -4, infinity)")), 0.0);
    assert$(_isNegZero(try$(_num("round(-4, infinity)"))));
    assertEq$(try$(_num("round(up, 4, infinity)")), Math::INF);
    assert$(_isNegZero(try$(_num("round(up, -4, infinity)"))));
    assertEq$(try$(_num("round(down, -4, infinity)")), Math::NEG_INF);
    assert$(_isPosZero(try$(_num("round(down, 4, infinity)"))));
    return Ok();
}

// MARK: mod() / rem() ---------------------------------------------------------

test$("calc-mod-same-sign") {
    assert$(_near(try$(_num("mod(1,1)")), 0));
    assert$(_near(try$(_num("mod(0, 2)")), 0));
    assert$(_near(try$(_num("mod(2, 2)")), 0));
    assert$(_near(try$(_num("mod(-2,-2)")), 0));
    assert$(_near(try$(_num("mod(-4,-2)")), 0));
    assert$(_near(try$(_num("mod(1, 2)")), 1));
    assert$(_near(try$(_num("mod(3, 2)")), 1));
    assert$(_near(try$(_num("mod(-1, -2)")), -1));
    assert$(_near(try$(_num("mod(18,5)")), 3));
    assert$(_near(try$(_num("mod(-140,-90)")), -50));
    assertEq$(try$(_px("mod(10px,6px)")), "4px"s);
    assertEq$(try$(_px("mod(10in,6in)")), "384px"s);
    assertEq$(try$(_px("MOD(10px, 6px)")), "4px"s);
    return Ok();
}

// FIXME: mod() doesn't follow CSS math semantics yet, the result must take the sign of B.
test$("calc-mod-mixed-sign", XFAIL) {
    assert$(_near(try$(_num("mod(-1, 2)")), 1));
    assert$(_near(try$(_num("mod(1, -2)")), -1));
    assert$(_near(try$(_num("mod(3, -2)")), -1));
    assert$(_near(try$(_num("mod(-18,5)")), 2));
    assert$(_near(try$(_num("mod(140,-90)")), -40));
    assertEq$(try$(_px("mod(-10px,6px)")), "2px"s);
    assert$(_near(try$(_num("calc(mod(rem(1,18)* -1,5))")), 4));
    return Ok();
}

test$("calc-rem") {
    assert$(_near(try$(_num("rem(1,1)")), 0));
    assert$(_near(try$(_num("rem(18,5)")), 3));
    assert$(_near(try$(_num("rem(-18,5)")), -3));
    assert$(_near(try$(_num("rem(140,-90)")), 50));
    assertEq$(try$(_px("rem(10px,6px)")), "4px"s);
    assertEq$(try$(_px("rem(-10px,6px)")), "-4px"s);
    assertEq$(try$(_px("rem(-7px, 5px)")), "-2px"s);
    return Ok();
}

test$("calc-mod-rem-nested") {
    assert$(_near(try$(_num("calc(mod(18,5) * 2 + mod(17,5))")), 8));
    assert$(_near(try$(_num("calc(rem(mod(18,5),5))")), 3));
    assert$(_near(try$(_num("calc(rem(mod(18,5),mod(17,5)))")), 1));
    return Ok();
}

test$("calc-mod-rem-percent") {
    assertEq$(try$(_used("mod(3px + 0%, 2px)", 100)), 1.0);
    assertEq$(try$(_used("rem(3px + 0%, 2px)", 100)), 1.0);
    assertEq$(try$(_used("calc(rem(3px + 0%, 2px + 0%))", 100)), 1.0);
    assertEq$(try$(_used("mod(50%, 30px)", 100)), 20.0);
    assertEq$(try$(_specified<LengthPercentage>("mod(50%, 30px)")), "mod(50%, 30px)"s);
    assertEq$(try$(_specified<LengthPercentage>("rem(50%, 30px)")), "rem(50%, 30px)"s);
    return Ok();
}

test$("calc-mod-rem-nan") {
    assert$(Math::isNan(try$(_num("mod(1, 0)"))));
    assert$(Math::isNan(try$(_num("rem(1, 0)"))));
    assert$(Math::isNan(try$(_num("mod(infinity, 1)"))));
    assert$(Math::isNan(try$(_num("rem(-infinity, 1)"))));
    return Ok();
}

test$("calc-mod-rem-infinite-b") {
    assert$(_near(try$(_num("mod(4, infinity)")), 4));
    assert$(_near(try$(_num("mod(-4, -infinity)")), -4));
    assert$(_near(try$(_num("rem(4, -infinity)")), 4));
    assert$(_near(try$(_num("rem(-4, infinity)")), -4));
    return Ok();
}

// FIXME: mod() doesn't follow CSS math semantics yet.
//        With B infinite and A of the opposite sign (including signed zeros), the result is NaN.
test$("calc-mod-infinite-b-opposite-sign", XFAIL) {
    assert$(Math::isNan(try$(_num("mod(-4, infinity)"))));
    assert$(Math::isNan(try$(_num("mod(4, -infinity)"))));
    assert$(Math::isNan(try$(_num("mod(0, -infinity)"))));
    assert$(Math::isNan(try$(_num("mod(-1 * 0, infinity)"))));
    return Ok();
}

// MARK: Simplification & serialization ----------------------------------------

test$("calc-percent-times-number-stays-percent") {
    assertEq$(try$(_lp("calc(50% * 2)")), "100%"s);
    assertEq$(try$(_lp("calc(50% / 2)")), "25%"s);
    assertEq$(try$(_lp("calc(50% + 50%)")), "100%"s);
    assertEq$(try$(_lp("calc(10% - 10%)")), "0%"s);
    return Ok();
}

test$("calc-percent-ratio-is-number") {
    // 10% / 1% is a plain number whatever the basis is.
    assertEq$(try$(_lp("calc(1px * 10% / 1%)")), "10px"s);
    return Ok();
}

test$("calc-sum-fold-and-sort") {
    assertEq$(try$(_lp("calc(1px + 2% + 3px)")), "calc(2% + 4px)"s);
    assertEq$(try$(_lp("calc(1px + 1%)")), "calc(1% + 1px)"s);
    assertEq$(try$(_lp("calc(10% - 10px + 10px)")), "calc(10% + 0px)"s);
    return Ok();
}

test$("calc-negative-terms") {
    assertEq$(try$(_lp("calc(-10% - 5px)")), "calc(-10% - 5px)"s);
    assertEq$(try$(_lp("calc(-1px + 1%)")), "calc(1% - 1px)"s);
    assertEq$(try$(_lp("calc(-1 * (1% + 1px))")), "calc(-1% - 1px)"s);
    return Ok();
}

test$("calc-distribute-number-over-sum") {
    assertEq$(try$(_lp("calc((1% + 1px) * 2)")), "calc(2% + 2px)"s);
    assertEq$(try$(_lp("calc((10% + 1px) / 2)")), "calc(5% + 0.5px)"s);
    assertEq$(try$(_lp("calc(2 * (1% + 1px) - 1px)")), "calc(2% + 1px)"s);
    return Ok();
}

test$("calc-unsimplifiable-functions") {
    assertEq$(try$(_lp("min(10px, max(5%, 2px))")), "min(10px, max(5%, 2px))"s);
    assertEq$(try$(_lp("min(1% + 1px, 2px)")), "min(1% + 1px, 2px)"s);
    assertEq$(try$(_lp("min(20%, 10%)")), "min(20%, 10%)"s);
    assertEq$(try$(_lp("calc(-1 * min(10%, 5px))")), "calc(-1 * min(10%, 5px))"s);
    assertEq$(try$(_lp("calc(min(10%, 5px) * 2)")), "calc(2 * min(10%, 5px))"s);
    assertEq$(try$(_lp("calc(2 * min(1%, 1px) * 3)")), "calc(6 * min(1%, 1px))"s);

    return Ok();
}

// FIXME: Computed values aren't clamped yet.
// https://drafts.csswg.org/css-values-4/#calc-ieee
// A top-level NaN acts as 0, and infinities clamp to the largest/smallest supported value
// (Chrome computes calc(1px / 0) to 3.35544e+07px and calc(NaN * 1px) to 0px).
// NOTE: `calc(infinity * 1px)` is only the serialization of a *specified* value.
test$("calc-computed-infinity-and-nan-are-clamped", XFAIL) {
    auto cx = Style::RelativeLengthContextData{};

    auto inf = Style::toComputed(try$(_parse<Length>("calc(1px / 0)")), cx).value();
    assert$(inf > 0 and not Math::isInf(inf));

    auto negInf = Style::toComputed(try$(_parse<Length>("calc(-infinity * 1px)")), cx).value();
    assert$(negInf < 0 and not Math::isInf(negInf));

    assertEq$(try$(_px("calc(NaN * 1px)")), "0px"s);
    return Ok();
}

test$("calc-infinity-and-nan-values") {
    auto inf = try$(_num("calc(1 / 0)"));
    assert$(inf > 0 and Math::isInf(inf));
    assert$(Math::isNan(try$(_num("sqrt(-1)"))));
    return Ok();
}

// MARK: Used values -----------------------------------------------------------

test$("calc-used-value") {
    assertEq$(try$(_used("calc(50% * 2)", 200)), 200.0);
    assertEq$(try$(_used("calc(100% - 10px)", 200)), 190.0);
    assertEq$(try$(_used("calc(50% + 0.5px)", 3)), 2.0);
    assertEq$(try$(_used("min(100%, 50px)", 200)), 50.0);
    return Ok();
}

test$("calc-used-value-clamp") {
    assertEq$(try$(_used("clamp(10px, 50%, 100px)", 1000)), 100.0);
    assertEq$(try$(_used("clamp(10px, 50%, 100px)", 10)), 10.0);
    assertEq$(try$(_used("clamp(10px, 50%, 100px)", 100)), 50.0);
    return Ok();
}

test$("calc-used-value-negative-basis") {
    // Against a negative basis, max(10%, 20%) is 10%, not 20%.
    assertEq$(try$(_used("max(10%, 20%)", -100)), -10.0);
    return Ok();
}

test$("calc-used-value-without-basis") {
    assertEq$(try$(_usedOpt("10px", NONE)), Opt<f64>{Some(10.0)});
    assertEq$(try$(_usedOpt("10%", NONE)), Opt<f64>{NONE});
    assertEq$(try$(_usedOpt("calc(10% + 5px)", NONE)), Opt<f64>{NONE});
    assertEq$(try$(_usedOpt("min(10%, 5px)", NONE)), Opt<f64>{NONE});
    assertEq$(try$(_usedOpt("calc(10% + 5px)", Some(100.0))), Opt<f64>{Some(15.0)});
    assertEq$(try$(_usedOpt("min(10%, 5px)", Some(100.0))), Opt<f64>{Some(5.0)});
    return Ok();
}

// MARK: Other contexts --------------------------------------------------------

test$("calc-number-context") {
    assert$(_near(try$(_num("calc(2 * 3 + 1)")), 7));
    assert$(_near(try$(_num("calc(pi)")), Math::PI));
    assert$(_near(try$(_num("calc(1px / 1px)")), 1));
    assert$(_near(try$(_num("calc(1in / 1px)")), 96));
    assert$(_near(try$(_num("calc(1e2)")), 100));
    assertNot$(_valid<Number>("calc(50%)"));
    assertNot$(_valid<Number>("calc(1px)"));
    return Ok();
}

test$("calc-percentage-context") {
    assert$(_valid<Percentage>("calc(50%)"));
    assertEq$(try$(_pct("calc(50% * 2)")), 100.0);
    assertEq$(try$(_pct("calc(50% + 10%)")), 60.0);
    assertEq$(try$(_pct("min(20%, 10%)")), 10.0);
    assertNot$(_valid<Percentage>("calc(1px)"));
    assertNot$(_valid<Percentage>("calc(1% + 1px)"));
    assertNot$(_valid<Percentage>("calc(10% * 10%)"));
    return Ok();
}

test$("calc-angle-context") {
    assert$(_valid<Angle>("calc(1turn + 90deg)"));
    assert$(_valid<Angle>("atan2(1px, 2px)"));
    assertNot$(_valid<Angle>("calc(1px)"));
    assertNot$(_valid<Angle>("calc(10%)"));
    assert$(_valid<AnglePercentage>("calc(50% + 1deg)"));
    assertNot$(_valid<AnglePercentage>("calc(50% + 1px)"));
    return Ok();
}

test$("calc-other-contexts") {
    assert$(_valid<Resolution>("calc(2dppx)"));
    assert$(_valid<Resolution>("calc(1dppx + 96dpi)"));
    assertNot$(_valid<Resolution>("calc(1px)"));
    assert$(_valid<FrequencyPercentage>("calc(1hz + 10%)"));
    assertNot$(_valid<FrequencyPercentage>("calc(1hz + 1px)"));
    return Ok();
}

// MARK: Parsing ---------------------------------------------------------------

test$("calc-parse-function-names-are-case-insensitive") {
    assertEq$(try$(_px("CALC(1px)")), "1px"s);
    assertEq$(try$(_px("Min(1px, 2px)")), "1px"s);
    return Ok();
}

test$("calc-parse-whitespace") {
    assertEq$(try$(_px("calc( 1px + 2px )")), "3px"s);
    assertEq$(try$(_px("min( 1px , 2px )")), "1px"s);
    assertEq$(try$(_px("calc(2px*3)")), "6px"s);
    assertEq$(try$(_px("calc(6px/3)")), "2px"s);
    assertEq$(try$(_px("calc((1px) + (2px))")), "3px"s);
    assertEq$(try$(_px("calc( ( 1px + 2px ) * 2 )")), "6px"s);
    assertEq$(try$(_px("calc(1px\n+\t2px)")), "3px"s);
    assertEq$(try$(_px("min(1px + 2px , 10px)")), "3px"s);
    assertEq$(try$(_px("clamp(none , 2px , none)")), "2px"s);
    assertEq$(try$(_px("max( 1px ,2px,  3px )")), "3px"s);
    return Ok();
}

test$("calc-parse-plus-minus-require-whitespace") {
    // https://drafts.csswg.org/css-values-4/#calc-syntax
    // White space is required on both sides of the + and - operators.
    assertNot$(_valid<Length>("calc(1px+ 2px)"));
    assertNot$(_valid<Length>("calc(1px -(2px))"));
    assertNot$(_valid<Length>("calc(1px +2px)"));
    assertNot$(_valid<Length>("calc((1px)+(2px))"));
    assertNot$(_valid<Length>("calc(1px +(2px))"));
    assertNot$(_valid<Length>("calc(1px- 2px)"));
    return Ok();
}

test$("calc-parse-rejects-type-errors") {
    assertNot$(_valid<Length>("calc(0)"));
    assertNot$(_valid<Length>("calc(1px * 2px)"));
    assertNot$(_valid<Length>("calc(1px + 1)"));
    assertNot$(_valid<Length>("calc(0 + 1px)"));
    assertNot$(_valid<Length>("calc(1px + 1deg)"));
    assertNot$(_valid<Length>("calc(1px * sin(1px))"));
    assertNot$(_valid<Length>("calc(1px * pow(1px, 2))"));
    assertNot$(_valid<Length>("calc(1px * pow(2px, 2))"));
    assertNot$(_valid<Length>("min(1px, 1deg)"));
    assertNot$(_valid<Length>("clamp(1px, 2px, 3deg)"));
    assertNot$(_valid<Length>("hypot(1px, 1)"));
    assertNot$(_valid<Length>("calc(1px * atan2(1px, 1deg) / 1deg)"));
    assertNot$(_valid<LengthPercentage>("calc(1px / 1%)"));
    assertNot$(_valid<LengthPercentage>("calc(1% * 1%)"));
    return Ok();
}

test$("calc-parse-rejects-percent-in-length") {
    assertNot$(_valid<Length>("calc(10%)"));
    assertNot$(_valid<Length>("min(1px, 2%)"));
    assertNot$(_valid<Length>("calc(1px * (10% / 1%))"));
    return Ok();
}

test$("calc-parse-rejects-bad-arity") {
    assertNot$(_valid<Length>("clamp(1px, 2px)"));
    assertNot$(_valid<Length>("min()"));
    assertNot$(_valid<Length>("calc()"));
    assertNot$(_valid<Length>("calc(1px,)"));
    assertNot$(_valid<Length>("min(1px, 2px,)"));
    assertNot$(_valid<Length>("calc(1px, 2px)"));
    assertNot$(_valid<Length>("hypot()"));
    assertNot$(_valid<Length>("max(1px,,2px)"));
    assertNot$(_valid<Length>("max(,1px)"));
    assertNot$(_valid<Length>("calc(1px * log(8, 2, 3))"));
    assertNot$(_valid<Length>("calc(1px * sin(1, 2))"));
    assertNot$(_valid<Length>("calc(1px * pow(2))"));
    assertNot$(_valid<Length>("calc(1px * sqrt())"));
    assertNot$(_valid<Length>("calc(1px * atan2(1))"));
    assertNot$(_valid<Number>("log(8,)"));
    assertNot$(_valid<Number>("abs()"));
    assertNot$(_valid<Number>("abs(1, 2)"));
    assertNot$(_valid<Number>("sign()"));
    assertNot$(_valid<Number>("sign(1, 2)"));
    return Ok();
}

test$("calc-parse-rejects-junk") {
    assertNot$(_valid<Length>("calc((1px 2px))"));
    assertNot$(_valid<Length>("calc(1px +)"));
    assertNot$(_valid<Length>("calc(1px *)"));
    assertNot$(_valid<Length>("calc(* 1px)"));
    assertNot$(_valid<Length>("calc(())"));
    assertNot$(_valid<Length>("calc(1px ())"));
    assertNot$(_valid<Length>("min(1px 2px)"));
    assertNot$(_valid<Length>("clamp(none none, 1px, 2px)"));
    assertNot$(_valid<Length>("clamp(none, 1px, none none)"));
    assertNot$(_valid<Number>("log(8 2)"));
    assertNot$(_valid<Number>("atan2(1 2)"));
    return Ok();
}

test$("calc-parse-rejects-unknown-names") {
    assertNot$(_valid<Length>("calcx(1px)"));
    assertNot$(_valid<Length>("calc(foo(1px))"));
    assertNot$(_valid<Length>("calc(foo * 1px)"));
    assertNot$(_valid<Length>("calc(1e)"));
    assertNot$(_valid<Length>("calc(1foo)"));
    // Only -infinity is a keyword; -pi and -e are unknown identifiers.
    assertNot$(_valid<Length>("calc(-pi * 1px)"));
    assertNot$(_valid<Length>("calc(-e * 1px)"));
    return Ok();
}

test$("calc-parse-optional-params") {
    assert$(_near(try$(_num("log( 8 , 2 )")), 3));
    assert$(_near(try$(_num("round( up , 1.1 )")), 2));
    assert$(_near(try$(_num("round( up , 101 , 10 )")), 110));
    assertEq$(try$(_px("min(5px, 4px, 3px, 2px, 1px)")), "1px"s);
    return Ok();
}

test$("calc-parse-round-invalid") {
    // https://wpt.fyi/results/css/css-values/round-mod-rem-invalid.html
    for (auto args : Array{
             ""s, " "s, ","s, "1, "s, ", 1"s, "1 + "s, "1 - "s, "1 * "s, "1 / "s, "1 2"s,
             "nearest, 1 2"s, "1, nearest, 12"s, "1, nearest"s, "nearest, 1, nearest"s, "1, , 2"s,
             "nearest"s, "nearest,"s, "sideways, 1, 2"s, "1, 2, 3"s, "0px"s, "1, 1%"s, "1, 0px"s,
             "up 1, 2"s, "up up, 1, 2"s, "up + 1, 2"s, "1, up, 2"s, "up"s, "up, "s,
         }) {
        auto input = Io::format("round({})", args);
        if (_valid<Number>(input.str()))
            return Error::invalidData("accepted invalid round()");
    }
    assertNot$(_valid<LengthPercentage>("round(0px)"));
    assertNot$(_valid<LengthPercentage>("round(0px, 0s)"));
    assertNot$(_valid<Length>("round(10%, 1px)"));
    return Ok();
}

test$("calc-parse-mod-rem-invalid") {
    // https://wpt.fyi/results/css/css-values/round-mod-rem-invalid.html
    for (auto fn : Array{"mod"s, "rem"s}) {
        for (auto args : Array{
                 ""s, " "s, ","s, "1, "s, ", 1"s, "1 + "s, "1 - "s, "1 * "s, "1 / "s,
                 "1 2"s, "1, , 2"s, "1"s, "1, 2, 3"s, "0px"s, "1, 1%"s, "1, 0px"s, "1, 0deg"s,
             }) {
            auto input = Io::format("{}({})", fn, args);
            if (_valid<Number>(input.str()))
                return Error::invalidData("accepted invalid mod()/rem()");
        }
        assertNot$(_valid<LengthPercentage>(Io::format("{}(0px, 0s)", fn).str()));
        assertNot$(_valid<Length>(Io::format("{}(10%, 1px)", fn).str()));
    }
    return Ok();
}

// FIXME: ValueParser<UnitValue> parses dimensions with `allowExp = false`,
//        so a dimension with an exponent is read as a bogus unit.
test$("calc-parse-dimension-with-exponent", XFAIL) {
    assertEq$(try$(_px("1e2px")), "100px"s);
    assertEq$(try$(_px("calc(1e2px)")), "100px"s);
    assertEq$(try$(_px("calc(1e+2px)")), "100px"s);
    assertEq$(try$(_px("calc(1E2px)")), "100px"s);
    return Ok();
}

} // namespace Vaev::Css::Tests
