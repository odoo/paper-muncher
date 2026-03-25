#include <karm/entry>

import Karm.Core;
import Karm.Sys;
import Karm.Ui;
import Vaev.View;
import Vaev.Engine;

using namespace Karm;
using namespace Vaev;

Async::Task<> entryPointAsync(Sys::Context& ctx, Async::CancellationToken ct) {
    auto& args = useArgs(ctx);

    if (args.len() < 1)
        co_return Error::invalidInput("Usage: vaev-view <url>");
    auto url = Ref::parseUrlOrPath(args[0], co_try$(Sys::pwd()));
    auto window = Dom::Window::create();
    co_trya$(window->loadLocationAsync(url, Ref::Uti::PUBLIC_OPEN, ct));
    co_return co_await Ui::runAsync(
        ctx,
        Ui::vflow(
            View::viewport(window, {}) | Ui::grow(),
            Ui::text("{}", url) |
                Ui::box({
                    .backgroundFill = Ui::GRAY300,
                    .foregroundFill = Ui::GRAY800,
                })
        ) |
            Ui::box({
                .padding = 1,
                .borderWidth = 1,
                .borderFill = Ui::GRAY300,
            }) |
            Ui::pinSize({1024, 720}),
        ct
    );
}
