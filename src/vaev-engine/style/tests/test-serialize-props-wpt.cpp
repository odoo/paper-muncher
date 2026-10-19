/* Ported from web-platform-tests @ ba6d9f67 */
/* https://github.com/web-platform-tests/wpt/tree/ba6d9f678ee91de9f58097921ed34e9b6b6849d3 */

/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * The 3-Clause BSD License
 *
 * Copyright © web-platform-tests contributors
 *
 * Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
 * following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following
 * disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the
 * following disclaimer in the documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote
 * products derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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

test$("vaev-style-serialize-props-wpt") {
    // https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-align/gaps/gap-parsing-002.html
    try$(_expectSerialized(_driver, "column-gap", "10px", "10px"));

    // https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-align/parsing/align-content-valid.html
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

    // https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-align/parsing/align-items-valid.html
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

    // https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-align/parsing/align-self-valid.html
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

    // https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-align/parsing/column-gap-computed.html
    try$(_expectSerialized(_driver, "column-gap", "40%", "40%"));
    try$(_expectSerialized(_driver, "column-gap", "calc(50% + 60px)", "calc(50% + 60px)"));

    // https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-align/parsing/column-gap-valid.html
    try$(_expectSerialized(_driver, "column-gap", "normal", "normal"));
    try$(_expectSerialized(_driver, "column-gap", "0", "0px"));
    try$(_expectSerialized(_driver, "column-gap", "1px", "1px"));
    try$(_expectSerialized(_driver, "column-gap", "calc(2em + 3ex)", "calc(2em + 3ex)"));
    try$(_expectSerialized(_driver, "column-gap", "4%", "4%"));
    try$(_expectSerialized(_driver, "column-gap", "5vmin", "5vmin"));

    // https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-align/parsing/grid-column-gap-computed.html
    try$(_expectSerialized(_driver, "grid-column-gap", "40%", "40%"));
    try$(_expectSerialized(_driver, "grid-column-gap", "calc(50% + 60px)", "calc(50% + 60px)"));

    // https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-align/parsing/grid-column-gap-valid.html
    try$(_expectSerialized(_driver, "grid-column-gap", "normal", "normal"));
    try$(_expectSerialized(_driver, "grid-column-gap", "0", "0px"));
    try$(_expectSerialized(_driver, "grid-column-gap", "1px", "1px"));
    try$(_expectSerialized(_driver, "grid-column-gap", "calc(2em + 3ex)", "calc(2em + 3ex)"));
    try$(_expectSerialized(_driver, "grid-column-gap", "4%", "4%"));
    try$(_expectSerialized(_driver, "grid-column-gap", "5vmin", "5vmin"));

    // https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-align/parsing/grid-row-gap-valid.html
    try$(_expectSerialized(_driver, "grid-row-gap", "normal", "normal"));
    try$(_expectSerialized(_driver, "grid-row-gap", "10px", "10px"));
    try$(_expectSerialized(_driver, "grid-row-gap", "20%", "20%"));
    try$(_expectSerialized(_driver, "grid-row-gap", "calc(20% + 10px)", "calc(20% + 10px)"));
    try$(_expectSerialized(_driver, "grid-row-gap", "0", "0px"));

    // https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-align/parsing/justify-content-valid.html
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

    // https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-align/parsing/justify-items-computed.html
    try$(_expectSerialized(_driver, "justify-items", "legacy left", "legacy left"));
    try$(_expectSerialized(_driver, "justify-items", "legacy right", "legacy right"));
    try$(_expectSerialized(_driver, "justify-items", "legacy center", "legacy center"));

    // https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-align/parsing/justify-items-valid.html
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

    // https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-align/parsing/justify-self-valid.html
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

    // https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-align/parsing/row-gap-valid.html
    try$(_expectSerialized(_driver, "row-gap", "normal", "normal"));
    try$(_expectSerialized(_driver, "row-gap", "10px", "10px"));
    try$(_expectSerialized(_driver, "row-gap", "20%", "20%"));
    try$(_expectSerialized(_driver, "row-gap", "calc(20% + 10px)", "calc(20% + 10px)"));
    try$(_expectSerialized(_driver, "row-gap", "0", "0px"));

    // https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-backgrounds/parsing/background-color-valid.html
    try$(_expectSerialized(_driver, "background-color", "#00FF00", "rgb(0, 255, 0)"));
    try$(_expectSerialized(_driver, "background-color", "rgb(0, 0, 255)", "rgb(0, 0, 255)"));
    try$(_expectSerialized(_driver, "background-color", "rgb(100%, 100%, 0%)", "rgb(255, 255, 0)"));
    try$(_expectSerialized(_driver, "background-color", "hsl(120, 100%, 50%)", "rgb(0, 255, 0)"));

    // https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-backgrounds/parsing/background-computed.html
    try$(_expectSerialized(_driver, "background-color", "rgb(255, 0, 0)", "rgb(255, 0, 0)"));

    // https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-backgrounds/parsing/border-radius-computed.html
    try$(_expectSerialized(_driver, "border-top-left-radius", "40px 0px", "40px 0px"));

    // https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-backgrounds/parsing/border-radius-valid.html
    try$(_expectSerialized(_driver, "border-radius", "initial", "initial", RegisteredPropertySet::TOP_LEVEL));
    try$(_expectSerialized(_driver, "border-radius", "inherit", "inherit", RegisteredPropertySet::TOP_LEVEL));
    try$(_expectSerialized(_driver, "border-radius", "unset", "unset", RegisteredPropertySet::TOP_LEVEL));
    try$(_expectSerialized(_driver, "border-radius", "revert", "revert", RegisteredPropertySet::TOP_LEVEL));
    try$(_expectSerialized(_driver, "border-bottom-right-radius", "30px 40%", "30px 40%"));
    try$(_expectSerialized(_driver, "border-bottom-left-radius", "50% 60px", "50% 60px"));

    // https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-backgrounds/parsing/border-style-valid.html
    try$(_expectSerialized(_driver, "border-top-style", "solid", "solid"));
    try$(_expectSerialized(_driver, "border-right-style", "double", "double"));
    try$(_expectSerialized(_driver, "border-bottom-style", "groove", "groove"));
    try$(_expectSerialized(_driver, "border-left-style", "ridge", "ridge"));

    // https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-backgrounds/parsing/border-width-computed.html
    try$(_expectSerialized(_driver, "border-right-width", "10px", "10px"));

    // https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-backgrounds/parsing/border-width-valid.html
    try$(_expectSerialized(_driver, "border-top-width", "0px", "0px"));
    try$(_expectSerialized(_driver, "border-right-width", "thin", "thin"));
    try$(_expectSerialized(_driver, "border-left-width", "thick", "thick"));

    // https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-box/parsing/clear-valid.html
    try$(_expectSerialized(_driver, "clear", "none", "none"));
    try$(_expectSerialized(_driver, "clear", "left", "left"));
    try$(_expectSerialized(_driver, "clear", "right", "right"));
    try$(_expectSerialized(_driver, "clear", "both", "both"));
    try$(_expectSerialized(_driver, "clear", "inline-start", "inline-start"));
    try$(_expectSerialized(_driver, "clear", "inline-end", "inline-end"));

    // https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-box/parsing/float-valid.html
    try$(_expectSerialized(_driver, "float", "left", "left"));
    try$(_expectSerialized(_driver, "float", "right", "right"));
    try$(_expectSerialized(_driver, "float", "none", "none"));
    try$(_expectSerialized(_driver, "float", "inline-start", "inline-start"));
    try$(_expectSerialized(_driver, "float", "inline-end", "inline-end"));

    // https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-box/parsing/height-valid.html
    try$(_expectSerialized(_driver, "height", "auto", "auto"));
    try$(_expectSerialized(_driver, "height", "10px", "10px"));
    try$(_expectSerialized(_driver, "height", "20%", "20%"));
    try$(_expectSerialized(_driver, "height", "calc(2em + 3ex)", "calc(2em + 3ex)"));
    try$(_expectSerialized(_driver, "height", "min-content", "min-content"));
    try$(_expectSerialized(_driver, "height", "max-content", "max-content"));
    try$(_expectSerialized(_driver, "height", "fit-content", "fit-content"));

    // https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-box/parsing/margin-computed.html
    try$(_expectSerialized(_driver, "margin-right", "20px", "20px"));
    try$(_expectSerialized(_driver, "margin-bottom", "30px", "30px"));
    try$(_expectSerialized(_driver, "margin-left", "40px", "40px"));

    // https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-box/parsing/margin-valid.html
    try$(_expectSerialized(_driver, "margin-top", "10px", "10px"));
    try$(_expectSerialized(_driver, "margin-right", "20%", "20%"));
    try$(_expectSerialized(_driver, "margin-bottom", "calc(2em + 3ex)", "calc(2em + 3ex)"));
    try$(_expectSerialized(_driver, "margin-left", "auto", "auto"));
    try$(_expectSerialized(_driver, "margin-top", "-10px", "-10px"));
    try$(_expectSerialized(_driver, "margin-right", "-20%", "-20%"));

    // https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-box/parsing/max-height-valid.html
    try$(_expectSerialized(_driver, "max-height", "none", "none"));
    try$(_expectSerialized(_driver, "max-height", "10px", "10px"));
    try$(_expectSerialized(_driver, "max-height", "20%", "20%"));
    try$(_expectSerialized(_driver, "max-height", "calc(2em + 3ex)", "calc(2em + 3ex)"));
    try$(_expectSerialized(_driver, "max-height", "min-content", "min-content"));
    try$(_expectSerialized(_driver, "max-height", "max-content", "max-content"));
    try$(_expectSerialized(_driver, "max-height", "fit-content", "fit-content"));

    // https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-box/parsing/max-width-valid.html
    try$(_expectSerialized(_driver, "max-width", "none", "none"));
    try$(_expectSerialized(_driver, "max-width", "10px", "10px"));
    try$(_expectSerialized(_driver, "max-width", "20%", "20%"));
    try$(_expectSerialized(_driver, "max-width", "calc(2em + 3ex)", "calc(2em + 3ex)"));
    try$(_expectSerialized(_driver, "max-width", "min-content", "min-content"));
    try$(_expectSerialized(_driver, "max-width", "max-content", "max-content"));
    try$(_expectSerialized(_driver, "max-width", "fit-content", "fit-content"));

    // https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-box/parsing/overflow-valid.html
    try$(_expectSerialized(_driver, "overflow-x", "hidden", "hidden"));

    // https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-box/parsing/padding-valid.html
    try$(_expectSerialized(_driver, "padding-top", "10px", "10px"));
    try$(_expectSerialized(_driver, "padding-right", "20px", "20px"));
    try$(_expectSerialized(_driver, "padding-bottom", "30px", "30px"));
    try$(_expectSerialized(_driver, "padding-left", "40px", "40px"));
    try$(_expectSerialized(_driver, "padding-right", "20%", "20%"));

    // https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-box/parsing/visibility-valid.html
    try$(_expectSerialized(_driver, "visibility", "visible", "visible"));
    try$(_expectSerialized(_driver, "visibility", "hidden", "hidden"));
    try$(_expectSerialized(_driver, "visibility", "collapse", "collapse"));

    // https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-box/parsing/width-valid.html
    try$(_expectSerialized(_driver, "width", "auto", "auto"));
    try$(_expectSerialized(_driver, "width", "10px", "10px"));
    try$(_expectSerialized(_driver, "width", "20%", "20%"));
    try$(_expectSerialized(_driver, "width", "calc(2em + 3ex)", "calc(2em + 3ex)"));
    try$(_expectSerialized(_driver, "width", "min-content", "min-content"));
    try$(_expectSerialized(_driver, "width", "max-content", "max-content"));
    try$(_expectSerialized(_driver, "width", "fit-content", "fit-content"));

    // https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-break/parsing/break-after-valid.html
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

    // https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-break/parsing/break-before-valid.html
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

    // https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-break/parsing/break-inside-valid.html
    try$(_expectSerialized(_driver, "break-inside", "auto", "auto"));
    try$(_expectSerialized(_driver, "break-inside", "avoid", "avoid"));
    try$(_expectSerialized(_driver, "break-inside", "avoid-page", "avoid-page"));
    try$(_expectSerialized(_driver, "break-inside", "avoid-column", "avoid-column"));
    try$(_expectSerialized(_driver, "break-inside", "avoid-region", "avoid-region"));

    // https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-color/parsing/color-valid.html
    try$(_expectSerialized(_driver, "color", "#234", "rgb(34, 51, 68)"));
    try$(_expectSerialized(_driver, "color", "#FEDCBA", "rgb(254, 220, 186)"));
    try$(_expectSerialized(_driver, "color", "rgb(2, 3, 4)", "rgb(2, 3, 4)"));
    try$(_expectSerialized(_driver, "color", "rgb(100%, 0%, 0%)", "rgb(255, 0, 0)"));
    try$(_expectSerialized(_driver, "color", "hsl(120, 100%, 50%)", "rgb(0, 255, 0)"));

    // https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-color/parsing/opacity-valid.html
    try$(_expectSerialized(_driver, "opacity", "1", "1"));
    try$(_expectSerialized(_driver, "opacity", "0.5", "0.5"));
    try$(_expectSerialized(_driver, "opacity", "0", "0"));
    try$(_expectSerialized(_driver, "opacity", "-2", "-2"));
    try$(_expectSerialized(_driver, "opacity", "3", "3"));
    try$(_expectSerialized(_driver, "opacity", "-100%", "-1"));
    try$(_expectSerialized(_driver, "opacity", "50%", "0.5"));
    try$(_expectSerialized(_driver, "opacity", "300%", "3"));

    // https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-display/parsing/display-valid.html
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

    // https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-flexbox/parsing/flex-basis-computed.html
    try$(_expectSerialized(_driver, "flex-basis", "calc(10%)", "10%"));
    try$(_expectSerialized(_driver, "flex-basis", "calc(0% + 10px)", "calc(0% + 10px)"));

    // https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-flexbox/parsing/flex-basis-valid.html
    try$(_expectSerialized(_driver, "flex-basis", "1px", "1px"));
    try$(_expectSerialized(_driver, "flex-basis", "calc(2em + 3ex)", "calc(2em + 3ex)"));
    try$(_expectSerialized(_driver, "flex-basis", "400%", "400%"));
    try$(_expectSerialized(_driver, "flex-basis", "auto", "auto"));
    try$(_expectSerialized(_driver, "flex-basis", "fit-content", "fit-content"));
    try$(_expectSerialized(_driver, "flex-basis", "min-content", "min-content"));
    try$(_expectSerialized(_driver, "flex-basis", "max-content", "max-content"));
    try$(_expectSerialized(_driver, "flex-basis", "content", "content"));

    // https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-flexbox/parsing/flex-direction-valid.html
    try$(_expectSerialized(_driver, "flex-direction", "row", "row"));
    try$(_expectSerialized(_driver, "flex-direction", "row-reverse", "row-reverse"));
    try$(_expectSerialized(_driver, "flex-direction", "column", "column"));
    try$(_expectSerialized(_driver, "flex-direction", "column-reverse", "column-reverse"));

    // https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-flexbox/parsing/flex-grow-computed.html
    try$(_expectSerialized(_driver, "flex-grow", "2.34e+06", "2340000"));
    try$(_expectSerialized(_driver, "flex-grow", "6.78e+08", "678000000"));
    try$(_expectSerialized(_driver, "flex-grow", "0", "0"));

    // https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-flexbox/parsing/flex-grow-valid.html
    try$(_expectSerialized(_driver, "flex-grow", "1", "1"));
    try$(_expectSerialized(_driver, "flex-grow", "23.4e5", "2340000"));
    try$(_expectSerialized(_driver, "flex-grow", "+.678E9", "678000000"));
    try$(_expectSerialized(_driver, "flex-grow", ".0", "0"));

    // https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-flexbox/parsing/flex-shrink-computed.html
    try$(_expectSerialized(_driver, "flex-shrink", "2.34e+06", "2340000"));
    try$(_expectSerialized(_driver, "flex-shrink", "6.78e+08", "678000000"));
    try$(_expectSerialized(_driver, "flex-shrink", "0", "0"));

    // https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-flexbox/parsing/flex-shrink-valid.html
    try$(_expectSerialized(_driver, "flex-shrink", "1", "1"));
    try$(_expectSerialized(_driver, "flex-shrink", "23.4e5", "2340000"));
    try$(_expectSerialized(_driver, "flex-shrink", "+.678E9", "678000000"));
    try$(_expectSerialized(_driver, "flex-shrink", ".0", "0"));

    // https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-flexbox/parsing/flex-wrap-valid.html
    try$(_expectSerialized(_driver, "flex-wrap", "nowrap", "nowrap"));
    try$(_expectSerialized(_driver, "flex-wrap", "wrap", "wrap"));
    try$(_expectSerialized(_driver, "flex-wrap", "wrap-reverse", "wrap-reverse"));

    // https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-fonts/parsing/font-family-computed.html
    try$(_expectSerialized(_driver, "font-family", "serif", "serif"));
    try$(_expectSerialized(_driver, "font-family", "sans-serif", "sans-serif"));
    try$(_expectSerialized(_driver, "font-family", "cursive", "cursive"));
    try$(_expectSerialized(_driver, "font-family", "fantasy", "fantasy"));
    try$(_expectSerialized(_driver, "font-family", "monospace", "monospace"));
    try$(_expectSerialized(_driver, "font-family", "serif, sans-serif, cursive, fantasy, monospace", "serif, sans-serif, cursive, fantasy, monospace"));

    // https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-fonts/parsing/font-family-valid.html
    try$(_expectSerialized(_driver, "font-family", "serif, sans-serif, cursive, fantasy, monospace, system-ui", "serif, sans-serif, cursive, fantasy, monospace, system-ui"));
    try$(_expectSerialized(_driver, "font-family", "Helvetica, Verdana, sans-serif", "Helvetica, Verdana, sans-serif"));
    try$(_expectSerialized(_driver, "font-family", "\"New Century Schoolbook\", serif", "New Century Schoolbook, serif"));

    // https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-fonts/parsing/font-size-valid.html
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

    // https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-fonts/parsing/font-style-valid.html
    try$(_expectSerialized(_driver, "font-style", "normal", "normal"));
    try$(_expectSerialized(_driver, "font-style", "italic", "italic"));
    try$(_expectSerialized(_driver, "font-style", "oblique", "oblique"));
    try$(_expectSerialized(_driver, "font-style", "oblique 0deg", "normal"));
    try$(_expectSerialized(_driver, "font-style", "oblique 10deg", "oblique 10deg"));
    try$(_expectSerialized(_driver, "font-style", "oblique -10deg", "oblique -10deg"));
    try$(_expectSerialized(_driver, "font-style", "oblique -14deg", "oblique -14deg"));
    try$(_expectSerialized(_driver, "font-style", "oblique -90deg", "oblique -90deg"));
    try$(_expectSerialized(_driver, "font-style", "oblique 90deg", "oblique 90deg"));

    // https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-fonts/parsing/font-weight-valid.html
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

    // https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-fonts/parsing/font-width-valid.html
    try$(_expectSerialized(_driver, "font-width", "234.5%", "234.5%"));
    try$(_expectSerialized(_driver, "font-stretch", "234.5%", "234.5%"));

    // https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-inline/baseline-source/baseline-source-valid.html
    try$(_expectSerialized(_driver, "baseline-source", "auto", "auto"));
    try$(_expectSerialized(_driver, "baseline-source", "first", "first"));
    try$(_expectSerialized(_driver, "baseline-source", "last", "last"));

    // https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-inline/parsing/alignment-baseline-valid.html
    try$(_expectSerialized(_driver, "alignment-baseline", "baseline", "baseline"));
    try$(_expectSerialized(_driver, "alignment-baseline", "text-bottom", "text-bottom"));
    try$(_expectSerialized(_driver, "alignment-baseline", "alphabetic", "alphabetic"));
    try$(_expectSerialized(_driver, "alignment-baseline", "ideographic", "ideographic"));
    try$(_expectSerialized(_driver, "alignment-baseline", "middle", "middle"));
    try$(_expectSerialized(_driver, "alignment-baseline", "central", "central"));
    try$(_expectSerialized(_driver, "alignment-baseline", "mathematical", "mathematical"));
    try$(_expectSerialized(_driver, "alignment-baseline", "text-top", "text-top"));

    // https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-inline/parsing/baseline-shift-valid.html
    try$(_expectSerialized(_driver, "baseline-shift", "-10px", "-10px"));
    try$(_expectSerialized(_driver, "baseline-shift", "20%", "20%"));
    try$(_expectSerialized(_driver, "baseline-shift", "calc(2em + 3ex)", "calc(2em + 3ex)"));
    try$(_expectSerialized(_driver, "baseline-shift", "0", "0px"));
    try$(_expectSerialized(_driver, "baseline-shift", "sub", "sub"));
    try$(_expectSerialized(_driver, "baseline-shift", "super", "super"));
    try$(_expectSerialized(_driver, "baseline-shift", "bottom", "bottom"));
    try$(_expectSerialized(_driver, "baseline-shift", "center", "center"));
    try$(_expectSerialized(_driver, "baseline-shift", "top", "top"));

    // https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-inline/parsing/dominant-baseline-valid.html
    try$(_expectSerialized(_driver, "dominant-baseline", "auto", "auto"));
    try$(_expectSerialized(_driver, "dominant-baseline", "text-bottom", "text-bottom"));
    try$(_expectSerialized(_driver, "dominant-baseline", "alphabetic", "alphabetic"));
    try$(_expectSerialized(_driver, "dominant-baseline", "ideographic", "ideographic"));
    try$(_expectSerialized(_driver, "dominant-baseline", "middle", "middle"));
    try$(_expectSerialized(_driver, "dominant-baseline", "central", "central"));
    try$(_expectSerialized(_driver, "dominant-baseline", "mathematical", "mathematical"));
    try$(_expectSerialized(_driver, "dominant-baseline", "text-top", "text-top"));

    // https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-inline/parsing/line-height-valid.html
    try$(_expectSerialized(_driver, "line-height", "normal", "normal"));
    try$(_expectSerialized(_driver, "line-height", "0", "0"));
    try$(_expectSerialized(_driver, "line-height", "2", "2"));
    try$(_expectSerialized(_driver, "line-height", "0px", "0px"));
    try$(_expectSerialized(_driver, "line-height", "10px", "10px"));
    try$(_expectSerialized(_driver, "line-height", "0%", "0%"));
    try$(_expectSerialized(_driver, "line-height", "200%", "200%"));
    try$(_expectSerialized(_driver, "line-height", "calc(200% + 10px)", "calc(200% + 10px)"));

    // https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-lists/parsing/list-style-position-valid.html
    try$(_expectSerialized(_driver, "list-style-position", "inside", "inside"));
    try$(_expectSerialized(_driver, "list-style-position", "outside", "outside"));

    // https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-logical/parsing/margin-block-inline-computed.html
    try$(_expectSerialized(_driver, "margin-block-start", "10px", "10px"));
    try$(_expectSerialized(_driver, "margin-inline-start", "30px", "30px"));

    // https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-logical/parsing/margin-block-inline-valid.html
    try$(_expectSerialized(_driver, "margin-block-start", "calc(20% + 10px)", "calc(20% + 10px)"));
    try$(_expectSerialized(_driver, "margin-block-start", "auto", "auto"));
    try$(_expectSerialized(_driver, "margin-block-end", "-10px", "-10px"));
    try$(_expectSerialized(_driver, "margin-inline-start", "-20%", "-20%"));
    try$(_expectSerialized(_driver, "margin-inline-end", "calc(2em + 3ex)", "calc(2em + 3ex)"));

    // https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-logical/parsing/padding-block-inline-computed.html
    try$(_expectSerialized(_driver, "padding-inline-start", "30px", "30px"));

    // https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-logical/parsing/padding-block-inline-valid.html
    try$(_expectSerialized(_driver, "padding-inline-start", "20%", "20%"));
    try$(_expectSerialized(_driver, "padding-inline-end", "calc(2em + 2ex)", "calc(2em + 2ex)"));

    // https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-overflow/parsing/overflow-computed.html
    try$(_expectSerialized(_driver, "overflow-y", "visible", "visible"));
    try$(_expectSerialized(_driver, "overflow-block", "visible", "visible"));

    // https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-overflow/parsing/overflow-valid.html
    try$(_expectSerialized(_driver, "overflow-x", "visible", "visible"));
    try$(_expectSerialized(_driver, "overflow-x", "scroll", "scroll"));
    try$(_expectSerialized(_driver, "overflow-y", "auto", "auto"));
    try$(_expectSerialized(_driver, "overflow-block", "hidden", "hidden"));
    try$(_expectSerialized(_driver, "overflow-inline", "scroll", "scroll"));
    try$(_expectSerialized(_driver, "overflow-inline", "visible", "visible"));

    // https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-position/parsing/bottom-computed.html
    try$(_expectSerialized(_driver, "bottom", "-40%", "-40%"));
    try$(_expectSerialized(_driver, "bottom", "calc(50% + 60px)", "calc(50% + 60px)"));

    // https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-position/parsing/bottom-valid.html
    try$(_expectSerialized(_driver, "bottom", "auto", "auto"));
    try$(_expectSerialized(_driver, "bottom", "-10px", "-10px"));
    try$(_expectSerialized(_driver, "bottom", "-20%", "-20%"));
    try$(_expectSerialized(_driver, "bottom", "calc(2em + 3ex)", "calc(2em + 3ex)"));

    // https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-position/parsing/left-computed.html
    try$(_expectSerialized(_driver, "left", "-40%", "-40%"));
    try$(_expectSerialized(_driver, "left", "calc(50% + 60px)", "calc(50% + 60px)"));

    // https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-position/parsing/left-valid.html
    try$(_expectSerialized(_driver, "left", "auto", "auto"));
    try$(_expectSerialized(_driver, "left", "-10px", "-10px"));
    try$(_expectSerialized(_driver, "left", "-20%", "-20%"));
    try$(_expectSerialized(_driver, "left", "calc(2em + 3ex)", "calc(2em + 3ex)"));

    // https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-position/parsing/right-computed.html
    try$(_expectSerialized(_driver, "right", "-40%", "-40%"));
    try$(_expectSerialized(_driver, "right", "calc(50% + 60px)", "calc(50% + 60px)"));

    // https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-position/parsing/right-valid.html
    try$(_expectSerialized(_driver, "right", "auto", "auto"));
    try$(_expectSerialized(_driver, "right", "-10px", "-10px"));
    try$(_expectSerialized(_driver, "right", "-20%", "-20%"));
    try$(_expectSerialized(_driver, "right", "calc(2em + 3ex)", "calc(2em + 3ex)"));

    // https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-position/parsing/top-computed.html
    try$(_expectSerialized(_driver, "top", "-40%", "-40%"));
    try$(_expectSerialized(_driver, "top", "calc(50% + 60px)", "calc(50% + 60px)"));

    // https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-position/parsing/top-valid.html
    try$(_expectSerialized(_driver, "top", "auto", "auto"));
    try$(_expectSerialized(_driver, "top", "-10px", "-10px"));
    try$(_expectSerialized(_driver, "top", "-20%", "-20%"));
    try$(_expectSerialized(_driver, "top", "calc(2em + 3ex)", "calc(2em + 3ex)"));

    // https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-position/parsing/z-index-positioned-computed.html
    try$(_expectSerialized(_driver, "z-index", "-1", "-1"));

    // https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-position/parsing/z-index-valid.html
    try$(_expectSerialized(_driver, "z-index", "auto", "auto"));
    try$(_expectSerialized(_driver, "z-index", "-789", "-789"));
    try$(_expectSerialized(_driver, "z-index", "0", "0"));
    try$(_expectSerialized(_driver, "z-index", "123", "123"));

    // https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-sizing/parsing/height-valid.html
    try$(_expectSerialized(_driver, "height", "0", "0px"));
    try$(_expectSerialized(_driver, "height", "10%", "10%"));
    try$(_expectSerialized(_driver, "height", "0.5em", "0.5em"));
    try$(_expectSerialized(_driver, "height", "calc(10% - 0.5em)", "calc(10% - 0.5em)"));

    // https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-sizing/parsing/max-height-computed.html
    try$(_expectSerialized(_driver, "max-height", "calc(10% + 40px)", "calc(10% + 40px)"));

    // https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-sizing/parsing/max-height-valid.html
    try$(_expectSerialized(_driver, "max-height", "0", "0px"));
    try$(_expectSerialized(_driver, "max-height", "10%", "10%"));
    try$(_expectSerialized(_driver, "max-height", "0.5em", "0.5em"));
    try$(_expectSerialized(_driver, "max-height", "calc(10% - 0.5em)", "calc(10% - 0.5em)"));

    // https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-sizing/parsing/max-width-computed.html
    try$(_expectSerialized(_driver, "max-width", "calc(10% + 40px)", "calc(10% + 40px)"));

    // https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-sizing/parsing/max-width-valid.html
    try$(_expectSerialized(_driver, "max-width", "0", "0px"));
    try$(_expectSerialized(_driver, "max-width", "10%", "10%"));
    try$(_expectSerialized(_driver, "max-width", "0.5em", "0.5em"));
    try$(_expectSerialized(_driver, "max-width", "calc(10% - 0.5em)", "calc(10% - 0.5em)"));

    // https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-sizing/parsing/min-height-computed.html
    try$(_expectSerialized(_driver, "min-height", "10px", "10px"));
    try$(_expectSerialized(_driver, "min-height", "20%", "20%"));
    try$(_expectSerialized(_driver, "min-height", "calc(10% + 40px)", "calc(10% + 40px)"));

    // https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-sizing/parsing/min-height-valid.html
    try$(_expectSerialized(_driver, "min-height", "auto", "auto"));
    try$(_expectSerialized(_driver, "min-height", "min-content", "min-content"));
    try$(_expectSerialized(_driver, "min-height", "max-content", "max-content"));
    try$(_expectSerialized(_driver, "min-height", "0", "0px"));
    try$(_expectSerialized(_driver, "min-height", "10%", "10%"));
    try$(_expectSerialized(_driver, "min-height", "0.5em", "0.5em"));
    try$(_expectSerialized(_driver, "min-height", "calc(10% - 0.5em)", "calc(10% - 0.5em)"));

    // https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-sizing/parsing/min-width-computed.html
    try$(_expectSerialized(_driver, "min-width", "10px", "10px"));
    try$(_expectSerialized(_driver, "min-width", "20%", "20%"));
    try$(_expectSerialized(_driver, "min-width", "calc(10% + 40px)", "calc(10% + 40px)"));

    // https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-sizing/parsing/min-width-valid.html
    try$(_expectSerialized(_driver, "min-width", "auto", "auto"));
    try$(_expectSerialized(_driver, "min-width", "min-content", "min-content"));
    try$(_expectSerialized(_driver, "min-width", "max-content", "max-content"));
    try$(_expectSerialized(_driver, "min-width", "0", "0px"));
    try$(_expectSerialized(_driver, "min-width", "10%", "10%"));
    try$(_expectSerialized(_driver, "min-width", "0.5em", "0.5em"));
    try$(_expectSerialized(_driver, "min-width", "calc(10% - 0.5em)", "calc(10% - 0.5em)"));

    // https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-sizing/parsing/width-valid.html
    try$(_expectSerialized(_driver, "width", "0", "0px"));
    try$(_expectSerialized(_driver, "width", "10%", "10%"));
    try$(_expectSerialized(_driver, "width", "0.5em", "0.5em"));
    try$(_expectSerialized(_driver, "width", "calc(10% - 0.5em)", "calc(10% - 0.5em)"));

    // https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-sizing/stretch/parsing.html
    try$(_expectSerialized(_driver, "width", "stretch", "stretch"));
    try$(_expectSerialized(_driver, "height", "stretch", "stretch"));
    try$(_expectSerialized(_driver, "max-width", "stretch", "stretch"));
    try$(_expectSerialized(_driver, "min-width", "stretch", "stretch"));
    try$(_expectSerialized(_driver, "max-height", "stretch", "stretch"));
    try$(_expectSerialized(_driver, "min-height", "stretch", "stretch"));

    // https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-tables/parsing/border-collapse-valid.html
    try$(_expectSerialized(_driver, "border-collapse", "separate", "separate"));
    try$(_expectSerialized(_driver, "border-collapse", "collapse", "collapse"));

    // https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-tables/parsing/border-spacing-computed.html
    try$(_expectSerialized(_driver, "border-spacing", "0", "0px"));

    // https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-tables/parsing/border-spacing-valid.html
    try$(_expectSerialized(_driver, "border-spacing", "0px", "0px"));
    try$(_expectSerialized(_driver, "border-spacing", "10px 20px", "10px 20px"));

    // https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-tables/parsing/caption-side-valid.html
    try$(_expectSerialized(_driver, "caption-side", "top", "top"));
    try$(_expectSerialized(_driver, "caption-side", "bottom", "bottom"));

    // https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-tables/parsing/table-layout-valid.html
    try$(_expectSerialized(_driver, "table-layout", "auto", "auto"));
    try$(_expectSerialized(_driver, "table-layout", "fixed", "fixed"));

    // https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-text/parsing/text-align-valid.html
    try$(_expectSerialized(_driver, "text-align", "start", "start"));
    try$(_expectSerialized(_driver, "text-align", "end", "end"));
    try$(_expectSerialized(_driver, "text-align", "left", "left"));
    try$(_expectSerialized(_driver, "text-align", "right", "right"));
    try$(_expectSerialized(_driver, "text-align", "center", "center"));
    try$(_expectSerialized(_driver, "text-align", "justify", "justify"));

    // https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-text/parsing/text-transform-valid.html
    try$(_expectSerialized(_driver, "text-transform", "none", "none"));
    try$(_expectSerialized(_driver, "text-transform", "uppercase", "uppercase"));

    // https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-text/parsing/white-space-valid.html
    try$(_expectSerialized(_driver, "white-space", "normal", "normal"));
    try$(_expectSerialized(_driver, "white-space", "pre", "pre"));
    try$(_expectSerialized(_driver, "white-space", "nowrap", "nowrap"));
    try$(_expectSerialized(_driver, "white-space", "pre-wrap", "pre-wrap"));
    try$(_expectSerialized(_driver, "white-space", "break-spaces", "break-spaces"));
    try$(_expectSerialized(_driver, "white-space", "pre-line", "pre-line"));

    // https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-transforms/parsing/transform-box-valid.html
    try$(_expectSerialized(_driver, "transform-box", "content-box", "content-box"));
    try$(_expectSerialized(_driver, "transform-box", "border-box", "border-box"));
    try$(_expectSerialized(_driver, "transform-box", "fill-box", "fill-box"));
    try$(_expectSerialized(_driver, "transform-box", "stroke-box", "stroke-box"));
    try$(_expectSerialized(_driver, "transform-box", "view-box", "view-box"));

    // https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-transforms/parsing/transform-origin-valid.html
    try$(_expectSerialized(_driver, "transform-origin", "left center", "left center"));
    try$(_expectSerialized(_driver, "transform-origin", "center top", "center top"));
    try$(_expectSerialized(_driver, "transform-origin", "right -4%", "right -4%"));

    // https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-ui/parsing/box-sizing-valid.html
    try$(_expectSerialized(_driver, "box-sizing", "content-box", "content-box"));
    try$(_expectSerialized(_driver, "box-sizing", "border-box", "border-box"));

    // https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-ui/parsing/outline-color-computed.html
    try$(_expectSerialized(_driver, "outline-color", "red", "rgb(255, 0, 0)"));

    // https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-ui/parsing/outline-color-valid.html
    try$(_expectSerialized(_driver, "outline-color", "rgba(10, 20, 30, 0.4)", "rgba(10, 20, 30, 0.4)"));
    try$(_expectSerialized(_driver, "outline-color", "auto", "auto"));

    // https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-ui/parsing/outline-offset-computed.html
    try$(_expectSerialized(_driver, "outline-offset", "2.5px", "2.5px"));
    try$(_expectSerialized(_driver, "outline-offset", "10px", "10px"));

    // https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-ui/parsing/outline-offset-valid.html
    try$(_expectSerialized(_driver, "outline-offset", "0", "0px"));
    try$(_expectSerialized(_driver, "outline-offset", "1px", "1px"));
    try$(_expectSerialized(_driver, "outline-offset", "2em", "2em"));
    try$(_expectSerialized(_driver, "outline-offset", "calc(3rem + 4vw)", "calc(3rem + 4vw)"));

    // https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-ui/parsing/outline-style-valid.html
    try$(_expectSerialized(_driver, "outline-style", "auto", "auto"));
    try$(_expectSerialized(_driver, "outline-style", "none", "none"));
    try$(_expectSerialized(_driver, "outline-style", "dotted", "dotted"));
    try$(_expectSerialized(_driver, "outline-style", "dashed", "dashed"));
    try$(_expectSerialized(_driver, "outline-style", "solid", "solid"));
    try$(_expectSerialized(_driver, "outline-style", "double", "double"));
    try$(_expectSerialized(_driver, "outline-style", "groove", "groove"));
    try$(_expectSerialized(_driver, "outline-style", "ridge", "ridge"));
    try$(_expectSerialized(_driver, "outline-style", "outset", "outset"));

    // https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-ui/parsing/outline-width-computed.html
    try$(_expectSerialized(_driver, "outline-width", "10px", "10px"));

    // https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-ui/parsing/outline-width-valid.html
    try$(_expectSerialized(_driver, "outline-width", "0", "0px"));
    try$(_expectSerialized(_driver, "outline-width", "1px", "1px"));
    try$(_expectSerialized(_driver, "outline-width", "2em", "2em"));
    try$(_expectSerialized(_driver, "outline-width", "calc(2em + 3ex)", "calc(2em + 3ex)"));
    try$(_expectSerialized(_driver, "outline-width", "thin", "thin"));
    try$(_expectSerialized(_driver, "outline-width", "medium", "medium"));
    try$(_expectSerialized(_driver, "outline-width", "thick", "thick"));

    // https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/css-variables/var-parsing.html
    try$(_expectSerialized(_driver, "width", "var(--x)", "var(--x)", RegisteredPropertySet::TOP_LEVEL));
    try$(_expectSerialized(_driver, "width", "var(--x,)", "var(--x,)", RegisteredPropertySet::TOP_LEVEL));
    try$(_expectSerialized(_driver, "width", "var(--x, )", "var(--x, )", RegisteredPropertySet::TOP_LEVEL));
    try$(_expectSerialized(_driver, "width", "var(--x ())", "var(--x ())", RegisteredPropertySet::TOP_LEVEL));
    try$(_expectSerialized(_driver, "width", "var(--x () )", "var(--x () )", RegisteredPropertySet::TOP_LEVEL));
    try$(_expectSerialized(_driver, "width", "var(--x() )", "var(--x() )", RegisteredPropertySet::TOP_LEVEL));
    try$(_expectSerialized(_driver, "width", "var(--x (),)", "var(--x (),)", RegisteredPropertySet::TOP_LEVEL));
    try$(_expectSerialized(_driver, "width", "var(--x(),)", "var(--x(),)", RegisteredPropertySet::TOP_LEVEL));

    // https://github.com/web-platform-tests/wpt/blob/ba6d9f67/css/cssom/serialize-values.html
    try$(_expectSerialized(_driver, "line-height", "-0", "0"));
    try$(_expectSerialized(_driver, "line-height", "1000", "1000"));
    try$(_expectSerialized(_driver, "line-height", "0.9", "0.9"));
    try$(_expectSerialized(_driver, "line-height", "1px", "1px"));
    try$(_expectSerialized(_driver, "line-height", ".1em", "0.1em"));
    try$(_expectSerialized(_driver, "line-height", "5%", "5%"));
    try$(_expectSerialized(_driver, "line-height", ".5%", "0.5%"));
    try$(_expectSerialized(_driver, "line-height", "inherit", "inherit", RegisteredPropertySet::TOP_LEVEL));

    // https://github.com/web-platform-tests/wpt/blob/ba6d9f67/svg/geometry/parsing/cx-computed.svg
    try$(_expectSerialized(_driver, "cx", "-10px", "-10px"));
    try$(_expectSerialized(_driver, "cx", "40%", "40%"));

    // https://github.com/web-platform-tests/wpt/blob/ba6d9f67/svg/geometry/parsing/cx-valid.svg
    try$(_expectSerialized(_driver, "cx", "0", "0px"));
    try$(_expectSerialized(_driver, "cx", "-1px", "-1px"));
    try$(_expectSerialized(_driver, "cx", "4%", "4%"));
    try$(_expectSerialized(_driver, "cx", "5ch", "5ch"));

    // https://github.com/web-platform-tests/wpt/blob/ba6d9f67/svg/geometry/parsing/cy-computed.svg
    try$(_expectSerialized(_driver, "cy", "-10px", "-10px"));
    try$(_expectSerialized(_driver, "cy", "40%", "40%"));

    // https://github.com/web-platform-tests/wpt/blob/ba6d9f67/svg/geometry/parsing/cy-valid.svg
    try$(_expectSerialized(_driver, "cy", "0", "0px"));
    try$(_expectSerialized(_driver, "cy", "-1px", "-1px"));
    try$(_expectSerialized(_driver, "cy", "4%", "4%"));
    try$(_expectSerialized(_driver, "cy", "5rem", "5rem"));

    // https://github.com/web-platform-tests/wpt/blob/ba6d9f67/svg/geometry/parsing/r-computed.svg
    try$(_expectSerialized(_driver, "r", "10px", "10px"));
    try$(_expectSerialized(_driver, "r", "40%", "40%"));

    // https://github.com/web-platform-tests/wpt/blob/ba6d9f67/svg/geometry/parsing/r-valid.svg
    try$(_expectSerialized(_driver, "r", "0", "0px"));
    try$(_expectSerialized(_driver, "r", "1px", "1px"));
    try$(_expectSerialized(_driver, "r", "4%", "4%"));
    try$(_expectSerialized(_driver, "r", "5vmin", "5vmin"));

    // https://github.com/web-platform-tests/wpt/blob/ba6d9f67/svg/geometry/parsing/x-computed.svg
    try$(_expectSerialized(_driver, "x", "-10px", "-10px"));
    try$(_expectSerialized(_driver, "x", "40%", "40%"));

    // https://github.com/web-platform-tests/wpt/blob/ba6d9f67/svg/geometry/parsing/x-valid.svg
    try$(_expectSerialized(_driver, "x", "0", "0px"));
    try$(_expectSerialized(_driver, "x", "-1px", "-1px"));
    try$(_expectSerialized(_driver, "x", "4%", "4%"));
    try$(_expectSerialized(_driver, "x", "5cm", "5cm"));

    // https://github.com/web-platform-tests/wpt/blob/ba6d9f67/svg/geometry/parsing/y-computed.svg
    try$(_expectSerialized(_driver, "y", "-10px", "-10px"));
    try$(_expectSerialized(_driver, "y", "40%", "40%"));

    // https://github.com/web-platform-tests/wpt/blob/ba6d9f67/svg/geometry/parsing/y-valid.svg
    try$(_expectSerialized(_driver, "y", "0", "0px"));
    try$(_expectSerialized(_driver, "y", "-1px", "-1px"));
    try$(_expectSerialized(_driver, "y", "4%", "4%"));
    try$(_expectSerialized(_driver, "y", "5mm", "5mm"));

    // https://github.com/web-platform-tests/wpt/blob/ba6d9f67/svg/painting/parsing/fill-opacity-valid.svg
    try$(_expectSerialized(_driver, "fill-opacity", "-1", "-1"));
    try$(_expectSerialized(_driver, "fill-opacity", "0.5", "0.5"));
    try$(_expectSerialized(_driver, "fill-opacity", "3", "3"));
    try$(_expectSerialized(_driver, "fill-opacity", "-100%", "-1"));
    try$(_expectSerialized(_driver, "fill-opacity", "50%", "0.5"));
    try$(_expectSerialized(_driver, "fill-opacity", "300%", "3"));

    // https://github.com/web-platform-tests/wpt/blob/ba6d9f67/svg/painting/parsing/fill-valid.svg
    try$(_expectSerialized(_driver, "fill", "none", "none"));
    try$(_expectSerialized(_driver, "fill", "rgb(12, 34, 56)", "rgb(12, 34, 56)"));

    // https://github.com/web-platform-tests/wpt/blob/ba6d9f67/svg/painting/parsing/stroke-opacity-valid.svg
    try$(_expectSerialized(_driver, "stroke-opacity", "-1", "-1"));
    try$(_expectSerialized(_driver, "stroke-opacity", "0.5", "0.5"));
    try$(_expectSerialized(_driver, "stroke-opacity", "3", "3"));
    try$(_expectSerialized(_driver, "stroke-opacity", "-100%", "-1"));
    try$(_expectSerialized(_driver, "stroke-opacity", "50%", "0.5"));
    try$(_expectSerialized(_driver, "stroke-opacity", "300%", "3"));

    // https://github.com/web-platform-tests/wpt/blob/ba6d9f67/svg/painting/parsing/stroke-valid.svg
    try$(_expectSerialized(_driver, "stroke", "none", "none"));
    try$(_expectSerialized(_driver, "stroke", "rgb(12, 34, 56)", "rgb(12, 34, 56)"));

    // https://github.com/web-platform-tests/wpt/blob/ba6d9f67/svg/painting/parsing/stroke-width-computed.svg
    try$(_expectSerialized(_driver, "stroke-width", "40%", "40%"));

    // https://github.com/web-platform-tests/wpt/blob/ba6d9f67/svg/painting/parsing/stroke-width-valid.svg
    try$(_expectSerialized(_driver, "stroke-width", "1px", "1px"));
    try$(_expectSerialized(_driver, "stroke-width", "4%", "4%"));
    try$(_expectSerialized(_driver, "stroke-width", "5vmin", "5vmin"));

    // https://github.com/web-platform-tests/wpt/blob/ba6d9f67/svg/path/property/serialization.svg
    try$(_expectSerialized(_driver, "d", "path(\"m 10 20 q 30 60 40 50 q 100 70 90 80\")", "path(\"m 10 20 q 30 60 40 50 q 100 70 90 80\")"));
    try$(_expectSerialized(_driver, "d", "path(\"M 0 0 L 100 100 m 0 100 l 100 0 Z l 160 20 Z\")", "path(\"M 0 0 L 100 100 m 0 100 l 100 0 Z l 160 20 Z\")"));

    return Ok();
}

} // namespace Vaev::Style::Tests
