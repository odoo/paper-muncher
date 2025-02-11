#pragma once

#include <karm-scene/base.h>
#include <vaev-base/length.h>
#include <vaev-dom/document.h>
#include <vaev-driver/fetcher.h>
#include <vaev-layout/base.h>
#include <vaev-style/media.h>

namespace Vaev::Driver {

struct RenderResult {
    Style::StyleBook style;
    Rc<Layout::Box> layout;
    Rc<Scene::Node> scenes;
    Rc<Layout::Frag> frag;
};

RenderResult render(Fetcher& fetcher, Gc::Ref<Dom::Document> dom, Style::Media const& media, Layout::Viewport viewport);

} // namespace Vaev::Driver
