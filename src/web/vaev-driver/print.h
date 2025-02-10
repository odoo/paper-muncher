#pragma once

#include <karm-print/page.h>
#include <karm-print/printer.h>
#include <vaev-dom/document.h>

namespace Vaev::Driver {

Vec<Print::Page> print(Gc::Ref<Dom::Document> dom, Print::Settings const& settings);

} // namespace Vaev::Driver
