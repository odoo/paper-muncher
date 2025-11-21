#include <karm-core/macros.h>
#include <karm-sys/entry.h>

import Karm.Core;
import Karm.Http;
import Karm.Cli;
import Vaev.Webdriver;

using namespace Karm;

Async::Task<> entryPointAsync(Sys::Context& ctx) {
    auto portOption = Cli::option<isize>('p', "port"s, "TCP port to listen to (default: 4444)."s, 4444);
    Cli::Section serverSection = {"Server Options"s, {portOption}};

    Cli::Command cmd{
        "vaev-webdriver"s,
        "Webdriver protocol implementation for vaev."s,
        {serverSection}
    };

    co_trya$(cmd.execAsync(ctx));
    if (not cmd)
        co_return Ok();

    auto webdriver = Vaev::WebDriver::createWebDriver();
    auto service = Vaev::WebDriver::createService(webdriver);
    co_return co_await Http::serveAsync(
        service,
        {
            .name = "Vaev WebDriver"s,
            .addr = Sys::Ip4::localhost(portOption.value()),
        }
    );
}