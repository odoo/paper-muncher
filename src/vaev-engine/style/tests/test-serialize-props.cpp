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
    try$(_expectSerialized(_driver, "columns", "calc(2em + 10px) 3", "calc(2em + 10px) 3"));
    try$(_expectSerialized(_driver, "columns", "3 calc(2em + 10px)", "calc(2em + 10px) 3"));

    return Ok();
}

} // namespace Vaev::Style::Tests
