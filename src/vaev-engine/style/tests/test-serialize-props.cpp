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
    // https://drafts.csswg.org/css-writing-modes-4/#propdef-writing-mode
    try$(_expectSerialized(_driver, "writing-mode", "sideways-rl", "sideways-rl"));
    try$(_expectSerialized(_driver, "writing-mode", "sideways-lr", "sideways-lr"));

    return Ok();
}

} // namespace Vaev::Style::Tests
