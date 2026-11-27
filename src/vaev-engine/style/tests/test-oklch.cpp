#include <karm/test>

import Vaev.Engine;
import Karm.Gfx;

using namespace Karm;

namespace Vaev::Style::Tests {

static Gfx::Color _used(Str input) {
    return toUsed(Css::parseValue<Css::Color>(input).take(), Gfx::BLACK);
}

// https://drafts.csswg.org/css-color-4/#css-gamut-mapping
// NOTE: The expected values were computed with color.js 0.5.2 (toGamut with method "css").
test$("vaev-style-oklch-used-value-is-gamut-mapped") {
    assertEq$(_used("oklch(0.76 0.27 60)"), Gfx::Color::fromRgba(255, 140, 0, 255));
    assertEq$(_used("oklch(0.5 0.3 150)"), Gfx::Color::fromRgba(0, 124, 37, 255));
    assertEq$(_used("oklch(0.9 0.2 270)"), Gfx::Color::fromRgba(204, 221, 255, 255));
    assertEq$(_used("oklch(0.3 0.25 330)"), Gfx::Color::fromRgba(84, 0, 82, 255));

    return Ok();
}

// https://drafts.csswg.org/css-color-4/#missing
// For all other purposes, a missing component behaves as a zero value
test$("vaev-style-oklch-used-value-missing-components") {
    assertEq$(_used("oklch(none none none / none)"), Gfx::Color::fromRgba(0, 0, 0, 0));
    assertEq$(_used("oklch(0.5 0 none / 0.5)"), Gfx::Color::fromRgba(99, 99, 99, 128));

    return Ok();
}

} // namespace Vaev::Style::Tests
