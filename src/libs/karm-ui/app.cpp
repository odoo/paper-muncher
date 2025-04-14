#include "app.h"

#include "_embed.h"
#include "host.h"

namespace Karm::Ui {

Async::Task<> runAsync(Sys::Context&, Child root) {
    auto host = co_try$(_Embed::makeHost(root));
    co_return co_await host->runAsync();
}

void mountApp(Cli::Command& cmd, Slot rootSlot) {
    Cli::Flag mobileArg = Cli::flag(NONE, "mobile"s, "Show mobile layout."s);

    cmd.option(mobileArg);
    cmd.callbackAsync = [rootSlot = std::move(rootSlot)](Sys::Context& ctx) -> Async::Task<> {
        auto root = rootSlot();
        co_return co_await runAsync(ctx, root);
    };
}

} // namespace Karm::Ui
