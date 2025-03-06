#include <karm-gc/heap.h>
#include <karm-sys/entry.h>
#include <karm-sys/proc.h>
#include <karm-ui/app.h>

import Vaev.Browser;
import Vaev.Driver;

Async::Task<> entryPointAsync(Sys::Context& ctx) {
    auto args = Sys::useArgs(ctx);
    auto url = args.len()
                   ? Mime::parseUrlOrPath(args[0], co_try$(Sys::pwd()))
                   : "about:start"_url;
    Gc::Heap heap;

    auto dom = Vaev::Driver::fetchDocument(heap, url);

    co_return Ui::runApp(
        ctx,
        Vaev::Browser::app(heap, url, dom)
    );
}
