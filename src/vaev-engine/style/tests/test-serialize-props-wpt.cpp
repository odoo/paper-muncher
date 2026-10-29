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

using namespace Karm;
using namespace Karm::Literals;

namespace Vaev::Style::Tests {

static Res<> _expectSerialized(Test::Driver& _driver, Str name, Str input, Str expected, Flags<RegisteredPropertySet::Options> options = {}) {
    static auto registry = defaultRegistry();
    auto prop = try$(registry.parseValue(Symbol::from(name), input, options));
    Io::StringWriter w;
    try$(prop->serialize(w));
    assertEq$(w.take(), expected);
    return Ok();
}

static Res<> _expectInvalid(Test::Driver& _driver, Str name, Str input) {
    static auto registry = defaultRegistry();
    assert$(not registry.parseValue(Symbol::from(name), input, {}));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-multicol/parsing/column-count-invalid.html
test$("vaev-style-wpt-css-css-multicol-parsing-column-count-invalid") {
    try$(_expectInvalid(_driver, "column-count", "none"));
    try$(_expectInvalid(_driver, "column-count", "2.5"));
    try$(_expectInvalid(_driver, "column-count", "auto 1"));
    try$(_expectInvalid(_driver, "column-count", "1 234"));
    return Ok();
}

test$("vaev-style-wpt-css-css-multicol-parsing-column-count-invalid-xfail", XFAIL) {
    try$(_expectInvalid(_driver, "column-count", "-1"));
    try$(_expectInvalid(_driver, "column-count", "0"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-multicol/parsing/column-count-valid.html
test$("vaev-style-wpt-css-css-multicol-parsing-column-count-valid") {
    try$(_expectSerialized(_driver, "column-count", "auto", "auto"));
    try$(_expectSerialized(_driver, "column-count", "1", "1"));
    try$(_expectSerialized(_driver, "column-count", "234", "234"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-multicol/parsing/column-width-invalid.html
test$("vaev-style-wpt-css-css-multicol-parsing-column-width-invalid") {
    try$(_expectInvalid(_driver, "column-width", "none"));
    try$(_expectInvalid(_driver, "column-width", "10"));
    try$(_expectInvalid(_driver, "column-width", "30%"));
    try$(_expectInvalid(_driver, "column-width", "auto 40px"));
    return Ok();
}

test$("vaev-style-wpt-css-css-multicol-parsing-column-width-invalid-xfail", XFAIL) {
    try$(_expectInvalid(_driver, "column-width", "-20px"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-multicol/parsing/column-width-valid.html
test$("vaev-style-wpt-css-css-multicol-parsing-column-width-valid") {
    try$(_expectSerialized(_driver, "column-width", "auto", "auto"));
    try$(_expectSerialized(_driver, "column-width", "0", "0px"));
    try$(_expectSerialized(_driver, "column-width", "10px", "10px"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-multicol/parsing/columns-invalid.html
test$("vaev-style-wpt-css-css-multicol-parsing-columns-invalid") {
    try$(_expectInvalid(_driver, "columns", "none"));
    try$(_expectInvalid(_driver, "columns", "10px 20px"));
    try$(_expectInvalid(_driver, "columns", "10 20"));
    try$(_expectInvalid(_driver, "columns", "0 7px"));
    try$(_expectInvalid(_driver, "columns", "auto auto auto"));
    try$(_expectInvalid(_driver, "columns", "10em auto auto"));
    try$(_expectInvalid(_driver, "columns", "initial initial"));
    try$(_expectInvalid(_driver, "columns", "inherit inherit"));
    try$(_expectInvalid(_driver, "columns", "/ 100px"));
    try$(_expectInvalid(_driver, "columns", "/ auto"));
    try$(_expectInvalid(_driver, "columns", "/"));
    try$(_expectInvalid(_driver, "columns", "auto /"));
    try$(_expectInvalid(_driver, "columns", "100px /"));
    try$(_expectInvalid(_driver, "columns", "2 /"));
    try$(_expectInvalid(_driver, "columns", "2 100px /"));
    try$(_expectInvalid(_driver, "columns", "100px 2 /"));
    return Ok();
}

test$("vaev-style-wpt-css-css-multicol-parsing-columns-invalid-xfail", XFAIL) {
    try$(_expectInvalid(_driver, "columns", "0 0"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-multicol/parsing/columns-valid.html
test$("vaev-style-wpt-css-css-multicol-parsing-columns-valid") {
    try$(_expectSerialized(_driver, "columns", "auto 3", "3"));
    try$(_expectSerialized(_driver, "columns", "auto 10em", "10em"));
    try$(_expectSerialized(_driver, "columns", "3 auto", "3"));
    try$(_expectSerialized(_driver, "columns", "10em auto", "10em"));
    try$(_expectSerialized(_driver, "columns", "2 10px", "10px 2"));
    try$(_expectSerialized(_driver, "columns", "10px 2", "10px 2"));
    try$(_expectSerialized(_driver, "columns", "auto", "auto"));
    try$(_expectSerialized(_driver, "columns", "auto auto", "auto"));
    try$(_expectSerialized(_driver, "columns", "7", "7"));
    try$(_expectSerialized(_driver, "columns", "7em", "7em"));
    try$(_expectSerialized(_driver, "columns", "0 1", "0px 1"));
    try$(_expectSerialized(_driver, "columns", "1 0", "0px 1"));
    try$(_expectSerialized(_driver, "columns", "0px 1", "0px 1"));
    try$(_expectSerialized(_driver, "columns", "10px 2 / 100px", "10px 2 / 100px"));
    try$(_expectSerialized(_driver, "columns", "2 10px / 100px", "10px 2 / 100px"));
    try$(_expectSerialized(_driver, "columns", "10px / 100px", "10px / 100px"));
    try$(_expectSerialized(_driver, "columns", "auto / 100px", "auto / 100px"));
    try$(_expectSerialized(_driver, "columns", "10px 2 / auto", "10px 2"));
    try$(_expectSerialized(_driver, "columns", "2 10px / auto", "10px 2"));
    try$(_expectSerialized(_driver, "columns", "10px / auto", "10px"));
    try$(_expectSerialized(_driver, "columns", "2 / auto", "2"));
    try$(_expectSerialized(_driver, "columns", "auto / auto", "auto"));
    return Ok();
}

test$("vaev-style-wpt-css-css-multicol-parsing-columns-valid-xfail", XFAIL) {
    try$(_expectSerialized(_driver, "columns", "initial", "initial", RegisteredPropertySet::TOP_LEVEL));
    try$(_expectSerialized(_driver, "columns", "inherit", "inherit", RegisteredPropertySet::TOP_LEVEL));
    return Ok();
}

} // namespace Vaev::Style::Tests
