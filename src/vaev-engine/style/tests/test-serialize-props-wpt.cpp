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

static Res<> _expectSerializedOneOf(Test::Driver& _driver, Str name, Str input, Slice<Str> expected) {
    static auto registry = defaultRegistry();
    auto prop = try$(registry.parseValue(Symbol::from(name), input, {}));
    Io::StringWriter w;
    try$(prop->serialize(w));
    auto actual = w.take();
    if (contains(expected, actual))
        return Ok();
    assertEq$(actual, expected[0]);
    return Ok();
}

static Res<> _expectInvalid(Test::Driver& _driver, Str name, Str input) {
    static auto registry = defaultRegistry();
    assert$(not registry.parseValue(Symbol::from(name), input, {}));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/CSS2/syntax/colors-007.html
test$("vaev-style-wpt-css-CSS2-syntax-colors-007") {
    try$(_expectSerialized(_driver, "color", "#000000", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "color", "#000001", "rgb(0, 0, 1)"));
    try$(_expectSerialized(_driver, "color", "#000099", "rgb(0, 0, 153)"));
    try$(_expectSerialized(_driver, "color", "#0000fe", "rgb(0, 0, 254)"));
    try$(_expectSerialized(_driver, "color", "#0000ff", "rgb(0, 0, 255)"));
    try$(_expectSerialized(_driver, "color", "#000100", "rgb(0, 1, 0)"));
    try$(_expectSerialized(_driver, "color", "#000", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "color", "#001", "rgb(0, 0, 17)"));
    try$(_expectSerialized(_driver, "color", "#009900", "rgb(0, 153, 0)"));
    try$(_expectSerialized(_driver, "color", "#009", "rgb(0, 0, 153)"));
    try$(_expectSerialized(_driver, "color", "#00e", "rgb(0, 0, 238)"));
    try$(_expectSerialized(_driver, "color", "#00f", "rgb(0, 0, 255)"));
    try$(_expectSerialized(_driver, "color", "#00fe00", "rgb(0, 254, 0)"));
    try$(_expectSerialized(_driver, "color", "#00ff00", "rgb(0, 255, 0)"));
    try$(_expectSerialized(_driver, "color", "#010000", "rgb(1, 0, 0)"));
    try$(_expectSerialized(_driver, "color", "#010101", "rgb(1, 1, 1)"));
    try$(_expectSerialized(_driver, "color", "#010", "rgb(0, 17, 0)"));
    try$(_expectSerialized(_driver, "color", "#090", "rgb(0, 153, 0)"));
    try$(_expectSerialized(_driver, "color", "#0e0", "rgb(0, 238, 0)"));
    try$(_expectSerialized(_driver, "color", "#0f0", "rgb(0, 255, 0)"));
    try$(_expectSerialized(_driver, "color", "#1000", "rgba(17, 0, 0, 0)"));
    try$(_expectSerialized(_driver, "color", "#100", "rgb(17, 0, 0)"));
    try$(_expectSerialized(_driver, "color", "#111", "rgb(17, 17, 17)"));
    try$(_expectSerialized(_driver, "color", "#900", "rgb(153, 0, 0)"));
    try$(_expectSerialized(_driver, "color", "#990000", "rgb(153, 0, 0)"));
    try$(_expectSerialized(_driver, "color", "#999999", "rgb(153, 153, 153)"));
    try$(_expectSerialized(_driver, "color", "#999", "rgb(153, 153, 153)"));
    try$(_expectSerialized(_driver, "color", "#e00", "rgb(238, 0, 0)"));
    try$(_expectSerialized(_driver, "color", "#eee", "rgb(238, 238, 238)"));
    try$(_expectSerialized(_driver, "color", "#f00", "rgb(255, 0, 0)"));
    try$(_expectSerialized(_driver, "color", "#fe0000", "rgb(254, 0, 0)"));
    try$(_expectSerialized(_driver, "color", "#fefefe", "rgb(254, 254, 254)"));
    try$(_expectSerialized(_driver, "color", "#ff0000", "rgb(255, 0, 0)"));
    try$(_expectSerialized(_driver, "color", "#fff", "rgb(255, 255, 255)"));
    try$(_expectSerialized(_driver, "color", "#ffffff", "rgb(255, 255, 255)"));
    try$(_expectSerialized(_driver, "color", "rgb(+0%, +0%, +0%)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "color", "rgb(+0%, 0%, 0%)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "color", "rgb(+0, +0, +0)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "color", "rgb(+0, 0, 0)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "color", "rgb(+100%, +100%, +100%)", "rgb(255, 255, 255)"));
    try$(_expectSerialized(_driver, "color", "rgb(+100%, 0%, 0%)", "rgb(255, 0, 0)"));
    try$(_expectSerialized(_driver, "color", "rgb(+128, +128, +128)", "rgb(128, 128, 128)"));
    try$(_expectSerialized(_driver, "color", "rgb(+128, 0, 0)", "rgb(128, 0, 0)"));
    try$(_expectSerialized(_driver, "color", "rgb(+255, +255, +255)", "rgb(255, 255, 255)"));
    try$(_expectSerialized(_driver, "color", "rgb(+255, 0, 0)", "rgb(255, 0, 0)"));
    try$(_expectSerialized(_driver, "color", "rgb(+40%, +40%, +40%)", "rgb(102, 102, 102)"));
    try$(_expectSerialized(_driver, "color", "rgb(+40%, 0%, 0%)", "rgb(102, 0, 0)"));
    try$(_expectSerialized(_driver, "color", "rgb(-0%, -0%, -0%)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "color", "rgb(-0%, 0%, 0%)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "color", "rgb(-0, -0, -0)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "color", "rgb(-0, 0, 0)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "color", "rgb(0%, +0%, 0%)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "color", "rgb(0%, +100%, 0%)", "rgb(0, 255, 0)"));
    try$(_expectSerialized(_driver, "color", "rgb(0%, +40%, 0%)", "rgb(0, 102, 0)"));
    try$(_expectSerialized(_driver, "color", "rgb(0%, -0%, 0%)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "color", "rgb(0%, 0%, +0%)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "color", "rgb(0%, 0%, +100%)", "rgb(0, 0, 255)"));
    try$(_expectSerialized(_driver, "color", "rgb(0%, 0%, +40%)", "rgb(0, 0, 102)"));
    try$(_expectSerialized(_driver, "color", "rgb(0%, 0%, -0%)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "color", "rgb(0%, 0%, 0%)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "color", "rgb(0%, 0%, 1%)", "rgb(0, 0, 3)"));
    try$(_expectSerialized(_driver, "color", "rgb(0%, 0%, 100%)", "rgb(0, 0, 255)"));
    try$(_expectSerialized(_driver, "color", "rgb(0%, 0%, 40%)", "rgb(0, 0, 102)"));
    try$(_expectSerialized(_driver, "color", "rgb(0%, 0%, 99%)", "rgb(0, 0, 252)"));
    try$(_expectSerialized(_driver, "color", "rgb(0%, 1%, 0%)", "rgb(0, 3, 0)"));
    try$(_expectSerialized(_driver, "color", "rgb(0%, 100%, 0%)", "rgb(0, 255, 0)"));
    try$(_expectSerialized(_driver, "color", "rgb(0%, 40%, 0%)", "rgb(0, 102, 0)"));
    try$(_expectSerialized(_driver, "color", "rgb(0%, 99%, 0%)", "rgb(0, 252, 0)"));
    try$(_expectSerialized(_driver, "color", "rgb(0, +0, 0)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "color", "rgb(0, +128, 0)", "rgb(0, 128, 0)"));
    try$(_expectSerialized(_driver, "color", "rgb(0, +255, 0)", "rgb(0, 255, 0)"));
    try$(_expectSerialized(_driver, "color", "rgb(0, -0, 0)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "color", "rgb(0, 0, +0)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "color", "rgb(0, 0, +128)", "rgb(0, 0, 128)"));
    try$(_expectSerialized(_driver, "color", "rgb(0, 0, +255)", "rgb(0, 0, 255)"));
    try$(_expectSerialized(_driver, "color", "rgb(0, 0, -0)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "color", "rgb(0, 0, 0)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "color", "rgb(0, 0, 1)", "rgb(0, 0, 1)"));
    try$(_expectSerialized(_driver, "color", "rgb(0, 0, 128)", "rgb(0, 0, 128)"));
    try$(_expectSerialized(_driver, "color", "rgb(0, 0, 254)", "rgb(0, 0, 254)"));
    try$(_expectSerialized(_driver, "color", "rgb(0, 0, 255)", "rgb(0, 0, 255)"));
    try$(_expectSerialized(_driver, "color", "rgb(0, 1, 0)", "rgb(0, 1, 0)"));
    try$(_expectSerialized(_driver, "color", "rgb(0, 128, 0)", "rgb(0, 128, 0)"));
    try$(_expectSerialized(_driver, "color", "rgb(0, 254, 0)", "rgb(0, 254, 0)"));
    try$(_expectSerialized(_driver, "color", "rgb(0, 255, 0)", "rgb(0, 255, 0)"));
    try$(_expectSerialized(_driver, "color", "rgb(1%, 0%, 0%)", "rgb(3, 0, 0)"));
    try$(_expectSerialized(_driver, "color", "rgb(1%, 1%, 1%)", "rgb(3, 3, 3)"));
    try$(_expectSerialized(_driver, "color", "rgb(1, 0, 0)", "rgb(1, 0, 0)"));
    try$(_expectSerialized(_driver, "color", "rgb(1, 1, 1)", "rgb(1, 1, 1)"));
    try$(_expectSerialized(_driver, "color", "rgb(100%, 0%, 0%)", "rgb(255, 0, 0)"));
    try$(_expectSerialized(_driver, "color", "rgb(100%, 100%, 100%)", "rgb(255, 255, 255)"));
    try$(_expectSerialized(_driver, "color", "rgb(128, 0, 0)", "rgb(128, 0, 0)"));
    try$(_expectSerialized(_driver, "color", "rgb(128, 128, 128)", "rgb(128, 128, 128)"));
    try$(_expectSerialized(_driver, "color", "rgb(254, 0, 0)", "rgb(254, 0, 0)"));
    try$(_expectSerialized(_driver, "color", "rgb(254, 254, 254)", "rgb(254, 254, 254)"));
    try$(_expectSerialized(_driver, "color", "rgb(255, 0, 0)", "rgb(255, 0, 0)"));
    try$(_expectSerialized(_driver, "color", "rgb(255, 255, 255)", "rgb(255, 255, 255)"));
    try$(_expectSerialized(_driver, "color", "rgb(40%, 0%, 0%)", "rgb(102, 0, 0)"));
    try$(_expectSerialized(_driver, "color", "rgb(40%, 40%, 40%)", "rgb(102, 102, 102)"));
    try$(_expectSerialized(_driver, "color", "rgb(99%, 0%, 0%)", "rgb(252, 0, 0)"));
    try$(_expectSerialized(_driver, "color", "rgb(99%, 99%, 99%)", "rgb(252, 252, 252)"));
    try$(_expectSerialized(_driver, "color", "rgb(0.0%, 100.0%, 0.0%)", "rgb(0, 255, 0)"));
    try$(_expectSerialized(_driver, "color", "inherit", "inherit", RegisteredPropertySet::TOP_LEVEL));
    try$(_expectInvalid(_driver, "color", "#00000"));
    try$(_expectInvalid(_driver, "color", "#00"));
    try$(_expectInvalid(_driver, "color", "#1000000"));
    try$(_expectInvalid(_driver, "color", "invalidValue"));
    try$(_expectSerialized(_driver, "background-color", "#000000", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "background-color", "#000001", "rgb(0, 0, 1)"));
    try$(_expectSerialized(_driver, "background-color", "#000099", "rgb(0, 0, 153)"));
    try$(_expectSerialized(_driver, "background-color", "#0000fe", "rgb(0, 0, 254)"));
    try$(_expectSerialized(_driver, "background-color", "#0000ff", "rgb(0, 0, 255)"));
    try$(_expectSerialized(_driver, "background-color", "#000100", "rgb(0, 1, 0)"));
    try$(_expectSerialized(_driver, "background-color", "#000", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "background-color", "#001", "rgb(0, 0, 17)"));
    try$(_expectSerialized(_driver, "background-color", "#009900", "rgb(0, 153, 0)"));
    try$(_expectSerialized(_driver, "background-color", "#009", "rgb(0, 0, 153)"));
    try$(_expectSerialized(_driver, "background-color", "#00e", "rgb(0, 0, 238)"));
    try$(_expectSerialized(_driver, "background-color", "#00f", "rgb(0, 0, 255)"));
    try$(_expectSerialized(_driver, "background-color", "#00fe00", "rgb(0, 254, 0)"));
    try$(_expectSerialized(_driver, "background-color", "#00ff00", "rgb(0, 255, 0)"));
    try$(_expectSerialized(_driver, "background-color", "#010000", "rgb(1, 0, 0)"));
    try$(_expectSerialized(_driver, "background-color", "#010101", "rgb(1, 1, 1)"));
    try$(_expectSerialized(_driver, "background-color", "#010", "rgb(0, 17, 0)"));
    try$(_expectSerialized(_driver, "background-color", "#090", "rgb(0, 153, 0)"));
    try$(_expectSerialized(_driver, "background-color", "#0e0", "rgb(0, 238, 0)"));
    try$(_expectSerialized(_driver, "background-color", "#0f0", "rgb(0, 255, 0)"));
    try$(_expectSerialized(_driver, "background-color", "#1000", "rgba(17, 0, 0, 0)"));
    try$(_expectSerialized(_driver, "background-color", "#100", "rgb(17, 0, 0)"));
    try$(_expectSerialized(_driver, "background-color", "#111", "rgb(17, 17, 17)"));
    try$(_expectSerialized(_driver, "background-color", "#900", "rgb(153, 0, 0)"));
    try$(_expectSerialized(_driver, "background-color", "#990000", "rgb(153, 0, 0)"));
    try$(_expectSerialized(_driver, "background-color", "#999999", "rgb(153, 153, 153)"));
    try$(_expectSerialized(_driver, "background-color", "#999", "rgb(153, 153, 153)"));
    try$(_expectSerialized(_driver, "background-color", "#e00", "rgb(238, 0, 0)"));
    try$(_expectSerialized(_driver, "background-color", "#eee", "rgb(238, 238, 238)"));
    try$(_expectSerialized(_driver, "background-color", "#f00", "rgb(255, 0, 0)"));
    try$(_expectSerialized(_driver, "background-color", "#fe0000", "rgb(254, 0, 0)"));
    try$(_expectSerialized(_driver, "background-color", "#fefefe", "rgb(254, 254, 254)"));
    try$(_expectSerialized(_driver, "background-color", "#ff0000", "rgb(255, 0, 0)"));
    try$(_expectSerialized(_driver, "background-color", "#fff", "rgb(255, 255, 255)"));
    try$(_expectSerialized(_driver, "background-color", "#ffffff", "rgb(255, 255, 255)"));
    try$(_expectSerialized(_driver, "background-color", "rgb(+0%, +0%, +0%)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "background-color", "rgb(+0%, 0%, 0%)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "background-color", "rgb(+0, +0, +0)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "background-color", "rgb(+0, 0, 0)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "background-color", "rgb(+100%, +100%, +100%)", "rgb(255, 255, 255)"));
    try$(_expectSerialized(_driver, "background-color", "rgb(+100%, 0%, 0%)", "rgb(255, 0, 0)"));
    try$(_expectSerialized(_driver, "background-color", "rgb(+128, +128, +128)", "rgb(128, 128, 128)"));
    try$(_expectSerialized(_driver, "background-color", "rgb(+128, 0, 0)", "rgb(128, 0, 0)"));
    try$(_expectSerialized(_driver, "background-color", "rgb(+255, +255, +255)", "rgb(255, 255, 255)"));
    try$(_expectSerialized(_driver, "background-color", "rgb(+255, 0, 0)", "rgb(255, 0, 0)"));
    try$(_expectSerialized(_driver, "background-color", "rgb(+40%, +40%, +40%)", "rgb(102, 102, 102)"));
    try$(_expectSerialized(_driver, "background-color", "rgb(+40%, 0%, 0%)", "rgb(102, 0, 0)"));
    try$(_expectSerialized(_driver, "background-color", "rgb(-0%, -0%, -0%)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "background-color", "rgb(-0%, 0%, 0%)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "background-color", "rgb(-0, -0, -0)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "background-color", "rgb(-0, 0, 0)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "background-color", "rgb(0%, +0%, 0%)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "background-color", "rgb(0%, +100%, 0%)", "rgb(0, 255, 0)"));
    try$(_expectSerialized(_driver, "background-color", "rgb(0%, +40%, 0%)", "rgb(0, 102, 0)"));
    try$(_expectSerialized(_driver, "background-color", "rgb(0%, -0%, 0%)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "background-color", "rgb(0%, 0%, +0%)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "background-color", "rgb(0%, 0%, +100%)", "rgb(0, 0, 255)"));
    try$(_expectSerialized(_driver, "background-color", "rgb(0%, 0%, +40%)", "rgb(0, 0, 102)"));
    try$(_expectSerialized(_driver, "background-color", "rgb(0%, 0%, -0%)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "background-color", "rgb(0%, 0%, 0%)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "background-color", "rgb(0%, 0%, 1%)", "rgb(0, 0, 3)"));
    try$(_expectSerialized(_driver, "background-color", "rgb(0%, 0%, 100%)", "rgb(0, 0, 255)"));
    try$(_expectSerialized(_driver, "background-color", "rgb(0%, 0%, 40%)", "rgb(0, 0, 102)"));
    try$(_expectSerialized(_driver, "background-color", "rgb(0%, 0%, 99%)", "rgb(0, 0, 252)"));
    try$(_expectSerialized(_driver, "background-color", "rgb(0%, 1%, 0%)", "rgb(0, 3, 0)"));
    try$(_expectSerialized(_driver, "background-color", "rgb(0%, 100%, 0%)", "rgb(0, 255, 0)"));
    try$(_expectSerialized(_driver, "background-color", "rgb(0%, 40%, 0%)", "rgb(0, 102, 0)"));
    try$(_expectSerialized(_driver, "background-color", "rgb(0%, 99%, 0%)", "rgb(0, 252, 0)"));
    try$(_expectSerialized(_driver, "background-color", "rgb(0, +0, 0)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "background-color", "rgb(0, +128, 0)", "rgb(0, 128, 0)"));
    try$(_expectSerialized(_driver, "background-color", "rgb(0, +255, 0)", "rgb(0, 255, 0)"));
    try$(_expectSerialized(_driver, "background-color", "rgb(0, -0, 0)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "background-color", "rgb(0, 0, +0)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "background-color", "rgb(0, 0, +128)", "rgb(0, 0, 128)"));
    try$(_expectSerialized(_driver, "background-color", "rgb(0, 0, +255)", "rgb(0, 0, 255)"));
    try$(_expectSerialized(_driver, "background-color", "rgb(0, 0, -0)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "background-color", "rgb(0, 0, 0)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "background-color", "rgb(0, 0, 1)", "rgb(0, 0, 1)"));
    try$(_expectSerialized(_driver, "background-color", "rgb(0, 0, 128)", "rgb(0, 0, 128)"));
    try$(_expectSerialized(_driver, "background-color", "rgb(0, 0, 254)", "rgb(0, 0, 254)"));
    try$(_expectSerialized(_driver, "background-color", "rgb(0, 0, 255)", "rgb(0, 0, 255)"));
    try$(_expectSerialized(_driver, "background-color", "rgb(0, 1, 0)", "rgb(0, 1, 0)"));
    try$(_expectSerialized(_driver, "background-color", "rgb(0, 128, 0)", "rgb(0, 128, 0)"));
    try$(_expectSerialized(_driver, "background-color", "rgb(0, 254, 0)", "rgb(0, 254, 0)"));
    try$(_expectSerialized(_driver, "background-color", "rgb(0, 255, 0)", "rgb(0, 255, 0)"));
    try$(_expectSerialized(_driver, "background-color", "rgb(1%, 0%, 0%)", "rgb(3, 0, 0)"));
    try$(_expectSerialized(_driver, "background-color", "rgb(1%, 1%, 1%)", "rgb(3, 3, 3)"));
    try$(_expectSerialized(_driver, "background-color", "rgb(1, 0, 0)", "rgb(1, 0, 0)"));
    try$(_expectSerialized(_driver, "background-color", "rgb(1, 1, 1)", "rgb(1, 1, 1)"));
    try$(_expectSerialized(_driver, "background-color", "rgb(100%, 0%, 0%)", "rgb(255, 0, 0)"));
    try$(_expectSerialized(_driver, "background-color", "rgb(100%, 100%, 100%)", "rgb(255, 255, 255)"));
    try$(_expectSerialized(_driver, "background-color", "rgb(128, 0, 0)", "rgb(128, 0, 0)"));
    try$(_expectSerialized(_driver, "background-color", "rgb(128, 128, 128)", "rgb(128, 128, 128)"));
    try$(_expectSerialized(_driver, "background-color", "rgb(254, 0, 0)", "rgb(254, 0, 0)"));
    try$(_expectSerialized(_driver, "background-color", "rgb(254, 254, 254)", "rgb(254, 254, 254)"));
    try$(_expectSerialized(_driver, "background-color", "rgb(255, 0, 0)", "rgb(255, 0, 0)"));
    try$(_expectSerialized(_driver, "background-color", "rgb(255, 255, 255)", "rgb(255, 255, 255)"));
    try$(_expectSerialized(_driver, "background-color", "rgb(40%, 0%, 0%)", "rgb(102, 0, 0)"));
    try$(_expectSerialized(_driver, "background-color", "rgb(40%, 40%, 40%)", "rgb(102, 102, 102)"));
    try$(_expectSerialized(_driver, "background-color", "rgb(99%, 0%, 0%)", "rgb(252, 0, 0)"));
    try$(_expectSerialized(_driver, "background-color", "rgb(99%, 99%, 99%)", "rgb(252, 252, 252)"));
    try$(_expectSerialized(_driver, "background-color", "rgb(0.0%, 100.0%, 0.0%)", "rgb(0, 255, 0)"));
    try$(_expectSerialized(_driver, "background-color", "inherit", "inherit", RegisteredPropertySet::TOP_LEVEL));
    try$(_expectInvalid(_driver, "background-color", "#00000"));
    try$(_expectInvalid(_driver, "background-color", "#00"));
    try$(_expectInvalid(_driver, "background-color", "#1000000"));
    try$(_expectInvalid(_driver, "background-color", "invalidValue"));
    try$(_expectSerialized(_driver, "border-top-color", "#000000", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "border-top-color", "#000001", "rgb(0, 0, 1)"));
    try$(_expectSerialized(_driver, "border-top-color", "#000099", "rgb(0, 0, 153)"));
    try$(_expectSerialized(_driver, "border-top-color", "#0000fe", "rgb(0, 0, 254)"));
    try$(_expectSerialized(_driver, "border-top-color", "#0000ff", "rgb(0, 0, 255)"));
    try$(_expectSerialized(_driver, "border-top-color", "#000100", "rgb(0, 1, 0)"));
    try$(_expectSerialized(_driver, "border-top-color", "#000", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "border-top-color", "#001", "rgb(0, 0, 17)"));
    try$(_expectSerialized(_driver, "border-top-color", "#009900", "rgb(0, 153, 0)"));
    try$(_expectSerialized(_driver, "border-top-color", "#009", "rgb(0, 0, 153)"));
    try$(_expectSerialized(_driver, "border-top-color", "#00e", "rgb(0, 0, 238)"));
    try$(_expectSerialized(_driver, "border-top-color", "#00f", "rgb(0, 0, 255)"));
    try$(_expectSerialized(_driver, "border-top-color", "#00fe00", "rgb(0, 254, 0)"));
    try$(_expectSerialized(_driver, "border-top-color", "#00ff00", "rgb(0, 255, 0)"));
    try$(_expectSerialized(_driver, "border-top-color", "#010000", "rgb(1, 0, 0)"));
    try$(_expectSerialized(_driver, "border-top-color", "#010101", "rgb(1, 1, 1)"));
    try$(_expectSerialized(_driver, "border-top-color", "#010", "rgb(0, 17, 0)"));
    try$(_expectSerialized(_driver, "border-top-color", "#090", "rgb(0, 153, 0)"));
    try$(_expectSerialized(_driver, "border-top-color", "#0e0", "rgb(0, 238, 0)"));
    try$(_expectSerialized(_driver, "border-top-color", "#0f0", "rgb(0, 255, 0)"));
    try$(_expectSerialized(_driver, "border-top-color", "#1000", "rgba(17, 0, 0, 0)"));
    try$(_expectSerialized(_driver, "border-top-color", "#100", "rgb(17, 0, 0)"));
    try$(_expectSerialized(_driver, "border-top-color", "#111", "rgb(17, 17, 17)"));
    try$(_expectSerialized(_driver, "border-top-color", "#900", "rgb(153, 0, 0)"));
    try$(_expectSerialized(_driver, "border-top-color", "#990000", "rgb(153, 0, 0)"));
    try$(_expectSerialized(_driver, "border-top-color", "#999999", "rgb(153, 153, 153)"));
    try$(_expectSerialized(_driver, "border-top-color", "#999", "rgb(153, 153, 153)"));
    try$(_expectSerialized(_driver, "border-top-color", "#e00", "rgb(238, 0, 0)"));
    try$(_expectSerialized(_driver, "border-top-color", "#eee", "rgb(238, 238, 238)"));
    try$(_expectSerialized(_driver, "border-top-color", "#f00", "rgb(255, 0, 0)"));
    try$(_expectSerialized(_driver, "border-top-color", "#fe0000", "rgb(254, 0, 0)"));
    try$(_expectSerialized(_driver, "border-top-color", "#fefefe", "rgb(254, 254, 254)"));
    try$(_expectSerialized(_driver, "border-top-color", "#ff0000", "rgb(255, 0, 0)"));
    try$(_expectSerialized(_driver, "border-top-color", "#fff", "rgb(255, 255, 255)"));
    try$(_expectSerialized(_driver, "border-top-color", "#ffffff", "rgb(255, 255, 255)"));
    try$(_expectSerialized(_driver, "border-top-color", "rgb(+0%, +0%, +0%)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "border-top-color", "rgb(+0%, 0%, 0%)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "border-top-color", "rgb(+0, +0, +0)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "border-top-color", "rgb(+0, 0, 0)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "border-top-color", "rgb(+100%, +100%, +100%)", "rgb(255, 255, 255)"));
    try$(_expectSerialized(_driver, "border-top-color", "rgb(+100%, 0%, 0%)", "rgb(255, 0, 0)"));
    try$(_expectSerialized(_driver, "border-top-color", "rgb(+128, +128, +128)", "rgb(128, 128, 128)"));
    try$(_expectSerialized(_driver, "border-top-color", "rgb(+128, 0, 0)", "rgb(128, 0, 0)"));
    try$(_expectSerialized(_driver, "border-top-color", "rgb(+255, +255, +255)", "rgb(255, 255, 255)"));
    try$(_expectSerialized(_driver, "border-top-color", "rgb(+255, 0, 0)", "rgb(255, 0, 0)"));
    try$(_expectSerialized(_driver, "border-top-color", "rgb(+40%, +40%, +40%)", "rgb(102, 102, 102)"));
    try$(_expectSerialized(_driver, "border-top-color", "rgb(+40%, 0%, 0%)", "rgb(102, 0, 0)"));
    try$(_expectSerialized(_driver, "border-top-color", "rgb(-0%, -0%, -0%)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "border-top-color", "rgb(-0%, 0%, 0%)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "border-top-color", "rgb(-0, -0, -0)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "border-top-color", "rgb(-0, 0, 0)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "border-top-color", "rgb(0%, +0%, 0%)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "border-top-color", "rgb(0%, +100%, 0%)", "rgb(0, 255, 0)"));
    try$(_expectSerialized(_driver, "border-top-color", "rgb(0%, +40%, 0%)", "rgb(0, 102, 0)"));
    try$(_expectSerialized(_driver, "border-top-color", "rgb(0%, -0%, 0%)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "border-top-color", "rgb(0%, 0%, +0%)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "border-top-color", "rgb(0%, 0%, +100%)", "rgb(0, 0, 255)"));
    try$(_expectSerialized(_driver, "border-top-color", "rgb(0%, 0%, +40%)", "rgb(0, 0, 102)"));
    try$(_expectSerialized(_driver, "border-top-color", "rgb(0%, 0%, -0%)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "border-top-color", "rgb(0%, 0%, 0%)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "border-top-color", "rgb(0%, 0%, 1%)", "rgb(0, 0, 3)"));
    try$(_expectSerialized(_driver, "border-top-color", "rgb(0%, 0%, 100%)", "rgb(0, 0, 255)"));
    try$(_expectSerialized(_driver, "border-top-color", "rgb(0%, 0%, 40%)", "rgb(0, 0, 102)"));
    try$(_expectSerialized(_driver, "border-top-color", "rgb(0%, 0%, 99%)", "rgb(0, 0, 252)"));
    try$(_expectSerialized(_driver, "border-top-color", "rgb(0%, 1%, 0%)", "rgb(0, 3, 0)"));
    try$(_expectSerialized(_driver, "border-top-color", "rgb(0%, 100%, 0%)", "rgb(0, 255, 0)"));
    try$(_expectSerialized(_driver, "border-top-color", "rgb(0%, 40%, 0%)", "rgb(0, 102, 0)"));
    try$(_expectSerialized(_driver, "border-top-color", "rgb(0%, 99%, 0%)", "rgb(0, 252, 0)"));
    try$(_expectSerialized(_driver, "border-top-color", "rgb(0, +0, 0)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "border-top-color", "rgb(0, +128, 0)", "rgb(0, 128, 0)"));
    try$(_expectSerialized(_driver, "border-top-color", "rgb(0, +255, 0)", "rgb(0, 255, 0)"));
    try$(_expectSerialized(_driver, "border-top-color", "rgb(0, -0, 0)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "border-top-color", "rgb(0, 0, +0)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "border-top-color", "rgb(0, 0, +128)", "rgb(0, 0, 128)"));
    try$(_expectSerialized(_driver, "border-top-color", "rgb(0, 0, +255)", "rgb(0, 0, 255)"));
    try$(_expectSerialized(_driver, "border-top-color", "rgb(0, 0, -0)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "border-top-color", "rgb(0, 0, 0)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "border-top-color", "rgb(0, 0, 1)", "rgb(0, 0, 1)"));
    try$(_expectSerialized(_driver, "border-top-color", "rgb(0, 0, 128)", "rgb(0, 0, 128)"));
    try$(_expectSerialized(_driver, "border-top-color", "rgb(0, 0, 254)", "rgb(0, 0, 254)"));
    try$(_expectSerialized(_driver, "border-top-color", "rgb(0, 0, 255)", "rgb(0, 0, 255)"));
    try$(_expectSerialized(_driver, "border-top-color", "rgb(0, 1, 0)", "rgb(0, 1, 0)"));
    try$(_expectSerialized(_driver, "border-top-color", "rgb(0, 128, 0)", "rgb(0, 128, 0)"));
    try$(_expectSerialized(_driver, "border-top-color", "rgb(0, 254, 0)", "rgb(0, 254, 0)"));
    try$(_expectSerialized(_driver, "border-top-color", "rgb(0, 255, 0)", "rgb(0, 255, 0)"));
    try$(_expectSerialized(_driver, "border-top-color", "rgb(1%, 0%, 0%)", "rgb(3, 0, 0)"));
    try$(_expectSerialized(_driver, "border-top-color", "rgb(1%, 1%, 1%)", "rgb(3, 3, 3)"));
    try$(_expectSerialized(_driver, "border-top-color", "rgb(1, 0, 0)", "rgb(1, 0, 0)"));
    try$(_expectSerialized(_driver, "border-top-color", "rgb(1, 1, 1)", "rgb(1, 1, 1)"));
    try$(_expectSerialized(_driver, "border-top-color", "rgb(100%, 0%, 0%)", "rgb(255, 0, 0)"));
    try$(_expectSerialized(_driver, "border-top-color", "rgb(100%, 100%, 100%)", "rgb(255, 255, 255)"));
    try$(_expectSerialized(_driver, "border-top-color", "rgb(128, 0, 0)", "rgb(128, 0, 0)"));
    try$(_expectSerialized(_driver, "border-top-color", "rgb(128, 128, 128)", "rgb(128, 128, 128)"));
    try$(_expectSerialized(_driver, "border-top-color", "rgb(254, 0, 0)", "rgb(254, 0, 0)"));
    try$(_expectSerialized(_driver, "border-top-color", "rgb(254, 254, 254)", "rgb(254, 254, 254)"));
    try$(_expectSerialized(_driver, "border-top-color", "rgb(255, 0, 0)", "rgb(255, 0, 0)"));
    try$(_expectSerialized(_driver, "border-top-color", "rgb(255, 255, 255)", "rgb(255, 255, 255)"));
    try$(_expectSerialized(_driver, "border-top-color", "rgb(40%, 0%, 0%)", "rgb(102, 0, 0)"));
    try$(_expectSerialized(_driver, "border-top-color", "rgb(40%, 40%, 40%)", "rgb(102, 102, 102)"));
    try$(_expectSerialized(_driver, "border-top-color", "rgb(99%, 0%, 0%)", "rgb(252, 0, 0)"));
    try$(_expectSerialized(_driver, "border-top-color", "rgb(99%, 99%, 99%)", "rgb(252, 252, 252)"));
    try$(_expectSerialized(_driver, "border-top-color", "rgb(0.0%, 100.0%, 0.0%)", "rgb(0, 255, 0)"));
    try$(_expectSerialized(_driver, "border-top-color", "inherit", "inherit", RegisteredPropertySet::TOP_LEVEL));
    try$(_expectInvalid(_driver, "border-top-color", "#00000"));
    try$(_expectInvalid(_driver, "border-top-color", "#00"));
    try$(_expectInvalid(_driver, "border-top-color", "#1000000"));
    try$(_expectInvalid(_driver, "border-top-color", "invalidValue"));
    try$(_expectSerialized(_driver, "border-bottom-color", "#000000", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "border-bottom-color", "#000001", "rgb(0, 0, 1)"));
    try$(_expectSerialized(_driver, "border-bottom-color", "#000099", "rgb(0, 0, 153)"));
    try$(_expectSerialized(_driver, "border-bottom-color", "#0000fe", "rgb(0, 0, 254)"));
    try$(_expectSerialized(_driver, "border-bottom-color", "#0000ff", "rgb(0, 0, 255)"));
    try$(_expectSerialized(_driver, "border-bottom-color", "#000100", "rgb(0, 1, 0)"));
    try$(_expectSerialized(_driver, "border-bottom-color", "#000", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "border-bottom-color", "#001", "rgb(0, 0, 17)"));
    try$(_expectSerialized(_driver, "border-bottom-color", "#009900", "rgb(0, 153, 0)"));
    try$(_expectSerialized(_driver, "border-bottom-color", "#009", "rgb(0, 0, 153)"));
    try$(_expectSerialized(_driver, "border-bottom-color", "#00e", "rgb(0, 0, 238)"));
    try$(_expectSerialized(_driver, "border-bottom-color", "#00f", "rgb(0, 0, 255)"));
    try$(_expectSerialized(_driver, "border-bottom-color", "#00fe00", "rgb(0, 254, 0)"));
    try$(_expectSerialized(_driver, "border-bottom-color", "#00ff00", "rgb(0, 255, 0)"));
    try$(_expectSerialized(_driver, "border-bottom-color", "#010000", "rgb(1, 0, 0)"));
    try$(_expectSerialized(_driver, "border-bottom-color", "#010101", "rgb(1, 1, 1)"));
    try$(_expectSerialized(_driver, "border-bottom-color", "#010", "rgb(0, 17, 0)"));
    try$(_expectSerialized(_driver, "border-bottom-color", "#090", "rgb(0, 153, 0)"));
    try$(_expectSerialized(_driver, "border-bottom-color", "#0e0", "rgb(0, 238, 0)"));
    try$(_expectSerialized(_driver, "border-bottom-color", "#0f0", "rgb(0, 255, 0)"));
    try$(_expectSerialized(_driver, "border-bottom-color", "#1000", "rgba(17, 0, 0, 0)"));
    try$(_expectSerialized(_driver, "border-bottom-color", "#100", "rgb(17, 0, 0)"));
    try$(_expectSerialized(_driver, "border-bottom-color", "#111", "rgb(17, 17, 17)"));
    try$(_expectSerialized(_driver, "border-bottom-color", "#900", "rgb(153, 0, 0)"));
    try$(_expectSerialized(_driver, "border-bottom-color", "#990000", "rgb(153, 0, 0)"));
    try$(_expectSerialized(_driver, "border-bottom-color", "#999999", "rgb(153, 153, 153)"));
    try$(_expectSerialized(_driver, "border-bottom-color", "#999", "rgb(153, 153, 153)"));
    try$(_expectSerialized(_driver, "border-bottom-color", "#e00", "rgb(238, 0, 0)"));
    try$(_expectSerialized(_driver, "border-bottom-color", "#eee", "rgb(238, 238, 238)"));
    try$(_expectSerialized(_driver, "border-bottom-color", "#f00", "rgb(255, 0, 0)"));
    try$(_expectSerialized(_driver, "border-bottom-color", "#fe0000", "rgb(254, 0, 0)"));
    try$(_expectSerialized(_driver, "border-bottom-color", "#fefefe", "rgb(254, 254, 254)"));
    try$(_expectSerialized(_driver, "border-bottom-color", "#ff0000", "rgb(255, 0, 0)"));
    try$(_expectSerialized(_driver, "border-bottom-color", "#fff", "rgb(255, 255, 255)"));
    try$(_expectSerialized(_driver, "border-bottom-color", "#ffffff", "rgb(255, 255, 255)"));
    try$(_expectSerialized(_driver, "border-bottom-color", "rgb(+0%, +0%, +0%)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "border-bottom-color", "rgb(+0%, 0%, 0%)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "border-bottom-color", "rgb(+0, +0, +0)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "border-bottom-color", "rgb(+0, 0, 0)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "border-bottom-color", "rgb(+100%, +100%, +100%)", "rgb(255, 255, 255)"));
    try$(_expectSerialized(_driver, "border-bottom-color", "rgb(+100%, 0%, 0%)", "rgb(255, 0, 0)"));
    try$(_expectSerialized(_driver, "border-bottom-color", "rgb(+128, +128, +128)", "rgb(128, 128, 128)"));
    try$(_expectSerialized(_driver, "border-bottom-color", "rgb(+128, 0, 0)", "rgb(128, 0, 0)"));
    try$(_expectSerialized(_driver, "border-bottom-color", "rgb(+255, +255, +255)", "rgb(255, 255, 255)"));
    try$(_expectSerialized(_driver, "border-bottom-color", "rgb(+255, 0, 0)", "rgb(255, 0, 0)"));
    try$(_expectSerialized(_driver, "border-bottom-color", "rgb(+40%, +40%, +40%)", "rgb(102, 102, 102)"));
    try$(_expectSerialized(_driver, "border-bottom-color", "rgb(+40%, 0%, 0%)", "rgb(102, 0, 0)"));
    try$(_expectSerialized(_driver, "border-bottom-color", "rgb(-0%, -0%, -0%)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "border-bottom-color", "rgb(-0%, 0%, 0%)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "border-bottom-color", "rgb(-0, -0, -0)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "border-bottom-color", "rgb(-0, 0, 0)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "border-bottom-color", "rgb(0%, +0%, 0%)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "border-bottom-color", "rgb(0%, +100%, 0%)", "rgb(0, 255, 0)"));
    try$(_expectSerialized(_driver, "border-bottom-color", "rgb(0%, +40%, 0%)", "rgb(0, 102, 0)"));
    try$(_expectSerialized(_driver, "border-bottom-color", "rgb(0%, -0%, 0%)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "border-bottom-color", "rgb(0%, 0%, +0%)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "border-bottom-color", "rgb(0%, 0%, +100%)", "rgb(0, 0, 255)"));
    try$(_expectSerialized(_driver, "border-bottom-color", "rgb(0%, 0%, +40%)", "rgb(0, 0, 102)"));
    try$(_expectSerialized(_driver, "border-bottom-color", "rgb(0%, 0%, -0%)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "border-bottom-color", "rgb(0%, 0%, 0%)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "border-bottom-color", "rgb(0%, 0%, 1%)", "rgb(0, 0, 3)"));
    try$(_expectSerialized(_driver, "border-bottom-color", "rgb(0%, 0%, 100%)", "rgb(0, 0, 255)"));
    try$(_expectSerialized(_driver, "border-bottom-color", "rgb(0%, 0%, 40%)", "rgb(0, 0, 102)"));
    try$(_expectSerialized(_driver, "border-bottom-color", "rgb(0%, 0%, 99%)", "rgb(0, 0, 252)"));
    try$(_expectSerialized(_driver, "border-bottom-color", "rgb(0%, 1%, 0%)", "rgb(0, 3, 0)"));
    try$(_expectSerialized(_driver, "border-bottom-color", "rgb(0%, 100%, 0%)", "rgb(0, 255, 0)"));
    try$(_expectSerialized(_driver, "border-bottom-color", "rgb(0%, 40%, 0%)", "rgb(0, 102, 0)"));
    try$(_expectSerialized(_driver, "border-bottom-color", "rgb(0%, 99%, 0%)", "rgb(0, 252, 0)"));
    try$(_expectSerialized(_driver, "border-bottom-color", "rgb(0, +0, 0)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "border-bottom-color", "rgb(0, +128, 0)", "rgb(0, 128, 0)"));
    try$(_expectSerialized(_driver, "border-bottom-color", "rgb(0, +255, 0)", "rgb(0, 255, 0)"));
    try$(_expectSerialized(_driver, "border-bottom-color", "rgb(0, -0, 0)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "border-bottom-color", "rgb(0, 0, +0)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "border-bottom-color", "rgb(0, 0, +128)", "rgb(0, 0, 128)"));
    try$(_expectSerialized(_driver, "border-bottom-color", "rgb(0, 0, +255)", "rgb(0, 0, 255)"));
    try$(_expectSerialized(_driver, "border-bottom-color", "rgb(0, 0, -0)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "border-bottom-color", "rgb(0, 0, 0)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "border-bottom-color", "rgb(0, 0, 1)", "rgb(0, 0, 1)"));
    try$(_expectSerialized(_driver, "border-bottom-color", "rgb(0, 0, 128)", "rgb(0, 0, 128)"));
    try$(_expectSerialized(_driver, "border-bottom-color", "rgb(0, 0, 254)", "rgb(0, 0, 254)"));
    try$(_expectSerialized(_driver, "border-bottom-color", "rgb(0, 0, 255)", "rgb(0, 0, 255)"));
    try$(_expectSerialized(_driver, "border-bottom-color", "rgb(0, 1, 0)", "rgb(0, 1, 0)"));
    try$(_expectSerialized(_driver, "border-bottom-color", "rgb(0, 128, 0)", "rgb(0, 128, 0)"));
    try$(_expectSerialized(_driver, "border-bottom-color", "rgb(0, 254, 0)", "rgb(0, 254, 0)"));
    try$(_expectSerialized(_driver, "border-bottom-color", "rgb(0, 255, 0)", "rgb(0, 255, 0)"));
    try$(_expectSerialized(_driver, "border-bottom-color", "rgb(1%, 0%, 0%)", "rgb(3, 0, 0)"));
    try$(_expectSerialized(_driver, "border-bottom-color", "rgb(1%, 1%, 1%)", "rgb(3, 3, 3)"));
    try$(_expectSerialized(_driver, "border-bottom-color", "rgb(1, 0, 0)", "rgb(1, 0, 0)"));
    try$(_expectSerialized(_driver, "border-bottom-color", "rgb(1, 1, 1)", "rgb(1, 1, 1)"));
    try$(_expectSerialized(_driver, "border-bottom-color", "rgb(100%, 0%, 0%)", "rgb(255, 0, 0)"));
    try$(_expectSerialized(_driver, "border-bottom-color", "rgb(100%, 100%, 100%)", "rgb(255, 255, 255)"));
    try$(_expectSerialized(_driver, "border-bottom-color", "rgb(128, 0, 0)", "rgb(128, 0, 0)"));
    try$(_expectSerialized(_driver, "border-bottom-color", "rgb(128, 128, 128)", "rgb(128, 128, 128)"));
    try$(_expectSerialized(_driver, "border-bottom-color", "rgb(254, 0, 0)", "rgb(254, 0, 0)"));
    try$(_expectSerialized(_driver, "border-bottom-color", "rgb(254, 254, 254)", "rgb(254, 254, 254)"));
    try$(_expectSerialized(_driver, "border-bottom-color", "rgb(255, 0, 0)", "rgb(255, 0, 0)"));
    try$(_expectSerialized(_driver, "border-bottom-color", "rgb(255, 255, 255)", "rgb(255, 255, 255)"));
    try$(_expectSerialized(_driver, "border-bottom-color", "rgb(40%, 0%, 0%)", "rgb(102, 0, 0)"));
    try$(_expectSerialized(_driver, "border-bottom-color", "rgb(40%, 40%, 40%)", "rgb(102, 102, 102)"));
    try$(_expectSerialized(_driver, "border-bottom-color", "rgb(99%, 0%, 0%)", "rgb(252, 0, 0)"));
    try$(_expectSerialized(_driver, "border-bottom-color", "rgb(99%, 99%, 99%)", "rgb(252, 252, 252)"));
    try$(_expectSerialized(_driver, "border-bottom-color", "rgb(0.0%, 100.0%, 0.0%)", "rgb(0, 255, 0)"));
    try$(_expectSerialized(_driver, "border-bottom-color", "inherit", "inherit", RegisteredPropertySet::TOP_LEVEL));
    try$(_expectInvalid(_driver, "border-bottom-color", "#00000"));
    try$(_expectInvalid(_driver, "border-bottom-color", "#00"));
    try$(_expectInvalid(_driver, "border-bottom-color", "#1000000"));
    try$(_expectInvalid(_driver, "border-bottom-color", "invalidValue"));
    try$(_expectSerialized(_driver, "border-left-color", "#000000", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "border-left-color", "#000001", "rgb(0, 0, 1)"));
    try$(_expectSerialized(_driver, "border-left-color", "#000099", "rgb(0, 0, 153)"));
    try$(_expectSerialized(_driver, "border-left-color", "#0000fe", "rgb(0, 0, 254)"));
    try$(_expectSerialized(_driver, "border-left-color", "#0000ff", "rgb(0, 0, 255)"));
    try$(_expectSerialized(_driver, "border-left-color", "#000100", "rgb(0, 1, 0)"));
    try$(_expectSerialized(_driver, "border-left-color", "#000", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "border-left-color", "#001", "rgb(0, 0, 17)"));
    try$(_expectSerialized(_driver, "border-left-color", "#009900", "rgb(0, 153, 0)"));
    try$(_expectSerialized(_driver, "border-left-color", "#009", "rgb(0, 0, 153)"));
    try$(_expectSerialized(_driver, "border-left-color", "#00e", "rgb(0, 0, 238)"));
    try$(_expectSerialized(_driver, "border-left-color", "#00f", "rgb(0, 0, 255)"));
    try$(_expectSerialized(_driver, "border-left-color", "#00fe00", "rgb(0, 254, 0)"));
    try$(_expectSerialized(_driver, "border-left-color", "#00ff00", "rgb(0, 255, 0)"));
    try$(_expectSerialized(_driver, "border-left-color", "#010000", "rgb(1, 0, 0)"));
    try$(_expectSerialized(_driver, "border-left-color", "#010101", "rgb(1, 1, 1)"));
    try$(_expectSerialized(_driver, "border-left-color", "#010", "rgb(0, 17, 0)"));
    try$(_expectSerialized(_driver, "border-left-color", "#090", "rgb(0, 153, 0)"));
    try$(_expectSerialized(_driver, "border-left-color", "#0e0", "rgb(0, 238, 0)"));
    try$(_expectSerialized(_driver, "border-left-color", "#0f0", "rgb(0, 255, 0)"));
    try$(_expectSerialized(_driver, "border-left-color", "#1000", "rgba(17, 0, 0, 0)"));
    try$(_expectSerialized(_driver, "border-left-color", "#100", "rgb(17, 0, 0)"));
    try$(_expectSerialized(_driver, "border-left-color", "#111", "rgb(17, 17, 17)"));
    try$(_expectSerialized(_driver, "border-left-color", "#900", "rgb(153, 0, 0)"));
    try$(_expectSerialized(_driver, "border-left-color", "#990000", "rgb(153, 0, 0)"));
    try$(_expectSerialized(_driver, "border-left-color", "#999999", "rgb(153, 153, 153)"));
    try$(_expectSerialized(_driver, "border-left-color", "#999", "rgb(153, 153, 153)"));
    try$(_expectSerialized(_driver, "border-left-color", "#e00", "rgb(238, 0, 0)"));
    try$(_expectSerialized(_driver, "border-left-color", "#eee", "rgb(238, 238, 238)"));
    try$(_expectSerialized(_driver, "border-left-color", "#f00", "rgb(255, 0, 0)"));
    try$(_expectSerialized(_driver, "border-left-color", "#fe0000", "rgb(254, 0, 0)"));
    try$(_expectSerialized(_driver, "border-left-color", "#fefefe", "rgb(254, 254, 254)"));
    try$(_expectSerialized(_driver, "border-left-color", "#ff0000", "rgb(255, 0, 0)"));
    try$(_expectSerialized(_driver, "border-left-color", "#fff", "rgb(255, 255, 255)"));
    try$(_expectSerialized(_driver, "border-left-color", "#ffffff", "rgb(255, 255, 255)"));
    try$(_expectSerialized(_driver, "border-left-color", "rgb(+0%, +0%, +0%)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "border-left-color", "rgb(+0%, 0%, 0%)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "border-left-color", "rgb(+0, +0, +0)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "border-left-color", "rgb(+0, 0, 0)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "border-left-color", "rgb(+100%, +100%, +100%)", "rgb(255, 255, 255)"));
    try$(_expectSerialized(_driver, "border-left-color", "rgb(+100%, 0%, 0%)", "rgb(255, 0, 0)"));
    try$(_expectSerialized(_driver, "border-left-color", "rgb(+128, +128, +128)", "rgb(128, 128, 128)"));
    try$(_expectSerialized(_driver, "border-left-color", "rgb(+128, 0, 0)", "rgb(128, 0, 0)"));
    try$(_expectSerialized(_driver, "border-left-color", "rgb(+255, +255, +255)", "rgb(255, 255, 255)"));
    try$(_expectSerialized(_driver, "border-left-color", "rgb(+255, 0, 0)", "rgb(255, 0, 0)"));
    try$(_expectSerialized(_driver, "border-left-color", "rgb(+40%, +40%, +40%)", "rgb(102, 102, 102)"));
    try$(_expectSerialized(_driver, "border-left-color", "rgb(+40%, 0%, 0%)", "rgb(102, 0, 0)"));
    try$(_expectSerialized(_driver, "border-left-color", "rgb(-0%, -0%, -0%)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "border-left-color", "rgb(-0%, 0%, 0%)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "border-left-color", "rgb(-0, -0, -0)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "border-left-color", "rgb(-0, 0, 0)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "border-left-color", "rgb(0%, +0%, 0%)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "border-left-color", "rgb(0%, +100%, 0%)", "rgb(0, 255, 0)"));
    try$(_expectSerialized(_driver, "border-left-color", "rgb(0%, +40%, 0%)", "rgb(0, 102, 0)"));
    try$(_expectSerialized(_driver, "border-left-color", "rgb(0%, -0%, 0%)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "border-left-color", "rgb(0%, 0%, +0%)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "border-left-color", "rgb(0%, 0%, +100%)", "rgb(0, 0, 255)"));
    try$(_expectSerialized(_driver, "border-left-color", "rgb(0%, 0%, +40%)", "rgb(0, 0, 102)"));
    try$(_expectSerialized(_driver, "border-left-color", "rgb(0%, 0%, -0%)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "border-left-color", "rgb(0%, 0%, 0%)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "border-left-color", "rgb(0%, 0%, 1%)", "rgb(0, 0, 3)"));
    try$(_expectSerialized(_driver, "border-left-color", "rgb(0%, 0%, 100%)", "rgb(0, 0, 255)"));
    try$(_expectSerialized(_driver, "border-left-color", "rgb(0%, 0%, 40%)", "rgb(0, 0, 102)"));
    try$(_expectSerialized(_driver, "border-left-color", "rgb(0%, 0%, 99%)", "rgb(0, 0, 252)"));
    try$(_expectSerialized(_driver, "border-left-color", "rgb(0%, 1%, 0%)", "rgb(0, 3, 0)"));
    try$(_expectSerialized(_driver, "border-left-color", "rgb(0%, 100%, 0%)", "rgb(0, 255, 0)"));
    try$(_expectSerialized(_driver, "border-left-color", "rgb(0%, 40%, 0%)", "rgb(0, 102, 0)"));
    try$(_expectSerialized(_driver, "border-left-color", "rgb(0%, 99%, 0%)", "rgb(0, 252, 0)"));
    try$(_expectSerialized(_driver, "border-left-color", "rgb(0, +0, 0)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "border-left-color", "rgb(0, +128, 0)", "rgb(0, 128, 0)"));
    try$(_expectSerialized(_driver, "border-left-color", "rgb(0, +255, 0)", "rgb(0, 255, 0)"));
    try$(_expectSerialized(_driver, "border-left-color", "rgb(0, -0, 0)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "border-left-color", "rgb(0, 0, +0)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "border-left-color", "rgb(0, 0, +128)", "rgb(0, 0, 128)"));
    try$(_expectSerialized(_driver, "border-left-color", "rgb(0, 0, +255)", "rgb(0, 0, 255)"));
    try$(_expectSerialized(_driver, "border-left-color", "rgb(0, 0, -0)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "border-left-color", "rgb(0, 0, 0)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "border-left-color", "rgb(0, 0, 1)", "rgb(0, 0, 1)"));
    try$(_expectSerialized(_driver, "border-left-color", "rgb(0, 0, 128)", "rgb(0, 0, 128)"));
    try$(_expectSerialized(_driver, "border-left-color", "rgb(0, 0, 254)", "rgb(0, 0, 254)"));
    try$(_expectSerialized(_driver, "border-left-color", "rgb(0, 0, 255)", "rgb(0, 0, 255)"));
    try$(_expectSerialized(_driver, "border-left-color", "rgb(0, 1, 0)", "rgb(0, 1, 0)"));
    try$(_expectSerialized(_driver, "border-left-color", "rgb(0, 128, 0)", "rgb(0, 128, 0)"));
    try$(_expectSerialized(_driver, "border-left-color", "rgb(0, 254, 0)", "rgb(0, 254, 0)"));
    try$(_expectSerialized(_driver, "border-left-color", "rgb(0, 255, 0)", "rgb(0, 255, 0)"));
    try$(_expectSerialized(_driver, "border-left-color", "rgb(1%, 0%, 0%)", "rgb(3, 0, 0)"));
    try$(_expectSerialized(_driver, "border-left-color", "rgb(1%, 1%, 1%)", "rgb(3, 3, 3)"));
    try$(_expectSerialized(_driver, "border-left-color", "rgb(1, 0, 0)", "rgb(1, 0, 0)"));
    try$(_expectSerialized(_driver, "border-left-color", "rgb(1, 1, 1)", "rgb(1, 1, 1)"));
    try$(_expectSerialized(_driver, "border-left-color", "rgb(100%, 0%, 0%)", "rgb(255, 0, 0)"));
    try$(_expectSerialized(_driver, "border-left-color", "rgb(100%, 100%, 100%)", "rgb(255, 255, 255)"));
    try$(_expectSerialized(_driver, "border-left-color", "rgb(128, 0, 0)", "rgb(128, 0, 0)"));
    try$(_expectSerialized(_driver, "border-left-color", "rgb(128, 128, 128)", "rgb(128, 128, 128)"));
    try$(_expectSerialized(_driver, "border-left-color", "rgb(254, 0, 0)", "rgb(254, 0, 0)"));
    try$(_expectSerialized(_driver, "border-left-color", "rgb(254, 254, 254)", "rgb(254, 254, 254)"));
    try$(_expectSerialized(_driver, "border-left-color", "rgb(255, 0, 0)", "rgb(255, 0, 0)"));
    try$(_expectSerialized(_driver, "border-left-color", "rgb(255, 255, 255)", "rgb(255, 255, 255)"));
    try$(_expectSerialized(_driver, "border-left-color", "rgb(40%, 0%, 0%)", "rgb(102, 0, 0)"));
    try$(_expectSerialized(_driver, "border-left-color", "rgb(40%, 40%, 40%)", "rgb(102, 102, 102)"));
    try$(_expectSerialized(_driver, "border-left-color", "rgb(99%, 0%, 0%)", "rgb(252, 0, 0)"));
    try$(_expectSerialized(_driver, "border-left-color", "rgb(99%, 99%, 99%)", "rgb(252, 252, 252)"));
    try$(_expectSerialized(_driver, "border-left-color", "rgb(0.0%, 100.0%, 0.0%)", "rgb(0, 255, 0)"));
    try$(_expectSerialized(_driver, "border-left-color", "inherit", "inherit", RegisteredPropertySet::TOP_LEVEL));
    try$(_expectInvalid(_driver, "border-left-color", "#00000"));
    try$(_expectInvalid(_driver, "border-left-color", "#00"));
    try$(_expectInvalid(_driver, "border-left-color", "#1000000"));
    try$(_expectInvalid(_driver, "border-left-color", "invalidValue"));
    try$(_expectSerialized(_driver, "border-right-color", "#000000", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "border-right-color", "#000001", "rgb(0, 0, 1)"));
    try$(_expectSerialized(_driver, "border-right-color", "#000099", "rgb(0, 0, 153)"));
    try$(_expectSerialized(_driver, "border-right-color", "#0000fe", "rgb(0, 0, 254)"));
    try$(_expectSerialized(_driver, "border-right-color", "#0000ff", "rgb(0, 0, 255)"));
    try$(_expectSerialized(_driver, "border-right-color", "#000100", "rgb(0, 1, 0)"));
    try$(_expectSerialized(_driver, "border-right-color", "#000", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "border-right-color", "#001", "rgb(0, 0, 17)"));
    try$(_expectSerialized(_driver, "border-right-color", "#009900", "rgb(0, 153, 0)"));
    try$(_expectSerialized(_driver, "border-right-color", "#009", "rgb(0, 0, 153)"));
    try$(_expectSerialized(_driver, "border-right-color", "#00e", "rgb(0, 0, 238)"));
    try$(_expectSerialized(_driver, "border-right-color", "#00f", "rgb(0, 0, 255)"));
    try$(_expectSerialized(_driver, "border-right-color", "#00fe00", "rgb(0, 254, 0)"));
    try$(_expectSerialized(_driver, "border-right-color", "#00ff00", "rgb(0, 255, 0)"));
    try$(_expectSerialized(_driver, "border-right-color", "#010000", "rgb(1, 0, 0)"));
    try$(_expectSerialized(_driver, "border-right-color", "#010101", "rgb(1, 1, 1)"));
    try$(_expectSerialized(_driver, "border-right-color", "#010", "rgb(0, 17, 0)"));
    try$(_expectSerialized(_driver, "border-right-color", "#090", "rgb(0, 153, 0)"));
    try$(_expectSerialized(_driver, "border-right-color", "#0e0", "rgb(0, 238, 0)"));
    try$(_expectSerialized(_driver, "border-right-color", "#0f0", "rgb(0, 255, 0)"));
    try$(_expectSerialized(_driver, "border-right-color", "#1000", "rgba(17, 0, 0, 0)"));
    try$(_expectSerialized(_driver, "border-right-color", "#100", "rgb(17, 0, 0)"));
    try$(_expectSerialized(_driver, "border-right-color", "#111", "rgb(17, 17, 17)"));
    try$(_expectSerialized(_driver, "border-right-color", "#900", "rgb(153, 0, 0)"));
    try$(_expectSerialized(_driver, "border-right-color", "#990000", "rgb(153, 0, 0)"));
    try$(_expectSerialized(_driver, "border-right-color", "#999999", "rgb(153, 153, 153)"));
    try$(_expectSerialized(_driver, "border-right-color", "#999", "rgb(153, 153, 153)"));
    try$(_expectSerialized(_driver, "border-right-color", "#e00", "rgb(238, 0, 0)"));
    try$(_expectSerialized(_driver, "border-right-color", "#eee", "rgb(238, 238, 238)"));
    try$(_expectSerialized(_driver, "border-right-color", "#f00", "rgb(255, 0, 0)"));
    try$(_expectSerialized(_driver, "border-right-color", "#fe0000", "rgb(254, 0, 0)"));
    try$(_expectSerialized(_driver, "border-right-color", "#fefefe", "rgb(254, 254, 254)"));
    try$(_expectSerialized(_driver, "border-right-color", "#ff0000", "rgb(255, 0, 0)"));
    try$(_expectSerialized(_driver, "border-right-color", "#fff", "rgb(255, 255, 255)"));
    try$(_expectSerialized(_driver, "border-right-color", "#ffffff", "rgb(255, 255, 255)"));
    try$(_expectSerialized(_driver, "border-right-color", "rgb(+0%, +0%, +0%)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "border-right-color", "rgb(+0%, 0%, 0%)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "border-right-color", "rgb(+0, +0, +0)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "border-right-color", "rgb(+0, 0, 0)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "border-right-color", "rgb(+100%, +100%, +100%)", "rgb(255, 255, 255)"));
    try$(_expectSerialized(_driver, "border-right-color", "rgb(+100%, 0%, 0%)", "rgb(255, 0, 0)"));
    try$(_expectSerialized(_driver, "border-right-color", "rgb(+128, +128, +128)", "rgb(128, 128, 128)"));
    try$(_expectSerialized(_driver, "border-right-color", "rgb(+128, 0, 0)", "rgb(128, 0, 0)"));
    try$(_expectSerialized(_driver, "border-right-color", "rgb(+255, +255, +255)", "rgb(255, 255, 255)"));
    try$(_expectSerialized(_driver, "border-right-color", "rgb(+255, 0, 0)", "rgb(255, 0, 0)"));
    try$(_expectSerialized(_driver, "border-right-color", "rgb(+40%, +40%, +40%)", "rgb(102, 102, 102)"));
    try$(_expectSerialized(_driver, "border-right-color", "rgb(+40%, 0%, 0%)", "rgb(102, 0, 0)"));
    try$(_expectSerialized(_driver, "border-right-color", "rgb(-0%, -0%, -0%)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "border-right-color", "rgb(-0%, 0%, 0%)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "border-right-color", "rgb(-0, -0, -0)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "border-right-color", "rgb(-0, 0, 0)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "border-right-color", "rgb(0%, +0%, 0%)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "border-right-color", "rgb(0%, +100%, 0%)", "rgb(0, 255, 0)"));
    try$(_expectSerialized(_driver, "border-right-color", "rgb(0%, +40%, 0%)", "rgb(0, 102, 0)"));
    try$(_expectSerialized(_driver, "border-right-color", "rgb(0%, -0%, 0%)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "border-right-color", "rgb(0%, 0%, +0%)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "border-right-color", "rgb(0%, 0%, +100%)", "rgb(0, 0, 255)"));
    try$(_expectSerialized(_driver, "border-right-color", "rgb(0%, 0%, +40%)", "rgb(0, 0, 102)"));
    try$(_expectSerialized(_driver, "border-right-color", "rgb(0%, 0%, -0%)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "border-right-color", "rgb(0%, 0%, 0%)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "border-right-color", "rgb(0%, 0%, 1%)", "rgb(0, 0, 3)"));
    try$(_expectSerialized(_driver, "border-right-color", "rgb(0%, 0%, 100%)", "rgb(0, 0, 255)"));
    try$(_expectSerialized(_driver, "border-right-color", "rgb(0%, 0%, 40%)", "rgb(0, 0, 102)"));
    try$(_expectSerialized(_driver, "border-right-color", "rgb(0%, 0%, 99%)", "rgb(0, 0, 252)"));
    try$(_expectSerialized(_driver, "border-right-color", "rgb(0%, 1%, 0%)", "rgb(0, 3, 0)"));
    try$(_expectSerialized(_driver, "border-right-color", "rgb(0%, 100%, 0%)", "rgb(0, 255, 0)"));
    try$(_expectSerialized(_driver, "border-right-color", "rgb(0%, 40%, 0%)", "rgb(0, 102, 0)"));
    try$(_expectSerialized(_driver, "border-right-color", "rgb(0%, 99%, 0%)", "rgb(0, 252, 0)"));
    try$(_expectSerialized(_driver, "border-right-color", "rgb(0, +0, 0)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "border-right-color", "rgb(0, +128, 0)", "rgb(0, 128, 0)"));
    try$(_expectSerialized(_driver, "border-right-color", "rgb(0, +255, 0)", "rgb(0, 255, 0)"));
    try$(_expectSerialized(_driver, "border-right-color", "rgb(0, -0, 0)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "border-right-color", "rgb(0, 0, +0)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "border-right-color", "rgb(0, 0, +128)", "rgb(0, 0, 128)"));
    try$(_expectSerialized(_driver, "border-right-color", "rgb(0, 0, +255)", "rgb(0, 0, 255)"));
    try$(_expectSerialized(_driver, "border-right-color", "rgb(0, 0, -0)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "border-right-color", "rgb(0, 0, 0)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "border-right-color", "rgb(0, 0, 1)", "rgb(0, 0, 1)"));
    try$(_expectSerialized(_driver, "border-right-color", "rgb(0, 0, 128)", "rgb(0, 0, 128)"));
    try$(_expectSerialized(_driver, "border-right-color", "rgb(0, 0, 254)", "rgb(0, 0, 254)"));
    try$(_expectSerialized(_driver, "border-right-color", "rgb(0, 0, 255)", "rgb(0, 0, 255)"));
    try$(_expectSerialized(_driver, "border-right-color", "rgb(0, 1, 0)", "rgb(0, 1, 0)"));
    try$(_expectSerialized(_driver, "border-right-color", "rgb(0, 128, 0)", "rgb(0, 128, 0)"));
    try$(_expectSerialized(_driver, "border-right-color", "rgb(0, 254, 0)", "rgb(0, 254, 0)"));
    try$(_expectSerialized(_driver, "border-right-color", "rgb(0, 255, 0)", "rgb(0, 255, 0)"));
    try$(_expectSerialized(_driver, "border-right-color", "rgb(1%, 0%, 0%)", "rgb(3, 0, 0)"));
    try$(_expectSerialized(_driver, "border-right-color", "rgb(1%, 1%, 1%)", "rgb(3, 3, 3)"));
    try$(_expectSerialized(_driver, "border-right-color", "rgb(1, 0, 0)", "rgb(1, 0, 0)"));
    try$(_expectSerialized(_driver, "border-right-color", "rgb(1, 1, 1)", "rgb(1, 1, 1)"));
    try$(_expectSerialized(_driver, "border-right-color", "rgb(100%, 0%, 0%)", "rgb(255, 0, 0)"));
    try$(_expectSerialized(_driver, "border-right-color", "rgb(100%, 100%, 100%)", "rgb(255, 255, 255)"));
    try$(_expectSerialized(_driver, "border-right-color", "rgb(128, 0, 0)", "rgb(128, 0, 0)"));
    try$(_expectSerialized(_driver, "border-right-color", "rgb(128, 128, 128)", "rgb(128, 128, 128)"));
    try$(_expectSerialized(_driver, "border-right-color", "rgb(254, 0, 0)", "rgb(254, 0, 0)"));
    try$(_expectSerialized(_driver, "border-right-color", "rgb(254, 254, 254)", "rgb(254, 254, 254)"));
    try$(_expectSerialized(_driver, "border-right-color", "rgb(255, 0, 0)", "rgb(255, 0, 0)"));
    try$(_expectSerialized(_driver, "border-right-color", "rgb(255, 255, 255)", "rgb(255, 255, 255)"));
    try$(_expectSerialized(_driver, "border-right-color", "rgb(40%, 0%, 0%)", "rgb(102, 0, 0)"));
    try$(_expectSerialized(_driver, "border-right-color", "rgb(40%, 40%, 40%)", "rgb(102, 102, 102)"));
    try$(_expectSerialized(_driver, "border-right-color", "rgb(99%, 0%, 0%)", "rgb(252, 0, 0)"));
    try$(_expectSerialized(_driver, "border-right-color", "rgb(99%, 99%, 99%)", "rgb(252, 252, 252)"));
    try$(_expectSerialized(_driver, "border-right-color", "rgb(0.0%, 100.0%, 0.0%)", "rgb(0, 255, 0)"));
    try$(_expectSerialized(_driver, "border-right-color", "inherit", "inherit", RegisteredPropertySet::TOP_LEVEL));
    try$(_expectInvalid(_driver, "border-right-color", "#00000"));
    try$(_expectInvalid(_driver, "border-right-color", "#00"));
    try$(_expectInvalid(_driver, "border-right-color", "#1000000"));
    try$(_expectInvalid(_driver, "border-right-color", "invalidValue"));
    try$(_expectSerialized(_driver, "outline-color", "#000000", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "outline-color", "#000001", "rgb(0, 0, 1)"));
    try$(_expectSerialized(_driver, "outline-color", "#000099", "rgb(0, 0, 153)"));
    try$(_expectSerialized(_driver, "outline-color", "#0000fe", "rgb(0, 0, 254)"));
    try$(_expectSerialized(_driver, "outline-color", "#0000ff", "rgb(0, 0, 255)"));
    try$(_expectSerialized(_driver, "outline-color", "#000100", "rgb(0, 1, 0)"));
    try$(_expectSerialized(_driver, "outline-color", "#000", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "outline-color", "#001", "rgb(0, 0, 17)"));
    try$(_expectSerialized(_driver, "outline-color", "#009900", "rgb(0, 153, 0)"));
    try$(_expectSerialized(_driver, "outline-color", "#009", "rgb(0, 0, 153)"));
    try$(_expectSerialized(_driver, "outline-color", "#00e", "rgb(0, 0, 238)"));
    try$(_expectSerialized(_driver, "outline-color", "#00f", "rgb(0, 0, 255)"));
    try$(_expectSerialized(_driver, "outline-color", "#00fe00", "rgb(0, 254, 0)"));
    try$(_expectSerialized(_driver, "outline-color", "#00ff00", "rgb(0, 255, 0)"));
    try$(_expectSerialized(_driver, "outline-color", "#010000", "rgb(1, 0, 0)"));
    try$(_expectSerialized(_driver, "outline-color", "#010101", "rgb(1, 1, 1)"));
    try$(_expectSerialized(_driver, "outline-color", "#010", "rgb(0, 17, 0)"));
    try$(_expectSerialized(_driver, "outline-color", "#090", "rgb(0, 153, 0)"));
    try$(_expectSerialized(_driver, "outline-color", "#0e0", "rgb(0, 238, 0)"));
    try$(_expectSerialized(_driver, "outline-color", "#0f0", "rgb(0, 255, 0)"));
    try$(_expectSerialized(_driver, "outline-color", "#1000", "rgba(17, 0, 0, 0)"));
    try$(_expectSerialized(_driver, "outline-color", "#100", "rgb(17, 0, 0)"));
    try$(_expectSerialized(_driver, "outline-color", "#111", "rgb(17, 17, 17)"));
    try$(_expectSerialized(_driver, "outline-color", "#900", "rgb(153, 0, 0)"));
    try$(_expectSerialized(_driver, "outline-color", "#990000", "rgb(153, 0, 0)"));
    try$(_expectSerialized(_driver, "outline-color", "#999999", "rgb(153, 153, 153)"));
    try$(_expectSerialized(_driver, "outline-color", "#999", "rgb(153, 153, 153)"));
    try$(_expectSerialized(_driver, "outline-color", "#e00", "rgb(238, 0, 0)"));
    try$(_expectSerialized(_driver, "outline-color", "#eee", "rgb(238, 238, 238)"));
    try$(_expectSerialized(_driver, "outline-color", "#f00", "rgb(255, 0, 0)"));
    try$(_expectSerialized(_driver, "outline-color", "#fe0000", "rgb(254, 0, 0)"));
    try$(_expectSerialized(_driver, "outline-color", "#fefefe", "rgb(254, 254, 254)"));
    try$(_expectSerialized(_driver, "outline-color", "#ff0000", "rgb(255, 0, 0)"));
    try$(_expectSerialized(_driver, "outline-color", "#fff", "rgb(255, 255, 255)"));
    try$(_expectSerialized(_driver, "outline-color", "#ffffff", "rgb(255, 255, 255)"));
    try$(_expectSerialized(_driver, "outline-color", "rgb(+0%, +0%, +0%)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "outline-color", "rgb(+0%, 0%, 0%)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "outline-color", "rgb(+0, +0, +0)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "outline-color", "rgb(+0, 0, 0)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "outline-color", "rgb(+100%, +100%, +100%)", "rgb(255, 255, 255)"));
    try$(_expectSerialized(_driver, "outline-color", "rgb(+100%, 0%, 0%)", "rgb(255, 0, 0)"));
    try$(_expectSerialized(_driver, "outline-color", "rgb(+128, +128, +128)", "rgb(128, 128, 128)"));
    try$(_expectSerialized(_driver, "outline-color", "rgb(+128, 0, 0)", "rgb(128, 0, 0)"));
    try$(_expectSerialized(_driver, "outline-color", "rgb(+255, +255, +255)", "rgb(255, 255, 255)"));
    try$(_expectSerialized(_driver, "outline-color", "rgb(+255, 0, 0)", "rgb(255, 0, 0)"));
    try$(_expectSerialized(_driver, "outline-color", "rgb(+40%, +40%, +40%)", "rgb(102, 102, 102)"));
    try$(_expectSerialized(_driver, "outline-color", "rgb(+40%, 0%, 0%)", "rgb(102, 0, 0)"));
    try$(_expectSerialized(_driver, "outline-color", "rgb(-0%, -0%, -0%)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "outline-color", "rgb(-0%, 0%, 0%)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "outline-color", "rgb(-0, -0, -0)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "outline-color", "rgb(-0, 0, 0)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "outline-color", "rgb(0%, +0%, 0%)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "outline-color", "rgb(0%, +100%, 0%)", "rgb(0, 255, 0)"));
    try$(_expectSerialized(_driver, "outline-color", "rgb(0%, +40%, 0%)", "rgb(0, 102, 0)"));
    try$(_expectSerialized(_driver, "outline-color", "rgb(0%, -0%, 0%)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "outline-color", "rgb(0%, 0%, +0%)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "outline-color", "rgb(0%, 0%, +100%)", "rgb(0, 0, 255)"));
    try$(_expectSerialized(_driver, "outline-color", "rgb(0%, 0%, +40%)", "rgb(0, 0, 102)"));
    try$(_expectSerialized(_driver, "outline-color", "rgb(0%, 0%, -0%)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "outline-color", "rgb(0%, 0%, 0%)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "outline-color", "rgb(0%, 0%, 1%)", "rgb(0, 0, 3)"));
    try$(_expectSerialized(_driver, "outline-color", "rgb(0%, 0%, 100%)", "rgb(0, 0, 255)"));
    try$(_expectSerialized(_driver, "outline-color", "rgb(0%, 0%, 40%)", "rgb(0, 0, 102)"));
    try$(_expectSerialized(_driver, "outline-color", "rgb(0%, 0%, 99%)", "rgb(0, 0, 252)"));
    try$(_expectSerialized(_driver, "outline-color", "rgb(0%, 1%, 0%)", "rgb(0, 3, 0)"));
    try$(_expectSerialized(_driver, "outline-color", "rgb(0%, 100%, 0%)", "rgb(0, 255, 0)"));
    try$(_expectSerialized(_driver, "outline-color", "rgb(0%, 40%, 0%)", "rgb(0, 102, 0)"));
    try$(_expectSerialized(_driver, "outline-color", "rgb(0%, 99%, 0%)", "rgb(0, 252, 0)"));
    try$(_expectSerialized(_driver, "outline-color", "rgb(0, +0, 0)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "outline-color", "rgb(0, +128, 0)", "rgb(0, 128, 0)"));
    try$(_expectSerialized(_driver, "outline-color", "rgb(0, +255, 0)", "rgb(0, 255, 0)"));
    try$(_expectSerialized(_driver, "outline-color", "rgb(0, -0, 0)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "outline-color", "rgb(0, 0, +0)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "outline-color", "rgb(0, 0, +128)", "rgb(0, 0, 128)"));
    try$(_expectSerialized(_driver, "outline-color", "rgb(0, 0, +255)", "rgb(0, 0, 255)"));
    try$(_expectSerialized(_driver, "outline-color", "rgb(0, 0, -0)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "outline-color", "rgb(0, 0, 0)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "outline-color", "rgb(0, 0, 1)", "rgb(0, 0, 1)"));
    try$(_expectSerialized(_driver, "outline-color", "rgb(0, 0, 128)", "rgb(0, 0, 128)"));
    try$(_expectSerialized(_driver, "outline-color", "rgb(0, 0, 254)", "rgb(0, 0, 254)"));
    try$(_expectSerialized(_driver, "outline-color", "rgb(0, 0, 255)", "rgb(0, 0, 255)"));
    try$(_expectSerialized(_driver, "outline-color", "rgb(0, 1, 0)", "rgb(0, 1, 0)"));
    try$(_expectSerialized(_driver, "outline-color", "rgb(0, 128, 0)", "rgb(0, 128, 0)"));
    try$(_expectSerialized(_driver, "outline-color", "rgb(0, 254, 0)", "rgb(0, 254, 0)"));
    try$(_expectSerialized(_driver, "outline-color", "rgb(0, 255, 0)", "rgb(0, 255, 0)"));
    try$(_expectSerialized(_driver, "outline-color", "rgb(1%, 0%, 0%)", "rgb(3, 0, 0)"));
    try$(_expectSerialized(_driver, "outline-color", "rgb(1%, 1%, 1%)", "rgb(3, 3, 3)"));
    try$(_expectSerialized(_driver, "outline-color", "rgb(1, 0, 0)", "rgb(1, 0, 0)"));
    try$(_expectSerialized(_driver, "outline-color", "rgb(1, 1, 1)", "rgb(1, 1, 1)"));
    try$(_expectSerialized(_driver, "outline-color", "rgb(100%, 0%, 0%)", "rgb(255, 0, 0)"));
    try$(_expectSerialized(_driver, "outline-color", "rgb(100%, 100%, 100%)", "rgb(255, 255, 255)"));
    try$(_expectSerialized(_driver, "outline-color", "rgb(128, 0, 0)", "rgb(128, 0, 0)"));
    try$(_expectSerialized(_driver, "outline-color", "rgb(128, 128, 128)", "rgb(128, 128, 128)"));
    try$(_expectSerialized(_driver, "outline-color", "rgb(254, 0, 0)", "rgb(254, 0, 0)"));
    try$(_expectSerialized(_driver, "outline-color", "rgb(254, 254, 254)", "rgb(254, 254, 254)"));
    try$(_expectSerialized(_driver, "outline-color", "rgb(255, 0, 0)", "rgb(255, 0, 0)"));
    try$(_expectSerialized(_driver, "outline-color", "rgb(255, 255, 255)", "rgb(255, 255, 255)"));
    try$(_expectSerialized(_driver, "outline-color", "rgb(40%, 0%, 0%)", "rgb(102, 0, 0)"));
    try$(_expectSerialized(_driver, "outline-color", "rgb(40%, 40%, 40%)", "rgb(102, 102, 102)"));
    try$(_expectSerialized(_driver, "outline-color", "rgb(99%, 0%, 0%)", "rgb(252, 0, 0)"));
    try$(_expectSerialized(_driver, "outline-color", "rgb(99%, 99%, 99%)", "rgb(252, 252, 252)"));
    try$(_expectSerialized(_driver, "outline-color", "rgb(0.0%, 100.0%, 0.0%)", "rgb(0, 255, 0)"));
    try$(_expectSerialized(_driver, "outline-color", "inherit", "inherit", RegisteredPropertySet::TOP_LEVEL));
    try$(_expectInvalid(_driver, "outline-color", "#00000"));
    try$(_expectInvalid(_driver, "outline-color", "#00"));
    try$(_expectInvalid(_driver, "outline-color", "#1000000"));
    try$(_expectInvalid(_driver, "outline-color", "invalidValue"));
    return Ok();
}

test$("vaev-style-wpt-css-CSS2-syntax-colors-007-xfail", XFAIL) {
    try$(_expectSerialized(_driver, "color", "rgb(-1%, -1%, -1%)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "color", "rgb(-1%, 0%, 0%)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "color", "rgb(-1, -1, -1)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "color", "rgb(-1, 0, 0)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "color", "rgb(0%, -1%, 0%)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "color", "rgb(0%, 0%, -1%)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "color", "rgb(0%, 0%, 101%)", "rgb(0, 0, 255)"));
    try$(_expectSerialized(_driver, "color", "rgb(0%, 101%, 0%)", "rgb(0, 255, 0)"));
    try$(_expectSerialized(_driver, "color", "rgb(0, -1, 0)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "color", "rgb(0, 0, -1)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "color", "rgb(0, 0, 256)", "rgb(0, 0, 255)"));
    try$(_expectSerialized(_driver, "color", "rgb(0, 256, 0)", "rgb(0, 255, 0)"));
    try$(_expectSerialized(_driver, "color", "rgb(101%, 0%, 0%)", "rgb(255, 0, 0)"));
    try$(_expectSerialized(_driver, "color", "rgb(101%, 101%, 101%)", "rgb(255, 255, 255)"));
    try$(_expectSerialized(_driver, "color", "rgb(256, 0, 0)", "rgb(255, 0, 0)"));
    try$(_expectSerialized(_driver, "color", "rgb(256, 256, 256)", "rgb(255, 255, 255)"));
    try$(_expectSerialized(_driver, "color", "aqua", "aqua"));
    try$(_expectSerialized(_driver, "color", "black", "black"));
    try$(_expectSerialized(_driver, "color", "blue", "blue"));
    try$(_expectSerialized(_driver, "color", "fuchsia", "fuchsia"));
    try$(_expectSerialized(_driver, "color", "gray", "gray"));
    try$(_expectSerialized(_driver, "color", "green", "green"));
    try$(_expectSerialized(_driver, "color", "lime", "lime"));
    try$(_expectSerialized(_driver, "color", "maroon", "maroon"));
    try$(_expectSerialized(_driver, "color", "navy", "navy"));
    try$(_expectSerialized(_driver, "color", "olive", "olive"));
    try$(_expectSerialized(_driver, "color", "orange", "orange"));
    try$(_expectSerialized(_driver, "color", "purple", "purple"));
    try$(_expectSerialized(_driver, "color", "red", "red"));
    try$(_expectSerialized(_driver, "color", "silver", "silver"));
    try$(_expectSerialized(_driver, "color", "teal", "teal"));
    try$(_expectSerialized(_driver, "color", "transparent", "transparent"));
    try$(_expectSerialized(_driver, "color", "white", "white"));
    try$(_expectSerialized(_driver, "color", "yellow", "yellow"));
    try$(_expectInvalid(_driver, "color", "#0000fg"));
    try$(_expectInvalid(_driver, "color", "#00fg00"));
    try$(_expectInvalid(_driver, "color", "#00g"));
    try$(_expectInvalid(_driver, "color", "#0g0"));
    try$(_expectInvalid(_driver, "color", "#fg0000"));
    try$(_expectInvalid(_driver, "color", "#fgfgfg"));
    try$(_expectInvalid(_driver, "color", "#g00"));
    try$(_expectInvalid(_driver, "color", "#ggg"));
    try$(_expectSerialized(_driver, "background-color", "rgb(-1%, -1%, -1%)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "background-color", "rgb(-1%, 0%, 0%)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "background-color", "rgb(-1, -1, -1)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "background-color", "rgb(-1, 0, 0)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "background-color", "rgb(0%, -1%, 0%)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "background-color", "rgb(0%, 0%, -1%)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "background-color", "rgb(0%, 0%, 101%)", "rgb(0, 0, 255)"));
    try$(_expectSerialized(_driver, "background-color", "rgb(0%, 101%, 0%)", "rgb(0, 255, 0)"));
    try$(_expectSerialized(_driver, "background-color", "rgb(0, -1, 0)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "background-color", "rgb(0, 0, -1)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "background-color", "rgb(0, 0, 256)", "rgb(0, 0, 255)"));
    try$(_expectSerialized(_driver, "background-color", "rgb(0, 256, 0)", "rgb(0, 255, 0)"));
    try$(_expectSerialized(_driver, "background-color", "rgb(101%, 0%, 0%)", "rgb(255, 0, 0)"));
    try$(_expectSerialized(_driver, "background-color", "rgb(101%, 101%, 101%)", "rgb(255, 255, 255)"));
    try$(_expectSerialized(_driver, "background-color", "rgb(256, 0, 0)", "rgb(255, 0, 0)"));
    try$(_expectSerialized(_driver, "background-color", "rgb(256, 256, 256)", "rgb(255, 255, 255)"));
    try$(_expectSerialized(_driver, "background-color", "aqua", "aqua"));
    try$(_expectSerialized(_driver, "background-color", "black", "black"));
    try$(_expectSerialized(_driver, "background-color", "blue", "blue"));
    try$(_expectSerialized(_driver, "background-color", "fuchsia", "fuchsia"));
    try$(_expectSerialized(_driver, "background-color", "gray", "gray"));
    try$(_expectSerialized(_driver, "background-color", "green", "green"));
    try$(_expectSerialized(_driver, "background-color", "lime", "lime"));
    try$(_expectSerialized(_driver, "background-color", "maroon", "maroon"));
    try$(_expectSerialized(_driver, "background-color", "navy", "navy"));
    try$(_expectSerialized(_driver, "background-color", "olive", "olive"));
    try$(_expectSerialized(_driver, "background-color", "orange", "orange"));
    try$(_expectSerialized(_driver, "background-color", "purple", "purple"));
    try$(_expectSerialized(_driver, "background-color", "red", "red"));
    try$(_expectSerialized(_driver, "background-color", "silver", "silver"));
    try$(_expectSerialized(_driver, "background-color", "teal", "teal"));
    try$(_expectSerialized(_driver, "background-color", "transparent", "transparent"));
    try$(_expectSerialized(_driver, "background-color", "white", "white"));
    try$(_expectSerialized(_driver, "background-color", "yellow", "yellow"));
    try$(_expectInvalid(_driver, "background-color", "#0000fg"));
    try$(_expectInvalid(_driver, "background-color", "#00fg00"));
    try$(_expectInvalid(_driver, "background-color", "#00g"));
    try$(_expectInvalid(_driver, "background-color", "#0g0"));
    try$(_expectInvalid(_driver, "background-color", "#fg0000"));
    try$(_expectInvalid(_driver, "background-color", "#fgfgfg"));
    try$(_expectInvalid(_driver, "background-color", "#g00"));
    try$(_expectInvalid(_driver, "background-color", "#ggg"));
    try$(_expectSerialized(_driver, "border-top-color", "rgb(-1%, -1%, -1%)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "border-top-color", "rgb(-1%, 0%, 0%)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "border-top-color", "rgb(-1, -1, -1)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "border-top-color", "rgb(-1, 0, 0)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "border-top-color", "rgb(0%, -1%, 0%)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "border-top-color", "rgb(0%, 0%, -1%)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "border-top-color", "rgb(0%, 0%, 101%)", "rgb(0, 0, 255)"));
    try$(_expectSerialized(_driver, "border-top-color", "rgb(0%, 101%, 0%)", "rgb(0, 255, 0)"));
    try$(_expectSerialized(_driver, "border-top-color", "rgb(0, -1, 0)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "border-top-color", "rgb(0, 0, -1)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "border-top-color", "rgb(0, 0, 256)", "rgb(0, 0, 255)"));
    try$(_expectSerialized(_driver, "border-top-color", "rgb(0, 256, 0)", "rgb(0, 255, 0)"));
    try$(_expectSerialized(_driver, "border-top-color", "rgb(101%, 0%, 0%)", "rgb(255, 0, 0)"));
    try$(_expectSerialized(_driver, "border-top-color", "rgb(101%, 101%, 101%)", "rgb(255, 255, 255)"));
    try$(_expectSerialized(_driver, "border-top-color", "rgb(256, 0, 0)", "rgb(255, 0, 0)"));
    try$(_expectSerialized(_driver, "border-top-color", "rgb(256, 256, 256)", "rgb(255, 255, 255)"));
    try$(_expectSerialized(_driver, "border-top-color", "aqua", "aqua"));
    try$(_expectSerialized(_driver, "border-top-color", "black", "black"));
    try$(_expectSerialized(_driver, "border-top-color", "blue", "blue"));
    try$(_expectSerialized(_driver, "border-top-color", "fuchsia", "fuchsia"));
    try$(_expectSerialized(_driver, "border-top-color", "gray", "gray"));
    try$(_expectSerialized(_driver, "border-top-color", "green", "green"));
    try$(_expectSerialized(_driver, "border-top-color", "lime", "lime"));
    try$(_expectSerialized(_driver, "border-top-color", "maroon", "maroon"));
    try$(_expectSerialized(_driver, "border-top-color", "navy", "navy"));
    try$(_expectSerialized(_driver, "border-top-color", "olive", "olive"));
    try$(_expectSerialized(_driver, "border-top-color", "orange", "orange"));
    try$(_expectSerialized(_driver, "border-top-color", "purple", "purple"));
    try$(_expectSerialized(_driver, "border-top-color", "red", "red"));
    try$(_expectSerialized(_driver, "border-top-color", "silver", "silver"));
    try$(_expectSerialized(_driver, "border-top-color", "teal", "teal"));
    try$(_expectSerialized(_driver, "border-top-color", "transparent", "transparent"));
    try$(_expectSerialized(_driver, "border-top-color", "white", "white"));
    try$(_expectSerialized(_driver, "border-top-color", "yellow", "yellow"));
    try$(_expectInvalid(_driver, "border-top-color", "#0000fg"));
    try$(_expectInvalid(_driver, "border-top-color", "#00fg00"));
    try$(_expectInvalid(_driver, "border-top-color", "#00g"));
    try$(_expectInvalid(_driver, "border-top-color", "#0g0"));
    try$(_expectInvalid(_driver, "border-top-color", "#fg0000"));
    try$(_expectInvalid(_driver, "border-top-color", "#fgfgfg"));
    try$(_expectInvalid(_driver, "border-top-color", "#g00"));
    try$(_expectInvalid(_driver, "border-top-color", "#ggg"));
    try$(_expectSerialized(_driver, "border-bottom-color", "rgb(-1%, -1%, -1%)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "border-bottom-color", "rgb(-1%, 0%, 0%)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "border-bottom-color", "rgb(-1, -1, -1)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "border-bottom-color", "rgb(-1, 0, 0)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "border-bottom-color", "rgb(0%, -1%, 0%)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "border-bottom-color", "rgb(0%, 0%, -1%)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "border-bottom-color", "rgb(0%, 0%, 101%)", "rgb(0, 0, 255)"));
    try$(_expectSerialized(_driver, "border-bottom-color", "rgb(0%, 101%, 0%)", "rgb(0, 255, 0)"));
    try$(_expectSerialized(_driver, "border-bottom-color", "rgb(0, -1, 0)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "border-bottom-color", "rgb(0, 0, -1)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "border-bottom-color", "rgb(0, 0, 256)", "rgb(0, 0, 255)"));
    try$(_expectSerialized(_driver, "border-bottom-color", "rgb(0, 256, 0)", "rgb(0, 255, 0)"));
    try$(_expectSerialized(_driver, "border-bottom-color", "rgb(101%, 0%, 0%)", "rgb(255, 0, 0)"));
    try$(_expectSerialized(_driver, "border-bottom-color", "rgb(101%, 101%, 101%)", "rgb(255, 255, 255)"));
    try$(_expectSerialized(_driver, "border-bottom-color", "rgb(256, 0, 0)", "rgb(255, 0, 0)"));
    try$(_expectSerialized(_driver, "border-bottom-color", "rgb(256, 256, 256)", "rgb(255, 255, 255)"));
    try$(_expectSerialized(_driver, "border-bottom-color", "aqua", "aqua"));
    try$(_expectSerialized(_driver, "border-bottom-color", "black", "black"));
    try$(_expectSerialized(_driver, "border-bottom-color", "blue", "blue"));
    try$(_expectSerialized(_driver, "border-bottom-color", "fuchsia", "fuchsia"));
    try$(_expectSerialized(_driver, "border-bottom-color", "gray", "gray"));
    try$(_expectSerialized(_driver, "border-bottom-color", "green", "green"));
    try$(_expectSerialized(_driver, "border-bottom-color", "lime", "lime"));
    try$(_expectSerialized(_driver, "border-bottom-color", "maroon", "maroon"));
    try$(_expectSerialized(_driver, "border-bottom-color", "navy", "navy"));
    try$(_expectSerialized(_driver, "border-bottom-color", "olive", "olive"));
    try$(_expectSerialized(_driver, "border-bottom-color", "orange", "orange"));
    try$(_expectSerialized(_driver, "border-bottom-color", "purple", "purple"));
    try$(_expectSerialized(_driver, "border-bottom-color", "red", "red"));
    try$(_expectSerialized(_driver, "border-bottom-color", "silver", "silver"));
    try$(_expectSerialized(_driver, "border-bottom-color", "teal", "teal"));
    try$(_expectSerialized(_driver, "border-bottom-color", "transparent", "transparent"));
    try$(_expectSerialized(_driver, "border-bottom-color", "white", "white"));
    try$(_expectSerialized(_driver, "border-bottom-color", "yellow", "yellow"));
    try$(_expectInvalid(_driver, "border-bottom-color", "#0000fg"));
    try$(_expectInvalid(_driver, "border-bottom-color", "#00fg00"));
    try$(_expectInvalid(_driver, "border-bottom-color", "#00g"));
    try$(_expectInvalid(_driver, "border-bottom-color", "#0g0"));
    try$(_expectInvalid(_driver, "border-bottom-color", "#fg0000"));
    try$(_expectInvalid(_driver, "border-bottom-color", "#fgfgfg"));
    try$(_expectInvalid(_driver, "border-bottom-color", "#g00"));
    try$(_expectInvalid(_driver, "border-bottom-color", "#ggg"));
    try$(_expectSerialized(_driver, "border-left-color", "rgb(-1%, -1%, -1%)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "border-left-color", "rgb(-1%, 0%, 0%)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "border-left-color", "rgb(-1, -1, -1)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "border-left-color", "rgb(-1, 0, 0)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "border-left-color", "rgb(0%, -1%, 0%)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "border-left-color", "rgb(0%, 0%, -1%)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "border-left-color", "rgb(0%, 0%, 101%)", "rgb(0, 0, 255)"));
    try$(_expectSerialized(_driver, "border-left-color", "rgb(0%, 101%, 0%)", "rgb(0, 255, 0)"));
    try$(_expectSerialized(_driver, "border-left-color", "rgb(0, -1, 0)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "border-left-color", "rgb(0, 0, -1)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "border-left-color", "rgb(0, 0, 256)", "rgb(0, 0, 255)"));
    try$(_expectSerialized(_driver, "border-left-color", "rgb(0, 256, 0)", "rgb(0, 255, 0)"));
    try$(_expectSerialized(_driver, "border-left-color", "rgb(101%, 0%, 0%)", "rgb(255, 0, 0)"));
    try$(_expectSerialized(_driver, "border-left-color", "rgb(101%, 101%, 101%)", "rgb(255, 255, 255)"));
    try$(_expectSerialized(_driver, "border-left-color", "rgb(256, 0, 0)", "rgb(255, 0, 0)"));
    try$(_expectSerialized(_driver, "border-left-color", "rgb(256, 256, 256)", "rgb(255, 255, 255)"));
    try$(_expectSerialized(_driver, "border-left-color", "aqua", "aqua"));
    try$(_expectSerialized(_driver, "border-left-color", "black", "black"));
    try$(_expectSerialized(_driver, "border-left-color", "blue", "blue"));
    try$(_expectSerialized(_driver, "border-left-color", "fuchsia", "fuchsia"));
    try$(_expectSerialized(_driver, "border-left-color", "gray", "gray"));
    try$(_expectSerialized(_driver, "border-left-color", "green", "green"));
    try$(_expectSerialized(_driver, "border-left-color", "lime", "lime"));
    try$(_expectSerialized(_driver, "border-left-color", "maroon", "maroon"));
    try$(_expectSerialized(_driver, "border-left-color", "navy", "navy"));
    try$(_expectSerialized(_driver, "border-left-color", "olive", "olive"));
    try$(_expectSerialized(_driver, "border-left-color", "orange", "orange"));
    try$(_expectSerialized(_driver, "border-left-color", "purple", "purple"));
    try$(_expectSerialized(_driver, "border-left-color", "red", "red"));
    try$(_expectSerialized(_driver, "border-left-color", "silver", "silver"));
    try$(_expectSerialized(_driver, "border-left-color", "teal", "teal"));
    try$(_expectSerialized(_driver, "border-left-color", "transparent", "transparent"));
    try$(_expectSerialized(_driver, "border-left-color", "white", "white"));
    try$(_expectSerialized(_driver, "border-left-color", "yellow", "yellow"));
    try$(_expectInvalid(_driver, "border-left-color", "#0000fg"));
    try$(_expectInvalid(_driver, "border-left-color", "#00fg00"));
    try$(_expectInvalid(_driver, "border-left-color", "#00g"));
    try$(_expectInvalid(_driver, "border-left-color", "#0g0"));
    try$(_expectInvalid(_driver, "border-left-color", "#fg0000"));
    try$(_expectInvalid(_driver, "border-left-color", "#fgfgfg"));
    try$(_expectInvalid(_driver, "border-left-color", "#g00"));
    try$(_expectInvalid(_driver, "border-left-color", "#ggg"));
    try$(_expectSerialized(_driver, "border-right-color", "rgb(-1%, -1%, -1%)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "border-right-color", "rgb(-1%, 0%, 0%)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "border-right-color", "rgb(-1, -1, -1)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "border-right-color", "rgb(-1, 0, 0)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "border-right-color", "rgb(0%, -1%, 0%)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "border-right-color", "rgb(0%, 0%, -1%)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "border-right-color", "rgb(0%, 0%, 101%)", "rgb(0, 0, 255)"));
    try$(_expectSerialized(_driver, "border-right-color", "rgb(0%, 101%, 0%)", "rgb(0, 255, 0)"));
    try$(_expectSerialized(_driver, "border-right-color", "rgb(0, -1, 0)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "border-right-color", "rgb(0, 0, -1)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "border-right-color", "rgb(0, 0, 256)", "rgb(0, 0, 255)"));
    try$(_expectSerialized(_driver, "border-right-color", "rgb(0, 256, 0)", "rgb(0, 255, 0)"));
    try$(_expectSerialized(_driver, "border-right-color", "rgb(101%, 0%, 0%)", "rgb(255, 0, 0)"));
    try$(_expectSerialized(_driver, "border-right-color", "rgb(101%, 101%, 101%)", "rgb(255, 255, 255)"));
    try$(_expectSerialized(_driver, "border-right-color", "rgb(256, 0, 0)", "rgb(255, 0, 0)"));
    try$(_expectSerialized(_driver, "border-right-color", "rgb(256, 256, 256)", "rgb(255, 255, 255)"));
    try$(_expectSerialized(_driver, "border-right-color", "aqua", "aqua"));
    try$(_expectSerialized(_driver, "border-right-color", "black", "black"));
    try$(_expectSerialized(_driver, "border-right-color", "blue", "blue"));
    try$(_expectSerialized(_driver, "border-right-color", "fuchsia", "fuchsia"));
    try$(_expectSerialized(_driver, "border-right-color", "gray", "gray"));
    try$(_expectSerialized(_driver, "border-right-color", "green", "green"));
    try$(_expectSerialized(_driver, "border-right-color", "lime", "lime"));
    try$(_expectSerialized(_driver, "border-right-color", "maroon", "maroon"));
    try$(_expectSerialized(_driver, "border-right-color", "navy", "navy"));
    try$(_expectSerialized(_driver, "border-right-color", "olive", "olive"));
    try$(_expectSerialized(_driver, "border-right-color", "orange", "orange"));
    try$(_expectSerialized(_driver, "border-right-color", "purple", "purple"));
    try$(_expectSerialized(_driver, "border-right-color", "red", "red"));
    try$(_expectSerialized(_driver, "border-right-color", "silver", "silver"));
    try$(_expectSerialized(_driver, "border-right-color", "teal", "teal"));
    try$(_expectSerialized(_driver, "border-right-color", "transparent", "transparent"));
    try$(_expectSerialized(_driver, "border-right-color", "white", "white"));
    try$(_expectSerialized(_driver, "border-right-color", "yellow", "yellow"));
    try$(_expectInvalid(_driver, "border-right-color", "#0000fg"));
    try$(_expectInvalid(_driver, "border-right-color", "#00fg00"));
    try$(_expectInvalid(_driver, "border-right-color", "#00g"));
    try$(_expectInvalid(_driver, "border-right-color", "#0g0"));
    try$(_expectInvalid(_driver, "border-right-color", "#fg0000"));
    try$(_expectInvalid(_driver, "border-right-color", "#fgfgfg"));
    try$(_expectInvalid(_driver, "border-right-color", "#g00"));
    try$(_expectInvalid(_driver, "border-right-color", "#ggg"));
    try$(_expectSerialized(_driver, "outline-color", "rgb(-1%, -1%, -1%)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "outline-color", "rgb(-1%, 0%, 0%)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "outline-color", "rgb(-1, -1, -1)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "outline-color", "rgb(-1, 0, 0)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "outline-color", "rgb(0%, -1%, 0%)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "outline-color", "rgb(0%, 0%, -1%)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "outline-color", "rgb(0%, 0%, 101%)", "rgb(0, 0, 255)"));
    try$(_expectSerialized(_driver, "outline-color", "rgb(0%, 101%, 0%)", "rgb(0, 255, 0)"));
    try$(_expectSerialized(_driver, "outline-color", "rgb(0, -1, 0)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "outline-color", "rgb(0, 0, -1)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "outline-color", "rgb(0, 0, 256)", "rgb(0, 0, 255)"));
    try$(_expectSerialized(_driver, "outline-color", "rgb(0, 256, 0)", "rgb(0, 255, 0)"));
    try$(_expectSerialized(_driver, "outline-color", "rgb(101%, 0%, 0%)", "rgb(255, 0, 0)"));
    try$(_expectSerialized(_driver, "outline-color", "rgb(101%, 101%, 101%)", "rgb(255, 255, 255)"));
    try$(_expectSerialized(_driver, "outline-color", "rgb(256, 0, 0)", "rgb(255, 0, 0)"));
    try$(_expectSerialized(_driver, "outline-color", "rgb(256, 256, 256)", "rgb(255, 255, 255)"));
    try$(_expectSerialized(_driver, "outline-color", "aqua", "aqua"));
    try$(_expectSerialized(_driver, "outline-color", "black", "black"));
    try$(_expectSerialized(_driver, "outline-color", "blue", "blue"));
    try$(_expectSerialized(_driver, "outline-color", "fuchsia", "fuchsia"));
    try$(_expectSerialized(_driver, "outline-color", "gray", "gray"));
    try$(_expectSerialized(_driver, "outline-color", "green", "green"));
    try$(_expectSerialized(_driver, "outline-color", "lime", "lime"));
    try$(_expectSerialized(_driver, "outline-color", "maroon", "maroon"));
    try$(_expectSerialized(_driver, "outline-color", "navy", "navy"));
    try$(_expectSerialized(_driver, "outline-color", "olive", "olive"));
    try$(_expectSerialized(_driver, "outline-color", "orange", "orange"));
    try$(_expectSerialized(_driver, "outline-color", "purple", "purple"));
    try$(_expectSerialized(_driver, "outline-color", "red", "red"));
    try$(_expectSerialized(_driver, "outline-color", "silver", "silver"));
    try$(_expectSerialized(_driver, "outline-color", "teal", "teal"));
    try$(_expectSerialized(_driver, "outline-color", "transparent", "transparent"));
    try$(_expectSerialized(_driver, "outline-color", "white", "white"));
    try$(_expectSerialized(_driver, "outline-color", "yellow", "yellow"));
    try$(_expectInvalid(_driver, "outline-color", "#0000fg"));
    try$(_expectInvalid(_driver, "outline-color", "#00fg00"));
    try$(_expectInvalid(_driver, "outline-color", "#00g"));
    try$(_expectInvalid(_driver, "outline-color", "#0g0"));
    try$(_expectInvalid(_driver, "outline-color", "#fg0000"));
    try$(_expectInvalid(_driver, "outline-color", "#fgfgfg"));
    try$(_expectInvalid(_driver, "outline-color", "#g00"));
    try$(_expectInvalid(_driver, "outline-color", "#ggg"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-align/gaps/gap-parsing-002.html
test$("vaev-style-wpt-css-css-align-gaps-gap-parsing-002") {
    try$(_expectSerialized(_driver, "column-gap", "normal", "normal"));
    try$(_expectSerialized(_driver, "column-gap", "10px", "10px"));
    try$(_expectSerialized(_driver, "row-gap", "normal", "normal"));
    try$(_expectSerialized(_driver, "row-gap", "10px", "10px"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-align/parsing/align-content-invalid.html
test$("vaev-style-wpt-css-css-align-parsing-align-content-invalid") {
    try$(_expectInvalid(_driver, "align-content", "baseline last"));
    try$(_expectInvalid(_driver, "align-content", "center baseline"));
    try$(_expectInvalid(_driver, "align-content", "first"));
    try$(_expectInvalid(_driver, "align-content", "flex-start flex-end"));
    try$(_expectInvalid(_driver, "align-content", "flow"));
    try$(_expectInvalid(_driver, "align-content", "flow-start flow-end"));
    try$(_expectInvalid(_driver, "align-content", "flow-start safe"));
    try$(_expectInvalid(_driver, "align-content", "last"));
    try$(_expectInvalid(_driver, "align-content", "normal baseline"));
    try$(_expectInvalid(_driver, "align-content", "right legacy"));
    try$(_expectInvalid(_driver, "align-content", "self-end unsafe"));
    try$(_expectInvalid(_driver, "align-content", "space-between flow-end"));
    try$(_expectInvalid(_driver, "align-content", "start safe"));
    return Ok();
}

test$("vaev-style-wpt-css-css-align-parsing-align-content-invalid-xfail", XFAIL) {
    try$(_expectInvalid(_driver, "align-content", "auto"));
    try$(_expectInvalid(_driver, "align-content", "left"));
    try$(_expectInvalid(_driver, "align-content", "legacy center"));
    try$(_expectInvalid(_driver, "align-content", "legacy left"));
    try$(_expectInvalid(_driver, "align-content", "legacy"));
    try$(_expectInvalid(_driver, "align-content", "safe self-end"));
    try$(_expectInvalid(_driver, "align-content", "safe"));
    try$(_expectInvalid(_driver, "align-content", "self-start"));
    try$(_expectInvalid(_driver, "align-content", "unsafe right"));
    try$(_expectInvalid(_driver, "align-content", "unsafe"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-align/parsing/align-content-valid.html
test$("vaev-style-wpt-css-css-align-parsing-align-content-valid") {
    try$(_expectSerialized(_driver, "align-content", "normal", "normal"));
    try$(_expectSerialized(_driver, "align-content", "space-between", "space-between"));
    try$(_expectSerialized(_driver, "align-content", "space-around", "space-around"));
    try$(_expectSerialized(_driver, "align-content", "space-evenly", "space-evenly"));
    try$(_expectSerialized(_driver, "align-content", "stretch", "stretch"));
    try$(_expectSerialized(_driver, "align-content", "center", "center"));
    try$(_expectSerialized(_driver, "align-content", "start", "start"));
    try$(_expectSerialized(_driver, "align-content", "end", "end"));
    try$(_expectSerialized(_driver, "align-content", "flex-start", "flex-start"));
    try$(_expectSerialized(_driver, "align-content", "flex-end", "flex-end"));
    try$(_expectSerialized(_driver, "align-content", "unsafe end", "unsafe end"));
    try$(_expectSerialized(_driver, "align-content", "safe flex-start", "safe flex-start"));
    return Ok();
}

test$("vaev-style-wpt-css-css-align-parsing-align-content-valid-xfail", XFAIL) {
    try$(_expectSerialized(_driver, "align-content", "baseline", "baseline"));
    try$(_expectSerialized(_driver, "align-content", "first baseline", "baseline"));
    try$(_expectSerialized(_driver, "align-content", "last baseline", "last baseline"));
    try$(_expectSerialized(_driver, "align-content", "flow-start", "flow-start"));
    try$(_expectSerialized(_driver, "align-content", "flow-end", "flow-end"));
    try$(_expectSerialized(_driver, "align-content", "safe flow-start", "safe flow-start"));
    try$(_expectSerialized(_driver, "align-content", "unsafe flow-end", "unsafe flow-end"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-align/parsing/align-items-invalid.html
test$("vaev-style-wpt-css-css-align-parsing-align-items-invalid") {
    try$(_expectInvalid(_driver, "align-items", "baseline last"));
    try$(_expectInvalid(_driver, "align-items", "center baseline"));
    try$(_expectInvalid(_driver, "align-items", "first"));
    try$(_expectInvalid(_driver, "align-items", "flex-start flex-end"));
    try$(_expectInvalid(_driver, "align-items", "flow"));
    try$(_expectInvalid(_driver, "align-items", "flow-start flow-end"));
    try$(_expectInvalid(_driver, "align-items", "flow-start safe"));
    try$(_expectInvalid(_driver, "align-items", "last"));
    try$(_expectInvalid(_driver, "align-items", "normal baseline"));
    try$(_expectInvalid(_driver, "align-items", "right legacy"));
    try$(_expectInvalid(_driver, "align-items", "self-end unsafe"));
    try$(_expectInvalid(_driver, "align-items", "start safe"));
    try$(_expectInvalid(_driver, "align-items", "anchor-center"));
    return Ok();
}

test$("vaev-style-wpt-css-css-align-parsing-align-items-invalid-xfail", XFAIL) {
    try$(_expectInvalid(_driver, "align-items", "auto"));
    try$(_expectInvalid(_driver, "align-items", "left"));
    try$(_expectInvalid(_driver, "align-items", "legacy center"));
    try$(_expectInvalid(_driver, "align-items", "legacy left"));
    try$(_expectInvalid(_driver, "align-items", "legacy"));
    try$(_expectInvalid(_driver, "align-items", "safe"));
    try$(_expectInvalid(_driver, "align-items", "space-around"));
    try$(_expectInvalid(_driver, "align-items", "space-between"));
    try$(_expectInvalid(_driver, "align-items", "space-evenly"));
    try$(_expectInvalid(_driver, "align-items", "unsafe right"));
    try$(_expectInvalid(_driver, "align-items", "unsafe"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-align/parsing/align-items-valid.html
test$("vaev-style-wpt-css-css-align-parsing-align-items-valid") {
    try$(_expectSerialized(_driver, "align-items", "normal", "normal"));
    try$(_expectSerialized(_driver, "align-items", "stretch", "stretch"));
    try$(_expectSerialized(_driver, "align-items", "center", "center"));
    try$(_expectSerialized(_driver, "align-items", "start", "start"));
    try$(_expectSerialized(_driver, "align-items", "end", "end"));
    try$(_expectSerialized(_driver, "align-items", "self-start", "self-start"));
    try$(_expectSerialized(_driver, "align-items", "self-end", "self-end"));
    try$(_expectSerialized(_driver, "align-items", "flex-start", "flex-start"));
    try$(_expectSerialized(_driver, "align-items", "flex-end", "flex-end"));
    try$(_expectSerialized(_driver, "align-items", "unsafe center", "unsafe center"));
    try$(_expectSerialized(_driver, "align-items", "safe self-end", "safe self-end"));
    return Ok();
}

test$("vaev-style-wpt-css-css-align-parsing-align-items-valid-xfail", XFAIL) {
    try$(_expectSerialized(_driver, "align-items", "baseline", "baseline"));
    try$(_expectSerialized(_driver, "align-items", "first baseline", "baseline"));
    try$(_expectSerialized(_driver, "align-items", "last baseline", "last baseline"));
    try$(_expectSerialized(_driver, "align-items", "flow-start", "flow-start"));
    try$(_expectSerialized(_driver, "align-items", "flow-end", "flow-end"));
    try$(_expectSerialized(_driver, "align-items", "safe flow-start", "safe flow-start"));
    try$(_expectSerialized(_driver, "align-items", "unsafe flow-end", "unsafe flow-end"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-align/parsing/align-self-invalid.html
test$("vaev-style-wpt-css-css-align-parsing-align-self-invalid") {
    try$(_expectInvalid(_driver, "align-self", "baseline last"));
    try$(_expectInvalid(_driver, "align-self", "center baseline"));
    try$(_expectInvalid(_driver, "align-self", "first"));
    try$(_expectInvalid(_driver, "align-self", "flex-start flex-end"));
    try$(_expectInvalid(_driver, "align-self", "flow"));
    try$(_expectInvalid(_driver, "align-self", "flow-start flow-end"));
    try$(_expectInvalid(_driver, "align-self", "flow-start safe"));
    try$(_expectInvalid(_driver, "align-self", "last"));
    try$(_expectInvalid(_driver, "align-self", "normal baseline"));
    try$(_expectInvalid(_driver, "align-self", "right legacy"));
    try$(_expectInvalid(_driver, "align-self", "self-end unsafe"));
    try$(_expectInvalid(_driver, "align-self", "start safe"));
    return Ok();
}

test$("vaev-style-wpt-css-css-align-parsing-align-self-invalid-xfail", XFAIL) {
    try$(_expectInvalid(_driver, "align-self", "left"));
    try$(_expectInvalid(_driver, "align-self", "legacy center"));
    try$(_expectInvalid(_driver, "align-self", "legacy left"));
    try$(_expectInvalid(_driver, "align-self", "legacy"));
    try$(_expectInvalid(_driver, "align-self", "safe"));
    try$(_expectInvalid(_driver, "align-self", "space-around"));
    try$(_expectInvalid(_driver, "align-self", "space-between"));
    try$(_expectInvalid(_driver, "align-self", "space-evenly"));
    try$(_expectInvalid(_driver, "align-self", "unsafe right"));
    try$(_expectInvalid(_driver, "align-self", "unsafe"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-align/parsing/align-self-valid.html
test$("vaev-style-wpt-css-css-align-parsing-align-self-valid") {
    try$(_expectSerialized(_driver, "align-self", "auto", "auto"));
    try$(_expectSerialized(_driver, "align-self", "normal", "normal"));
    try$(_expectSerialized(_driver, "align-self", "stretch", "stretch"));
    try$(_expectSerialized(_driver, "align-self", "center", "center"));
    try$(_expectSerialized(_driver, "align-self", "start", "start"));
    try$(_expectSerialized(_driver, "align-self", "end", "end"));
    try$(_expectSerialized(_driver, "align-self", "self-start", "self-start"));
    try$(_expectSerialized(_driver, "align-self", "self-end", "self-end"));
    try$(_expectSerialized(_driver, "align-self", "flex-start", "flex-start"));
    try$(_expectSerialized(_driver, "align-self", "flex-end", "flex-end"));
    try$(_expectSerialized(_driver, "align-self", "unsafe center", "unsafe center"));
    try$(_expectSerialized(_driver, "align-self", "safe self-end", "safe self-end"));
    return Ok();
}

test$("vaev-style-wpt-css-css-align-parsing-align-self-valid-xfail", XFAIL) {
    try$(_expectSerialized(_driver, "align-self", "baseline", "baseline"));
    try$(_expectSerialized(_driver, "align-self", "first baseline", "baseline"));
    try$(_expectSerialized(_driver, "align-self", "last baseline", "last baseline"));
    try$(_expectSerialized(_driver, "align-self", "flow-start", "flow-start"));
    try$(_expectSerialized(_driver, "align-self", "flow-end", "flow-end"));
    try$(_expectSerialized(_driver, "align-self", "safe flow-start", "safe flow-start"));
    try$(_expectSerialized(_driver, "align-self", "unsafe flow-end", "unsafe flow-end"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-align/parsing/column-gap-invalid.html
test$("vaev-style-wpt-css-css-align-parsing-column-gap-invalid") {
    try$(_expectInvalid(_driver, "column-gap", "auto"));
    try$(_expectInvalid(_driver, "column-gap", "10"));
    try$(_expectInvalid(_driver, "column-gap", "10px 20px"));
    return Ok();
}

test$("vaev-style-wpt-css-css-align-parsing-column-gap-invalid-xfail", XFAIL) {
    try$(_expectInvalid(_driver, "column-gap", "-1px"));
    try$(_expectInvalid(_driver, "column-gap", "-10%"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-align/parsing/column-gap-valid.html
test$("vaev-style-wpt-css-css-align-parsing-column-gap-valid") {
    try$(_expectSerialized(_driver, "column-gap", "normal", "normal"));
    try$(_expectSerialized(_driver, "column-gap", "0", "0px"));
    try$(_expectSerialized(_driver, "column-gap", "1px", "1px"));
    try$(_expectSerialized(_driver, "column-gap", "calc(2em + 3ex)", "calc(2em + 3ex)"));
    try$(_expectSerialized(_driver, "column-gap", "4%", "4%"));
    try$(_expectSerialized(_driver, "column-gap", "5vmin", "5vmin"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-align/parsing/grid-column-gap-invalid.html
test$("vaev-style-wpt-css-css-align-parsing-grid-column-gap-invalid") {
    try$(_expectInvalid(_driver, "grid-column-gap", "auto"));
    try$(_expectInvalid(_driver, "grid-column-gap", "10"));
    try$(_expectInvalid(_driver, "grid-column-gap", "10px 20px"));
    return Ok();
}

test$("vaev-style-wpt-css-css-align-parsing-grid-column-gap-invalid-xfail", XFAIL) {
    try$(_expectInvalid(_driver, "grid-column-gap", "-1px"));
    try$(_expectInvalid(_driver, "grid-column-gap", "-10%"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-align/parsing/grid-column-gap-valid.html
test$("vaev-style-wpt-css-css-align-parsing-grid-column-gap-valid") {
    try$(_expectSerialized(_driver, "grid-column-gap", "normal", "normal"));
    try$(_expectSerialized(_driver, "grid-column-gap", "0", "0px"));
    try$(_expectSerialized(_driver, "grid-column-gap", "1px", "1px"));
    try$(_expectSerialized(_driver, "grid-column-gap", "calc(2em + 3ex)", "calc(2em + 3ex)"));
    try$(_expectSerialized(_driver, "grid-column-gap", "4%", "4%"));
    try$(_expectSerialized(_driver, "grid-column-gap", "5vmin", "5vmin"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-align/parsing/grid-row-gap-invalid.html
test$("vaev-style-wpt-css-css-align-parsing-grid-row-gap-invalid") {
    try$(_expectInvalid(_driver, "grid-row-gap", "auto"));
    try$(_expectInvalid(_driver, "grid-row-gap", "10px 20%"));
    try$(_expectInvalid(_driver, "grid-row-gap", "normal 10px"));
    return Ok();
}

test$("vaev-style-wpt-css-css-align-parsing-grid-row-gap-invalid-xfail", XFAIL) {
    try$(_expectInvalid(_driver, "grid-row-gap", "-10px"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-align/parsing/grid-row-gap-valid.html
test$("vaev-style-wpt-css-css-align-parsing-grid-row-gap-valid") {
    try$(_expectSerialized(_driver, "grid-row-gap", "normal", "normal"));
    try$(_expectSerialized(_driver, "grid-row-gap", "10px", "10px"));
    try$(_expectSerialized(_driver, "grid-row-gap", "20%", "20%"));
    try$(_expectSerialized(_driver, "grid-row-gap", "calc(20% + 10px)", "calc(20% + 10px)"));
    try$(_expectSerialized(_driver, "grid-row-gap", "0", "0px"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-align/parsing/justify-content-invalid.html
test$("vaev-style-wpt-css-css-align-parsing-justify-content-invalid") {
    try$(_expectInvalid(_driver, "justify-content", "baseline last"));
    try$(_expectInvalid(_driver, "justify-content", "baseline"));
    try$(_expectInvalid(_driver, "justify-content", "center baseline"));
    try$(_expectInvalid(_driver, "justify-content", "first baseline"));
    try$(_expectInvalid(_driver, "justify-content", "first"));
    try$(_expectInvalid(_driver, "justify-content", "flex-start flex-end"));
    try$(_expectInvalid(_driver, "justify-content", "flow"));
    try$(_expectInvalid(_driver, "justify-content", "flow-start flow-end"));
    try$(_expectInvalid(_driver, "justify-content", "flow-start safe"));
    try$(_expectInvalid(_driver, "justify-content", "last baseline"));
    try$(_expectInvalid(_driver, "justify-content", "last"));
    try$(_expectInvalid(_driver, "justify-content", "normal baseline"));
    try$(_expectInvalid(_driver, "justify-content", "right legacy"));
    try$(_expectInvalid(_driver, "justify-content", "self-end unsafe"));
    try$(_expectInvalid(_driver, "justify-content", "space-between flow-end"));
    try$(_expectInvalid(_driver, "justify-content", "start safe"));
    return Ok();
}

test$("vaev-style-wpt-css-css-align-parsing-justify-content-invalid-xfail", XFAIL) {
    try$(_expectInvalid(_driver, "justify-content", "auto"));
    try$(_expectInvalid(_driver, "justify-content", "legacy center"));
    try$(_expectInvalid(_driver, "justify-content", "legacy left"));
    try$(_expectInvalid(_driver, "justify-content", "legacy"));
    try$(_expectInvalid(_driver, "justify-content", "safe self-end"));
    try$(_expectInvalid(_driver, "justify-content", "safe"));
    try$(_expectInvalid(_driver, "justify-content", "self-start"));
    try$(_expectInvalid(_driver, "justify-content", "unsafe"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-align/parsing/justify-content-valid.html
test$("vaev-style-wpt-css-css-align-parsing-justify-content-valid") {
    try$(_expectSerialized(_driver, "justify-content", "normal", "normal"));
    try$(_expectSerialized(_driver, "justify-content", "space-between", "space-between"));
    try$(_expectSerialized(_driver, "justify-content", "space-around", "space-around"));
    try$(_expectSerialized(_driver, "justify-content", "space-evenly", "space-evenly"));
    try$(_expectSerialized(_driver, "justify-content", "stretch", "stretch"));
    try$(_expectSerialized(_driver, "justify-content", "center", "center"));
    try$(_expectSerialized(_driver, "justify-content", "start", "start"));
    try$(_expectSerialized(_driver, "justify-content", "end", "end"));
    try$(_expectSerialized(_driver, "justify-content", "flex-start", "flex-start"));
    try$(_expectSerialized(_driver, "justify-content", "flex-end", "flex-end"));
    try$(_expectSerialized(_driver, "justify-content", "unsafe end", "unsafe end"));
    try$(_expectSerialized(_driver, "justify-content", "safe flex-start", "safe flex-start"));
    try$(_expectSerialized(_driver, "justify-content", "left", "left"));
    try$(_expectSerialized(_driver, "justify-content", "unsafe right", "unsafe right"));
    return Ok();
}

test$("vaev-style-wpt-css-css-align-parsing-justify-content-valid-xfail", XFAIL) {
    try$(_expectSerialized(_driver, "justify-content", "flow-start", "flow-start"));
    try$(_expectSerialized(_driver, "justify-content", "flow-end", "flow-end"));
    try$(_expectSerialized(_driver, "justify-content", "safe flow-start", "safe flow-start"));
    try$(_expectSerialized(_driver, "justify-content", "unsafe flow-end", "unsafe flow-end"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-align/parsing/justify-items-invalid.html
test$("vaev-style-wpt-css-css-align-parsing-justify-items-invalid") {
    try$(_expectInvalid(_driver, "justify-items", "baseline last"));
    try$(_expectInvalid(_driver, "justify-items", "center baseline"));
    try$(_expectInvalid(_driver, "justify-items", "first"));
    try$(_expectInvalid(_driver, "justify-items", "flex-start flex-end"));
    try$(_expectInvalid(_driver, "justify-items", "flow"));
    try$(_expectInvalid(_driver, "justify-items", "flow-start flow-end"));
    try$(_expectInvalid(_driver, "justify-items", "flow-start safe"));
    try$(_expectInvalid(_driver, "justify-items", "last"));
    try$(_expectInvalid(_driver, "justify-items", "legacy flow-start"));
    try$(_expectInvalid(_driver, "justify-items", "normal baseline"));
    try$(_expectInvalid(_driver, "justify-items", "self-end unsafe"));
    try$(_expectInvalid(_driver, "justify-items", "start safe"));
    try$(_expectInvalid(_driver, "justify-items", "anchor-center"));
    return Ok();
}

test$("vaev-style-wpt-css-css-align-parsing-justify-items-invalid-xfail", XFAIL) {
    try$(_expectInvalid(_driver, "justify-items", "auto"));
    try$(_expectInvalid(_driver, "justify-items", "safe"));
    try$(_expectInvalid(_driver, "justify-items", "space-around"));
    try$(_expectInvalid(_driver, "justify-items", "space-between"));
    try$(_expectInvalid(_driver, "justify-items", "space-evenly"));
    try$(_expectInvalid(_driver, "justify-items", "unsafe"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-align/parsing/justify-items-valid.html
test$("vaev-style-wpt-css-css-align-parsing-justify-items-valid") {
    try$(_expectSerialized(_driver, "justify-items", "normal", "normal"));
    try$(_expectSerialized(_driver, "justify-items", "stretch", "stretch"));
    try$(_expectSerialized(_driver, "justify-items", "center", "center"));
    try$(_expectSerialized(_driver, "justify-items", "start", "start"));
    try$(_expectSerialized(_driver, "justify-items", "end", "end"));
    try$(_expectSerialized(_driver, "justify-items", "self-start", "self-start"));
    try$(_expectSerialized(_driver, "justify-items", "self-end", "self-end"));
    try$(_expectSerialized(_driver, "justify-items", "flex-start", "flex-start"));
    try$(_expectSerialized(_driver, "justify-items", "flex-end", "flex-end"));
    try$(_expectSerialized(_driver, "justify-items", "unsafe center", "unsafe center"));
    try$(_expectSerialized(_driver, "justify-items", "safe self-end", "safe self-end"));
    try$(_expectSerialized(_driver, "justify-items", "right", "right"));
    try$(_expectSerialized(_driver, "justify-items", "safe left", "safe left"));
    try$(_expectSerialized(_driver, "justify-items", "legacy", "legacy"));
    return Ok();
}

test$("vaev-style-wpt-css-css-align-parsing-justify-items-valid-xfail", XFAIL) {
    try$(_expectSerialized(_driver, "justify-items", "baseline", "baseline"));
    try$(_expectSerialized(_driver, "justify-items", "first baseline", "baseline"));
    try$(_expectSerialized(_driver, "justify-items", "last baseline", "last baseline"));
    try$(_expectSerialized(_driver, "justify-items", "flow-start", "flow-start"));
    try$(_expectSerialized(_driver, "justify-items", "flow-end", "flow-end"));
    try$(_expectSerialized(_driver, "justify-items", "safe flow-start", "safe flow-start"));
    try$(_expectSerialized(_driver, "justify-items", "unsafe flow-end", "unsafe flow-end"));
    try$(_expectSerialized(_driver, "justify-items", "left legacy", "legacy left"));
    try$(_expectSerialized(_driver, "justify-items", "right legacy", "legacy right"));
    try$(_expectSerialized(_driver, "justify-items", "center legacy", "legacy center"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-align/parsing/justify-self-invalid.html
test$("vaev-style-wpt-css-css-align-parsing-justify-self-invalid") {
    try$(_expectInvalid(_driver, "justify-self", "baseline last"));
    try$(_expectInvalid(_driver, "justify-self", "center baseline"));
    try$(_expectInvalid(_driver, "justify-self", "first"));
    try$(_expectInvalid(_driver, "justify-self", "flex-start flex-end"));
    try$(_expectInvalid(_driver, "justify-self", "flow"));
    try$(_expectInvalid(_driver, "justify-self", "flow-start flow-end"));
    try$(_expectInvalid(_driver, "justify-self", "flow-start safe"));
    try$(_expectInvalid(_driver, "justify-self", "last"));
    try$(_expectInvalid(_driver, "justify-self", "normal baseline"));
    try$(_expectInvalid(_driver, "justify-self", "right legacy"));
    try$(_expectInvalid(_driver, "justify-self", "self-end unsafe"));
    try$(_expectInvalid(_driver, "justify-self", "start safe"));
    return Ok();
}

test$("vaev-style-wpt-css-css-align-parsing-justify-self-invalid-xfail", XFAIL) {
    try$(_expectInvalid(_driver, "justify-self", "legacy center"));
    try$(_expectInvalid(_driver, "justify-self", "legacy left"));
    try$(_expectInvalid(_driver, "justify-self", "legacy"));
    try$(_expectInvalid(_driver, "justify-self", "safe"));
    try$(_expectInvalid(_driver, "justify-self", "space-around"));
    try$(_expectInvalid(_driver, "justify-self", "space-between"));
    try$(_expectInvalid(_driver, "justify-self", "space-evenly"));
    try$(_expectInvalid(_driver, "justify-self", "unsafe"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-align/parsing/justify-self-valid.html
test$("vaev-style-wpt-css-css-align-parsing-justify-self-valid") {
    try$(_expectSerialized(_driver, "justify-self", "auto", "auto"));
    try$(_expectSerialized(_driver, "justify-self", "normal", "normal"));
    try$(_expectSerialized(_driver, "justify-self", "stretch", "stretch"));
    try$(_expectSerialized(_driver, "justify-self", "center", "center"));
    try$(_expectSerialized(_driver, "justify-self", "start", "start"));
    try$(_expectSerialized(_driver, "justify-self", "end", "end"));
    try$(_expectSerialized(_driver, "justify-self", "self-start", "self-start"));
    try$(_expectSerialized(_driver, "justify-self", "self-end", "self-end"));
    try$(_expectSerialized(_driver, "justify-self", "flex-start", "flex-start"));
    try$(_expectSerialized(_driver, "justify-self", "flex-end", "flex-end"));
    try$(_expectSerialized(_driver, "justify-self", "unsafe center", "unsafe center"));
    try$(_expectSerialized(_driver, "justify-self", "safe self-end", "safe self-end"));
    try$(_expectSerialized(_driver, "justify-self", "left", "left"));
    try$(_expectSerialized(_driver, "justify-self", "unsafe right", "unsafe right"));
    return Ok();
}

test$("vaev-style-wpt-css-css-align-parsing-justify-self-valid-xfail", XFAIL) {
    try$(_expectSerialized(_driver, "justify-self", "baseline", "baseline"));
    try$(_expectSerialized(_driver, "justify-self", "first baseline", "baseline"));
    try$(_expectSerialized(_driver, "justify-self", "last baseline", "last baseline"));
    try$(_expectSerialized(_driver, "justify-self", "flow-start", "flow-start"));
    try$(_expectSerialized(_driver, "justify-self", "flow-end", "flow-end"));
    try$(_expectSerialized(_driver, "justify-self", "safe flow-start", "safe flow-start"));
    try$(_expectSerialized(_driver, "justify-self", "unsafe flow-end", "unsafe flow-end"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-align/parsing/row-gap-invalid.html
test$("vaev-style-wpt-css-css-align-parsing-row-gap-invalid") {
    try$(_expectInvalid(_driver, "row-gap", "auto"));
    try$(_expectInvalid(_driver, "row-gap", "10px 20%"));
    try$(_expectInvalid(_driver, "row-gap", "normal 10px"));
    return Ok();
}

test$("vaev-style-wpt-css-css-align-parsing-row-gap-invalid-xfail", XFAIL) {
    try$(_expectInvalid(_driver, "row-gap", "-10px"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-align/parsing/row-gap-valid.html
test$("vaev-style-wpt-css-css-align-parsing-row-gap-valid") {
    try$(_expectSerialized(_driver, "row-gap", "normal", "normal"));
    try$(_expectSerialized(_driver, "row-gap", "10px", "10px"));
    try$(_expectSerialized(_driver, "row-gap", "20%", "20%"));
    try$(_expectSerialized(_driver, "row-gap", "calc(20% + 10px)", "calc(20% + 10px)"));
    try$(_expectSerialized(_driver, "row-gap", "0", "0px"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-anchor-position/anchor-parse-invalid.html
test$("vaev-style-wpt-css-css-anchor-position-anchor-parse-invalid") {
    try$(_expectInvalid(_driver, "margin-top", "anchor(--foo top)"));
    try$(_expectInvalid(_driver, "height", "anchor(--foo top)"));
    try$(_expectInvalid(_driver, "font-size", "anchor(--foo top)"));
    try$(_expectInvalid(_driver, "top", "anchor(--foo, top)"));
    try$(_expectInvalid(_driver, "top", "anchor(--foo top,)"));
    try$(_expectInvalid(_driver, "top", "anchor(--foo top bottom)"));
    try$(_expectInvalid(_driver, "top", "anchor(--foo top, 10px 20%)"));
    try$(_expectInvalid(_driver, "top", "anchor(--foo top, 10px, 20%)"));
    try$(_expectInvalid(_driver, "top", "anchor(2 * 20%)"));
    try$(_expectInvalid(_driver, "top", "anchor((2 * 20%))"));
    try$(_expectInvalid(_driver, "top", "anchor(foo top)"));
    try$(_expectInvalid(_driver, "top", "anchor(top foo)"));
    try$(_expectInvalid(_driver, "top", "anchor(--foo height)"));
    try$(_expectInvalid(_driver, "top", "anchor(--foo 10em)"));
    try$(_expectInvalid(_driver, "top", "anchor(--foo 100s)"));
    try$(_expectInvalid(_driver, "top", "anchor(--foo top, 1)"));
    try$(_expectInvalid(_driver, "top", "anchor(--foo top, 100s)"));
    try$(_expectInvalid(_driver, "top", "anchor(--foo top, bottom)"));
    try$(_expectInvalid(_driver, "top", "anchor(--foo top, anchor(bar top))"));
    try$(_expectInvalid(_driver, "top", "anchor(--foo top, anchor-size(bar height))"));
    try$(_expectInvalid(_driver, "top", "anchor(--foo top, auto"));
    try$(_expectInvalid(_driver, "top", "calc(anchor(foo top) + 10px + 10%)"));
    try$(_expectInvalid(_driver, "top", "min(anchor(--foo top), anchor(--bar bottom), anchor-size(baz height))"));
    try$(_expectInvalid(_driver, "top", "calc(anchor(--foo top, 1) + 1px)"));
    return Ok();
}

test$("vaev-style-wpt-css-css-anchor-position-anchor-parse-invalid-xfail", XFAIL) {
    try$(_expectInvalid(_driver, "top", "calc(10px + 100 * anchor(--foo top, anchor(bar bottom)))"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-anchor-position/anchor-size-parse-invalid.html
test$("vaev-style-wpt-css-css-anchor-position-anchor-size-parse-invalid") {
    try$(_expectInvalid(_driver, "font-size", "anchor-size(--foo width)"));
    try$(_expectInvalid(_driver, "width", "anchor-size(--foo, width)"));
    try$(_expectInvalid(_driver, "width", "anchor-size(--foo width,)"));
    try$(_expectInvalid(_driver, "width", "anchor-size(--foo width height)"));
    try$(_expectInvalid(_driver, "width", "anchor-size(--foo width, 10px 20%)"));
    try$(_expectInvalid(_driver, "width", "anchor-size(--foo width, 10px, 20%)"));
    try$(_expectInvalid(_driver, "width", "anchor-size(--foo,)"));
    try$(_expectInvalid(_driver, "width", "anchor-size(, 10px)"));
    try$(_expectInvalid(_driver, "width", "anchor-size(foo width)"));
    try$(_expectInvalid(_driver, "width", "anchor-size(--foo top)"));
    try$(_expectInvalid(_driver, "width", "anchor-size(--foo 10em)"));
    try$(_expectInvalid(_driver, "width", "anchor-size(--foo 100s)"));
    try$(_expectInvalid(_driver, "width", "anchor-size(--foo 50%)"));
    try$(_expectInvalid(_driver, "width", "anchor-size(--foo width, 1)"));
    try$(_expectInvalid(_driver, "width", "anchor-size(--foo width, 100s)"));
    try$(_expectInvalid(_driver, "width", "anchor-size(--foo width, height)"));
    try$(_expectInvalid(_driver, "width", "anchor-size(--foo width, anchor-size(bar width))"));
    try$(_expectInvalid(_driver, "width", "anchor-size(--foo width, anchor(--bar top))"));
    try$(_expectInvalid(_driver, "width", "calc(anchor-size(foo width) + 10px + 10%)"));
    try$(_expectInvalid(_driver, "width", "min(anchor-size(--foo width), anchor-size(--bar height), anchor(--baz top))"));
    try$(_expectInvalid(_driver, "top", "calc(anchor-size(--foo width, 1) + 1px)"));
    return Ok();
}

test$("vaev-style-wpt-css-css-anchor-position-anchor-size-parse-invalid-xfail", XFAIL) {
    try$(_expectInvalid(_driver, "width", "calc(10px + 100 * anchor-size(--foo width, anchor-size(bar bottom)))"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-backgrounds/parsing/background-attachment-invalid.html
test$("vaev-style-wpt-css-css-backgrounds-parsing-background-attachment-invalid") {
    try$(_expectInvalid(_driver, "background-attachment", "auto"));
    try$(_expectInvalid(_driver, "background-attachment", "local, none"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-backgrounds/parsing/background-color-invalid.html
test$("vaev-style-wpt-css-css-backgrounds-parsing-background-color-invalid") {
    try$(_expectInvalid(_driver, "background-color", "none"));
    try$(_expectInvalid(_driver, "background-color", "black white"));
    try$(_expectInvalid(_driver, "background-color", "black, white"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-backgrounds/parsing/background-color-valid.html
test$("vaev-style-wpt-css-css-backgrounds-parsing-background-color-valid") {
    try$(_expectSerialized(_driver, "background-color", "#00FF00", "rgb(0, 255, 0)"));
    try$(_expectSerialized(_driver, "background-color", "rgb(0, 0, 255)", "rgb(0, 0, 255)"));
    try$(_expectSerialized(_driver, "background-color", "rgb(100%, 100%, 0%)", "rgb(255, 255, 0)"));
    try$(_expectSerializedOneOf(_driver, "background-color", "hsl(120, 100%, 50%)", {"rgb(0, 255, 0)", "hsl(120, 100%, 50%)"}));
    return Ok();
}

test$("vaev-style-wpt-css-css-backgrounds-parsing-background-color-valid-xfail", XFAIL) {
    try$(_expectSerialized(_driver, "background-color", "currentcolor", "currentcolor"));
    try$(_expectSerialized(_driver, "background-color", "currentColor", "currentcolor"));
    try$(_expectSerialized(_driver, "background-color", "red", "red"));
    try$(_expectSerialized(_driver, "background-color", "teal", "teal"));
    try$(_expectSerialized(_driver, "background-color", "transparent", "transparent"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-backgrounds/parsing/background-image-invalid.html
test$("vaev-style-wpt-css-css-backgrounds-parsing-background-image-invalid") {
    try$(_expectInvalid(_driver, "background-image", "none, auto"));
    try$(_expectInvalid(_driver, "background-image", "radial-gradient(circle -10px at center, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "repeating-radial-gradient(-10px at center, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "radial-gradient(ellipse -20px 30px at center, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "repeating-radial-gradient(-20% 30% at center, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "radial-gradient(20px -30px at center, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "repeating-radial-gradient(20px -30px ellipse at center, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "cross-fade(auto blue, 50% red)"));
    try$(_expectInvalid(_driver, "background-image", "cross-fade(1px red, green)"));
    try$(_expectInvalid(_driver, "background-image", "cross-fade(calc(1% + 1px) red, green)"));
    try$(_expectInvalid(_driver, "background-image", "cross-fade(-1% red, green)"));
    try$(_expectInvalid(_driver, "background-image", "cross-fade(101% red, green)"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-backgrounds/parsing/background-invalid.html
test$("vaev-style-wpt-css-css-backgrounds-parsing-background-invalid") {
    try$(_expectInvalid(_driver, "background", "red, green"));
    try$(_expectInvalid(_driver, "background", "black 0 url(https://example.invalid/) / cover"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-backgrounds/parsing/background-position-invalid.html
test$("vaev-style-wpt-css-css-backgrounds-parsing-background-position-invalid") {
    try$(_expectInvalid(_driver, "background-position", "left right"));
    try$(_expectInvalid(_driver, "background-position", "top bottom"));
    try$(_expectInvalid(_driver, "background-position", "1% center 2px"));
    try$(_expectInvalid(_driver, "background-position", "right 7% 50%"));
    try$(_expectInvalid(_driver, "background-position", "50% top 8px"));
    try$(_expectInvalid(_driver, "background-position", "left 10px 50%"));
    try$(_expectInvalid(_driver, "background-position", "right 11% 100%"));
    try$(_expectInvalid(_driver, "background-position", "100% top 14%"));
    try$(_expectInvalid(_driver, "background-position", "50% top 15px"));
    try$(_expectInvalid(_driver, "background-position", "0% bottom 16%"));
    try$(_expectInvalid(_driver, "background-position", "right 19% 100%"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-backgrounds/parsing/background-repeat-invalid.html
test$("vaev-style-wpt-css-css-backgrounds-parsing-background-repeat-invalid") {
    try$(_expectInvalid(_driver, "background-repeat", "repeat repeat-x"));
    try$(_expectInvalid(_driver, "background-repeat", "repeat-y round"));
    try$(_expectInvalid(_driver, "background-repeat", "repeat space round"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-backgrounds/parsing/border-color-invalid.html
test$("vaev-style-wpt-css-css-backgrounds-parsing-border-color-invalid") {
    try$(_expectInvalid(_driver, "border-color", "auto"));
    try$(_expectInvalid(_driver, "border-color", "black, white"));
    try$(_expectInvalid(_driver, "border-color", "black white red green blue"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-backgrounds/parsing/border-invalid.html
test$("vaev-style-wpt-css-css-backgrounds-parsing-border-invalid") {
    try$(_expectInvalid(_driver, "border", "auto"));
    return Ok();
}

test$("vaev-style-wpt-css-css-backgrounds-parsing-border-invalid-xfail", XFAIL) {
    try$(_expectInvalid(_driver, "border", "thin double green 1px"));
    try$(_expectInvalid(_driver, "border", "-2em"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-backgrounds/parsing/border-radius-invalid.html
test$("vaev-style-wpt-css-css-backgrounds-parsing-border-radius-invalid") {
    try$(_expectInvalid(_driver, "border-radius", "auto"));
    try$(_expectInvalid(_driver, "border-radius", "none"));
    try$(_expectInvalid(_driver, "border-radius", "1px 2px 3px 4px 5px"));
    try$(_expectInvalid(_driver, "border-radius", "1px / 2px / 3px"));
    try$(_expectInvalid(_driver, "border-radius", "4 / 5"));
    try$(_expectInvalid(_driver, "border-radius", "5em 1px 5em 2% 5em 3px 5em 4%"));
    try$(_expectInvalid(_driver, "border-radius", "1px 5em 2% 5em 3px 5em 4% 5em"));
    try$(_expectInvalid(_driver, "border-top-left-radius", "10px 20px 30px"));
    return Ok();
}

test$("vaev-style-wpt-css-css-backgrounds-parsing-border-radius-invalid-xfail", XFAIL) {
    try$(_expectInvalid(_driver, "border-radius", "-1px"));
    try$(_expectInvalid(_driver, "border-radius", "1px -2px"));
    try$(_expectInvalid(_driver, "border-radius", "1em /"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-backgrounds/parsing/border-radius-valid.html
test$("vaev-style-wpt-css-css-backgrounds-parsing-border-radius-valid") {
    try$(_expectSerialized(_driver, "border-radius", "initial", "initial", RegisteredPropertySet::TOP_LEVEL));
    try$(_expectSerialized(_driver, "border-radius", "inherit", "inherit", RegisteredPropertySet::TOP_LEVEL));
    try$(_expectSerialized(_driver, "border-radius", "unset", "unset", RegisteredPropertySet::TOP_LEVEL));
    try$(_expectSerialized(_driver, "border-radius", "revert", "revert", RegisteredPropertySet::TOP_LEVEL));
    try$(_expectSerialized(_driver, "border-bottom-right-radius", "30px 40%", "30px 40%"));
    try$(_expectSerialized(_driver, "border-bottom-left-radius", "50% 60px", "50% 60px"));
    return Ok();
}

test$("vaev-style-wpt-css-css-backgrounds-parsing-border-radius-valid-xfail", XFAIL) {
    try$(_expectSerialized(_driver, "border-radius", "1px", "1px"));
    try$(_expectSerialized(_driver, "border-radius", "1px 5%", "1px 5%"));
    try$(_expectSerialized(_driver, "border-radius", "1px 2% 3px", "1px 2% 3px"));
    try$(_expectSerialized(_driver, "border-radius", "1px 2% 3px 4%", "1px 2% 3px 4%"));
    try$(_expectSerialized(_driver, "border-radius", "1px / 2px", "1px / 2px"));
    try$(_expectSerialized(_driver, "border-radius", "5em / 1px 2% 3px 4%", "5em / 1px 2% 3px 4%"));
    try$(_expectSerialized(_driver, "border-radius", "1px 2% / 3px 4px", "1px 2% / 3px 4px"));
    try$(_expectSerialized(_driver, "border-radius", "1px 2px 3em / 1px 2px 3%", "1px 2px 3em / 1px 2px 3%"));
    try$(_expectSerialized(_driver, "border-radius", "1px 2% / 2px 3em 4px 5em", "1px 2% / 2px 3em 4px 5em"));
    try$(_expectSerialized(_driver, "border-radius", "1px 2% 3px 4% / 5em", "1px 2% 3px 4% / 5em"));
    try$(_expectSerialized(_driver, "border-radius", "1px 1px 1px 2% / 1px 2% 1px 2%", "1px 1px 1px 2% / 1px 2%"));
    try$(_expectSerialized(_driver, "border-radius", "1px 1px 1px 1px / 1px 1px 2% 1px", "1px / 1px 1px 2%"));
    try$(_expectSerialized(_driver, "border-radius", "1px 1px 2% 2%", "1px 1px 2% 2%"));
    try$(_expectSerialized(_driver, "border-radius", "1px 2% 1px 1px", "1px 2% 1px 1px"));
    try$(_expectSerialized(_driver, "border-radius", "1px 2% 2% 2% / 1px 2% 3px 2%", "1px 2% 2% / 1px 2% 3px"));
    try$(_expectSerialized(_driver, "border-top-left-radius", "10px", "10px"));
    try$(_expectSerialized(_driver, "border-top-right-radius", "20%", "20%"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-backgrounds/parsing/border-shorthand.html
test$("vaev-style-wpt-css-css-backgrounds-parsing-border-shorthand") {
    try$(_expectInvalid(_driver, "border", "2px solid color-mix(42deg)"));
    try$(_expectInvalid(_driver, "border", "2px solid color-contrast(42deg)"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-backgrounds/parsing/border-style-invalid.html
test$("vaev-style-wpt-css-css-backgrounds-parsing-border-style-invalid") {
    try$(_expectInvalid(_driver, "border-style", "auto"));
    try$(_expectInvalid(_driver, "border-style", "solid double groove ridge none"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-backgrounds/parsing/border-style-valid.html
test$("vaev-style-wpt-css-css-backgrounds-parsing-border-style-valid") {
    try$(_expectSerialized(_driver, "border-top-style", "solid", "solid"));
    try$(_expectSerialized(_driver, "border-right-style", "double", "double"));
    try$(_expectSerialized(_driver, "border-bottom-style", "groove", "groove"));
    try$(_expectSerialized(_driver, "border-left-style", "ridge", "ridge"));
    return Ok();
}

test$("vaev-style-wpt-css-css-backgrounds-parsing-border-style-valid-xfail", XFAIL) {
    try$(_expectSerialized(_driver, "border-style", "none", "none"));
    try$(_expectSerialized(_driver, "border-style", "none hidden dotted dashed", "none hidden dotted dashed"));
    try$(_expectSerialized(_driver, "border-style", "solid double groove ridge", "solid double groove ridge"));
    try$(_expectSerialized(_driver, "border-style", "inset outset", "inset outset"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-backgrounds/parsing/border-width-invalid.html
test$("vaev-style-wpt-css-css-backgrounds-parsing-border-width-invalid") {
    try$(_expectInvalid(_driver, "border-width", "none"));
    try$(_expectInvalid(_driver, "border-width", "thin medium thick medium thin"));
    return Ok();
}

test$("vaev-style-wpt-css-css-backgrounds-parsing-border-width-invalid-xfail", XFAIL) {
    try$(_expectInvalid(_driver, "border-width", "-1px"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-backgrounds/parsing/border-width-valid.html
test$("vaev-style-wpt-css-css-backgrounds-parsing-border-width-valid") {
    try$(_expectSerialized(_driver, "border-top-width", "0px", "0px"));
    try$(_expectSerialized(_driver, "border-right-width", "thin", "thin"));
    try$(_expectSerialized(_driver, "border-left-width", "thick", "thick"));
    return Ok();
}

test$("vaev-style-wpt-css-css-backgrounds-parsing-border-width-valid-xfail", XFAIL) {
    try$(_expectSerialized(_driver, "border-width", "1em", "1em"));
    try$(_expectSerialized(_driver, "border-width", "2px thin medium thick", "2px thin medium thick"));
    try$(_expectSerialized(_driver, "border-bottom-width", "medium", "medium"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-box/parsing/clear-invalid.html
test$("vaev-style-wpt-css-css-box-parsing-clear-invalid") {
    try$(_expectInvalid(_driver, "clear", "auto"));
    try$(_expectInvalid(_driver, "clear", "left right"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-box/parsing/clear-valid.html
test$("vaev-style-wpt-css-css-box-parsing-clear-valid") {
    try$(_expectSerialized(_driver, "clear", "none", "none"));
    try$(_expectSerialized(_driver, "clear", "left", "left"));
    try$(_expectSerialized(_driver, "clear", "right", "right"));
    try$(_expectSerialized(_driver, "clear", "both", "both"));
    try$(_expectSerialized(_driver, "clear", "inline-start", "inline-start"));
    try$(_expectSerialized(_driver, "clear", "inline-end", "inline-end"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-box/parsing/float-invalid.html
test$("vaev-style-wpt-css-css-box-parsing-float-invalid") {
    try$(_expectInvalid(_driver, "float", "auto"));
    try$(_expectInvalid(_driver, "float", "right bottom"));
    try$(_expectInvalid(_driver, "float", "top, left"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-box/parsing/float-valid.html
test$("vaev-style-wpt-css-css-box-parsing-float-valid") {
    try$(_expectSerialized(_driver, "float", "left", "left"));
    try$(_expectSerialized(_driver, "float", "right", "right"));
    try$(_expectSerialized(_driver, "float", "none", "none"));
    try$(_expectSerialized(_driver, "float", "inline-start", "inline-start"));
    try$(_expectSerialized(_driver, "float", "inline-end", "inline-end"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-box/parsing/height-invalid.html
test$("vaev-style-wpt-css-css-box-parsing-height-invalid") {
    try$(_expectInvalid(_driver, "height", "none"));
    try$(_expectInvalid(_driver, "height", "min-content available"));
    try$(_expectInvalid(_driver, "height", "max-content 10px"));
    try$(_expectInvalid(_driver, "height", "20% available"));
    try$(_expectInvalid(_driver, "height", "60"));
    try$(_expectInvalid(_driver, "height", "10px 20%"));
    try$(_expectInvalid(_driver, "height", "10px border-box"));
    try$(_expectInvalid(_driver, "height", "content-box 20%"));
    return Ok();
}

test$("vaev-style-wpt-css-css-box-parsing-height-invalid-xfail", XFAIL) {
    try$(_expectInvalid(_driver, "height", "-10px"));
    try$(_expectInvalid(_driver, "height", "-20%"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-box/parsing/height-valid.html
test$("vaev-style-wpt-css-css-box-parsing-height-valid") {
    try$(_expectSerialized(_driver, "height", "auto", "auto"));
    try$(_expectSerialized(_driver, "height", "10px", "10px"));
    try$(_expectSerialized(_driver, "height", "20%", "20%"));
    try$(_expectSerialized(_driver, "height", "calc(2em + 3ex)", "calc(2em + 3ex)"));
    try$(_expectSerialized(_driver, "height", "min-content", "min-content"));
    try$(_expectSerialized(_driver, "height", "max-content", "max-content"));
    try$(_expectSerialized(_driver, "height", "fit-content", "fit-content"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-box/parsing/margin-invalid.html
test$("vaev-style-wpt-css-css-box-parsing-margin-invalid") {
    try$(_expectInvalid(_driver, "margin", "available"));
    try$(_expectInvalid(_driver, "margin", "10px border-box"));
    try$(_expectInvalid(_driver, "margin", "1% 2% 3% 4% 5%"));
    try$(_expectInvalid(_driver, "margin-top", "calc(2em + 3ex) auto"));
    try$(_expectInvalid(_driver, "margin-right", "auto calc(2em + 3ex) 20%"));
    try$(_expectInvalid(_driver, "margin-bottom", "10px 20% calc(2em + 3ex) auto"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-box/parsing/margin-valid.html
test$("vaev-style-wpt-css-css-box-parsing-margin-valid") {
    try$(_expectSerialized(_driver, "margin-top", "10px", "10px"));
    try$(_expectSerialized(_driver, "margin-right", "20%", "20%"));
    try$(_expectSerialized(_driver, "margin-bottom", "calc(2em + 3ex)", "calc(2em + 3ex)"));
    try$(_expectSerialized(_driver, "margin-left", "auto", "auto"));
    try$(_expectSerialized(_driver, "margin-top", "-10px", "-10px"));
    try$(_expectSerialized(_driver, "margin-right", "-20%", "-20%"));
    return Ok();
}

test$("vaev-style-wpt-css-css-box-parsing-margin-valid-xfail", XFAIL) {
    try$(_expectSerialized(_driver, "margin", "10px", "10px"));
    try$(_expectSerialized(_driver, "margin", "20%", "20%"));
    try$(_expectSerialized(_driver, "margin", "calc(2em + 3ex)", "calc(2em + 3ex)"));
    try$(_expectSerialized(_driver, "margin", "auto", "auto"));
    try$(_expectSerialized(_driver, "margin", "-10px", "-10px"));
    try$(_expectSerialized(_driver, "margin", "-20%", "-20%"));
    try$(_expectSerialized(_driver, "margin", "calc(2em + 3ex) auto", "calc(2em + 3ex) auto"));
    try$(_expectSerialized(_driver, "margin", "auto calc(2em + 3ex) 20%", "auto calc(2em + 3ex) 20%"));
    try$(_expectSerialized(_driver, "margin", "10px 20% calc(2em + 3ex) auto", "10px 20% calc(2em + 3ex) auto"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-box/parsing/max-height-invalid.html
test$("vaev-style-wpt-css-css-box-parsing-max-height-invalid") {
    try$(_expectInvalid(_driver, "max-height", "complex"));
    try$(_expectInvalid(_driver, "max-height", "auto"));
    try$(_expectInvalid(_driver, "max-height", "none available"));
    try$(_expectInvalid(_driver, "max-height", "max-content 10px"));
    try$(_expectInvalid(_driver, "max-height", "20% available"));
    try$(_expectInvalid(_driver, "max-height", "60"));
    try$(_expectInvalid(_driver, "max-height", "10px 20%"));
    try$(_expectInvalid(_driver, "max-height", "content-box border-box"));
    try$(_expectInvalid(_driver, "max-height", "10px border-box 20%"));
    try$(_expectInvalid(_driver, "max-height", "content-box 20% border-box"));
    return Ok();
}

test$("vaev-style-wpt-css-css-box-parsing-max-height-invalid-xfail", XFAIL) {
    try$(_expectInvalid(_driver, "max-height", "-10px"));
    try$(_expectInvalid(_driver, "max-height", "-20%"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-box/parsing/max-height-valid.html
test$("vaev-style-wpt-css-css-box-parsing-max-height-valid") {
    try$(_expectSerialized(_driver, "max-height", "none", "none"));
    try$(_expectSerialized(_driver, "max-height", "10px", "10px"));
    try$(_expectSerialized(_driver, "max-height", "20%", "20%"));
    try$(_expectSerialized(_driver, "max-height", "calc(2em + 3ex)", "calc(2em + 3ex)"));
    try$(_expectSerialized(_driver, "max-height", "min-content", "min-content"));
    try$(_expectSerialized(_driver, "max-height", "max-content", "max-content"));
    try$(_expectSerialized(_driver, "max-height", "fit-content", "fit-content"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-box/parsing/max-width-invalid.html
test$("vaev-style-wpt-css-css-box-parsing-max-width-invalid") {
    try$(_expectInvalid(_driver, "max-width", "complex"));
    try$(_expectInvalid(_driver, "max-width", "auto"));
    try$(_expectInvalid(_driver, "max-width", "none available"));
    try$(_expectInvalid(_driver, "max-width", "max-content 10px"));
    try$(_expectInvalid(_driver, "max-width", "20% available"));
    try$(_expectInvalid(_driver, "max-width", "60"));
    try$(_expectInvalid(_driver, "max-width", "10px 20%"));
    try$(_expectInvalid(_driver, "max-width", "content-box border-box"));
    try$(_expectInvalid(_driver, "max-width", "10px border-box 20%"));
    try$(_expectInvalid(_driver, "max-width", "content-box 20% border-box"));
    return Ok();
}

test$("vaev-style-wpt-css-css-box-parsing-max-width-invalid-xfail", XFAIL) {
    try$(_expectInvalid(_driver, "max-width", "-10px"));
    try$(_expectInvalid(_driver, "max-width", "-20%"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-box/parsing/max-width-valid.html
test$("vaev-style-wpt-css-css-box-parsing-max-width-valid") {
    try$(_expectSerialized(_driver, "max-width", "none", "none"));
    try$(_expectSerialized(_driver, "max-width", "10px", "10px"));
    try$(_expectSerialized(_driver, "max-width", "20%", "20%"));
    try$(_expectSerialized(_driver, "max-width", "calc(2em + 3ex)", "calc(2em + 3ex)"));
    try$(_expectSerialized(_driver, "max-width", "min-content", "min-content"));
    try$(_expectSerialized(_driver, "max-width", "max-content", "max-content"));
    try$(_expectSerialized(_driver, "max-width", "fit-content", "fit-content"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-box/parsing/overflow-invalid.html
test$("vaev-style-wpt-css-css-box-parsing-overflow-invalid") {
    try$(_expectInvalid(_driver, "overflow", "none"));
    try$(_expectInvalid(_driver, "overflow", "visible hidden scroll"));
    try$(_expectInvalid(_driver, "overflow", "no-display, no-content"));
    try$(_expectInvalid(_driver, "overflow-x", "none"));
    try$(_expectInvalid(_driver, "overflow-y", "visible hidden"));
    try$(_expectInvalid(_driver, "overflow-y", "no-display, no-content"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-box/parsing/overflow-valid.html
test$("vaev-style-wpt-css-css-box-parsing-overflow-valid") {
    try$(_expectSerialized(_driver, "overflow-x", "visible", "visible"));
    try$(_expectSerialized(_driver, "overflow-x", "hidden", "hidden"));
    try$(_expectSerialized(_driver, "overflow-x", "scroll", "scroll"));
    try$(_expectSerialized(_driver, "overflow-y", "auto", "auto"));
    return Ok();
}

test$("vaev-style-wpt-css-css-box-parsing-overflow-valid-xfail", XFAIL) {
    try$(_expectSerialized(_driver, "overflow", "visible", "visible"));
    try$(_expectSerialized(_driver, "overflow", "hidden", "hidden"));
    try$(_expectSerialized(_driver, "overflow", "scroll", "scroll"));
    try$(_expectSerialized(_driver, "overflow", "auto", "auto"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-box/parsing/padding-invalid.html
test$("vaev-style-wpt-css-css-box-parsing-padding-invalid") {
    try$(_expectInvalid(_driver, "padding", "auto"));
    try$(_expectInvalid(_driver, "padding", "available"));
    try$(_expectInvalid(_driver, "padding", "10px border-box"));
    try$(_expectInvalid(_driver, "padding", "calc(2em + 3ex) auto"));
    try$(_expectInvalid(_driver, "padding", "10px 20px 30px 40px 50px"));
    try$(_expectInvalid(_driver, "padding-top", "auto"));
    try$(_expectInvalid(_driver, "padding-bottom", "10px 20px calc(2em + 3ex) auto"));
    return Ok();
}

test$("vaev-style-wpt-css-css-box-parsing-padding-invalid-xfail", XFAIL) {
    try$(_expectInvalid(_driver, "padding-top", "-10px"));
    try$(_expectInvalid(_driver, "padding-right", "-20%"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-box/parsing/padding-valid.html
test$("vaev-style-wpt-css-css-box-parsing-padding-valid") {
    try$(_expectSerialized(_driver, "padding-top", "10px", "10px"));
    try$(_expectSerialized(_driver, "padding-right", "20px", "20px"));
    try$(_expectSerialized(_driver, "padding-bottom", "30px", "30px"));
    try$(_expectSerialized(_driver, "padding-left", "40px", "40px"));
    try$(_expectSerialized(_driver, "padding-right", "20%", "20%"));
    return Ok();
}

test$("vaev-style-wpt-css-css-box-parsing-padding-valid-xfail", XFAIL) {
    try$(_expectSerialized(_driver, "padding", "10px", "10px"));
    try$(_expectSerialized(_driver, "padding", "10px 20px 30px 40px", "10px 20px 30px 40px"));
    try$(_expectSerialized(_driver, "padding", "calc(2em + 3ex)", "calc(2em + 3ex)"));
    try$(_expectSerialized(_driver, "padding", "20%", "20%"));
    try$(_expectSerialized(_driver, "padding", "10px 20% 30% 40px", "10px 20% 30% 40px"));
    try$(_expectSerialized(_driver, "padding-right", "calc(2em + 3%)", "calc(3% + 2em)"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-box/parsing/visibility-invalid.html
test$("vaev-style-wpt-css-css-box-parsing-visibility-invalid") {
    try$(_expectInvalid(_driver, "visibility", "auto"));
    try$(_expectInvalid(_driver, "visibility", "hidden collapse"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-box/parsing/visibility-valid.html
test$("vaev-style-wpt-css-css-box-parsing-visibility-valid") {
    try$(_expectSerialized(_driver, "visibility", "visible", "visible"));
    try$(_expectSerialized(_driver, "visibility", "hidden", "hidden"));
    try$(_expectSerialized(_driver, "visibility", "collapse", "collapse"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-box/parsing/width-invalid.html
test$("vaev-style-wpt-css-css-box-parsing-width-invalid") {
    try$(_expectInvalid(_driver, "width", "complex"));
    try$(_expectInvalid(_driver, "width", "none"));
    try$(_expectInvalid(_driver, "width", "min-content available"));
    try$(_expectInvalid(_driver, "width", "max-content 10px"));
    try$(_expectInvalid(_driver, "width", "20% available"));
    try$(_expectInvalid(_driver, "width", "60"));
    try$(_expectInvalid(_driver, "width", "10px 20%"));
    try$(_expectInvalid(_driver, "width", "calc(1 / max(2px, 2em))"));
    try$(_expectInvalid(_driver, "width", "10px border-box"));
    try$(_expectInvalid(_driver, "width", "content-box 20%"));
    return Ok();
}

test$("vaev-style-wpt-css-css-box-parsing-width-invalid-xfail", XFAIL) {
    try$(_expectInvalid(_driver, "width", "-10px"));
    try$(_expectInvalid(_driver, "width", "-20%"));
    try$(_expectInvalid(_driver, "width", "calc(1 / 2px)"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-box/parsing/width-valid.html
test$("vaev-style-wpt-css-css-box-parsing-width-valid") {
    try$(_expectSerialized(_driver, "width", "auto", "auto"));
    try$(_expectSerialized(_driver, "width", "10px", "10px"));
    try$(_expectSerialized(_driver, "width", "20%", "20%"));
    try$(_expectSerialized(_driver, "width", "calc(2em + 3ex)", "calc(2em + 3ex)"));
    try$(_expectSerialized(_driver, "width", "min-content", "min-content"));
    try$(_expectSerialized(_driver, "width", "max-content", "max-content"));
    try$(_expectSerialized(_driver, "width", "fit-content", "fit-content"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-break/parsing/break-after-invalid.html
test$("vaev-style-wpt-css-css-break-parsing-break-after-invalid") {
    try$(_expectInvalid(_driver, "break-after", "none"));
    try$(_expectInvalid(_driver, "break-after", "avoid region"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-break/parsing/break-after-valid.html
test$("vaev-style-wpt-css-css-break-parsing-break-after-valid") {
    try$(_expectSerialized(_driver, "break-after", "auto", "auto"));
    try$(_expectSerialized(_driver, "break-after", "avoid", "avoid"));
    try$(_expectSerialized(_driver, "break-after", "avoid-page", "avoid-page"));
    try$(_expectSerialized(_driver, "break-after", "page", "page"));
    try$(_expectSerialized(_driver, "break-after", "left", "left"));
    try$(_expectSerialized(_driver, "break-after", "right", "right"));
    try$(_expectSerialized(_driver, "break-after", "recto", "recto"));
    try$(_expectSerialized(_driver, "break-after", "verso", "verso"));
    try$(_expectSerialized(_driver, "break-after", "avoid-column", "avoid-column"));
    try$(_expectSerialized(_driver, "break-after", "column", "column"));
    try$(_expectSerialized(_driver, "break-after", "avoid-region", "avoid-region"));
    try$(_expectSerialized(_driver, "break-after", "region", "region"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-break/parsing/break-before-invalid.html
test$("vaev-style-wpt-css-css-break-parsing-break-before-invalid") {
    try$(_expectInvalid(_driver, "break-before", "none"));
    try$(_expectInvalid(_driver, "break-before", "avoid region"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-break/parsing/break-before-valid.html
test$("vaev-style-wpt-css-css-break-parsing-break-before-valid") {
    try$(_expectSerialized(_driver, "break-before", "auto", "auto"));
    try$(_expectSerialized(_driver, "break-before", "avoid", "avoid"));
    try$(_expectSerialized(_driver, "break-before", "avoid-page", "avoid-page"));
    try$(_expectSerialized(_driver, "break-before", "page", "page"));
    try$(_expectSerialized(_driver, "break-before", "left", "left"));
    try$(_expectSerialized(_driver, "break-before", "right", "right"));
    try$(_expectSerialized(_driver, "break-before", "recto", "recto"));
    try$(_expectSerialized(_driver, "break-before", "verso", "verso"));
    try$(_expectSerialized(_driver, "break-before", "avoid-column", "avoid-column"));
    try$(_expectSerialized(_driver, "break-before", "column", "column"));
    try$(_expectSerialized(_driver, "break-before", "avoid-region", "avoid-region"));
    try$(_expectSerialized(_driver, "break-before", "region", "region"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-break/parsing/break-inside-invalid.html
test$("vaev-style-wpt-css-css-break-parsing-break-inside-invalid") {
    try$(_expectInvalid(_driver, "break-inside", "region"));
    try$(_expectInvalid(_driver, "break-inside", "auto avoid"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-break/parsing/break-inside-valid.html
test$("vaev-style-wpt-css-css-break-parsing-break-inside-valid") {
    try$(_expectSerialized(_driver, "break-inside", "auto", "auto"));
    try$(_expectSerialized(_driver, "break-inside", "avoid", "avoid"));
    try$(_expectSerialized(_driver, "break-inside", "avoid-page", "avoid-page"));
    try$(_expectSerialized(_driver, "break-inside", "avoid-column", "avoid-column"));
    try$(_expectSerialized(_driver, "break-inside", "avoid-region", "avoid-region"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-color/parsing/alpha-color-parsing-invalid.html
test$("vaev-style-wpt-css-css-color-parsing-alpha-color-parsing-invalid") {
    try$(_expectInvalid(_driver, "color", "alpha(red / 0.5)"));
    try$(_expectInvalid(_driver, "color", "alpha()"));
    try$(_expectInvalid(_driver, "color", "alpha(from / 0.5)"));
    try$(_expectInvalid(_driver, "color", "alpha(from)"));
    try$(_expectInvalid(_driver, "color", "alpha(from red)"));
    try$(_expectInvalid(_driver, "color", "alpha(from currentcolor)"));
    try$(_expectInvalid(_driver, "color", "alpha(from rgba(255, 0, 0, 0.3))"));
    try$(_expectInvalid(_driver, "color", "alpha(from alpha(from currentcolor / 0.5))"));
    try$(_expectInvalid(_driver, "color", "alpha(from alpha(from currentcolor) / 0.5)"));
    try$(_expectInvalid(_driver, "color", "alpha(from red / 0.5 0.5)"));
    try$(_expectInvalid(_driver, "color", "alpha(from red 0.5)"));
    try$(_expectInvalid(_driver, "color", "alpha(from red r g b / 0.5)"));
    try$(_expectInvalid(_driver, "color", "alpha(from 42 / 0.5)"));
    try$(_expectInvalid(_driver, "color", "alpha(from none / 0.5)"));
    try$(_expectInvalid(_driver, "color", "alpha(from red /)"));
    try$(_expectInvalid(_driver, "color", "alpha(from red / / 0.5)"));
    try$(_expectInvalid(_driver, "color", "alpha(from red / 0.5 / 0.5)"));
    try$(_expectInvalid(_driver, "color", "alpha(from red / alpha alpha)"));
    try$(_expectInvalid(_driver, "color", "alpha(from red, blue)"));
    try$(_expectInvalid(_driver, "color", "alpha(from red / r)"));
    try$(_expectInvalid(_driver, "color", "alpha(from red / l)"));
    try$(_expectInvalid(_driver, "color", "alpha(from red / red)"));
    try$(_expectInvalid(_driver, "color", "alpha(from red / calc(r * 0.5))"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-color/parsing/alpha-color-parsing-valid.html
test$("vaev-style-wpt-css-css-color-parsing-alpha-color-parsing-valid") {
    try$(_expectSerialized(_driver, "color", "alpha(from red / var(--alpha))", "alpha(from red / var(--alpha))", RegisteredPropertySet::TOP_LEVEL));
    return Ok();
}

test$("vaev-style-wpt-css-css-color-parsing-alpha-color-parsing-valid-xfail", XFAIL) {
    try$(_expectSerialized(_driver, "color", "alpha(from red / 0.5)", "alpha(from red / 0.5)"));
    try$(_expectSerialized(_driver, "color", "alpha(from blue / 1)", "alpha(from blue / 1)"));
    try$(_expectSerialized(_driver, "color", "alpha(from green / 0)", "alpha(from green / 0)"));
    try$(_expectSerialized(_driver, "color", "alpha(from rgba(0, 0, 0, 0.5) / 1)", "alpha(from rgba(0, 0, 0, 0.5) / 1)"));
    try$(_expectSerialized(_driver, "color", "alpha(from currentcolor / 1)", "alpha(from currentcolor / 1)"));
    try$(_expectSerialized(_driver, "color", "alpha(from currentcolor / 100%)", "alpha(from currentcolor / 100%)"));
    try$(_expectSerialized(_driver, "color", "alpha(from alpha(from currentcolor / 0.5) / 1)", "alpha(from alpha(from currentcolor / 0.5) / 1)"));
    try$(_expectSerialized(_driver, "color", "alpha(from red / 50%)", "alpha(from red / 50%)"));
    try$(_expectSerialized(_driver, "color", "alpha(from red / 0%)", "alpha(from red / 0%)"));
    try$(_expectSerialized(_driver, "color", "alpha(from red / 100%)", "alpha(from red / 100%)"));
    try$(_expectSerialized(_driver, "color", "alpha(from red / none)", "alpha(from red / none)"));
    try$(_expectSerialized(_driver, "color", "alpha(from currentcolor / alpha)", "alpha(from currentcolor / alpha)"));
    try$(_expectSerialized(_driver, "color", "alpha(from currentcolor / calc(alpha * 0.5))", "alpha(from currentcolor / calc(0.5 * alpha))"));
    try$(_expectSerialized(_driver, "color", "alpha(from currentcolor / calc(alpha + 0.1))", "alpha(from currentcolor / calc(0.1 + alpha))"));
    try$(_expectSerialized(_driver, "color", "color-mix(in srgb, alpha(from red / 0.5), blue)", "color-mix(in srgb, alpha(from red / 0.5), blue)"));
    try$(_expectSerialized(_driver, "color", "rgb(from alpha(from currentcolor / 0.5) r g b)", "rgb(from alpha(from currentcolor / 0.5) r g b)"));
    try$(_expectSerialized(_driver, "color", "alpha(from color-mix(in srgb, red, blue) / 0.5)", "alpha(from color-mix(in srgb, red, blue) / 0.5)"));
    try$(_expectSerialized(_driver, "color", "alpha(from rgb(from red r g b) / 0.8)", "alpha(from rgb(from red r g b) / 0.8)"));
    try$(_expectSerialized(_driver, "color", "alpha(from color(display-p3 1 0 0) / 0.5)", "alpha(from color(display-p3 1 0 0) / 0.5)"));
    try$(_expectSerialized(_driver, "color", "alpha(from lab(50 20 -30) / 0.5)", "alpha(from lab(50 20 -30) / 0.5)"));
    try$(_expectSerialized(_driver, "color", "alpha(from oklch(0.5 0.2 120) / 0.5)", "alpha(from oklch(0.5 0.2 120) / 0.5)"));
    try$(_expectSerialized(_driver, "color", "alpha(from currentcolor / 0.5)", "alpha(from currentcolor / 0.5)"));
    try$(_expectSerialized(_driver, "color", "alpha(from red / 2)", "alpha(from red / 2)"));
    try$(_expectSerialized(_driver, "color", "alpha(from red / -1)", "alpha(from red / -1)"));
    try$(_expectSerialized(_driver, "color", "alpha(from rgba(255, 0, 0, 0.8) / alpha)", "alpha(from rgba(255, 0, 0, 0.8) / alpha)"));
    try$(_expectSerialized(_driver, "color", "alpha(from color(srgb 1 0 0) / 50%)", "alpha(from color(srgb 1 0 0) / 50%)"));
    try$(_expectSerialized(_driver, "color", "alpha(from oklab(0.5 0.2 -0.3) / 0.5)", "alpha(from oklab(0.5 0.2 -0.3) / 0.5)"));
    try$(_expectSerialized(_driver, "color", "alpha(from lch(50% 20 30) / 0.5)", "alpha(from lch(50 20 30) / 0.5)"));
    try$(_expectSerialized(_driver, "color", "alpha(from hsl(120 50% 50%) / 0.5)", "alpha(from rgb(64, 191, 64) / 0.5)"));
    try$(_expectSerialized(_driver, "color", "alpha(from hwb(120 0% 0%) / 0.5)", "alpha(from rgb(0, 255, 0) / 0.5)"));
    try$(_expectSerialized(_driver, "color", "alpha(from ActiveText / 0.5)", "alpha(from activetext / 0.5)"));
    try$(_expectSerialized(_driver, "color", "alpha(from green / sibling-index())", "alpha(from green / sibling-index())"));
    try$(_expectSerialized(_driver, "color", "alpha(from green / sibling-count())", "alpha(from green / sibling-count())"));
    try$(_expectSerialized(_driver, "color", "alpha(from green / calc(sibling-index() * 0.2))", "alpha(from green / calc(0.2 * sibling-index()))"));
    try$(_expectSerialized(_driver, "color", "alpha(from alpha(from currentcolor / 0.5) / alpha)", "alpha(from alpha(from currentcolor / 0.5) / alpha)"));
    try$(_expectSerialized(_driver, "color", "alpha(from alpha(from currentcolor / 0.5) / calc(alpha + 0.25))", "alpha(from alpha(from currentcolor / 0.5) / calc(0.25 + alpha))"));
    try$(_expectSerialized(_driver, "color", "rgb(from alpha(from red / 0.5) r g b / alpha)", "rgb(from alpha(from red / 0.5) r g b / alpha)"));
    try$(_expectSerialized(_driver, "color", "hsl(from alpha(from currentcolor / 0.5) h s l / alpha)", "hsl(from alpha(from currentcolor / 0.5) h s l / alpha)"));
    try$(_expectSerialized(_driver, "color", "hwb(from alpha(from currentcolor / 0.5) h w b / alpha)", "hwb(from alpha(from currentcolor / 0.5) h w b / alpha)"));
    try$(_expectSerialized(_driver, "color", "lab(from alpha(from currentcolor / 0.5) l a b / alpha)", "lab(from alpha(from currentcolor / 0.5) l a b / alpha)"));
    try$(_expectSerialized(_driver, "color", "lch(from alpha(from currentcolor / 0.5) l c h / alpha)", "lch(from alpha(from currentcolor / 0.5) l c h / alpha)"));
    try$(_expectSerialized(_driver, "color", "oklab(from alpha(from currentcolor / 0.5) l a b / alpha)", "oklab(from alpha(from currentcolor / 0.5) l a b / alpha)"));
    try$(_expectSerialized(_driver, "color", "oklch(from alpha(from currentcolor / 0.5) l c h / alpha)", "oklch(from alpha(from currentcolor / 0.5) l c h / alpha)"));
    try$(_expectSerialized(_driver, "color", "color(from alpha(from currentcolor / 0.5) display-p3 r g b / alpha)", "color(from alpha(from currentcolor / 0.5) display-p3 r g b / alpha)"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-color/parsing/color-invalid-color-function.html
test$("vaev-style-wpt-css-css-color-parsing-color-invalid-color-function") {
    try$(_expectInvalid(_driver, "color", "color(srgb 0 0 0 0)"));
    try$(_expectInvalid(_driver, "color", "color(srgb 0deg 0% 0)"));
    try$(_expectInvalid(_driver, "color", "color(srgb 0% 0 0 1)"));
    try$(_expectInvalid(_driver, "color", "color(srgb 0% 0 0 10%)"));
    try$(_expectInvalid(_driver, "color", "color(srgb 0% 0 0deg)"));
    try$(_expectInvalid(_driver, "color", "color(srgb 0% 0% 0deg)"));
    try$(_expectInvalid(_driver, "color", "color(srgb 40% 0 0deg)"));
    try$(_expectInvalid(_driver, "color", "color(srgb 50% -200)"));
    try$(_expectInvalid(_driver, "color", "color(srgb 50%)"));
    try$(_expectInvalid(_driver, "color", "color(srgb)"));
    try$(_expectInvalid(_driver, "color", "color(srgb 50% -200 / 0.5)"));
    try$(_expectInvalid(_driver, "color", "color(srgb 50% / 0.5)"));
    try$(_expectInvalid(_driver, "color", "color(srgb / 0.5)"));
    try$(_expectInvalid(_driver, "color", "color(srgb-linear 0 0 0 0)"));
    try$(_expectInvalid(_driver, "color", "color(srgb-linear 0deg 0% 0)"));
    try$(_expectInvalid(_driver, "color", "color(srgb-linear 0% 0 0 1)"));
    try$(_expectInvalid(_driver, "color", "color(srgb-linear 0% 0 0 10%)"));
    try$(_expectInvalid(_driver, "color", "color(srgb-linear 0% 0 0deg)"));
    try$(_expectInvalid(_driver, "color", "color(srgb-linear 0% 0% 0deg)"));
    try$(_expectInvalid(_driver, "color", "color(srgb-linear 40% 0 0deg)"));
    try$(_expectInvalid(_driver, "color", "color(srgb-linear 50% -200)"));
    try$(_expectInvalid(_driver, "color", "color(srgb-linear 50%)"));
    try$(_expectInvalid(_driver, "color", "color(srgb-linear)"));
    try$(_expectInvalid(_driver, "color", "color(srgb-linear 50% -200 / 0.5)"));
    try$(_expectInvalid(_driver, "color", "color(srgb-linear 50% / 0.5)"));
    try$(_expectInvalid(_driver, "color", "color(srgb-linear / 0.5)"));
    try$(_expectInvalid(_driver, "color", "color(a98-rgb 0 0 0 0)"));
    try$(_expectInvalid(_driver, "color", "color(a98-rgb 0deg 0% 0)"));
    try$(_expectInvalid(_driver, "color", "color(a98-rgb 0% 0 0 1)"));
    try$(_expectInvalid(_driver, "color", "color(a98-rgb 0% 0 0 10%)"));
    try$(_expectInvalid(_driver, "color", "color(a98-rgb 0% 0 0deg)"));
    try$(_expectInvalid(_driver, "color", "color(a98-rgb 0% 0% 0deg)"));
    try$(_expectInvalid(_driver, "color", "color(a98-rgb 40% 0 0deg)"));
    try$(_expectInvalid(_driver, "color", "color(a98-rgb 50% -200)"));
    try$(_expectInvalid(_driver, "color", "color(a98-rgb 50%)"));
    try$(_expectInvalid(_driver, "color", "color(a98-rgb)"));
    try$(_expectInvalid(_driver, "color", "color(a98-rgb 50% -200 / 0.5)"));
    try$(_expectInvalid(_driver, "color", "color(a98-rgb 50% / 0.5)"));
    try$(_expectInvalid(_driver, "color", "color(a98-rgb / 0.5)"));
    try$(_expectInvalid(_driver, "color", "color(rec2020 0 0 0 0)"));
    try$(_expectInvalid(_driver, "color", "color(rec2020 0deg 0% 0)"));
    try$(_expectInvalid(_driver, "color", "color(rec2020 0% 0 0 1)"));
    try$(_expectInvalid(_driver, "color", "color(rec2020 0% 0 0 10%)"));
    try$(_expectInvalid(_driver, "color", "color(rec2020 0% 0 0deg)"));
    try$(_expectInvalid(_driver, "color", "color(rec2020 0% 0% 0deg)"));
    try$(_expectInvalid(_driver, "color", "color(rec2020 40% 0 0deg)"));
    try$(_expectInvalid(_driver, "color", "color(rec2020 50% -200)"));
    try$(_expectInvalid(_driver, "color", "color(rec2020 50%)"));
    try$(_expectInvalid(_driver, "color", "color(rec2020)"));
    try$(_expectInvalid(_driver, "color", "color(rec2020 50% -200 / 0.5)"));
    try$(_expectInvalid(_driver, "color", "color(rec2020 50% / 0.5)"));
    try$(_expectInvalid(_driver, "color", "color(rec2020 / 0.5)"));
    try$(_expectInvalid(_driver, "color", "color(prophoto-rgb 0 0 0 0)"));
    try$(_expectInvalid(_driver, "color", "color(prophoto-rgb 0deg 0% 0)"));
    try$(_expectInvalid(_driver, "color", "color(prophoto-rgb 0% 0 0 1)"));
    try$(_expectInvalid(_driver, "color", "color(prophoto-rgb 0% 0 0 10%)"));
    try$(_expectInvalid(_driver, "color", "color(prophoto-rgb 0% 0 0deg)"));
    try$(_expectInvalid(_driver, "color", "color(prophoto-rgb 0% 0% 0deg)"));
    try$(_expectInvalid(_driver, "color", "color(prophoto-rgb 40% 0 0deg)"));
    try$(_expectInvalid(_driver, "color", "color(prophoto-rgb 50% -200)"));
    try$(_expectInvalid(_driver, "color", "color(prophoto-rgb 50%)"));
    try$(_expectInvalid(_driver, "color", "color(prophoto-rgb)"));
    try$(_expectInvalid(_driver, "color", "color(prophoto-rgb 50% -200 / 0.5)"));
    try$(_expectInvalid(_driver, "color", "color(prophoto-rgb 50% / 0.5)"));
    try$(_expectInvalid(_driver, "color", "color(prophoto-rgb / 0.5)"));
    try$(_expectInvalid(_driver, "color", "color()"));
    try$(_expectInvalid(_driver, "color", "color(1 1 1)"));
    try$(_expectInvalid(_driver, "color", "color(srgb 1 1)"));
    try$(_expectInvalid(_driver, "color", "color(srgb 1)"));
    try$(_expectInvalid(_driver, "color", "color(srgb 0, 0, 0)"));
    try$(_expectInvalid(_driver, "color", "color(srgb 1 1 1 1)"));
    try$(_expectInvalid(_driver, "color", "color(srgb 1 1 1 1 1)"));
    try$(_expectInvalid(_driver, "color", "color(srgb 1 eggs 1)"));
    try$(_expectInvalid(_driver, "color", "color(srgb 1 1 1 / bacon)"));
    try$(_expectInvalid(_driver, "color", "srgb(0 0 0)"));
    try$(_expectInvalid(_driver, "color", "srgb-linear(0 0 0)"));
    try$(_expectInvalid(_driver, "color", "a98-rgb(0 0 0)"));
    try$(_expectInvalid(_driver, "color", "rec2020(0 0 0)"));
    try$(_expectInvalid(_driver, "color", "prophoto-rgb(0 0 0)"));
    try$(_expectInvalid(_driver, "color", "xyz(0 0 0)"));
    try$(_expectInvalid(_driver, "color", "xyz-d50(0 0 0)"));
    try$(_expectInvalid(_driver, "color", "xyz-d65(0 0 0)"));
    return Ok();
}

test$("vaev-style-wpt-css-css-color-parsing-color-invalid-color-function-xfail", XFAIL) {
    try$(_expectInvalid(_driver, "color", "color(xyz 0 0 0 0)"));
    try$(_expectInvalid(_driver, "color", "color(xyz 0deg 0% 0)"));
    try$(_expectInvalid(_driver, "color", "color(xyz 0% 0 0 1)"));
    try$(_expectInvalid(_driver, "color", "color(xyz 0% 0 0 10%)"));
    try$(_expectInvalid(_driver, "color", "color(xyz 0% 0 0deg)"));
    try$(_expectInvalid(_driver, "color", "color(xyz 0% 0% 0deg)"));
    try$(_expectInvalid(_driver, "color", "color(xyz 40% 0 0deg)"));
    try$(_expectInvalid(_driver, "color", "color(xyz 1 1)"));
    try$(_expectInvalid(_driver, "color", "color(xyz 1)"));
    try$(_expectInvalid(_driver, "color", "color(xyz)"));
    try$(_expectInvalid(_driver, "color", "color(xyz 1 1 / .5)"));
    try$(_expectInvalid(_driver, "color", "color(xyz 1 / 0.5)"));
    try$(_expectInvalid(_driver, "color", "color(xyz / 50%)"));
    try$(_expectInvalid(_driver, "color", "color(xyz-d50 0 0 0 0)"));
    try$(_expectInvalid(_driver, "color", "color(xyz-d50 0deg 0% 0)"));
    try$(_expectInvalid(_driver, "color", "color(xyz-d50 0% 0 0 1)"));
    try$(_expectInvalid(_driver, "color", "color(xyz-d50 0% 0 0 10%)"));
    try$(_expectInvalid(_driver, "color", "color(xyz-d50 0% 0 0deg)"));
    try$(_expectInvalid(_driver, "color", "color(xyz-d50 0% 0% 0deg)"));
    try$(_expectInvalid(_driver, "color", "color(xyz-d50 40% 0 0deg)"));
    try$(_expectInvalid(_driver, "color", "color(xyz-d50 1 1)"));
    try$(_expectInvalid(_driver, "color", "color(xyz-d50 1)"));
    try$(_expectInvalid(_driver, "color", "color(xyz-d50)"));
    try$(_expectInvalid(_driver, "color", "color(xyz-d50 1 1 / .5)"));
    try$(_expectInvalid(_driver, "color", "color(xyz-d50 1 / 0.5)"));
    try$(_expectInvalid(_driver, "color", "color(xyz-d50 / 50%)"));
    try$(_expectInvalid(_driver, "color", "color(xyz-d65 0 0 0 0)"));
    try$(_expectInvalid(_driver, "color", "color(xyz-d65 0deg 0% 0)"));
    try$(_expectInvalid(_driver, "color", "color(xyz-d65 0% 0 0 1)"));
    try$(_expectInvalid(_driver, "color", "color(xyz-d65 0% 0 0 10%)"));
    try$(_expectInvalid(_driver, "color", "color(xyz-d65 0% 0 0deg)"));
    try$(_expectInvalid(_driver, "color", "color(xyz-d65 0% 0% 0deg)"));
    try$(_expectInvalid(_driver, "color", "color(xyz-d65 40% 0 0deg)"));
    try$(_expectInvalid(_driver, "color", "color(xyz-d65 1 1)"));
    try$(_expectInvalid(_driver, "color", "color(xyz-d65 1)"));
    try$(_expectInvalid(_driver, "color", "color(xyz-d65)"));
    try$(_expectInvalid(_driver, "color", "color(xyz-d65 1 1 / .5)"));
    try$(_expectInvalid(_driver, "color", "color(xyz-d65 1 / 0.5)"));
    try$(_expectInvalid(_driver, "color", "color(xyz-d65 / 50%)"));
    try$(_expectInvalid(_driver, "color", "color(banana 1 1 1)"));
    try$(_expectInvalid(_driver, "color", "color(displayp3 1 1 1)"));
    try$(_expectInvalid(_driver, "color", "color(srgb 1 1 1 / 1 cucumber)"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-color/parsing/color-invalid-color-layers-function.html
test$("vaev-style-wpt-css-css-color-parsing-color-invalid-color-layers-function") {
    try$(_expectInvalid(_driver, "color", "color-layers()"));
    try$(_expectInvalid(_driver, "color", "color-layers(normal)"));
    try$(_expectInvalid(_driver, "color", "color-layers(multiply)"));
    try$(_expectInvalid(_driver, "color", "color-layers(red blue)"));
    try$(_expectInvalid(_driver, "color", "color-layers(red, blue, )"));
    try$(_expectInvalid(_driver, "color", "color-layers(, red, blue)"));
    try$(_expectInvalid(_driver, "color", "color-layers(, red, blue, green)"));
    try$(_expectInvalid(_driver, "color", "color-layers(, rgba(0, 0, 0, 0.5), color(display-p3 1 0.5 0))"));
    try$(_expectInvalid(_driver, "color", "color-layers(, currentcolor, blue)"));
    try$(_expectInvalid(_driver, "color", "color-layers(, canvastext, green)"));
    try$(_expectInvalid(_driver, "color", "color-layers(, rgb(from black r g b / 0.5), green)"));
    try$(_expectInvalid(_driver, "color", "color-layers(, color-mix(in srgb, red, blue), blue)"));
    try$(_expectInvalid(_driver, "color", "color-layers(, color-layers(red, blue), green)"));
    try$(_expectInvalid(_driver, "color", "color-layers(plus-lighter, red, blue)"));
    try$(_expectInvalid(_driver, "color", "color-layers(plus-lighter, red, blue, green)"));
    try$(_expectInvalid(_driver, "color", "color-layers(plus-lighter, rgba(0, 0, 0, 0.5), color(display-p3 1 0.5 0))"));
    try$(_expectInvalid(_driver, "color", "color-layers(plus-lighter, currentcolor, blue)"));
    try$(_expectInvalid(_driver, "color", "color-layers(plus-lighter, canvastext, green)"));
    try$(_expectInvalid(_driver, "color", "color-layers(plus-lighter, rgb(from black r g b / 0.5), green)"));
    try$(_expectInvalid(_driver, "color", "color-layers(plus-lighter, color-mix(in srgb, red, blue), blue)"));
    try$(_expectInvalid(_driver, "color", "color-layers(plus-lighter, color-layers(red, blue), green)"));
    try$(_expectInvalid(_driver, "color", "color-layers(plus-darker, red, blue)"));
    try$(_expectInvalid(_driver, "color", "color-layers(plus-darker, red, blue, green)"));
    try$(_expectInvalid(_driver, "color", "color-layers(plus-darker, rgba(0, 0, 0, 0.5), color(display-p3 1 0.5 0))"));
    try$(_expectInvalid(_driver, "color", "color-layers(plus-darker, currentcolor, blue)"));
    try$(_expectInvalid(_driver, "color", "color-layers(plus-darker, canvastext, green)"));
    try$(_expectInvalid(_driver, "color", "color-layers(plus-darker, rgb(from black r g b / 0.5), green)"));
    try$(_expectInvalid(_driver, "color", "color-layers(plus-darker, color-mix(in srgb, red, blue), blue)"));
    try$(_expectInvalid(_driver, "color", "color-layers(plus-darker, color-layers(red, blue), green)"));
    try$(_expectInvalid(_driver, "color", "color-layers(foo-bar, red, blue)"));
    try$(_expectInvalid(_driver, "color", "color-layers(foo-bar, red, blue, green)"));
    try$(_expectInvalid(_driver, "color", "color-layers(foo-bar, rgba(0, 0, 0, 0.5), color(display-p3 1 0.5 0))"));
    try$(_expectInvalid(_driver, "color", "color-layers(foo-bar, currentcolor, blue)"));
    try$(_expectInvalid(_driver, "color", "color-layers(foo-bar, canvastext, green)"));
    try$(_expectInvalid(_driver, "color", "color-layers(foo-bar, rgb(from black r g b / 0.5), green)"));
    try$(_expectInvalid(_driver, "color", "color-layers(foo-bar, color-mix(in srgb, red, blue), blue)"));
    try$(_expectInvalid(_driver, "color", "color-layers(foo-bar, color-layers(red, blue), green)"));
    try$(_expectInvalid(_driver, "color", "color-layers(custom-ident, red, blue)"));
    try$(_expectInvalid(_driver, "color", "color-layers(custom-ident, red, blue, green)"));
    try$(_expectInvalid(_driver, "color", "color-layers(custom-ident, rgba(0, 0, 0, 0.5), color(display-p3 1 0.5 0))"));
    try$(_expectInvalid(_driver, "color", "color-layers(custom-ident, currentcolor, blue)"));
    try$(_expectInvalid(_driver, "color", "color-layers(custom-ident, canvastext, green)"));
    try$(_expectInvalid(_driver, "color", "color-layers(custom-ident, rgb(from black r g b / 0.5), green)"));
    try$(_expectInvalid(_driver, "color", "color-layers(custom-ident, color-mix(in srgb, red, blue), blue)"));
    try$(_expectInvalid(_driver, "color", "color-layers(custom-ident, color-layers(red, blue), green)"));
    try$(_expectInvalid(_driver, "color", "color-layers(calc(12px + 100%), red, blue)"));
    try$(_expectInvalid(_driver, "color", "color-layers(calc(12px + 100%), red, blue, green)"));
    try$(_expectInvalid(_driver, "color", "color-layers(calc(12px + 100%), rgba(0, 0, 0, 0.5), color(display-p3 1 0.5 0))"));
    try$(_expectInvalid(_driver, "color", "color-layers(calc(12px + 100%), currentcolor, blue)"));
    try$(_expectInvalid(_driver, "color", "color-layers(calc(12px + 100%), canvastext, green)"));
    try$(_expectInvalid(_driver, "color", "color-layers(calc(12px + 100%), rgb(from black r g b / 0.5), green)"));
    try$(_expectInvalid(_driver, "color", "color-layers(calc(12px + 100%), color-mix(in srgb, red, blue), blue)"));
    try$(_expectInvalid(_driver, "color", "color-layers(calc(12px + 100%), color-layers(red, blue), green)"));
    try$(_expectInvalid(_driver, "color", "color-layers(50%, red, blue)"));
    try$(_expectInvalid(_driver, "color", "color-layers(50%, red, blue, green)"));
    try$(_expectInvalid(_driver, "color", "color-layers(50%, rgba(0, 0, 0, 0.5), color(display-p3 1 0.5 0))"));
    try$(_expectInvalid(_driver, "color", "color-layers(50%, currentcolor, blue)"));
    try$(_expectInvalid(_driver, "color", "color-layers(50%, canvastext, green)"));
    try$(_expectInvalid(_driver, "color", "color-layers(50%, rgb(from black r g b / 0.5), green)"));
    try$(_expectInvalid(_driver, "color", "color-layers(50%, color-mix(in srgb, red, blue), blue)"));
    try$(_expectInvalid(_driver, "color", "color-layers(50%, color-layers(red, blue), green)"));
    try$(_expectInvalid(_driver, "color", "color-layers(in srgb, red, blue)"));
    try$(_expectInvalid(_driver, "color", "color-layers(in srgb, red, blue, green)"));
    try$(_expectInvalid(_driver, "color", "color-layers(in srgb, rgba(0, 0, 0, 0.5), color(display-p3 1 0.5 0))"));
    try$(_expectInvalid(_driver, "color", "color-layers(in srgb, currentcolor, blue)"));
    try$(_expectInvalid(_driver, "color", "color-layers(in srgb, canvastext, green)"));
    try$(_expectInvalid(_driver, "color", "color-layers(in srgb, rgb(from black r g b / 0.5), green)"));
    try$(_expectInvalid(_driver, "color", "color-layers(in srgb, color-mix(in srgb, red, blue), blue)"));
    try$(_expectInvalid(_driver, "color", "color-layers(in srgb, color-layers(red, blue), green)"));
    try$(_expectInvalid(_driver, "color", "color-layers(display-p3, red, blue)"));
    try$(_expectInvalid(_driver, "color", "color-layers(display-p3, red, blue, green)"));
    try$(_expectInvalid(_driver, "color", "color-layers(display-p3, rgba(0, 0, 0, 0.5), color(display-p3 1 0.5 0))"));
    try$(_expectInvalid(_driver, "color", "color-layers(display-p3, currentcolor, blue)"));
    try$(_expectInvalid(_driver, "color", "color-layers(display-p3, canvastext, green)"));
    try$(_expectInvalid(_driver, "color", "color-layers(display-p3, rgb(from black r g b / 0.5), green)"));
    try$(_expectInvalid(_driver, "color", "color-layers(display-p3, color-mix(in srgb, red, blue), blue)"));
    try$(_expectInvalid(_driver, "color", "color-layers(display-p3, color-layers(red, blue), green)"));
    try$(_expectInvalid(_driver, "color", "color-layers(multiply, multiply, red, blue)"));
    try$(_expectInvalid(_driver, "color", "color-layers(multiply, multiply, red, blue, green)"));
    try$(_expectInvalid(_driver, "color", "color-layers(multiply, multiply, rgba(0, 0, 0, 0.5), color(display-p3 1 0.5 0))"));
    try$(_expectInvalid(_driver, "color", "color-layers(multiply, multiply, currentcolor, blue)"));
    try$(_expectInvalid(_driver, "color", "color-layers(multiply, multiply, canvastext, green)"));
    try$(_expectInvalid(_driver, "color", "color-layers(multiply, multiply, rgb(from black r g b / 0.5), green)"));
    try$(_expectInvalid(_driver, "color", "color-layers(multiply, multiply, color-mix(in srgb, red, blue), blue)"));
    try$(_expectInvalid(_driver, "color", "color-layers(multiply, multiply, color-layers(red, blue), green)"));
    try$(_expectInvalid(_driver, "color", "color-layers(normal, normal, red, blue)"));
    try$(_expectInvalid(_driver, "color", "color-layers(normal, normal, red, blue, green)"));
    try$(_expectInvalid(_driver, "color", "color-layers(normal, normal, rgba(0, 0, 0, 0.5), color(display-p3 1 0.5 0))"));
    try$(_expectInvalid(_driver, "color", "color-layers(normal, normal, currentcolor, blue)"));
    try$(_expectInvalid(_driver, "color", "color-layers(normal, normal, canvastext, green)"));
    try$(_expectInvalid(_driver, "color", "color-layers(normal, normal, rgb(from black r g b / 0.5), green)"));
    try$(_expectInvalid(_driver, "color", "color-layers(normal, normal, color-mix(in srgb, red, blue), blue)"));
    try$(_expectInvalid(_driver, "color", "color-layers(normal, normal, color-layers(red, blue), green)"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-color/parsing/color-invalid-color-mix-function.html
test$("vaev-style-wpt-css-css-color-parsing-color-invalid-color-mix-function") {
    try$(_expectInvalid(_driver, "color", "color-mix(in hsl, hsl(120deg 10% 20% 40%) -10%, hsl(30deg 30% 40% 80%))"));
    try$(_expectInvalid(_driver, "color", "color-mix(in hsl, hsl(120deg 10% 20% 40%) 150%, hsl(30deg 30% 40% 80%))"));
    try$(_expectInvalid(_driver, "color", "color-mix(in hsl hue, hsl(120deg 10% 20%), hsl(30deg 30% 40%))"));
    try$(_expectInvalid(_driver, "color", "color-mix(in hsl shorter, hsl(120deg 10% 20%), hsl(30deg 30% 40%))"));
    try$(_expectInvalid(_driver, "color", "color-mix(in hsl foo, hsl(120deg 10% 20%), hsl(30deg 30% 40%))"));
    try$(_expectInvalid(_driver, "color", "color-mix(in hsl hsl(120deg 10% 20%), hsl(30deg 30% 40%))"));
    try$(_expectInvalid(_driver, "color", "color-mix(in hsl, hsl(120deg 10% 20%) hsl(30deg 30% 40%))"));
    try$(_expectInvalid(_driver, "color", "color-mix(hsl(120deg 10% 20%), hsl(30deg 30% 40%), in hsl)"));
    try$(_expectInvalid(_driver, "color", "color-mix(in hwb, hwb(120deg 10% 20%) -10%, hwb(30deg 30% 40%))"));
    try$(_expectInvalid(_driver, "color", "color-mix(in hwb, hwb(120deg 10% 20%) 150%, hwb(30deg 30% 40%))"));
    try$(_expectInvalid(_driver, "color", "color-mix(in hwb, hwb(120deg 10% 20% 40%) -10%, hwb(30deg 30% 40% 80%))"));
    try$(_expectInvalid(_driver, "color", "color-mix(in hwb, hwb(120deg 10% 20% 40%) 150%, hwb(30deg 30% 40% 80%))"));
    try$(_expectInvalid(_driver, "color", "color-mix(in hwb hue, hwb(120deg 10% 20%), hwb(30deg 30% 40%))"));
    try$(_expectInvalid(_driver, "color", "color-mix(in hwb shorter, hwb(120deg 10% 20%), hwb(30deg 30% 40%))"));
    try$(_expectInvalid(_driver, "color", "color-mix(in hwb foo, hwb(120deg 10% 20%), hwb(30deg 30% 40%))"));
    try$(_expectInvalid(_driver, "color", "color-mix(in hwb hwb(120deg 10% 20%), hwb(30deg 30% 40%))"));
    try$(_expectInvalid(_driver, "color", "color-mix(in hwb, hwb(120deg 10% 20%) hwb(30deg 30% 40%))"));
    try$(_expectInvalid(_driver, "color", "color-mix(hwb(120deg 10% 20%), hwb(30deg 30% 40%), in hwb)"));
    try$(_expectInvalid(_driver, "color", "color-mix(in lch, lch(10% 20 30deg) -10%, lch(50% 60 70deg))"));
    try$(_expectInvalid(_driver, "color", "color-mix(in lch, lch(10% 20 30deg) 150%, lch(50% 60 70deg))"));
    try$(_expectInvalid(_driver, "color", "color-mix(in lch, lch(10% 20 30deg / .4) -10%, lch(50% 60 70deg / .8))"));
    try$(_expectInvalid(_driver, "color", "color-mix(in lch, lch(10% 20 30deg / .4) 150%, lch(50% 60 70deg / .8))"));
    try$(_expectInvalid(_driver, "color", "color-mix(in lch hue, lch(10% 20 30deg), lch(50% 60 70deg))"));
    try$(_expectInvalid(_driver, "color", "color-mix(in lch shorter, lch(10% 20 30deg), lch(50% 60 70deg))"));
    try$(_expectInvalid(_driver, "color", "color-mix(in lch foo, lch(10% 20 30deg), lch(50% 60 70deg))"));
    try$(_expectInvalid(_driver, "color", "color-mix(in lch lch(10% 20 30deg), lch(50% 60 70deg))"));
    try$(_expectInvalid(_driver, "color", "color-mix(in lch, lch(10% 20 30deg) lch(50% 60 70deg))"));
    try$(_expectInvalid(_driver, "color", "color-mix(lch(10% 20 30deg), lch(50% 60 70deg), in lch)"));
    try$(_expectInvalid(_driver, "color", "color-mix(in oklch, oklch(10% 20 30deg) -10%, oklch(50% 60 70deg))"));
    try$(_expectInvalid(_driver, "color", "color-mix(in oklch, oklch(10% 20 30deg) 150%, oklch(50% 60 70deg))"));
    try$(_expectInvalid(_driver, "color", "color-mix(in oklch, oklch(10% 20 30deg / .4) -10%, oklch(50% 60 70deg / .8))"));
    try$(_expectInvalid(_driver, "color", "color-mix(in oklch, oklch(10% 20 30deg / .4) 150%, oklch(50% 60 70deg / .8))"));
    try$(_expectInvalid(_driver, "color", "color-mix(in oklch hue, oklch(10% 20 30deg), oklch(50% 60 70deg))"));
    try$(_expectInvalid(_driver, "color", "color-mix(in oklch shorter, oklch(10% 20 30deg), oklch(50% 60 70deg))"));
    try$(_expectInvalid(_driver, "color", "color-mix(in oklch foo, oklch(10% 20 30deg), oklch(50% 60 70deg))"));
    try$(_expectInvalid(_driver, "color", "color-mix(in oklch oklch(10% 20 30deg), oklch(50% 60 70deg))"));
    try$(_expectInvalid(_driver, "color", "color-mix(in oklch, oklch(10% 20 30deg) oklch(50% 60 70deg))"));
    try$(_expectInvalid(_driver, "color", "color-mix(oklch(10% 20 30deg), oklch(50% 60 70deg), in oklch)"));
    try$(_expectInvalid(_driver, "color", "color-mix(in lab, lab(10% 20 30) -10%, lab(50% 60 70))"));
    try$(_expectInvalid(_driver, "color", "color-mix(in lab, lab(10% 20 30) 150%, lab(50% 60 70))"));
    try$(_expectInvalid(_driver, "color", "color-mix(in lab, lab(10% 20 30 / .4) -10%, lab(50% 60 70 / .8))"));
    try$(_expectInvalid(_driver, "color", "color-mix(in lab, lab(10% 20 30 / .4) 150%, lab(50% 60 70 / .8))"));
    try$(_expectInvalid(_driver, "color", "color-mix(in lab longer hue, lab(10% 20 30), lab(50% 60 70))"));
    try$(_expectInvalid(_driver, "color", "color-mix(in lab lab(10% 20 30), lab(50% 60 70))"));
    try$(_expectInvalid(_driver, "color", "color-mix(in lab, lab(10% 20 30) lab(50% 60 70))"));
    try$(_expectInvalid(_driver, "color", "color-mix(lab(10% 20 30), lab(50% 60 70), in lab)"));
    try$(_expectInvalid(_driver, "color", "color-mix(in oklab, oklab(10% 20 30) -10%, oklab(50% 60 70))"));
    try$(_expectInvalid(_driver, "color", "color-mix(in oklab, oklab(10% 20 30) 150%, oklab(50% 60 70))"));
    try$(_expectInvalid(_driver, "color", "color-mix(in oklab, oklab(10% 20 30 / .4) -10%, oklab(50% 60 70 / .8))"));
    try$(_expectInvalid(_driver, "color", "color-mix(in oklab, oklab(10% 20 30 / .4) 150%, oklab(50% 60 70 / .8))"));
    try$(_expectInvalid(_driver, "color", "color-mix(in oklab longer hue, oklab(10% 20 30), oklab(50% 60 70))"));
    try$(_expectInvalid(_driver, "color", "color-mix(in oklab oklab(10% 20 30), oklab(50% 60 70))"));
    try$(_expectInvalid(_driver, "color", "color-mix(in oklab, oklab(10% 20 30) oklab(50% 60 70))"));
    try$(_expectInvalid(_driver, "color", "color-mix(oklab(10% 20 30), oklab(50% 60 70), in oklab)"));
    try$(_expectInvalid(_driver, "color", "color-mix(in srgb longer hue, color(srgb .1 .2 .3), color(srgb .5 .6 .7))"));
    try$(_expectInvalid(_driver, "color", "color-mix(in srgb color(srgb .1 .2 .3), color(srgb .5 .6 .7))"));
    try$(_expectInvalid(_driver, "color", "color-mix(in srgb, color(srgb .1 .2 .3) color(srgb .5 .6 .7))"));
    try$(_expectInvalid(_driver, "color", "color-mix(color(srgb .1 .2 .3), color(srgb .5 .6 .7), in srgb)"));
    try$(_expectInvalid(_driver, "color", "color-mix(in srgb-linear longer hue, color(srgb-linear .1 .2 .3), color(srgb-linear .5 .6 .7))"));
    try$(_expectInvalid(_driver, "color", "color-mix(in srgb-linear color(srgb-linear .1 .2 .3), color(srgb-linear .5 .6 .7))"));
    try$(_expectInvalid(_driver, "color", "color-mix(in srgb-linear, color(srgb-linear .1 .2 .3) color(srgb-linear .5 .6 .7))"));
    try$(_expectInvalid(_driver, "color", "color-mix(color(srgb-linear .1 .2 .3), color(srgb-linear .5 .6 .7), in srgb-linear)"));
    try$(_expectInvalid(_driver, "color", "color-mix(in display-p3 longer hue, color(display-p3 .1 .2 .3), color(display-p3 .5 .6 .7))"));
    try$(_expectInvalid(_driver, "color", "color-mix(in display-p3 color(display-p3 .1 .2 .3), color(display-p3 .5 .6 .7))"));
    try$(_expectInvalid(_driver, "color", "color-mix(in display-p3, color(display-p3 .1 .2 .3) color(display-p3 .5 .6 .7))"));
    try$(_expectInvalid(_driver, "color", "color-mix(color(display-p3 .1 .2 .3), color(display-p3 .5 .6 .7), in display-p3)"));
    try$(_expectInvalid(_driver, "color", "color-mix(in display-p3-linear longer hue, color(display-p3-linear .1 .2 .3), color(display-p3-linear .5 .6 .7))"));
    try$(_expectInvalid(_driver, "color", "color-mix(in display-p3-linear color(display-p3-linear .1 .2 .3), color(display-p3-linear .5 .6 .7))"));
    try$(_expectInvalid(_driver, "color", "color-mix(in display-p3-linear, color(display-p3-linear .1 .2 .3) color(display-p3-linear .5 .6 .7))"));
    try$(_expectInvalid(_driver, "color", "color-mix(color(display-p3-linear .1 .2 .3), color(display-p3-linear .5 .6 .7), in display-p3-linear)"));
    try$(_expectInvalid(_driver, "color", "color-mix(in a98-rgb longer hue, color(a98-rgb .1 .2 .3), color(a98-rgb .5 .6 .7))"));
    try$(_expectInvalid(_driver, "color", "color-mix(in a98-rgb color(a98-rgb .1 .2 .3), color(a98-rgb .5 .6 .7))"));
    try$(_expectInvalid(_driver, "color", "color-mix(in a98-rgb, color(a98-rgb .1 .2 .3) color(a98-rgb .5 .6 .7))"));
    try$(_expectInvalid(_driver, "color", "color-mix(color(a98-rgb .1 .2 .3), color(a98-rgb .5 .6 .7), in a98-rgb)"));
    try$(_expectInvalid(_driver, "color", "color-mix(in prophoto-rgb longer hue, color(prophoto-rgb .1 .2 .3), color(prophoto-rgb .5 .6 .7))"));
    try$(_expectInvalid(_driver, "color", "color-mix(in prophoto-rgb color(prophoto-rgb .1 .2 .3), color(prophoto-rgb .5 .6 .7))"));
    try$(_expectInvalid(_driver, "color", "color-mix(in prophoto-rgb, color(prophoto-rgb .1 .2 .3) color(prophoto-rgb .5 .6 .7))"));
    try$(_expectInvalid(_driver, "color", "color-mix(color(prophoto-rgb .1 .2 .3), color(prophoto-rgb .5 .6 .7), in prophoto-rgb)"));
    try$(_expectInvalid(_driver, "color", "color-mix(in rec2020 longer hue, color(rec2020 .1 .2 .3), color(rec2020 .5 .6 .7))"));
    try$(_expectInvalid(_driver, "color", "color-mix(in rec2020 color(rec2020 .1 .2 .3), color(rec2020 .5 .6 .7))"));
    try$(_expectInvalid(_driver, "color", "color-mix(in rec2020, color(rec2020 .1 .2 .3) color(rec2020 .5 .6 .7))"));
    try$(_expectInvalid(_driver, "color", "color-mix(color(rec2020 .1 .2 .3), color(rec2020 .5 .6 .7), in rec2020)"));
    try$(_expectInvalid(_driver, "color", "color-mix(in xyz longer hue, color(xyz .1 .2 .3), color(xyz .5 .6 .7))"));
    try$(_expectInvalid(_driver, "color", "color-mix(in xyz color(xyz .1 .2 .3), color(xyz .5 .6 .7))"));
    try$(_expectInvalid(_driver, "color", "color-mix(in xyz, color(xyz .1 .2 .3) color(xyz .5 .6 .7))"));
    try$(_expectInvalid(_driver, "color", "color-mix(color(xyz .1 .2 .3), color(xyz .5 .6 .7), in xyz)"));
    try$(_expectInvalid(_driver, "color", "color-mix(in xyz-d50 longer hue, color(xyz-d50 .1 .2 .3), color(xyz-d50 .5 .6 .7))"));
    try$(_expectInvalid(_driver, "color", "color-mix(in xyz-d50 color(xyz-d50 .1 .2 .3), color(xyz-d50 .5 .6 .7))"));
    try$(_expectInvalid(_driver, "color", "color-mix(in xyz-d50, color(xyz-d50 .1 .2 .3) color(xyz-d50 .5 .6 .7))"));
    try$(_expectInvalid(_driver, "color", "color-mix(color(xyz-d50 .1 .2 .3), color(xyz-d50 .5 .6 .7), in xyz-d50)"));
    try$(_expectInvalid(_driver, "color", "color-mix(in xyz-d65 longer hue, color(xyz-d65 .1 .2 .3), color(xyz-d65 .5 .6 .7))"));
    try$(_expectInvalid(_driver, "color", "color-mix(in xyz-d65 color(xyz-d65 .1 .2 .3), color(xyz-d65 .5 .6 .7))"));
    try$(_expectInvalid(_driver, "color", "color-mix(in xyz-d65, color(xyz-d65 .1 .2 .3) color(xyz-d65 .5 .6 .7))"));
    try$(_expectInvalid(_driver, "color", "color-mix(color(xyz-d65 .1 .2 .3), color(xyz-d65 .5 .6 .7), in xyz-d65)"));
    return Ok();
}

test$("vaev-style-wpt-css-css-color-parsing-color-invalid-color-mix-function-xfail", XFAIL) {
    try$(_expectInvalid(_driver, "color", "color-mix(in hsl, hsl(120deg 10% 20%) -10%, hsl(30deg 30% 40%))"));
    try$(_expectInvalid(_driver, "color", "color-mix(in hsl, hsl(120deg 10% 20%) 150%, hsl(30deg 30% 40%))"));
    try$(_expectInvalid(_driver, "color", "color-mix(in srgb, red, blue blue)"));
    try$(_expectInvalid(_driver, "color", "color-mix(in srgb, color(srgb .1 .2 .3) -10%, color(srgb .5 .6 .7))"));
    try$(_expectInvalid(_driver, "color", "color-mix(in srgb, color(srgb .1 .2 .3) 150%, color(srgb .5 .6 .7))"));
    try$(_expectInvalid(_driver, "color", "color-mix(in srgb, color(srgb .1 .2 .3 / .4) -10%, color(srgb .5 .6 .7 / .8))"));
    try$(_expectInvalid(_driver, "color", "color-mix(in srgb, color(srgb .1 .2 .3 / .4) 150%, color(srgb .5 .6 .7 / .8))"));
    try$(_expectInvalid(_driver, "color", "color-mix(in srgb-linear, color(srgb-linear .1 .2 .3) -10%, color(srgb-linear .5 .6 .7))"));
    try$(_expectInvalid(_driver, "color", "color-mix(in srgb-linear, color(srgb-linear .1 .2 .3) 150%, color(srgb-linear .5 .6 .7))"));
    try$(_expectInvalid(_driver, "color", "color-mix(in srgb-linear, color(srgb-linear .1 .2 .3 / .4) -10%, color(srgb-linear .5 .6 .7 / .8))"));
    try$(_expectInvalid(_driver, "color", "color-mix(in srgb-linear, color(srgb-linear .1 .2 .3 / .4) 150%, color(srgb-linear .5 .6 .7 / .8))"));
    try$(_expectInvalid(_driver, "color", "color-mix(in display-p3, color(display-p3 .1 .2 .3) -10%, color(display-p3 .5 .6 .7))"));
    try$(_expectInvalid(_driver, "color", "color-mix(in display-p3, color(display-p3 .1 .2 .3) 150%, color(display-p3 .5 .6 .7))"));
    try$(_expectInvalid(_driver, "color", "color-mix(in display-p3, color(display-p3 .1 .2 .3 / .4) -10%, color(display-p3 .5 .6 .7 / .8))"));
    try$(_expectInvalid(_driver, "color", "color-mix(in display-p3, color(display-p3 .1 .2 .3 / .4) 150%, color(display-p3 .5 .6 .7 / .8))"));
    try$(_expectInvalid(_driver, "color", "color-mix(in display-p3-linear, color(display-p3-linear .1 .2 .3) -10%, color(display-p3-linear .5 .6 .7))"));
    try$(_expectInvalid(_driver, "color", "color-mix(in display-p3-linear, color(display-p3-linear .1 .2 .3) 150%, color(display-p3-linear .5 .6 .7))"));
    try$(_expectInvalid(_driver, "color", "color-mix(in display-p3-linear, color(display-p3-linear .1 .2 .3 / .4) -10%, color(display-p3-linear .5 .6 .7 / .8))"));
    try$(_expectInvalid(_driver, "color", "color-mix(in display-p3-linear, color(display-p3-linear .1 .2 .3 / .4) 150%, color(display-p3-linear .5 .6 .7 / .8))"));
    try$(_expectInvalid(_driver, "color", "color-mix(in a98-rgb, color(a98-rgb .1 .2 .3) -10%, color(a98-rgb .5 .6 .7))"));
    try$(_expectInvalid(_driver, "color", "color-mix(in a98-rgb, color(a98-rgb .1 .2 .3) 150%, color(a98-rgb .5 .6 .7))"));
    try$(_expectInvalid(_driver, "color", "color-mix(in a98-rgb, color(a98-rgb .1 .2 .3 / .4) -10%, color(a98-rgb .5 .6 .7 / .8))"));
    try$(_expectInvalid(_driver, "color", "color-mix(in a98-rgb, color(a98-rgb .1 .2 .3 / .4) 150%, color(a98-rgb .5 .6 .7 / .8))"));
    try$(_expectInvalid(_driver, "color", "color-mix(in prophoto-rgb, color(prophoto-rgb .1 .2 .3) -10%, color(prophoto-rgb .5 .6 .7))"));
    try$(_expectInvalid(_driver, "color", "color-mix(in prophoto-rgb, color(prophoto-rgb .1 .2 .3) 150%, color(prophoto-rgb .5 .6 .7))"));
    try$(_expectInvalid(_driver, "color", "color-mix(in prophoto-rgb, color(prophoto-rgb .1 .2 .3 / .4) -10%, color(prophoto-rgb .5 .6 .7 / .8))"));
    try$(_expectInvalid(_driver, "color", "color-mix(in prophoto-rgb, color(prophoto-rgb .1 .2 .3 / .4) 150%, color(prophoto-rgb .5 .6 .7 / .8))"));
    try$(_expectInvalid(_driver, "color", "color-mix(in rec2020, color(rec2020 .1 .2 .3) -10%, color(rec2020 .5 .6 .7))"));
    try$(_expectInvalid(_driver, "color", "color-mix(in rec2020, color(rec2020 .1 .2 .3) 150%, color(rec2020 .5 .6 .7))"));
    try$(_expectInvalid(_driver, "color", "color-mix(in rec2020, color(rec2020 .1 .2 .3 / .4) -10%, color(rec2020 .5 .6 .7 / .8))"));
    try$(_expectInvalid(_driver, "color", "color-mix(in rec2020, color(rec2020 .1 .2 .3 / .4) 150%, color(rec2020 .5 .6 .7 / .8))"));
    try$(_expectInvalid(_driver, "color", "color-mix(in xyz, color(xyz .1 .2 .3) -10%, color(xyz .5 .6 .7))"));
    try$(_expectInvalid(_driver, "color", "color-mix(in xyz, color(xyz .1 .2 .3) 150%, color(xyz .5 .6 .7))"));
    try$(_expectInvalid(_driver, "color", "color-mix(in xyz, color(xyz .1 .2 .3 / .4) -10%, color(xyz .5 .6 .7 / .8))"));
    try$(_expectInvalid(_driver, "color", "color-mix(in xyz, color(xyz .1 .2 .3 / .4) 150%, color(xyz .5 .6 .7 / .8))"));
    try$(_expectInvalid(_driver, "color", "color-mix(in xyz-d50, color(xyz-d50 .1 .2 .3) -10%, color(xyz-d50 .5 .6 .7))"));
    try$(_expectInvalid(_driver, "color", "color-mix(in xyz-d50, color(xyz-d50 .1 .2 .3) 150%, color(xyz-d50 .5 .6 .7))"));
    try$(_expectInvalid(_driver, "color", "color-mix(in xyz-d50, color(xyz-d50 .1 .2 .3 / .4) -10%, color(xyz-d50 .5 .6 .7 / .8))"));
    try$(_expectInvalid(_driver, "color", "color-mix(in xyz-d50, color(xyz-d50 .1 .2 .3 / .4) 150%, color(xyz-d50 .5 .6 .7 / .8))"));
    try$(_expectInvalid(_driver, "color", "color-mix(in xyz-d65, color(xyz-d65 .1 .2 .3) -10%, color(xyz-d65 .5 .6 .7))"));
    try$(_expectInvalid(_driver, "color", "color-mix(in xyz-d65, color(xyz-d65 .1 .2 .3) 150%, color(xyz-d65 .5 .6 .7))"));
    try$(_expectInvalid(_driver, "color", "color-mix(in xyz-d65, color(xyz-d65 .1 .2 .3 / .4) -10%, color(xyz-d65 .5 .6 .7 / .8))"));
    try$(_expectInvalid(_driver, "color", "color-mix(in xyz-d65, color(xyz-d65 .1 .2 .3 / .4) 150%, color(xyz-d65 .5 .6 .7 / .8))"));
    try$(_expectInvalid(_driver, "color", "color-mix(in srgb, red -10%, green 50%, blue)"));
    try$(_expectInvalid(_driver, "color", "color-mix(in hsl, hsl(120deg 10% 20%) -10%, hsl(30deg 30% 40%), hsl(60deg 20% 30%))"));
    try$(_expectInvalid(_driver, "color", "color-mix(in srgb, red 150%, green 50%, blue)"));
    try$(_expectInvalid(_driver, "color", "color-mix(in hsl, hsl(120deg 10% 20%) 150%, hsl(30deg 30% 40%), hsl(60deg 20% 30%))"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-color/parsing/color-invalid-contrast-color-function.html
test$("vaev-style-wpt-css-css-color-parsing-color-invalid-contrast-color-function") {
    try$(_expectInvalid(_driver, "background-color", "contrast-color()"));
    try$(_expectInvalid(_driver, "background-color", "contrast-color(1)"));
    try$(_expectInvalid(_driver, "background-color", "contrast-color(max)"));
    try$(_expectInvalid(_driver, "background-color", "contrast-color(max max)"));
    try$(_expectInvalid(_driver, "background-color", "contrast-color(max white)"));
    try$(_expectInvalid(_driver, "background-color", "contrast-color(white white)"));
    try$(_expectInvalid(_driver, "background-color", "contrast-color(white max)"));
    try$(_expectInvalid(_driver, "background-color", "contrast-color(white min)"));
    try$(_expectInvalid(_driver, "background-color", "contrast-color(white max bar)"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-color/parsing/color-invalid-hex-color.html
test$("vaev-style-wpt-css-css-color-parsing-color-invalid-hex-color") {
    try$(_expectInvalid(_driver, "color", "#"));
    try$(_expectInvalid(_driver, "color", "#f"));
    try$(_expectInvalid(_driver, "color", "#ff"));
    try$(_expectInvalid(_driver, "color", "#fffff"));
    try$(_expectInvalid(_driver, "color", "#fffffff"));
    try$(_expectInvalid(_driver, "color", "#fffffffff"));
    return Ok();
}

test$("vaev-style-wpt-css-css-color-parsing-color-invalid-hex-color-xfail", XFAIL) {
    try$(_expectInvalid(_driver, "color", "#ffg"));
    try$(_expectInvalid(_driver, "color", "#fffg"));
    try$(_expectInvalid(_driver, "color", "#fffffg"));
    try$(_expectInvalid(_driver, "color", "#fffffffg"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-color/parsing/color-invalid-hsl.html
test$("vaev-style-wpt-css-css-color-parsing-color-invalid-hsl") {
    try$(_expectInvalid(_driver, "color", "hsl(none, none, none)"));
    try$(_expectInvalid(_driver, "color", "hsla(none, none, none, none)"));
    try$(_expectInvalid(_driver, "color", "hsla(120, 100%, 50%, none)"));
    try$(_expectInvalid(_driver, "color", "hsl(10, 50%, 0)"));
    try$(_expectInvalid(_driver, "color", "hsl(50%, 50%, 0%)"));
    try$(_expectInvalid(_driver, "color", "hsl(0, 0% 0%)"));
    try$(_expectInvalid(_driver, "color", "hsl(0, 0%, light)"));
    try$(_expectInvalid(_driver, "color", "hsl()"));
    try$(_expectInvalid(_driver, "color", "hsl(0)"));
    try$(_expectInvalid(_driver, "color", "hsl(0, 0%)"));
    try$(_expectInvalid(_driver, "color", "hsl(0, 50, 30%)"));
    try$(_expectInvalid(_driver, "color", "hsl(0, 50%, 30)"));
    try$(_expectInvalid(_driver, "color", "hsla(10, 50%, 0, 1)"));
    try$(_expectInvalid(_driver, "color", "hsla(50%, 50%, 0%, 1)"));
    try$(_expectInvalid(_driver, "color", "hsla(0, 0% 0%, 1)"));
    try$(_expectInvalid(_driver, "color", "hsla(0, 0%, light, 1)"));
    try$(_expectInvalid(_driver, "color", "hsla()"));
    try$(_expectInvalid(_driver, "color", "hsla(0)"));
    try$(_expectInvalid(_driver, "color", "hsla(0, 0%)"));
    try$(_expectInvalid(_driver, "color", "hsl(0, 50, 30%, 1)"));
    try$(_expectInvalid(_driver, "color", "hsl(0, 50%, 30, 1)"));
    return Ok();
}

test$("vaev-style-wpt-css-css-color-parsing-color-invalid-hsl-xfail", XFAIL) {
    try$(_expectInvalid(_driver, "color", "hsl(none, 100%, 50%)"));
    try$(_expectInvalid(_driver, "color", "hsla(0, 0%, 0%, 1, 0%)"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-color/parsing/color-invalid-hwb.html
test$("vaev-style-wpt-css-css-color-parsing-color-invalid-hwb") {
    try$(_expectInvalid(_driver, "color", "hwba(120 30% 50%)"));
    try$(_expectInvalid(_driver, "color", "hwba(120 30% 50% / 0.5)"));
    try$(_expectInvalid(_driver, "color", "hwb(90deg, 50%, 50%)"));
    try$(_expectInvalid(_driver, "color", "hwb(90deg, 50%, 50%, 0.2)"));
    try$(_expectInvalid(_driver, "color", "hwb(90, 50%, 50%)"));
    try$(_expectInvalid(_driver, "color", "hwb(90, 50%, 50%, 0.2)"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-color/parsing/color-invalid-lab.html
test$("vaev-style-wpt-css-css-color-parsing-color-invalid-lab") {
    try$(_expectInvalid(_driver, "color", "lab(0% 0 0 1)"));
    try$(_expectInvalid(_driver, "color", "lab(0% 0 0 10%)"));
    try$(_expectInvalid(_driver, "color", "lab(0% 0 0deg)"));
    try$(_expectInvalid(_driver, "color", "lab(0% 0% 0deg)"));
    try$(_expectInvalid(_driver, "color", "lab(40% 0 0deg)"));
    try$(_expectInvalid(_driver, "color", "oklab(0% 0 0 1)"));
    try$(_expectInvalid(_driver, "color", "oklab(0% 0 0 10%)"));
    try$(_expectInvalid(_driver, "color", "oklab(0% 0 0deg)"));
    try$(_expectInvalid(_driver, "color", "oklab(0% 0% 0deg)"));
    try$(_expectInvalid(_driver, "color", "oklab(40% 0 0deg)"));
    try$(_expectInvalid(_driver, "color", "lch(20% 10 10deg 10)"));
    try$(_expectInvalid(_driver, "color", "lch(20% 10 10deg 10 / 0.5)"));
    try$(_expectInvalid(_driver, "color", "oklch(20% 10 10deg 10)"));
    try$(_expectInvalid(_driver, "color", "oklch(20% 10 10deg 10 / 0.5)"));
    return Ok();
}

test$("vaev-style-wpt-css-css-color-parsing-color-invalid-lab-xfail", XFAIL) {
    try$(_expectInvalid(_driver, "color", "color(lab 20% 0 10 / 50%)"));
    try$(_expectInvalid(_driver, "color", "color(oklab 20% 0 10 / 50%)"));
    try$(_expectInvalid(_driver, "color", "color(lch 20% 0 10 / 50%)"));
    try$(_expectInvalid(_driver, "color", "color(oklch 20% 0 10 / 50%)"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-color/parsing/color-invalid-named-color.html
test$("vaev-style-wpt-css-css-color-parsing-color-invalid-named-color") {
    try$(_expectInvalid(_driver, "color", ""));
    try$(_expectInvalid(_driver, "color", " /* hey */\n"));
    try$(_expectInvalid(_driver, "color", "4"));
    try$(_expectInvalid(_driver, "color", "top"));
    try$(_expectInvalid(_driver, "color", "current-Color"));
    try$(_expectInvalid(_driver, "color", "ransparent"));
    try$(_expectInvalid(_driver, "color", "aiceblue"));
    try$(_expectInvalid(_driver, "color", "aniquewhite"));
    try$(_expectInvalid(_driver, "color", "aqu"));
    try$(_expectInvalid(_driver, "color", "aquaarine"));
    try$(_expectInvalid(_driver, "color", "zure"));
    try$(_expectInvalid(_driver, "color", "bige"));
    try$(_expectInvalid(_driver, "color", "bsque"));
    try$(_expectInvalid(_driver, "color", "blak"));
    try$(_expectInvalid(_driver, "color", "blacK"));
    try$(_expectInvalid(_driver, "color", "blanchedamond"));
    try$(_expectInvalid(_driver, "color", "ble"));
    try$(_expectInvalid(_driver, "color", "bueviolet"));
    try$(_expectInvalid(_driver, "color", "brwn"));
    try$(_expectInvalid(_driver, "color", "burlwood"));
    try$(_expectInvalid(_driver, "color", "cadetlue"));
    try$(_expectInvalid(_driver, "color", "charteuse"));
    try$(_expectInvalid(_driver, "color", "chocolae"));
    try$(_expectInvalid(_driver, "color", "coal"));
    try$(_expectInvalid(_driver, "color", "cornlowerblue"));
    try$(_expectInvalid(_driver, "color", "corsilk"));
    try$(_expectInvalid(_driver, "color", "cornsilK"));
    try$(_expectInvalid(_driver, "color", "crimso"));
    try$(_expectInvalid(_driver, "color", "can"));
    try$(_expectInvalid(_driver, "color", "darkble"));
    try$(_expectInvalid(_driver, "color", "darKblue"));
    try$(_expectInvalid(_driver, "color", "darkcya"));
    try$(_expectInvalid(_driver, "color", "darKcyan"));
    try$(_expectInvalid(_driver, "color", "darkgoldenrd"));
    try$(_expectInvalid(_driver, "color", "darKgoldenrod"));
    try$(_expectInvalid(_driver, "color", "drkgray"));
    try$(_expectInvalid(_driver, "color", "darKgray"));
    try$(_expectInvalid(_driver, "color", "darkgree"));
    try$(_expectInvalid(_driver, "color", "darKgreen"));
    try$(_expectInvalid(_driver, "color", "dargrey"));
    try$(_expectInvalid(_driver, "color", "darKgrey"));
    try$(_expectInvalid(_driver, "color", "drkkhaki"));
    try$(_expectInvalid(_driver, "color", "darKkhaki"));
    try$(_expectInvalid(_driver, "color", "darkmagnta"));
    try$(_expectInvalid(_driver, "color", "darKmagenta"));
    try$(_expectInvalid(_driver, "color", "dakolivegreen"));
    try$(_expectInvalid(_driver, "color", "darKolivegreen"));
    try$(_expectInvalid(_driver, "color", "drkorange"));
    try$(_expectInvalid(_driver, "color", "darKorange"));
    try$(_expectInvalid(_driver, "color", "dakorchid"));
    try$(_expectInvalid(_driver, "color", "darKorchid"));
    try$(_expectInvalid(_driver, "color", "darkrd"));
    try$(_expectInvalid(_driver, "color", "darKred"));
    try$(_expectInvalid(_driver, "color", "darkalmon"));
    try$(_expectInvalid(_driver, "color", "darKsalmon"));
    try$(_expectInvalid(_driver, "color", "darkseagree"));
    try$(_expectInvalid(_driver, "color", "darKseagreen"));
    try$(_expectInvalid(_driver, "color", "darkslatebue"));
    try$(_expectInvalid(_driver, "color", "darKslateblue"));
    try$(_expectInvalid(_driver, "color", "darkslategry"));
    try$(_expectInvalid(_driver, "color", "darKslategray"));
    try$(_expectInvalid(_driver, "color", "darkslategre"));
    try$(_expectInvalid(_driver, "color", "darKslategrey"));
    try$(_expectInvalid(_driver, "color", "arkturquoise"));
    try$(_expectInvalid(_driver, "color", "darKturquoise"));
    try$(_expectInvalid(_driver, "color", "arkviolet"));
    try$(_expectInvalid(_driver, "color", "darKviolet"));
    try$(_expectInvalid(_driver, "color", "deppink"));
    try$(_expectInvalid(_driver, "color", "deeppinK"));
    try$(_expectInvalid(_driver, "color", "deepskyble"));
    try$(_expectInvalid(_driver, "color", "deepsKyblue"));
    try$(_expectInvalid(_driver, "color", "dmgray"));
    try$(_expectInvalid(_driver, "color", "digrey"));
    try$(_expectInvalid(_driver, "color", "dodgeblue"));
    try$(_expectInvalid(_driver, "color", "frebrick"));
    try$(_expectInvalid(_driver, "color", "firebricK"));
    try$(_expectInvalid(_driver, "color", "floalwhite"));
    try$(_expectInvalid(_driver, "color", "foretgreen"));
    try$(_expectInvalid(_driver, "color", "uchsia"));
    try$(_expectInvalid(_driver, "color", "gainsoro"));
    try$(_expectInvalid(_driver, "color", "gostwhite"));
    try$(_expectInvalid(_driver, "color", "old"));
    try$(_expectInvalid(_driver, "color", "goldenro"));
    try$(_expectInvalid(_driver, "color", "gry"));
    try$(_expectInvalid(_driver, "color", "reen"));
    try$(_expectInvalid(_driver, "color", "geenyellow"));
    try$(_expectInvalid(_driver, "color", "gey"));
    try$(_expectInvalid(_driver, "color", "hoeydew"));
    try$(_expectInvalid(_driver, "color", "hotink"));
    try$(_expectInvalid(_driver, "color", "hotpinK"));
    try$(_expectInvalid(_driver, "color", "indianed"));
    try$(_expectInvalid(_driver, "color", "idigo"));
    try$(_expectInvalid(_driver, "color", "ivry"));
    try$(_expectInvalid(_driver, "color", "khai"));
    try$(_expectInvalid(_driver, "color", "Khaki"));
    try$(_expectInvalid(_driver, "color", "avender"));
    try$(_expectInvalid(_driver, "color", "avenderblush"));
    try$(_expectInvalid(_driver, "color", "lawgreen"));
    try$(_expectInvalid(_driver, "color", "lemonchffon"));
    try$(_expectInvalid(_driver, "color", "lightlue"));
    try$(_expectInvalid(_driver, "color", "lightcora"));
    try$(_expectInvalid(_driver, "color", "lightcyn"));
    try$(_expectInvalid(_driver, "color", "lightgoldenodyellow"));
    try$(_expectInvalid(_driver, "color", "ightgray"));
    try$(_expectInvalid(_driver, "color", "ligtgreen"));
    try$(_expectInvalid(_driver, "color", "lihtgrey"));
    try$(_expectInvalid(_driver, "color", "ligtpink"));
    try$(_expectInvalid(_driver, "color", "lightpinK"));
    try$(_expectInvalid(_driver, "color", "lightsalmo"));
    try$(_expectInvalid(_driver, "color", "lightseagree"));
    try$(_expectInvalid(_driver, "color", "lightsyblue"));
    try$(_expectInvalid(_driver, "color", "lightsKyblue"));
    try$(_expectInvalid(_driver, "color", "lightslatgray"));
    try$(_expectInvalid(_driver, "color", "lightslaterey"));
    try$(_expectInvalid(_driver, "color", "lightsteelbue"));
    try$(_expectInvalid(_driver, "color", "lightellow"));
    try$(_expectInvalid(_driver, "color", "lim"));
    try$(_expectInvalid(_driver, "color", "limgreen"));
    try$(_expectInvalid(_driver, "color", "inen"));
    try$(_expectInvalid(_driver, "color", "maenta"));
    try$(_expectInvalid(_driver, "color", "maron"));
    try$(_expectInvalid(_driver, "color", "mediumaqamarine"));
    try$(_expectInvalid(_driver, "color", "mediumblu"));
    try$(_expectInvalid(_driver, "color", "mediumrchid"));
    try$(_expectInvalid(_driver, "color", "mediumprple"));
    try$(_expectInvalid(_driver, "color", "mediumsegreen"));
    try$(_expectInvalid(_driver, "color", "medumslateblue"));
    try$(_expectInvalid(_driver, "color", "mediumsprnggreen"));
    try$(_expectInvalid(_driver, "color", "mediuturquoise"));
    try$(_expectInvalid(_driver, "color", "mediumioletred"));
    try$(_expectInvalid(_driver, "color", "mdnightblue"));
    try$(_expectInvalid(_driver, "color", "mintcrea"));
    try$(_expectInvalid(_driver, "color", "istyrose"));
    try$(_expectInvalid(_driver, "color", "moccsin"));
    try$(_expectInvalid(_driver, "color", "naajowhite"));
    try$(_expectInvalid(_driver, "color", "nay"));
    try$(_expectInvalid(_driver, "color", "oldlae"));
    try$(_expectInvalid(_driver, "color", "oliv"));
    try$(_expectInvalid(_driver, "color", "olivedab"));
    try$(_expectInvalid(_driver, "color", "orane"));
    try$(_expectInvalid(_driver, "color", "orangere"));
    try$(_expectInvalid(_driver, "color", "rchid"));
    try$(_expectInvalid(_driver, "color", "palegldenrod"));
    try$(_expectInvalid(_driver, "color", "paegreen"));
    try$(_expectInvalid(_driver, "color", "paleturuoise"));
    try$(_expectInvalid(_driver, "color", "palevioltred"));
    try$(_expectInvalid(_driver, "color", "papyawhip"));
    try$(_expectInvalid(_driver, "color", "peachpff"));
    try$(_expectInvalid(_driver, "color", "per"));
    try$(_expectInvalid(_driver, "color", "ink"));
    try$(_expectInvalid(_driver, "color", "pinK"));
    try$(_expectInvalid(_driver, "color", "pum"));
    try$(_expectInvalid(_driver, "color", "powderble"));
    try$(_expectInvalid(_driver, "color", "purpl"));
    try$(_expectInvalid(_driver, "color", "ed"));
    try$(_expectInvalid(_driver, "color", "rosyrown"));
    try$(_expectInvalid(_driver, "color", "royallue"));
    try$(_expectInvalid(_driver, "color", "sadlebrown"));
    try$(_expectInvalid(_driver, "color", "salmn"));
    try$(_expectInvalid(_driver, "color", "sandyrown"));
    try$(_expectInvalid(_driver, "color", "seagren"));
    try$(_expectInvalid(_driver, "color", "seashel"));
    try$(_expectInvalid(_driver, "color", "sinna"));
    try$(_expectInvalid(_driver, "color", "siler"));
    try$(_expectInvalid(_driver, "color", "skybue"));
    try$(_expectInvalid(_driver, "color", "sKyblue"));
    try$(_expectInvalid(_driver, "color", "slatelue"));
    try$(_expectInvalid(_driver, "color", "slategay"));
    try$(_expectInvalid(_driver, "color", "slategry"));
    try$(_expectInvalid(_driver, "color", "snw"));
    try$(_expectInvalid(_driver, "color", "sprnggreen"));
    try$(_expectInvalid(_driver, "color", "seelblue"));
    try$(_expectInvalid(_driver, "color", "ta"));
    try$(_expectInvalid(_driver, "color", "tel"));
    try$(_expectInvalid(_driver, "color", "thistl"));
    try$(_expectInvalid(_driver, "color", "toato"));
    try$(_expectInvalid(_driver, "color", "turquose"));
    try$(_expectInvalid(_driver, "color", "violt"));
    try$(_expectInvalid(_driver, "color", "whet"));
    try$(_expectInvalid(_driver, "color", "whit"));
    try$(_expectInvalid(_driver, "color", "whitemoke"));
    try$(_expectInvalid(_driver, "color", "whitesmoKe"));
    try$(_expectInvalid(_driver, "color", "yelow"));
    try$(_expectInvalid(_driver, "color", "yellwgreen"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-color/parsing/color-invalid-relative-color.html
test$("vaev-style-wpt-css-css-color-parsing-color-invalid-relative-color") {
    try$(_expectInvalid(_driver, "color", "rgb(from rebeccapurple r 10deg 10)"));
    try$(_expectInvalid(_driver, "color", "rgb(from rebeccapurple r 10 10deg)"));
    try$(_expectInvalid(_driver, "color", "rgb(from rebeccapurple 10deg g b)"));
    try$(_expectInvalid(_driver, "color", "rgb(from rgb(10%, 20%, 30%, 40%) r 10deg 10)"));
    try$(_expectInvalid(_driver, "color", "rgb(from rgb(10%, 20%, 30%, 40%) r 10 10deg)"));
    try$(_expectInvalid(_driver, "color", "rgb(from rgb(10%, 20%, 30%, 40%) 10deg g b)"));
    try$(_expectInvalid(_driver, "color", "rgb(from rebeccapurple red g b)"));
    try$(_expectInvalid(_driver, "color", "rgb(from rebeccapurple l g b)"));
    try$(_expectInvalid(_driver, "color", "rgb(from rebeccapurple h g b)"));
    try$(_expectInvalid(_driver, "color", "rgb(from rebeccapurple, r, g, b)"));
    try$(_expectInvalid(_driver, "color", "rgba(from rgb(10%, 20%, 30%, 40%), r, g, b, alpha)"));
    try$(_expectInvalid(_driver, "color", "rgb(from rebeccapurple calc(r + 1%) g b)"));
    try$(_expectInvalid(_driver, "color", "rgb(0 0 0 / alpha)"));
    try$(_expectInvalid(_driver, "color", "hsl(from rebeccapurple 10% s l)"));
    try$(_expectInvalid(_driver, "color", "hsl(from rgb(10%, 20%, 30%, 40%) 10% s l)"));
    try$(_expectInvalid(_driver, "color", "hsl(from rebeccapurple hue s l)"));
    try$(_expectInvalid(_driver, "color", "hsl(from rebeccapurple x s l)"));
    try$(_expectInvalid(_driver, "color", "hsl(from rebeccapurple h g b)"));
    try$(_expectInvalid(_driver, "color", "hsl(from rebeccapurple, h, s, l)"));
    try$(_expectInvalid(_driver, "color", "hsla(from rgb(10%, 20%, 30%, 40%), h, s, l, alpha)"));
    try$(_expectInvalid(_driver, "color", "hsl(from rebeccapurple calc(h + 1deg) s l)"));
    try$(_expectInvalid(_driver, "color", "hsl(from rebeccapurple calc(h + 1%) s l)"));
    try$(_expectInvalid(_driver, "color", "hwb(from rebeccapurple 10% w b)"));
    try$(_expectInvalid(_driver, "color", "hwb(from rgb(10%, 20%, 30%, 40%) 10% w b)"));
    try$(_expectInvalid(_driver, "color", "hwb(from rebeccapurple hue w b)"));
    try$(_expectInvalid(_driver, "color", "hwb(from rebeccapurple x w b)"));
    try$(_expectInvalid(_driver, "color", "hwb(from rebeccapurple h g b)"));
    try$(_expectInvalid(_driver, "color", "hwb(from rebeccapurple calc(h + 1deg) w b)"));
    try$(_expectInvalid(_driver, "color", "hwb(from rebeccapurple calc(h + 1%) w b)"));
    try$(_expectInvalid(_driver, "color", "lab(from lab(.25 20 50) l 10deg 10)"));
    try$(_expectInvalid(_driver, "color", "lab(from lab(.25 20 50) l 10 10deg)"));
    try$(_expectInvalid(_driver, "color", "lab(from lab(.25 20 50) 10deg a b)"));
    try$(_expectInvalid(_driver, "color", "lab(from lab(.25 20 50 / 40%) l 10deg 10)"));
    try$(_expectInvalid(_driver, "color", "lab(from lab(.25 20 50 / 40%) l 10 10deg)"));
    try$(_expectInvalid(_driver, "color", "lab(from lab(.25 20 50 / 40%) 10deg a b)"));
    try$(_expectInvalid(_driver, "color", "lab(from lab(.25 20 50) lightness a b)"));
    try$(_expectInvalid(_driver, "color", "lab(from lab(.25 20 50) x a b)"));
    try$(_expectInvalid(_driver, "color", "lab(from lab(.25 20 50) h g b)"));
    try$(_expectInvalid(_driver, "color", "lab(from lab(.25 20 50) l calc(a + 1%) b)"));
    try$(_expectInvalid(_driver, "color", "oklab(from oklab(.25 20 50) l 10deg 10)"));
    try$(_expectInvalid(_driver, "color", "oklab(from oklab(.25 20 50) l 10 10deg)"));
    try$(_expectInvalid(_driver, "color", "oklab(from oklab(.25 20 50) 10deg a b)"));
    try$(_expectInvalid(_driver, "color", "oklab(from oklab(.25 20 50 / 40%) l 10deg 10)"));
    try$(_expectInvalid(_driver, "color", "oklab(from oklab(.25 20 50 / 40%) l 10 10deg)"));
    try$(_expectInvalid(_driver, "color", "oklab(from oklab(.25 20 50 / 40%) 10deg a b)"));
    try$(_expectInvalid(_driver, "color", "oklab(from oklab(.25 20 50) lightness a b)"));
    try$(_expectInvalid(_driver, "color", "oklab(from oklab(.25 20 50) x a b)"));
    try$(_expectInvalid(_driver, "color", "oklab(from oklab(.25 20 50) h g b)"));
    try$(_expectInvalid(_driver, "color", "oklab(from oklab(.25 20 50) l calc(a + 1%) b)"));
    try$(_expectInvalid(_driver, "color", "lch(from lch(.70 45 30) l 10deg h)"));
    try$(_expectInvalid(_driver, "color", "lch(from lch(.70 45 30) l c 10%)"));
    try$(_expectInvalid(_driver, "color", "lch(from lch(.70 45 30) 10deg c h)"));
    try$(_expectInvalid(_driver, "color", "lch(from lch(.70 45 30 / 40%) l 10deg h)"));
    try$(_expectInvalid(_driver, "color", "lch(from lch(.70 45 30 / 40%) l c 10%)"));
    try$(_expectInvalid(_driver, "color", "lch(from lch(.70 45 30 / 40%) 10deg c h)"));
    try$(_expectInvalid(_driver, "color", "lch(from lch(.70 45 30) lightness c h)"));
    try$(_expectInvalid(_driver, "color", "lch(from lch(.70 45 30) x c h)"));
    try$(_expectInvalid(_driver, "color", "lch(from lch(.70 45 30) l g b)"));
    try$(_expectInvalid(_driver, "color", "lch(from lch(.70 45 30) l c calc(h + 1%))"));
    try$(_expectInvalid(_driver, "color", "lch(from lch(.70 45 30) l c calc(h + 1deg))"));
    try$(_expectInvalid(_driver, "color", "oklch(from oklch(.70 45 30) l 10deg h)"));
    try$(_expectInvalid(_driver, "color", "oklch(from oklch(.70 45 30) l c 10%)"));
    try$(_expectInvalid(_driver, "color", "oklch(from oklch(.70 45 30) 10deg c h)"));
    try$(_expectInvalid(_driver, "color", "oklch(from oklch(.70 45 30 / 40%) l 10deg h)"));
    try$(_expectInvalid(_driver, "color", "oklch(from oklch(.70 45 30 / 40%) l c 10%)"));
    try$(_expectInvalid(_driver, "color", "oklch(from oklch(.70 45 30 / 40%) 10deg c h)"));
    try$(_expectInvalid(_driver, "color", "oklch(from oklch(.70 45 30) lightness c h)"));
    try$(_expectInvalid(_driver, "color", "oklch(from oklch(.70 45 30) x c h)"));
    try$(_expectInvalid(_driver, "color", "oklch(from oklch(.70 45 30) l g b)"));
    try$(_expectInvalid(_driver, "color", "oklch(from oklch(.70 45 30) l c calc(h + 1%))"));
    try$(_expectInvalid(_driver, "color", "oklch(from oklch(.70 45 30) l c calc(h + 1deg))"));
    return Ok();
}

test$("vaev-style-wpt-css-css-color-parsing-color-invalid-relative-color-xfail", XFAIL) {
    try$(_expectInvalid(_driver, "color", "color(from color(srgb 0.7 0.5 0.3) srgb 10deg g b)"));
    try$(_expectInvalid(_driver, "color", "color(from color(srgb 0.7 0.5 0.3) srgb r 10deg b)"));
    try$(_expectInvalid(_driver, "color", "color(from color(srgb 0.7 0.5 0.3) srgb r g 10deg)"));
    try$(_expectInvalid(_driver, "color", "color(from color(srgb 0.7 0.5 0.3) srgb r g b / 10deg)"));
    try$(_expectInvalid(_driver, "color", "color(from color(srgb 0.7 0.5 0.3) srgb red g b)"));
    try$(_expectInvalid(_driver, "color", "color(from color(srgb 0.7 0.5 0.3) srgb x g b)"));
    try$(_expectInvalid(_driver, "color", "color(from color(srgb 0.7 0.5 0.3) srgb l g b)"));
    try$(_expectInvalid(_driver, "color", "color(from color(srgb 0.7 0.5 0.3) srgb x y z)"));
    try$(_expectInvalid(_driver, "color", "color(from color(srgb 0.7 0.5 0.3) srgb calc(r + 1%) g b)"));
    try$(_expectInvalid(_driver, "color", "color(from color(srgb-linear 0.7 0.5 0.3) srgb-linear 10deg g b)"));
    try$(_expectInvalid(_driver, "color", "color(from color(srgb-linear 0.7 0.5 0.3) srgb-linear r 10deg b)"));
    try$(_expectInvalid(_driver, "color", "color(from color(srgb-linear 0.7 0.5 0.3) srgb-linear r g 10deg)"));
    try$(_expectInvalid(_driver, "color", "color(from color(srgb-linear 0.7 0.5 0.3) srgb-linear r g b / 10deg)"));
    try$(_expectInvalid(_driver, "color", "color(from color(srgb-linear 0.7 0.5 0.3) srgb-linear red g b)"));
    try$(_expectInvalid(_driver, "color", "color(from color(srgb-linear 0.7 0.5 0.3) srgb-linear x g b)"));
    try$(_expectInvalid(_driver, "color", "color(from color(srgb-linear 0.7 0.5 0.3) srgb-linear l g b)"));
    try$(_expectInvalid(_driver, "color", "color(from color(srgb-linear 0.7 0.5 0.3) srgb-linear x y z)"));
    try$(_expectInvalid(_driver, "color", "color(from color(srgb-linear 0.7 0.5 0.3) srgb-linear calc(r + 1%) g b)"));
    try$(_expectInvalid(_driver, "color", "color(from color(a98-rgb 0.7 0.5 0.3) a98-rgb 10deg g b)"));
    try$(_expectInvalid(_driver, "color", "color(from color(a98-rgb 0.7 0.5 0.3) a98-rgb r 10deg b)"));
    try$(_expectInvalid(_driver, "color", "color(from color(a98-rgb 0.7 0.5 0.3) a98-rgb r g 10deg)"));
    try$(_expectInvalid(_driver, "color", "color(from color(a98-rgb 0.7 0.5 0.3) a98-rgb r g b / 10deg)"));
    try$(_expectInvalid(_driver, "color", "color(from color(a98-rgb 0.7 0.5 0.3) a98-rgb red g b)"));
    try$(_expectInvalid(_driver, "color", "color(from color(a98-rgb 0.7 0.5 0.3) a98-rgb x g b)"));
    try$(_expectInvalid(_driver, "color", "color(from color(a98-rgb 0.7 0.5 0.3) a98-rgb l g b)"));
    try$(_expectInvalid(_driver, "color", "color(from color(a98-rgb 0.7 0.5 0.3) a98-rgb x y z)"));
    try$(_expectInvalid(_driver, "color", "color(from color(a98-rgb 0.7 0.5 0.3) a98-rgb calc(r + 1%) g b)"));
    try$(_expectInvalid(_driver, "color", "color(from color(rec2020 0.7 0.5 0.3) rec2020 10deg g b)"));
    try$(_expectInvalid(_driver, "color", "color(from color(rec2020 0.7 0.5 0.3) rec2020 r 10deg b)"));
    try$(_expectInvalid(_driver, "color", "color(from color(rec2020 0.7 0.5 0.3) rec2020 r g 10deg)"));
    try$(_expectInvalid(_driver, "color", "color(from color(rec2020 0.7 0.5 0.3) rec2020 r g b / 10deg)"));
    try$(_expectInvalid(_driver, "color", "color(from color(rec2020 0.7 0.5 0.3) rec2020 red g b)"));
    try$(_expectInvalid(_driver, "color", "color(from color(rec2020 0.7 0.5 0.3) rec2020 x g b)"));
    try$(_expectInvalid(_driver, "color", "color(from color(rec2020 0.7 0.5 0.3) rec2020 l g b)"));
    try$(_expectInvalid(_driver, "color", "color(from color(rec2020 0.7 0.5 0.3) rec2020 x y z)"));
    try$(_expectInvalid(_driver, "color", "color(from color(rec2020 0.7 0.5 0.3) rec2020 calc(r + 1%) g b)"));
    try$(_expectInvalid(_driver, "color", "color(from color(prophoto-rgb 0.7 0.5 0.3) prophoto-rgb 10deg g b)"));
    try$(_expectInvalid(_driver, "color", "color(from color(prophoto-rgb 0.7 0.5 0.3) prophoto-rgb r 10deg b)"));
    try$(_expectInvalid(_driver, "color", "color(from color(prophoto-rgb 0.7 0.5 0.3) prophoto-rgb r g 10deg)"));
    try$(_expectInvalid(_driver, "color", "color(from color(prophoto-rgb 0.7 0.5 0.3) prophoto-rgb r g b / 10deg)"));
    try$(_expectInvalid(_driver, "color", "color(from color(prophoto-rgb 0.7 0.5 0.3) prophoto-rgb red g b)"));
    try$(_expectInvalid(_driver, "color", "color(from color(prophoto-rgb 0.7 0.5 0.3) prophoto-rgb x g b)"));
    try$(_expectInvalid(_driver, "color", "color(from color(prophoto-rgb 0.7 0.5 0.3) prophoto-rgb l g b)"));
    try$(_expectInvalid(_driver, "color", "color(from color(prophoto-rgb 0.7 0.5 0.3) prophoto-rgb x y z)"));
    try$(_expectInvalid(_driver, "color", "color(from color(prophoto-rgb 0.7 0.5 0.3) prophoto-rgb calc(r + 1%) g b)"));
    try$(_expectInvalid(_driver, "color", "color(from color(display-p3 0.7 0.5 0.3) display-p3 10deg g b)"));
    try$(_expectInvalid(_driver, "color", "color(from color(display-p3 0.7 0.5 0.3) display-p3 r 10deg b)"));
    try$(_expectInvalid(_driver, "color", "color(from color(display-p3 0.7 0.5 0.3) display-p3 r g 10deg)"));
    try$(_expectInvalid(_driver, "color", "color(from color(display-p3 0.7 0.5 0.3) display-p3 r g b / 10deg)"));
    try$(_expectInvalid(_driver, "color", "color(from color(display-p3 0.7 0.5 0.3) display-p3 red g b)"));
    try$(_expectInvalid(_driver, "color", "color(from color(display-p3 0.7 0.5 0.3) display-p3 x g b)"));
    try$(_expectInvalid(_driver, "color", "color(from color(display-p3 0.7 0.5 0.3) display-p3 l g b)"));
    try$(_expectInvalid(_driver, "color", "color(from color(display-p3 0.7 0.5 0.3) display-p3 x y z)"));
    try$(_expectInvalid(_driver, "color", "color(from color(display-p3 0.7 0.5 0.3) display-p3 calc(r + 1%) g b)"));
    try$(_expectInvalid(_driver, "color", "color(from color(display-p3-linear 0.7 0.5 0.3) display-p3-linear 10deg g b)"));
    try$(_expectInvalid(_driver, "color", "color(from color(display-p3-linear 0.7 0.5 0.3) display-p3-linear r 10deg b)"));
    try$(_expectInvalid(_driver, "color", "color(from color(display-p3-linear 0.7 0.5 0.3) display-p3-linear r g 10deg)"));
    try$(_expectInvalid(_driver, "color", "color(from color(display-p3-linear 0.7 0.5 0.3) display-p3-linear r g b / 10deg)"));
    try$(_expectInvalid(_driver, "color", "color(from color(display-p3-linear 0.7 0.5 0.3) display-p3-linear red g b)"));
    try$(_expectInvalid(_driver, "color", "color(from color(display-p3-linear 0.7 0.5 0.3) display-p3-linear x g b)"));
    try$(_expectInvalid(_driver, "color", "color(from color(display-p3-linear 0.7 0.5 0.3) display-p3-linear l g b)"));
    try$(_expectInvalid(_driver, "color", "color(from color(display-p3-linear 0.7 0.5 0.3) display-p3-linear x y z)"));
    try$(_expectInvalid(_driver, "color", "color(from color(display-p3-linear 0.7 0.5 0.3) display-p3-linear calc(r + 1%) g b)"));
    try$(_expectInvalid(_driver, "color", "color(from color(xyz 7 -20.5 100) xyz 10deg y z)"));
    try$(_expectInvalid(_driver, "color", "color(from color(xyz 7 -20.5 100) xyz x 10deg z)"));
    try$(_expectInvalid(_driver, "color", "color(from color(xyz 7 -20.5 100) xyz x y 10deg)"));
    try$(_expectInvalid(_driver, "color", "color(from color(xyz 7 -20.5 100) xyz x y z / 10deg)"));
    try$(_expectInvalid(_driver, "color", "color(from color(xyz 7 -20.5 100) xyz red y)"));
    try$(_expectInvalid(_driver, "color", "color(from color(xyz 7 -20.5 100) xyz r y z)"));
    try$(_expectInvalid(_driver, "color", "color(from color(xyz 7 -20.5 100) xyz l y z)"));
    try$(_expectInvalid(_driver, "color", "color(from color(xyz 7 -20.5 100) xyz r g b)"));
    try$(_expectInvalid(_driver, "color", "color(from color(xyz 7 -20.5 100) xyz calc(x + 1%) y z)"));
    try$(_expectInvalid(_driver, "color", "color(from color(xyz-d50 7 -20.5 100) xyz-d50 10deg y z)"));
    try$(_expectInvalid(_driver, "color", "color(from color(xyz-d50 7 -20.5 100) xyz-d50 x 10deg z)"));
    try$(_expectInvalid(_driver, "color", "color(from color(xyz-d50 7 -20.5 100) xyz-d50 x y 10deg)"));
    try$(_expectInvalid(_driver, "color", "color(from color(xyz-d50 7 -20.5 100) xyz-d50 x y z / 10deg)"));
    try$(_expectInvalid(_driver, "color", "color(from color(xyz-d50 7 -20.5 100) xyz-d50 red y)"));
    try$(_expectInvalid(_driver, "color", "color(from color(xyz-d50 7 -20.5 100) xyz-d50 r y z)"));
    try$(_expectInvalid(_driver, "color", "color(from color(xyz-d50 7 -20.5 100) xyz-d50 l y z)"));
    try$(_expectInvalid(_driver, "color", "color(from color(xyz-d50 7 -20.5 100) xyz-d50 r g b)"));
    try$(_expectInvalid(_driver, "color", "color(from color(xyz-d50 7 -20.5 100) xyz-d50 calc(x + 1%) y z)"));
    try$(_expectInvalid(_driver, "color", "color(from color(xyz-d65 7 -20.5 100) xyz-d65 10deg y z)"));
    try$(_expectInvalid(_driver, "color", "color(from color(xyz-d65 7 -20.5 100) xyz-d65 x 10deg z)"));
    try$(_expectInvalid(_driver, "color", "color(from color(xyz-d65 7 -20.5 100) xyz-d65 x y 10deg)"));
    try$(_expectInvalid(_driver, "color", "color(from color(xyz-d65 7 -20.5 100) xyz-d65 x y z / 10deg)"));
    try$(_expectInvalid(_driver, "color", "color(from color(xyz-d65 7 -20.5 100) xyz-d65 red y)"));
    try$(_expectInvalid(_driver, "color", "color(from color(xyz-d65 7 -20.5 100) xyz-d65 r y z)"));
    try$(_expectInvalid(_driver, "color", "color(from color(xyz-d65 7 -20.5 100) xyz-d65 l y z)"));
    try$(_expectInvalid(_driver, "color", "color(from color(xyz-d65 7 -20.5 100) xyz-d65 r g b)"));
    try$(_expectInvalid(_driver, "color", "color(from color(xyz-d65 7 -20.5 100) xyz-d65 calc(x + 1%) y z)"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-color/parsing/color-invalid-rgb.html
test$("vaev-style-wpt-css-css-color-parsing-color-invalid-rgb") {
    try$(_expectInvalid(_driver, "color", "rgb(none, none, none)"));
    try$(_expectInvalid(_driver, "color", "rgba(none, none, none, none)"));
    try$(_expectInvalid(_driver, "color", "rgb(128, 0, none)"));
    try$(_expectInvalid(_driver, "color", "rgb(255, 255, 255, none)"));
    try$(_expectInvalid(_driver, "color", "rgb(10%, 50%, 0)"));
    try$(_expectInvalid(_driver, "color", "rgb(255, 50%, 0%)"));
    try$(_expectInvalid(_driver, "color", "rgb(0, 0 0)"));
    try$(_expectInvalid(_driver, "color", "rgb(0 0, 0)"));
    try$(_expectInvalid(_driver, "color", "rgb(,0, 0, 0)"));
    try$(_expectInvalid(_driver, "color", "rgb(0, 0, 0,)"));
    try$(_expectInvalid(_driver, "color", "rgb(0, 0,, 0)"));
    try$(_expectInvalid(_driver, "color", "rgb(0, 0, 0deg)"));
    try$(_expectInvalid(_driver, "color", "rgb(0, 0, light)"));
    try$(_expectInvalid(_driver, "color", "rgb()"));
    try$(_expectInvalid(_driver, "color", "rgb(0)"));
    try$(_expectInvalid(_driver, "color", "rgb(0, 0)"));
    try$(_expectInvalid(_driver, "color", "rgb(0%)"));
    try$(_expectInvalid(_driver, "color", "rgb(0%, 0%)"));
    try$(_expectInvalid(_driver, "color", "rgba(10%, 50%, 0, 1)"));
    try$(_expectInvalid(_driver, "color", "rgba(255, 50%, 0%, 1)"));
    try$(_expectInvalid(_driver, "color", "rgba(0, 0, 0 0)"));
    try$(_expectInvalid(_driver, "color", "rgba(0, 0, 0, 0deg)"));
    try$(_expectInvalid(_driver, "color", "rgba(0, 0, 0, light)"));
    try$(_expectInvalid(_driver, "color", "rgba()"));
    try$(_expectInvalid(_driver, "color", "rgba(0)"));
    try$(_expectInvalid(_driver, "color", "rgba(0%)"));
    try$(_expectInvalid(_driver, "color", "rgba(0%, 0%)"));
    try$(_expectInvalid(_driver, "color", "rgb(257, 0, 5 / 0)"));
    return Ok();
}

test$("vaev-style-wpt-css-css-color-parsing-color-invalid-rgb-xfail", XFAIL) {
    try$(_expectInvalid(_driver, "color", "rgba(0, 0, 0, 0, 0)"));
    try$(_expectInvalid(_driver, "color", "rgba(0%, 0%, 0%, 0%, 0%)"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-color/parsing/color-invalid.html
test$("vaev-style-wpt-css-css-color-parsing-color-invalid") {
    try$(_expectInvalid(_driver, "color", "auto"));
    try$(_expectInvalid(_driver, "color", "123"));
    try$(_expectInvalid(_driver, "color", "#12"));
    try$(_expectInvalid(_driver, "color", "#123456789"));
    try$(_expectInvalid(_driver, "color", "rgb"));
    try$(_expectInvalid(_driver, "color", "rgb(1)"));
    try$(_expectInvalid(_driver, "color", "hsla(1,2,3,4,5)"));
    try$(_expectInvalid(_driver, "color", "rgb(10%, 20, 30%)"));
    try$(_expectInvalid(_driver, "color", "rgba(-2, 300, 400%, -0.5)"));
    try$(_expectInvalid(_driver, "color", "rgb(sign(0% - 0px), 0, 0)"));
    try$(_expectInvalid(_driver, "color", "hsl(calc(0.56turn * -0.43turn), 47%, 4884.6%)"));
    return Ok();
}

test$("vaev-style-wpt-css-css-color-parsing-color-invalid-xfail", XFAIL) {
    try$(_expectInvalid(_driver, "color", "rgb(1,2,3,4,5)"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-color/parsing/color-valid-hsl.html
test$("vaev-style-wpt-css-css-color-parsing-color-valid-hsl") {
    try$(_expectSerialized(_driver, "color", "hsl(120 30% 50%)", "rgb(89, 166, 89)"));
    try$(_expectSerialized(_driver, "color", "hsl(0 0% 0%)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "color", "hsl(0 0% 0% / 0)", "rgba(0, 0, 0, 0)"));
    try$(_expectSerialized(_driver, "color", "hsla(0 0% 0%)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "color", "hsla(0 0% 0% / 0)", "rgba(0, 0, 0, 0)"));
    try$(_expectSerialized(_driver, "color", "hsl(120 0% 0%)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "color", "hsl(120 80% 0%)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "color", "hsl(120 0% 50%)", "rgb(128, 128, 128)"));
    try$(_expectSerialized(_driver, "color", "hsl(120 100% 50% / 0)", "rgba(0, 255, 0, 0)"));
    try$(_expectSerialized(_driver, "color", "hsl(0 100% 50%)", "rgb(255, 0, 0)"));
    try$(_expectSerialized(_driver, "color", "hsl(0 -50% 40%)", "rgb(102, 102, 102)"));
    try$(_expectSerialized(_driver, "color", "hsl(0 -50 40%)", "rgb(102, 102, 102)"));
    return Ok();
}

test$("vaev-style-wpt-css-css-color-parsing-color-valid-hsl-xfail", XFAIL) {
    try$(_expectSerialized(_driver, "color", "hsl(120 30% 50% / 0.5)", "rgba(89, 166, 89, 0.5)"));
    try$(_expectSerialized(_driver, "color", "hsl(none none none)", "hsl(none none none)"));
    try$(_expectSerialized(_driver, "color", "hsl(none none none / none)", "hsl(none none none / none)"));
    try$(_expectSerialized(_driver, "color", "hsla(none none none)", "hsl(none none none)"));
    try$(_expectSerialized(_driver, "color", "hsla(none none none / none)", "hsl(none none none / none)"));
    try$(_expectSerialized(_driver, "color", "hsl(120 none none)", "hsl(120 none none)"));
    try$(_expectSerialized(_driver, "color", "hsl(120 80% none)", "hsl(120 80 none)"));
    try$(_expectSerialized(_driver, "color", "hsl(120 none 50%)", "hsl(120 none 50)"));
    try$(_expectSerialized(_driver, "color", "hsl(120 100% 50% / none)", "hsl(120 100 50 / none)"));
    try$(_expectSerialized(_driver, "color", "hsl(none 100% 50%)", "hsl(none 100 50)"));
    try$(_expectSerialized(_driver, "color", "hsl(120 30 50)", "rgb(89, 166, 89)"));
    try$(_expectSerialized(_driver, "color", "hsl(120 30 50 / 0.5)", "rgba(89, 166, 89, 0.5)"));
    try$(_expectSerialized(_driver, "color", "hsl(120 30% 50)", "rgb(89, 166, 89)"));
    try$(_expectSerialized(_driver, "color", "hsl(120 30% 50 / 0.5)", "rgba(89, 166, 89, 0.5)"));
    try$(_expectSerialized(_driver, "color", "hsl(120 30 50%)", "rgb(89, 166, 89)"));
    try$(_expectSerialized(_driver, "color", "hsl(120 30 50% / 0.5)", "rgba(89, 166, 89, 0.5)"));
    try$(_expectSerialized(_driver, "color", "hsl(120 none 50)", "hsl(120 none 50)"));
    try$(_expectSerialized(_driver, "color", "hsl(120 none 50 / 0.5)", "hsl(120 none 50 / 0.5)"));
    try$(_expectSerialized(_driver, "color", "hsl(120 30 none)", "hsl(120 30 none)"));
    try$(_expectSerialized(_driver, "color", "hsl(120 30 none / 0.5)", "hsl(120 30 none / 0.5)"));
    try$(_expectSerialized(_driver, "color", "hsl(120 30 50 / none)", "hsl(120 30 50 / none)"));
    try$(_expectSerialized(_driver, "color", "hsl(30 -50% 60)", "rgb(153, 153, 153)"));
    try$(_expectSerialized(_driver, "color", "hsl(30 -50 60)", "rgb(153, 153, 153)"));
    try$(_expectSerialized(_driver, "color", "hsl(calc(infinity) 100% 50%)", "rgb(255, 0, 0)"));
    try$(_expectSerialized(_driver, "color", "hsl(calc(-infinity) 100% 50%)", "rgb(255, 0, 0)"));
    try$(_expectSerialized(_driver, "color", "hsl(calc(0 / 0) 100% 50%)", "rgb(255, 0, 0)"));
    try$(_expectSerialized(_driver, "color", "hsl(90 50% 50% / calc(infinity))", "rgb(128, 191, 64)"));
    try$(_expectSerialized(_driver, "color", "hsl(90 50% 50% / calc(-infinity))", "rgba(128, 191, 64, 0)"));
    try$(_expectSerialized(_driver, "color", "hsl(90 50% 50% / calc(0 / 0))", "rgba(128, 191, 64, 0)"));
    try$(_expectSerialized(_driver, "color", "hsl(90 50% 50% / calc(NaN))", "rgba(128, 191, 64, 0)"));
    try$(_expectSerialized(_driver, "color", "hsl(calc(50deg + (sign(1em - 10px) * 10deg)), 0%, 0%, 50%)", "hsl(calc(50deg + (10deg * sign(1em - 10px))) 0 0 / 0.5)"));
    try$(_expectSerialized(_driver, "color", "hsla(calc(50deg + (sign(1em - 10px) * 10deg)), 0%, 0%, 50%)", "hsl(calc(50deg + (10deg * sign(1em - 10px))) 0 0 / 0.5)"));
    try$(_expectSerialized(_driver, "color", "hsl(calc(50 + (sign(1em - 10px) * 10)), 0%, 0%, 50%)", "hsl(calc(50 + (10 * sign(1em - 10px))) 0 0 / 0.5)"));
    try$(_expectSerialized(_driver, "color", "hsla(calc(50 + (sign(1em - 10px) * 10)), 0%, 0%, 50%)", "hsl(calc(50 + (10 * sign(1em - 10px))) 0 0 / 0.5)"));
    try$(_expectSerialized(_driver, "color", "hsl(0deg, 0%, 0%, calc(50% + (sign(1em - 10px) * 10%)))", "hsl(0 0 0 / calc(50% + (10% * sign(1em - 10px))))"));
    try$(_expectSerialized(_driver, "color", "hsla(0deg, 0%, 0%, calc(50% + (sign(1em - 10px) * 10%)))", "hsl(0 0 0 / calc(50% + (10% * sign(1em - 10px))))"));
    try$(_expectSerialized(_driver, "color", "hsl(0, 0%, 0%, calc(50% + (sign(1em - 10px) * 10%)))", "hsl(0 0 0 / calc(50% + (10% * sign(1em - 10px))))"));
    try$(_expectSerialized(_driver, "color", "hsla(0, 0%, 0%, calc(50% + (sign(1em - 10px) * 10%)))", "hsl(0 0 0 / calc(50% + (10% * sign(1em - 10px))))"));
    try$(_expectSerialized(_driver, "color", "hsl(calc(50deg + (sign(1em - 10px) * 10deg)) 0% 0% / 50%)", "hsl(calc(50deg + (10deg * sign(1em - 10px))) 0 0 / 0.5)"));
    try$(_expectSerialized(_driver, "color", "hsla(calc(50deg + (sign(1em - 10px) * 10deg)) 0% 0% / 50%)", "hsl(calc(50deg + (10deg * sign(1em - 10px))) 0 0 / 0.5)"));
    try$(_expectSerialized(_driver, "color", "hsl(calc(50 + (sign(1em - 10px) * 10)) 0 0 / 0.5)", "hsl(calc(50 + (10 * sign(1em - 10px))) 0 0 / 0.5)"));
    try$(_expectSerialized(_driver, "color", "hsla(calc(50 + (sign(1em - 10px) * 10)) 0 0 / 0.5)", "hsl(calc(50 + (10 * sign(1em - 10px))) 0 0 / 0.5)"));
    try$(_expectSerialized(_driver, "color", "hsl(0deg 0% 0% / calc(50% + (sign(1em - 10px) * 10%)))", "hsl(0 0 0 / calc(50% + (10% * sign(1em - 10px))))"));
    try$(_expectSerialized(_driver, "color", "hsla(0deg 0% 0% / calc(50% + (sign(1em - 10px) * 10%)))", "hsl(0 0 0 / calc(50% + (10% * sign(1em - 10px))))"));
    try$(_expectSerialized(_driver, "color", "hsl(0 0 0 / calc(0.75 + (sign(1em - 10px) * 0.1)))", "hsl(0 0 0 / calc(0.75 + (0.1 * sign(1em - 10px))))"));
    try$(_expectSerialized(_driver, "color", "hsla(0 0 0 / calc(0.75 + (sign(1em - 10px) * 0.1)))", "hsl(0 0 0 / calc(0.75 + (0.1 * sign(1em - 10px))))"));
    try$(_expectSerialized(_driver, "color", "hsla(calc(50deg + (sign(1em - 10px) * 10deg)) -100 300 / 0.5)", "hsl(calc(50deg + (10deg * sign(1em - 10px))) 0 300 / 0.5)"));
    try$(_expectSerialized(_driver, "color", "hsla(calc(50deg + (sign(1em - 10px) * 10deg)) -100% 300% / 0.5)", "hsl(calc(50deg + (10deg * sign(1em - 10px))) 0 300 / 0.5)"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-color/parsing/color-valid-rgb.html
test$("vaev-style-wpt-css-css-color-parsing-color-valid-rgb") {
    try$(_expectSerialized(_driver, "color", "rgb(none none none)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "color", "rgb(none none none / none)", "rgba(0, 0, 0, 0)"));
    try$(_expectSerialized(_driver, "color", "rgb(128 none none)", "rgb(128, 0, 0)"));
    try$(_expectSerialized(_driver, "color", "rgb(128 none none / none)", "rgba(128, 0, 0, 0)"));
    try$(_expectSerialized(_driver, "color", "rgb(20% none none)", "rgb(51, 0, 0)"));
    try$(_expectSerialized(_driver, "color", "rgb(20% none none / none)", "rgba(51, 0, 0, 0)"));
    try$(_expectSerialized(_driver, "color", "rgba(none none none)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "color", "rgba(none none none / none)", "rgba(0, 0, 0, 0)"));
    try$(_expectSerialized(_driver, "color", "rgba(128 none none)", "rgb(128, 0, 0)"));
    try$(_expectSerialized(_driver, "color", "rgba(128 none none / none)", "rgba(128, 0, 0, 0)"));
    try$(_expectSerialized(_driver, "color", "rgba(20% none none)", "rgb(51, 0, 0)"));
    try$(_expectSerialized(_driver, "color", "rgba(20% none none / none)", "rgba(51, 0, 0, 0)"));
    try$(_expectSerialized(_driver, "color", "rgb(255 20% 102)", "rgb(255, 51, 102)"));
    return Ok();
}

test$("vaev-style-wpt-css-css-color-parsing-color-valid-rgb-xfail", XFAIL) {
    try$(_expectSerialized(_driver, "color", "rgb(none none none / .5)", "rgba(0, 0, 0, 0.5)"));
    try$(_expectSerialized(_driver, "color", "rgb(none none none / 50%)", "rgba(0, 0, 0, 0.5)"));
    try$(_expectSerialized(_driver, "color", "rgba(none none none / .5)", "rgba(0, 0, 0, 0.5)"));
    try$(_expectSerialized(_driver, "color", "rgba(none none none / 50%)", "rgba(0, 0, 0, 0.5)"));
    try$(_expectSerialized(_driver, "color", "rgb(-2 3 4)", "rgb(0, 3, 4)"));
    try$(_expectSerialized(_driver, "color", "rgb(-20% 20% 40%)", "rgb(0, 51, 102)"));
    try$(_expectSerialized(_driver, "color", "rgb(257 30 40)", "rgb(255, 30, 40)"));
    try$(_expectSerialized(_driver, "color", "rgb(250% 20% 40%)", "rgb(255, 51, 102)"));
    try$(_expectSerialized(_driver, "color", "rgba(-2 3 4)", "rgb(0, 3, 4)"));
    try$(_expectSerialized(_driver, "color", "rgba(-20% 20% 40%)", "rgb(0, 51, 102)"));
    try$(_expectSerialized(_driver, "color", "rgba(257 30 40)", "rgb(255, 30, 40)"));
    try$(_expectSerialized(_driver, "color", "rgba(250% 20% 40%)", "rgb(255, 51, 102)"));
    try$(_expectSerialized(_driver, "color", "rgba(-2 3 4 / .5)", "rgba(0, 3, 4, 0.5)"));
    try$(_expectSerialized(_driver, "color", "rgba(-20% 20% 40% / 50%)", "rgba(0, 51, 102, 0.5)"));
    try$(_expectSerialized(_driver, "color", "rgba(257 30 40 / 50%)", "rgba(255, 30, 40, 0.5)"));
    try$(_expectSerialized(_driver, "color", "rgba(250% 20% 40% / .5)", "rgba(255, 51, 102, 0.5)"));
    try$(_expectSerialized(_driver, "color", "rgb(250% 51 40%)", "rgb(255, 51, 102)"));
    try$(_expectSerialized(_driver, "color", "rgb(500, 0, 0)", "rgb(255, 0, 0)"));
    try$(_expectSerialized(_driver, "color", "rgb(-500, 64, 128)", "rgb(0, 64, 128)"));
    try$(_expectSerialized(_driver, "color", "rgb(calc(infinity), 0, 0)", "rgb(255, 0, 0)"));
    try$(_expectSerialized(_driver, "color", "rgb(0, calc(infinity), 0)", "rgb(0, 255, 0)"));
    try$(_expectSerialized(_driver, "color", "rgb(0, 0, calc(infinity))", "rgb(0, 0, 255)"));
    try$(_expectSerialized(_driver, "color", "rgba(0, 0, 0, calc(infinity))", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "color", "rgb(calc(-infinity), 0, 0)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "color", "rgb(0, calc(-infinity), 0)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "color", "rgb(0, 0, calc(-infinity))", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "color", "rgba(0, 0, 0, calc(-infinity))", "rgba(0, 0, 0, 0)"));
    try$(_expectSerialized(_driver, "color", "rgb(calc(NaN), 0, 0)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "color", "rgb(0, calc(NaN), 0)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "color", "rgb(0, 0, calc(NaN))", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "color", "rgba(0, 0, 0, calc(NaN))", "rgba(0, 0, 0, 0)"));
    try$(_expectSerialized(_driver, "color", "rgb(calc(0 / 0), 0, 0)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "color", "rgb(0, calc(0 / 0), 0)", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "color", "rgb(0, 0, calc(0 / 0))", "rgb(0, 0, 0)"));
    try$(_expectSerialized(_driver, "color", "rgba(0, 0, 0, calc(0 / 0))", "rgba(0, 0, 0, 0)"));
    try$(_expectSerialized(_driver, "color", "rgb(calc(50% + (sign(1em - 10px) * 10%)), 0%, 0%, 50%)", "rgb(calc(50% + (10% * sign(1em - 10px))) 0 0 / 0.5)"));
    try$(_expectSerialized(_driver, "color", "rgba(calc(50% + (sign(1em - 10px) * 10%)), 0%, 0%, 50%)", "rgb(calc(50% + (10% * sign(1em - 10px))) 0 0 / 0.5)"));
    try$(_expectSerialized(_driver, "color", "rgb(calc(50 + (sign(1em - 10px) * 10)), 0, 0, 0.5)", "rgb(calc(50 + (10 * sign(1em - 10px))) 0 0 / 0.5)"));
    try$(_expectSerialized(_driver, "color", "rgba(calc(50 + (sign(1em - 10px) * 10)), 0, 0, 0.5)", "rgb(calc(50 + (10 * sign(1em - 10px))) 0 0 / 0.5)"));
    try$(_expectSerialized(_driver, "color", "rgb(0%, 0%, 0%, calc(50% + (sign(1em - 10px) * 10%)))", "rgb(0 0 0 / calc(50% + (10% * sign(1em - 10px))))"));
    try$(_expectSerialized(_driver, "color", "rgba(0%, 0%, 0%, calc(50% + (sign(1em - 10px) * 10%)))", "rgb(0 0 0 / calc(50% + (10% * sign(1em - 10px))))"));
    try$(_expectSerialized(_driver, "color", "rgb(0, 0, 0, calc(0.75 + (sign(1em - 10px) * 0.1)))", "rgb(0 0 0 / calc(0.75 + (0.1 * sign(1em - 10px))))"));
    try$(_expectSerialized(_driver, "color", "rgba(0, 0, 0, calc(0.75 + (sign(1em - 10px) * 0.1)))", "rgb(0 0 0 / calc(0.75 + (0.1 * sign(1em - 10px))))"));
    try$(_expectSerialized(_driver, "color", "rgb(calc(50% + (sign(1em - 10px) * 10%)) 0% 0% / 50%)", "rgb(calc(50% + (10% * sign(1em - 10px))) 0 0 / 0.5)"));
    try$(_expectSerialized(_driver, "color", "rgba(calc(50% + (sign(1em - 10px) * 10%)) 0% 0% / 50%)", "rgb(calc(50% + (10% * sign(1em - 10px))) 0 0 / 0.5)"));
    try$(_expectSerialized(_driver, "color", "rgb(calc(50 + (sign(1em - 10px) * 10)) 0 0 / 0.5)", "rgb(calc(50 + (10 * sign(1em - 10px))) 0 0 / 0.5)"));
    try$(_expectSerialized(_driver, "color", "rgba(calc(50 + (sign(1em - 10px) * 10)) 0 0 / 0.5)", "rgb(calc(50 + (10 * sign(1em - 10px))) 0 0 / 0.5)"));
    try$(_expectSerialized(_driver, "color", "rgb(0% 0% 0% / calc(50% + (sign(1em - 10px) * 10%)))", "rgb(0 0 0 / calc(50% + (10% * sign(1em - 10px))))"));
    try$(_expectSerialized(_driver, "color", "rgba(0% 0% 0% / calc(50% + (sign(1em - 10px) * 10%)))", "rgb(0 0 0 / calc(50% + (10% * sign(1em - 10px))))"));
    try$(_expectSerialized(_driver, "color", "rgb(0 0 0 / calc(0.75 + (sign(1em - 10px) * 0.1)))", "rgb(0 0 0 / calc(0.75 + (0.1 * sign(1em - 10px))))"));
    try$(_expectSerialized(_driver, "color", "rgba(0 0 0 / calc(0.75 + (sign(1em - 10px) * 0.1)))", "rgb(0 0 0 / calc(0.75 + (0.1 * sign(1em - 10px))))"));
    try$(_expectSerialized(_driver, "color", "rgba(calc(50% + (sign(1em - 10px) * 10%)) 0 0% / 0.5)", "rgb(calc(50% + (10% * sign(1em - 10px))) 0 0 / 0.5)"));
    try$(_expectSerialized(_driver, "color", "rgba(0% 0 0% / calc(0.75 + (sign(1em - 10px) * 0.1)))", "rgb(0 0 0 / calc(0.75 + (0.1 * sign(1em - 10px))))"));
    try$(_expectSerialized(_driver, "color", "rgba(calc(50 + (sign(1em - 10px) * 10)) 400 -400 / 0.5)", "rgb(calc(50 + (10 * sign(1em - 10px))) 255 0 / 0.5)"));
    try$(_expectSerialized(_driver, "color", "rgba(calc(50% + (sign(1em - 10px) * 10%)) 400% -400% / 0.5)", "rgb(calc(50% + (10% * sign(1em - 10px))) 255 0 / 0.5)"));
    try$(_expectSerialized(_driver, "color", "rgba(calc(50 + (sign(1em - 10px) * 10)), 400, -400, 0.5)", "rgb(calc(50 + (10 * sign(1em - 10px))) 255 0 / 0.5)"));
    try$(_expectSerialized(_driver, "color", "rgba(calc(50% + (sign(1em - 10px) * 10%)), 400%, -400%, 0.5)", "rgb(calc(50% + (10% * sign(1em - 10px))) 255 0 / 0.5)"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-color/parsing/color-valid.html
test$("vaev-style-wpt-css-css-color-parsing-color-valid") {
    try$(_expectSerialized(_driver, "color", "#234", "rgb(34, 51, 68)"));
    try$(_expectSerialized(_driver, "color", "#FEDCBA", "rgb(254, 220, 186)"));
    try$(_expectSerialized(_driver, "color", "rgb(2, 3, 4)", "rgb(2, 3, 4)"));
    try$(_expectSerialized(_driver, "color", "rgb(100%, 0%, 0%)", "rgb(255, 0, 0)"));
    try$(_expectSerialized(_driver, "color", "hsl(120, 100%, 50%)", "rgb(0, 255, 0)"));
    return Ok();
}

test$("vaev-style-wpt-css-css-color-parsing-color-valid-xfail", XFAIL) {
    try$(_expectSerialized(_driver, "color", "currentcolor", "currentcolor"));
    try$(_expectSerialized(_driver, "color", "transparent", "transparent"));
    try$(_expectSerialized(_driver, "color", "red", "red"));
    try$(_expectSerialized(_driver, "color", "magenta", "magenta"));
    try$(_expectSerialized(_driver, "color", "rgba(2, 3, 4, 0.5)", "rgba(2, 3, 4, 0.5)"));
    try$(_expectSerialized(_driver, "color", "rgba(2, 3, 4, 50%)", "rgba(2, 3, 4, 0.5)"));
    try$(_expectSerialized(_driver, "color", "hsla(120, 100%, 50%, 0.25)", "rgba(0, 255, 0, 0.25)"));
    try$(_expectSerialized(_driver, "color", "rgb(-2, 3, 4)", "rgb(0, 3, 4)"));
    try$(_expectSerialized(_driver, "color", "rgb(100, 200, 300)", "rgb(100, 200, 255)"));
    try$(_expectSerialized(_driver, "color", "rgb(20, 10, 0, -10)", "rgba(20, 10, 0, 0)"));
    try$(_expectSerialized(_driver, "color", "rgb(100%, 200%, 300%)", "rgb(255, 255, 255)"));
    try$(_expectSerialized(_driver, "color", "light-dark(black, white)", "light-dark(black, white)"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-color/parsing/opacity-invalid.html
test$("vaev-style-wpt-css-css-color-parsing-opacity-invalid") {
    try$(_expectInvalid(_driver, "opacity", "auto"));
    try$(_expectInvalid(_driver, "opacity", "10px"));
    try$(_expectInvalid(_driver, "opacity", "0 1"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-color/parsing/opacity-valid.html
test$("vaev-style-wpt-css-css-color-parsing-opacity-valid") {
    try$(_expectSerialized(_driver, "opacity", "1", "1"));
    try$(_expectSerialized(_driver, "opacity", "0.5", "0.5"));
    try$(_expectSerialized(_driver, "opacity", "0", "0"));
    try$(_expectSerialized(_driver, "opacity", "-2", "-2"));
    try$(_expectSerialized(_driver, "opacity", "3", "3"));
    try$(_expectSerialized(_driver, "opacity", "-100%", "-1"));
    try$(_expectSerialized(_driver, "opacity", "50%", "0.5"));
    try$(_expectSerialized(_driver, "opacity", "300%", "3"));
    return Ok();
}

test$("vaev-style-wpt-css-css-color-parsing-opacity-valid-xfail", XFAIL) {
    try$(_expectSerialized(_driver, "opacity", "calc(-50% - 50%)", "calc(-100%)"));
    try$(_expectSerialized(_driver, "opacity", "calc(25% * 2)", "calc(50%)"));
    try$(_expectSerialized(_driver, "opacity", "calc(100% + 100%)", "calc(200%)"));
    try$(_expectSerialized(_driver, "opacity", "calc(-0.5 - 0.5)", "calc(-1)"));
    try$(_expectSerialized(_driver, "opacity", "calc(0.25 * 2)", "calc(0.5)"));
    try$(_expectSerialized(_driver, "opacity", "calc(1 + 1)", "calc(2)"));
    try$(_expectSerialized(_driver, "opacity", "clamp(50%, 0%, 70%)", "calc(50%)"));
    try$(_expectSerialized(_driver, "opacity", "clamp(50%, 80%, 70%)", "calc(70%)"));
    try$(_expectSerialized(_driver, "opacity", "clamp(50%, 60%, 70%)", "calc(60%)"));
    try$(_expectSerialized(_driver, "opacity", "min(50%, 0%)", "calc(0%)"));
    try$(_expectSerialized(_driver, "opacity", "min(0%, 50%)", "calc(0%)"));
    try$(_expectSerialized(_driver, "opacity", "max(50%, 0%)", "calc(50%)"));
    try$(_expectSerialized(_driver, "opacity", "max(0%, 50%)", "calc(50%)"));
    try$(_expectSerialized(_driver, "opacity", "min(-40%, 50%)", "calc(-40%)"));
    try$(_expectSerialized(_driver, "opacity", "clamp(0.5, 0, 0.7)", "calc(0.5)"));
    try$(_expectSerialized(_driver, "opacity", "clamp(0.5, 0.8, 0.7)", "calc(0.7)"));
    try$(_expectSerialized(_driver, "opacity", "clamp(0.5, 0.6, 0.7)", "calc(0.6)"));
    try$(_expectSerialized(_driver, "opacity", "min(0.5, 0)", "calc(0)"));
    try$(_expectSerialized(_driver, "opacity", "min(0, 0.5)", "calc(0)"));
    try$(_expectSerialized(_driver, "opacity", "max(0.5, 0)", "calc(0.5)"));
    try$(_expectSerialized(_driver, "opacity", "max(0, 0.5)", "calc(0.5)"));
    try$(_expectSerialized(_driver, "opacity", "min(-0.4, 0.5)", "calc(-0.4)"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-content/parsing/content-invalid.html
test$("vaev-style-wpt-css-css-content-parsing-content-invalid") {
    try$(_expectInvalid(_driver, "content", "attr()"));
    try$(_expectInvalid(_driver, "content", "attr() / \"alt text\""));
    try$(_expectInvalid(_driver, "content", "counters(counter-name)"));
    try$(_expectInvalid(_driver, "content", "counters(counter-name) / \"alt text\""));
    try$(_expectInvalid(_driver, "content", "counter()"));
    try$(_expectInvalid(_driver, "content", "counter() / \"alt text\""));
    try$(_expectInvalid(_driver, "content", "open-quote / url(\"https://www.example.com/picture.svg\")"));
    try$(_expectInvalid(_driver, "content", "open-quote / no-open-quote"));
    try$(_expectInvalid(_driver, "content", "open-quote / no-close-quote"));
    try$(_expectInvalid(_driver, "content", "open-quote / \"hi\" no-close-quote"));
    try$(_expectInvalid(_driver, "content", "close-quote / url(\"https://www.example.com/picture.svg\")"));
    try$(_expectInvalid(_driver, "content", "close-quote / no-open-quote"));
    try$(_expectInvalid(_driver, "content", "close-quote / no-close-quote"));
    try$(_expectInvalid(_driver, "content", "close-quote / \"hi\" no-close-quote"));
    try$(_expectInvalid(_driver, "content", "no-open-quote / url(\"https://www.example.com/picture.svg\")"));
    try$(_expectInvalid(_driver, "content", "no-open-quote / no-open-quote"));
    try$(_expectInvalid(_driver, "content", "no-open-quote / no-close-quote"));
    try$(_expectInvalid(_driver, "content", "no-open-quote / \"hi\" no-close-quote"));
    try$(_expectInvalid(_driver, "content", "no-close-quote / url(\"https://www.example.com/picture.svg\")"));
    try$(_expectInvalid(_driver, "content", "no-close-quote / no-open-quote"));
    try$(_expectInvalid(_driver, "content", "no-close-quote / no-close-quote"));
    try$(_expectInvalid(_driver, "content", "no-close-quote / \"hi\" no-close-quote"));
    try$(_expectInvalid(_driver, "content", "counter(counter-name) / url(\"https://www.example.com/picture.svg\")"));
    try$(_expectInvalid(_driver, "content", "counter(counter-name) / no-open-quote"));
    try$(_expectInvalid(_driver, "content", "counter(counter-name) / no-close-quote"));
    try$(_expectInvalid(_driver, "content", "counter(counter-name) / \"hi\" no-close-quote"));
    try$(_expectInvalid(_driver, "content", "counter(counter-name, counter-style) / url(\"https://www.example.com/picture.svg\")"));
    try$(_expectInvalid(_driver, "content", "counter(counter-name, counter-style) / no-open-quote"));
    try$(_expectInvalid(_driver, "content", "counter(counter-name, counter-style) / no-close-quote"));
    try$(_expectInvalid(_driver, "content", "counter(counter-name, counter-style) / \"hi\" no-close-quote"));
    try$(_expectInvalid(_driver, "content", "counters(counter-name, \".\") / url(\"https://www.example.com/picture.svg\")"));
    try$(_expectInvalid(_driver, "content", "counters(counter-name, \".\") / no-open-quote"));
    try$(_expectInvalid(_driver, "content", "counters(counter-name, \".\") / no-close-quote"));
    try$(_expectInvalid(_driver, "content", "counters(counter-name, \".\") / \"hi\" no-close-quote"));
    try$(_expectInvalid(_driver, "content", "counters(counter-name, \".\", counter-style) / url(\"https://www.example.com/picture.svg\")"));
    try$(_expectInvalid(_driver, "content", "counters(counter-name, \".\", counter-style) / no-open-quote"));
    try$(_expectInvalid(_driver, "content", "counters(counter-name, \".\", counter-style) / no-close-quote"));
    try$(_expectInvalid(_driver, "content", "counters(counter-name, \".\", counter-style) / \"hi\" no-close-quote"));
    try$(_expectInvalid(_driver, "content", "counter(counter-name, symbols()) / \"alt text\""));
    try$(_expectInvalid(_driver, "content", "counter(counter-name, symbols(fixed)) / \"alt text\""));
    try$(_expectInvalid(_driver, "content", "counter(counter-name, symbols(a b)) / \"alt text\""));
    try$(_expectInvalid(_driver, "content", "counter(counter-name, symbols(additive \"a\" \"b\")) / \"alt text\""));
    try$(_expectInvalid(_driver, "content", "counter(counter-name, symbols(numeric \"a\")) / \"alt text\""));
    try$(_expectInvalid(_driver, "content", "counter(counter-name, symbols(alphabetic \"a\")) / \"alt text\""));
    try$(_expectInvalid(_driver, "content", "counters(counter-name, \".\", symbols(fixed))"));
    try$(_expectInvalid(_driver, "content", "counters(counter-name, \".\", symbols(fixed)) / \"alt text\""));
    try$(_expectInvalid(_driver, "content", "url(\"https://www.example.com/picture.svg\") / url(\"https://www.example.com/picture.svg\")"));
    try$(_expectInvalid(_driver, "content", "url(\"https://www.example.com/picture.svg\") / no-open-quote"));
    try$(_expectInvalid(_driver, "content", "url(\"https://www.example.com/picture.svg\") / no-close-quote"));
    try$(_expectInvalid(_driver, "content", "url(\"https://www.example.com/picture.svg\") / \"hi\" no-close-quote"));
    try$(_expectInvalid(_driver, "content", "\"hello\" / url(\"https://www.example.com/picture.svg\")"));
    try$(_expectInvalid(_driver, "content", "\"hello\" / no-open-quote"));
    try$(_expectInvalid(_driver, "content", "\"hello\" / no-close-quote"));
    try$(_expectInvalid(_driver, "content", "\"hello\" / \"hi\" no-close-quote"));
    try$(_expectInvalid(_driver, "content", "\"hello\" \"world\" / url(\"https://www.example.com/picture.svg\")"));
    try$(_expectInvalid(_driver, "content", "\"hello\" \"world\" / no-open-quote"));
    try$(_expectInvalid(_driver, "content", "\"hello\" \"world\" / no-close-quote"));
    try$(_expectInvalid(_driver, "content", "\"hello\" \"world\" / \"hi\" no-close-quote"));
    try$(_expectInvalid(_driver, "content", "counter(counter-name) \"potato\" / url(\"https://www.example.com/picture.svg\")"));
    try$(_expectInvalid(_driver, "content", "counter(counter-name) \"potato\" / no-open-quote"));
    try$(_expectInvalid(_driver, "content", "counter(counter-name) \"potato\" / no-close-quote"));
    try$(_expectInvalid(_driver, "content", "counter(counter-name) \"potato\" / \"hi\" no-close-quote"));
    try$(_expectInvalid(_driver, "content", "counters(counter-name, \".\") \"potato\" / url(\"https://www.example.com/picture.svg\")"));
    try$(_expectInvalid(_driver, "content", "counters(counter-name, \".\") \"potato\" / no-open-quote"));
    try$(_expectInvalid(_driver, "content", "counters(counter-name, \".\") \"potato\" / no-close-quote"));
    try$(_expectInvalid(_driver, "content", "counters(counter-name, \".\") \"potato\" / \"hi\" no-close-quote"));
    try$(_expectInvalid(_driver, "content", "\"(\" counters(counter-name, \".\", counter-style) \")\" / url(\"https://www.example.com/picture.svg\")"));
    try$(_expectInvalid(_driver, "content", "\"(\" counters(counter-name, \".\", counter-style) \")\" / no-open-quote"));
    try$(_expectInvalid(_driver, "content", "\"(\" counters(counter-name, \".\", counter-style) \")\" / no-close-quote"));
    try$(_expectInvalid(_driver, "content", "\"(\" counters(counter-name, \".\", counter-style) \")\" / \"hi\" no-close-quote"));
    try$(_expectInvalid(_driver, "content", "open-quote \"hello\" \"world\" close-quote / url(\"https://www.example.com/picture.svg\")"));
    try$(_expectInvalid(_driver, "content", "open-quote \"hello\" \"world\" close-quote / no-open-quote"));
    try$(_expectInvalid(_driver, "content", "open-quote \"hello\" \"world\" close-quote / no-close-quote"));
    try$(_expectInvalid(_driver, "content", "open-quote \"hello\" \"world\" close-quote / \"hi\" no-close-quote"));
    try$(_expectInvalid(_driver, "content", "url(\"https://www.example.com/picture.svg\") \"hello\" / url(\"https://www.example.com/picture.svg\")"));
    try$(_expectInvalid(_driver, "content", "url(\"https://www.example.com/picture.svg\") \"hello\" / no-open-quote"));
    try$(_expectInvalid(_driver, "content", "url(\"https://www.example.com/picture.svg\") \"hello\" / no-close-quote"));
    try$(_expectInvalid(_driver, "content", "url(\"https://www.example.com/picture.svg\") \"hello\" / \"hi\" no-close-quote"));
    return Ok();
}

test$("vaev-style-wpt-css-css-content-parsing-content-invalid-xfail", XFAIL) {
    try$(_expectInvalid(_driver, "content", "counter(counter-name, symbols())"));
    try$(_expectInvalid(_driver, "content", "counter(counter-name, symbols(fixed))"));
    try$(_expectInvalid(_driver, "content", "counter(counter-name, symbols(a b))"));
    try$(_expectInvalid(_driver, "content", "counter(counter-name, symbols(additive \"a\" \"b\"))"));
    try$(_expectInvalid(_driver, "content", "counter(counter-name, symbols(numeric \"a\"))"));
    try$(_expectInvalid(_driver, "content", "counter(counter-name, symbols(alphabetic \"a\"))"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-display/parsing/display-invalid.html
test$("vaev-style-wpt-css-css-display-parsing-display-invalid") {
    try$(_expectInvalid(_driver, "display", "grid inline-grid"));
    try$(_expectInvalid(_driver, "display", "none grid"));
    try$(_expectInvalid(_driver, "display", "none flow"));
    try$(_expectInvalid(_driver, "display", "none ruby"));
    try$(_expectInvalid(_driver, "display", "none flow-root"));
    try$(_expectInvalid(_driver, "display", "none list-item"));
    try$(_expectInvalid(_driver, "display", "contents list-item"));
    try$(_expectInvalid(_driver, "display", "list-item contents"));
    try$(_expectInvalid(_driver, "display", "flow flow"));
    try$(_expectInvalid(_driver, "display", "block block"));
    try$(_expectInvalid(_driver, "display", "flow flow-root"));
    try$(_expectInvalid(_driver, "display", "flow-root flow-root"));
    try$(_expectInvalid(_driver, "display", "block inline"));
    try$(_expectInvalid(_driver, "display", "flex ruby"));
    try$(_expectInvalid(_driver, "display", "ruby flex"));
    try$(_expectInvalid(_driver, "display", "inline inline"));
    try$(_expectInvalid(_driver, "display", "flex flex"));
    try$(_expectInvalid(_driver, "display", "grid flex"));
    try$(_expectInvalid(_driver, "display", "ruby grid"));
    try$(_expectInvalid(_driver, "display", "flex grid"));
    try$(_expectInvalid(_driver, "display", "grid table"));
    try$(_expectInvalid(_driver, "display", "table flex"));
    try$(_expectInvalid(_driver, "display", "flex table"));
    try$(_expectInvalid(_driver, "display", "ruby table"));
    try$(_expectInvalid(_driver, "display", "table table"));
    try$(_expectInvalid(_driver, "display", "table flow-root"));
    try$(_expectInvalid(_driver, "display", "flow-root ruby"));
    try$(_expectInvalid(_driver, "display", "list-item list-item"));
    try$(_expectInvalid(_driver, "display", "list-item table"));
    try$(_expectInvalid(_driver, "display", "list-item grid"));
    try$(_expectInvalid(_driver, "display", "flow flow list-item"));
    try$(_expectInvalid(_driver, "display", "grid flow list-item"));
    try$(_expectInvalid(_driver, "display", "flow list-item flow"));
    try$(_expectInvalid(_driver, "display", "block list-item block"));
    try$(_expectInvalid(_driver, "display", "list-item flow-root flow"));
    try$(_expectInvalid(_driver, "display", "list-item block block"));
    try$(_expectInvalid(_driver, "display", "list-item block list-item"));
    try$(_expectInvalid(_driver, "display", "list-item flow-root list-item"));
    try$(_expectInvalid(_driver, "display", "list-item flow-root block list-item"));
    try$(_expectInvalid(_driver, "display", "list-item block flow-root list-item"));
    try$(_expectInvalid(_driver, "display", "block list-item flow-root list-item"));
    try$(_expectInvalid(_driver, "display", "flow-root list-item block list-item"));
    try$(_expectInvalid(_driver, "display", "list-item ruby"));
    try$(_expectInvalid(_driver, "display", "ruby ruby"));
    try$(_expectInvalid(_driver, "display", "ruby flow"));
    try$(_expectInvalid(_driver, "display", "flow ruby"));
    try$(_expectInvalid(_driver, "display", "ruby flow-root"));
    try$(_expectInvalid(_driver, "display", "grid ruby"));
    try$(_expectInvalid(_driver, "display", "table ruby"));
    try$(_expectInvalid(_driver, "display", "table-row flow"));
    try$(_expectInvalid(_driver, "display", "flow table-row"));
    try$(_expectInvalid(_driver, "display", "table-row-group flow-root"));
    try$(_expectInvalid(_driver, "display", "flex table-row-group"));
    return Ok();
}

test$("vaev-style-wpt-css-css-display-parsing-display-invalid-xfail", XFAIL) {
    try$(_expectInvalid(_driver, "display", "flex list-item"));
    try$(_expectInvalid(_driver, "display", "ruby list-item"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-display/parsing/display-valid.html
test$("vaev-style-wpt-css-css-display-parsing-display-valid") {
    try$(_expectSerialized(_driver, "display", "grid", "grid"));
    try$(_expectSerialized(_driver, "display", "inline-grid", "inline-grid"));
    try$(_expectSerialized(_driver, "display", "inline", "inline"));
    try$(_expectSerialized(_driver, "display", "block", "block"));
    try$(_expectSerialized(_driver, "display", "list-item", "list-item"));
    try$(_expectSerialized(_driver, "display", "inline-block", "inline-block"));
    try$(_expectSerialized(_driver, "display", "table", "table"));
    try$(_expectSerialized(_driver, "display", "inline-table", "inline-table"));
    try$(_expectSerialized(_driver, "display", "table-row-group", "table-row-group"));
    try$(_expectSerialized(_driver, "display", "table-header-group", "table-header-group"));
    try$(_expectSerialized(_driver, "display", "table-footer-group", "table-footer-group"));
    try$(_expectSerialized(_driver, "display", "table-row", "table-row"));
    try$(_expectSerialized(_driver, "display", "table-column-group", "table-column-group"));
    try$(_expectSerialized(_driver, "display", "table-column", "table-column"));
    try$(_expectSerialized(_driver, "display", "table-cell", "table-cell"));
    try$(_expectSerialized(_driver, "display", "table-caption", "table-caption"));
    try$(_expectSerialized(_driver, "display", "none", "none"));
    try$(_expectSerialized(_driver, "display", "flex", "flex"));
    try$(_expectSerialized(_driver, "display", "inline-flex", "inline-flex"));
    try$(_expectSerialized(_driver, "display", "contents", "contents"));
    try$(_expectSerialized(_driver, "display", "run-in", "run-in"));
    try$(_expectSerialized(_driver, "display", "flow", "block"));
    try$(_expectSerialized(_driver, "display", "flow-root", "flow-root"));
    try$(_expectSerialized(_driver, "display", "ruby-base", "ruby-base"));
    try$(_expectSerialized(_driver, "display", "ruby-text", "ruby-text"));
    try$(_expectSerialized(_driver, "display", "flow list-item", "list-item"));
    try$(_expectSerialized(_driver, "display", "flow block", "block"));
    try$(_expectSerialized(_driver, "display", "flow-root block", "flow-root"));
    try$(_expectSerialized(_driver, "display", "flex block", "flex"));
    try$(_expectSerialized(_driver, "display", "grid block", "grid"));
    try$(_expectSerialized(_driver, "display", "table block", "table"));
    try$(_expectSerialized(_driver, "display", "ruby block", "block ruby"));
    try$(_expectSerialized(_driver, "display", "block list-item", "list-item"));
    try$(_expectSerialized(_driver, "display", "flow block list-item", "list-item"));
    try$(_expectSerialized(_driver, "display", "flow inline", "inline"));
    try$(_expectSerialized(_driver, "display", "flow-root inline", "inline-block"));
    try$(_expectSerialized(_driver, "display", "flex inline", "inline-flex"));
    try$(_expectSerialized(_driver, "display", "grid inline", "inline-grid"));
    try$(_expectSerialized(_driver, "display", "table inline", "inline-table"));
    try$(_expectSerialized(_driver, "display", "ruby inline", "ruby"));
    try$(_expectSerialized(_driver, "display", "inline list-item", "inline list-item"));
    try$(_expectSerialized(_driver, "display", "flow inline list-item", "inline list-item"));
    try$(_expectSerialized(_driver, "display", "flow run-in", "run-in"));
    return Ok();
}

test$("vaev-style-wpt-css-css-display-parsing-display-valid-xfail", XFAIL) {
    try$(_expectSerialized(_driver, "display", "ruby", "ruby"));
    try$(_expectSerialized(_driver, "display", "list-item flow", "list-item"));
    try$(_expectSerialized(_driver, "display", "flow-root list-item", "flow-root list-item"));
    try$(_expectSerialized(_driver, "display", "list-item flow-root", "flow-root list-item"));
    try$(_expectSerialized(_driver, "display", "block flow", "block"));
    try$(_expectSerialized(_driver, "display", "block flow-root", "flow-root"));
    try$(_expectSerialized(_driver, "display", "block flex", "flex"));
    try$(_expectSerialized(_driver, "display", "block grid", "grid"));
    try$(_expectSerialized(_driver, "display", "block table", "table"));
    try$(_expectSerialized(_driver, "display", "block ruby", "block ruby"));
    try$(_expectSerialized(_driver, "display", "list-item block", "list-item"));
    try$(_expectSerialized(_driver, "display", "block flow list-item", "list-item"));
    try$(_expectSerialized(_driver, "display", "flow list-item block", "list-item"));
    try$(_expectSerialized(_driver, "display", "block list-item flow", "list-item"));
    try$(_expectSerialized(_driver, "display", "list-item block flow", "list-item"));
    try$(_expectSerialized(_driver, "display", "list-item flow block", "list-item"));
    try$(_expectSerialized(_driver, "display", "flow-root block list-item", "flow-root list-item"));
    try$(_expectSerialized(_driver, "display", "block flow-root list-item", "flow-root list-item"));
    try$(_expectSerialized(_driver, "display", "flow-root list-item block", "flow-root list-item"));
    try$(_expectSerialized(_driver, "display", "block list-item flow-root", "flow-root list-item"));
    try$(_expectSerialized(_driver, "display", "list-item block flow-root", "flow-root list-item"));
    try$(_expectSerialized(_driver, "display", "list-item flow-root block", "flow-root list-item"));
    try$(_expectSerialized(_driver, "display", "inline flow", "inline"));
    try$(_expectSerialized(_driver, "display", "inline flow-root", "inline-block"));
    try$(_expectSerialized(_driver, "display", "inline flex", "inline-flex"));
    try$(_expectSerialized(_driver, "display", "inline grid", "inline-grid"));
    try$(_expectSerialized(_driver, "display", "inline table", "inline-table"));
    try$(_expectSerialized(_driver, "display", "inline ruby", "ruby"));
    try$(_expectSerialized(_driver, "display", "list-item inline", "inline list-item"));
    try$(_expectSerialized(_driver, "display", "inline flow list-item", "inline list-item"));
    try$(_expectSerialized(_driver, "display", "flow list-item inline", "inline list-item"));
    try$(_expectSerialized(_driver, "display", "inline list-item flow", "inline list-item"));
    try$(_expectSerialized(_driver, "display", "list-item inline flow", "inline list-item"));
    try$(_expectSerialized(_driver, "display", "list-item flow inline", "inline list-item"));
    try$(_expectSerialized(_driver, "display", "flow-root inline list-item", "inline flow-root list-item"));
    try$(_expectSerialized(_driver, "display", "inline flow-root list-item", "inline flow-root list-item"));
    try$(_expectSerialized(_driver, "display", "flow-root list-item inline", "inline flow-root list-item"));
    try$(_expectSerialized(_driver, "display", "inline list-item flow-root", "inline flow-root list-item"));
    try$(_expectSerialized(_driver, "display", "list-item inline flow-root", "inline flow-root list-item"));
    try$(_expectSerialized(_driver, "display", "list-item flow-root inline", "inline flow-root list-item"));
    try$(_expectSerialized(_driver, "display", "run-in flow", "run-in"));
    try$(_expectSerialized(_driver, "display", "flow-root run-in", "run-in flow-root"));
    try$(_expectSerialized(_driver, "display", "run-in flow-root", "run-in flow-root"));
    try$(_expectSerialized(_driver, "display", "flex run-in", "run-in flex"));
    try$(_expectSerialized(_driver, "display", "run-in flex", "run-in flex"));
    try$(_expectSerialized(_driver, "display", "grid run-in", "run-in grid"));
    try$(_expectSerialized(_driver, "display", "run-in grid", "run-in grid"));
    try$(_expectSerialized(_driver, "display", "table run-in", "run-in table"));
    try$(_expectSerialized(_driver, "display", "run-in table", "run-in table"));
    try$(_expectSerialized(_driver, "display", "run-in ruby", "run-in ruby"));
    try$(_expectSerialized(_driver, "display", "ruby run-in", "run-in ruby"));
    try$(_expectSerialized(_driver, "display", "run-in list-item", "run-in list-item"));
    try$(_expectSerialized(_driver, "display", "list-item run-in", "run-in list-item"));
    try$(_expectSerialized(_driver, "display", "flow run-in list-item", "run-in list-item"));
    try$(_expectSerialized(_driver, "display", "run-in flow list-item", "run-in list-item"));
    try$(_expectSerialized(_driver, "display", "flow list-item run-in", "run-in list-item"));
    try$(_expectSerialized(_driver, "display", "run-in list-item flow", "run-in list-item"));
    try$(_expectSerialized(_driver, "display", "list-item run-in flow", "run-in list-item"));
    try$(_expectSerialized(_driver, "display", "list-item flow run-in", "run-in list-item"));
    try$(_expectSerialized(_driver, "display", "flow-root run-in list-item", "run-in flow-root list-item"));
    try$(_expectSerialized(_driver, "display", "run-in flow-root list-item", "run-in flow-root list-item"));
    try$(_expectSerialized(_driver, "display", "flow-root list-item run-in", "run-in flow-root list-item"));
    try$(_expectSerialized(_driver, "display", "run-in list-item flow-root", "run-in flow-root list-item"));
    try$(_expectSerialized(_driver, "display", "list-item run-in flow-root", "run-in flow-root list-item"));
    try$(_expectSerialized(_driver, "display", "list-item flow-root run-in", "run-in flow-root list-item"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-env/env-parsing.html
test$("vaev-style-wpt-css-css-env-env-parsing") {
    try$(_expectInvalid(_driver, "width", "env(safe-area-inset-top ())"));
    try$(_expectInvalid(_driver, "width", "env(safe-area-inset-top () )"));
    try$(_expectInvalid(_driver, "width", "env(safe-area-inset-top() )"));
    try$(_expectInvalid(_driver, "width", "env(safe-area-inset-top (),)"));
    try$(_expectInvalid(_driver, "width", "env(safe-area-inset-top(),)"));
    return Ok();
}

test$("vaev-style-wpt-css-css-env-env-parsing-xfail", XFAIL) {
    try$(_expectSerialized(_driver, "width", "env(safe-area-inset-top)", "env(safe-area-inset-top)"));
    try$(_expectSerialized(_driver, "width", "env(safe-area-inset-top,)", "env(safe-area-inset-top,)"));
    try$(_expectSerialized(_driver, "width", "env(safe-area-inset-top, )", "env(safe-area-inset-top, )"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-flexbox/balance/flex-wrap-invalid.html
test$("vaev-style-wpt-css-css-flexbox-balance-flex-wrap-invalid") {
    try$(_expectInvalid(_driver, "flex-wrap", "auto"));
    try$(_expectInvalid(_driver, "flex-wrap", "nowrap wrap"));
    try$(_expectInvalid(_driver, "flex-wrap", "wrap wrap"));
    try$(_expectInvalid(_driver, "flex-wrap", "balance balance"));
    try$(_expectInvalid(_driver, "flex-wrap", "balance nowrap"));
    try$(_expectInvalid(_driver, "flex-wrap", "nowrap balance"));
    try$(_expectInvalid(_driver, "flex-wrap", "wrap balance wrap"));
    try$(_expectInvalid(_driver, "flex-wrap", "balance wrap balance"));
    try$(_expectInvalid(_driver, "flex-wrap", "wrap 2 balance"));
    try$(_expectInvalid(_driver, "flex-wrap", "2 balance wrap"));
    try$(_expectInvalid(_driver, "flex-wrap", "balance 0"));
    try$(_expectInvalid(_driver, "flex-wrap", "balance 1.5"));
    try$(_expectInvalid(_driver, "flex-wrap", "balance -1"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-flexbox/balance/flex-wrap-valid.html
test$("vaev-style-wpt-css-css-flexbox-balance-flex-wrap-valid") {
    try$(_expectSerialized(_driver, "flex-wrap", "nowrap", "nowrap"));
    try$(_expectSerialized(_driver, "flex-wrap", "wrap", "wrap"));
    try$(_expectSerialized(_driver, "flex-wrap", "wrap-reverse", "wrap-reverse"));
    return Ok();
}

test$("vaev-style-wpt-css-css-flexbox-balance-flex-wrap-valid-xfail", XFAIL) {
    try$(_expectSerialized(_driver, "flex-wrap", "balance", "balance"));
    try$(_expectSerialized(_driver, "flex-wrap", "wrap balance", "balance"));
    try$(_expectSerialized(_driver, "flex-wrap", "wrap-reverse balance", "wrap-reverse balance"));
    try$(_expectSerialized(_driver, "flex-wrap", "balance wrap", "balance"));
    try$(_expectSerialized(_driver, "flex-wrap", "balance wrap-reverse", "wrap-reverse balance"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-flexbox/parsing/flex-basis-invalid.html
test$("vaev-style-wpt-css-css-flexbox-parsing-flex-basis-invalid") {
    try$(_expectInvalid(_driver, "flex-basis", "none"));
    try$(_expectInvalid(_driver, "flex-basis", "auto content"));
    try$(_expectInvalid(_driver, "flex-basis", "3px 4%"));
    try$(_expectInvalid(_driver, "flex-basis", "anchor-size(--a width)"));
    try$(_expectInvalid(_driver, "flex-basis", "anchor-size(--a width, 10px)"));
    return Ok();
}

test$("vaev-style-wpt-css-css-flexbox-parsing-flex-basis-invalid-xfail", XFAIL) {
    try$(_expectInvalid(_driver, "flex-basis", "-1px"));
    try$(_expectInvalid(_driver, "flex-basis", "-2%"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-flexbox/parsing/flex-basis-valid.html
test$("vaev-style-wpt-css-css-flexbox-parsing-flex-basis-valid") {
    try$(_expectSerialized(_driver, "flex-basis", "1px", "1px"));
    try$(_expectSerialized(_driver, "flex-basis", "calc(2em + 3ex)", "calc(2em + 3ex)"));
    try$(_expectSerialized(_driver, "flex-basis", "400%", "400%"));
    try$(_expectSerialized(_driver, "flex-basis", "auto", "auto"));
    try$(_expectSerialized(_driver, "flex-basis", "fit-content", "fit-content"));
    try$(_expectSerialized(_driver, "flex-basis", "min-content", "min-content"));
    try$(_expectSerialized(_driver, "flex-basis", "max-content", "max-content"));
    try$(_expectSerialized(_driver, "flex-basis", "content", "content"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-flexbox/parsing/flex-direction-invalid.html
test$("vaev-style-wpt-css-css-flexbox-parsing-flex-direction-invalid") {
    try$(_expectInvalid(_driver, "flex-direction", "auto"));
    try$(_expectInvalid(_driver, "flex-direction", "column row-reverse"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-flexbox/parsing/flex-direction-valid.html
test$("vaev-style-wpt-css-css-flexbox-parsing-flex-direction-valid") {
    try$(_expectSerialized(_driver, "flex-direction", "row", "row"));
    try$(_expectSerialized(_driver, "flex-direction", "row-reverse", "row-reverse"));
    try$(_expectSerialized(_driver, "flex-direction", "column", "column"));
    try$(_expectSerialized(_driver, "flex-direction", "column-reverse", "column-reverse"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-flexbox/parsing/flex-flow-invalid.html
test$("vaev-style-wpt-css-css-flexbox-parsing-flex-flow-invalid") {
    try$(_expectInvalid(_driver, "flex-flow", "nowrap row nowrap"));
    try$(_expectInvalid(_driver, "flex-flow", "column wrap column"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-flexbox/parsing/flex-grow-invalid.html
test$("vaev-style-wpt-css-css-flexbox-parsing-flex-grow-invalid") {
    try$(_expectInvalid(_driver, "flex-grow", "2e3.4"));
    try$(_expectInvalid(_driver, "flex-grow", "-+5"));
    try$(_expectInvalid(_driver, "flex-grow", "6 7"));
    try$(_expectInvalid(_driver, "flex-grow", "1."));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-flexbox/parsing/flex-grow-valid.html
test$("vaev-style-wpt-css-css-flexbox-parsing-flex-grow-valid") {
    try$(_expectSerialized(_driver, "flex-grow", "1", "1"));
    try$(_expectSerializedOneOf(_driver, "flex-grow", "23.4e5", {"2.34e+06", "2.34e+006", "2340000"}));
    try$(_expectSerializedOneOf(_driver, "flex-grow", "+.678E9", {"6.78e+08", "6.78e+008", "678000000"}));
    try$(_expectSerialized(_driver, "flex-grow", ".0", "0"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-flexbox/parsing/flex-invalid.html
test$("vaev-style-wpt-css-css-flexbox-parsing-flex-invalid") {
    try$(_expectInvalid(_driver, "flex", "none 1"));
    try$(_expectInvalid(_driver, "flex", "2 3 4"));
    try$(_expectInvalid(_driver, "flex", "5px 7%"));
    try$(_expectInvalid(_driver, "flex", "9 none"));
    return Ok();
}

test$("vaev-style-wpt-css-css-flexbox-parsing-flex-invalid-xfail", XFAIL) {
    try$(_expectInvalid(_driver, "flex", "1 2 calc(0)"));
    try$(_expectInvalid(_driver, "flex", "1 2 calc(3 - 3)"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-flexbox/parsing/flex-shrink-invalid.html
test$("vaev-style-wpt-css-css-flexbox-parsing-flex-shrink-invalid") {
    try$(_expectInvalid(_driver, "flex-shrink", "2e3.4"));
    try$(_expectInvalid(_driver, "flex-shrink", "-+5"));
    try$(_expectInvalid(_driver, "flex-shrink", "6 7"));
    try$(_expectInvalid(_driver, "flex-shrink", "1."));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-flexbox/parsing/flex-shrink-valid.html
test$("vaev-style-wpt-css-css-flexbox-parsing-flex-shrink-valid") {
    try$(_expectSerialized(_driver, "flex-shrink", "1", "1"));
    try$(_expectSerializedOneOf(_driver, "flex-shrink", "23.4e5", {"2.34e+06", "2.34e+006", "2340000"}));
    try$(_expectSerializedOneOf(_driver, "flex-shrink", "+.678E9", {"6.78e+08", "6.78e+008", "678000000"}));
    try$(_expectSerialized(_driver, "flex-shrink", ".0", "0"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-flexbox/parsing/flex-valid.html
test$("vaev-style-wpt-css-css-flexbox-parsing-flex-valid") {
    try$(_expectSerialized(_driver, "flex", "4 5 6px", "4 5 6px"));
    try$(_expectSerialized(_driver, "flex", "6px 4 5", "4 5 6px"));
    try$(_expectSerialized(_driver, "flex", "6px 4", "4 1 6px"));
    try$(_expectSerialized(_driver, "flex", "6px", "1 1 6px"));
    try$(_expectSerialized(_driver, "flex", "7% 8", "8 1 7%"));
    try$(_expectSerialized(_driver, "flex", "1 1 10em", "1 1 10em"));
    return Ok();
}

test$("vaev-style-wpt-css-css-flexbox-parsing-flex-valid-xfail", XFAIL) {
    try$(_expectSerialized(_driver, "flex", "none", "0 0 auto"));
    try$(_expectSerialized(_driver, "flex", "1", "1 1 0%"));
    try$(_expectSerialized(_driver, "flex", "2 3", "2 3 0%"));
    try$(_expectSerialized(_driver, "flex", "8 auto", "8 1 auto"));
    try$(_expectSerialized(_driver, "flex", "1 1 calc(10em)", "1 1 calc(10em)"));
    try$(_expectSerialized(_driver, "flex", "1 1 calc(-10em)", "1 1 calc(-10em)"));
    try$(_expectSerialized(_driver, "flex", "calc(10 + (sign(20cqw - 10px) * 5)) calc(10 + (sign(20cqw - 10px) * 5)) 1px", "calc(10 + (5 * sign(20cqw - 10px))) calc(10 + (5 * sign(20cqw - 10px))) 1px"));
    try$(_expectSerialized(_driver, "flex", "1 1 calc(10px + (sign(20cqw - 10px) * 5px))", "1 1 calc(10px + (5px * sign(20cqw - 10px)))"));
    try$(_expectSerialized(_driver, "flex", "calc(1) calc(2 + 1) calc(3px)", "calc(1) calc(3) calc(3px)"));
    try$(_expectSerialized(_driver, "flex", "calc(-1) calc(-1) 0", "calc(-1) calc(-1) 0px"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-flexbox/parsing/flex-wrap-invalid.html
test$("vaev-style-wpt-css-css-flexbox-parsing-flex-wrap-invalid") {
    try$(_expectInvalid(_driver, "flex-wrap", "auto"));
    try$(_expectInvalid(_driver, "flex-wrap", "nowrap wrap"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-flexbox/parsing/flex-wrap-valid.html
test$("vaev-style-wpt-css-css-flexbox-parsing-flex-wrap-valid") {
    try$(_expectSerialized(_driver, "flex-wrap", "nowrap", "nowrap"));
    try$(_expectSerialized(_driver, "flex-wrap", "wrap", "wrap"));
    try$(_expectSerialized(_driver, "flex-wrap", "wrap-reverse", "wrap-reverse"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-fonts/parsing/font-family-invalid.html
test$("vaev-style-wpt-css-css-fonts-parsing-font-family-invalid") {
    try$(_expectInvalid(_driver, "font-family", "Red/Black, sans-serif"));
    try$(_expectInvalid(_driver, "font-family", "Ahem!, sans-serif"));
    try$(_expectInvalid(_driver, "font-family", "test@foo, sans-serif"));
    try$(_expectInvalid(_driver, "font-family", "#POUND, sans-serif"));
    try$(_expectInvalid(_driver, "font-family", "Hawaii 5-0, sans-serif"));
    return Ok();
}

test$("vaev-style-wpt-css-css-fonts-parsing-font-family-invalid-xfail", XFAIL) {
    try$(_expectInvalid(_driver, "font-family", "cursive serif"));
    try$(_expectInvalid(_driver, "font-family", "\"Lucida\" Grande, sans-serif"));
    try$(_expectInvalid(_driver, "font-family", "FirstName, Initial, LastName"));
    try$(_expectInvalid(_driver, "font-family", "FirstName, inherit, LastName"));
    try$(_expectInvalid(_driver, "font-family", "FirstName, LastName, unset"));
    try$(_expectInvalid(_driver, "font-family", "default, FirstName, LastName"));
    try$(_expectInvalid(_driver, "font-family", "FirstName, LastName, revert"));
    try$(_expectInvalid(_driver, "font-family", "FirstName, LastName, revert-layer"));
    try$(_expectInvalid(_driver, "font-family", "FirstName, LastName, revert-rule"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-fonts/parsing/font-family-valid.html
test$("vaev-style-wpt-css-css-fonts-parsing-font-family-valid") {
    try$(_expectSerialized(_driver, "font-family", "serif, sans-serif, cursive, fantasy, monospace, system-ui", "serif, sans-serif, cursive, fantasy, monospace, system-ui"));
    try$(_expectSerialized(_driver, "font-family", "Helvetica, Verdana, sans-serif", "Helvetica, Verdana, sans-serif"));
    try$(_expectSerialized(_driver, "font-family", "\"New Century Schoolbook\", serif", "New Century Schoolbook, serif"));
    return Ok();
}

test$("vaev-style-wpt-css-css-fonts-parsing-font-family-valid-xfail", XFAIL) {
    try$(_expectSerialized(_driver, "font-family", "Serif", "serif"));
    try$(_expectSerialized(_driver, "font-family", "Sans-Serif", "sans-serif"));
    try$(_expectSerialized(_driver, "font-family", "Cursive", "cursive"));
    try$(_expectSerialized(_driver, "font-family", "Fantasy", "fantasy"));
    try$(_expectSerialized(_driver, "font-family", "Monospace", "monospace"));
    try$(_expectSerialized(_driver, "font-family", "System-UI", "system-ui"));
    try$(_expectSerialized(_driver, "font-family", "'21st Century', fantasy", "\"21st Century\", fantasy"));
    try$(_expectSerialized(_driver, "font-family", "\"inherit\", \"serif\"", "\"inherit\", \"serif\""));
    try$(_expectSerialized(_driver, "font-family", "\"Serif\"", "\"Serif\""));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-fonts/parsing/font-invalid.html
test$("vaev-style-wpt-css-css-fonts-parsing-font-invalid") {
    try$(_expectInvalid(_driver, "font", "menu icon"));
    try$(_expectInvalid(_driver, "font", "small-caps small-caps medium/normal sans-serif"));
    try$(_expectInvalid(_driver, "font", "italic small-caps lighter condensed normal smaller monospace"));
    try$(_expectInvalid(_driver, "font", "normal 100 semi-condensed oblique small-caps 10px/normal Menu"));
    try$(_expectInvalid(_driver, "font", "normal normal normal normal normal 20%/1.2 \"FB Armada\""));
    try$(_expectInvalid(_driver, "font", "italic small-caps lighter condensed smaller"));
    try$(_expectInvalid(_driver, "font", "normal 100 semi-condensed oblique small-caps Menu"));
    try$(_expectInvalid(_driver, "font", "oldstyle-nums medium serif"));
    try$(_expectInvalid(_driver, "font", "common-ligatures medium serif"));
    try$(_expectInvalid(_driver, "font", "normal full-width medium serif"));
    try$(_expectInvalid(_driver, "font", "italic titling-caps medium serif"));
    try$(_expectInvalid(_driver, "font", "petite-caps medium serif"));
    return Ok();
}

test$("vaev-style-wpt-css-css-fonts-parsing-font-invalid-xfail", XFAIL) {
    try$(_expectInvalid(_driver, "font", "italic oblique xx-small serif"));
    try$(_expectInvalid(_driver, "font", "bold bolder xx-large/1.2 cursive"));
    try$(_expectInvalid(_driver, "font", "ultra-condensed extra-condensed larger/calc(120% + 1.2em) fantasy"));
    try$(_expectInvalid(_driver, "font", "100% larger / 2 fantasy"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-fonts/parsing/font-size-invalid.html
test$("vaev-style-wpt-css-css-fonts-parsing-font-size-invalid") {
    try$(_expectInvalid(_driver, "font-size", "auto"));
    try$(_expectInvalid(_driver, "font-size", "medium small"));
    return Ok();
}

test$("vaev-style-wpt-css-css-fonts-parsing-font-size-invalid-xfail", XFAIL) {
    try$(_expectInvalid(_driver, "font-size", "-10px"));
    try$(_expectInvalid(_driver, "font-size", "-20%"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-fonts/parsing/font-size-valid.html
test$("vaev-style-wpt-css-css-fonts-parsing-font-size-valid") {
    try$(_expectSerialized(_driver, "font-size", "xx-small", "xx-small"));
    try$(_expectSerialized(_driver, "font-size", "x-small", "x-small"));
    try$(_expectSerialized(_driver, "font-size", "small", "small"));
    try$(_expectSerialized(_driver, "font-size", "medium", "medium"));
    try$(_expectSerialized(_driver, "font-size", "large", "large"));
    try$(_expectSerialized(_driver, "font-size", "x-large", "x-large"));
    try$(_expectSerialized(_driver, "font-size", "xx-large", "xx-large"));
    try$(_expectSerialized(_driver, "font-size", "larger", "larger"));
    try$(_expectSerialized(_driver, "font-size", "smaller", "smaller"));
    try$(_expectSerialized(_driver, "font-size", "10px", "10px"));
    try$(_expectSerialized(_driver, "font-size", "20%", "20%"));
    return Ok();
}

test$("vaev-style-wpt-css-css-fonts-parsing-font-size-valid-xfail", XFAIL) {
    try$(_expectSerialized(_driver, "font-size", "xxx-large", "xxx-large"));
    try$(_expectSerialized(_driver, "font-size", "calc(30% - 40px)", "calc(30% - 40px)"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-fonts/parsing/font-style-invalid.html
test$("vaev-style-wpt-css-css-fonts-parsing-font-style-invalid") {
    try$(_expectInvalid(_driver, "font-style", "auto"));
    try$(_expectInvalid(_driver, "font-style", "italic oblique"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-fonts/parsing/font-style-valid.html
test$("vaev-style-wpt-css-css-fonts-parsing-font-style-valid") {
    try$(_expectSerialized(_driver, "font-style", "normal", "normal"));
    try$(_expectSerialized(_driver, "font-style", "italic", "italic"));
    try$(_expectSerialized(_driver, "font-style", "oblique", "oblique"));
    try$(_expectSerialized(_driver, "font-style", "oblique 0deg", "normal"));
    try$(_expectSerialized(_driver, "font-style", "oblique 10deg", "oblique 10deg"));
    try$(_expectSerialized(_driver, "font-style", "oblique -10deg", "oblique -10deg"));
    try$(_expectSerialized(_driver, "font-style", "oblique -14deg", "oblique -14deg"));
    try$(_expectSerialized(_driver, "font-style", "oblique -90deg", "oblique -90deg"));
    try$(_expectSerialized(_driver, "font-style", "oblique 90deg", "oblique 90deg"));
    return Ok();
}

test$("vaev-style-wpt-css-css-fonts-parsing-font-style-valid-xfail", XFAIL) {
    try$(_expectSerialized(_driver, "font-style", "oblique calc(10deg - 10deg)", "oblique calc(0deg)"));
    try$(_expectSerialized(_driver, "font-style", "oblique 14deg", "oblique 14deg"));
    try$(_expectSerialized(_driver, "font-style", "oblique 10grad", "oblique 10grad"));
    try$(_expectSerialized(_driver, "font-style", "oblique calc(90deg)", "oblique calc(90deg)"));
    try$(_expectSerialized(_driver, "font-style", "oblique calc(100deg)", "oblique calc(100deg)"));
    try$(_expectSerialized(_driver, "font-style", "oblique calc(-100deg)", "oblique calc(-100deg)"));
    try$(_expectSerialized(_driver, "font-style", "oblique calc(30deg * 2)", "oblique calc(60deg)"));
    try$(_expectSerialized(_driver, "font-style", "oblique calc(30deg + (sign(2cqw - 10px) * 5deg))", "oblique calc(30deg + (5deg * sign(2cqw - 10px)))"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-fonts/parsing/font-weight-invalid.html
test$("vaev-style-wpt-css-css-fonts-parsing-font-weight-invalid") {
    try$(_expectInvalid(_driver, "font-weight", "auto"));
    try$(_expectInvalid(_driver, "font-weight", "bold 900"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-fonts/parsing/font-weight-valid.html
test$("vaev-style-wpt-css-css-fonts-parsing-font-weight-valid") {
    try$(_expectSerialized(_driver, "font-weight", "bolder", "bolder"));
    try$(_expectSerialized(_driver, "font-weight", "lighter", "lighter"));
    try$(_expectSerialized(_driver, "font-weight", "1", "1"));
    try$(_expectSerialized(_driver, "font-weight", "100", "100"));
    try$(_expectSerialized(_driver, "font-weight", "200", "200"));
    try$(_expectSerialized(_driver, "font-weight", "300", "300"));
    try$(_expectSerialized(_driver, "font-weight", "400", "400"));
    try$(_expectSerialized(_driver, "font-weight", "500", "500"));
    try$(_expectSerialized(_driver, "font-weight", "600", "600"));
    try$(_expectSerialized(_driver, "font-weight", "700", "700"));
    try$(_expectSerialized(_driver, "font-weight", "800", "800"));
    try$(_expectSerialized(_driver, "font-weight", "900", "900"));
    try$(_expectSerialized(_driver, "font-weight", "1000", "1000"));
    try$(_expectSerialized(_driver, "font-weight", "101", "101"));
    return Ok();
}

test$("vaev-style-wpt-css-css-fonts-parsing-font-weight-valid-xfail", XFAIL) {
    try$(_expectSerialized(_driver, "font-weight", "normal", "normal"));
    try$(_expectSerialized(_driver, "font-weight", "bold", "bold"));
    try$(_expectSerialized(_driver, "font-weight", "150.25", "150.25"));
    try$(_expectSerialized(_driver, "font-weight", "calc(100)", "calc(100)"));
    try$(_expectSerialized(_driver, "font-weight", "calc(0)", "calc(0)"));
    try$(_expectSerialized(_driver, "font-weight", "calc(-100)", "calc(-100)"));
    try$(_expectSerialized(_driver, "font-weight", "calc(100 + 100)", "calc(200)"));
    try$(_expectSerialized(_driver, "font-weight", "calc(100 + (sign(20cqw - 10px) * 5))", "calc(100 + (5 * sign(20cqw - 10px)))"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-fonts/parsing/font-width-invalid.html
test$("vaev-style-wpt-css-css-fonts-parsing-font-width-invalid") {
    try$(_expectInvalid(_driver, "font-width", "auto"));
    try$(_expectInvalid(_driver, "font-width", "normal, ultra-condensed"));
    try$(_expectInvalid(_driver, "font-width", "condensed expanded"));
    try$(_expectInvalid(_driver, "font-stretch", "auto"));
    try$(_expectInvalid(_driver, "font-stretch", "normal, ultra-condensed"));
    try$(_expectInvalid(_driver, "font-stretch", "condensed expanded"));
    return Ok();
}

test$("vaev-style-wpt-css-css-fonts-parsing-font-width-invalid-xfail", XFAIL) {
    try$(_expectInvalid(_driver, "font-width", "-50%"));
    try$(_expectInvalid(_driver, "font-stretch", "-50%"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-fonts/parsing/font-width-valid.html
test$("vaev-style-wpt-css-css-fonts-parsing-font-width-valid") {
    try$(_expectSerialized(_driver, "font-width", "234.5%", "234.5%"));
    try$(_expectSerialized(_driver, "font-stretch", "234.5%", "234.5%"));
    return Ok();
}

test$("vaev-style-wpt-css-css-fonts-parsing-font-width-valid-xfail", XFAIL) {
    try$(_expectSerialized(_driver, "font-width", "normal", "normal"));
    try$(_expectSerialized(_driver, "font-width", "ultra-condensed", "ultra-condensed"));
    try$(_expectSerialized(_driver, "font-width", "extra-condensed", "extra-condensed"));
    try$(_expectSerialized(_driver, "font-width", "condensed", "condensed"));
    try$(_expectSerialized(_driver, "font-width", "semi-condensed", "semi-condensed"));
    try$(_expectSerialized(_driver, "font-width", "semi-expanded", "semi-expanded"));
    try$(_expectSerialized(_driver, "font-width", "expanded", "expanded"));
    try$(_expectSerialized(_driver, "font-width", "extra-expanded", "extra-expanded"));
    try$(_expectSerialized(_driver, "font-width", "ultra-expanded", "ultra-expanded"));
    try$(_expectSerialized(_driver, "font-width", "calc(100%)", "calc(100%)"));
    try$(_expectSerialized(_driver, "font-width", "calc(0%)", "calc(0%)"));
    try$(_expectSerialized(_driver, "font-width", "calc(-100%)", "calc(-100%)"));
    try$(_expectSerialized(_driver, "font-width", "calc(100% + 100%)", "calc(200%)"));
    try$(_expectSerialized(_driver, "font-width", "calc(100% + (sign(20cqw - 10px) * 5%))", "calc(100% + (5% * sign(20cqw - 10px)))"));
    try$(_expectSerialized(_driver, "font-stretch", "normal", "normal"));
    try$(_expectSerialized(_driver, "font-stretch", "ultra-condensed", "ultra-condensed"));
    try$(_expectSerialized(_driver, "font-stretch", "extra-condensed", "extra-condensed"));
    try$(_expectSerialized(_driver, "font-stretch", "condensed", "condensed"));
    try$(_expectSerialized(_driver, "font-stretch", "semi-condensed", "semi-condensed"));
    try$(_expectSerialized(_driver, "font-stretch", "semi-expanded", "semi-expanded"));
    try$(_expectSerialized(_driver, "font-stretch", "expanded", "expanded"));
    try$(_expectSerialized(_driver, "font-stretch", "extra-expanded", "extra-expanded"));
    try$(_expectSerialized(_driver, "font-stretch", "ultra-expanded", "ultra-expanded"));
    try$(_expectSerialized(_driver, "font-stretch", "calc(100%)", "calc(100%)"));
    try$(_expectSerialized(_driver, "font-stretch", "calc(0%)", "calc(0%)"));
    try$(_expectSerialized(_driver, "font-stretch", "calc(-100%)", "calc(-100%)"));
    try$(_expectSerialized(_driver, "font-stretch", "calc(100% + 100%)", "calc(200%)"));
    try$(_expectSerialized(_driver, "font-stretch", "calc(100% + (sign(20cqw - 10px) * 5%))", "calc(100% + (5% * sign(20cqw - 10px)))"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-images/image-set/image-set-parsing.html
test$("vaev-style-wpt-css-css-images-image-set-image-set-parsing") {
    try$(_expectInvalid(_driver, "background-image", "image-set(url(\"example.png\") -20x)"));
    try$(_expectInvalid(_driver, "background-image", "-webkit-image-set(url(\"example.png\") -20x)"));
    try$(_expectInvalid(_driver, "background-image", "image-set(none, url(example.png) 1x)"));
    try$(_expectInvalid(_driver, "background-image", "-webkit-image-set(none, url(example.png) 1x)"));
    try$(_expectInvalid(_driver, "background-image", "image-set()"));
    try$(_expectInvalid(_driver, "background-image", "-webkit-image-set()"));
    try$(_expectInvalid(_driver, "background-image", "image-set('example.jpeg' 92pid url(example.png) 1x)"));
    try$(_expectInvalid(_driver, "background-image", "-webkit-image-set('example.jpeg' 92pid url(example.png) 1x)"));
    try$(_expectInvalid(_driver, "background-image", "image-set(url(example.png) 1x url(example.jpeg))"));
    try$(_expectInvalid(_driver, "background-image", "-webkit-image-set(url(example.png) 1x url(example.jpeg))"));
    try$(_expectInvalid(_driver, "background-image", "image-set(url(example.png) 1x 2x)"));
    try$(_expectInvalid(_driver, "background-image", "-webkit-image-set(url(example.png) 1x 2x)"));
    try$(_expectInvalid(_driver, "background-image", "image-set(url(\"example.png\") 1invalidResUnit)"));
    try$(_expectInvalid(_driver, "background-image", "-webkit-image-set(url(\"example.png\") 1invalidResUnit)"));
    try$(_expectInvalid(_driver, "background-image", "image-set(url(\"example.png\") calc(3 * 4))"));
    try$(_expectInvalid(_driver, "background-image", "-webkit-image-set(url(\"example.png\") calc(3 * 4))"));
    try$(_expectInvalid(_driver, "background-image", "image-set(url(\"example.png\") calc(2 - 1))"));
    try$(_expectInvalid(_driver, "background-image", "-webkit-image-set(url(\"example.png\") calc(2 - 1))"));
    try$(_expectInvalid(_driver, "background-image", "image-set(url(\"example.png\") calc(2x - 1))"));
    try$(_expectInvalid(_driver, "background-image", "-webkit-image-set(url(\"example.png\") calc(2x - 1))"));
    try$(_expectInvalid(_driver, "background-image", "image-set(url(\"example.png\") calc(1 + 4dpi))"));
    try$(_expectInvalid(_driver, "background-image", "-webkit-image-set(url(\"example.png\") calc(1 + 4dpi))"));
    try$(_expectInvalid(_driver, "background-image", "image-set(url(\"example.png\") -1x)"));
    try$(_expectInvalid(_driver, "background-image", "-webkit-image-set(url(\"example.png\") -1x)"));
    try$(_expectInvalid(_driver, "background-image", "image-set(url(\"example.png\") -3dppx)"));
    try$(_expectInvalid(_driver, "background-image", "-webkit-image-set(url(\"example.png\") -3dppx)"));
    try$(_expectInvalid(_driver, "background-image", "image-set(url(\"example.png\") -96dpi)"));
    try$(_expectInvalid(_driver, "background-image", "-webkit-image-set(url(\"example.png\") -96dpi)"));
    try$(_expectInvalid(_driver, "background-image", "image-set(url(\"example.png\") -113dpcm)"));
    try$(_expectInvalid(_driver, "background-image", "-webkit-image-set(url(\"example.png\") -113dpcm)"));
    try$(_expectInvalid(_driver, "background-image", "image-set(url(example.png) type(image/png))"));
    try$(_expectInvalid(_driver, "background-image", "-webkit-image-set(url(example.png) type(image/png))"));
    try$(_expectInvalid(_driver, "background-image", "image-set(url(example.png) type('image/png') type('image/png'))"));
    try$(_expectInvalid(_driver, "background-image", "-webkit-image-set(url(example.png) type('image/png') type('image/png'))"));
    try$(_expectInvalid(_driver, "background-image", "image-set(url(example.png) type('image/png' 'image/png'))"));
    try$(_expectInvalid(_driver, "background-image", "-webkit-image-set(url(example.png) type('image/png' 'image/png'))"));
    try$(_expectInvalid(_driver, "background-image", "image-set(url(example.png) type(url('image/png')))"));
    try$(_expectInvalid(_driver, "background-image", "-webkit-image-set(url(example.png) type(url('image/png')))"));
    try$(_expectInvalid(_driver, "background-image", "image-set(url(example.png) 1xtype('image/png'))"));
    try$(_expectInvalid(_driver, "background-image", "-webkit-image-set(url(example.png) 1xtype('image/png'))"));
    try$(_expectInvalid(_driver, "background-image", "image-set(type('image/png') url(example.png) 1x)"));
    try$(_expectInvalid(_driver, "background-image", "-webkit-image-set(type('image/png') url(example.png) 1x)"));
    try$(_expectInvalid(_driver, "background-image", "image-set(image-set(url(example.png)) 2x)"));
    try$(_expectInvalid(_driver, "background-image", "-webkit-image-set(image-set(url(example.png)) 2x)"));
    try$(_expectInvalid(_driver, "background-image", "image-set(image(image-set(url(example.png)) 2x) 2x)"));
    try$(_expectInvalid(_driver, "background-image", "-webkit-image-set(image(image-set(url(example.png)) 2x) 2x)"));
    return Ok();
}

test$("vaev-style-wpt-css-css-images-image-set-image-set-parsing-xfail", XFAIL) {
    try$(_expectSerialized(_driver, "background-image", "image-set(url(example.png) 1x)", "image-set(url(\"example.png\") 1x)"));
    try$(_expectSerialized(_driver, "background-image", "-webkit-image-set(url(example.png) 1x)", "image-set(url(\"example.png\") 1x)"));
    try$(_expectSerialized(_driver, "background-image", "image-set('example.jpg' 1x)", "image-set(url(\"example.jpg\") 1x)"));
    try$(_expectSerialized(_driver, "background-image", "-webkit-image-set('example.jpg' 1x)", "image-set(url(\"example.jpg\") 1x)"));
    try$(_expectSerialized(_driver, "background-image", "image-set(url(example.png) 1x, 'example.png' 2x)", "image-set(url(\"example.png\") 1x, url(\"example.png\") 2x)"));
    try$(_expectSerialized(_driver, "background-image", "-webkit-image-set(url(example.png) 1x, 'example.png' 2x)", "image-set(url(\"example.png\") 1x, url(\"example.png\") 2x)"));
    try$(_expectSerialized(_driver, "background-image", "image-set(url(example.png) 1dpcm, 'example.png' 2x)", "image-set(url(\"example.png\") 1dpcm, url(\"example.png\") 2x)"));
    try$(_expectSerialized(_driver, "background-image", "-webkit-image-set(url(example.png) 1dpcm, 'example.png' 2x)", "image-set(url(\"example.png\") 1dpcm, url(\"example.png\") 2x)"));
    try$(_expectSerialized(_driver, "background-image", "image-set('example.jpeg' 222dpi, url(example.png) 3.5x)", "image-set(url(\"example.jpeg\") 222dpi, url(\"example.png\") 3.5x)"));
    try$(_expectSerialized(_driver, "background-image", "-webkit-image-set('example.jpeg' 222dpi, url(example.png) 3.5x)", "image-set(url(\"example.jpeg\") 222dpi, url(\"example.png\") 3.5x)"));
    try$(_expectSerialized(_driver, "content", "image-set(url(\"example.png\") 1x)", "image-set(url(\"example.png\") 1x)"));
    try$(_expectSerialized(_driver, "content", "-webkit-image-set(url(\"example.png\") 1x)", "image-set(url(\"example.png\") 1x)"));
    try$(_expectSerialized(_driver, "content", "image-set(url(\"example.png\") 1x, \"example.png\" 3x)", "image-set(url(\"example.png\") 1x, url(\"example.png\") 3x)"));
    try$(_expectSerialized(_driver, "content", "-webkit-image-set(url(\"example.png\") 1x, \"example.png\" 3x)", "image-set(url(\"example.png\") 1x, url(\"example.png\") 3x)"));
    try$(_expectSerialized(_driver, "background-image", "image-set(url(foo))", "image-set(url(\"foo\") 1x)"));
    try$(_expectSerialized(_driver, "background-image", "-webkit-image-set(url(foo))", "image-set(url(\"foo\") 1x)"));
    try$(_expectSerialized(_driver, "background-image", "image-set(url(foo), url(bar) 1x)", "image-set(url(\"foo\") 1x, url(\"bar\") 1x)"));
    try$(_expectSerialized(_driver, "background-image", "-webkit-image-set(url(foo), url(bar) 1x)", "image-set(url(\"foo\") 1x, url(\"bar\") 1x)"));
    try$(_expectSerialized(_driver, "background-image", "image-set(url(foo) 1x, url(bar))", "image-set(url(\"foo\") 1x, url(\"bar\") 1x)"));
    try$(_expectSerialized(_driver, "background-image", "-webkit-image-set(url(foo) 1x, url(bar))", "image-set(url(\"foo\") 1x, url(\"bar\") 1x)"));
    try$(_expectSerialized(_driver, "background-image", "image-set(url(foo), url(bar))", "image-set(url(\"foo\") 1x, url(\"bar\") 1x)"));
    try$(_expectSerialized(_driver, "background-image", "-webkit-image-set(url(foo), url(bar))", "image-set(url(\"foo\") 1x, url(\"bar\") 1x)"));
    try$(_expectSerialized(_driver, "background-image", "image-set(url(foo) 1x, url(bar), url(baz) 2x)", "image-set(url(\"foo\") 1x, url(\"bar\") 1x, url(\"baz\") 2x)"));
    try$(_expectSerialized(_driver, "background-image", "-webkit-image-set(url(foo) 1x, url(bar), url(baz) 2x)", "image-set(url(\"foo\") 1x, url(\"bar\") 1x, url(\"baz\") 2x)"));
    try$(_expectSerialized(_driver, "background-image", "image-set(url(\"example.png\") 1x)", "image-set(url(\"example.png\") 1x)"));
    try$(_expectSerialized(_driver, "background-image", "-webkit-image-set(url(\"example.png\") 1x)", "image-set(url(\"example.png\") 1x)"));
    try$(_expectSerialized(_driver, "background-image", "image-set(url(\"example.png\") calc(2x * 3))", "image-set(url(\"example.png\") calc(6dppx))"));
    try$(_expectSerialized(_driver, "background-image", "-webkit-image-set(url(\"example.png\") calc(2x * 3))", "image-set(url(\"example.png\") calc(6dppx))"));
    try$(_expectSerialized(_driver, "background-image", "image-set(url(\"example.png\") 1dppx)", "image-set(url(\"example.png\") 1dppx)"));
    try$(_expectSerialized(_driver, "background-image", "-webkit-image-set(url(\"example.png\") 1dppx)", "image-set(url(\"example.png\") 1dppx)"));
    try$(_expectSerialized(_driver, "background-image", "image-set(url(\"example.png\") calc(1dppx * 1))", "image-set(url(\"example.png\") calc(1dppx))"));
    try$(_expectSerialized(_driver, "background-image", "-webkit-image-set(url(\"example.png\") calc(1dppx * 1))", "image-set(url(\"example.png\") calc(1dppx))"));
    try$(_expectSerialized(_driver, "background-image", "image-set(url(\"example.png\") 1dpi)", "image-set(url(\"example.png\") 1dpi)"));
    try$(_expectSerialized(_driver, "background-image", "-webkit-image-set(url(\"example.png\") 1dpi)", "image-set(url(\"example.png\") 1dpi)"));
    try$(_expectSerialized(_driver, "background-image", "image-set(url(\"example.png\") calc(96dpi * 2))", "image-set(url(\"example.png\") calc(2dppx))"));
    try$(_expectSerialized(_driver, "background-image", "-webkit-image-set(url(\"example.png\") calc(96dpi * 2))", "image-set(url(\"example.png\") calc(2dppx))"));
    try$(_expectSerialized(_driver, "background-image", "image-set(url(\"example.png\") 1dpcm)", "image-set(url(\"example.png\") 1dpcm)"));
    try$(_expectSerialized(_driver, "background-image", "-webkit-image-set(url(\"example.png\") 1dpcm)", "image-set(url(\"example.png\") 1dpcm)"));
    try$(_expectSerialized(_driver, "background-image", "image-set(url(\"example.png\") calc(1dpcm * 96/2.54))", "image-set(url(\"example.png\") calc(1dppx))"));
    try$(_expectSerialized(_driver, "background-image", "-webkit-image-set(url(\"example.png\") calc(1dpcm * 96/2.54))", "image-set(url(\"example.png\") calc(1dppx))"));
    try$(_expectSerialized(_driver, "background-image", "image-set(url(\"example.png\") 1x, url(\"example.png\") 2dppx, \"example.png\" 250dpi, \"example.png\" 1dpcm)", "image-set(url(\"example.png\") 1x, url(\"example.png\") 2dppx, url(\"example.png\") 250dpi, url(\"example.png\") 1dpcm)"));
    try$(_expectSerialized(_driver, "background-image", "-webkit-image-set(url(\"example.png\") 1x, url(\"example.png\") 2dppx, \"example.png\" 250dpi, \"example.png\" 1dpcm)", "image-set(url(\"example.png\") 1x, url(\"example.png\") 2dppx, url(\"example.png\") 250dpi, url(\"example.png\") 1dpcm)"));
    try$(_expectSerialized(_driver, "content", "image-set(url(\"example.png\") 1dpi)", "image-set(url(\"example.png\") 1dpi)"));
    try$(_expectSerialized(_driver, "content", "-webkit-image-set(url(\"example.png\") 1dpi)", "image-set(url(\"example.png\") 1dpi)"));
    try$(_expectSerialized(_driver, "content", "image-set(url(\"example.png\") calc(1 * 96dpi))", "image-set(url(\"example.png\") calc(1dppx))"));
    try$(_expectSerialized(_driver, "content", "-webkit-image-set(url(\"example.png\") calc(1 * 96dpi))", "image-set(url(\"example.png\") calc(1dppx))"));
    try$(_expectSerialized(_driver, "background-image", "image-set(url(\"example.png\") calc(1dppx * sibling-index()))", "image-set(url(\"example.png\") calc(1dppx * sibling-index()))"));
    try$(_expectSerialized(_driver, "background-image", "-webkit-image-set(url(\"example.png\") calc(1dppx * sibling-index()))", "image-set(url(\"example.png\") calc(1dppx * sibling-index()))"));
    try$(_expectSerialized(_driver, "background-image", "image-set(url(\"example.png\") calc(1dppx * sign(1em - 10px)))", "image-set(url(\"example.png\") calc(1dppx * sign(1em - 10px)))"));
    try$(_expectSerialized(_driver, "background-image", "-webkit-image-set(url(\"example.png\") calc(1dppx * sign(1em - 10px)))", "image-set(url(\"example.png\") calc(1dppx * sign(1em - 10px)))"));
    try$(_expectSerialized(_driver, "background-image", "image-set(url(\"example.png\") calc(1dppx * sign(10px)))", "image-set(url(\"example.png\") calc(1dppx))"));
    try$(_expectSerialized(_driver, "background-image", "-webkit-image-set(url(\"example.png\") calc(1dppx * sign(10px)))", "image-set(url(\"example.png\") calc(1dppx))"));
    try$(_expectSerialized(_driver, "background-image", "image-set(url(\"example.png\") 0x)", "image-set(url(\"example.png\") 0x)"));
    try$(_expectSerialized(_driver, "background-image", "-webkit-image-set(url(\"example.png\") 0x)", "image-set(url(\"example.png\") 0x)"));
    try$(_expectSerialized(_driver, "background-image", "image-set(url(\"example.png\") 0dppx)", "image-set(url(\"example.png\") 0dppx)"));
    try$(_expectSerialized(_driver, "background-image", "-webkit-image-set(url(\"example.png\") 0dppx)", "image-set(url(\"example.png\") 0dppx)"));
    try$(_expectSerialized(_driver, "background-image", "image-set(url(\"example.png\") 0dpi)", "image-set(url(\"example.png\") 0dpi)"));
    try$(_expectSerialized(_driver, "background-image", "-webkit-image-set(url(\"example.png\") 0dpi)", "image-set(url(\"example.png\") 0dpi)"));
    try$(_expectSerialized(_driver, "background-image", "image-set(url(\"example.png\") 0dpcm)", "image-set(url(\"example.png\") 0dpcm)"));
    try$(_expectSerialized(_driver, "background-image", "-webkit-image-set(url(\"example.png\") 0dpcm)", "image-set(url(\"example.png\") 0dpcm)"));
    try$(_expectSerialized(_driver, "background-image", "image-set(url(\"example.png\") calc(-1 * 1x))", "image-set(url(\"example.png\") calc(-1dppx))"));
    try$(_expectSerialized(_driver, "background-image", "-webkit-image-set(url(\"example.png\") calc(-1 * 1x))", "image-set(url(\"example.png\") calc(-1dppx))"));
    try$(_expectSerialized(_driver, "background-image", "image-set(url(\"example.png\") calc(1x + -1x))", "image-set(url(\"example.png\") calc(0dppx))"));
    try$(_expectSerialized(_driver, "background-image", "-webkit-image-set(url(\"example.png\") calc(1x + -1x))", "image-set(url(\"example.png\") calc(0dppx))"));
    try$(_expectSerialized(_driver, "background-image", "image-set(linear-gradient(black, white) 1x)", "image-set(linear-gradient(black, white) 1x)"));
    try$(_expectSerialized(_driver, "background-image", "-webkit-image-set(linear-gradient(black, white) 1x)", "image-set(linear-gradient(black, white) 1x)"));
    try$(_expectSerialized(_driver, "background-image", "image-set(repeating-linear-gradient(red, blue 25%) 1x)", "image-set(repeating-linear-gradient(red, blue 25%) 1x)"));
    try$(_expectSerialized(_driver, "background-image", "-webkit-image-set(repeating-linear-gradient(red, blue 25%) 1x)", "image-set(repeating-linear-gradient(red, blue 25%) 1x)"));
    try$(_expectSerialized(_driver, "background-image", "image-set(radial-gradient(black, white) 1x)", "image-set(radial-gradient(black, white) 1x)"));
    try$(_expectSerialized(_driver, "background-image", "-webkit-image-set(radial-gradient(black, white) 1x)", "image-set(radial-gradient(black, white) 1x)"));
    try$(_expectSerialized(_driver, "background-image", "image-set(repeating-radial-gradient(red, blue 25%) 1x)", "image-set(repeating-radial-gradient(red, blue 25%) 1x)"));
    try$(_expectSerialized(_driver, "background-image", "-webkit-image-set(repeating-radial-gradient(red, blue 25%) 1x)", "image-set(repeating-radial-gradient(red, blue 25%) 1x)"));
    try$(_expectSerialized(_driver, "background-image", "image-set(conic-gradient(black, white) 1x)", "image-set(conic-gradient(black, white) 1x)"));
    try$(_expectSerialized(_driver, "background-image", "-webkit-image-set(conic-gradient(black, white) 1x)", "image-set(conic-gradient(black, white) 1x)"));
    try$(_expectSerialized(_driver, "background-image", "image-set(repeating-conic-gradient(red, blue 25%) 1x)", "image-set(repeating-conic-gradient(red, blue 25%) 1x)"));
    try$(_expectSerialized(_driver, "background-image", "-webkit-image-set(repeating-conic-gradient(red, blue 25%) 1x)", "image-set(repeating-conic-gradient(red, blue 25%) 1x)"));
    try$(_expectSerialized(_driver, "content", "image-set(linear-gradient(black, white) 1x, url(\"example.png\") 4x)", "image-set(linear-gradient(black, white) 1x, url(\"example.png\") 4x)"));
    try$(_expectSerialized(_driver, "content", "-webkit-image-set(linear-gradient(black, white) 1x, url(\"example.png\") 4x)", "image-set(linear-gradient(black, white) 1x, url(\"example.png\") 4x)"));
    try$(_expectSerialized(_driver, "content", "image-set(url(\"example.png\") 192dpi, linear-gradient(black, white) 1x)", "image-set(url(\"example.png\") 192dpi, linear-gradient(black, white) 1x)"));
    try$(_expectSerialized(_driver, "content", "-webkit-image-set(url(\"example.png\") 192dpi, linear-gradient(black, white) 1x)", "image-set(url(\"example.png\") 192dpi, linear-gradient(black, white) 1x)"));
    try$(_expectSerialized(_driver, "background-image", "image-set(linear-gradient(red) 1x)", "image-set(linear-gradient(red) 1x)"));
    try$(_expectSerialized(_driver, "background-image", "-webkit-image-set(linear-gradient(red) 1x)", "image-set(linear-gradient(red) 1x)"));
    try$(_expectSerialized(_driver, "background-image", "image-set(url(\"example.png\") 1x type(\"image/png\"))", "image-set(url(\"example.png\") 1x type(\"image/png\"))"));
    try$(_expectSerialized(_driver, "background-image", "-webkit-image-set(url(\"example.png\") 1x type(\"image/png\"))", "image-set(url(\"example.png\") 1x type(\"image/png\"))"));
    try$(_expectSerialized(_driver, "background-image", "image-set(url(example.png) type('image/png'))", "image-set(url(\"example.png\") 1x type(\"image/png\"))"));
    try$(_expectSerialized(_driver, "background-image", "-webkit-image-set(url(example.png) type('image/png'))", "image-set(url(\"example.png\") 1x type(\"image/png\"))"));
    try$(_expectSerialized(_driver, "background-image", "image-set(url(example.png) type('image/png') 1x)", "image-set(url(\"example.png\") 1x type(\"image/png\"))"));
    try$(_expectSerialized(_driver, "background-image", "-webkit-image-set(url(example.png) type('image/png') 1x)", "image-set(url(\"example.png\") 1x type(\"image/png\"))"));
    try$(_expectSerialized(_driver, "background-image", "image-set(url(example.png) 1x type('image/jpeg'))", "image-set(url(\"example.png\") 1x type(\"image/jpeg\"))"));
    try$(_expectSerialized(_driver, "background-image", "-webkit-image-set(url(example.png) 1x type('image/jpeg'))", "image-set(url(\"example.png\") 1x type(\"image/jpeg\"))"));
    try$(_expectSerialized(_driver, "background-image", "image-set(url(\"x\") 1x type(\"a\\\"\"))", "image-set(url(\"x\") 1x type(\"a\\\"\"))"));
    try$(_expectSerialized(_driver, "background-image", "-webkit-image-set(url(\"x\") 1x type(\"a\\\"\"))", "image-set(url(\"x\") 1x type(\"a\\\"\"))"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-images/parsing/conic-gradient-calc-angle-percentage-invalid.html
test$("vaev-style-wpt-css-css-images-parsing-conic-gradient-calc-angle-percentage-invalid") {
    try$(_expectInvalid(_driver, "background-image", "linear-gradient(red calc(50% + 30deg), blue)"));
    try$(_expectInvalid(_driver, "background-image", "radial-gradient(red calc(50% + 30deg), blue)"));
    try$(_expectInvalid(_driver, "background-image", "conic-gradient(from calc(50% + 30deg), red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "conic-gradient(red calc(50% + 0), blue)"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-images/parsing/gradient-interpolation-method-invalid.html
test$("vaev-style-wpt-css-css-images-parsing-gradient-interpolation-method-invalid") {
    try$(_expectInvalid(_driver, "background-image", "linear-gradient(, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "linear-gradient(red, blue, lab)"));
    try$(_expectInvalid(_driver, "background-image", "linear-gradient(lab lab, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "linear-gradient(lab shorter hue, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "linear-gradient(red, blue, oklab)"));
    try$(_expectInvalid(_driver, "background-image", "linear-gradient(oklab oklab, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "linear-gradient(oklab shorter hue, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "linear-gradient(red, blue, srgb)"));
    try$(_expectInvalid(_driver, "background-image", "linear-gradient(srgb srgb, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "linear-gradient(srgb shorter hue, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "linear-gradient(red, blue, srgb-linear)"));
    try$(_expectInvalid(_driver, "background-image", "linear-gradient(srgb-linear srgb-linear, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "linear-gradient(srgb-linear shorter hue, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "linear-gradient(red, blue, xyz)"));
    try$(_expectInvalid(_driver, "background-image", "linear-gradient(xyz xyz, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "linear-gradient(xyz shorter hue, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "linear-gradient(red, blue, xyz-d50)"));
    try$(_expectInvalid(_driver, "background-image", "linear-gradient(xyz-d50 xyz-d50, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "linear-gradient(xyz-d50 shorter hue, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "linear-gradient(red, blue, xyz-d65)"));
    try$(_expectInvalid(_driver, "background-image", "linear-gradient(xyz-d65 xyz-d65, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "linear-gradient(xyz-d65 shorter hue, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "linear-gradient(hsl foo hue, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "linear-gradient(hsl hue, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "linear-gradient(hsl hsl, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "linear-gradient(hsl shorter, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "linear-gradient(shorter hue hsl, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "linear-gradient(red, blue, hsl shorter hue)"));
    try$(_expectInvalid(_driver, "background-image", "linear-gradient(hsl longer, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "linear-gradient(longer hue hsl, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "linear-gradient(red, blue, hsl longer hue)"));
    try$(_expectInvalid(_driver, "background-image", "linear-gradient(hsl increasing, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "linear-gradient(increasing hue hsl, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "linear-gradient(red, blue, hsl increasing hue)"));
    try$(_expectInvalid(_driver, "background-image", "linear-gradient(hsl decreasing, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "linear-gradient(decreasing hue hsl, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "linear-gradient(red, blue, hsl decreasing hue)"));
    try$(_expectInvalid(_driver, "background-image", "linear-gradient(hsl specified, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "linear-gradient(specified hue hsl, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "linear-gradient(red, blue, hsl specified hue)"));
    try$(_expectInvalid(_driver, "background-image", "linear-gradient(hwb foo hue, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "linear-gradient(hwb hue, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "linear-gradient(hwb hwb, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "linear-gradient(hwb shorter, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "linear-gradient(shorter hue hwb, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "linear-gradient(red, blue, hwb shorter hue)"));
    try$(_expectInvalid(_driver, "background-image", "linear-gradient(hwb longer, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "linear-gradient(longer hue hwb, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "linear-gradient(red, blue, hwb longer hue)"));
    try$(_expectInvalid(_driver, "background-image", "linear-gradient(hwb increasing, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "linear-gradient(increasing hue hwb, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "linear-gradient(red, blue, hwb increasing hue)"));
    try$(_expectInvalid(_driver, "background-image", "linear-gradient(hwb decreasing, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "linear-gradient(decreasing hue hwb, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "linear-gradient(red, blue, hwb decreasing hue)"));
    try$(_expectInvalid(_driver, "background-image", "linear-gradient(hwb specified, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "linear-gradient(specified hue hwb, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "linear-gradient(red, blue, hwb specified hue)"));
    try$(_expectInvalid(_driver, "background-image", "linear-gradient(lch foo hue, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "linear-gradient(lch hue, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "linear-gradient(lch lch, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "linear-gradient(lch shorter, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "linear-gradient(shorter hue lch, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "linear-gradient(red, blue, lch shorter hue)"));
    try$(_expectInvalid(_driver, "background-image", "linear-gradient(lch longer, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "linear-gradient(longer hue lch, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "linear-gradient(red, blue, lch longer hue)"));
    try$(_expectInvalid(_driver, "background-image", "linear-gradient(lch increasing, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "linear-gradient(increasing hue lch, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "linear-gradient(red, blue, lch increasing hue)"));
    try$(_expectInvalid(_driver, "background-image", "linear-gradient(lch decreasing, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "linear-gradient(decreasing hue lch, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "linear-gradient(red, blue, lch decreasing hue)"));
    try$(_expectInvalid(_driver, "background-image", "linear-gradient(lch specified, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "linear-gradient(specified hue lch, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "linear-gradient(red, blue, lch specified hue)"));
    try$(_expectInvalid(_driver, "background-image", "linear-gradient(oklch foo hue, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "linear-gradient(oklch hue, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "linear-gradient(oklch oklch, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "linear-gradient(oklch shorter, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "linear-gradient(shorter hue oklch, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "linear-gradient(red, blue, oklch shorter hue)"));
    try$(_expectInvalid(_driver, "background-image", "linear-gradient(oklch longer, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "linear-gradient(longer hue oklch, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "linear-gradient(red, blue, oklch longer hue)"));
    try$(_expectInvalid(_driver, "background-image", "linear-gradient(oklch increasing, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "linear-gradient(increasing hue oklch, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "linear-gradient(red, blue, oklch increasing hue)"));
    try$(_expectInvalid(_driver, "background-image", "linear-gradient(oklch decreasing, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "linear-gradient(decreasing hue oklch, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "linear-gradient(red, blue, oklch decreasing hue)"));
    try$(_expectInvalid(_driver, "background-image", "linear-gradient(oklch specified, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "linear-gradient(specified hue oklch, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "linear-gradient(red, blue, oklch specified hue)"));
    try$(_expectInvalid(_driver, "background-image", "radial-gradient(, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "radial-gradient(red, blue, lab)"));
    try$(_expectInvalid(_driver, "background-image", "radial-gradient(lab lab, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "radial-gradient(lab shorter hue, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "radial-gradient(red, blue, oklab)"));
    try$(_expectInvalid(_driver, "background-image", "radial-gradient(oklab oklab, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "radial-gradient(oklab shorter hue, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "radial-gradient(red, blue, srgb)"));
    try$(_expectInvalid(_driver, "background-image", "radial-gradient(srgb srgb, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "radial-gradient(srgb shorter hue, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "radial-gradient(red, blue, srgb-linear)"));
    try$(_expectInvalid(_driver, "background-image", "radial-gradient(srgb-linear srgb-linear, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "radial-gradient(srgb-linear shorter hue, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "radial-gradient(red, blue, xyz)"));
    try$(_expectInvalid(_driver, "background-image", "radial-gradient(xyz xyz, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "radial-gradient(xyz shorter hue, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "radial-gradient(red, blue, xyz-d50)"));
    try$(_expectInvalid(_driver, "background-image", "radial-gradient(xyz-d50 xyz-d50, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "radial-gradient(xyz-d50 shorter hue, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "radial-gradient(red, blue, xyz-d65)"));
    try$(_expectInvalid(_driver, "background-image", "radial-gradient(xyz-d65 xyz-d65, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "radial-gradient(xyz-d65 shorter hue, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "radial-gradient(hsl foo hue, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "radial-gradient(hsl hue, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "radial-gradient(hsl hsl, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "radial-gradient(hsl shorter, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "radial-gradient(shorter hue hsl, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "radial-gradient(red, blue, hsl shorter hue)"));
    try$(_expectInvalid(_driver, "background-image", "radial-gradient(hsl longer, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "radial-gradient(longer hue hsl, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "radial-gradient(red, blue, hsl longer hue)"));
    try$(_expectInvalid(_driver, "background-image", "radial-gradient(hsl increasing, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "radial-gradient(increasing hue hsl, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "radial-gradient(red, blue, hsl increasing hue)"));
    try$(_expectInvalid(_driver, "background-image", "radial-gradient(hsl decreasing, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "radial-gradient(decreasing hue hsl, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "radial-gradient(red, blue, hsl decreasing hue)"));
    try$(_expectInvalid(_driver, "background-image", "radial-gradient(hsl specified, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "radial-gradient(specified hue hsl, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "radial-gradient(red, blue, hsl specified hue)"));
    try$(_expectInvalid(_driver, "background-image", "radial-gradient(hwb foo hue, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "radial-gradient(hwb hue, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "radial-gradient(hwb hwb, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "radial-gradient(hwb shorter, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "radial-gradient(shorter hue hwb, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "radial-gradient(red, blue, hwb shorter hue)"));
    try$(_expectInvalid(_driver, "background-image", "radial-gradient(hwb longer, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "radial-gradient(longer hue hwb, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "radial-gradient(red, blue, hwb longer hue)"));
    try$(_expectInvalid(_driver, "background-image", "radial-gradient(hwb increasing, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "radial-gradient(increasing hue hwb, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "radial-gradient(red, blue, hwb increasing hue)"));
    try$(_expectInvalid(_driver, "background-image", "radial-gradient(hwb decreasing, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "radial-gradient(decreasing hue hwb, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "radial-gradient(red, blue, hwb decreasing hue)"));
    try$(_expectInvalid(_driver, "background-image", "radial-gradient(hwb specified, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "radial-gradient(specified hue hwb, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "radial-gradient(red, blue, hwb specified hue)"));
    try$(_expectInvalid(_driver, "background-image", "radial-gradient(lch foo hue, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "radial-gradient(lch hue, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "radial-gradient(lch lch, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "radial-gradient(lch shorter, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "radial-gradient(shorter hue lch, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "radial-gradient(red, blue, lch shorter hue)"));
    try$(_expectInvalid(_driver, "background-image", "radial-gradient(lch longer, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "radial-gradient(longer hue lch, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "radial-gradient(red, blue, lch longer hue)"));
    try$(_expectInvalid(_driver, "background-image", "radial-gradient(lch increasing, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "radial-gradient(increasing hue lch, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "radial-gradient(red, blue, lch increasing hue)"));
    try$(_expectInvalid(_driver, "background-image", "radial-gradient(lch decreasing, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "radial-gradient(decreasing hue lch, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "radial-gradient(red, blue, lch decreasing hue)"));
    try$(_expectInvalid(_driver, "background-image", "radial-gradient(lch specified, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "radial-gradient(specified hue lch, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "radial-gradient(red, blue, lch specified hue)"));
    try$(_expectInvalid(_driver, "background-image", "radial-gradient(oklch foo hue, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "radial-gradient(oklch hue, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "radial-gradient(oklch oklch, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "radial-gradient(oklch shorter, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "radial-gradient(shorter hue oklch, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "radial-gradient(red, blue, oklch shorter hue)"));
    try$(_expectInvalid(_driver, "background-image", "radial-gradient(oklch longer, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "radial-gradient(longer hue oklch, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "radial-gradient(red, blue, oklch longer hue)"));
    try$(_expectInvalid(_driver, "background-image", "radial-gradient(oklch increasing, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "radial-gradient(increasing hue oklch, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "radial-gradient(red, blue, oklch increasing hue)"));
    try$(_expectInvalid(_driver, "background-image", "radial-gradient(oklch decreasing, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "radial-gradient(decreasing hue oklch, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "radial-gradient(red, blue, oklch decreasing hue)"));
    try$(_expectInvalid(_driver, "background-image", "radial-gradient(oklch specified, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "radial-gradient(specified hue oklch, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "radial-gradient(red, blue, oklch specified hue)"));
    try$(_expectInvalid(_driver, "background-image", "conic-gradient(, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "conic-gradient(red, blue, lab)"));
    try$(_expectInvalid(_driver, "background-image", "conic-gradient(lab lab, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "conic-gradient(lab shorter hue, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "conic-gradient(red, blue, oklab)"));
    try$(_expectInvalid(_driver, "background-image", "conic-gradient(oklab oklab, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "conic-gradient(oklab shorter hue, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "conic-gradient(red, blue, srgb)"));
    try$(_expectInvalid(_driver, "background-image", "conic-gradient(srgb srgb, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "conic-gradient(srgb shorter hue, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "conic-gradient(red, blue, srgb-linear)"));
    try$(_expectInvalid(_driver, "background-image", "conic-gradient(srgb-linear srgb-linear, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "conic-gradient(srgb-linear shorter hue, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "conic-gradient(red, blue, xyz)"));
    try$(_expectInvalid(_driver, "background-image", "conic-gradient(xyz xyz, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "conic-gradient(xyz shorter hue, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "conic-gradient(red, blue, xyz-d50)"));
    try$(_expectInvalid(_driver, "background-image", "conic-gradient(xyz-d50 xyz-d50, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "conic-gradient(xyz-d50 shorter hue, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "conic-gradient(red, blue, xyz-d65)"));
    try$(_expectInvalid(_driver, "background-image", "conic-gradient(xyz-d65 xyz-d65, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "conic-gradient(xyz-d65 shorter hue, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "conic-gradient(hsl foo hue, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "conic-gradient(hsl hue, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "conic-gradient(hsl hsl, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "conic-gradient(hsl shorter, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "conic-gradient(shorter hue hsl, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "conic-gradient(red, blue, hsl shorter hue)"));
    try$(_expectInvalid(_driver, "background-image", "conic-gradient(hsl longer, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "conic-gradient(longer hue hsl, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "conic-gradient(red, blue, hsl longer hue)"));
    try$(_expectInvalid(_driver, "background-image", "conic-gradient(hsl increasing, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "conic-gradient(increasing hue hsl, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "conic-gradient(red, blue, hsl increasing hue)"));
    try$(_expectInvalid(_driver, "background-image", "conic-gradient(hsl decreasing, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "conic-gradient(decreasing hue hsl, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "conic-gradient(red, blue, hsl decreasing hue)"));
    try$(_expectInvalid(_driver, "background-image", "conic-gradient(hsl specified, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "conic-gradient(specified hue hsl, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "conic-gradient(red, blue, hsl specified hue)"));
    try$(_expectInvalid(_driver, "background-image", "conic-gradient(hwb foo hue, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "conic-gradient(hwb hue, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "conic-gradient(hwb hwb, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "conic-gradient(hwb shorter, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "conic-gradient(shorter hue hwb, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "conic-gradient(red, blue, hwb shorter hue)"));
    try$(_expectInvalid(_driver, "background-image", "conic-gradient(hwb longer, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "conic-gradient(longer hue hwb, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "conic-gradient(red, blue, hwb longer hue)"));
    try$(_expectInvalid(_driver, "background-image", "conic-gradient(hwb increasing, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "conic-gradient(increasing hue hwb, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "conic-gradient(red, blue, hwb increasing hue)"));
    try$(_expectInvalid(_driver, "background-image", "conic-gradient(hwb decreasing, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "conic-gradient(decreasing hue hwb, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "conic-gradient(red, blue, hwb decreasing hue)"));
    try$(_expectInvalid(_driver, "background-image", "conic-gradient(hwb specified, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "conic-gradient(specified hue hwb, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "conic-gradient(red, blue, hwb specified hue)"));
    try$(_expectInvalid(_driver, "background-image", "conic-gradient(lch foo hue, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "conic-gradient(lch hue, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "conic-gradient(lch lch, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "conic-gradient(lch shorter, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "conic-gradient(shorter hue lch, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "conic-gradient(red, blue, lch shorter hue)"));
    try$(_expectInvalid(_driver, "background-image", "conic-gradient(lch longer, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "conic-gradient(longer hue lch, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "conic-gradient(red, blue, lch longer hue)"));
    try$(_expectInvalid(_driver, "background-image", "conic-gradient(lch increasing, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "conic-gradient(increasing hue lch, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "conic-gradient(red, blue, lch increasing hue)"));
    try$(_expectInvalid(_driver, "background-image", "conic-gradient(lch decreasing, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "conic-gradient(decreasing hue lch, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "conic-gradient(red, blue, lch decreasing hue)"));
    try$(_expectInvalid(_driver, "background-image", "conic-gradient(lch specified, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "conic-gradient(specified hue lch, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "conic-gradient(red, blue, lch specified hue)"));
    try$(_expectInvalid(_driver, "background-image", "conic-gradient(oklch foo hue, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "conic-gradient(oklch hue, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "conic-gradient(oklch oklch, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "conic-gradient(oklch shorter, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "conic-gradient(shorter hue oklch, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "conic-gradient(red, blue, oklch shorter hue)"));
    try$(_expectInvalid(_driver, "background-image", "conic-gradient(oklch longer, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "conic-gradient(longer hue oklch, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "conic-gradient(red, blue, oklch longer hue)"));
    try$(_expectInvalid(_driver, "background-image", "conic-gradient(oklch increasing, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "conic-gradient(increasing hue oklch, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "conic-gradient(red, blue, oklch increasing hue)"));
    try$(_expectInvalid(_driver, "background-image", "conic-gradient(oklch decreasing, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "conic-gradient(decreasing hue oklch, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "conic-gradient(red, blue, oklch decreasing hue)"));
    try$(_expectInvalid(_driver, "background-image", "conic-gradient(oklch specified, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "conic-gradient(specified hue oklch, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "conic-gradient(red, blue, oklch specified hue)"));
    try$(_expectInvalid(_driver, "background-image", "linear-gradient(in, black, transparent)"));
    try$(_expectInvalid(_driver, "background-image", "linear-gradient(45deg in, black, transparent)"));
    try$(_expectInvalid(_driver, "background-image", "linear-gradient(in 45deg, black, transparent)"));
    try$(_expectInvalid(_driver, "background-image", "linear-gradient(in to right, black, transparent)"));
    try$(_expectInvalid(_driver, "background-image", "radial-gradient(closest-side in, black, transparent)"));
    try$(_expectInvalid(_driver, "background-image", "conic-gradient(in from 90deg, black, transparent)"));
    try$(_expectInvalid(_driver, "background-image", "linear-gradient(90deg in hsl longer, black, transparent)"));
    try$(_expectInvalid(_driver, "background-image", "linear-gradient(90deg in hsl shorter, black)"));
    try$(_expectInvalid(_driver, "background-image", "linear-gradient(to right in hsl increasing, black, transparent)"));
    try$(_expectInvalid(_driver, "background-image", "linear-gradient(to right in hsl decreasing, black, transparent)"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-images/parsing/gradient-position-invalid.html
test$("vaev-style-wpt-css-css-images-parsing-gradient-position-invalid") {
    try$(_expectInvalid(_driver, "background-image", "radial-gradient(at top 0px, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "radial-gradient(at center left 1px, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "radial-gradient(at center top 2px, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "radial-gradient(at right 3% center, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "radial-gradient(at left 4px top, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "radial-gradient(at right top 5px, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "radial-gradient(at bottom 6% center, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "radial-gradient(at bottom 7% left, red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "radial-gradient(at bottom right 8%, red, blue)"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-images/parsing/image-function-invalid.html
test$("vaev-style-wpt-css-css-images-parsing-image-function-invalid") {
    try$(_expectInvalid(_driver, "background-image", "image()"));
    try$(_expectInvalid(_driver, "background-image", "image(none)"));
    try$(_expectInvalid(_driver, "background-image", "image(red, blue)"));
    try$(_expectInvalid(_driver, "background-image", "image(notacolor)"));
    try$(_expectInvalid(_driver, "background-image", "image(url(foo.png))"));
    try$(_expectInvalid(_driver, "background-color", "image(red)"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-inline/baseline-source/baseline-source-invalid.html
test$("vaev-style-wpt-css-css-inline-baseline-source-baseline-source-invalid") {
    try$(_expectInvalid(_driver, "baseline-source", "foobar"));
    try$(_expectInvalid(_driver, "baseline-source", "default"));
    try$(_expectInvalid(_driver, "baseline-source", "auto first"));
    try$(_expectInvalid(_driver, "baseline-source", "first auto"));
    try$(_expectInvalid(_driver, "baseline-source", "3px"));
    try$(_expectInvalid(_driver, "baseline-source", "10%"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-inline/baseline-source/baseline-source-valid.html
test$("vaev-style-wpt-css-css-inline-baseline-source-baseline-source-valid") {
    try$(_expectSerialized(_driver, "baseline-source", "auto", "auto"));
    try$(_expectSerialized(_driver, "baseline-source", "first", "first"));
    try$(_expectSerialized(_driver, "baseline-source", "last", "last"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-inline/parsing/alignment-baseline-invalid.html
test$("vaev-style-wpt-css-css-inline-parsing-alignment-baseline-invalid") {
    try$(_expectInvalid(_driver, "alignment-baseline", "auto"));
    try$(_expectInvalid(_driver, "alignment-baseline", "none"));
    try$(_expectInvalid(_driver, "alignment-baseline", "baseline text-bottom"));
    try$(_expectInvalid(_driver, "alignment-baseline", "top"));
    try$(_expectInvalid(_driver, "alignment-baseline", "center"));
    try$(_expectInvalid(_driver, "alignment-baseline", "bottom"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-inline/parsing/alignment-baseline-valid.html
test$("vaev-style-wpt-css-css-inline-parsing-alignment-baseline-valid") {
    try$(_expectSerialized(_driver, "alignment-baseline", "baseline", "baseline"));
    try$(_expectSerialized(_driver, "alignment-baseline", "text-bottom", "text-bottom"));
    try$(_expectSerialized(_driver, "alignment-baseline", "alphabetic", "alphabetic"));
    try$(_expectSerialized(_driver, "alignment-baseline", "ideographic", "ideographic"));
    try$(_expectSerialized(_driver, "alignment-baseline", "middle", "middle"));
    try$(_expectSerialized(_driver, "alignment-baseline", "central", "central"));
    try$(_expectSerialized(_driver, "alignment-baseline", "mathematical", "mathematical"));
    try$(_expectSerialized(_driver, "alignment-baseline", "text-top", "text-top"));
    return Ok();
}

test$("vaev-style-wpt-css-css-inline-parsing-alignment-baseline-valid-xfail", XFAIL) {
    try$(_expectSerialized(_driver, "alignment-baseline", "hanging", "hanging"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-inline/parsing/baseline-shift-invalid.html
test$("vaev-style-wpt-css-css-inline-parsing-baseline-shift-invalid") {
    try$(_expectInvalid(_driver, "baseline-shift", "5"));
    try$(_expectInvalid(_driver, "baseline-shift", "-10px 20%"));
    try$(_expectInvalid(_driver, "baseline-shift", "calc(2em + 3ex), 0"));
    try$(_expectInvalid(_driver, "baseline-shift", "sub super"));
    try$(_expectInvalid(_driver, "baseline-shift", "20px sub"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-inline/parsing/baseline-shift-valid.html
test$("vaev-style-wpt-css-css-inline-parsing-baseline-shift-valid") {
    try$(_expectSerialized(_driver, "baseline-shift", "-10px", "-10px"));
    try$(_expectSerialized(_driver, "baseline-shift", "20%", "20%"));
    try$(_expectSerialized(_driver, "baseline-shift", "calc(2em + 3ex)", "calc(2em + 3ex)"));
    try$(_expectSerialized(_driver, "baseline-shift", "0", "0px"));
    try$(_expectSerialized(_driver, "baseline-shift", "sub", "sub"));
    try$(_expectSerialized(_driver, "baseline-shift", "super", "super"));
    try$(_expectSerialized(_driver, "baseline-shift", "bottom", "bottom"));
    try$(_expectSerialized(_driver, "baseline-shift", "center", "center"));
    try$(_expectSerialized(_driver, "baseline-shift", "top", "top"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-inline/parsing/dominant-baseline-invalid.html
test$("vaev-style-wpt-css-css-inline-parsing-dominant-baseline-invalid") {
    try$(_expectInvalid(_driver, "dominant-baseline", "normal"));
    try$(_expectInvalid(_driver, "dominant-baseline", "none"));
    try$(_expectInvalid(_driver, "dominant-baseline", "alphabetic, ideographic"));
    try$(_expectInvalid(_driver, "dominant-baseline", "middle central"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-inline/parsing/dominant-baseline-valid.html
test$("vaev-style-wpt-css-css-inline-parsing-dominant-baseline-valid") {
    try$(_expectSerialized(_driver, "dominant-baseline", "auto", "auto"));
    try$(_expectSerialized(_driver, "dominant-baseline", "text-bottom", "text-bottom"));
    try$(_expectSerialized(_driver, "dominant-baseline", "alphabetic", "alphabetic"));
    try$(_expectSerialized(_driver, "dominant-baseline", "ideographic", "ideographic"));
    try$(_expectSerialized(_driver, "dominant-baseline", "middle", "middle"));
    try$(_expectSerialized(_driver, "dominant-baseline", "central", "central"));
    try$(_expectSerialized(_driver, "dominant-baseline", "mathematical", "mathematical"));
    try$(_expectSerialized(_driver, "dominant-baseline", "text-top", "text-top"));
    return Ok();
}

test$("vaev-style-wpt-css-css-inline-parsing-dominant-baseline-valid-xfail", XFAIL) {
    try$(_expectSerialized(_driver, "dominant-baseline", "hanging", "hanging"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-inline/parsing/line-height-invalid.html
test$("vaev-style-wpt-css-css-inline-parsing-line-height-invalid") {
    try$(_expectInvalid(_driver, "line-height", "auto"));
    try$(_expectInvalid(_driver, "line-height", "2 10px"));
    try$(_expectInvalid(_driver, "line-height", "200% 3"));
    try$(_expectInvalid(_driver, "line-height", "auto 10px"));
    try$(_expectInvalid(_driver, "line-height", "3 auto"));
    return Ok();
}

test$("vaev-style-wpt-css-css-inline-parsing-line-height-invalid-xfail", XFAIL) {
    try$(_expectInvalid(_driver, "line-height", "-2"));
    try$(_expectInvalid(_driver, "line-height", "-10px"));
    try$(_expectInvalid(_driver, "line-height", "-200%"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-inline/parsing/line-height-valid.html
test$("vaev-style-wpt-css-css-inline-parsing-line-height-valid") {
    try$(_expectSerialized(_driver, "line-height", "normal", "normal"));
    try$(_expectSerialized(_driver, "line-height", "0", "0"));
    try$(_expectSerialized(_driver, "line-height", "2", "2"));
    try$(_expectSerialized(_driver, "line-height", "0px", "0px"));
    try$(_expectSerialized(_driver, "line-height", "10px", "10px"));
    try$(_expectSerialized(_driver, "line-height", "0%", "0%"));
    try$(_expectSerialized(_driver, "line-height", "200%", "200%"));
    try$(_expectSerialized(_driver, "line-height", "calc(200% + 10px)", "calc(200% + 10px)"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-inline/parsing/vertical-align-invalid.html
test$("vaev-style-wpt-css-css-inline-parsing-vertical-align-invalid") {
    try$(_expectInvalid(_driver, "vertical-align", "none"));
    try$(_expectInvalid(_driver, "vertical-align", "first last"));
    try$(_expectInvalid(_driver, "vertical-align", "first, last"));
    try$(_expectInvalid(_driver, "vertical-align", "sub super"));
    try$(_expectInvalid(_driver, "vertical-align", "super 10px"));
    try$(_expectInvalid(_driver, "vertical-align", "20% sub"));
    try$(_expectInvalid(_driver, "vertical-align", "baseline middle"));
    try$(_expectInvalid(_driver, "vertical-align", "text-top, bottom"));
    try$(_expectInvalid(_driver, "vertical-align", "auto"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-lists/parsing/content-invalid.html
test$("vaev-style-wpt-css-css-lists-parsing-content-invalid") {
    try$(_expectInvalid(_driver, "content", "counters(foo, \"\", none)"));
    try$(_expectInvalid(_driver, "content", "counters(foo, \"\", unset)"));
    try$(_expectInvalid(_driver, "content", "counters(foo, \"\", initial)"));
    try$(_expectInvalid(_driver, "content", "counters(foo, \"\", inherit)"));
    try$(_expectInvalid(_driver, "content", "\"z\" counter(foo, none)"));
    try$(_expectInvalid(_driver, "content", "\"z\" counters(foo, \"\", none)"));
    try$(_expectInvalid(_driver, "content", "\"z\" counter(foo, unset)"));
    try$(_expectInvalid(_driver, "content", "\"z\" counters(foo, \"\", unset)"));
    try$(_expectInvalid(_driver, "content", "\"z\" counter(foo, initial)"));
    try$(_expectInvalid(_driver, "content", "\"z\" counters(foo, \"\", initial)"));
    try$(_expectInvalid(_driver, "content", "\"z\" counter(foo, inherit)"));
    try$(_expectInvalid(_driver, "content", "\"z\" counters(foo, \"\", inherit)"));
    try$(_expectInvalid(_driver, "content", "counter(foo, none) \"z\""));
    try$(_expectInvalid(_driver, "content", "counters(foo, \"\", none) \"z\""));
    try$(_expectInvalid(_driver, "content", "counter(foo, unset) \"z\""));
    try$(_expectInvalid(_driver, "content", "counters(foo, \"\", unset) \"z\""));
    try$(_expectInvalid(_driver, "content", "counter(foo, initial) \"z\""));
    try$(_expectInvalid(_driver, "content", "counters(foo, \"\", initial) \"z\""));
    try$(_expectInvalid(_driver, "content", "counter(foo, inherit) \"z\""));
    try$(_expectInvalid(_driver, "content", "counters(foo, \"\", inherit) \"z\""));
    return Ok();
}

test$("vaev-style-wpt-css-css-lists-parsing-content-invalid-xfail", XFAIL) {
    try$(_expectInvalid(_driver, "content", "counter(foo, none)"));
    try$(_expectInvalid(_driver, "content", "counter(foo, unset)"));
    try$(_expectInvalid(_driver, "content", "counter(foo, initial)"));
    try$(_expectInvalid(_driver, "content", "counter(foo, inherit)"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-lists/parsing/counter-increment-invalid.html
test$("vaev-style-wpt-css-css-lists-parsing-counter-increment-invalid") {
    try$(_expectInvalid(_driver, "counter-increment", "none chapter"));
    try$(_expectInvalid(_driver, "counter-increment", "reversed(none)"));
    try$(_expectInvalid(_driver, "counter-increment", "reversed(chapter)"));
    try$(_expectInvalid(_driver, "counter-increment", "3"));
    try$(_expectInvalid(_driver, "counter-increment", "99 imagenum"));
    try$(_expectInvalid(_driver, "counter-increment", "section -1, imagenum 99"));
    return Ok();
}

test$("vaev-style-wpt-css-css-lists-parsing-counter-increment-invalid-xfail", XFAIL) {
    try$(_expectInvalid(_driver, "counter-increment", "section 3.14"));
    try$(_expectInvalid(_driver, "counter-increment", "inherit 1"));
    try$(_expectInvalid(_driver, "counter-increment", "initial 1"));
    try$(_expectInvalid(_driver, "counter-increment", "unset 1"));
    try$(_expectInvalid(_driver, "counter-increment", "default 1"));
    try$(_expectInvalid(_driver, "counter-increment", "revert 1"));
    try$(_expectInvalid(_driver, "counter-increment", "revert-layer 1"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-lists/parsing/counter-reset-invalid.html
test$("vaev-style-wpt-css-css-lists-parsing-counter-reset-invalid") {
    try$(_expectInvalid(_driver, "counter-reset", "none chapter"));
    try$(_expectInvalid(_driver, "counter-reset", "reversed(none)"));
    try$(_expectInvalid(_driver, "counter-reset", "reversed(chapter) none"));
    try$(_expectInvalid(_driver, "counter-reset", "3"));
    try$(_expectInvalid(_driver, "counter-reset", "reversed(3)"));
    try$(_expectInvalid(_driver, "counter-reset", "99 imagenum"));
    try$(_expectInvalid(_driver, "counter-reset", "99 reversed(imagenum)"));
    try$(_expectInvalid(_driver, "counter-reset", "section -1, imagenum 99"));
    return Ok();
}

test$("vaev-style-wpt-css-css-lists-parsing-counter-reset-invalid-xfail", XFAIL) {
    try$(_expectInvalid(_driver, "counter-reset", "section 3.14"));
    try$(_expectInvalid(_driver, "counter-reset", "inherit 0"));
    try$(_expectInvalid(_driver, "counter-reset", "initial 0"));
    try$(_expectInvalid(_driver, "counter-reset", "unset 0"));
    try$(_expectInvalid(_driver, "counter-reset", "default 0"));
    try$(_expectInvalid(_driver, "counter-reset", "revert 0"));
    try$(_expectInvalid(_driver, "counter-reset", "revert-layer 0"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-lists/parsing/counter-set-invalid.html
test$("vaev-style-wpt-css-css-lists-parsing-counter-set-invalid") {
    try$(_expectInvalid(_driver, "counter-set", "none chapter"));
    try$(_expectInvalid(_driver, "counter-set", "reversed(none)"));
    try$(_expectInvalid(_driver, "counter-set", "reversed(chapter)"));
    try$(_expectInvalid(_driver, "counter-set", "3"));
    try$(_expectInvalid(_driver, "counter-set", "99 imagenum"));
    try$(_expectInvalid(_driver, "counter-set", "section -1, imagenum 99"));
    return Ok();
}

test$("vaev-style-wpt-css-css-lists-parsing-counter-set-invalid-xfail", XFAIL) {
    try$(_expectInvalid(_driver, "counter-set", "section 3.14"));
    try$(_expectInvalid(_driver, "counter-set", "inherit 2"));
    try$(_expectInvalid(_driver, "counter-set", "initial 2"));
    try$(_expectInvalid(_driver, "counter-set", "unset 2"));
    try$(_expectInvalid(_driver, "counter-set", "default 2"));
    try$(_expectInvalid(_driver, "counter-set", "revert 2"));
    try$(_expectInvalid(_driver, "counter-set", "revert-layer 2"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-lists/parsing/list-style-image-invalid.html
test$("vaev-style-wpt-css-css-lists-parsing-list-style-image-invalid") {
    try$(_expectInvalid(_driver, "list-style-image", "auto"));
    try$(_expectInvalid(_driver, "list-style-image", "url(\"https://example.com/\") none"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-lists/parsing/list-style-invalid.html
test$("vaev-style-wpt-css-css-lists-parsing-list-style-invalid") {
    try$(_expectInvalid(_driver, "list-style", "symbols(numeric \"a\")"));
    try$(_expectInvalid(_driver, "list-style", "symbols(fixed)"));
    return Ok();
}

test$("vaev-style-wpt-css-css-lists-parsing-list-style-invalid-xfail", XFAIL) {
    try$(_expectInvalid(_driver, "list-style", "inside disc outside"));
    try$(_expectInvalid(_driver, "list-style", "square circle"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-lists/parsing/list-style-position-invalid.html
test$("vaev-style-wpt-css-css-lists-parsing-list-style-position-invalid") {
    try$(_expectInvalid(_driver, "list-style-position", "auto"));
    try$(_expectInvalid(_driver, "list-style-position", "inside outside"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-lists/parsing/list-style-position-valid.html
test$("vaev-style-wpt-css-css-lists-parsing-list-style-position-valid") {
    try$(_expectSerialized(_driver, "list-style-position", "inside", "inside"));
    try$(_expectSerialized(_driver, "list-style-position", "outside", "outside"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-lists/parsing/list-style-type-invalid.html
test$("vaev-style-wpt-css-css-lists-parsing-list-style-type-invalid") {
    try$(_expectInvalid(_driver, "list-style-type", "\"marker string\" none"));
    try$(_expectInvalid(_driver, "list-style-type", "counter-Style-Name \"marker string\""));
    try$(_expectInvalid(_driver, "list-style-type", "symbols(cyclic)"));
    try$(_expectInvalid(_driver, "list-style-type", "symbols(numeric \"n\")"));
    try$(_expectInvalid(_driver, "list-style-type", "symbols(alphabetic \"a\")"));
    try$(_expectInvalid(_driver, "list-style-type", "symbols(\"s\" symbolic)"));
    try$(_expectInvalid(_driver, "list-style-type", "symbols(fixed url(\"https://example.com\"))"));
    try$(_expectInvalid(_driver, "list-style-type", "symbols(\"a\") extra"));
    try$(_expectInvalid(_driver, "list-style-type", "symbols(cyclic \"a\") disc"));
    try$(_expectInvalid(_driver, "list-style-type", "symbols(repeating '○' '●')"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-logical/parsing/margin-block-inline-invalid.html
test$("vaev-style-wpt-css-css-logical-parsing-margin-block-inline-invalid") {
    try$(_expectInvalid(_driver, "margin-block-start", "none"));
    try$(_expectInvalid(_driver, "margin-block-end", "10"));
    try$(_expectInvalid(_driver, "margin-inline-start", "20% calc(10px - 0.5em)"));
    try$(_expectInvalid(_driver, "margin-inline-end", "10px, auto"));
    try$(_expectInvalid(_driver, "margin-block", "none"));
    try$(_expectInvalid(_driver, "margin-block", "20%, calc(10px - 0.5em)"));
    return Ok();
}

test$("vaev-style-wpt-css-css-logical-parsing-margin-block-inline-invalid-xfail", XFAIL) {
    try$(_expectInvalid(_driver, "margin-inline", "10px auto 20px"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-logical/parsing/margin-block-inline-valid.html
test$("vaev-style-wpt-css-css-logical-parsing-margin-block-inline-valid") {
    try$(_expectSerialized(_driver, "margin-block-start", "calc(20% + 10px)", "calc(20% + 10px)"));
    try$(_expectSerialized(_driver, "margin-block-start", "auto", "auto"));
    try$(_expectSerialized(_driver, "margin-block-end", "-10px", "-10px"));
    try$(_expectSerialized(_driver, "margin-inline-start", "-20%", "-20%"));
    try$(_expectSerialized(_driver, "margin-inline-end", "calc(2em + 3ex)", "calc(2em + 3ex)"));
    return Ok();
}

test$("vaev-style-wpt-css-css-logical-parsing-margin-block-inline-valid-xfail", XFAIL) {
    try$(_expectSerialized(_driver, "margin-block", "auto", "auto"));
    try$(_expectSerialized(_driver, "margin-block", "-10px", "-10px"));
    try$(_expectSerialized(_driver, "margin-block", "calc(2em + 3ex) -20%", "calc(2em + 3ex) -20%"));
    try$(_expectSerialized(_driver, "margin-block", "auto auto", "auto"));
    try$(_expectSerialized(_driver, "margin-block", "-20% calc(20% + 10px)", "-20% calc(20% + 10px)"));
    try$(_expectSerialized(_driver, "margin-inline", "20%", "20%"));
    try$(_expectSerialized(_driver, "margin-inline", "calc(2em + 3ex)", "calc(2em + 3ex)"));
    try$(_expectSerialized(_driver, "margin-inline", "-10px auto", "-10px auto"));
    try$(_expectSerialized(_driver, "margin-inline", "auto calc(2em + 3ex)", "auto calc(2em + 3ex)"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-logical/parsing/padding-block-inline-invalid.html
test$("vaev-style-wpt-css-css-logical-parsing-padding-block-inline-invalid") {
    try$(_expectInvalid(_driver, "padding-inline-start", "20% calc(10px - 0.5em)"));
    try$(_expectInvalid(_driver, "padding-inline-start", "2px auto"));
    try$(_expectInvalid(_driver, "padding-inline-end", "10px, auto"));
    return Ok();
}

test$("vaev-style-wpt-css-css-logical-parsing-padding-block-inline-invalid-xfail", XFAIL) {
    try$(_expectInvalid(_driver, "padding-inline-end", "-20%"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-logical/parsing/padding-block-inline-valid.html
test$("vaev-style-wpt-css-css-logical-parsing-padding-block-inline-valid") {
    try$(_expectSerialized(_driver, "padding-inline-start", "20%", "20%"));
    try$(_expectSerialized(_driver, "padding-inline-end", "calc(2em + 2ex)", "calc(2em + 2ex)"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-masking/parsing/clip-path-invalid.html
test$("vaev-style-wpt-css-css-masking-parsing-clip-path-invalid") {
    try$(_expectInvalid(_driver, "clip-path", "auto"));
    try$(_expectInvalid(_driver, "clip-path", "ray(0deg)"));
    try$(_expectInvalid(_driver, "clip-path", "inset()"));
    try$(_expectInvalid(_driver, "clip-path", "inset(123)"));
    try$(_expectInvalid(_driver, "clip-path", "inset(1% 2% 3% 4% 5%)"));
    try$(_expectInvalid(_driver, "clip-path", "inset(round 0)"));
    try$(_expectInvalid(_driver, "clip-path", "inset(0px round 123)"));
    try$(_expectInvalid(_driver, "clip-path", "inset(0px round 1% 2% 3% 4% 5%)"));
    try$(_expectInvalid(_driver, "clip-path", "circle(123)"));
    try$(_expectInvalid(_driver, "clip-path", "circle(10% 20%)"));
    try$(_expectInvalid(_driver, "clip-path", "circle(1% 2% at 0% 100%)"));
    try$(_expectInvalid(_driver, "clip-path", "ellipse(farthest-side at)"));
    try$(_expectInvalid(_driver, "clip-path", "ellipse(1% 2% top right)"));
    try$(_expectInvalid(_driver, "clip-path", "ellipse(3%)"));
    try$(_expectInvalid(_driver, "clip-path", "ellipse(3% at 100% 0%)"));
    try$(_expectInvalid(_driver, "clip-path", "ellipse(closest-side)"));
    try$(_expectInvalid(_driver, "clip-path", "ellipse(farthest-side at 100% 0%)"));
    try$(_expectInvalid(_driver, "clip-path", "polygon(1%)"));
    try$(_expectInvalid(_driver, "clip-path", "polygon(round, 1px 2px)"));
    try$(_expectInvalid(_driver, "clip-path", "polygon(round -1px, 1px 2px)"));
    try$(_expectInvalid(_driver, "clip-path", "polygon(round 1%, 1px 2px)"));
    try$(_expectInvalid(_driver, "clip-path", "polygon(round 45deg, 1px 2px)"));
    try$(_expectInvalid(_driver, "clip-path", "polygon(round 1px 1px 2px)"));
    try$(_expectInvalid(_driver, "clip-path", "polygon(1px 2px, round 1px)"));
    try$(_expectInvalid(_driver, "clip-path", "polygon(round 1px round 2px, 1px 2px)"));
    try$(_expectInvalid(_driver, "clip-path", "polygon(round 1px nonzero, 1px 2px)"));
    try$(_expectInvalid(_driver, "clip-path", "unknown-box"));
    try$(_expectInvalid(_driver, "clip-path", "path(\" \")"));
    try$(_expectInvalid(_driver, "clip-path", "path(abc, \"m 20 0 h -100 z\")"));
    try$(_expectInvalid(_driver, "clip-path", "path(\"m 20 0 h -100\", nonzero)"));
    try$(_expectInvalid(_driver, "clip-path", "xywh(0px)"));
    try$(_expectInvalid(_driver, "clip-path", "xywh(0px 1%)"));
    try$(_expectInvalid(_driver, "clip-path", "xywh(0px 1% 2em)"));
    try$(_expectInvalid(_driver, "clip-path", "rect(0px)"));
    try$(_expectInvalid(_driver, "clip-path", "rect(0px 1%)"));
    try$(_expectInvalid(_driver, "clip-path", "rect(0px 1% auto)"));
    return Ok();
}

test$("vaev-style-wpt-css-css-masking-parsing-clip-path-invalid-xfail", XFAIL) {
    try$(_expectInvalid(_driver, "clip-path", "inset(0px round)"));
    try$(_expectInvalid(_driver, "clip-path", "inset(0px round / 1px)"));
    try$(_expectInvalid(_driver, "clip-path", "inset(10px round -20px)"));
    try$(_expectInvalid(_driver, "clip-path", "inset(30% round -40%)"));
    try$(_expectInvalid(_driver, "clip-path", "circle(at)"));
    try$(_expectInvalid(_driver, "clip-path", "circle(-10px at 20px 30px)"));
    try$(_expectInvalid(_driver, "clip-path", "circle(-10% at 20% 30%)"));
    try$(_expectInvalid(_driver, "clip-path", "ellipse(10% -20% at 30% 40%)"));
    try$(_expectInvalid(_driver, "clip-path", "ellipse(-50px 60px at 70% 80%)"));
    try$(_expectInvalid(_driver, "clip-path", "path(nonzero)"));
    try$(_expectInvalid(_driver, "clip-path", "path(nonzero, 'M0 0, L100 0, L0 100, L 0 0');"));
    // FIXME: clip-path: path(evenodd, "") crashes the parser (panic: empty slice), so it is not ported.
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-masking/parsing/clip-path-shape-parsing.html
test$("vaev-style-wpt-css-css-masking-parsing-clip-path-shape-parsing") {
    try$(_expectSerialized(_driver, "clip-path", "shape(from 0 0,curve to 5% calc(5% * var(--s,1)) with 5% 5%, vline to 100%)", "shape(from 0 0,curve to 5% calc(5% * var(--s,1)) with 5% 5%, vline to 100%)", RegisteredPropertySet::TOP_LEVEL));
    try$(_expectSerialized(_driver, "clip-path", "shape(from 0 0,curve to 5% var(--s,1) with 5% 5%, vline to 100%)", "shape(from 0 0,curve to 5% var(--s,1) with 5% 5%, vline to 100%)", RegisteredPropertySet::TOP_LEVEL));
    try$(_expectInvalid(_driver, "clip-path", "shape(from 10% 1rem, arc of 20% 1pt to 50px cw large rotate 25deg)"));
    try$(_expectInvalid(_driver, "clip-path", "shape(from 10% 1rem, arc cw to 50px of 20% 1pt large rotate 25deg)"));
    try$(_expectInvalid(_driver, "clip-path", "shape(from 10% 1rem, arc rotate 25deg to 50px 1pt of 20% cw large )"));
    try$(_expectInvalid(_driver, "clip-path", "shape(evenodd from 0px 0px, close path)"));
    try$(_expectInvalid(_driver, "clip-path", "shape(nonzero, from 0px 0px, line to 10px 10px)"));
    try$(_expectInvalid(_driver, "clip-path", "shape(from 10px 10px, curve to 50px 20px via 10rem)"));
    try$(_expectInvalid(_driver, "clip-path", "shape(from 10px 10px, curve to 50px 20px via 10rem 1% 12px)"));
    try$(_expectInvalid(_driver, "clip-path", "shape(from 10px 10px, hline byy 10px, vline to 5rem)"));
    try$(_expectInvalid(_driver, "clip-path", "shape(from 10px 10px, vline by 5% hline by 1vw"));
    try$(_expectInvalid(_driver, "clip-path", "shape(from 10px 10px, smooth to 50px 20px via 10rem)"));
    try$(_expectInvalid(_driver, "clip-path", "shape(from 10px 10px, smooth to 50px 20px via 10rem 2px 2pt)"));
    try$(_expectInvalid(_driver, "clip-path", "shape()"));
    try$(_expectInvalid(_driver, "clip-path", "shape(from)"));
    try$(_expectInvalid(_driver, "clip-path", "shape(from 0px)"));
    try$(_expectInvalid(_driver, "clip-path", "shape(from 0px 20px,)"));
    try$(_expectInvalid(_driver, "clip-path", "shape(close)"));
    try$(_expectInvalid(_driver, "clip-path", "shape(nonzero, close)"));
    try$(_expectInvalid(_driver, "clip-path", "shape(from 0px 10px)"));
    try$(_expectInvalid(_driver, "clip-path", "shape(allkindsofnonsense)"));
    try$(_expectInvalid(_driver, "clip-path", "shape(arc)"));
    return Ok();
}

test$("vaev-style-wpt-css-css-masking-parsing-clip-path-shape-parsing-xfail", XFAIL) {
    try$(_expectSerialized(_driver, "clip-path", "shape(from 0px 0px, line to 10px 10px)", "shape(from 0px 0px, line to 10px 10px)"));
    try$(_expectSerialized(_driver, "clip-path", "shape(evenodd from 0px 0px, line to 10px 10px)", "shape(evenodd from 0px 0px, line to 10px 10px)"));
    try$(_expectSerialized(_driver, "clip-path", "shape(nonzero from 0px 0px, line to 10px 10px)", "shape(from 0px 0px, line to 10px 10px)"));
    try$(_expectSerialized(_driver, "clip-path", "shape(  from 0px    0px, line  to 10px  10px     )", "shape(from 0px 0px, line to 10px 10px)"));
    try$(_expectSerialized(_driver, "clip-path", "shape(from 1em 50%, line to 10px 10px)", "shape(from 1em 50%, line to 10px 10px)"));
    try$(_expectSerialized(_driver, "clip-path", "shape(EvenOdd from 0px 0Px, CLOSE)", "shape(evenodd from 0px 0px, close)"));
    try$(_expectSerialized(_driver, "clip-path", "shape(from 1ch 50px, line to 10rem 10vh)", "shape(from 1ch 50px, line to 10rem 10vh)"));
    try$(_expectSerialized(_driver, "clip-path", "shape(from 1ch -50px, line to -10% 12px)", "shape(from 1ch -50px, line to -10% 12px)"));
    try$(_expectSerialized(_driver, "clip-path", "shape(from 10px 10px, move by 10px 5px, line by 20px 40%, close)", "shape(from 10px 10px, move by 10px 5px, line by 20px 40%, close)"));
    try$(_expectSerialized(_driver, "clip-path", "shape(from 10px 10px, hline by 10px, vline to 5rem)", "shape(from 10px 10px, hline by 10px, vline to 5rem)"));
    try$(_expectSerialized(_driver, "clip-path", "shape(from 10px 10px, vline by 5%, hline to 1vw)", "shape(from 10px 10px, vline by 5%, hline to 1vw)"));
    try$(_expectSerialized(_driver, "clip-path", "shape(from 10px 10px, curve to 50px 20px with 10rem 1%)", "shape(from 10px 10px, curve to 50px 20px with 10rem 1%)"));
    try$(_expectSerialized(_driver, "clip-path", "shape(from 10px 10px, curve to 50px 20px with 10rem 1px / 20vh 1ch)", "shape(from 10px 10px, curve to 50px 20px with 10rem 1px / 20vh 1ch)"));
    try$(_expectSerialized(_driver, "clip-path", "shape(from 10px 10px, curve by 50px 20px with 10rem 1px / 20vh 1ch)", "shape(from 10px 10px, curve by 50px 20px with 10rem 1px / 20vh 1ch)"));
    try$(_expectSerialized(_driver, "clip-path", "shape(from 10px 10px, smooth to 50px 20px with 10rem 1%)", "shape(from 10px 10px, smooth to 50px 20px with 10rem 1%)"));
    try$(_expectSerialized(_driver, "clip-path", "shape(from 10px 10px, smooth to 50px 1pt)", "shape(from 10px 10px, smooth to 50px 1pt)"));
    try$(_expectSerialized(_driver, "clip-path", "shape(from 10px 10px, arc to 50px 1pt of 10px 10px)", "shape(from 10px 10px, arc to 50px 1pt of 10px 10px)"));
    try$(_expectSerialized(_driver, "clip-path", "shape(from 10px 10px, arc to 50px 1pt of 10%)", "shape(from 10px 10px, arc to 50px 1pt of 10%)"));
    try$(_expectSerialized(_driver, "clip-path", "shape(from 10px 10px, arc to 50px 1pt of 10px 10px small rotate 0deg)", "shape(from 10px 10px, arc to 50px 1pt of 10px 10px)"));
    try$(_expectSerialized(_driver, "clip-path", "shape(from 10px 10px, arc to 50px 1pt of 10px small rotate 0deg)", "shape(from 10px 10px, arc to 50px 1pt of 10px)"));
    try$(_expectSerialized(_driver, "clip-path", "shape(from 10% 1rem, arc to 50px 1pt of 20% cw large rotate 25deg)", "shape(from 10% 1rem, arc to 50px 1pt of 20% cw large rotate 25deg)"));
    try$(_expectSerialized(_driver, "clip-path", "shape(evenodd from 0px 0px, close)", "shape(evenodd from 0px 0px, close)"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-mixins/functions/dashed-function-named-arg.tentative.html
test$("vaev-style-wpt-css-css-mixins-functions-dashed-function-named-arg.tentative") {
    try$(_expectInvalid(_driver, "top", "--func(--myident:)"));
    try$(_expectInvalid(_driver, "top", "--func( --myident:)"));
    try$(_expectInvalid(_driver, "top", "--func(--myident :)"));
    try$(_expectInvalid(_driver, "top", "--func(--myident: )"));
    try$(_expectInvalid(_driver, "top", "--func( --myident : )"));
    try$(_expectInvalid(_driver, "top", "--func(10px, --myident : )"));
    return Ok();
}

test$("vaev-style-wpt-css-css-mixins-functions-dashed-function-named-arg.tentative-xfail", XFAIL) {
    try$(_expectSerialized(_driver, "top", "--func(myident)", "--func(myident)"));
    try$(_expectSerialized(_driver, "top", "--func(--myident)", "--func(--myident)"));
    try$(_expectSerialized(_driver, "top", "--func(--)", "--func(--)"));
    try$(_expectSerialized(_driver, "top", "--func(50px --myident:)", "--func(50px --myident:)"));
    try$(_expectSerialized(_driver, "top", "--func({--myident:})", "--func({--myident:})"));
    try$(_expectSerialized(_driver, "top", "--func({ --myident:})", "--func({ --myident:})"));
    try$(_expectSerialized(_driver, "top", "--func({--myident :})", "--func({--myident :})"));
    try$(_expectSerialized(_driver, "top", "--func({--myident: })", "--func({--myident: })"));
    try$(_expectSerialized(_driver, "top", "--func(10px, { --myident : })", "--func(10px, { --myident : })"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-mixins/functions/dashed-function-parsing.html
test$("vaev-style-wpt-css-css-mixins-functions-dashed-function-parsing") {
    try$(_expectSerialized(_driver, "top", "--func(var(--x))", "--func(var(--x))", RegisteredPropertySet::TOP_LEVEL));
    try$(_expectSerialized(_driver, "top", "--func(var(--x, auto))", "--func(var(--x, auto))", RegisteredPropertySet::TOP_LEVEL));
    try$(_expectInvalid(_driver, "top", "--func(!)"));
    try$(_expectInvalid(_driver, "top", "--func( !)"));
    try$(_expectInvalid(_driver, "top", "--func( ! )"));
    try$(_expectInvalid(_driver, "top", "--func(! )"));
    try$(_expectInvalid(_driver, "top", "--func(;)"));
    try$(_expectInvalid(_driver, "top", "--func(})"));
    try$(_expectInvalid(_driver, "top", "--func(])"));
    try$(_expectInvalid(_driver, "top", "--func(url(\\\n))"));
    try$(_expectInvalid(_driver, "top", "--func(red !important)"));
    try$(_expectInvalid(_driver, "top", "--func({red !important})"));
    try$(_expectInvalid(_driver, "top", "--func({red} !important)"));
    try$(_expectInvalid(_driver, "top", "--func(asdf,)"));
    try$(_expectInvalid(_driver, "top", "--func(,)"));
    try$(_expectInvalid(_driver, "top", "--func(, )"));
    try$(_expectInvalid(_driver, "top", "--func( ,)"));
    try$(_expectInvalid(_driver, "top", "--func( , )"));
    try$(_expectInvalid(_driver, "top", "--func(a,,b)"));
    try$(_expectInvalid(_driver, "top", "--func(a, ,b)"));
    try$(_expectInvalid(_driver, "top", "--func(123 {})"));
    try$(_expectInvalid(_driver, "top", "--func(1{})"));
    try$(_expectInvalid(_driver, "top", "--func({}1)"));
    try$(_expectInvalid(_driver, "top", "--func( {}1)"));
    try$(_expectInvalid(_driver, "top", "--func({}1 )"));
    try$(_expectInvalid(_driver, "top", "--func({} 1)"));
    try$(_expectInvalid(_driver, "top", "--func(1, 2{})"));
    try$(_expectInvalid(_driver, "top", "--func(1,2{})"));
    try$(_expectInvalid(_driver, "top", "--func(1,2{} )"));
    try$(_expectInvalid(_driver, "top", "--func(1, {}2)"));
    try$(_expectInvalid(_driver, "top", "--func({})"));
    try$(_expectInvalid(_driver, "top", "--func(1, {})"));
    try$(_expectInvalid(_driver, "top", "--func(1, { })"));
    return Ok();
}

test$("vaev-style-wpt-css-css-mixins-functions-dashed-function-parsing-xfail", XFAIL) {
    try$(_expectSerialized(_driver, "top", "--func()", "--func()"));
    try$(_expectSerialized(_driver, "top", "--func(auto)", "--func(auto)"));
    try$(_expectSerialized(_driver, "top", "--func(100px)", "--func(100px)"));
    try$(_expectSerialized(_driver, "top", "--func(#fff)", "--func(#fff)"));
    try$(_expectSerialized(_driver, "top", "--func(auto, 100px, #fff)", "--func(auto, 100px, #fff)"));
    try$(_expectSerialized(_driver, "top", "--func(auto ,100px ,#fff)", "--func(auto ,100px ,#fff)"));
    try$(_expectSerialized(_driver, "top", "--func(auto , 100px , #fff)", "--func(auto , 100px , #fff)"));
    try$(_expectSerialized(_driver, "top", "--func(auto,100px,#fff)", "--func(auto,100px,#fff)"));
    try$(_expectSerialized(_driver, "top", "--func(\"thing\")", "--func(\"thing\")"));
    try$(_expectSerialized(_driver, "top", "--func(--bar(), --baz(--fez()))", "--func(--bar(), --baz(--fez()))"));
    try$(_expectSerialized(_driver, "top", "--func({1}, 2)", "--func({1}, 2)"));
    try$(_expectSerialized(_driver, "top", "--func({ 1}, 2)", "--func({ 1}, 2)"));
    try$(_expectSerialized(_driver, "top", "--func({1 }, 2)", "--func({1 }, 2)"));
    try$(_expectSerialized(_driver, "top", "--func({ 1 }, 2)", "--func({ 1 }, 2)"));
    try$(_expectSerialized(_driver, "top", "--func({1},{2})", "--func({1},{2})"));
    try$(_expectSerialized(_driver, "top", "--func({1, 2, 3},{4})", "--func({1, 2, 3},{4})"));
    try$(_expectSerialized(_driver, "top", "--func({,},{4})", "--func({,},{4})"));
    try$(_expectSerialized(_driver, "top", "--func({{}},{4})", "--func({{}},{4})"));
    try$(_expectSerialized(_driver, "top", "--func({[]},{4})", "--func({[]},{4})"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-overflow/parsing/overflow-invalid.html
test$("vaev-style-wpt-css-css-overflow-parsing-overflow-invalid") {
    try$(_expectInvalid(_driver, "overflow", "none"));
    try$(_expectInvalid(_driver, "overflow", "visible clip auto"));
    try$(_expectInvalid(_driver, "overflow-x", "visible clip"));
    try$(_expectInvalid(_driver, "overflow-y", "clip hidden"));
    try$(_expectInvalid(_driver, "overflow-block", "hidden scroll"));
    try$(_expectInvalid(_driver, "overflow-inline", "scroll auto"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-overflow/parsing/overflow-valid.html
test$("vaev-style-wpt-css-css-overflow-parsing-overflow-valid") {
    try$(_expectSerialized(_driver, "overflow-x", "visible", "visible"));
    try$(_expectSerialized(_driver, "overflow-x", "scroll", "scroll"));
    try$(_expectSerialized(_driver, "overflow-y", "auto", "auto"));
    try$(_expectSerialized(_driver, "overflow-block", "hidden", "hidden"));
    try$(_expectSerialized(_driver, "overflow-inline", "scroll", "scroll"));
    try$(_expectSerialized(_driver, "overflow-inline", "visible", "visible"));
    return Ok();
}

test$("vaev-style-wpt-css-css-overflow-parsing-overflow-valid-xfail", XFAIL) {
    try$(_expectSerialized(_driver, "overflow", "visible", "visible"));
    try$(_expectSerialized(_driver, "overflow", "hidden", "hidden"));
    try$(_expectSerialized(_driver, "overflow", "clip", "clip"));
    try$(_expectSerialized(_driver, "overflow", "scroll", "scroll"));
    try$(_expectSerialized(_driver, "overflow", "auto", "auto"));
    try$(_expectSerialized(_driver, "overflow", "visible visible", "visible"));
    try$(_expectSerialized(_driver, "overflow", "hidden visible", "hidden visible"));
    try$(_expectSerialized(_driver, "overflow", "clip clip", "clip"));
    try$(_expectSerialized(_driver, "overflow", "scroll auto", "scroll auto"));
    try$(_expectSerialized(_driver, "overflow", "auto auto", "auto"));
    try$(_expectSerialized(_driver, "overflow-y", "clip", "clip"));
    try$(_expectSerialized(_driver, "overflow-block", "clip", "clip"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-position/parsing/bottom-invalid.html
test$("vaev-style-wpt-css-css-position-parsing-bottom-invalid") {
    try$(_expectInvalid(_driver, "bottom", "min-content"));
    try$(_expectInvalid(_driver, "bottom", "60"));
    try$(_expectInvalid(_driver, "bottom", "10px 20%"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-position/parsing/bottom-valid.html
test$("vaev-style-wpt-css-css-position-parsing-bottom-valid") {
    try$(_expectSerialized(_driver, "bottom", "auto", "auto"));
    try$(_expectSerialized(_driver, "bottom", "-10px", "-10px"));
    try$(_expectSerialized(_driver, "bottom", "-20%", "-20%"));
    try$(_expectSerialized(_driver, "bottom", "calc(2em + 3ex)", "calc(2em + 3ex)"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-position/parsing/left-invalid.html
test$("vaev-style-wpt-css-css-position-parsing-left-invalid") {
    try$(_expectInvalid(_driver, "left", "min-content"));
    try$(_expectInvalid(_driver, "left", "60"));
    try$(_expectInvalid(_driver, "left", "10px 20%"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-position/parsing/left-valid.html
test$("vaev-style-wpt-css-css-position-parsing-left-valid") {
    try$(_expectSerialized(_driver, "left", "auto", "auto"));
    try$(_expectSerialized(_driver, "left", "-10px", "-10px"));
    try$(_expectSerialized(_driver, "left", "-20%", "-20%"));
    try$(_expectSerialized(_driver, "left", "calc(2em + 3ex)", "calc(2em + 3ex)"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-position/parsing/position-invalid.html
test$("vaev-style-wpt-css-css-position-parsing-position-invalid") {
    try$(_expectInvalid(_driver, "position", "auto"));
    try$(_expectInvalid(_driver, "position", "static relative"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-position/parsing/right-invalid.html
test$("vaev-style-wpt-css-css-position-parsing-right-invalid") {
    try$(_expectInvalid(_driver, "right", "min-content"));
    try$(_expectInvalid(_driver, "right", "60"));
    try$(_expectInvalid(_driver, "right", "10px 20%"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-position/parsing/right-valid.html
test$("vaev-style-wpt-css-css-position-parsing-right-valid") {
    try$(_expectSerialized(_driver, "right", "auto", "auto"));
    try$(_expectSerialized(_driver, "right", "-10px", "-10px"));
    try$(_expectSerialized(_driver, "right", "-20%", "-20%"));
    try$(_expectSerialized(_driver, "right", "calc(2em + 3ex)", "calc(2em + 3ex)"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-position/parsing/top-invalid.html
test$("vaev-style-wpt-css-css-position-parsing-top-invalid") {
    try$(_expectInvalid(_driver, "top", "min-content"));
    try$(_expectInvalid(_driver, "top", "60"));
    try$(_expectInvalid(_driver, "top", "10px 20%"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-position/parsing/top-valid.html
test$("vaev-style-wpt-css-css-position-parsing-top-valid") {
    try$(_expectSerialized(_driver, "top", "auto", "auto"));
    try$(_expectSerialized(_driver, "top", "-10px", "-10px"));
    try$(_expectSerialized(_driver, "top", "-20%", "-20%"));
    try$(_expectSerialized(_driver, "top", "calc(2em + 3ex)", "calc(2em + 3ex)"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-position/parsing/z-index-invalid.html
test$("vaev-style-wpt-css-css-position-parsing-z-index-invalid") {
    try$(_expectInvalid(_driver, "z-index", "none"));
    try$(_expectInvalid(_driver, "z-index", "10px"));
    try$(_expectInvalid(_driver, "z-index", "0.5"));
    try$(_expectInvalid(_driver, "z-index", "auto 123"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-position/parsing/z-index-valid.html
test$("vaev-style-wpt-css-css-position-parsing-z-index-valid") {
    try$(_expectSerialized(_driver, "z-index", "auto", "auto"));
    try$(_expectSerialized(_driver, "z-index", "-789", "-789"));
    try$(_expectSerialized(_driver, "z-index", "0", "0"));
    try$(_expectSerialized(_driver, "z-index", "123", "123"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-shapes/shape-functions/path-function-invalid.html
test$("vaev-style-wpt-css-css-shapes-shape-functions-path-function-invalid") {
    try$(_expectInvalid(_driver, "clip-path", "path(evenodd, 'not-a-path')"));
    try$(_expectInvalid(_driver, "clip-path", "path(badfillrule, 'M10,10')"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-shapes/shape-functions/rect-function-invalid.html
test$("vaev-style-wpt-css-css-shapes-shape-functions-rect-function-invalid") {
    try$(_expectInvalid(_driver, "clip-path", "rect()"));
    try$(_expectInvalid(_driver, "clip-path", "rect(0px)"));
    try$(_expectInvalid(_driver, "clip-path", "rect(0px 1px)"));
    try$(_expectInvalid(_driver, "clip-path", "rect(0px 1px 2px)"));
    try$(_expectInvalid(_driver, "clip-path", "rect(0px, 1px, 2px, 3px)"));
    try$(_expectInvalid(_driver, "clip-path", "rect(0, 1, 2, 3)"));
    try$(_expectInvalid(_driver, "clip-path", "rect(1% 2% 3% 4% 5%)"));
    try$(_expectInvalid(_driver, "clip-path", "rect(round 0)"));
    try$(_expectInvalid(_driver, "clip-path", "rect(10px auto 20px 30px round 123)"));
    try$(_expectInvalid(_driver, "clip-path", "rect(10px auto 20px 30px round 1% 2% 3% 4% 5%)"));
    return Ok();
}

test$("vaev-style-wpt-css-css-shapes-shape-functions-rect-function-invalid-xfail", XFAIL) {
    try$(_expectInvalid(_driver, "clip-path", "rect(10px auto 20px 30px round)"));
    try$(_expectInvalid(_driver, "clip-path", "rect(10px auto 20px 30px round / 1px)"));
    try$(_expectInvalid(_driver, "clip-path", "rect(10px auto 20px 30px round -20px)"));
    try$(_expectInvalid(_driver, "clip-path", "rect(10px auto 20px 30px round -40%)"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-shapes/shape-functions/shape-function-invalid.html
test$("vaev-style-wpt-css-css-shapes-shape-functions-shape-function-invalid") {
    try$(_expectInvalid(_driver, "clip-path", "shape(from 20px 40px line to 20px 30px)"));
    try$(_expectInvalid(_driver, "clip-path", "shape(from 20px 40px line to 20px 30px,)"));
    try$(_expectInvalid(_driver, "clip-path", "shape(from 20px, 40px, line to 20px, 30px)"));
    try$(_expectInvalid(_driver, "clip-path", "shape(from 20px 40px, move to 20px 30px, hline to top)"));
    try$(_expectInvalid(_driver, "clip-path", "shape(from 20px 40px, move to 20px 30px, hline to bottom)"));
    try$(_expectInvalid(_driver, "clip-path", "shape(from 20px 40px, move to 20px 30px, hline to y-start)"));
    try$(_expectInvalid(_driver, "clip-path", "shape(from 20px 40px, move to 20px 30px, hline to y-end)"));
    try$(_expectInvalid(_driver, "clip-path", "shape(from 20px 40px, move to 20px 30px, vline to left)"));
    try$(_expectInvalid(_driver, "clip-path", "shape(from 20px 40px, move to 20px 30px, vline to right)"));
    try$(_expectInvalid(_driver, "clip-path", "shape(from 20px 40px, move to 20px 30px, vline to x-start)"));
    try$(_expectInvalid(_driver, "clip-path", "shape(from 20px 40px, move to 20px 30px, vline to x-end)"));
    try$(_expectInvalid(_driver, "clip-path", "shape(from 20px 40px, move to 20px 30px, hline to top 20px)"));
    try$(_expectInvalid(_driver, "clip-path", "shape(from 20px 40px, move to 20px 30px, hline to right, vline to bottom left, close)"));
    try$(_expectInvalid(_driver, "clip-path", "shape(from 20px 40px, move to 20px 30px, hline by left)"));
    try$(_expectInvalid(_driver, "clip-path", "shape(from 20px 40px, move to 20px 30px, hline by right)"));
    try$(_expectInvalid(_driver, "clip-path", "shape(from 20px 40px, move to 20px 30px, vline by top)"));
    try$(_expectInvalid(_driver, "clip-path", "shape(from 20px 40px, move to 20px 30px, vline by bottom)"));
    try$(_expectInvalid(_driver, "clip-path", "shape(from 20px 40px, curve to 20px 20px, using 10px 30px)"));
    try$(_expectInvalid(_driver, "clip-path", "shape(from 20px 40px, curve to 20px 20px using 10px 30px, 12px 32px)"));
    try$(_expectInvalid(_driver, "clip-path", "shape(from 20px 40px, curve by 20px 20px, using 10px 30px)"));
    try$(_expectInvalid(_driver, "clip-path", "shape(from 20px 40px, curve by 20px 20px using 10px 30px, 12px 32px)"));
    try$(_expectInvalid(_driver, "clip-path", "shape(from 20px 40px, curve by 20px 20px via 10px 30px)"));
    try$(_expectInvalid(_driver, "clip-path", "shape(from 20px 40px, curve by 20px 20px via 10px 30px 12px 32px)"));
    try$(_expectInvalid(_driver, "clip-path", "shape(from 20px 40px, smooth by 20px 20px via 10px 30px / 12px 32px)"));
    try$(_expectInvalid(_driver, "clip-path", "shape(from 20px 40px, smooth by 20px 20px using 10px 30px 12px 32px)"));
    try$(_expectInvalid(_driver, "clip-path", "shape(from 20px 40px, curve to 20px 20px with 10px 30px 12px 32px)"));
    try$(_expectInvalid(_driver, "clip-path", "shape(from 20px 40px, curve by 20px 20px with 10px 30px 12px 32px)"));
    try$(_expectInvalid(_driver, "clip-path", "shape(from 20px 40px, curve by 20px 20px with 10px 30px /)"));
    try$(_expectInvalid(_driver, "clip-path", "shape(from 20px 40px, curve by top left using 10px 30px)"));
    try$(_expectInvalid(_driver, "clip-path", "shape(from 20px 40px, curve by 20px 30px using top right)"));
    try$(_expectInvalid(_driver, "clip-path", "shape(from 20px 40px, curve to 20px 30px using top right / 20px from end)"));
    try$(_expectInvalid(_driver, "clip-path", "shape(from 20px 40px, arc by top right of 10% 20%)"));
    try$(_expectInvalid(_driver, "clip-path", "shape(from 20px 40px, arc by 10px 20px of top right small cw)"));
    try$(_expectInvalid(_driver, "clip-path", "shape(from 20px 40px, curve by 20px 30px from start, using 23px 32px)"));
    try$(_expectInvalid(_driver, "clip-path", "shape(from 20px 40px, curve to 20px 20px using top left from origin / 12px 32px from end)"));
    try$(_expectInvalid(_driver, "clip-path", "shape(from 20px 40px, arc by 20px 20px of 10% 20% 12deg)"));
    try$(_expectInvalid(_driver, "clip-path", "shape(from 20px 40px, arc by 20px 20px of 10% 20% rotate 12deg rotate 13deg)"));
    try$(_expectInvalid(_driver, "clip-path", "shape(from 20px 40px, arc by 20px 20px of 10% 20% cw large 12deg)"));
    try$(_expectInvalid(_driver, "clip-path", "shape(from 20px 40px, arc by 20px 20px of 10% 20% small large)"));
    try$(_expectInvalid(_driver, "clip-path", "shape(from 20px 40px, arc by 20px 20px of 10% 20% cw ccw)"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-sizing/parsing/box-sizing-invalid.html
test$("vaev-style-wpt-css-css-sizing-parsing-box-sizing-invalid") {
    try$(_expectInvalid(_driver, "box-sizing", "margin-box"));
    try$(_expectInvalid(_driver, "box-sizing", "padding-box"));
    try$(_expectInvalid(_driver, "box-sizing", "fill-box"));
    try$(_expectInvalid(_driver, "box-sizing", "stroke-box"));
    try$(_expectInvalid(_driver, "box-sizing", "view-box"));
    try$(_expectInvalid(_driver, "box-sizing", "content-box border-box"));
    try$(_expectInvalid(_driver, "box-sizing", "content-box, border-box"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-sizing/parsing/box-sizing-valid.html
test$("vaev-style-wpt-css-css-sizing-parsing-box-sizing-valid") {
    try$(_expectSerialized(_driver, "box-sizing", "content-box", "content-box"));
    try$(_expectSerialized(_driver, "box-sizing", "border-box", "border-box"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-sizing/parsing/height-invalid.html
test$("vaev-style-wpt-css-css-sizing-parsing-height-invalid") {
    try$(_expectInvalid(_driver, "height", "none"));
    try$(_expectInvalid(_driver, "height", "min-content max-content"));
    return Ok();
}

test$("vaev-style-wpt-css-css-sizing-parsing-height-invalid-xfail", XFAIL) {
    try$(_expectInvalid(_driver, "height", "-10%"));
    try$(_expectInvalid(_driver, "height", "-0.5em"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-sizing/parsing/height-valid.html
test$("vaev-style-wpt-css-css-sizing-parsing-height-valid") {
    try$(_expectSerialized(_driver, "height", "auto", "auto"));
    try$(_expectSerialized(_driver, "height", "min-content", "min-content"));
    try$(_expectSerialized(_driver, "height", "max-content", "max-content"));
    try$(_expectSerialized(_driver, "height", "0", "0px"));
    try$(_expectSerialized(_driver, "height", "10%", "10%"));
    try$(_expectSerialized(_driver, "height", "0.5em", "0.5em"));
    try$(_expectSerialized(_driver, "height", "calc(10% - 0.5em)", "calc(10% - 0.5em)"));
    return Ok();
}

test$("vaev-style-wpt-css-css-sizing-parsing-height-valid-xfail", XFAIL) {
    try$(_expectSerialized(_driver, "height", "fit-content(10%)", "fit-content(10%)"));
    try$(_expectSerialized(_driver, "height", "fit-content(0.5em)", "fit-content(0.5em)"));
    try$(_expectSerialized(_driver, "height", "fit-content(calc(10% - 0.5em))", "fit-content(calc(10% - 0.5em))"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-sizing/parsing/max-height-invalid.html
test$("vaev-style-wpt-css-css-sizing-parsing-max-height-invalid") {
    try$(_expectInvalid(_driver, "max-height", "auto"));
    try$(_expectInvalid(_driver, "max-height", "min-content max-content"));
    return Ok();
}

test$("vaev-style-wpt-css-css-sizing-parsing-max-height-invalid-xfail", XFAIL) {
    try$(_expectInvalid(_driver, "max-height", "-10%"));
    try$(_expectInvalid(_driver, "max-height", "-0.5em"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-sizing/parsing/max-height-valid.html
test$("vaev-style-wpt-css-css-sizing-parsing-max-height-valid") {
    try$(_expectSerialized(_driver, "max-height", "none", "none"));
    try$(_expectSerialized(_driver, "max-height", "min-content", "min-content"));
    try$(_expectSerialized(_driver, "max-height", "max-content", "max-content"));
    try$(_expectSerialized(_driver, "max-height", "0", "0px"));
    try$(_expectSerialized(_driver, "max-height", "10%", "10%"));
    try$(_expectSerialized(_driver, "max-height", "0.5em", "0.5em"));
    try$(_expectSerialized(_driver, "max-height", "calc(10% - 0.5em)", "calc(10% - 0.5em)"));
    return Ok();
}

test$("vaev-style-wpt-css-css-sizing-parsing-max-height-valid-xfail", XFAIL) {
    try$(_expectSerialized(_driver, "max-height", "fit-content(10%)", "fit-content(10%)"));
    try$(_expectSerialized(_driver, "max-height", "fit-content(0.5em)", "fit-content(0.5em)"));
    try$(_expectSerialized(_driver, "max-height", "fit-content(calc(10% - 0.5em))", "fit-content(calc(10% - 0.5em))"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-sizing/parsing/max-width-invalid.html
test$("vaev-style-wpt-css-css-sizing-parsing-max-width-invalid") {
    try$(_expectInvalid(_driver, "max-width", "auto"));
    try$(_expectInvalid(_driver, "max-width", "min-content max-content"));
    return Ok();
}

test$("vaev-style-wpt-css-css-sizing-parsing-max-width-invalid-xfail", XFAIL) {
    try$(_expectInvalid(_driver, "max-width", "-10%"));
    try$(_expectInvalid(_driver, "max-width", "-0.5em"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-sizing/parsing/max-width-valid.html
test$("vaev-style-wpt-css-css-sizing-parsing-max-width-valid") {
    try$(_expectSerialized(_driver, "max-width", "none", "none"));
    try$(_expectSerialized(_driver, "max-width", "min-content", "min-content"));
    try$(_expectSerialized(_driver, "max-width", "max-content", "max-content"));
    try$(_expectSerialized(_driver, "max-width", "0", "0px"));
    try$(_expectSerialized(_driver, "max-width", "10%", "10%"));
    try$(_expectSerialized(_driver, "max-width", "0.5em", "0.5em"));
    try$(_expectSerialized(_driver, "max-width", "calc(10% - 0.5em)", "calc(10% - 0.5em)"));
    return Ok();
}

test$("vaev-style-wpt-css-css-sizing-parsing-max-width-valid-xfail", XFAIL) {
    try$(_expectSerialized(_driver, "max-width", "fit-content(10%)", "fit-content(10%)"));
    try$(_expectSerialized(_driver, "max-width", "fit-content(0.5em)", "fit-content(0.5em)"));
    try$(_expectSerialized(_driver, "max-width", "fit-content(calc(10% - 0.5em))", "fit-content(calc(10% - 0.5em))"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-sizing/parsing/min-height-invalid.html
test$("vaev-style-wpt-css-css-sizing-parsing-min-height-invalid") {
    try$(_expectInvalid(_driver, "min-height", "none"));
    try$(_expectInvalid(_driver, "min-height", "complex"));
    try$(_expectInvalid(_driver, "min-height", "60"));
    try$(_expectInvalid(_driver, "min-height", "10px 20%"));
    try$(_expectInvalid(_driver, "min-height", "max-content 10px"));
    try$(_expectInvalid(_driver, "min-height", "min-content max-content"));
    try$(_expectInvalid(_driver, "min-height", "available"));
    try$(_expectInvalid(_driver, "min-height", "10px border-box"));
    try$(_expectInvalid(_driver, "min-height", "content-box 20%"));
    return Ok();
}

test$("vaev-style-wpt-css-css-sizing-parsing-min-height-invalid-xfail", XFAIL) {
    try$(_expectInvalid(_driver, "min-height", "-10%"));
    try$(_expectInvalid(_driver, "min-height", "-0.5em"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-sizing/parsing/min-height-valid.html
test$("vaev-style-wpt-css-css-sizing-parsing-min-height-valid") {
    try$(_expectSerialized(_driver, "min-height", "auto", "auto"));
    try$(_expectSerialized(_driver, "min-height", "min-content", "min-content"));
    try$(_expectSerialized(_driver, "min-height", "max-content", "max-content"));
    try$(_expectSerialized(_driver, "min-height", "0", "0px"));
    try$(_expectSerialized(_driver, "min-height", "10%", "10%"));
    try$(_expectSerialized(_driver, "min-height", "0.5em", "0.5em"));
    try$(_expectSerialized(_driver, "min-height", "calc(10% - 0.5em)", "calc(10% - 0.5em)"));
    return Ok();
}

test$("vaev-style-wpt-css-css-sizing-parsing-min-height-valid-xfail", XFAIL) {
    try$(_expectSerialized(_driver, "min-height", "fit-content(10%)", "fit-content(10%)"));
    try$(_expectSerialized(_driver, "min-height", "fit-content(0.5em)", "fit-content(0.5em)"));
    try$(_expectSerialized(_driver, "min-height", "fit-content(calc(10% - 0.5em))", "fit-content(calc(10% - 0.5em))"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-sizing/parsing/min-width-invalid.html
test$("vaev-style-wpt-css-css-sizing-parsing-min-width-invalid") {
    try$(_expectInvalid(_driver, "min-width", "none"));
    try$(_expectInvalid(_driver, "min-width", "complex"));
    try$(_expectInvalid(_driver, "min-width", "60"));
    try$(_expectInvalid(_driver, "min-width", "10px 20%"));
    try$(_expectInvalid(_driver, "min-width", "max-content 10px"));
    try$(_expectInvalid(_driver, "min-width", "min-content max-content"));
    try$(_expectInvalid(_driver, "min-width", "available"));
    try$(_expectInvalid(_driver, "min-width", "10px border-box"));
    try$(_expectInvalid(_driver, "min-width", "content-box 20%"));
    return Ok();
}

test$("vaev-style-wpt-css-css-sizing-parsing-min-width-invalid-xfail", XFAIL) {
    try$(_expectInvalid(_driver, "min-width", "-10%"));
    try$(_expectInvalid(_driver, "min-width", "-0.5em"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-sizing/parsing/min-width-valid.html
test$("vaev-style-wpt-css-css-sizing-parsing-min-width-valid") {
    try$(_expectSerialized(_driver, "min-width", "auto", "auto"));
    try$(_expectSerialized(_driver, "min-width", "min-content", "min-content"));
    try$(_expectSerialized(_driver, "min-width", "max-content", "max-content"));
    try$(_expectSerialized(_driver, "min-width", "0", "0px"));
    try$(_expectSerialized(_driver, "min-width", "10%", "10%"));
    try$(_expectSerialized(_driver, "min-width", "0.5em", "0.5em"));
    try$(_expectSerialized(_driver, "min-width", "calc(10% - 0.5em)", "calc(10% - 0.5em)"));
    return Ok();
}

test$("vaev-style-wpt-css-css-sizing-parsing-min-width-valid-xfail", XFAIL) {
    try$(_expectSerialized(_driver, "min-width", "fit-content(10%)", "fit-content(10%)"));
    try$(_expectSerialized(_driver, "min-width", "fit-content(0.5em)", "fit-content(0.5em)"));
    try$(_expectSerialized(_driver, "min-width", "fit-content(calc(10% - 0.5em))", "fit-content(calc(10% - 0.5em))"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-sizing/parsing/width-invalid.html
test$("vaev-style-wpt-css-css-sizing-parsing-width-invalid") {
    try$(_expectInvalid(_driver, "width", "none"));
    try$(_expectInvalid(_driver, "width", "min-content max-content"));
    return Ok();
}

test$("vaev-style-wpt-css-css-sizing-parsing-width-invalid-xfail", XFAIL) {
    try$(_expectInvalid(_driver, "width", "-10%"));
    try$(_expectInvalid(_driver, "width", "-0.5em"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-sizing/parsing/width-valid.html
test$("vaev-style-wpt-css-css-sizing-parsing-width-valid") {
    try$(_expectSerialized(_driver, "width", "auto", "auto"));
    try$(_expectSerialized(_driver, "width", "min-content", "min-content"));
    try$(_expectSerialized(_driver, "width", "max-content", "max-content"));
    try$(_expectSerialized(_driver, "width", "0", "0px"));
    try$(_expectSerialized(_driver, "width", "10%", "10%"));
    try$(_expectSerialized(_driver, "width", "0.5em", "0.5em"));
    try$(_expectSerialized(_driver, "width", "calc(10% - 0.5em)", "calc(10% - 0.5em)"));
    return Ok();
}

test$("vaev-style-wpt-css-css-sizing-parsing-width-valid-xfail", XFAIL) {
    try$(_expectSerialized(_driver, "width", "fit-content(10%)", "fit-content(10%)"));
    try$(_expectSerialized(_driver, "width", "fit-content(0.5em)", "fit-content(0.5em)"));
    try$(_expectSerialized(_driver, "width", "fit-content(calc(10% - 0.5em))", "fit-content(calc(10% - 0.5em))"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-sizing/stretch/parsing.html
test$("vaev-style-wpt-css-css-sizing-stretch-parsing") {
    try$(_expectSerialized(_driver, "width", "stretch", "stretch"));
    try$(_expectSerialized(_driver, "height", "stretch", "stretch"));
    try$(_expectSerialized(_driver, "max-width", "stretch", "stretch"));
    try$(_expectSerialized(_driver, "min-width", "stretch", "stretch"));
    try$(_expectSerialized(_driver, "max-height", "stretch", "stretch"));
    try$(_expectSerialized(_driver, "min-height", "stretch", "stretch"));
    return Ok();
}

test$("vaev-style-wpt-css-css-sizing-stretch-parsing-xfail", XFAIL) {
    try$(_expectSerialized(_driver, "flex-basis", "stretch", "stretch"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-tables/parsing/border-collapse-invalid.html
test$("vaev-style-wpt-css-css-tables-parsing-border-collapse-invalid") {
    try$(_expectInvalid(_driver, "border-collapse", "none"));
    try$(_expectInvalid(_driver, "border-collapse", "separate collapse"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-tables/parsing/border-collapse-valid.html
test$("vaev-style-wpt-css-css-tables-parsing-border-collapse-valid") {
    try$(_expectSerialized(_driver, "border-collapse", "separate", "separate"));
    try$(_expectSerialized(_driver, "border-collapse", "collapse", "collapse"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-tables/parsing/border-spacing-invalid.html
test$("vaev-style-wpt-css-css-tables-parsing-border-spacing-invalid") {
    try$(_expectInvalid(_driver, "border-spacing", "10%"));
    try$(_expectInvalid(_driver, "border-spacing", "30"));
    try$(_expectInvalid(_driver, "border-spacing", "40px 50px 60px"));
    return Ok();
}

test$("vaev-style-wpt-css-css-tables-parsing-border-spacing-invalid-xfail", XFAIL) {
    try$(_expectInvalid(_driver, "border-spacing", "-20px"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-tables/parsing/border-spacing-valid.html
test$("vaev-style-wpt-css-css-tables-parsing-border-spacing-valid") {
    try$(_expectSerialized(_driver, "border-spacing", "0px", "0px"));
    try$(_expectSerialized(_driver, "border-spacing", "10px 20px", "10px 20px"));
    return Ok();
}

test$("vaev-style-wpt-css-css-tables-parsing-border-spacing-valid-xfail", XFAIL) {
    try$(_expectSerialized(_driver, "border-spacing", "calc(10px + 0.5em) calc(10px - 0.5em)", "calc(0.5em + 10px) calc(-0.5em + 10px)"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-tables/parsing/caption-side-invalid.html
test$("vaev-style-wpt-css-css-tables-parsing-caption-side-invalid") {
    try$(_expectInvalid(_driver, "caption-side", "auto"));
    try$(_expectInvalid(_driver, "caption-side", "left"));
    try$(_expectInvalid(_driver, "caption-side", "right"));
    try$(_expectInvalid(_driver, "caption-side", "top bottom"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-tables/parsing/caption-side-valid.html
test$("vaev-style-wpt-css-css-tables-parsing-caption-side-valid") {
    try$(_expectSerialized(_driver, "caption-side", "top", "top"));
    try$(_expectSerialized(_driver, "caption-side", "bottom", "bottom"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-tables/parsing/table-layout-invalid.html
test$("vaev-style-wpt-css-css-tables-parsing-table-layout-invalid") {
    try$(_expectInvalid(_driver, "table-layout", "none"));
    try$(_expectInvalid(_driver, "table-layout", "auto fixed"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-tables/parsing/table-layout-valid.html
test$("vaev-style-wpt-css-css-tables-parsing-table-layout-valid") {
    try$(_expectSerialized(_driver, "table-layout", "auto", "auto"));
    try$(_expectSerialized(_driver, "table-layout", "fixed", "fixed"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-text/parsing/text-align-invalid.html
test$("vaev-style-wpt-css-css-text-parsing-text-align-invalid") {
    try$(_expectInvalid(_driver, "text-align", "auto"));
    try$(_expectInvalid(_driver, "text-align", "start end"));
    try$(_expectInvalid(_driver, "text-align", "left right"));
    try$(_expectInvalid(_driver, "text-align", "center justify"));
    try$(_expectInvalid(_driver, "text-align", "justify-all match-parent"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-text/parsing/text-align-valid.html
test$("vaev-style-wpt-css-css-text-parsing-text-align-valid") {
    try$(_expectSerialized(_driver, "text-align", "start", "start"));
    try$(_expectSerialized(_driver, "text-align", "end", "end"));
    try$(_expectSerialized(_driver, "text-align", "left", "left"));
    try$(_expectSerialized(_driver, "text-align", "right", "right"));
    try$(_expectSerialized(_driver, "text-align", "center", "center"));
    try$(_expectSerialized(_driver, "text-align", "justify", "justify"));
    return Ok();
}

test$("vaev-style-wpt-css-css-text-parsing-text-align-valid-xfail", XFAIL) {
    try$(_expectSerialized(_driver, "text-align", "match-parent", "match-parent"));
    try$(_expectSerialized(_driver, "text-align", "justify-all", "justify-all"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-text/parsing/text-transform-invalid.html
test$("vaev-style-wpt-css-css-text-parsing-text-transform-invalid") {
    try$(_expectInvalid(_driver, "text-transform", "none full-width"));
    try$(_expectInvalid(_driver, "text-transform", "capitalize none"));
    try$(_expectInvalid(_driver, "text-transform", "capitalize full-width lowercase"));
    try$(_expectInvalid(_driver, "text-transform", "uppercase full-size-kana uppercase"));
    try$(_expectInvalid(_driver, "text-transform", "full-width full-size-kana full-width"));
    try$(_expectInvalid(_driver, "text-transform", "full-size-kana capitalize full-size-kana"));
    try$(_expectInvalid(_driver, "text-transform", "none math-auto"));
    try$(_expectInvalid(_driver, "text-transform", "math-auto none"));
    try$(_expectInvalid(_driver, "text-transform", "uppercase math-auto"));
    try$(_expectInvalid(_driver, "text-transform", "math-auto uppercase"));
    try$(_expectInvalid(_driver, "text-transform", "lowercase math-auto"));
    try$(_expectInvalid(_driver, "text-transform", "math-auto lowercase"));
    try$(_expectInvalid(_driver, "text-transform", "capitalize math-auto"));
    try$(_expectInvalid(_driver, "text-transform", "math-auto capitalize"));
    try$(_expectInvalid(_driver, "text-transform", "full-width math-auto"));
    try$(_expectInvalid(_driver, "text-transform", "math-auto full-width"));
    try$(_expectInvalid(_driver, "text-transform", "full-size-kana math-auto"));
    try$(_expectInvalid(_driver, "text-transform", "math-auto full-size-kana"));
    try$(_expectInvalid(_driver, "text-transform", "math-auto math-auto"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-text/parsing/text-transform-valid.html
test$("vaev-style-wpt-css-css-text-parsing-text-transform-valid") {
    try$(_expectSerialized(_driver, "text-transform", "none", "none"));
    try$(_expectSerialized(_driver, "text-transform", "uppercase", "uppercase"));
    return Ok();
}

test$("vaev-style-wpt-css-css-text-parsing-text-transform-valid-xfail", XFAIL) {
    try$(_expectSerialized(_driver, "text-transform", "capitalize", "capitalize"));
    try$(_expectSerialized(_driver, "text-transform", "lowercase", "lowercase"));
    try$(_expectSerialized(_driver, "text-transform", "full-width", "full-width"));
    try$(_expectSerialized(_driver, "text-transform", "full-size-kana", "full-size-kana"));
    try$(_expectSerialized(_driver, "text-transform", "math-auto", "math-auto"));
    try$(_expectSerialized(_driver, "text-transform", "capitalize full-width", "capitalize full-width"));
    try$(_expectSerialized(_driver, "text-transform", "uppercase full-size-kana", "uppercase full-size-kana"));
    try$(_expectSerialized(_driver, "text-transform", "full-width full-size-kana", "full-width full-size-kana"));
    try$(_expectSerialized(_driver, "text-transform", "full-width lowercase", "lowercase full-width"));
    try$(_expectSerialized(_driver, "text-transform", "full-size-kana capitalize", "capitalize full-size-kana"));
    try$(_expectSerialized(_driver, "text-transform", "full-size-kana full-width", "full-width full-size-kana"));
    try$(_expectSerialized(_driver, "text-transform", "capitalize full-width full-size-kana", "capitalize full-width full-size-kana"));
    try$(_expectSerialized(_driver, "text-transform", "full-width full-size-kana uppercase", "uppercase full-width full-size-kana"));
    try$(_expectSerialized(_driver, "text-transform", "full-size-kana lowercase full-width", "lowercase full-width full-size-kana"));
    try$(_expectSerialized(_driver, "text-transform", "lowercase full-size-kana full-width", "lowercase full-width full-size-kana"));
    try$(_expectSerialized(_driver, "text-transform", "full-width uppercase full-size-kana", "uppercase full-width full-size-kana"));
    try$(_expectSerialized(_driver, "text-transform", "full-size-kana full-width capitalize", "capitalize full-width full-size-kana"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-text/parsing/white-space-invalid.html
test$("vaev-style-wpt-css-css-text-parsing-white-space-invalid") {
    try$(_expectInvalid(_driver, "white-space", "auto"));
    try$(_expectInvalid(_driver, "white-space", "normal pre"));
    try$(_expectInvalid(_driver, "white-space", "nowrap pre-wrap"));
    try$(_expectInvalid(_driver, "white-space", "pre-line break-spaces"));
    try$(_expectInvalid(_driver, "white-space", "balance"));
    try$(_expectInvalid(_driver, "white-space", "pretty"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-text/parsing/white-space-shorthand.html
test$("vaev-style-wpt-css-css-text-parsing-white-space-shorthand") {
    try$(_expectSerialized(_driver, "white-space", "nowrap", "nowrap"));
    try$(_expectSerialized(_driver, "white-space", "break-spaces", "break-spaces"));
    try$(_expectInvalid(_driver, "white-space", "balance"));
    try$(_expectInvalid(_driver, "white-space", "collapse balance"));
    try$(_expectInvalid(_driver, "white-space", "balance collapse"));
    try$(_expectInvalid(_driver, "white-space", "preserve balance"));
    try$(_expectInvalid(_driver, "white-space", "balance preserve"));
    return Ok();
}

test$("vaev-style-wpt-css-css-text-parsing-white-space-shorthand-xfail", XFAIL) {
    try$(_expectSerialized(_driver, "white-space", "collapse", "normal"));
    try$(_expectSerialized(_driver, "white-space", "wrap", "normal"));
    try$(_expectSerialized(_driver, "white-space", "collapse wrap", "normal"));
    try$(_expectSerialized(_driver, "white-space", "wrap collapse", "normal"));
    try$(_expectSerialized(_driver, "white-space", "preserve nowrap", "pre"));
    try$(_expectSerialized(_driver, "white-space", "nowrap preserve", "pre"));
    try$(_expectSerialized(_driver, "white-space", "collapse nowrap", "nowrap"));
    try$(_expectSerialized(_driver, "white-space", "nowrap collapse", "nowrap"));
    try$(_expectSerialized(_driver, "white-space", "preserve", "pre-wrap"));
    try$(_expectSerialized(_driver, "white-space", "preserve wrap", "pre-wrap"));
    try$(_expectSerialized(_driver, "white-space", "wrap preserve", "pre-wrap"));
    try$(_expectSerialized(_driver, "white-space", "break-spaces wrap", "break-spaces"));
    try$(_expectSerialized(_driver, "white-space", "wrap break-spaces", "break-spaces"));
    try$(_expectSerialized(_driver, "white-space", "preserve-breaks", "pre-line"));
    try$(_expectSerialized(_driver, "white-space", "preserve-breaks wrap", "pre-line"));
    try$(_expectSerialized(_driver, "white-space", "wrap preserve-breaks", "pre-line"));
    try$(_expectSerialized(_driver, "white-space", "preserve-breaks nowrap", "preserve-breaks nowrap"));
    try$(_expectSerialized(_driver, "white-space", "nowrap preserve-breaks", "preserve-breaks nowrap"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-text/parsing/white-space-valid.html
test$("vaev-style-wpt-css-css-text-parsing-white-space-valid") {
    try$(_expectSerialized(_driver, "white-space", "normal", "normal"));
    try$(_expectSerialized(_driver, "white-space", "pre", "pre"));
    try$(_expectSerialized(_driver, "white-space", "nowrap", "nowrap"));
    try$(_expectSerialized(_driver, "white-space", "pre-wrap", "pre-wrap"));
    try$(_expectSerialized(_driver, "white-space", "break-spaces", "break-spaces"));
    try$(_expectSerialized(_driver, "white-space", "pre-line", "pre-line"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-transforms/parsing/transform-box-invalid.html
test$("vaev-style-wpt-css-css-transforms-parsing-transform-box-invalid") {
    try$(_expectInvalid(_driver, "transform-box", "margin-box"));
    try$(_expectInvalid(_driver, "transform-box", "padding-box"));
    try$(_expectInvalid(_driver, "transform-box", "fill-box view-box"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-transforms/parsing/transform-box-valid.html
test$("vaev-style-wpt-css-css-transforms-parsing-transform-box-valid") {
    try$(_expectSerialized(_driver, "transform-box", "content-box", "content-box"));
    try$(_expectSerialized(_driver, "transform-box", "border-box", "border-box"));
    try$(_expectSerialized(_driver, "transform-box", "fill-box", "fill-box"));
    try$(_expectSerialized(_driver, "transform-box", "stroke-box", "stroke-box"));
    try$(_expectSerialized(_driver, "transform-box", "view-box", "view-box"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-transforms/parsing/transform-invalid.html
test$("vaev-style-wpt-css-css-transforms-parsing-transform-invalid") {
    try$(_expectInvalid(_driver, "transform", "none scale(2)"));
    try$(_expectInvalid(_driver, "transform", "translateX(3%) none"));
    try$(_expectInvalid(_driver, "transform", "matrix(1, 2)"));
    try$(_expectInvalid(_driver, "transform", "translate(1px, 2px, 3px)"));
    try$(_expectInvalid(_driver, "transform", "translateX(-4px, 5px)"));
    try$(_expectInvalid(_driver, "transform", "translateY(4%, 5%)"));
    try$(_expectInvalid(_driver, "transform", "scale(6, 7, 8)"));
    try$(_expectInvalid(_driver, "transform", "scale(6%, 7%, 8%)"));
    try$(_expectInvalid(_driver, "transform", "scaleX(1, 2)"));
    try$(_expectInvalid(_driver, "transform", "scaleX(1%, 2%)"));
    try$(_expectInvalid(_driver, "transform", "scaleY(3, 4)"));
    try$(_expectInvalid(_driver, "transform", "scaleY(3%, 4%)"));
    try$(_expectInvalid(_driver, "transform", "rotate(0, 0)"));
    try$(_expectInvalid(_driver, "transform", "rotate(0, 0, 0)"));
    try$(_expectInvalid(_driver, "transform", "rotate(0, 0, 0, 0)"));
    try$(_expectInvalid(_driver, "transform", "skew(0, 0, 0)"));
    try$(_expectInvalid(_driver, "transform", "skewX(0, 0)"));
    try$(_expectInvalid(_driver, "transform", "skewY(0, 0)"));
    try$(_expectInvalid(_driver, "transform", "scaleX(2), scaleY(3)"));
    try$(_expectInvalid(_driver, "transform", "perspective(1000)"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-transforms/parsing/transform-origin-invalid.html
test$("vaev-style-wpt-css-css-transforms-parsing-transform-origin-invalid") {
    try$(_expectInvalid(_driver, "transform-origin", "1px 2px 3%"));
    try$(_expectInvalid(_driver, "transform-origin", "1px 2px left"));
    try$(_expectInvalid(_driver, "transform-origin", "1px 2px 3px 4px"));
    try$(_expectInvalid(_driver, "transform-origin", "1px left"));
    try$(_expectInvalid(_driver, "transform-origin", "top 1px"));
    try$(_expectInvalid(_driver, "transform-origin", "right left"));
    try$(_expectInvalid(_driver, "transform-origin", "top bottom"));
    try$(_expectInvalid(_driver, "transform-origin", "bottom 10% right 20%"));
    try$(_expectInvalid(_driver, "transform-origin", "right 30% top -60px"));
    try$(_expectInvalid(_driver, "transform-origin", "right 20px bottom 30px"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-transforms/parsing/transform-origin-valid.html
test$("vaev-style-wpt-css-css-transforms-parsing-transform-origin-valid") {
    try$(_expectSerializedOneOf(_driver, "transform-origin", "left center", {"left center 0px", "left center"}));
    try$(_expectSerializedOneOf(_driver, "transform-origin", "center top", {"center top 0px", "center top"}));
    try$(_expectSerializedOneOf(_driver, "transform-origin", "right -4%", {"right -4% 0px", "right -4%"}));
    return Ok();
}

test$("vaev-style-wpt-css-css-transforms-parsing-transform-origin-valid-xfail", XFAIL) {
    try$(_expectSerializedOneOf(_driver, "transform-origin", "left", {"left center 0px", "left 50% 0px", "left center"}));
    try$(_expectSerializedOneOf(_driver, "transform-origin", "center", {"center center 0px", "center 50% 0px", "center center"}));
    try$(_expectSerializedOneOf(_driver, "transform-origin", "right", {"right center 0px", "right 50% 0px", "right center"}));
    try$(_expectSerializedOneOf(_driver, "transform-origin", "top", {"center top 0px", "50% top 0px", "center top"}));
    try$(_expectSerializedOneOf(_driver, "transform-origin", "bottom", {"center bottom 0px", "50% bottom 0px", "center bottom"}));
    try$(_expectSerializedOneOf(_driver, "transform-origin", "-1px", {"-1px center 0px", "-1px 50% 0px", "-1px center"}));
    try$(_expectSerializedOneOf(_driver, "transform-origin", "calc(2em + 3ex)", {"calc(2em + 3ex) center 0px", "calc(2em + 3ex) 50% 0px", "calc(2em + 3ex) center"}));
    try$(_expectSerializedOneOf(_driver, "transform-origin", "-4%", {"-4% center 0px", "-4% 50% 0px", "-4% center"}));
    try$(_expectSerialized(_driver, "transform-origin", "-1px bottom 5px", "-1px bottom 5px"));
    try$(_expectSerialized(_driver, "transform-origin", "center left 6px", "left center 6px"));
    try$(_expectSerializedOneOf(_driver, "transform-origin", "top center", {"center top 0px", "center top"}));
    try$(_expectSerialized(_driver, "transform-origin", "bottom right 7px", "right bottom 7px"));
    try$(_expectSerialized(_driver, "transform-origin", "-1px -2px -3px", "-1px -2px -3px"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-ui/parsing/box-sizing-invalid.html
test$("vaev-style-wpt-css-css-ui-parsing-box-sizing-invalid") {
    try$(_expectInvalid(_driver, "box-sizing", "auto"));
    try$(_expectInvalid(_driver, "box-sizing", "content-box border-box"));
    try$(_expectInvalid(_driver, "box-sizing", "fill-box"));
    try$(_expectInvalid(_driver, "box-sizing", "margin-box"));
    try$(_expectInvalid(_driver, "box-sizing", "padding-box"));
    try$(_expectInvalid(_driver, "box-sizing", "view-box"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-ui/parsing/box-sizing-valid.html
test$("vaev-style-wpt-css-css-ui-parsing-box-sizing-valid") {
    try$(_expectSerialized(_driver, "box-sizing", "content-box", "content-box"));
    try$(_expectSerialized(_driver, "box-sizing", "border-box", "border-box"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-ui/parsing/outline-color-invalid.html
test$("vaev-style-wpt-css-css-ui-parsing-outline-color-invalid") {
    try$(_expectInvalid(_driver, "outline-color", "invert"));
    try$(_expectInvalid(_driver, "outline-color", "none"));
    try$(_expectInvalid(_driver, "outline-color", "50%"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-ui/parsing/outline-color-valid.html
test$("vaev-style-wpt-css-css-ui-parsing-outline-color-valid") {
    try$(_expectSerialized(_driver, "outline-color", "rgba(10, 20, 30, 0.4)", "rgba(10, 20, 30, 0.4)"));
    try$(_expectSerialized(_driver, "outline-color", "auto", "auto"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-ui/parsing/outline-invalid.html
test$("vaev-style-wpt-css-css-ui-parsing-outline-invalid") {
    try$(_expectInvalid(_driver, "outline", "invert solid rgba(10, 20, 30, 0.4)"));
    try$(_expectInvalid(_driver, "outline", "invert solid"));
    try$(_expectInvalid(_driver, "outline", "double invert groove"));
    try$(_expectInvalid(_driver, "outline", "thin outset thick"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-ui/parsing/outline-offset-invalid.html
test$("vaev-style-wpt-css-css-ui-parsing-outline-offset-invalid") {
    try$(_expectInvalid(_driver, "outline-offset", "auto"));
    try$(_expectInvalid(_driver, "outline-offset", "1%"));
    try$(_expectInvalid(_driver, "outline-offset", "2px 3px"));
    try$(_expectInvalid(_driver, "outline-offset", "inset 3px"));
    try$(_expectInvalid(_driver, "outline-offset", "3px inset"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-ui/parsing/outline-offset-valid.html
test$("vaev-style-wpt-css-css-ui-parsing-outline-offset-valid") {
    try$(_expectSerialized(_driver, "outline-offset", "0", "0px"));
    try$(_expectSerialized(_driver, "outline-offset", "1px", "1px"));
    try$(_expectSerialized(_driver, "outline-offset", "2em", "2em"));
    try$(_expectSerialized(_driver, "outline-offset", "calc(3rem + 4vw)", "calc(3rem + 4vw)"));
    return Ok();
}

test$("vaev-style-wpt-css-css-ui-parsing-outline-offset-valid-xfail", XFAIL) {
    try$(_expectSerialized(_driver, "outline-offset", "inset", "inset"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-ui/parsing/outline-style-invalid.html
test$("vaev-style-wpt-css-css-ui-parsing-outline-style-invalid") {
    try$(_expectInvalid(_driver, "outline-style", "dotted dashed"));
    try$(_expectInvalid(_driver, "outline-style", "solid double groove ridge"));
    return Ok();
}

test$("vaev-style-wpt-css-css-ui-parsing-outline-style-invalid-xfail", XFAIL) {
    try$(_expectInvalid(_driver, "outline-style", "hidden"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-ui/parsing/outline-style-valid.html
test$("vaev-style-wpt-css-css-ui-parsing-outline-style-valid") {
    try$(_expectSerialized(_driver, "outline-style", "auto", "auto"));
    try$(_expectSerialized(_driver, "outline-style", "none", "none"));
    try$(_expectSerialized(_driver, "outline-style", "dotted", "dotted"));
    try$(_expectSerialized(_driver, "outline-style", "dashed", "dashed"));
    try$(_expectSerialized(_driver, "outline-style", "solid", "solid"));
    try$(_expectSerialized(_driver, "outline-style", "double", "double"));
    try$(_expectSerialized(_driver, "outline-style", "groove", "groove"));
    try$(_expectSerialized(_driver, "outline-style", "ridge", "ridge"));
    try$(_expectSerialized(_driver, "outline-style", "outset", "outset"));
    return Ok();
}

test$("vaev-style-wpt-css-css-ui-parsing-outline-style-valid-xfail", XFAIL) {
    try$(_expectSerialized(_driver, "outline-style", "inset", "inset"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-ui/parsing/outline-width-invalid.html
test$("vaev-style-wpt-css-css-ui-parsing-outline-width-invalid") {
    try$(_expectInvalid(_driver, "outline-width", "auto"));
    try$(_expectInvalid(_driver, "outline-width", "1%"));
    try$(_expectInvalid(_driver, "outline-width", "thin medium thick medium thin"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-ui/parsing/outline-width-valid.html
test$("vaev-style-wpt-css-css-ui-parsing-outline-width-valid") {
    try$(_expectSerialized(_driver, "outline-width", "0", "0px"));
    try$(_expectSerialized(_driver, "outline-width", "1px", "1px"));
    try$(_expectSerialized(_driver, "outline-width", "2em", "2em"));
    try$(_expectSerialized(_driver, "outline-width", "calc(2em + 3ex)", "calc(2em + 3ex)"));
    try$(_expectSerialized(_driver, "outline-width", "thin", "thin"));
    try$(_expectSerialized(_driver, "outline-width", "medium", "medium"));
    try$(_expectSerialized(_driver, "outline-width", "thick", "thick"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-values/acos-asin-atan-atan2-invalid.html
test$("vaev-style-wpt-css-css-values-acos-asin-atan-atan2-invalid") {
    try$(_expectInvalid(_driver, "transform", "rotate(asin())"));
    try$(_expectInvalid(_driver, "transform", "rotate(asin( ))"));
    try$(_expectInvalid(_driver, "transform", "rotate(asin(,))"));
    try$(_expectInvalid(_driver, "transform", "rotate(asin(1dag))"));
    try$(_expectInvalid(_driver, "transform", "rotate(asin(1deg, ))"));
    try$(_expectInvalid(_driver, "transform", "rotate(asin(, 1deg))"));
    try$(_expectInvalid(_driver, "transform", "rotate(asin(1deg + ))"));
    try$(_expectInvalid(_driver, "transform", "rotate(asin(1deg - ))"));
    try$(_expectInvalid(_driver, "transform", "rotate(asin(1deg * ))"));
    try$(_expectInvalid(_driver, "transform", "rotate(asin(1deg / ))"));
    try$(_expectInvalid(_driver, "transform", "rotate(asin(1deg 2deg))"));
    try$(_expectInvalid(_driver, "transform", "rotate(asin(1deg, , 2deg))"));
    try$(_expectInvalid(_driver, "transform", "rotate(acos())"));
    try$(_expectInvalid(_driver, "transform", "rotate(acos( ))"));
    try$(_expectInvalid(_driver, "transform", "rotate(acos(,))"));
    try$(_expectInvalid(_driver, "transform", "rotate(acos(1dag))"));
    try$(_expectInvalid(_driver, "transform", "rotate(acos(1deg, ))"));
    try$(_expectInvalid(_driver, "transform", "rotate(acos(, 1deg))"));
    try$(_expectInvalid(_driver, "transform", "rotate(acos(1deg + ))"));
    try$(_expectInvalid(_driver, "transform", "rotate(acos(1deg - ))"));
    try$(_expectInvalid(_driver, "transform", "rotate(acos(1deg * ))"));
    try$(_expectInvalid(_driver, "transform", "rotate(acos(1deg / ))"));
    try$(_expectInvalid(_driver, "transform", "rotate(acos(1deg 2deg))"));
    try$(_expectInvalid(_driver, "transform", "rotate(acos(1deg, , 2deg))"));
    try$(_expectInvalid(_driver, "transform", "rotate(atan())"));
    try$(_expectInvalid(_driver, "transform", "rotate(atan( ))"));
    try$(_expectInvalid(_driver, "transform", "rotate(atan(,))"));
    try$(_expectInvalid(_driver, "transform", "rotate(atan(1dag))"));
    try$(_expectInvalid(_driver, "transform", "rotate(atan(1deg, ))"));
    try$(_expectInvalid(_driver, "transform", "rotate(atan(, 1deg))"));
    try$(_expectInvalid(_driver, "transform", "rotate(atan(1deg + ))"));
    try$(_expectInvalid(_driver, "transform", "rotate(atan(1deg - ))"));
    try$(_expectInvalid(_driver, "transform", "rotate(atan(1deg * ))"));
    try$(_expectInvalid(_driver, "transform", "rotate(atan(1deg / ))"));
    try$(_expectInvalid(_driver, "transform", "rotate(atan(1deg 2deg))"));
    try$(_expectInvalid(_driver, "transform", "rotate(atan(1deg, , 2deg))"));
    try$(_expectInvalid(_driver, "transform", "rotate(asin(90px))"));
    try$(_expectInvalid(_driver, "transform", "rotate(asin(30deg + 1.0471967rad, 0))"));
    try$(_expectInvalid(_driver, "transform", "rotate(acos( 0 ,))"));
    try$(_expectInvalid(_driver, "transform", "rotate(acos( () 30deg - 0.523599rad ))"));
    try$(_expectInvalid(_driver, "transform", "rotate(atan(45deg ))"));
    try$(_expectInvalid(_driver, "transform", "rotate(atan(30deg, + 0.261799rad))"));
    try$(_expectInvalid(_driver, "transform", "rotate(atan2())"));
    try$(_expectInvalid(_driver, "transform", "rotate(atan2( ))"));
    try$(_expectInvalid(_driver, "transform", "rotate(atan2(,))"));
    try$(_expectInvalid(_driver, "transform", "rotate(atan2(1dag))"));
    try$(_expectInvalid(_driver, "transform", "rotate(atan2(1deg, ))"));
    try$(_expectInvalid(_driver, "transform", "rotate(atan2(, 1deg))"));
    try$(_expectInvalid(_driver, "transform", "rotate(atan2(1deg + ))"));
    try$(_expectInvalid(_driver, "transform", "rotate(atan2(1deg - ))"));
    try$(_expectInvalid(_driver, "transform", "rotate(atan2(1deg * ))"));
    try$(_expectInvalid(_driver, "transform", "rotate(atan2(1deg / ))"));
    try$(_expectInvalid(_driver, "transform", "rotate(atan2(1deg 2deg))"));
    try$(_expectInvalid(_driver, "transform", "rotate(atan2(1deg, , 2deg))"));
    try$(_expectInvalid(_driver, "transform", "rotate(atan2(90px))"));
    try$(_expectInvalid(_driver, "transform", "rotate(atan2(90px, 100%))"));
    try$(_expectInvalid(_driver, "transform", "rotate(atan2(30deg + 1.0471967rad, 0))"));
    try$(_expectInvalid(_driver, "transform", "rotate(atan2( 0 ,))"));
    try$(_expectInvalid(_driver, "transform", "rotate(atan2( () 30deg - 0.523599rad ))"));
    try$(_expectInvalid(_driver, "transform", "rotate(atan2(45deg ))"));
    try$(_expectInvalid(_driver, "transform", "rotate(atan2(30deg, + 0.261799rad))"));
    try$(_expectInvalid(_driver, "transform", "rotate(atan2(2, 1px))"));
    try$(_expectInvalid(_driver, "transform", "rotate(atan2(2, 1%))"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-values/calc-invalid-parsing.html
test$("vaev-style-wpt-css-css-values-calc-invalid-parsing") {
    try$(_expectInvalid(_driver, "transform", "rotate(calc((0.25turn error)))"));
    try$(_expectInvalid(_driver, "width", "calc(7px * up)"));
    try$(_expectInvalid(_driver, "width", "round(nearest, 1px, 1px, 1px)"));
    try$(_expectInvalid(_driver, "width", "round(nearest, 1px)"));
    try$(_expectInvalid(_driver, "width", "calc([])"));
    try$(_expectInvalid(_driver, "width", "calc( [])"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-values/calc-size/calc-size-parsing.html
test$("vaev-style-wpt-css-css-values-calc-size-calc-size-parsing") {
    try$(_expectInvalid(_driver, "max-height", "calc-size(auto, size)"));
    try$(_expectInvalid(_driver, "max-width", "calc-size(auto, size)"));
    try$(_expectInvalid(_driver, "height", "calc-size(none, size)"));
    try$(_expectInvalid(_driver, "width", "calc-size(none, size)"));
    try$(_expectInvalid(_driver, "min-height", "calc-size(none, size)"));
    try$(_expectInvalid(_driver, "min-width", "calc-size(none, size)"));
    try$(_expectInvalid(_driver, "max-height", "calc-size(none, size)"));
    try$(_expectInvalid(_driver, "max-width", "calc-size(none, size)"));
    try$(_expectInvalid(_driver, "width", "calc-size(any, size)"));
    try$(_expectInvalid(_driver, "width", "calc-size(any, fit-content)"));
    try$(_expectInvalid(_driver, "width", "calc-size(any, max-content)"));
    try$(_expectInvalid(_driver, "width", "calc-size(any, min-content)"));
    try$(_expectInvalid(_driver, "width", "size"));
    try$(_expectInvalid(_driver, "width", "sign(size)"));
    try$(_expectInvalid(_driver, "width", "calc(50px * sign(size))"));
    try$(_expectInvalid(_driver, "width", "calc-size(any, calc-size(10px, sign(size) * size))"));
    try$(_expectInvalid(_driver, "width", "calc-size(any, calc-size(any, 10px))"));
    try$(_expectInvalid(_driver, "width", "calc(calc-size(auto, size))"));
    try$(_expectInvalid(_driver, "width", "calc(50px * sign(calc-size(auto, size)))"));
    try$(_expectInvalid(_driver, "width", "calc(calc-size(auto, size) + calc-size(auto, size))"));
    try$(_expectInvalid(_driver, "width", "calc(abs(calc-size(auto, size)))"));
    try$(_expectInvalid(_driver, "width", "calc(100px * progress(calc-size(auto, size) from calc-size(auto, 0px) to calc-size(auto, size)))"));
    try$(_expectInvalid(_driver, "width", "calc(100px * progress(calc-size(auto, size) from 0px to 100px))"));
    try$(_expectInvalid(_driver, "width", "calc(100px * progress(50px from calc-size(auto, 0px) to calc-size(auto, size)))"));
    try$(_expectInvalid(_driver, "width", "min(calc-size(auto, 0px), calc-size(auto, size))"));
    try$(_expectInvalid(_driver, "width", "calc(min(calc-size(auto, 0px), calc-size(auto, size)))"));
    try$(_expectInvalid(_driver, "width", "max(calc-size(auto, 0px), calc-size(auto, size))"));
    try$(_expectInvalid(_driver, "width", "calc(max(calc-size(auto, 0px), calc-size(auto, size)))"));
    try$(_expectInvalid(_driver, "width", "clamp(calc-size(auto, 0px), calc-size(auto, 30px), calc-size(auto, size))"));
    try$(_expectInvalid(_driver, "width", "calc(clamp(calc-size(auto, 0px), calc-size(auto, 30px), calc-size(auto, size)))"));
    try$(_expectInvalid(_driver, "width", "calc(cos(calc-size(auto, 0px)))"));
    try$(_expectInvalid(_driver, "width", "calc(atan2(calc-size(auto, size), calc-size(auto, 50px)))"));
    try$(_expectInvalid(_driver, "width", "calc(sqrt(calc-size(auto, 0px)))"));
    try$(_expectInvalid(_driver, "width", "calc(hypot(calc-size(auto, size * 0.5), calc-size(auto, size)))"));
    try$(_expectInvalid(_driver, "width", "calc(round(calc-size(auto, size * 0.5), calc-size(auto, size)))"));
    try$(_expectInvalid(_driver, "width", "calc(round(calc-size(auto, size * 0.5)))"));
    try$(_expectInvalid(_driver, "width", "calc(mod(calc-size(auto, size * 0.5), calc-size(auto, size)))"));
    try$(_expectInvalid(_driver, "width", "calc(rem(calc-size(auto, size * 0.5), calc-size(auto, size)))"));
    try$(_expectInvalid(_driver, "height", "calc(12% + calc-size(any, 31%))"));
    try$(_expectInvalid(_driver, "width", "calc-size(30px)"));
    try$(_expectInvalid(_driver, "width", "calc-size(min(30px, 2em))"));
    try$(_expectInvalid(_driver, "width", "calc-size(any)"));
    try$(_expectInvalid(_driver, "width", "calc-size(calc-size(any, 30px))"));
    try$(_expectInvalid(_driver, "width", "calc-size(size)"));
    try$(_expectInvalid(_driver, "width", "calc-size(fit-content)"));
    try$(_expectInvalid(_driver, "width", "calc-size(calc-size(fit-content, size * 2))"));
    try$(_expectInvalid(_driver, "width", "calc-size(calc-size(30px))"));
    try$(_expectInvalid(_driver, "width", "calc-size(content, size)"));
    try$(_expectInvalid(_driver, "height", "calc-size(content, size)"));
    try$(_expectInvalid(_driver, "max-width", "calc-size(content, size)"));
    try$(_expectInvalid(_driver, "max-height", "calc-size(content, size)"));
    try$(_expectInvalid(_driver, "min-width", "calc-size(content, size)"));
    try$(_expectInvalid(_driver, "min-height", "calc-size(content, size)"));
    try$(_expectInvalid(_driver, "width", "calc-size(0, 0px)"));
    try$(_expectInvalid(_driver, "width", "calc-size(0px, 0)"));
    try$(_expectInvalid(_driver, "width", "calc-size(0, 0)"));
    return Ok();
}

test$("vaev-style-wpt-css-css-values-calc-size-calc-size-parsing-xfail", XFAIL) {
    try$(_expectSerialized(_driver, "width", "calc-size(auto, size)", "calc-size(auto, size)"));
    try$(_expectSerialized(_driver, "min-width", "calc-size(auto, size)", "calc-size(auto, size)"));
    try$(_expectSerialized(_driver, "height", "calc-size(auto, size)", "calc-size(auto, size)"));
    try$(_expectSerialized(_driver, "min-height", "calc-size(auto, size)", "calc-size(auto, size)"));
    try$(_expectSerialized(_driver, "width", "calc-size(max-content, size)", "calc-size(max-content, size)"));
    try$(_expectSerialized(_driver, "height", "calc-size(max-content, size)", "calc-size(max-content, size)"));
    try$(_expectSerialized(_driver, "max-width", "calc-size(max-content, size)", "calc-size(max-content, size)"));
    try$(_expectSerialized(_driver, "max-height", "calc-size(max-content, size)", "calc-size(max-content, size)"));
    try$(_expectSerialized(_driver, "min-width", "calc-size(max-content, size)", "calc-size(max-content, size)"));
    try$(_expectSerialized(_driver, "min-height", "calc-size(max-content, size)", "calc-size(max-content, size)"));
    try$(_expectSerialized(_driver, "width", "calc-size(fit-content, size)", "calc-size(fit-content, size)"));
    try$(_expectSerialized(_driver, "height", "calc-size(min-content, size * 2)", "calc-size(min-content, 2 * size)"));
    try$(_expectSerialized(_driver, "max-width", "calc-size(max-content, size / 2)", "calc-size(max-content, 0.5 * size)"));
    try$(_expectSerialized(_driver, "max-height", "calc-size(fit-content, 30px + size / 2)", "calc-size(fit-content, 30px + (0.5 * size))"));
    try$(_expectSerialized(_driver, "width", "calc-size(fit-content, 50% + size / 2)", "calc-size(fit-content, 50% + (0.5 * size))"));
    try$(_expectSerialized(_driver, "width", "calc-size(any, 25em)", "calc-size(any, 25em)"));
    try$(_expectSerialized(_driver, "width", "calc-size(any, 40%)", "calc-size(any, 40%)"));
    try$(_expectSerialized(_driver, "width", "calc-size(any, 50px + 30%)", "calc-size(any, 30% + 50px)"));
    try$(_expectSerialized(_driver, "width", "calc-size(calc-size(any, 30px), size)", "calc-size(calc-size(any, 30px), size)"));
    try$(_expectSerialized(_driver, "width", "calc-size(10px, sign(size) * size)", "calc-size(10px, sign(size) * size)"));
    try$(_expectSerialized(_driver, "width", "calc-size(30px, 25em)", "calc-size(30px, 25em)"));
    try$(_expectSerialized(_driver, "width", "calc-size(calc-size(any, 30px), 25em)", "calc-size(calc-size(any, 30px), 25em)"));
    try$(_expectSerialized(_driver, "width", "calc-size(calc-size(2in, 30px), 25em)", "calc-size(calc-size(192px, 30px), 25em)"));
    try$(_expectSerialized(_driver, "width", "calc-size(calc-size(min-content, 30px), 25em)", "calc-size(calc-size(min-content, 30px), 25em)"));
    try$(_expectSerialized(_driver, "width", "calc-size(calc-size(min-content, size), size)", "calc-size(calc-size(min-content, size), size)"));
    try$(_expectSerialized(_driver, "flex-basis", "calc-size(any, 50px)", "calc-size(any, 50px)"));
    try$(_expectSerialized(_driver, "flex-basis", "calc-size(auto, size)", "calc-size(auto, size)"));
    try$(_expectSerialized(_driver, "flex-basis", "calc-size(min-content, size)", "calc-size(min-content, size)"));
    try$(_expectSerialized(_driver, "flex-basis", "calc-size(max-content, size)", "calc-size(max-content, size)"));
    try$(_expectSerialized(_driver, "flex-basis", "calc-size(fit-content, size)", "calc-size(fit-content, size)"));
    try$(_expectSerialized(_driver, "flex-basis", "calc-size(content, size)", "calc-size(content, size)"));
    try$(_expectSerialized(_driver, "width", "calc-size(0px, 0px)", "calc-size(0px, 0px)"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-values/clamp-color-invalid.html
test$("vaev-style-wpt-css-css-values-clamp-color-invalid") {
    try$(_expectInvalid(_driver, "color", "rgb(clamp(10, -none, 20) 0 0)"));
    try$(_expectInvalid(_driver, "color", "rgb(clamp(10, none, 20) 0 0)"));
    try$(_expectInvalid(_driver, "color", "rgb(clamp(10, 15 + none, 20) 0 0)"));
    try$(_expectInvalid(_driver, "color", "rgb(clamp(none + 10, 15, 20) 0 0)"));
    try$(_expectInvalid(_driver, "color", "rgb(clamp(none / 10, 15, 20) 0 0)"));
    try$(_expectInvalid(_driver, "color", "rgb(clamp(-none, 15, 20) 0 0)"));
    try$(_expectInvalid(_driver, "color", "rgb(clamp(-none, -none, -none) 0 0)"));
    try$(_expectInvalid(_driver, "color", "rgb(clamp(10, abs(none), 20) 0 0)"));
    try$(_expectInvalid(_driver, "color", "rgb(clamp(10, 15, abs(none)) 0 0)"));
    try$(_expectInvalid(_driver, "color", "rgb(clamp(10, abs(none + 15), 20) 0 0)"));
    try$(_expectInvalid(_driver, "color", "rgb(clamp(10, sign(none), 20) 0 0)"));
    try$(_expectInvalid(_driver, "color", "rgb(clamp(10, sign(none + 1), 20) 0 0)"));
    try$(_expectInvalid(_driver, "color", "rgb(clamp(10, progress(none, none, none), 20) 0 0)"));
    try$(_expectInvalid(_driver, "color", "rgb(clamp(10, progress(none + 1, none + 1, none + 1), 20) 0 0)"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-values/clamp-integer-invalid.html
test$("vaev-style-wpt-css-css-values-clamp-integer-invalid") {
    try$(_expectInvalid(_driver, "z-index", "clamp(10, -none, 20)"));
    try$(_expectInvalid(_driver, "z-index", "clamp(10, none, 20)"));
    try$(_expectInvalid(_driver, "z-index", "clamp(10, 15 + none, 20)"));
    try$(_expectInvalid(_driver, "z-index", "clamp(none + 10, 15, 20)"));
    try$(_expectInvalid(_driver, "z-index", "clamp(none / 10, 15, 20)"));
    try$(_expectInvalid(_driver, "z-index", "clamp(-none, 15, 20)"));
    try$(_expectInvalid(_driver, "z-index", "clamp(-none, -none, -none)"));
    try$(_expectInvalid(_driver, "z-index", "clamp(10, abs(none), 20)"));
    try$(_expectInvalid(_driver, "z-index", "clamp(10, 15, abs(none))"));
    try$(_expectInvalid(_driver, "z-index", "clamp(10, abs(none + 15), 20)"));
    try$(_expectInvalid(_driver, "z-index", "clamp(10, sign(none), 20)"));
    try$(_expectInvalid(_driver, "z-index", "clamp(10, sign(none + 1), 20)"));
    try$(_expectInvalid(_driver, "z-index", "clamp(10, progress(none, none, none), 20)"));
    try$(_expectInvalid(_driver, "z-index", "clamp(10, progress(none + 1, none + 1, none + 1), 20)"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-values/exp-log-invalid.html
test$("vaev-style-wpt-css-css-values-exp-log-invalid") {
    try$(_expectInvalid(_driver, "opacity", "exp()"));
    try$(_expectInvalid(_driver, "opacity", "exp( )"));
    try$(_expectInvalid(_driver, "opacity", "exp(,)"));
    try$(_expectInvalid(_driver, "opacity", "exp(1, )"));
    try$(_expectInvalid(_driver, "opacity", "exp(, 1)"));
    try$(_expectInvalid(_driver, "opacity", "exp(1 + )"));
    try$(_expectInvalid(_driver, "opacity", "exp(1 - )"));
    try$(_expectInvalid(_driver, "opacity", "exp(1 * )"));
    try$(_expectInvalid(_driver, "opacity", "exp(1 / )"));
    try$(_expectInvalid(_driver, "opacity", "exp(1 2)"));
    try$(_expectInvalid(_driver, "opacity", "exp(1, , 2)"));
    try$(_expectInvalid(_driver, "opacity", "log()"));
    try$(_expectInvalid(_driver, "opacity", "log( )"));
    try$(_expectInvalid(_driver, "opacity", "log(,)"));
    try$(_expectInvalid(_driver, "opacity", "log(1, )"));
    try$(_expectInvalid(_driver, "opacity", "log(, 1)"));
    try$(_expectInvalid(_driver, "opacity", "log(1 + )"));
    try$(_expectInvalid(_driver, "opacity", "log(1 - )"));
    try$(_expectInvalid(_driver, "opacity", "log(1 * )"));
    try$(_expectInvalid(_driver, "opacity", "log(1 / )"));
    try$(_expectInvalid(_driver, "opacity", "log(1 2)"));
    try$(_expectInvalid(_driver, "opacity", "log(1, , 2)"));
    try$(_expectInvalid(_driver, "opacity", "exp(0px)"));
    try$(_expectInvalid(_driver, "opacity", "exp(0s)"));
    try$(_expectInvalid(_driver, "opacity", "exp(0deg)"));
    try$(_expectInvalid(_driver, "opacity", "exp(0Hz)"));
    try$(_expectInvalid(_driver, "opacity", "exp(0dpi)"));
    try$(_expectInvalid(_driver, "opacity", "exp(0fr)"));
    try$(_expectInvalid(_driver, "opacity", "exp(1, 1%)"));
    try$(_expectInvalid(_driver, "opacity", "exp(1, 0px)"));
    try$(_expectInvalid(_driver, "opacity", "exp(1, 0s)"));
    try$(_expectInvalid(_driver, "opacity", "exp(1, 0deg)"));
    try$(_expectInvalid(_driver, "opacity", "exp(1, 0Hz)"));
    try$(_expectInvalid(_driver, "opacity", "exp(1, 0dpi)"));
    try$(_expectInvalid(_driver, "opacity", "exp(1, 0fr)"));
    try$(_expectInvalid(_driver, "opacity", "log(0px)"));
    try$(_expectInvalid(_driver, "opacity", "log(0s)"));
    try$(_expectInvalid(_driver, "opacity", "log(0deg)"));
    try$(_expectInvalid(_driver, "opacity", "log(0Hz)"));
    try$(_expectInvalid(_driver, "opacity", "log(0dpi)"));
    try$(_expectInvalid(_driver, "opacity", "log(0fr)"));
    try$(_expectInvalid(_driver, "opacity", "log(1, 1%)"));
    try$(_expectInvalid(_driver, "opacity", "log(1, 0px)"));
    try$(_expectInvalid(_driver, "opacity", "log(1, 0s)"));
    try$(_expectInvalid(_driver, "opacity", "log(1, 0deg)"));
    try$(_expectInvalid(_driver, "opacity", "log(1, 0Hz)"));
    try$(_expectInvalid(_driver, "opacity", "log(1, 0dpi)"));
    try$(_expectInvalid(_driver, "opacity", "log(1, 0fr)"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-values/hypot-pow-sqrt-invalid.html
test$("vaev-style-wpt-css-css-values-hypot-pow-sqrt-invalid") {
    try$(_expectInvalid(_driver, "opacity", "hypot()"));
    try$(_expectInvalid(_driver, "opacity", "hypot( )"));
    try$(_expectInvalid(_driver, "opacity", "hypot(,)"));
    try$(_expectInvalid(_driver, "opacity", "hypot(1, )"));
    try$(_expectInvalid(_driver, "opacity", "hypot(, 1)"));
    try$(_expectInvalid(_driver, "opacity", "hypot(1 + )"));
    try$(_expectInvalid(_driver, "opacity", "hypot(1 - )"));
    try$(_expectInvalid(_driver, "opacity", "hypot(1 * )"));
    try$(_expectInvalid(_driver, "opacity", "hypot(1 / )"));
    try$(_expectInvalid(_driver, "opacity", "hypot(1 2)"));
    try$(_expectInvalid(_driver, "opacity", "hypot(1, , 2)"));
    try$(_expectInvalid(_driver, "opacity", "sqrt()"));
    try$(_expectInvalid(_driver, "opacity", "sqrt( )"));
    try$(_expectInvalid(_driver, "opacity", "sqrt(,)"));
    try$(_expectInvalid(_driver, "opacity", "sqrt(1, )"));
    try$(_expectInvalid(_driver, "opacity", "sqrt(, 1)"));
    try$(_expectInvalid(_driver, "opacity", "sqrt(1 + )"));
    try$(_expectInvalid(_driver, "opacity", "sqrt(1 - )"));
    try$(_expectInvalid(_driver, "opacity", "sqrt(1 * )"));
    try$(_expectInvalid(_driver, "opacity", "sqrt(1 / )"));
    try$(_expectInvalid(_driver, "opacity", "sqrt(1 2)"));
    try$(_expectInvalid(_driver, "opacity", "sqrt(1, , 2)"));
    try$(_expectInvalid(_driver, "opacity", "sqrt(1, 2)"));
    try$(_expectInvalid(_driver, "opacity", "pow( )"));
    try$(_expectInvalid(_driver, "opacity", "pow(,)"));
    try$(_expectInvalid(_driver, "opacity", "pow(1, )"));
    try$(_expectInvalid(_driver, "opacity", "pow(, 1)"));
    try$(_expectInvalid(_driver, "opacity", "pow(1 + )"));
    try$(_expectInvalid(_driver, "opacity", "pow(1 - )"));
    try$(_expectInvalid(_driver, "opacity", "pow(1 * )"));
    try$(_expectInvalid(_driver, "opacity", "pow(1 / )"));
    try$(_expectInvalid(_driver, "opacity", "pow(1 2)"));
    try$(_expectInvalid(_driver, "opacity", "pow(1, , 2)"));
    try$(_expectInvalid(_driver, "opacity", "pow(2px, 2)"));
    try$(_expectInvalid(_driver, "opacity", "pow(10, 1px)"));
    try$(_expectInvalid(_driver, "outline-offset", "calc(1px * pow(1))"));
    try$(_expectInvalid(_driver, "outline-offset", "calc(1px * pow(2px, 3px))"));
    try$(_expectInvalid(_driver, "outline-offset", "calc(sqrt(100px)"));
    try$(_expectInvalid(_driver, "outline-offset", "hypot(2px, 3)"));
    try$(_expectInvalid(_driver, "outline-offset", "hypot(3, ,4)"));
    try$(_expectInvalid(_driver, "outline-offset", "hypot(1, 2)"));
    try$(_expectInvalid(_driver, "outline-offset", "calc(1px * pow(2 3))"));
    try$(_expectInvalid(_driver, "outline-offset", "hypot()"));
    try$(_expectInvalid(_driver, "outline-offset", "calc(pow(2))"));
    try$(_expectInvalid(_driver, "outline-offset", "pow())"));
    try$(_expectInvalid(_driver, "outline-offset", "pow(1, 2)"));
    try$(_expectInvalid(_driver, "outline-offset", "calc(sqrt())"));
    try$(_expectInvalid(_driver, "outline-offset", "calc(sqrt(100, 200))"));
    try$(_expectInvalid(_driver, "outline-offset", "pow(10px, 1)"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-values/inherit-function-parsing.html
test$("vaev-style-wpt-css-css-values-inherit-function-parsing") {
    try$(_expectInvalid(_driver, "left", "inherit(!!, foo)"));
    try$(_expectInvalid(_driver, "left", "inherit(, foo)"));
    return Ok();
}

test$("vaev-style-wpt-css-css-values-inherit-function-parsing-xfail", XFAIL) {
    try$(_expectSerialized(_driver, "left", "inherit(--x)", "inherit(--x)"));
    try$(_expectSerialized(_driver, "left", "calc(inherit(--x) + 1px)", "calc(inherit(--x) + 1px)"));
    try$(_expectSerialized(_driver, "left", "inherit(--x,)", "inherit(--x,)"));
    try$(_expectSerialized(_driver, "left", "inherit(--x, )", "inherit(--x, )"));
    try$(_expectSerialized(_driver, "left", "inherit(--x , )", "inherit(--x , )"));
    try$(_expectSerialized(_driver, "left", "inherit(--x, foo)", "inherit(--x, foo)"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-values/minmax-angle-invalid.html
test$("vaev-style-wpt-css-css-values-minmax-angle-invalid") {
    try$(_expectInvalid(_driver, "transform", "rotate(min())"));
    try$(_expectInvalid(_driver, "transform", "rotate(min( ))"));
    try$(_expectInvalid(_driver, "transform", "rotate(min(,))"));
    try$(_expectInvalid(_driver, "transform", "rotate(min(1dag))"));
    try$(_expectInvalid(_driver, "transform", "rotate(min(1deg, ))"));
    try$(_expectInvalid(_driver, "transform", "rotate(min(, 1deg))"));
    try$(_expectInvalid(_driver, "transform", "rotate(min(1deg + ))"));
    try$(_expectInvalid(_driver, "transform", "rotate(min(1deg - ))"));
    try$(_expectInvalid(_driver, "transform", "rotate(min(1deg * ))"));
    try$(_expectInvalid(_driver, "transform", "rotate(min(1deg / ))"));
    try$(_expectInvalid(_driver, "transform", "rotate(min(1deg 2deg))"));
    try$(_expectInvalid(_driver, "transform", "rotate(min(1deg, , 2deg))"));
    try$(_expectInvalid(_driver, "transform", "rotate(max())"));
    try$(_expectInvalid(_driver, "transform", "rotate(max( ))"));
    try$(_expectInvalid(_driver, "transform", "rotate(max(,))"));
    try$(_expectInvalid(_driver, "transform", "rotate(max(1dag))"));
    try$(_expectInvalid(_driver, "transform", "rotate(max(1deg, ))"));
    try$(_expectInvalid(_driver, "transform", "rotate(max(, 1deg))"));
    try$(_expectInvalid(_driver, "transform", "rotate(max(1deg + ))"));
    try$(_expectInvalid(_driver, "transform", "rotate(max(1deg - ))"));
    try$(_expectInvalid(_driver, "transform", "rotate(max(1deg * ))"));
    try$(_expectInvalid(_driver, "transform", "rotate(max(1deg / ))"));
    try$(_expectInvalid(_driver, "transform", "rotate(max(1deg 2deg))"));
    try$(_expectInvalid(_driver, "transform", "rotate(max(1deg, , 2deg))"));
    try$(_expectInvalid(_driver, "transform", "rotate(min(0))"));
    try$(_expectInvalid(_driver, "transform", "rotate(min(0%))"));
    try$(_expectInvalid(_driver, "transform", "rotate(min(0px))"));
    try$(_expectInvalid(_driver, "transform", "rotate(min(0s))"));
    try$(_expectInvalid(_driver, "transform", "rotate(min(0Hz))"));
    try$(_expectInvalid(_driver, "transform", "rotate(min(0dpi))"));
    try$(_expectInvalid(_driver, "transform", "rotate(min(0fr))"));
    try$(_expectInvalid(_driver, "transform", "rotate(min(1deg, 0))"));
    try$(_expectInvalid(_driver, "transform", "rotate(min(1deg, 0%))"));
    try$(_expectInvalid(_driver, "transform", "rotate(min(1deg, 0px))"));
    try$(_expectInvalid(_driver, "transform", "rotate(min(1deg, 0s))"));
    try$(_expectInvalid(_driver, "transform", "rotate(min(1deg, 0Hz))"));
    try$(_expectInvalid(_driver, "transform", "rotate(min(1deg, 0dpi))"));
    try$(_expectInvalid(_driver, "transform", "rotate(min(1deg, 0fr))"));
    try$(_expectInvalid(_driver, "transform", "rotate(max(0))"));
    try$(_expectInvalid(_driver, "transform", "rotate(max(0%))"));
    try$(_expectInvalid(_driver, "transform", "rotate(max(0px))"));
    try$(_expectInvalid(_driver, "transform", "rotate(max(0s))"));
    try$(_expectInvalid(_driver, "transform", "rotate(max(0Hz))"));
    try$(_expectInvalid(_driver, "transform", "rotate(max(0dpi))"));
    try$(_expectInvalid(_driver, "transform", "rotate(max(0fr))"));
    try$(_expectInvalid(_driver, "transform", "rotate(max(1deg, 0))"));
    try$(_expectInvalid(_driver, "transform", "rotate(max(1deg, 0%))"));
    try$(_expectInvalid(_driver, "transform", "rotate(max(1deg, 0px))"));
    try$(_expectInvalid(_driver, "transform", "rotate(max(1deg, 0s))"));
    try$(_expectInvalid(_driver, "transform", "rotate(max(1deg, 0Hz))"));
    try$(_expectInvalid(_driver, "transform", "rotate(max(1deg, 0dpi))"));
    try$(_expectInvalid(_driver, "transform", "rotate(max(1deg, 0fr))"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-values/minmax-length-invalid.html
test$("vaev-style-wpt-css-css-values-minmax-length-invalid") {
    try$(_expectInvalid(_driver, "border-left-width", "min()"));
    try$(_expectInvalid(_driver, "border-left-width", "min( )"));
    try$(_expectInvalid(_driver, "border-left-width", "min(,)"));
    try$(_expectInvalid(_driver, "border-left-width", "min(1py)"));
    try$(_expectInvalid(_driver, "border-left-width", "min(1px, )"));
    try$(_expectInvalid(_driver, "border-left-width", "min(, 1px)"));
    try$(_expectInvalid(_driver, "border-left-width", "min(1px + )"));
    try$(_expectInvalid(_driver, "border-left-width", "min(1px - )"));
    try$(_expectInvalid(_driver, "border-left-width", "min(1px * )"));
    try$(_expectInvalid(_driver, "border-left-width", "min(1px / )"));
    try$(_expectInvalid(_driver, "border-left-width", "min(1px 2px)"));
    try$(_expectInvalid(_driver, "border-left-width", "min(1px, , 2px)"));
    try$(_expectInvalid(_driver, "border-left-width", "max()"));
    try$(_expectInvalid(_driver, "border-left-width", "max( )"));
    try$(_expectInvalid(_driver, "border-left-width", "max(,)"));
    try$(_expectInvalid(_driver, "border-left-width", "max(1py)"));
    try$(_expectInvalid(_driver, "border-left-width", "max(1px, )"));
    try$(_expectInvalid(_driver, "border-left-width", "max(, 1px)"));
    try$(_expectInvalid(_driver, "border-left-width", "max(1px + )"));
    try$(_expectInvalid(_driver, "border-left-width", "max(1px - )"));
    try$(_expectInvalid(_driver, "border-left-width", "max(1px * )"));
    try$(_expectInvalid(_driver, "border-left-width", "max(1px / )"));
    try$(_expectInvalid(_driver, "border-left-width", "max(1px 2px)"));
    try$(_expectInvalid(_driver, "border-left-width", "max(1px, , 2px)"));
    try$(_expectInvalid(_driver, "border-left-width", "min(0)"));
    try$(_expectInvalid(_driver, "border-left-width", "min(0%)"));
    try$(_expectInvalid(_driver, "border-left-width", "min(0s)"));
    try$(_expectInvalid(_driver, "border-left-width", "min(0Hz)"));
    try$(_expectInvalid(_driver, "border-left-width", "min(0dpi)"));
    try$(_expectInvalid(_driver, "border-left-width", "min(0fr)"));
    try$(_expectInvalid(_driver, "border-left-width", "min(1px, 0)"));
    try$(_expectInvalid(_driver, "border-left-width", "min(1px, 0%)"));
    try$(_expectInvalid(_driver, "border-left-width", "min(1px, 0s)"));
    try$(_expectInvalid(_driver, "border-left-width", "min(1px, 0Hz)"));
    try$(_expectInvalid(_driver, "border-left-width", "min(1px, 0dpi)"));
    try$(_expectInvalid(_driver, "border-left-width", "min(1px, 0fr)"));
    try$(_expectInvalid(_driver, "border-left-width", "max(0)"));
    try$(_expectInvalid(_driver, "border-left-width", "max(0%)"));
    try$(_expectInvalid(_driver, "border-left-width", "max(0s)"));
    try$(_expectInvalid(_driver, "border-left-width", "max(0Hz)"));
    try$(_expectInvalid(_driver, "border-left-width", "max(0dpi)"));
    try$(_expectInvalid(_driver, "border-left-width", "max(0fr)"));
    try$(_expectInvalid(_driver, "border-left-width", "max(1px, 0)"));
    try$(_expectInvalid(_driver, "border-left-width", "max(1px, 0%)"));
    try$(_expectInvalid(_driver, "border-left-width", "max(1px, 0s)"));
    try$(_expectInvalid(_driver, "border-left-width", "max(1px, 0Hz)"));
    try$(_expectInvalid(_driver, "border-left-width", "max(1px, 0dpi)"));
    try$(_expectInvalid(_driver, "border-left-width", "max(1px, 0fr)"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-values/minmax-length-percent-invalid.html
test$("vaev-style-wpt-css-css-values-minmax-length-percent-invalid") {
    try$(_expectInvalid(_driver, "margin-left", "min(1px, 0)"));
    try$(_expectInvalid(_driver, "margin-left", "min(1px, 0s)"));
    try$(_expectInvalid(_driver, "margin-left", "min(1px, 0Hz)"));
    try$(_expectInvalid(_driver, "margin-left", "min(1px, 0dpi)"));
    try$(_expectInvalid(_driver, "margin-left", "min(1px, 0fr)"));
    try$(_expectInvalid(_driver, "margin-left", "min(1%, 0)"));
    try$(_expectInvalid(_driver, "margin-left", "min(1%, 0s)"));
    try$(_expectInvalid(_driver, "margin-left", "min(1%, 0Hz)"));
    try$(_expectInvalid(_driver, "margin-left", "min(1%, 0dpi)"));
    try$(_expectInvalid(_driver, "margin-left", "min(1%, 0fr)"));
    try$(_expectInvalid(_driver, "margin-left", "max(1px, 0)"));
    try$(_expectInvalid(_driver, "margin-left", "max(1px, 0s)"));
    try$(_expectInvalid(_driver, "margin-left", "max(1px, 0Hz)"));
    try$(_expectInvalid(_driver, "margin-left", "max(1px, 0dpi)"));
    try$(_expectInvalid(_driver, "margin-left", "max(1px, 0fr)"));
    try$(_expectInvalid(_driver, "margin-left", "max(1%, 0)"));
    try$(_expectInvalid(_driver, "margin-left", "max(1%, 0s)"));
    try$(_expectInvalid(_driver, "margin-left", "max(1%, 0Hz)"));
    try$(_expectInvalid(_driver, "margin-left", "max(1%, 0dpi)"));
    try$(_expectInvalid(_driver, "margin-left", "max(1%, 0fr)"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-values/minmax-number-invalid.html
test$("vaev-style-wpt-css-css-values-minmax-number-invalid") {
    try$(_expectInvalid(_driver, "opacity", "min()"));
    try$(_expectInvalid(_driver, "opacity", "min( )"));
    try$(_expectInvalid(_driver, "opacity", "min(,)"));
    try$(_expectInvalid(_driver, "opacity", "min(1, )"));
    try$(_expectInvalid(_driver, "opacity", "min(, 1)"));
    try$(_expectInvalid(_driver, "opacity", "min(1 + )"));
    try$(_expectInvalid(_driver, "opacity", "min(1 - )"));
    try$(_expectInvalid(_driver, "opacity", "min(1 * )"));
    try$(_expectInvalid(_driver, "opacity", "min(1 / )"));
    try$(_expectInvalid(_driver, "opacity", "min(1 2)"));
    try$(_expectInvalid(_driver, "opacity", "min(1, , 2)"));
    try$(_expectInvalid(_driver, "opacity", "max()"));
    try$(_expectInvalid(_driver, "opacity", "max( )"));
    try$(_expectInvalid(_driver, "opacity", "max(,)"));
    try$(_expectInvalid(_driver, "opacity", "max(1, )"));
    try$(_expectInvalid(_driver, "opacity", "max(, 1)"));
    try$(_expectInvalid(_driver, "opacity", "max(1 + )"));
    try$(_expectInvalid(_driver, "opacity", "max(1 - )"));
    try$(_expectInvalid(_driver, "opacity", "max(1 * )"));
    try$(_expectInvalid(_driver, "opacity", "max(1 / )"));
    try$(_expectInvalid(_driver, "opacity", "max(1 2)"));
    try$(_expectInvalid(_driver, "opacity", "max(1, , 2)"));
    try$(_expectInvalid(_driver, "opacity", "min(0px)"));
    try$(_expectInvalid(_driver, "opacity", "min(0s)"));
    try$(_expectInvalid(_driver, "opacity", "min(0deg)"));
    try$(_expectInvalid(_driver, "opacity", "min(0Hz)"));
    try$(_expectInvalid(_driver, "opacity", "min(0dpi)"));
    try$(_expectInvalid(_driver, "opacity", "min(0fr)"));
    try$(_expectInvalid(_driver, "opacity", "min(1, 1%)"));
    try$(_expectInvalid(_driver, "opacity", "min(1, 0px)"));
    try$(_expectInvalid(_driver, "opacity", "min(1, 0s)"));
    try$(_expectInvalid(_driver, "opacity", "min(1, 0deg)"));
    try$(_expectInvalid(_driver, "opacity", "min(1, 0Hz)"));
    try$(_expectInvalid(_driver, "opacity", "min(1, 0dpi)"));
    try$(_expectInvalid(_driver, "opacity", "min(1, 0fr)"));
    try$(_expectInvalid(_driver, "opacity", "max(0px)"));
    try$(_expectInvalid(_driver, "opacity", "max(0s)"));
    try$(_expectInvalid(_driver, "opacity", "max(0deg)"));
    try$(_expectInvalid(_driver, "opacity", "max(0Hz)"));
    try$(_expectInvalid(_driver, "opacity", "max(0dpi)"));
    try$(_expectInvalid(_driver, "opacity", "max(0fr)"));
    try$(_expectInvalid(_driver, "opacity", "max(1, 1%)"));
    try$(_expectInvalid(_driver, "opacity", "max(1, 0px)"));
    try$(_expectInvalid(_driver, "opacity", "max(1, 0s)"));
    try$(_expectInvalid(_driver, "opacity", "max(1, 0deg)"));
    try$(_expectInvalid(_driver, "opacity", "max(1, 0Hz)"));
    try$(_expectInvalid(_driver, "opacity", "max(1, 0dpi)"));
    try$(_expectInvalid(_driver, "opacity", "max(1, 0fr)"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-values/minmax-percentage-invalid.html
test$("vaev-style-wpt-css-css-values-minmax-percentage-invalid") {
    try$(_expectInvalid(_driver, "margin-left", "min()"));
    try$(_expectInvalid(_driver, "margin-left", "min( )"));
    try$(_expectInvalid(_driver, "margin-left", "min(,)"));
    try$(_expectInvalid(_driver, "margin-left", "min(1#)"));
    try$(_expectInvalid(_driver, "margin-left", "min(%1)"));
    try$(_expectInvalid(_driver, "margin-left", "min(1%, )"));
    try$(_expectInvalid(_driver, "margin-left", "min(, 1%)"));
    try$(_expectInvalid(_driver, "margin-left", "min(1% + )"));
    try$(_expectInvalid(_driver, "margin-left", "min(1% - )"));
    try$(_expectInvalid(_driver, "margin-left", "min(1% * )"));
    try$(_expectInvalid(_driver, "margin-left", "min(1% / )"));
    try$(_expectInvalid(_driver, "margin-left", "min(1% 2%)"));
    try$(_expectInvalid(_driver, "margin-left", "min(1%, , 2%)"));
    try$(_expectInvalid(_driver, "margin-left", "max()"));
    try$(_expectInvalid(_driver, "margin-left", "max( )"));
    try$(_expectInvalid(_driver, "margin-left", "max(,)"));
    try$(_expectInvalid(_driver, "margin-left", "max(1#)"));
    try$(_expectInvalid(_driver, "margin-left", "max(%1)"));
    try$(_expectInvalid(_driver, "margin-left", "max(1%, )"));
    try$(_expectInvalid(_driver, "margin-left", "max(, 1%)"));
    try$(_expectInvalid(_driver, "margin-left", "max(1% + )"));
    try$(_expectInvalid(_driver, "margin-left", "max(1% - )"));
    try$(_expectInvalid(_driver, "margin-left", "max(1% * )"));
    try$(_expectInvalid(_driver, "margin-left", "max(1% / )"));
    try$(_expectInvalid(_driver, "margin-left", "max(1% 2%)"));
    try$(_expectInvalid(_driver, "margin-left", "max(1%, , 2%)"));
    try$(_expectInvalid(_driver, "margin-left", "min(0s)"));
    try$(_expectInvalid(_driver, "margin-left", "min(0deg)"));
    try$(_expectInvalid(_driver, "margin-left", "min(0Hz)"));
    try$(_expectInvalid(_driver, "margin-left", "min(0dpi)"));
    try$(_expectInvalid(_driver, "margin-left", "min(0fr)"));
    try$(_expectInvalid(_driver, "margin-left", "min(1%, 0)"));
    try$(_expectInvalid(_driver, "margin-left", "min(1%, 0s)"));
    try$(_expectInvalid(_driver, "margin-left", "min(1%, 0deg)"));
    try$(_expectInvalid(_driver, "margin-left", "min(1%, 0Hz)"));
    try$(_expectInvalid(_driver, "margin-left", "min(1%, 0dpi)"));
    try$(_expectInvalid(_driver, "margin-left", "min(1%, 0fr)"));
    try$(_expectInvalid(_driver, "margin-left", "max(0s)"));
    try$(_expectInvalid(_driver, "margin-left", "max(0deg)"));
    try$(_expectInvalid(_driver, "margin-left", "max(0Hz)"));
    try$(_expectInvalid(_driver, "margin-left", "max(0dpi)"));
    try$(_expectInvalid(_driver, "margin-left", "max(0fr)"));
    try$(_expectInvalid(_driver, "margin-left", "max(1%, 0)"));
    try$(_expectInvalid(_driver, "margin-left", "max(1%, 0s)"));
    try$(_expectInvalid(_driver, "margin-left", "max(1%, 0deg)"));
    try$(_expectInvalid(_driver, "margin-left", "max(1%, 0Hz)"));
    try$(_expectInvalid(_driver, "margin-left", "max(1%, 0dpi)"));
    try$(_expectInvalid(_driver, "margin-left", "max(1%, 0fr)"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-values/percentage-without-context.html
test$("vaev-style-wpt-css-css-values-percentage-without-context") {
    try$(_expectInvalid(_driver, "font-style", "oblique calc(sign(50%) * 1deg)"));
    try$(_expectInvalid(_driver, "background", "linear-gradient(calc(sign(50%) * 1turn), #3f87a6, #ebf8e1, #f69d3c)"));
    try$(_expectInvalid(_driver, "background", "conic-gradient(from calc(sign(50%) * 1rad) at 10% 50%, #e66465, #9198e5)"));
    try$(_expectInvalid(_driver, "color", "hsl(calc(sign(50%) * 1deg) 82% 43%)"));
    try$(_expectInvalid(_driver, "transform", "rotate(calc(sign(50%) * 1deg))"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-values/progress-invalid.html
test$("vaev-style-wpt-css-css-values-progress-invalid") {
    try$(_expectInvalid(_driver, "opacity", "progress()"));
    try$(_expectInvalid(_driver, "opacity", "progress( )"));
    try$(_expectInvalid(_driver, "opacity", "progress(,)"));
    try$(_expectInvalid(_driver, "opacity", "progress(1, )"));
    try$(_expectInvalid(_driver, "opacity", "progress(1)"));
    try$(_expectInvalid(_driver, "opacity", "progress(50%, 0)"));
    try$(_expectInvalid(_driver, "opacity", "progress(0, 1,)"));
    try$(_expectInvalid(_driver, "opacity", "progress(from,)"));
    try$(_expectInvalid(_driver, "opacity", "progress(from 1, 0)"));
    try$(_expectInvalid(_driver, "opacity", "progress(from 1, 0 1)"));
    try$(_expectInvalid(_driver, "opacity", "progress(from 1 0)"));
    try$(_expectInvalid(_driver, "opacity", "progress(0,, 0)"));
    try$(_expectInvalid(_driver, "opacity", "progress(to,,,,)"));
    try$(_expectInvalid(_driver, "opacity", "progress(0,,, 10,, 200)"));
    try$(_expectInvalid(_driver, "opacity", "progress(0,,, 10,,, 200)"));
    try$(_expectInvalid(_driver, "opacity", "progress(0,, 10,, 200)"));
    try$(_expectInvalid(_driver, "opacity", "progress(0 from 10 to 200)"));
    try$(_expectInvalid(_driver, "opacity", "progress(no-clamp)"));
    try$(_expectInvalid(_driver, "opacity", "progress(no-clamp 1)"));
    try$(_expectInvalid(_driver, "opacity", "progress(no-clamp 1, 0)"));
    try$(_expectInvalid(_driver, "opacity", "progress(no-clamp 1 0 1)"));
    try$(_expectInvalid(_driver, "opacity", "progress(no-clamp 1, 0 1)"));
    try$(_expectInvalid(_driver, "opacity", "progress(no-clamp, 1, 0 1)"));
    try$(_expectInvalid(_driver, "opacity", "progress(1 no-clamp, 0, 1)"));
    try$(_expectInvalid(_driver, "opacity", "progress(1, 0, 1 no-clamp)"));
    try$(_expectInvalid(_driver, "opacity", "progress(5%, 0, 8"));
    try$(_expectInvalid(_driver, "opacity", "progress(5%, 0deg, 8deg"));
    try$(_expectInvalid(_driver, "opacity", "progress(5, 0deg, 8deg"));
    try$(_expectInvalid(_driver, "opacity", "progress(5, 0%, 8deg"));
    try$(_expectInvalid(_driver, "opacity", "progress(5, 0%, sign(10px)"));
    try$(_expectInvalid(_driver, "opacity", "progress(5%, 0px, 10px"));
    try$(_expectInvalid(_driver, "opacity", "progress(10px * 10px, 10px * 10px, 10px * 10px)"));
    try$(_expectInvalid(_driver, "opacity", "progress(10px, 10px * 10px, 10px * 10px)"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-values/random-invalid.tentative.html
test$("vaev-style-wpt-css-css-values-random-invalid.tentative") {
    try$(_expectInvalid(_driver, "width", "random()"));
    try$(_expectInvalid(_driver, "width", "random( )"));
    try$(_expectInvalid(_driver, "width", "random(,)"));
    try$(_expectInvalid(_driver, "width", "random(1px, )"));
    try$(_expectInvalid(_driver, "width", "random(,1px)"));
    try$(_expectInvalid(_driver, "width", "random(1px)"));
    try$(_expectInvalid(_driver, "width", "random(1px,2px,)"));
    try$(_expectInvalid(_driver, "width", "random(1px,2px,1px,)"));
    try$(_expectInvalid(_driver, "width", "random(1px,2px,1px foo)"));
    try$(_expectInvalid(_driver, "width", "random(foo, 1px, 2px)"));
    try$(_expectInvalid(_driver, "width", "random(\"foo\", 1px, 2px)"));
    try$(_expectInvalid(_driver, "width", "random(\"--foo\", 1px, 2px)"));
    try$(_expectInvalid(_driver, "width", "random(element-shared foo, 1px, 2px)"));
    try$(_expectInvalid(_driver, "width", "random(element-shared 1px, 2px)"));
    try$(_expectInvalid(_driver, "width", "random(1px, 2px, 1px, element-shared)"));
    try$(_expectInvalid(_driver, "width", "random(--foo --bar, 1px, 2px)"));
    try$(_expectInvalid(_driver, "width", "random(fixed 0.5 element-shared, 1px, 2px)"));
    try$(_expectInvalid(_driver, "width", "random(fixed 0.5 auto, 1px, 2px)"));
    try$(_expectInvalid(_driver, "width", "random(fixed 0.5 --foo, 1px, 2px)"));
    try$(_expectInvalid(_driver, "width", "random(fixed 0.5px, 1px, 2px)"));
    try$(_expectInvalid(_driver, "width", "random(fixed 0.5%, 1px, 2px)"));
    try$(_expectInvalid(_driver, "width", "random(fixed -1, 1px, 2px)"));
    try$(_expectInvalid(_driver, "width", "random(10deg, 20deg)"));
    try$(_expectInvalid(_driver, "width", "random(element-scoped --foo --bar, 1px, 2px)"));
    try$(_expectInvalid(_driver, "width", "random(element-scoped uawidth, 1px, 2px)"));
    try$(_expectInvalid(_driver, "width", "random(foo element-scoped, 1px, 2px)"));
    try$(_expectInvalid(_driver, "width", "random(element-scoped 1px, 2px)"));
    try$(_expectInvalid(_driver, "width", "random(--foo element-scoped element-scoped, 1px, 2px)"));
    try$(_expectInvalid(_driver, "width", "random(property-scoped --foo ua-width, 1px, 2px)"));
    try$(_expectInvalid(_driver, "width", "random(property-scoped ua-width-1, 1px, 2px)"));
    try$(_expectInvalid(_driver, "width", "random(--foo element-scoped property-scoped element-scoped, 1px, 2px)"));
    try$(_expectInvalid(_driver, "width", "random(foo property-scoped, 1px, 2px)"));
    try$(_expectInvalid(_driver, "width", "random(property-scoped 1px, 2px)"));
    try$(_expectInvalid(_driver, "width", "random(property-scoped property-index-scoped, 1px, 2px)"));
    try$(_expectInvalid(_driver, "width", "random(property-index-scoped --foo property-scoped, 1px, 2px)"));
    try$(_expectInvalid(_driver, "width", "random(property-index-scoped ua-width-1, 1px, 2px)"));
    try$(_expectInvalid(_driver, "width", "random(foo property-index-scoped, 1px, 2px)"));
    try$(_expectInvalid(_driver, "width", "random(property-index-scoped 1px, 2px)"));
    try$(_expectInvalid(_driver, "width", "random(property-index-scoped property-scoped, 1px, 2px)"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-values/random-item-invalid.html
test$("vaev-style-wpt-css-css-values-random-item-invalid") {
    try$(_expectInvalid(_driver, "font-family", "random-item()"));
    try$(_expectInvalid(_driver, "font-family", "random-item( )"));
    try$(_expectInvalid(_driver, "font-family", "random-item(auto)"));
    try$(_expectInvalid(_driver, "font-family", "random-item(, serif, sans-serif)"));
    try$(_expectInvalid(_driver, "font-family", "random-item(auto, !)"));
    try$(_expectInvalid(_driver, "font-family", "random-item(auto, ;)"));
    try$(_expectInvalid(_driver, "font-family", "random-item(auto, })"));
    try$(_expectInvalid(_driver, "font-family", "random-item(auto, ])"));
    try$(_expectInvalid(_driver, "font-family", "random-item(auto, {serif)"));
    try$(_expectInvalid(_driver, "font-family", "random-item(auto, serif})"));
    try$(_expectInvalid(_driver, "font-family", "random-item(auto, {Times, serif)"));
    try$(_expectInvalid(_driver, "font-family", "random-item({auto, serif, sans-serif)"));
    try$(_expectInvalid(_driver, "font-family", "random-item(auto, {Times, serif} extra)"));
    try$(_expectInvalid(_driver, "font-family", "random-item(auto, extra {Times, serif})"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-values/random-item-serialize.html
test$("vaev-style-wpt-css-css-values-random-item-serialize") {
    try$(_expectSerialized(_driver, "width", "random-item(--x, var(--a), var(--b))", "random-item(--x, var(--a), var(--b))", RegisteredPropertySet::TOP_LEVEL));
    return Ok();
}

test$("vaev-style-wpt-css-css-values-random-item-serialize-xfail", XFAIL) {
    try$(_expectSerialized(_driver, "font-family", "random-item(auto, serif, sans-serif)", "random-item(auto, serif, sans-serif)"));
    try$(_expectSerialized(_driver, "font-family", "random-item(--x, serif, sans-serif)", "random-item(--x, serif, sans-serif)"));
    try$(_expectSerialized(_driver, "font-family", "random-item(element-scoped, serif, sans-serif)", "random-item(element-scoped, serif, sans-serif)"));
    try$(_expectSerialized(_driver, "font-family", "random-item(--x element-scoped, serif, sans-serif)", "random-item(--x element-scoped, serif, sans-serif)"));
    try$(_expectSerialized(_driver, "font-family", "random-item(element-scoped --x, serif, sans-serif)", "random-item(element-scoped --x, serif, sans-serif)"));
    try$(_expectSerialized(_driver, "font-family", "random-item(fixed 0, serif, sans-serif)", "random-item(fixed 0, serif, sans-serif)"));
    try$(_expectSerialized(_driver, "font-family", "random-item(fixed 0.5, serif, sans-serif)", "random-item(fixed 0.5, serif, sans-serif)"));
    try$(_expectSerialized(_driver, "font-family", "random-item(fixed 1, serif, sans-serif)", "random-item(fixed 1, serif, sans-serif)"));
    try$(_expectSerialized(_driver, "font-family", "random-item(auto, serif)", "random-item(auto, serif)"));
    try$(_expectSerialized(_driver, "font-family", "random-item(auto, {Times, serif}, {Arial, sans-serif})", "random-item(auto, {Times, serif}, {Arial, sans-serif})"));
    try$(_expectSerialized(_driver, "font-family", "random-item(auto, {Times, serif}, sans-serif, {monospace})", "random-item(auto, {Times, serif}, sans-serif, {monospace})"));
    try$(_expectSerialized(_driver, "font-family", "random-item(auto, { Times, serif }, { Arial, sans-serif })", "random-item(auto, { Times, serif }, { Arial, sans-serif })"));
    try$(_expectSerialized(_driver, "color", "random-item(auto, red, green, blue)", "random-item(auto, red, green, blue)"));
    try$(_expectSerialized(_driver, "width", "random-item(auto, 1px, 2px, 3px)", "random-item(auto, 1px, 2px, 3px)"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-values/random-item-valid.html
test$("vaev-style-wpt-css-css-values-random-item-valid") {
    try$(_expectSerialized(_driver, "font-family", "random-item(var(--key), serif, sans-serif)", "random-item(var(--key), serif, sans-serif)", RegisteredPropertySet::TOP_LEVEL));
    return Ok();
}

test$("vaev-style-wpt-css-css-values-random-item-valid-xfail", XFAIL) {
    try$(_expectSerialized(_driver, "font-family", "random-item(auto, serif)", "random-item(auto, serif)"));
    try$(_expectSerialized(_driver, "font-family", "random-item(auto, serif, sans-serif, monospace)", "random-item(auto, serif, sans-serif, monospace)"));
    try$(_expectSerialized(_driver, "font-family", "random-item(auto ,serif ,sans-serif)", "random-item(auto ,serif ,sans-serif)"));
    try$(_expectSerialized(_driver, "font-family", "random-item(auto , serif , sans-serif)", "random-item(auto , serif , sans-serif)"));
    try$(_expectSerialized(_driver, "font-family", "random-item(auto, )", "random-item(auto, )"));
    try$(_expectSerialized(_driver, "font-family", "random-item(auto, serif,)", "random-item(auto, serif,)"));
    try$(_expectSerialized(_driver, "font-family", "random-item(auto, , serif)", "random-item(auto, , serif)"));
    try$(_expectSerialized(_driver, "font-family", "random-item(auto, ,)", "random-item(auto, ,)"));
    try$(_expectSerialized(_driver, "font-family", "random-item(auto, {Times, serif}, {Arial, sans-serif})", "random-item(auto, {Times, serif}, {Arial, sans-serif})"));
    try$(_expectSerialized(_driver, "font-family", "random-item(auto, {Times, serif}, sans-serif, {monospace})", "random-item(auto, {Times, serif}, sans-serif, {monospace})"));
    try$(_expectSerialized(_driver, "font-family", "random-item(auto, {Times, serif})", "random-item(auto, {Times, serif})"));
    try$(_expectSerialized(_driver, "font-family", "random-item(auto, { Times, serif }, { Arial, sans-serif })", "random-item(auto, { Times, serif }, { Arial, sans-serif })"));
    try$(_expectSerialized(_driver, "font-family", "random-item(auto, {Times, serif}, { Arial, sans-serif })", "random-item(auto, {Times, serif}, { Arial, sans-serif })"));
    try$(_expectSerialized(_driver, "font-family", "random-item(auto, { monospace })", "random-item(auto, { monospace })"));
    try$(_expectSerialized(_driver, "font-family", "random-item(--x, serif, sans-serif)", "random-item(--x, serif, sans-serif)"));
    try$(_expectSerialized(_driver, "font-family", "random-item(fixed 0.5, serif, sans-serif)", "random-item(fixed 0.5, serif, sans-serif)"));
    try$(_expectSerialized(_driver, "font-family", "random-item(serif, sans-serif)", "random-item(serif, sans-serif)"));
    try$(_expectSerialized(_driver, "font-family", "random-item(foo, serif, sans-serif)", "random-item(foo, serif, sans-serif)"));
    try$(_expectSerialized(_driver, "font-family", "random-item(fixed -0.5, serif, sans-serif)", "random-item(fixed -0.5, serif, sans-serif)"));
    try$(_expectSerialized(_driver, "font-family", "random-item(property-scoped, serif, sans-serif)", "random-item(property-scoped, serif, sans-serif)"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-values/round-mod-rem-invalid.html
test$("vaev-style-wpt-css-css-values-round-mod-rem-invalid") {
    try$(_expectInvalid(_driver, "opacity", "round()"));
    try$(_expectInvalid(_driver, "opacity", "round( )"));
    try$(_expectInvalid(_driver, "opacity", "round(,)"));
    try$(_expectInvalid(_driver, "opacity", "round(1, )"));
    try$(_expectInvalid(_driver, "opacity", "round(, 1)"));
    try$(_expectInvalid(_driver, "opacity", "round(1 + )"));
    try$(_expectInvalid(_driver, "opacity", "round(1 - )"));
    try$(_expectInvalid(_driver, "opacity", "round(1 * )"));
    try$(_expectInvalid(_driver, "opacity", "round(1 / )"));
    try$(_expectInvalid(_driver, "opacity", "round(1 2)"));
    try$(_expectInvalid(_driver, "opacity", "round(nearest, 1 2)"));
    try$(_expectInvalid(_driver, "opacity", "round(1, nearest, 12)"));
    try$(_expectInvalid(_driver, "opacity", "round(1, nearest)"));
    try$(_expectInvalid(_driver, "opacity", "round(nearest, 1, nearest)"));
    try$(_expectInvalid(_driver, "opacity", "round(1, , 2)"));
    try$(_expectInvalid(_driver, "opacity", "mod()"));
    try$(_expectInvalid(_driver, "opacity", "mod( )"));
    try$(_expectInvalid(_driver, "opacity", "mod(,)"));
    try$(_expectInvalid(_driver, "opacity", "mod(1, )"));
    try$(_expectInvalid(_driver, "opacity", "mod(, 1)"));
    try$(_expectInvalid(_driver, "opacity", "mod(1 + )"));
    try$(_expectInvalid(_driver, "opacity", "mod(1 - )"));
    try$(_expectInvalid(_driver, "opacity", "mod(1 * )"));
    try$(_expectInvalid(_driver, "opacity", "mod(1 / )"));
    try$(_expectInvalid(_driver, "opacity", "mod(1 2)"));
    try$(_expectInvalid(_driver, "opacity", "mod(1, , 2)"));
    try$(_expectInvalid(_driver, "opacity", "rem()"));
    try$(_expectInvalid(_driver, "opacity", "rem( )"));
    try$(_expectInvalid(_driver, "opacity", "rem(,)"));
    try$(_expectInvalid(_driver, "opacity", "rem(1, )"));
    try$(_expectInvalid(_driver, "opacity", "rem(, 1)"));
    try$(_expectInvalid(_driver, "opacity", "rem(1 + )"));
    try$(_expectInvalid(_driver, "opacity", "rem(1 - )"));
    try$(_expectInvalid(_driver, "opacity", "rem(1 * )"));
    try$(_expectInvalid(_driver, "opacity", "rem(1 / )"));
    try$(_expectInvalid(_driver, "opacity", "rem(1 2)"));
    try$(_expectInvalid(_driver, "opacity", "rem(1, , 2)"));
    try$(_expectInvalid(_driver, "opacity", "round(0px)"));
    try$(_expectInvalid(_driver, "opacity", "round(0s)"));
    try$(_expectInvalid(_driver, "opacity", "round(0deg)"));
    try$(_expectInvalid(_driver, "opacity", "round(0Hz)"));
    try$(_expectInvalid(_driver, "opacity", "round(0dpi)"));
    try$(_expectInvalid(_driver, "opacity", "round(0fr)"));
    try$(_expectInvalid(_driver, "opacity", "round(1, 1%)"));
    try$(_expectInvalid(_driver, "opacity", "round(1, 0px)"));
    try$(_expectInvalid(_driver, "opacity", "round(1, 0s)"));
    try$(_expectInvalid(_driver, "opacity", "round(1, 0deg)"));
    try$(_expectInvalid(_driver, "opacity", "round(1, 0Hz)"));
    try$(_expectInvalid(_driver, "opacity", "round(1, 0dpi)"));
    try$(_expectInvalid(_driver, "opacity", "round(1, 0fr)"));
    try$(_expectInvalid(_driver, "opacity", "mod(0px)"));
    try$(_expectInvalid(_driver, "opacity", "mod(0s)"));
    try$(_expectInvalid(_driver, "opacity", "mod(0deg)"));
    try$(_expectInvalid(_driver, "opacity", "mod(0Hz)"));
    try$(_expectInvalid(_driver, "opacity", "mod(0dpi)"));
    try$(_expectInvalid(_driver, "opacity", "mod(0fr)"));
    try$(_expectInvalid(_driver, "opacity", "mod(1, 1%)"));
    try$(_expectInvalid(_driver, "opacity", "mod(1, 0px)"));
    try$(_expectInvalid(_driver, "opacity", "mod(1, 0s)"));
    try$(_expectInvalid(_driver, "opacity", "mod(1, 0deg)"));
    try$(_expectInvalid(_driver, "opacity", "mod(1, 0Hz)"));
    try$(_expectInvalid(_driver, "opacity", "mod(1, 0dpi)"));
    try$(_expectInvalid(_driver, "opacity", "mod(1, 0fr)"));
    try$(_expectInvalid(_driver, "opacity", "rem(0px)"));
    try$(_expectInvalid(_driver, "opacity", "rem(0s)"));
    try$(_expectInvalid(_driver, "opacity", "rem(0deg)"));
    try$(_expectInvalid(_driver, "opacity", "rem(0Hz)"));
    try$(_expectInvalid(_driver, "opacity", "rem(0dpi)"));
    try$(_expectInvalid(_driver, "opacity", "rem(0fr)"));
    try$(_expectInvalid(_driver, "opacity", "rem(1, 1%)"));
    try$(_expectInvalid(_driver, "opacity", "rem(1, 0px)"));
    try$(_expectInvalid(_driver, "opacity", "rem(1, 0s)"));
    try$(_expectInvalid(_driver, "opacity", "rem(1, 0deg)"));
    try$(_expectInvalid(_driver, "opacity", "rem(1, 0Hz)"));
    try$(_expectInvalid(_driver, "opacity", "rem(1, 0dpi)"));
    try$(_expectInvalid(_driver, "opacity", "rem(1, 0fr)"));
    try$(_expectInvalid(_driver, "height", "round(0px)"));
    try$(_expectInvalid(_driver, "height", "calc(round(0px))"));
    try$(_expectInvalid(_driver, "height", "round(0px, 0s)"));
    try$(_expectInvalid(_driver, "height", "calc(round(0px, 0s))"));
    try$(_expectInvalid(_driver, "height", "round(0px, 0deg)"));
    try$(_expectInvalid(_driver, "height", "calc(round(0px, 0deg))"));
    try$(_expectInvalid(_driver, "height", "round(0px, 0Hz)"));
    try$(_expectInvalid(_driver, "height", "calc(round(0px, 0Hz))"));
    try$(_expectInvalid(_driver, "height", "round(0px, 0dpi)"));
    try$(_expectInvalid(_driver, "height", "calc(round(0px, 0dpi))"));
    try$(_expectInvalid(_driver, "height", "round(0px, 0fr)"));
    try$(_expectInvalid(_driver, "height", "calc(round(0px, 0fr))"));
    try$(_expectInvalid(_driver, "height", "mod(0px, 0s)"));
    try$(_expectInvalid(_driver, "height", "calc(mod(0px, 0s))"));
    try$(_expectInvalid(_driver, "height", "mod(0px, 0deg)"));
    try$(_expectInvalid(_driver, "height", "calc(mod(0px, 0deg))"));
    try$(_expectInvalid(_driver, "height", "mod(0px, 0Hz)"));
    try$(_expectInvalid(_driver, "height", "calc(mod(0px, 0Hz))"));
    try$(_expectInvalid(_driver, "height", "mod(0px, 0dpi)"));
    try$(_expectInvalid(_driver, "height", "calc(mod(0px, 0dpi))"));
    try$(_expectInvalid(_driver, "height", "mod(0px, 0fr)"));
    try$(_expectInvalid(_driver, "height", "calc(mod(0px, 0fr))"));
    try$(_expectInvalid(_driver, "height", "rem(0px, 0s)"));
    try$(_expectInvalid(_driver, "height", "calc(rem(0px, 0s))"));
    try$(_expectInvalid(_driver, "height", "rem(0px, 0deg)"));
    try$(_expectInvalid(_driver, "height", "calc(rem(0px, 0deg))"));
    try$(_expectInvalid(_driver, "height", "rem(0px, 0Hz)"));
    try$(_expectInvalid(_driver, "height", "calc(rem(0px, 0Hz))"));
    try$(_expectInvalid(_driver, "height", "rem(0px, 0dpi)"));
    try$(_expectInvalid(_driver, "height", "calc(rem(0px, 0dpi))"));
    try$(_expectInvalid(_driver, "height", "rem(0px, 0fr)"));
    try$(_expectInvalid(_driver, "height", "calc(rem(0px, 0fr))"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-values/signs-abs-invalid.html
test$("vaev-style-wpt-css-css-values-signs-abs-invalid") {
    try$(_expectInvalid(_driver, "font-weight", "abs()"));
    try$(_expectInvalid(_driver, "font-weight", "abs( )"));
    try$(_expectInvalid(_driver, "font-weight", "abs(,)"));
    try$(_expectInvalid(_driver, "font-weight", "abs(1, )"));
    try$(_expectInvalid(_driver, "font-weight", "abs(, 1)"));
    try$(_expectInvalid(_driver, "font-weight", "abs(1 + )"));
    try$(_expectInvalid(_driver, "font-weight", "abs(1 - )"));
    try$(_expectInvalid(_driver, "font-weight", "abs(1 * )"));
    try$(_expectInvalid(_driver, "font-weight", "abs(1 / )"));
    try$(_expectInvalid(_driver, "font-weight", "abs(1 2)"));
    try$(_expectInvalid(_driver, "font-weight", "abs(1, , 2)"));
    try$(_expectInvalid(_driver, "font-weight", "abs(1, 2)"));
    try$(_expectInvalid(_driver, "font-weight", "abs(1, 1%)"));
    try$(_expectInvalid(_driver, "font-weight", "abs(1, 0px)"));
    try$(_expectInvalid(_driver, "font-weight", "abs(1, 0s)"));
    try$(_expectInvalid(_driver, "font-weight", "abs(1, 0deg)"));
    try$(_expectInvalid(_driver, "font-weight", "abs(1, 0Hz)"));
    try$(_expectInvalid(_driver, "font-weight", "abs(1, 0dpi)"));
    try$(_expectInvalid(_driver, "font-weight", "abs(1, 0fr)"));
    try$(_expectInvalid(_driver, "font-weight", "sign()"));
    try$(_expectInvalid(_driver, "font-weight", "sign( )"));
    try$(_expectInvalid(_driver, "font-weight", "sign(,)"));
    try$(_expectInvalid(_driver, "font-weight", "sign(1, )"));
    try$(_expectInvalid(_driver, "font-weight", "sign(, 1)"));
    try$(_expectInvalid(_driver, "font-weight", "sign(1 + )"));
    try$(_expectInvalid(_driver, "font-weight", "sign(1 - )"));
    try$(_expectInvalid(_driver, "font-weight", "sign(1 * )"));
    try$(_expectInvalid(_driver, "font-weight", "sign(1 / )"));
    try$(_expectInvalid(_driver, "font-weight", "sign(1 2)"));
    try$(_expectInvalid(_driver, "font-weight", "sign(1, , 2)"));
    try$(_expectInvalid(_driver, "font-weight", "sign(1, 2)"));
    try$(_expectInvalid(_driver, "font-weight", "sign(1, 1%)"));
    try$(_expectInvalid(_driver, "font-weight", "sign(1, 0px)"));
    try$(_expectInvalid(_driver, "font-weight", "sign(1, 0s)"));
    try$(_expectInvalid(_driver, "font-weight", "sign(1, 0deg)"));
    try$(_expectInvalid(_driver, "font-weight", "sign(1, 0Hz)"));
    try$(_expectInvalid(_driver, "font-weight", "sign(1, 0dpi)"));
    try$(_expectInvalid(_driver, "font-weight", "sign(1, 0fr)"));
    try$(_expectInvalid(_driver, "font-weight", "abs(0px)"));
    try$(_expectInvalid(_driver, "font-weight", "abs(0s)"));
    try$(_expectInvalid(_driver, "font-weight", "abs(0deg)"));
    try$(_expectInvalid(_driver, "font-weight", "abs(0Hz)"));
    try$(_expectInvalid(_driver, "font-weight", "abs(0dpi)"));
    try$(_expectInvalid(_driver, "font-weight", "abs(0fr)"));
    try$(_expectInvalid(_driver, "font-weight", "abs(1%)"));
    try$(_expectInvalid(_driver, "font-weight", "sign(10px + 5rad)"));
    try$(_expectInvalid(_driver, "font-weight", "sign(10%)"));
    try$(_expectInvalid(_driver, "margin-left", "1px * sign(10px + 5rad)"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-values/sin-cos-tan-invalid.html
test$("vaev-style-wpt-css-css-values-sin-cos-tan-invalid") {
    try$(_expectInvalid(_driver, "transform", "rotate(sin())"));
    try$(_expectInvalid(_driver, "transform", "rotate(sin( ))"));
    try$(_expectInvalid(_driver, "transform", "rotate(sin(,))"));
    try$(_expectInvalid(_driver, "transform", "rotate(sin(1dag))"));
    try$(_expectInvalid(_driver, "transform", "rotate(sin(1deg, ))"));
    try$(_expectInvalid(_driver, "transform", "rotate(sin(, 1deg))"));
    try$(_expectInvalid(_driver, "transform", "rotate(sin(1deg + ))"));
    try$(_expectInvalid(_driver, "transform", "rotate(sin(1deg - ))"));
    try$(_expectInvalid(_driver, "transform", "rotate(sin(1deg * ))"));
    try$(_expectInvalid(_driver, "transform", "rotate(sin(1deg / ))"));
    try$(_expectInvalid(_driver, "transform", "rotate(sin(1deg 2deg))"));
    try$(_expectInvalid(_driver, "transform", "rotate(sin(1deg, , 2deg))"));
    try$(_expectInvalid(_driver, "transform", "rotate(cos())"));
    try$(_expectInvalid(_driver, "transform", "rotate(cos( ))"));
    try$(_expectInvalid(_driver, "transform", "rotate(cos(,))"));
    try$(_expectInvalid(_driver, "transform", "rotate(cos(1dag))"));
    try$(_expectInvalid(_driver, "transform", "rotate(cos(1deg, ))"));
    try$(_expectInvalid(_driver, "transform", "rotate(cos(, 1deg))"));
    try$(_expectInvalid(_driver, "transform", "rotate(cos(1deg + ))"));
    try$(_expectInvalid(_driver, "transform", "rotate(cos(1deg - ))"));
    try$(_expectInvalid(_driver, "transform", "rotate(cos(1deg * ))"));
    try$(_expectInvalid(_driver, "transform", "rotate(cos(1deg / ))"));
    try$(_expectInvalid(_driver, "transform", "rotate(cos(1deg 2deg))"));
    try$(_expectInvalid(_driver, "transform", "rotate(cos(1deg, , 2deg))"));
    try$(_expectInvalid(_driver, "transform", "rotate(tan())"));
    try$(_expectInvalid(_driver, "transform", "rotate(tan( ))"));
    try$(_expectInvalid(_driver, "transform", "rotate(tan(,))"));
    try$(_expectInvalid(_driver, "transform", "rotate(tan(1dag))"));
    try$(_expectInvalid(_driver, "transform", "rotate(tan(1deg, ))"));
    try$(_expectInvalid(_driver, "transform", "rotate(tan(, 1deg))"));
    try$(_expectInvalid(_driver, "transform", "rotate(tan(1deg + ))"));
    try$(_expectInvalid(_driver, "transform", "rotate(tan(1deg - ))"));
    try$(_expectInvalid(_driver, "transform", "rotate(tan(1deg * ))"));
    try$(_expectInvalid(_driver, "transform", "rotate(tan(1deg / ))"));
    try$(_expectInvalid(_driver, "transform", "rotate(tan(1deg 2deg))"));
    try$(_expectInvalid(_driver, "transform", "rotate(tan(1deg, , 2deg))"));
    try$(_expectInvalid(_driver, "transform", "rotate(sin(90px))"));
    try$(_expectInvalid(_driver, "transform", "rotate(sin(30deg + 1.0471967rad, 0))"));
    try$(_expectInvalid(_driver, "transform", "rotate(cos( 0 ,))"));
    try$(_expectInvalid(_driver, "transform", "rotate(cos( () 30deg - 0.523599rad ))"));
    try$(_expectInvalid(_driver, "transform", "rotate(tan(45deg ))"));
    try$(_expectInvalid(_driver, "transform", "rotate(tan(30deg, + 0.261799rad))"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-values/tree-counting/calc-sibling-function-parsing.html
test$("vaev-style-wpt-css-css-values-tree-counting-calc-sibling-function-parsing") {
    try$(_expectInvalid(_driver, "left", "calc(1px * sibling-index(100px))"));
    try$(_expectInvalid(_driver, "left", "calc(1px * sibling-index(1))"));
    try$(_expectInvalid(_driver, "left", "calc(10 * sibling-index())"));
    try$(_expectInvalid(_driver, "z-index", "sibling-index(100px)"));
    try$(_expectInvalid(_driver, "z-index", "sibling-count(1)"));
    try$(_expectInvalid(_driver, "opacity", "calc(1ms * sibling-index())"));
    try$(_expectInvalid(_driver, "opacity", "calc(1px * sibling-count())"));
    try$(_expectInvalid(_driver, "color", "color(srgb calc(1px * sibling-index()) 0 0)"));
    try$(_expectInvalid(_driver, "color", "oklch(calc(1px * sibling-index()) 0 0)"));
    return Ok();
}

test$("vaev-style-wpt-css-css-values-tree-counting-calc-sibling-function-parsing-xfail", XFAIL) {
    try$(_expectSerialized(_driver, "left", "calc(1px * sibling-index())", "calc(1px * sibling-index())"));
    try$(_expectSerialized(_driver, "left", "calc(1px * sibling-index( ))", "calc(1px * sibling-index())"));
    try$(_expectSerialized(_driver, "left", "calc(1em * sibling-index())", "calc(1em * sibling-index())"));
    try$(_expectSerialized(_driver, "left", "calc(1rem * sibling-count())", "calc(1rem * sibling-count())"));
    try$(_expectSerialized(_driver, "left", "calc(1vh * sibling-index())", "calc(1vh * sibling-index())"));
    try$(_expectSerialized(_driver, "left", "calc(1ch * sibling-count())", "calc(1ch * sibling-count())"));
    try$(_expectSerialized(_driver, "left", "calc(1% * sibling-index())", "calc(1% * sibling-index())"));
    try$(_expectSerialized(_driver, "left", "calc(5% * sibling-count())", "calc(5% * sibling-count())"));
    try$(_expectSerialized(_driver, "z-index", "sibling-index()", "sibling-index()"));
    try$(_expectSerialized(_driver, "z-index", "sibling-count()", "sibling-count()"));
    try$(_expectSerialized(_driver, "z-index", "calc(2 * sibling-index())", "calc(2 * sibling-index())"));
    try$(_expectSerialized(_driver, "z-index", "calc(sibling-index())", "sibling-index()"));
    try$(_expectSerialized(_driver, "z-index", "calc(sibling-count())", "sibling-count()"));
    try$(_expectSerialized(_driver, "opacity", "calc(0.1 * sibling-index())", "calc(0.1 * sibling-index())"));
    try$(_expectSerialized(_driver, "opacity", "calc(0.5 * sibling-count())", "calc(0.5 * sibling-count())"));
    try$(_expectSerialized(_driver, "color", "color(srgb calc(0.1 * sibling-index()) 0.5 0.5)", "color(srgb calc(0.1 * sibling-index()) 0.5 0.5)"));
    try$(_expectSerialized(_driver, "color", "color(srgb 0 calc(0.1 * sibling-count()) 0.5)", "color(srgb 0 calc(0.1 * sibling-count()) 0.5)"));
    try$(_expectSerialized(_driver, "color", "color(srgb 0 0 calc(0.1 * sibling-index()))", "color(srgb 0 0 calc(0.1 * sibling-index()))"));
    try$(_expectSerialized(_driver, "color", "color(srgb 0.5 0.5 0.5 / calc(0.1 * sibling-count()))", "color(srgb 0.5 0.5 0.5 / calc(0.1 * sibling-count()))"));
    try$(_expectSerialized(_driver, "color", "oklch(calc(0.1 * sibling-index()) 0.2 180)", "oklch(calc(0.1 * sibling-index()) 0.2 180)"));
    try$(_expectSerialized(_driver, "color", "oklch(0.5 calc(0.05 * sibling-count()) 180)", "oklch(0.5 calc(0.05 * sibling-count()) 180)"));
    try$(_expectSerialized(_driver, "color", "oklch(0.5 0.2 calc(30deg * sibling-index()))", "oklch(0.5 0.2 calc(30deg * sibling-index()))"));
    try$(_expectSerialized(_driver, "color", "oklch(0.5 0.2 180 / calc(0.1 * sibling-index()))", "oklch(0.5 0.2 180 / calc(0.1 * sibling-index()))"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-values/typed_arithmetic.html
test$("vaev-style-wpt-css-css-values-typed_arithmetic") {
    try$(_expectInvalid(_driver, "z-index", "calc(10% / 20%)"));
    try$(_expectInvalid(_driver, "width", "calc((1% * 1deg) / 1px)"));
    try$(_expectInvalid(_driver, "width", "calc((1% * 1% * 1%) / 1px)"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-values/urls/url-request-modifiers-invalid.sub.html
test$("vaev-style-wpt-css-css-values-urls-url-request-modifiers-invalid.sub") {
    try$(_expectInvalid(_driver, "background-image", "url(\"http://{{hosts[][]}}:{{ports[http][0]}}/css/support/1x1-green.png\" cross-origin())"));
    try$(_expectInvalid(_driver, "background-image", "url(\"http://{{hosts[][]}}:{{ports[http][0]}}/css/support/1x1-green.png\" cross-origin(,))"));
    try$(_expectInvalid(_driver, "background-image", "url(\"http://{{hosts[][]}}:{{ports[http][0]}}/css/support/1x1-green.png\" cross-origin(anonymous,))"));
    try$(_expectInvalid(_driver, "background-image", "url(\"http://{{hosts[][]}}:{{ports[http][0]}}/css/support/1x1-green.png\" cross-origin(,anonymous))"));
    try$(_expectInvalid(_driver, "background-image", "url(\"http://{{hosts[][]}}:{{ports[http][0]}}/css/support/1x1-green.png\" cross-origin(anonymous foobar))"));
    try$(_expectInvalid(_driver, "background-image", "url(\"http://{{hosts[][]}}:{{ports[http][0]}}/css/support/1x1-green.png\" cross-origin(anonymous) cross-origin(anonymous))"));
    try$(_expectInvalid(_driver, "background-image", "url(\"http://{{hosts[][]}}:{{ports[http][0]}}/css/support/1x1-green.png\" cross-origin(anonymous) cross-origin(use-credentials))"));
    try$(_expectInvalid(_driver, "background-image", "url(cross-origin(anonymous) \"http://{{hosts[][]}}:{{ports[http][0]}}/css/support/1x1-green.png\")"));
    try$(_expectInvalid(_driver, "background-image", "\"http://{{hosts[][]}}:{{ports[http][0]}}/css/support/1x1-green.png\" cross-origin(anonymous)"));
    try$(_expectInvalid(_driver, "background-image", "url(\"http://{{hosts[][]}}:{{ports[http][0]}}/css/support/1x1-green.png\" integrity())"));
    try$(_expectInvalid(_driver, "background-image", "url(\"http://{{hosts[][]}}:{{ports[http][0]}}/css/support/1x1-green.png\" integrity(,))"));
    try$(_expectInvalid(_driver, "background-image", "url(\"http://{{hosts[][]}}:{{ports[http][0]}}/css/support/1x1-green.png\" integrity(\"sha384-foobar\",))"));
    try$(_expectInvalid(_driver, "background-image", "url(\"http://{{hosts[][]}}:{{ports[http][0]}}/css/support/1x1-green.png\" integrity(,\"sha384-foobar\"))"));
    try$(_expectInvalid(_driver, "background-image", "url(\"http://{{hosts[][]}}:{{ports[http][0]}}/css/support/1x1-green.png\" integrity(\"sha384-foobar\" foobar))"));
    try$(_expectInvalid(_driver, "background-image", "url(\"http://{{hosts[][]}}:{{ports[http][0]}}/css/support/1x1-green.png\" integrity(sha384-foobar))"));
    try$(_expectInvalid(_driver, "background-image", "url(\"http://{{hosts[][]}}:{{ports[http][0]}}/css/support/1x1-green.png\" integrity(\"sha384-foobar\") integrity(\"sha384-foobar\"))"));
    try$(_expectInvalid(_driver, "background-image", "url(\"http://{{hosts[][]}}:{{ports[http][0]}}/css/support/1x1-green.png\" integrity(\"sha384-foobar\") integrity(\"sha384-barbaz\"))"));
    try$(_expectInvalid(_driver, "background-image", "url(integrity(\"sha384-foobar\") \"http://{{hosts[][]}}:{{ports[http][0]}}/css/support/1x1-green.png\")"));
    try$(_expectInvalid(_driver, "background-image", "\"http://{{hosts[][]}}:{{ports[http][0]}}/css/support/1x1-green.png\" integrity(\"sha384-foobar\")"));
    try$(_expectInvalid(_driver, "background-image", "url(\"http://{{hosts[][]}}:{{ports[http][0]}}/css/support/1x1-green.png\" referrer-policy())"));
    try$(_expectInvalid(_driver, "background-image", "url(\"http://{{hosts[][]}}:{{ports[http][0]}}/css/support/1x1-green.png\" referrer-policy(,))"));
    try$(_expectInvalid(_driver, "background-image", "url(\"http://{{hosts[][]}}:{{ports[http][0]}}/css/support/1x1-green.png\" referrer-policy(no-referrer,))"));
    try$(_expectInvalid(_driver, "background-image", "url(\"http://{{hosts[][]}}:{{ports[http][0]}}/css/support/1x1-green.png\" referrer-policy(,no-referrer))"));
    try$(_expectInvalid(_driver, "background-image", "url(\"http://{{hosts[][]}}:{{ports[http][0]}}/css/support/1x1-green.png\" referrer-policy(no-referrer foobar))"));
    try$(_expectInvalid(_driver, "background-image", "url(\"http://{{hosts[][]}}:{{ports[http][0]}}/css/support/1x1-green.png\" referrer-policy(no-referrer same-origin))"));
    try$(_expectInvalid(_driver, "background-image", "url(\"http://{{hosts[][]}}:{{ports[http][0]}}/css/support/1x1-green.png\" referrer-policy(no-referrer) referrer-policy(no-referrer))"));
    try$(_expectInvalid(_driver, "background-image", "url(\"http://{{hosts[][]}}:{{ports[http][0]}}/css/support/1x1-green.png\" referrer-policy(no-referrer) referrer-policy(same-origin))"));
    try$(_expectInvalid(_driver, "background-image", "url(referrer-policy(no-referrer) \"http://{{hosts[][]}}:{{ports[http][0]}}/css/support/1x1-green.png\")"));
    try$(_expectInvalid(_driver, "background-image", "\"http://{{hosts[][]}}:{{ports[http][0]}}/css/support/1x1-green.png\" referrer-policy(no-referrer)"));
    try$(_expectInvalid(_driver, "background-image", "url(\"http://{{hosts[][]}}:{{ports[http][0]}}/css/support/1x1-green.png\" crossorigin(anonymous))"));
    try$(_expectInvalid(_driver, "background-image", "url(\"http://{{hosts[][]}}:{{ports[http][0]}}/css/support/1x1-green.png\" referrerpolicy(no-referrer))"));
    try$(_expectInvalid(_driver, "background-image", "url(http://{{hosts[][]}}:{{ports[http][0]}}/css/support/1x1-green.png cross-origin(anonymous))"));
    try$(_expectInvalid(_driver, "background-image", "url(http://{{hosts[][]}}:{{ports[http][0]}}/css/support/1x1-green.png integrity(\"sha384-foobar\"))"));
    try$(_expectInvalid(_driver, "background-image", "url(http://{{hosts[][]}}:{{ports[http][0]}}/css/support/1x1-green.png referrer-policy(no-referrer))"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-values/viewport-units-parsing.html
test$("vaev-style-wpt-css-css-values-viewport-units-parsing") {
    try$(_expectSerialized(_driver, "width", "1vw", "1vw"));
    try$(_expectSerialized(_driver, "width", "1vh", "1vh"));
    try$(_expectSerialized(_driver, "width", "1vi", "1vi"));
    try$(_expectSerialized(_driver, "width", "1vb", "1vb"));
    try$(_expectSerialized(_driver, "width", "1vmin", "1vmin"));
    try$(_expectSerialized(_driver, "width", "1vmax", "1vmax"));
    try$(_expectSerialized(_driver, "width", "1svw", "1svw"));
    try$(_expectSerialized(_driver, "width", "1svh", "1svh"));
    try$(_expectSerialized(_driver, "width", "1svi", "1svi"));
    try$(_expectSerialized(_driver, "width", "1svb", "1svb"));
    try$(_expectSerialized(_driver, "width", "1svmin", "1svmin"));
    try$(_expectSerialized(_driver, "width", "1svmax", "1svmax"));
    try$(_expectSerialized(_driver, "width", "1lvw", "1lvw"));
    try$(_expectSerialized(_driver, "width", "1lvh", "1lvh"));
    try$(_expectSerialized(_driver, "width", "1lvi", "1lvi"));
    try$(_expectSerialized(_driver, "width", "1lvb", "1lvb"));
    try$(_expectSerialized(_driver, "width", "1lvmin", "1lvmin"));
    try$(_expectSerialized(_driver, "width", "1lvmax", "1lvmax"));
    try$(_expectSerialized(_driver, "width", "1dvw", "1dvw"));
    try$(_expectSerialized(_driver, "width", "1dvh", "1dvh"));
    try$(_expectSerialized(_driver, "width", "1dvi", "1dvi"));
    try$(_expectSerialized(_driver, "width", "1dvb", "1dvb"));
    try$(_expectSerialized(_driver, "width", "1dvmin", "1dvmin"));
    try$(_expectSerialized(_driver, "width", "1dvmax", "1dvmax"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-variables/var-parsing.html
test$("vaev-style-wpt-css-css-variables-var-parsing") {
    try$(_expectSerialized(_driver, "width", "var(--x)", "var(--x)", RegisteredPropertySet::TOP_LEVEL));
    try$(_expectSerialized(_driver, "width", "var(--x,)", "var(--x,)", RegisteredPropertySet::TOP_LEVEL));
    try$(_expectSerialized(_driver, "width", "var(--x, )", "var(--x, )", RegisteredPropertySet::TOP_LEVEL));
    try$(_expectSerialized(_driver, "width", "var(--x ())", "var(--x ())", RegisteredPropertySet::TOP_LEVEL));
    try$(_expectSerialized(_driver, "width", "var(--x () )", "var(--x () )", RegisteredPropertySet::TOP_LEVEL));
    try$(_expectSerialized(_driver, "width", "var(--x() )", "var(--x() )", RegisteredPropertySet::TOP_LEVEL));
    try$(_expectSerialized(_driver, "width", "var(--x (),)", "var(--x (),)", RegisteredPropertySet::TOP_LEVEL));
    try$(_expectSerialized(_driver, "width", "var(--x(),)", "var(--x(),)", RegisteredPropertySet::TOP_LEVEL));
    try$(_expectSerialized(_driver, "width", "var({--x})", "var({--x})", RegisteredPropertySet::TOP_LEVEL));
    try$(_expectSerialized(_driver, "width", "var({--x}, 10px)", "var({--x}, 10px)", RegisteredPropertySet::TOP_LEVEL));
    try$(_expectSerialized(_driver, "width", "var({--x, --y})", "var({--x, --y})", RegisteredPropertySet::TOP_LEVEL));
    try$(_expectInvalid(_driver, "width", "var(--x {--y})"));
    try$(_expectInvalid(_driver, "width", "var({--x} --y)"));
    try$(_expectInvalid(_driver, "width", "var(--x {--y}, 10px)"));
    try$(_expectInvalid(_driver, "width", "var()"));
    try$(_expectInvalid(_driver, "width", "var({})"));
    try$(_expectInvalid(_driver, "width", "var({}, 10px)"));
    try$(_expectInvalid(_driver, "width", "var(, 10px)"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/svg/geometry/parsing/cx-invalid.svg
test$("vaev-style-wpt-svg-geometry-parsing-cx-invalid") {
    try$(_expectInvalid(_driver, "cx", "10"));
    try$(_expectInvalid(_driver, "cx", "auto"));
    try$(_expectInvalid(_driver, "cx", "10px 20px"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/svg/geometry/parsing/cx-valid.svg
test$("vaev-style-wpt-svg-geometry-parsing-cx-valid") {
    try$(_expectSerialized(_driver, "cx", "0", "0px"));
    try$(_expectSerialized(_driver, "cx", "-1px", "-1px"));
    try$(_expectSerialized(_driver, "cx", "4%", "4%"));
    try$(_expectSerialized(_driver, "cx", "5ch", "5ch"));
    return Ok();
}

test$("vaev-style-wpt-svg-geometry-parsing-cx-valid-xfail", XFAIL) {
    try$(_expectSerialized(_driver, "cx", "calc(2em + 3ex)", "calc(2em + 3ex)"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/svg/geometry/parsing/cy-invalid.svg
test$("vaev-style-wpt-svg-geometry-parsing-cy-invalid") {
    try$(_expectInvalid(_driver, "cy", "10"));
    try$(_expectInvalid(_driver, "cy", "auto"));
    try$(_expectInvalid(_driver, "cy", "10px 20px"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/svg/geometry/parsing/cy-valid.svg
test$("vaev-style-wpt-svg-geometry-parsing-cy-valid") {
    try$(_expectSerialized(_driver, "cy", "0", "0px"));
    try$(_expectSerialized(_driver, "cy", "-1px", "-1px"));
    try$(_expectSerialized(_driver, "cy", "4%", "4%"));
    try$(_expectSerialized(_driver, "cy", "5rem", "5rem"));
    return Ok();
}

test$("vaev-style-wpt-svg-geometry-parsing-cy-valid-xfail", XFAIL) {
    try$(_expectSerialized(_driver, "cy", "calc(2em + 3ex)", "calc(2em + 3ex)"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/svg/geometry/parsing/r-invalid.svg
test$("vaev-style-wpt-svg-geometry-parsing-r-invalid") {
    try$(_expectInvalid(_driver, "r", "10"));
    try$(_expectInvalid(_driver, "r", "auto"));
    try$(_expectInvalid(_driver, "r", "10px 20px"));
    return Ok();
}

test$("vaev-style-wpt-svg-geometry-parsing-r-invalid-xfail", XFAIL) {
    try$(_expectInvalid(_driver, "r", "-1px"));
    try$(_expectInvalid(_driver, "r", "-10%"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/svg/geometry/parsing/r-valid.svg
test$("vaev-style-wpt-svg-geometry-parsing-r-valid") {
    try$(_expectSerialized(_driver, "r", "0", "0px"));
    try$(_expectSerialized(_driver, "r", "1px", "1px"));
    try$(_expectSerialized(_driver, "r", "4%", "4%"));
    try$(_expectSerialized(_driver, "r", "5vmin", "5vmin"));
    return Ok();
}

test$("vaev-style-wpt-svg-geometry-parsing-r-valid-xfail", XFAIL) {
    try$(_expectSerialized(_driver, "r", "calc(2em + 3ex)", "calc(2em + 3ex)"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/svg/geometry/parsing/x-invalid.svg
test$("vaev-style-wpt-svg-geometry-parsing-x-invalid") {
    try$(_expectInvalid(_driver, "x", "10"));
    try$(_expectInvalid(_driver, "x", "auto"));
    try$(_expectInvalid(_driver, "x", "10px 20px"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/svg/geometry/parsing/x-valid.svg
test$("vaev-style-wpt-svg-geometry-parsing-x-valid") {
    try$(_expectSerialized(_driver, "x", "0", "0px"));
    try$(_expectSerialized(_driver, "x", "-1px", "-1px"));
    try$(_expectSerialized(_driver, "x", "4%", "4%"));
    try$(_expectSerialized(_driver, "x", "5cm", "5cm"));
    return Ok();
}

test$("vaev-style-wpt-svg-geometry-parsing-x-valid-xfail", XFAIL) {
    try$(_expectSerialized(_driver, "x", "calc(2em + 3ex)", "calc(2em + 3ex)"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/svg/geometry/parsing/y-invalid.svg
test$("vaev-style-wpt-svg-geometry-parsing-y-invalid") {
    try$(_expectInvalid(_driver, "y", "10"));
    try$(_expectInvalid(_driver, "y", "auto"));
    try$(_expectInvalid(_driver, "y", "10px 20px"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/svg/geometry/parsing/y-valid.svg
test$("vaev-style-wpt-svg-geometry-parsing-y-valid") {
    try$(_expectSerialized(_driver, "y", "0", "0px"));
    try$(_expectSerialized(_driver, "y", "-1px", "-1px"));
    try$(_expectSerialized(_driver, "y", "4%", "4%"));
    try$(_expectSerialized(_driver, "y", "5mm", "5mm"));
    return Ok();
}

test$("vaev-style-wpt-svg-geometry-parsing-y-valid-xfail", XFAIL) {
    try$(_expectSerialized(_driver, "y", "calc(2em + 3ex)", "calc(2em + 3ex)"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/svg/painting/parsing/fill-invalid.svg
test$("vaev-style-wpt-svg-painting-parsing-fill-invalid") {
    try$(_expectInvalid(_driver, "fill", "auto"));
    try$(_expectInvalid(_driver, "fill", "none red"));
    try$(_expectInvalid(_driver, "fill", "none url(\"https://example.com/\")"));
    try$(_expectInvalid(_driver, "fill", "red url(\"https://example.com/\")"));
    try$(_expectInvalid(_driver, "fill", "url(\"https://example.com/\") none red"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/svg/painting/parsing/fill-opacity-invalid.svg
test$("vaev-style-wpt-svg-painting-parsing-fill-opacity-invalid") {
    try$(_expectInvalid(_driver, "fill-opacity", "1."));
    try$(_expectInvalid(_driver, "fill-opacity", "2 3"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/svg/painting/parsing/fill-opacity-valid.svg
test$("vaev-style-wpt-svg-painting-parsing-fill-opacity-valid") {
    try$(_expectSerialized(_driver, "fill-opacity", "-1", "-1"));
    try$(_expectSerialized(_driver, "fill-opacity", "0.5", "0.5"));
    try$(_expectSerialized(_driver, "fill-opacity", "3", "3"));
    try$(_expectSerialized(_driver, "fill-opacity", "-100%", "-1"));
    try$(_expectSerialized(_driver, "fill-opacity", "50%", "0.5"));
    try$(_expectSerialized(_driver, "fill-opacity", "300%", "3"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/svg/painting/parsing/fill-valid.svg
test$("vaev-style-wpt-svg-painting-parsing-fill-valid") {
    try$(_expectSerialized(_driver, "fill", "none", "none"));
    try$(_expectSerialized(_driver, "fill", "rgb(12, 34, 56)", "rgb(12, 34, 56)"));
    return Ok();
}

test$("vaev-style-wpt-svg-painting-parsing-fill-valid-xfail", XFAIL) {
    try$(_expectSerialized(_driver, "fill", "context-fill", "context-fill"));
    try$(_expectSerialized(_driver, "fill", "context-stroke", "context-stroke"));
    try$(_expectSerialized(_driver, "fill", "url(\"https://example.com/\")", "url(\"https://example.com/\")"));
    try$(_expectSerialized(_driver, "fill", "url(\"https://example.com/\") none", "url(\"https://example.com/\") none"));
    try$(_expectSerialized(_driver, "fill", "url(\"https://example.com/\") rgb(12, 34, 56)", "url(\"https://example.com/\") rgb(12, 34, 56)"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/svg/painting/parsing/stroke-invalid.svg
test$("vaev-style-wpt-svg-painting-parsing-stroke-invalid") {
    try$(_expectInvalid(_driver, "stroke", "auto"));
    try$(_expectInvalid(_driver, "stroke", "none red"));
    try$(_expectInvalid(_driver, "stroke", "none url(\"https://example.com/\")"));
    try$(_expectInvalid(_driver, "stroke", "red url(\"https://example.com/\")"));
    try$(_expectInvalid(_driver, "stroke", "url(\"https://example.com/\") none red"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/svg/painting/parsing/stroke-opacity-invalid.svg
test$("vaev-style-wpt-svg-painting-parsing-stroke-opacity-invalid") {
    try$(_expectInvalid(_driver, "stroke-opacity", "1."));
    try$(_expectInvalid(_driver, "stroke-opacity", "2 3"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/svg/painting/parsing/stroke-opacity-valid.svg
test$("vaev-style-wpt-svg-painting-parsing-stroke-opacity-valid") {
    try$(_expectSerialized(_driver, "stroke-opacity", "-1", "-1"));
    try$(_expectSerialized(_driver, "stroke-opacity", "0.5", "0.5"));
    try$(_expectSerialized(_driver, "stroke-opacity", "3", "3"));
    try$(_expectSerialized(_driver, "stroke-opacity", "-100%", "-1"));
    try$(_expectSerialized(_driver, "stroke-opacity", "50%", "0.5"));
    try$(_expectSerialized(_driver, "stroke-opacity", "300%", "3"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/svg/painting/parsing/stroke-valid.svg
test$("vaev-style-wpt-svg-painting-parsing-stroke-valid") {
    try$(_expectSerialized(_driver, "stroke", "none", "none"));
    try$(_expectSerialized(_driver, "stroke", "rgb(12, 34, 56)", "rgb(12, 34, 56)"));
    return Ok();
}

test$("vaev-style-wpt-svg-painting-parsing-stroke-valid-xfail", XFAIL) {
    try$(_expectSerialized(_driver, "stroke", "context-fill", "context-fill"));
    try$(_expectSerialized(_driver, "stroke", "context-stroke", "context-stroke"));
    try$(_expectSerialized(_driver, "stroke", "url(\"https://example.com/\")", "url(\"https://example.com/\")"));
    try$(_expectSerialized(_driver, "stroke", "url(\"https://example.com/\") none", "url(\"https://example.com/\") none"));
    try$(_expectSerialized(_driver, "stroke", "url(\"https://example.com/\") rgb(12, 34, 56)", "url(\"https://example.com/\") rgb(12, 34, 56)"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/svg/painting/parsing/stroke-width-invalid.svg
test$("vaev-style-wpt-svg-painting-parsing-stroke-width-invalid") {
    try$(_expectInvalid(_driver, "stroke-width", "auto"));
    try$(_expectInvalid(_driver, "stroke-width", "10px 20px"));
    try$(_expectInvalid(_driver, "stroke-width", "calc(2px + 3)"));
    try$(_expectInvalid(_driver, "stroke-width", "calc(10% + 5)"));
    try$(_expectInvalid(_driver, "stroke-width", "calc(40 + calc(3px + 6%))"));
    return Ok();
}

test$("vaev-style-wpt-svg-painting-parsing-stroke-width-invalid-xfail", XFAIL) {
    try$(_expectInvalid(_driver, "stroke-width", "-1px"));
    try$(_expectInvalid(_driver, "stroke-width", "-10%"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/svg/painting/parsing/stroke-width-valid.svg
test$("vaev-style-wpt-svg-painting-parsing-stroke-width-valid") {
    try$(_expectSerialized(_driver, "stroke-width", "1px", "1px"));
    try$(_expectSerialized(_driver, "stroke-width", "4%", "4%"));
    try$(_expectSerialized(_driver, "stroke-width", "5vmin", "5vmin"));
    return Ok();
}

test$("vaev-style-wpt-svg-painting-parsing-stroke-width-valid-xfail", XFAIL) {
    try$(_expectSerialized(_driver, "stroke-width", "0", "0"));
    try$(_expectSerialized(_driver, "stroke-width", "10", "10"));
    try$(_expectSerialized(_driver, "stroke-width", "calc(2em + 3ex)", "calc(2em + 3ex)"));
    try$(_expectSerialized(_driver, "stroke-width", "calc(50% + 60px)", "calc(50% + 60px)"));
    try$(_expectSerialized(_driver, "stroke-width", "calc(3)", "calc(3)"));
    try$(_expectSerialized(_driver, "stroke-width", "calc(2 + 1)", "calc(3)"));
    try$(_expectSerialized(_driver, "stroke-width", "calc(2 + (7 - 5))", "calc(4)"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/svg/path/property/serialization.svg
test$("vaev-style-wpt-svg-path-property-serialization") {
    try$(_expectSerialized(_driver, "d", "path(\"m 10 20 q 30 60 40 50 q 100 70 90 80\")", "path(\"m 10 20 q 30 60 40 50 q 100 70 90 80\")"));
    try$(_expectSerialized(_driver, "d", "path(\"M 0 0 L 100 100 m 0 100 l 100 0 Z l 160 20 Z\")", "path(\"M 0 0 L 100 100 m 0 100 l 100 0 Z l 160 20 Z\")"));
    return Ok();
}

test$("vaev-style-wpt-svg-path-property-serialization-xfail", XFAIL) {
    try$(_expectSerialized(_driver, "d", "path(\"m 10 20   l 20 30   Z   l 50 60   Z   m 70 80   l 90 60   Z   t 70 120\")", "path(\"m 10 20 l 20 30 Z l 50 60 Z m 70 80 l 90 60 Z t 70 120\")"));
    try$(_expectSerialized(_driver, "d", "path(\"m 10.0 170.0 h 90.00 v 30.00 m 0 0 s 1 2 3 4 z c 9 8 7 6 5 4\")", "path(\"m 10 170 h 90 v 30 m 0 0 s 1 2 3 4 Z c 9 8 7 6 5 4\")"));
    try$(_expectSerialized(_driver, "d", "  path(  \"m 10 20 a 10 20 30 1 0 40 50 a 110 120 30 1 1 140 50\"  )", "path(\"m 10 20 a 10 20 30 1 0 40 50 a 110 120 30 1 1 140 50\")"));
    return Ok();
}

} // namespace Vaev::Style::Tests
