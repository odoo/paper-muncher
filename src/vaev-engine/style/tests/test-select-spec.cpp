#include <karm-test/macros.h>

import Karm.Gc;
import Vaev.Engine;

using namespace Karm;

namespace Vaev::Style::Tests {

test$("select-class-spec") {
    Gc::Heap gc;
    Selector sel = ClassSelector{"foo"s};
    auto el = gc.alloc<Dom::Element>(Html::DIV_TAG);
    el->classList.add("foo");
    expectNe$(matchSelector(sel, el), NONE);
    return Ok();
}

test$("select-attr-spec-exact") {
    Gc::Heap gc;
    auto el = gc.alloc<Dom::Element>(Html::DIV_TAG);

    {
        el->setAttribute(Html::ID_ATTR, "test"s);
        Selector sel = AttributeSelector{
            .qualifiedName = Html::ID_ATTR,
            .case_ = AttributeSelector::SENSITIVE,
            .match = AttributeSelector::EXACT,
            .value = "test"s,
        };
        expectNe$(matchSelector(sel, el), NONE);
    }

    {
        el->setAttribute(Html::ID_ATTR, "tesi"s);
        Selector sel = AttributeSelector{
            .qualifiedName = Html::ID_ATTR,
            .case_ = AttributeSelector::SENSITIVE,
            .match = AttributeSelector::EXACT,
            .value = "test"s,
        };
        expectEq$(matchSelector(sel, el), NONE);
    }

    return Ok();
}

test$("select-attr-spec-contains") {
    Gc::Heap gc;
    auto el = gc.alloc<Dom::Element>(Html::DIV_TAG);

    {
        el->setAttribute(Html::ID_ATTR, "some test value"s);
        Selector sel = AttributeSelector{
            .qualifiedName = Html::ID_ATTR,
            .case_ = AttributeSelector::SENSITIVE,
            .match = AttributeSelector::CONTAINS,
            .value = "test"s,
        };
        expectNe$(matchSelector(sel, el), NONE);
    }

    {
        el->setAttribute(Html::ID_ATTR, "some testi value"s);
        Selector sel = AttributeSelector{
            .qualifiedName = Html::ID_ATTR,
            .case_ = AttributeSelector::SENSITIVE,
            .match = AttributeSelector::CONTAINS,
            .value = "test"s,
        };
        expectEq$(matchSelector(sel, el), NONE);
    }

    return Ok();
}

test$("select-attr-spec-hyphenated") {
    Gc::Heap gc;
    auto el = gc.alloc<Dom::Element>(Html::DIV_TAG);

    {
        el->setAttribute(Html::ID_ATTR, "test-value"s);
        Selector sel = AttributeSelector{
            .qualifiedName = Html::ID_ATTR,
            .case_ = AttributeSelector::SENSITIVE,
            .match = AttributeSelector::HYPHENATED,
            .value = "test"s,
        };
        expectNe$(matchSelector(sel, el), NONE);
    }

    {
        el->setAttribute(Html::ID_ATTR, "test"s);
        Selector sel = AttributeSelector{
            .qualifiedName = Html::ID_ATTR,
            .case_ = AttributeSelector::SENSITIVE,
            .match = AttributeSelector::HYPHENATED,
            .value = "test"s,
        };
        expectNe$(matchSelector(sel, el), NONE);
    }

    {
        el->setAttribute(Html::ID_ATTR, "tesi-value"s);
        Selector sel = AttributeSelector{
            .qualifiedName = Html::ID_ATTR,
            .case_ = AttributeSelector::SENSITIVE,
            .match = AttributeSelector::HYPHENATED,
            .value = "test"s,
        };
        expectEq$(matchSelector(sel, el), NONE);
    }

    {
        el->setAttribute(Html::ID_ATTR, "value-test"s);
        Selector sel = AttributeSelector{
            .qualifiedName = Html::ID_ATTR,
            .case_ = AttributeSelector::SENSITIVE,
            .match = AttributeSelector::HYPHENATED,
            .value = "test"s,
        };
        expectEq$(matchSelector(sel, el), NONE);
    }

    return Ok();
}

test$("select-attr-spec-str-start-with") {
    Gc::Heap gc;
    auto el = gc.alloc<Dom::Element>(Html::DIV_TAG);

    {
        el->setAttribute(Html::ID_ATTR, "teste"s);
        Selector sel = AttributeSelector{
            .qualifiedName = Html::ID_ATTR,
            .case_ = AttributeSelector::SENSITIVE,
            .match = AttributeSelector::STR_START_WITH,
            .value = "test"s,
        };
        expectNe$(matchSelector(sel, el), NONE);
    }

    {
        el->setAttribute(Html::ID_ATTR, "tesitest"s);
        Selector sel = AttributeSelector{
            .qualifiedName = Html::ID_ATTR,
            .case_ = AttributeSelector::SENSITIVE,
            .match = AttributeSelector::STR_START_WITH,
            .value = "test"s,
        };
        expectEq$(matchSelector(sel, el), NONE);
    }

    {
        el->setAttribute(Html::ID_ATTR, ""s);
        Selector sel = AttributeSelector{
            .qualifiedName = Html::ID_ATTR,
            .case_ = AttributeSelector::SENSITIVE,
            .match = AttributeSelector::STR_START_WITH,
            .value = "test"s,
        };
        expectEq$(matchSelector(sel, el), NONE);
    }

    return Ok();
}

test$("select-attr-spec-str-end-with") {
    Gc::Heap gc;
    auto el = gc.alloc<Dom::Element>(Html::DIV_TAG);

    {
        el->setAttribute(Html::ID_ATTR, "itest"s);
        Selector sel = AttributeSelector{
            .qualifiedName = Html::ID_ATTR,
            .case_ = AttributeSelector::SENSITIVE,
            .match = AttributeSelector::STR_END_WITH,
            .value = "test"s,
        };
        expectNe$(matchSelector(sel, el), NONE);
    }

    {
        el->setAttribute(Html::ID_ATTR, "testtesi"s);
        Selector sel = AttributeSelector{
            .qualifiedName = Html::ID_ATTR,
            .case_ = AttributeSelector::SENSITIVE,
            .match = AttributeSelector::STR_END_WITH,
            .value = "test"s,
        };
        expectEq$(matchSelector(sel, el), NONE);
    }

    {
        el->setAttribute(Html::ID_ATTR, ""s);
        Selector sel = AttributeSelector{
            .qualifiedName = Html::ID_ATTR,
            .case_ = AttributeSelector::SENSITIVE,
            .match = AttributeSelector::STR_END_WITH,
            .value = "test"s,
        };
        expectEq$(matchSelector(sel, el), NONE);
    }

    return Ok();
}

test$("select-attr-spec-str-contain") {
    Gc::Heap gc;
    auto el = gc.alloc<Dom::Element>(Html::DIV_TAG);

    {
        el->setAttribute(Html::ID_ATTR, "value-test-value"s);
        Selector sel = AttributeSelector{
            .qualifiedName = Html::ID_ATTR,
            .case_ = AttributeSelector::SENSITIVE,
            .match = AttributeSelector::STR_CONTAIN,
            .value = "test"s,
        };
        expectNe$(matchSelector(sel, el), NONE);
    }

    {
        el->setAttribute(Html::ID_ATTR, "est-tesi-tes"s);
        Selector sel = AttributeSelector{
            .qualifiedName = Html::ID_ATTR,
            .case_ = AttributeSelector::SENSITIVE,
            .match = AttributeSelector::STR_CONTAIN,
            .value = "test"s,
        };
        expectEq$(matchSelector(sel, el), NONE);
    }

    {
        el->setAttribute(Html::ID_ATTR, ""s);
        Selector sel = AttributeSelector{
            .qualifiedName = Html::ID_ATTR,
            .case_ = AttributeSelector::SENSITIVE,
            .match = AttributeSelector::STR_CONTAIN,
            .value = "test"s,
        };
        expectEq$(matchSelector(sel, el), NONE);
    }

    return Ok();
}

test$("select-attr-spec-case") {
    Gc::Heap gc;
    auto el = gc.alloc<Dom::Element>(Html::DIV_TAG);

    {
        el->setAttribute(Html::ID_ATTR, "teST"s);
        Selector sel = AttributeSelector{
            .qualifiedName = Html::ID_ATTR,
            .case_ = AttributeSelector::INSENSITIVE,
            .match = AttributeSelector::EXACT,
            .value = "test"s,
        };
        expectNe$(matchSelector(sel, el), NONE);
    }

    {
        el->setAttribute(Html::ID_ATTR, "tesT"s);
        Selector sel = AttributeSelector{
            .qualifiedName = Html::ID_ATTR,
            .case_ = AttributeSelector::SENSITIVE,
            .match = AttributeSelector::EXACT,
            .value = "test"s,
        };
        expectEq$(matchSelector(sel, el), NONE);
    }

    {
        el->setAttribute(Html::ID_ATTR, "teST"s);
        Selector sel = AttributeSelector{
            .qualifiedName = Html::ID_ATTR,
            .case_ = AttributeSelector::INSENSITIVE,
            .match = AttributeSelector::CONTAINS,
            .value = "test"s,
        };
        expectNe$(matchSelector(sel, el), NONE);
    }

    {
        el->setAttribute(Html::ID_ATTR, "tesT"s);
        Selector sel = AttributeSelector{
            .qualifiedName = Html::ID_ATTR,
            .case_ = AttributeSelector::SENSITIVE,
            .match = AttributeSelector::CONTAINS,
            .value = "test"s,
        };
        expectEq$(matchSelector(sel, el), NONE);
    }

    {
        el->setAttribute(Html::ID_ATTR, "teST"s);
        Selector sel = AttributeSelector{
            .qualifiedName = Html::ID_ATTR,
            .case_ = AttributeSelector::INSENSITIVE,
            .match = AttributeSelector::STR_START_WITH,
            .value = "test"s,
        };
        expectNe$(matchSelector(sel, el), NONE);
    }

    {
        el->setAttribute(Html::ID_ATTR, "tesT"s);
        Selector sel = AttributeSelector{
            .qualifiedName = Html::ID_ATTR,
            .case_ = AttributeSelector::SENSITIVE,
            .match = AttributeSelector::STR_START_WITH,
            .value = "test"s,
        };
        expectEq$(matchSelector(sel, el), NONE);
    }

    return Ok();
}

} // namespace Vaev::Style::Tests
