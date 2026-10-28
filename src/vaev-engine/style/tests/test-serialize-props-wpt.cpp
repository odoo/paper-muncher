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

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-writing-modes/parsing/direction-invalid.html
test$("vaev-style-wpt-css-css-writing-modes-parsing-direction-invalid") {
    try$(_expectInvalid(_driver, "direction", "auto"));
    try$(_expectInvalid(_driver, "direction", "ltr rtl"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-writing-modes/parsing/direction-valid.html
test$("vaev-style-wpt-css-css-writing-modes-parsing-direction-valid") {
    try$(_expectSerialized(_driver, "direction", "ltr", "ltr"));
    try$(_expectSerialized(_driver, "direction", "rtl", "rtl"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-writing-modes/parsing/writing-mode-invalid.html
test$("vaev-style-wpt-css-css-writing-modes-parsing-writing-mode-invalid") {
    try$(_expectInvalid(_driver, "writing-mode", "auto"));
    try$(_expectInvalid(_driver, "writing-mode", "horizontal-tb vertical-rl"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-writing-modes/parsing/writing-mode-valid.html
test$("vaev-style-wpt-css-css-writing-modes-parsing-writing-mode-valid") {
    try$(_expectSerialized(_driver, "writing-mode", "horizontal-tb", "horizontal-tb"));
    try$(_expectSerialized(_driver, "writing-mode", "vertical-rl", "vertical-rl"));
    try$(_expectSerialized(_driver, "writing-mode", "vertical-lr", "vertical-lr"));
    return Ok();
}

} // namespace Vaev::Style::Tests
