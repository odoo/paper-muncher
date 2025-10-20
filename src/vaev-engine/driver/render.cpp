export module Vaev.Engine:driver.render;

import Karm.Gc;
import Karm.Scene;
import Karm.Font;
import Karm.Gfx;
import Karm.Math;
import Karm.Logger;
import Karm.Debug;
import :layout;
import :style;
import :paint;
import :dom.document;
import :values;

namespace Vaev::Driver {

auto debugPaint = Debug::Flag::debug("web-paint", "Dump the paint tree");

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
    Paint::paintDocument(root, *sceneRoot, RectAu{viewport.small.width, viewport.small.height}.cast<f64>());
    sceneRoot->prepare();
    if (debugPaint)
        logDebug("scene: {}", sceneRoot);

    return {
        makeRc<Layout::Box>(std::move(tree.root)),
        makeRc<Scene::Clear>(sceneRoot, canvasColor),
        makeRc<Layout::Frag>(std::move(root)),
    };
}

} // namespace Vaev::Driver
