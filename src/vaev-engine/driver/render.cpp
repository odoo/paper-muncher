module;

#include <karm-logger/logger.h>
#include <karm-math/au.h>
#include <karm-text/book.h>

export module Vaev.Engine:driver.render;

import Karm.Gc;
import Karm.Scene;
import :layout;
import :style;
import :dom;

namespace Vaev::Driver {

export struct RenderResult {
    Rc<Layout::Box> layout;
    Rc<Scene::Node> scenes;
    Rc<Layout::Frag> frag;
};

export RenderResult render(Gc::Ref<Dom::Document> dom, Style::Media const& media, Layout::Viewport viewport) {
    Text::FontBook fontBook;
    if (not fontBook.loadAll())
        logWarn("not all fonts were properly loaded into fontbook");

    Style::Computer computer{media, *dom->styleSheets, fontBook};
    computer.build();
    computer.styleDocument(*dom);

    Layout::Tree tree = {
        Layout::build(dom),
        viewport
    };

    auto canvasColor = fixupBackgrounds(computer, dom, tree);

    auto [outDiscovery, root] = Layout::layoutCreateFragment(
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

    return {
        makeRc<Layout::Box>(std::move(tree.root)),
        makeRc<Scene::Clear>(sceneRoot, canvasColor),
        makeRc<Layout::Frag>(std::move(root)),
    };
}

} // namespace Vaev::Driver
