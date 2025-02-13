#pragma once

#include <karm-gc/heap.h>
#include <karm-ui/node.h>
#include <vaev-dom/document.h>
#include <vaev-driver/fetcher.h>

namespace Vaev::Browser {

Ui::Child app(Driver::Fetcher& fetcher, Gc::Heap& heap, Mime::Url url, Res<Gc::Ref<Vaev::Dom::Document>> dom);

} // namespace Vaev::Browser
