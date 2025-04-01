#include <karm-gc/heap.h>
#include <karm-test/macros.h>

namespace marK::Gc::Tests {

struct Foo {};

test$("gc-simple-lifetime") {
    Heap heap;

    [[maybe_unused]] auto obj1 = heap.alloc<Foo>();
    [[maybe_unused]] auto obj2 = heap.alloc<Foo>();
    [[maybe_unused]] auto obj3 = heap.alloc<Foo>();

    return Ok();
}

} // namespace marK::Gc::Tests
