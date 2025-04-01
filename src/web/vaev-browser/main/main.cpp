#include <karm-gc/heap.h>
#include <karm-sys/entry.h>
#include <karm-sys/proc.h>
#include <karm-ui/app.h>

import Vaev.Browser;
import Vaev.Driver;
import Karm.Http;

Async::Task<> entryPointAsync(Sys::Context& ctx) {
    auto args = Sys::useArgs(ctx);
    auto url = args.len()
                   ? Mime::parseUrlOrPath(args[0], co_try$(Sys::pwd()))
                   : "about:start"_url;
    Gc::Heap heap;
    auto client = Http::defaultClient();
    client->userAgent = "Vive-Browser/" stringify$(__ck_version_value) ""s;

    auto dom = co_await Vive::Driver::fetchDocumentAsync(heap, *client, url);

    co_return co_await Ui::runAsync(
        ctx,
        Vive::Browser::app(heap, *client, url, dom)
    );
}
