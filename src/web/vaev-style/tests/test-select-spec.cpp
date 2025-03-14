#include <karm-gc/heap.h>
#include <karm-test/macros.h>
#include <vaev-dom/element.h>

import Vaev.Style;

namespace Vaev::Style::Tests {

test$("select-class-spec") {
    Gc::Heap gc;
    Selector sel = ClassSelector{"foo"s};
    auto el = gc.alloc<Dom::Element>(Html::DIV);
    el->classList.add("foo");
    expectNe$(matchSelector(sel, el), NONE);
    return Ok();
}

} // namespace Vaev::Style::Tests
