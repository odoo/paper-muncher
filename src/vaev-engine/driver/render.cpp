export module Vaev.Engine:driver.render;

import Karm.Gc;
import Karm.Font;
import Karm.Gfx;
import Karm.Math;
import Karm.Logger;

import :layout;
import :style;
import :dom.document;
import :paint;
import :values;

namespace Vaev::Driver {

static auto dumpFragments = Debug::Flag::debug("web-fragments"s, "Dump the constructed fragments"s);
static auto dumpStacking = Debug::Flag::debug("web-stacking"s, "Dump the stacking context tree"s);

export struct RenderResult {
    Rc<Layout::Tree> tree;
    Rc<Layout::Fragment> fragments;
    Rc<Paint::StackingContext> stacking;
};

export RenderResult render(Gc::Heap& heap, Gc::Ref<Dom::Document> dom, Style::Media const& media, Style::Viewport viewport) {
    Style::Computer computer{
        heap,
        media,
        dom->registeredPropertySet,
        *dom->styleSheets,
        dom->fontDatabase,
    };

    computer.build();
    computer.styleDocument(*dom);

    auto tree = makeRc<Layout::Tree>(
        Layout::buildDocument(dom),
        viewport
    );

    auto layout = Layout::layoutRoot(
        *tree,
        {
            .generateFragment = true,
            .knownSize = {Some(viewport.small.width), NONE},
            .availableSpace = {viewport.small.width, 0_au},
            .containingBlock = {viewport.small.width, viewport.small.height},
        }
    );

    auto stacking = Paint::StackingContext::establishStackingContext(layout.fragment.unwrap());

    if (dumpFragments)
        logDebugIf(dumpFragments, "fragments: {}", *layout.fragment);

    if (dumpStacking)
        logDebugIf(dumpStacking, "stacking: {}", stacking);

    return {
        tree,
        *layout.fragment,
        stacking
    };
}

} // namespace Vaev::Driver
