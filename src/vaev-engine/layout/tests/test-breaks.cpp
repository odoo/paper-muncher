#include <karm/test>

import Vaev.Engine;
import Karm.Core;

using namespace Karm;

namespace Vaev::Layout::Tests {

test$("breakpoint-equal") {
    assertEq$(Breakpoint::classB(2, false), Breakpoint::classB(2, false));
    assertEq$(
        Breakpoint::fromChild(Breakpoint::overflow(), 1, false),
        Breakpoint::fromChild(Breakpoint::overflow(), 1, false)
    );
    return Ok();
}

test$("breakpoint-not-equal-end") {
    assertNe$(Breakpoint::classB(1, false), Breakpoint::classB(2, false));
    return Ok();
}

test$("breakpoint-not-equal-appeal") {
    assertNe$(Breakpoint::classB(1, false), Breakpoint::classB(1, true));
    return Ok();
}

test$("breakpoint-not-equal-advance") {
    auto withChildren = Breakpoint::fromChildren({Some(Breakpoint::classB(1, false))}, 1, false, true).expect();
    auto keepingChildren = Breakpoint::fromChildren({Some(Breakpoint::classB(1, false))}, 1, false, false).expect();
    assertNe$(withChildren, keepingChildren);
    return Ok();
}

test$("breakpoint-not-equal-nested-child") {
    assertNe$(
        Breakpoint::fromChild(Breakpoint::fromChild(Breakpoint::classB(1, false), 1, false), 1, false),
        Breakpoint::fromChild(Breakpoint::fromChild(Breakpoint::classB(2, false), 1, false), 1, false)
    );
    return Ok();
}

test$("breakpoint-not-equal-children-count") {
    auto one = Breakpoint::fromChildren({Some(Breakpoint::classB(1, false))}, 1, false, false).expect();
    auto two = Breakpoint::fromChildren({Some(Breakpoint::classB(1, false)), Some(Breakpoint::classB(1, false))}, 1, false, false).expect();
    assertNe$(one, two);
    return Ok();
}

test$("breakpoint-not-equal-missing-child") {
    auto both = Breakpoint::fromChildren({Some(Breakpoint::classB(1, false)), Some(Breakpoint::classB(1, false))}, 1, false, false).expect();
    auto first = Breakpoint::fromChildren({Some(Breakpoint::classB(1, false)), NONE}, 1, false, false).expect();
    assertNe$(both, first);
    return Ok();
}

} // namespace Vaev::Layout::Tests
