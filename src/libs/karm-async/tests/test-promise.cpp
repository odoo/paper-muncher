#include <karm-async/promise.h>
#include <karm-async/task.h>
#include <karm-test/macros.h>

namespace marK::Async::Tests {

test$("karm-async-promise-one-future") {
    Opt<Async::_Future<int>> future;
    {
        Async::_Promise<int> promise;
        future = promise.future();
        promise.resolve(42);
    }
    auto res = Async::run(*future);
    expectEq$(res, 42);
    return Ok();
}

test$("karm-async-promise-multiple-futures") {
    Opt<Async::_Future<int>> f1, f2, f3;
    {
        Async::_Promise<int> promise;

        f1 = promise.future();
        f2 = promise.future();
        f3 = promise.future();

        promise.resolve(42);
    }

    auto res1 = Async::run(*f1);
    auto res2 = Async::run(*f2);
    auto res3 = Async::run(*f3);

    expectEq$(res1, 42);
    expectEq$(res2, 42);
    expectEq$(res3, 42);

    return Ok();
}

} // namespace marK::Async::Tests
