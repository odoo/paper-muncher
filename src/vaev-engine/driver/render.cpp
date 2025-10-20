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

export struct RenderResult {
    Rc<Layout::Box> layout;
    Rc<Scene::Node> scenes;
    Rc<Layout::Frag> frag;
};

export RenderResult render(Gc::Ref<Dom::Document> dom, Style::Media const& media, Layout::Viewport viewport) {
    Font::Database db;
    if (not db.loadSystemFonts())
        logWarn("not all fonts were properly loaded into database");

    Style::Computer computer{media, *dom->styleSheets, db};
    computer.build();
    computer.styleDocument(*dom);

    Layout::Tree tree = {
        Layout::build(dom),
        viewport
    };

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
        sceneRoot,
        makeRc<Layout::Frag>(std::move(root)),
    };
}

} // namespace Vaev::Driver
