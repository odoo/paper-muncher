#include <karm/test>

import Vaev.Engine;
import Karm.Diag;
import Karm.Logger;

using namespace Karm;
using namespace Karm::Literals;

namespace Vaev::Ng::Tests {

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

template <typename T>
static Res<String> _computed(Str input) {
    auto calc = try$(_parse<T>(input));
    auto computed = resolve(calc);
    return _serialize(toSpecifid(computed));
}

template <>
Res<String> _computed<Length>(Str input) {
    auto calc = try$(_parse<Length>(input));
    auto cx = RelativeLengthContextData{};
    auto computed = resolve(calc, cx);
    return _serialize(toSpecified(computed));
}

template <>
Res<String> _computed<LengthPercentage>(Str input) {
    auto calc = try$(_parse<LengthPercentage>(input));
    auto cx = RelativeLengthContextData{};
    auto computed = resolve(calc, cx);
    return _serialize(toSpecified(computed));
}

test$("parse-simple-numeric-values") {
    assertEq$(try$(_computed<Length>("1px")), "1px"s);
    assertEq$(try$(_computed<Length>("calc(1px)")), "1px"s);
    assertEq$(try$(_computed<Length>("min(1px, 2px, 3px)")), "1px"s);
    assertEq$(try$(_computed<Length>("max(1px, 2px, 3px)")), "3px"s);
    assertEq$(try$(_computed<Length>("clamp(1px, 0px, 3px)")), "1px"s);
    assertEq$(try$(_computed<Length>("clamp(1px, 5px, 3px)")), "3px"s);
    assertEq$(try$(_computed<Length>("clamp(1px, 2px, 3px)")), "2px"s);
    assertEq$(try$(_computed<Length>("clamp(none, 5px, 3px)")), "3px"s);
    assertEq$(try$(_computed<Length>("clamp(1px, 0px, none)")), "1px"s);
    assertEq$(try$(_computed<Length>("clamp(none, 2px, none)")), "2px"s);
    assertEq$(try$(_computed<Length>("hypot(1px, 2px, 3px)")), "3.741657px"s);
    assertEq$(try$(_computed<Length>("calc(1px * sin(30deg))")), "0.5px"s);

    return Ok();
}

test$("parse-mixed-numeric-values") {
    assertEq$(try$(_computed<LengthPercentage>("1px")), "1px"s);
    assertEq$(try$(_computed<LengthPercentage>("1%")), "1%"s);
    assertEq$(try$(_computed<LengthPercentage>("calc(1px)")), "1px"s);
    assertEq$(try$(_computed<LengthPercentage>("calc(1%)")), "1%"s);
    assertEq$(try$(_computed<LengthPercentage>("min(1px, 2%, 3px)")), "min(1px, 2%, 3px)"s);
    assertEq$(try$(_computed<LengthPercentage>("max(1px, 2%, 3px)")), "max(1px, 2%, 3px)"s);
    assertEq$(try$(_computed<LengthPercentage>("clamp(1px, 2%, 3px)")), "clamp(1px, 2%, 3px)"s);
    assertEq$(try$(_computed<LengthPercentage>("clamp(none, 5%, 3px)")), "min(5%, 3px)"s);
    assertEq$(try$(_computed<LengthPercentage>("clamp(1px, 0%, none)")), "max(1px, 0%)"s);
    assertEq$(try$(_computed<LengthPercentage>("clamp(none, 2px, none)")), "2px"s);
    assertEq$(try$(_computed<LengthPercentage>("hypot(1px, 2px, 3%)")), "hypot(1px, 2px, 3%)"s);
    assertEq$(try$(_computed<LengthPercentage>("hypot(1px, 2px, 1px + 3%)")), "hypot(1px, 2px, 3% + 1px)"s);
    assertEq$(try$(_computed<LengthPercentage>("hypot(1px, 2px, 1px + 3%)")), "hypot(1px, 2px, 3% + 1px)"s);

    return Ok();
}

} // namespace Vaev::Ng::Tests
