#include <karm/entry>

import Karm.Cli;
import Karm.Debug;
import Karm.Gc;
import Karm.Http;
import Karm.Logger;
import Karm.Sys;
import Karm.Ui;

import Vaev.Browser;
import Vaev.Engine;

using namespace Karm;
using namespace Karm::Literals;
using namespace Karm::Ref::Literals;

Async::Task<> entryPointAsync(Sys::Env& env, Async::CancellationToken ct) {
    co_try$(Debug::toggleFlag(Debug::FEATURE, "*", true));

    auto urlArg = Cli::operand<Ref::Url>("url"s, "Url to navigate to (default: about:blank)"s, "about:blank"_url);
    auto devArg = Cli::flag(NONE, "dev"s, "Open the developer tools on startup"s);

    Cli::Command cmd{
        "vaev-browser"s,
        "Browse the web"s,
        {
            {
                .title = "Browser Options"s,
                .options = {urlArg, devArg},
            },
        }
    };

    co_trya$(cmd.execAsync(env));
    if (not cmd)
        co_return Ok();

    auto client = Http::defaultClient();
    client->userAgent = "Mozilla/5.0 Vaev Browser/" stringify$(__ck_version_value) ""s;
    auto window = Vaev::Dom::Window::create(client);
    window->changeMedia(
        Vaev::Style::Media::forView(
            {},
            Ui::darkMode ? Vaev::ColorScheme::DARK : Vaev::ColorScheme::LIGHT
        )
    );
    co_trya$(window->loadLocationAsync(
        urlArg.value(),
        Ref::Uti::PUBLIC_OPEN,
        Async::CancellationToken::uninterruptible()
    ));

    co_return co_await Ui::runAsync(
        env,
        Vaev::Browser::app({window, devArg.value()}),
        ct
    );
}
