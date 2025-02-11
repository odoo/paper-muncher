#pragma once

#include <karm-ui/node.h>
#include <vaev-dom/document.h>
#include <vaev-driver/fetcher.h>

namespace Vaev::View {

Ui::Child printDialog(Driver::Fetcher& fetcher, Gc::Ref<Dom::Document> dom);

} // namespace Vaev::View
