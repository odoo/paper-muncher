#include <karm/test>

import Vaev.Engine;

using namespace Karm;
using namespace Karm::Literals;

namespace Vaev::Style::Tests {

static Res<String> _format(Str input) {
    Io::StringWriter w;
    try$(serialize(w, try$(parseSelector(input))));
    return Ok(w.take());
}

test$("selector-formatting") {
    assertEq$(try$(_format(".a")), ".a"s);
    assertEq$(try$(_format(".a .b")), ".a .b"s);
    assertEq$(try$(_format(".a.b")), ".a.b"s);
    assertEq$(try$(_format(".a,.b")), ".a, .b"s);
    assertEq$(try$(_format(".a>.b")), ".a > .b"s);
    assertEq$(try$(_format(".a~.b")), ".a ~ .b"s);
    assertEq$(try$(_format(".a+.b")), ".a + .b"s);
    assertEq$(try$(_format("div#a.b[c]:hover::before")), "div#a.b[c]:hover::before"s);
    assertEq$(try$(_format("[a='b' i]")), "[a=\"b\" i]"s);
    assertEq$(try$(_format("|a")), "|a"s);
    assertEq$(try$(_format(":not(.a, .b)")), ":not(.a, .b)"s);
    assertEq$(try$(_format(":is(.a)")), ":is(.a)"s);
    assertEq$(try$(_format(":where(.a)")), ":where(.a)"s);
    assertEq$(try$(_format(":has(> .a, .b + .c)")), ":has(> .a, .b + .c)"s);
    assertEq$(try$(_format(":first-child:last-of-type")), ":first-child:last-of-type"s);
    assertEq$(try$(_format(":nth-child(2n+1 of .a)")), ":nth-child(2n+1 of .a)"s);
    assertEq$(try$(_format(":nth-last-of-type(3)")), ":nth-last-of-type(0n+3)"s);
    assertEq$(try$(_format(":dir(rtl)")), ":dir(rtl)"s);
    assertEq$(try$(_format(":lang(en, \"*-CH\")")), ":lang(\"en\", \"*-CH\")"s);

    return Ok();
}

} // namespace Vaev::Style::Tests
