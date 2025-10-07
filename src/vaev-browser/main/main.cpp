#include <karm-sys/entry.h>

import Karm.Ui;
import Vaev.Browser;
import Vaev.Engine;
import Karm.Http;
import Karm.Gc;
import Karm.Sys;
import Karm.Debug;

using namespace Karm;

Async::Task<> entryPointAsync(Sys::Context& ctx) {
    co_try$(Debug::toggleFlag(Debug::FEATURE, "*", true));

    auto args = Sys::useArgs(ctx);
    auto url = args.len()
                   ? Ref::parseUrlOrPath(args[0], co_try$(Sys::pwd()))
                   : "about:start"_url;

    auto client = Http::defaultClient();
    client->userAgent = "Vaev-Browser/" stringify$(__ck_version_value) ""s;
    auto window = Vaev::Dom::Window::create(client);
    window->changeMedia(
        Vaev::Style::Media::forView(
            {},
            Ui::darkMode ? Vaev::ColorScheme::DARK : Vaev::ColorScheme::LIGHT
        )
    );
    co_trya$(window->loadLocationAsync(url));

    co_return co_await Ui::runAsync(
        ctx,
        Vaev::Browser::app(window)
    );
}
