#include <karm-sys/addr.h>
#include <karm-test/macros.h>

namespace marK::Sys::Tests {

test$("ip4-eq") {
    expectEq$(Ip4::localhost(), Ip4::localhost());
    expectEq$(Ip4::localhost(80), Ip4::localhost(80));
    return Ok();
}

} // namespace marK::Sys::Tests
