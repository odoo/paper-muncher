/* Ported from web-platform-tests @ ba6d9f67 */
/* https://github.com/web-platform-tests/wpt/tree/ba6d9f678ee91de9f58097921ed34e9b6b6849d3 */

/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * The 3-Clause BSD License
 *
 * Copyright © web-platform-tests contributors
 *
 * Redistribution and use in source and binary forms, with or without modification, are permitted
 * provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 *    conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list of
 *    conditions and the following disclaimer in the documentation and/or other materials provided
 *    with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used to
 *    endorse or promote products derived from this software without specific prior written
 *    permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <karm/test>

import Vaev.Engine;
import Karm.Diag;

using namespace Karm;
using namespace Karm::Literals;

namespace Vaev::Style::Tests {

static Css::Content _parseComponentValues(Str text) {
    Css::Lexer lex{text};
    auto diags = Diag::Collector::ignore();
    Css::Content content;
    while (lex.peek() != Css::Token::END_OF_FILE)
        content.pushBack(Css::consumeComponentValue(lex, diags));
    return content;
}

// https://drafts.csswg.org/css-conditional-3/#dom-css-supports-conditiontext
static bool _supports(Str conditionText) {
    static auto registry = defaultRegistry();

    // If conditionText, parsed and evaluated as a <supports-condition>, would return true, return true.
    auto content = _parseComponentValues(conditionText);
    Cursor<Css::Sst> c = content;
    if (parseSupportsCondition(registry, c))
        return true;

    // Otherwise, If conditionText, wrapped in parentheses and then parsed and evaluated as a <supports-condition>,
    // would return true, return true.
    auto wrapped = _parseComponentValues(Io::format("({})", conditionText));
    Cursor<Css::Sst> w = wrapped;
    if (parseSupportsCondition(registry, w))
        return true;

    // Otherwise, return false.
    return false;
}

// https://drafts.csswg.org/css-conditional-3/#dom-css-supports
static bool _supports(Str property, Str value) {
    static auto registry = defaultRegistry();

    // If property is an ASCII case-insensitive match for any defined CSS property that the UA supports, or is a
    // custom property name string, and value successfully parses according to that property’s grammar, return true.
    if (startWith(property, "--"s) == Match::PARTIAL)
        return true;
    auto name = Symbol::from(Io::toLowerCase(property).expect("lowercasing a property name"));
    if (registry.resolveRegistration(name, {}) and registry.parseValue(name, value, RegisteredPropertySet::ALLOW_DEFAULTING))
        return true;

    // Otherwise, return false.
    return false;
}

static Res<> _expectSupports(Test::Driver& _driver, Str conditionText, bool expected) {
    assertEq$(_supports(conditionText), expected);
    return Ok();
}

static Res<> _expectSupports(Test::Driver& _driver, Str property, Str value, bool expected) {
    assertEq$(_supports(property, value), expected);
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-conditional/js/CSS-supports-L3.html
test$("vaev-style-wpt-css-css-conditional-js-CSS-supports-L3") {
    try$(_expectSupports(_driver, "(color: red)", true));
    try$(_expectSupports(_driver, "color: red", true));
    try$(_expectSupports(_driver, "(color: red) and (color: blue)", true));
    try$(_expectSupports(_driver, "not (foobar)", true));
    try$(_expectSupports(_driver, "color: something-pointless var(--foo)", true));
    try$(_expectSupports(_driver, "color: something-pointless(var(--foo))", true));
    try$(_expectSupports(_driver, "color", "red", true));
    try$(_expectSupports(_driver, "unknownproperty: blah", false));
    try$(_expectSupports(_driver, "unknownproperty", "blah", false));
    try$(_expectSupports(_driver, "unicode-range: U+0-7F", false));
    try$(_expectSupports(_driver, "unicode-range", "U+0-7F", false));
    try$(_expectSupports(_driver, "unicode-range: inherit", false));
    try$(_expectSupports(_driver, "unicode-range", "inherit", false));
    try$(_expectSupports(_driver, "width: blah", false));
    try$(_expectSupports(_driver, "width", "blah", false));
    try$(_expectSupports(_driver, "--foo: blah", true));
    try$(_expectSupports(_driver, "--foo", "blah", true));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-conditional/js/CSS-supports-L4.html
test$("vaev-style-wpt-css-css-conditional-js-CSS-supports-L4") {
    try$(_expectSupports(_driver, "selector(div, div)", false));
    try$(_expectSupports(_driver, "selector(::-webkit-unknown-pseudo-element)", false));
    try$(_expectSupports(_driver, "selector(div | .c)", false));
    try$(_expectSupports(_driver, "selector(:is(:foo))", false));
    try$(_expectSupports(_driver, "selector(:is(:foo, div))", false));
    try$(_expectSupports(_driver, "selector(:where(:foo))", false));
    try$(_expectSupports(_driver, "selector(:where(:foo, div))", false));
    try$(_expectSupports(_driver, "selector(:has(:foo))", false));
    try$(_expectSupports(_driver, "selector(:has(:foo, div))", false));
    try$(_expectSupports(_driver, "selector(:nth-child(2n + 3 of :foo))", false));
    try$(_expectSupports(_driver, "selector(:nth-last-child(2n + 3 of :foo))", false));
    try$(_expectSupports(_driver, "selector(:nth-child(2n + 3 of :foo, div))", false));
    try$(_expectSupports(_driver, "selector(:nth-last-child(2n + 3 of :foo, div))", false));
    try$(_expectSupports(_driver, "selector(:nth-child(2n 4))", false));
    try$(_expectSupports(_driver, "selector(:nth-last-child(2n 4))", false));
    return Ok();
}

test$("vaev-style-wpt-css-css-conditional-js-CSS-supports-L4-xfail", XFAIL) {
    try$(_expectSupports(_driver, "selector(div)", true));
    try$(_expectSupports(_driver, "selector(::before)", true));
    try$(_expectSupports(_driver, "selector(div + .c)", true));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-conditional/js/CSS-supports-L5.html
test$("vaev-style-wpt-css-css-conditional-js-CSS-supports-L5") {
    try$(_expectSupports(_driver, "font-format(xyzzy)", false));
    try$(_expectSupports(_driver, "font-format(opentype, truetype)", false));
    try$(_expectSupports(_driver, "font-format('opentype')", false));
    try$(_expectSupports(_driver, "font-tech(feature-opentype)", false));
    try$(_expectSupports(_driver, "font-tech(foobar)", false));
    try$(_expectSupports(_driver, "font-tech(features-opentype, color-COLRv0)", false));
    try$(_expectSupports(_driver, "font-tech('features-opentype')", false));
    return Ok();
}

test$("vaev-style-wpt-css-css-conditional-js-CSS-supports-L5-xfail", XFAIL) {
    try$(_expectSupports(_driver, "font-format(opentype)", true));
    try$(_expectSupports(_driver, "font-tech(features-opentype)", true));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-conditional/js/CSS-supports-details-content-pseudo-parsing.html
test$("vaev-style-wpt-css-css-conditional-js-CSS-supports-details-content-pseudo-parsing") {
    try$(_expectSupports(_driver, "selector(::details-content:nth-of-type(slot)", false));
    try$(_expectSupports(_driver, "selector(::details-content:has(a)", false));
    try$(_expectSupports(_driver, "selector(::details-content::part(a)", false));
    return Ok();
}

test$("vaev-style-wpt-css-css-conditional-js-CSS-supports-details-content-pseudo-parsing-xfail", XFAIL) {
    try$(_expectSupports(_driver, "selector(::details-content)", true));
    try$(_expectSupports(_driver, "selector(::details-content::before)", true));
    try$(_expectSupports(_driver, "selector(::details-content::first-line)", true));
    try$(_expectSupports(_driver, "selector(::details-content:hover", true));
    try$(_expectSupports(_driver, "selector(::details-content:lang(en)", true));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-conditional/js/CSS-supports-selector-detecting-invalid-in-logical-combinations.html
test$("vaev-style-wpt-css-css-conditional-js-CSS-supports-selector-detecting-invalid-in-logical-combinations") {
    try$(_expectSupports(_driver, "selector(:has(:foo))", false));
    try$(_expectSupports(_driver, "selector(:has(.a, :foo))", false));
    try$(_expectSupports(_driver, "selector(:has(:foo, .a))", false));
    try$(_expectSupports(_driver, "selector(:not(:foo))", false));
    try$(_expectSupports(_driver, "selector(:not(.a, :foo))", false));
    try$(_expectSupports(_driver, "selector(:not(:foo, .a))", false));
    try$(_expectSupports(_driver, "selector(:is(:foo))", false));
    try$(_expectSupports(_driver, "selector(:is(.a, :foo))", false));
    try$(_expectSupports(_driver, "selector(:is(:foo, .a))", false));
    try$(_expectSupports(_driver, "selector(:is(:has(:foo, a), .b))", false));
    try$(_expectSupports(_driver, "selector(:where(:foo))", false));
    try$(_expectSupports(_driver, "selector(:where(.a, :foo))", false));
    try$(_expectSupports(_driver, "selector(:where(:foo, .a))", false));
    try$(_expectSupports(_driver, "selector(:where(:is(:foo, a), .b))", false));
    try$(_expectSupports(_driver, "selector(:has(:where(:foo, a), .b))", false));
    try$(_expectSupports(_driver, "selector(:is())", false));
    try$(_expectSupports(_driver, "selector(:where())", false));
    try$(_expectSupports(_driver, "selector(:has())", false));
    try$(_expectSupports(_driver, "selector(:has(:has(.a)))", false));
    try$(_expectSupports(_driver, "selector(:has(:has(.a), b))", false));
    try$(_expectSupports(_driver, "selector(.a, :has(:has(.b)))", false));
    try$(_expectSupports(_driver, "selector(:has(:is(:has(.a))))", false));
    try$(_expectSupports(_driver, "selector(:has(:is(:has(.a), .b)))", false));
    try$(_expectSupports(_driver, "selector(:has(:is(.a, :has(.b))))", false));
    try$(_expectSupports(_driver, "selector(:is(::after)", false));
    try$(_expectSupports(_driver, "selector(:is(::before)", false));
    try$(_expectSupports(_driver, "selector(:is(::first-letter)", false));
    try$(_expectSupports(_driver, "selector(:is(::first-line)", false));
    try$(_expectSupports(_driver, "selector(:where(::after)", false));
    try$(_expectSupports(_driver, "selector(:where(::before)", false));
    try$(_expectSupports(_driver, "selector(:where(::first-letter)", false));
    try$(_expectSupports(_driver, "selector(:where(::first-line)", false));
    try$(_expectSupports(_driver, "selector(:has(::after)", false));
    try$(_expectSupports(_driver, "selector(:has(::before)", false));
    try$(_expectSupports(_driver, "selector(:has(::first-letter)", false));
    try$(_expectSupports(_driver, "selector(:has(::first-line)", false));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-conditional/js/CSS-supports-selector-picker.html
test$("vaev-style-wpt-css-css-conditional-js-CSS-supports-selector-picker") {
    try$(_expectSupports(_driver, "selector(select::picker)", false));
    try$(_expectSupports(_driver, "selector(select::picker())", false));
    return Ok();
}

test$("vaev-style-wpt-css-css-conditional-js-CSS-supports-selector-picker-xfail", XFAIL) {
    try$(_expectSupports(_driver, "selector(select::picker-icon)", true));
    try$(_expectSupports(_driver, "selector(select::picker(select))", true));
    try$(_expectSupports(_driver, "selector(::picker-icon)", true));
    try$(_expectSupports(_driver, "selector(::picker(select))", true));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-conditional/js/supports-at-rule.html
test$("vaev-style-wpt-css-css-conditional-js-supports-at-rule") {
    try$(_expectSupports(_driver, "at-rule(@doesnotexist)", false));
    try$(_expectSupports(_driver, "at-rule(@charset)", false));
    try$(_expectSupports(_driver, "at-rule(@counter-style; system: fixed)", false));
    try$(_expectSupports(_driver, "at-rule(@doesnotexist) and (color: red)", false));
    try$(_expectSupports(_driver, "at-rule(@supports) and (doesnotexist: red)", false));
    try$(_expectSupports(_driver, "at-rule(@supports) or (color: red)", true));
    try$(_expectSupports(_driver, "at-rule(@doesnotexist) or (color: red)", true));
    try$(_expectSupports(_driver, "at-rule(@doesnotexist) or (doesnotexist: red)", false));
    try$(_expectSupports(_driver, "not at-rule(@doesnotexist)", true));
    try$(_expectSupports(_driver, "(color: red) and at-rule(@doesnotexist)", false));
    try$(_expectSupports(_driver, "(color: red) or at-rule(@doesnotexist)", true));
    try$(_expectSupports(_driver, "(doesnotexist: red) or at-rule(@doesnotexist)", false));
    return Ok();
}

test$("vaev-style-wpt-css-css-conditional-js-supports-at-rule-xfail", XFAIL) {
    try$(_expectSupports(_driver, "at-rule(@supports)", true));
    try$(_expectSupports(_driver, "at-rule( @supports)", true));
    try$(_expectSupports(_driver, "at-rule(@supports )", true));
    try$(_expectSupports(_driver, "at-rule(@media)", true));
    try$(_expectSupports(_driver, "at-rule(@counter-style)", true));
    try$(_expectSupports(_driver, "at-rule(@import)", true));
    try$(_expectSupports(_driver, "at-rule(@swash)", true));
    try$(_expectSupports(_driver, "at-rule(@starting-style)", true));
    try$(_expectSupports(_driver, "at-rule(@supports) and (color: red)", true));
    try$(_expectSupports(_driver, "at-rule(@media) and (display: flex)", true));
    try$(_expectSupports(_driver, "at-rule(@supports) or (doesnotexist: red)", true));
    try$(_expectSupports(_driver, "not at-rule(@supports)", false));
    try$(_expectSupports(_driver, "(color: red) and at-rule(@supports)", true));
    return Ok();
}

} // namespace Vaev::Style::Tests
