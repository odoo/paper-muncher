#include <karm/entry>

import Karm.Ui;
import Hideo.Browser;
import Vaev.Engine;
import Karm.Http;
import Karm.Gc;
import Karm.Sys;
import Karm.Debug;
import Karm.Logger;

using namespace Karm;
using namespace Karm::Literals;
using namespace Karm::Ref::Literals;

Async::Task<> entryPointAsync(Sys::Env& env, Async::CancellationToken ct) {
    co_try$(Debug::toggleFlag(Debug::FEATURE, "*", true));

    auto& args = env.args();
    auto url = args.len()
                   ? Ref::parseUrlOrPath(args[0], env.cwd())
                   : "about:blank"_url;

    auto client = Http::defaultClient();
    client->userAgent = "Hideo.Browser/" stringify$(__ck_version_value) ""s;
    auto window = Vaev::Dom::Window::create(client);
    window->changeMedia(
        Vaev::Style::Media::forView(
            {},
            Ui::darkMode ? Vaev::ColorScheme::DARK : Vaev::ColorScheme::LIGHT
        )
    );
    co_trya$(window->loadLocationAsync(
        url,
        Ref::Uti::PUBLIC_OPEN,
        Async::CancellationToken::uninterruptible()
    ));

    co_return co_await Ui::runAsync(
        env,
        Vaev::Browser::app(window),
        ct
    );
}
