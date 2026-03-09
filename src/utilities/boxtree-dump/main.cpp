#include <karm/entry>

import Karm.Core;
import Karm.Cli;
import Vaev.Engine;

using namespace Karm;
using namespace Vaev;

Async::Task<> entryPointAsync(Sys::Context& ctx, Async::CancellationToken ct) {
    auto inputArg = Cli::operand<Str>("path"s, "Input file (default: stdin)"s, {"-"s});
    Cli::Command cmd{
        "boxtree-dump"s,
        "Dump the CSS box tree of an HTML document."s,
        {
            Cli::Section{
                "Input Options"s,
                {inputArg},
            },
        }
    };

    co_trya$(cmd.execAsync(ctx));
    if (not cmd)
        co_return Ok();

    auto window = Dom::Window::create();
    auto url = Ref::parseUrlOrPath(inputArg.value(), co_try$(Sys::pwd()));
    co_trya$(window->loadLocationAsync(url, Ref::Uti::PUBLIC_OPEN, ct));
    auto& boxTree = window->ensureLayoutTree();
    Sys::println("{}", boxTree.root);
    co_return Ok();
}
