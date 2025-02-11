#pragma once

#include <karm-print/page.h>
#include <karm-print/printer.h>
#include <vaev-dom/document.h>
#include <vaev-driver/fetcher.h>

namespace Vaev::Driver {

Generator<Print::Page> print(Fetcher& fetcher, Gc::Ref<Dom::Document> dom, Print::Settings const& settings);

} // namespace Vaev::Driver
