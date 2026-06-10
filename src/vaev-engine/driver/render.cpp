export module Vaev.Engine:driver.render;

import Karm.Gc;
import Karm.Scene;
import Karm.Font;
import Karm.Gfx;
import Karm.Math;
import Karm.Logger;

import :layout;
import :style;
import :dom.document;
import :values;

namespace Vaev::Driver {

static auto dumpFragments = Debug::Flag::debug("web-fragments"s, "Dump the constructed fragments"s);

export struct RenderResult {
    Rc<Layout::Box> layout;
    Rc<Scene::Node> scenes;
    Rc<Layout::Fragment> frag;
};

export RenderResult render(Gc::Heap& heap, Gc::Ref<Dom::Document> dom, Style::Media const& media, Layout::Viewport viewport) {
    Style::Computer computer{
        heap,
        media,
        dom->registeredPropertySet,
        *dom->styleSheets,
        *dom->fontDatabase,
    };

    computer.build();
    computer.styleDocument(*dom);

    Layout::Tree tree = {
        Layout::buildDocument(dom),
        viewport
    };

    auto [outDiscovery, root] = Layout::layoutAndCommitRoot(
        tree,
        {
            .knownSize = {viewport.small.width, NONE},
            .availableSpace = {viewport.small.width, 0_au},
            .containingBlock = {viewport.small.width, viewport.small.height},
        }
    );

    auto sceneRoot = makeRc<Scene::Stack>();
    Layout::paint(root, *sceneRoot);
    sceneRoot->prepare();

    if (dumpFragments)
        logDebugIf(dumpFragments, "fragments: {}", root);

    return {
        makeRc<Layout::Box>(std::move(tree.root)),
        sceneRoot,
        root,
    };
}

} // namespace Vaev::Driver
