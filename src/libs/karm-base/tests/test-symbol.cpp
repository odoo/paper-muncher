#include <karm-base/symbol.h>
#include <karm-test/macros.h>

namespace Karm::Tests {

test$("Karm.Base/symbol") {
    auto sym0 = Symbol::from("test");
    auto sym1 = Symbol::from("test");
    auto sym2 = Symbol::from("test but different");
    auto sym3 = Symbol::from("test but different");

    expectEq$(sym0, sym1);
    expectEq$(sym2, sym3);

    return Ok();
}

} // namespace Karm::Tests
