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
import Karm.Gfx;

using namespace Karm;

namespace Vaev::Style::Tests {

// NOTE: oklch-008.html is not ported, its reference is a display-p3 color.

static Res<> _expectSameUsed(Test::Driver& _driver, Str test, Str ref) {
    auto testColor = try$(Css::parseValue<Css::Color>(test));
    auto refColor = try$(Css::parseValue<Css::Color>(ref));
    assertEq$(toUsed(testColor, Gfx::BLACK), toUsed(refColor, Gfx::BLACK));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-color/oklch-001.html
test$("vaev-style-wpt-css-css-color-oklch-001") {
    try$(_expectSameUsed(_driver, "oklch(51.975% 0.17686 142.495)", "#008000"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-color/oklch-002.html
test$("vaev-style-wpt-css-css-color-oklch-002") {
    try$(_expectSameUsed(_driver, "oklch(0% 0 0)", "#000000"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-color/oklch-003.html
test$("vaev-style-wpt-css-css-color-oklch-003") {
    try$(_expectSameUsed(_driver, "oklch(100% 0 0)", "#FFFFFF"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-color/oklch-004.html
test$("vaev-style-wpt-css-css-color-oklch-004") {
    try$(_expectSameUsed(_driver, "oklch(50% 0.2 0)", "rgb(70.492% 2.351% 37.073%)"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-color/oklch-005.html
test$("vaev-style-wpt-css-css-color-oklch-005") {
    try$(_expectSameUsed(_driver, "oklch(50% 0.2 270)", "rgb(23.056% 31.73% 82.628%)"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-color/oklch-006.html
test$("vaev-style-wpt-css-css-color-oklch-006") {
    try$(_expectSameUsed(_driver, "oklch(80% 0.15 160)", "rgb(32.022% 85.805% 61.147%)"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-color/oklch-007.html
test$("vaev-style-wpt-css-css-color-oklch-007") {
    try$(_expectSameUsed(_driver, "oklch(55% 0.15 345)", "rgb(67.293% 27.791% 52.28%)"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-color/oklch-009.html
test$("vaev-style-wpt-css-css-color-oklch-009") {
    try$(_expectSameUsed(_driver, "oklch(100% 110 60)", "#FFFFFF"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-color/oklch-010.html
test$("vaev-style-wpt-css-css-color-oklch-010") {
    try$(_expectSameUsed(_driver, "oklch(0% 1.1 60)", "#000000"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-color/oklch-011.html
test$("vaev-style-wpt-css-css-color-oklch-011") {
    try$(_expectSameUsed(_driver, "oklch(51.975% 44.215% 142.495)", "#008000"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-color/oklch-l-almost-0.html
test$("vaev-style-wpt-css-css-color-oklch-l-almost-0") {
    try$(_expectSameUsed(_driver, "oklch(0.0001% 0.2 45)", "oklch(0 0.2 45)"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-color/oklch-l-almost-1.html
// FIXME: The binary search gamut mapping maps it to rgb(255, 251, 242), not to white.
test$("vaev-style-wpt-css-css-color-oklch-l-almost-1-xfail", XFAIL) {
    try$(_expectSameUsed(_driver, "oklch(99.9999% 0.2 45)", "oklch(1 0.2 45)"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-color/oklch-l-over-1-1.html
test$("vaev-style-wpt-css-css-color-oklch-l-over-1-1") {
    try$(_expectSameUsed(_driver, "oklch(1.5 0.5 50)", "oklch(1 0.5 50)"));
    return Ok();
}

// https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-color/oklch-l-over-1-2.html
test$("vaev-style-wpt-css-css-color-oklch-l-over-1-2") {
    try$(_expectSameUsed(_driver, "oklch(150% 0.5 50)", "oklch(1 0.5 50)"));
    return Ok();
}

} // namespace Vaev::Style::Tests
