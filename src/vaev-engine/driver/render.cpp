export module Vaev.Engine:driver.render;

import Karm.Gc;
import Karm.Font;
import Karm.Gfx;
import Karm.Math;
import Karm.Logger;

import :layout;
import :layout2;
import :style;
import :dom.document;
import :paint;
import :values;

namespace Vaev::Driver {

static auto dumpFragments = Debug::Flag::debug("web-fragments"s, "Dump the constructed fragments"s);
static auto dumpStacking = Debug::Flag::debug("web-stacking"s, "Dump the stacking context tree"s);

static auto layout2 = Debug::Flag::feature("layout2-render"s, "Enable the work in progress layout engine rewrite in render mode"s);

export struct RenderResult {
    Rc<Layout::Tree> tree;
    Rc<Layout::Fragment> fragments;
    Rc<Paint::StackingContext> stacking;
};

export RenderResult render(Gc::Heap& heap, Gc::Ref<Dom::Document> dom, Style::Media const& media, Style::Viewport viewport, Style::CounterSet const& initialCounterSet = {}) {
    Style::Computer computer{
        heap,
        media,
        dom->registeredPropertySet,
        *dom->styleSheets,
        dom->fontDatabase,
    };

    computer.build();
    computer.styleDocument(*dom, initialCounterSet);

    auto tree = makeRc<Layout::Tree>(
        Layout::buildDocument(dom),
        viewport
    );

    Opt<Rc<Layout::Fragment>> rootFrag = NONE;
    if (layout2) {
        // FIXME: Extract into a layoutRoot, and investigate the right init params.
        auto output = Layout2::layout(
            *tree, tree->root,
            Layout2::Constraints{
                .knownSize = {Some(viewport.small.width), NONE},
                .containingBlock = {
                    Some(viewport.small.width),
                    Some(viewport.small.height),
                },
                .availableSpace = {
                    viewport.small.width,
                    Layout2::MAX_CONTENT,
                },

                // https://www.w3.org/TR/CSS22/box.html#collapsing-margins
                // - Margins of the root element's box do not collapse.
                .collapseMargins = false,

                // NoSpec AF >.<
                .margins = Layout2::LogicalInsets<Au>::all(0_au),
            }
        );
        rootFrag = Some(output.is<Layout2::Placed>()->fragment);
        Layout2::absolutize(*rootFrag);
    } else {
        rootFrag = Layout::layoutRoot(
            *tree,
            {
                .generateFragment = true,
                .knownSize = {Some(viewport.small.width), NONE},
                .availableSpace = {viewport.small.width, 0_au},
                .containingBlock = {viewport.small.width, viewport.small.height},
            }
        ).fragment;
    }

    auto stacking = Paint::StackingContext::establishStackingContext(rootFrag.expect());

    if (dumpFragments)
        logDebugIf(dumpFragments, "fragments: {}", rootFrag.expect());

    if (dumpStacking)
        logDebugIf(dumpStacking, "stacking: {}", stacking);

    return {
        tree,
        *rootFrag,
        stacking
    };
}

} // namespace Vaev::Driver
