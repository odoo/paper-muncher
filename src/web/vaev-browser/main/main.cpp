#include <karm-sys/entry.h>
#include <karm-ui/app.h>
#include <vaev-driver/fetcher.h>

import Vaev.Browser;

Async::Task<> entryPointAsync(Sys::Context& ctx) {
    auto args = Sys::useArgs(ctx);
    auto url = args.len()
                   ? Mime::parseUrlOrPath(args[0])
                   : "about:start"_url;
    Gc::Heap heap;
    Vaev::Driver::FileFetcher fetcher;

    auto dom = Vaev::Driver::fetchDocument(fetcher, heap, url);

    co_return Ui::runApp(
        ctx,
        Vaev::Browser::app(fetcher, heap, url, dom)
    );
}
