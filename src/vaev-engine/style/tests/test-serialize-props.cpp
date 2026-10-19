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

test$("vaev-style-serialize-props") {
    // https://drafts.csswg.org/css-tables-3/#border-spacing-property
    try$(_expectSerialized(_driver, "border-spacing", "10px 20px", "10px 20px"));
    try$(_expectSerialized(_driver, "border-spacing", "10px 10px", "10px"));

    // https://drafts.csswg.org/css-backgrounds-3/#border-color
    // https://drafts.csswg.org/css-color-4/#serializing-sRGB-values
    try$(_expectSerialized(_driver, "border-top-color", "rgba(0, 0, 0, 0)", "rgba(0, 0, 0, 0)"));
    try$(_expectSerialized(_driver, "border-right-color", "rgba(0, 0, 0, 0)", "rgba(0, 0, 0, 0)"));
    try$(_expectSerialized(_driver, "border-bottom-color", "rgba(0, 0, 0, 0)", "rgba(0, 0, 0, 0)"));
    try$(_expectSerialized(_driver, "border-left-color", "rgba(0, 0, 0, 0)", "rgba(0, 0, 0, 0)"));

    // https://drafts.csswg.org/css-backgrounds-3/#border-radius
    try$(_expectSerialized(_driver, "border-top-right-radius", "2em 1em", "2em 1em"));

    // https://svgwg.org/svg2-draft/painting.html#SpecifyingPaint
    // https://drafts.csswg.org/css-color-4/#serializing-sRGB-values
    try$(_expectSerialized(_driver, "fill", "rgba(0, 0, 0, 0)", "rgba(0, 0, 0, 0)"));

    // https://drafts.csswg.org/cssom/#serialize-a-css-component-value
    try$(_expectSerialized(_driver, "d", "path('M 1 2')", "path(\"M 1 2\")"));

    return Ok();
}

test$("vaev-style-serialize-special-props") {
    // https://drafts.csswg.org/css-variables-1/#serializing-custom-props
    try$(_expectSerialized(_driver, "--size", "10px", "10px", RegisteredPropertySet::TOP_LEVEL));
    try$(_expectSerialized(_driver, "--size", "1.0px", "1.0px", RegisteredPropertySet::TOP_LEVEL));

    return Ok();
}

} // namespace Vaev::Style::Tests
