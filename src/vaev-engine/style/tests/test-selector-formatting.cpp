#include <karm/test>

import Vaev.Engine;

using namespace Karm;
using namespace Karm::Literals;

namespace Vaev::Style::Tests {

test$("selector-formatting") {

    auto sel = try$(Selector::parse(".a"));
    assertEq$(
        ".a"s,
        Io::toStr(sel.unparsed())
    );

    sel = try$(Selector::parse(".a .b"));
    assertEq$(
        ".a .b"s,
        Io::toStr(sel.unparsed())
    );

    sel = try$(Selector::parse(".a.b"));
    assertEq$(
        ".a.b"s,
        Io::toStr(sel.unparsed())
    );

    sel = try$(Selector::parse(".a,.b"));
    assertEq$(
        ".a,.b"s,
        Io::toStr(sel.unparsed())
    );

    sel = try$(Selector::parse(".a>.b"));
    assertEq$(
        ".a>.b"s,
        Io::toStr(sel.unparsed())
    );

    sel = try$(Selector::parse(".a~.b"));
    assertEq$(
        ".a~.b"s,
        Io::toStr(sel.unparsed())
    );

    sel = try$(Selector::parse(".a+.b"));
    assertEq$(
        ".a+.b"s,
        Io::toStr(sel.unparsed())
    );

    return Ok();
}

} // namespace Vaev::Style::Tests
