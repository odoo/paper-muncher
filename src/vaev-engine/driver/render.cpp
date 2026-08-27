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
static auto dumpScene = Debug::Flag::debug("web-scene"s, "Dump the constructed scene"s);

export struct RenderResult {
    Rc<Layout::Box> layout;
    Gfx::Snapshot scene;
    Rc<Layout::Fragment> frag;
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

    Layout::Tree tree = {
        Layout::buildDocument(dom),
        viewport
    };

    auto layout = Layout::layoutRoot(
        tree,
        {
            .generateFragment = true,
            .knownSize = {Some(viewport.small.width), NONE},
            .availableSpace = {viewport.small.width, 0_au},
            .containingBlock = {viewport.small.width, viewport.small.height},
        }
    );

    auto sceneRoot = makeRc<Scene::Stack>();
    Layout::paint(*layout.fragment, *sceneRoot);
    sceneRoot->prepare();

    if (dumpFragments)
        logDebugIf(dumpFragments, "fragments: {}", *layout.fragment);

    if (dumpScene)
        logDebugIf(dumpScene, "scene: {}", sceneRoot);

    auto bound = sceneRoot->bound();
    Gfx::Snapshot::Recorder recorder{bound.bottomEnd().ceil().cast<isize>()};
    sceneRoot->paint(recorder, bound, {});
    return {
        makeRc<Layout::Box>(std::move(tree.root)),
        recorder.finalize(),
        *layout.fragment,
    };
}

} // namespace Vaev::Driver
