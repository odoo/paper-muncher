#include <karm/test>

import Vaev.Engine;

using namespace Karm;
using namespace Karm::Literals;

namespace Vaev::Style::Tests {

// FIXME: Shorthands expand to several longhands, serializing them back
//        into a shorthand is not implemented yet.
static Res<Rc<Property>> _parseLonghand(Str name, Str input) {
    Vec<Rc<Property>> properties;
    try$(parseProperty(Symbol::from(name), input, properties));
    if (properties.len() != 1)
        return Error::notImplemented("shorthand serialization");
    return Ok(properties[0]);
}

static Res<> _expectSerialized(Test::Driver& _driver, Str name, Str input, Str expected) {
    auto prop = try$(_parseLonghand(name, input));
    Io::StringWriter w;
    try$(prop->serialize(w));
    assertEq$(w.take(), expected);
    return Ok();
}

test$("vaev-style-serialize-props") {
    // https://drafts.csswg.org/css-writing-modes-4/#propdef-writing-mode
    try$(_expectSerialized(_driver, "writing-mode", "sideways-rl", "sideways-rl"));
    try$(_expectSerialized(_driver, "writing-mode", "sideways-lr", "sideways-lr"));

    // https://drafts.csswg.org/css-tables-3/#border-spacing-property
    try$(_expectSerialized(_driver, "border-spacing", "10px 20px", "10px 20px"));
    try$(_expectSerialized(_driver, "border-spacing", "10px 10px", "10px"));

    // https://drafts.csswg.org/css-lists-3/#marker-side
    try$(_expectSerialized(_driver, "marker-side", "match-self", "match-self"));
    try$(_expectSerialized(_driver, "marker-side", "match-parent", "match-parent"));

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

    // https://drafts.csswg.org/css-multicol-2/#cw
    // https://drafts.csswg.org/css-values-4/#lengths
    try$(_expectSerialized(_driver, "column-width", "1in", "1in"));
    try$(_expectSerialized(_driver, "column-width", "2rem", "2rem"));
    try$(_expectSerialized(_driver, "column-width", "5vw", "5vw"));
    // https://drafts.csswg.org/css-values-4/#sort-a-calculations-children
    try$(_expectSerialized(_driver, "column-width", "calc(2em + 10px)", "calc(2em + 10px)"));

    // https://drafts.csswg.org/css-multicol-2/#ch
    // https://drafts.csswg.org/css-values-4/#lengths
    try$(_expectSerialized(_driver, "column-height", "1in", "1in"));
    try$(_expectSerialized(_driver, "column-height", "2rem", "2rem"));
    try$(_expectSerialized(_driver, "column-height", "5vw", "5vw"));

    // https://drafts.csswg.org/css-multicol-2/#columns

    return Ok();
}

test$("vaev-style-serialize-special-props") {
    // https://drafts.csswg.org/css-variables-1/#serializing-custom-props
    try$(_expectSerialized(_driver, "--size", "10px", "10px"));
    try$(_expectSerialized(_driver, "--size", "1.0px", "1.0px"));

    return Ok();
}

} // namespace Vaev::Style::Tests
