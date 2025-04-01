#include <karm-base/rc.h>
#include <karm-test/macros.h>

namespace marK::Base::Tests {

test$("strong-rc") {
    struct S {
        int x = 0;
    };

    auto s = makeRc<S>();

    return Ok();
}

} // namespace marK::Base::Tests
