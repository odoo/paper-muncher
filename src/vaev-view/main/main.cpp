#include <karm/entry>

import Karm.Core;
import Karm.Sys;
import Karm.Ui;
import Karm.Gfx;
import Karm.Debug;
import Vaev.View;
import Vaev.Engine;

using namespace Karm;
using namespace Vaev;

Async::Task<> entryPointAsync(Sys::Env& env, Async::CancellationToken ct) {
    Debug::toggleFlag(Debug::FEATURE, "*", true).unwrap();

    auto& args = env.args();
    if (args.len() < 1)
        co_return Error::invalidInput("Usage: vaev-view <url>");

    auto url = Ref::parseUrlOrPath(args[0], env.cwd());
    auto window = Dom::Window::create();
    co_trya$(window->loadLocationAsync(url, Ref::Uti::PUBLIC_OPEN, ct));
    co_return co_await Ui::runAsync(
        env,
        Ui::vflow(
            View::viewport(window, {}) | Ui::grow(),
            Ui::text("{}", url) |
                Ui::box({
                    .backgroundFill = Ui::GRAY300,
                    .foregroundFill = Ui::GRAY800,
                })
        ) |
            Ui::box({.margin = 1, .backgroundFill = Gfx::WHITE}) |
            Ui::pinSize({1024, 720}),
        ct
    );
}
