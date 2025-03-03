#include <karm-cli/args.h>
#include <karm-http/client.h>
#include <karm-sys/entry.h>

Async::Task<> entryPointAsync(Sys::Context& ctx) {
    auto urlArg = Cli::operand<Str>("url"s, "URL to fetch"s, "localhost"s);

    Cli::Command cmd{
        "http-get"s,
        NONE,
        "Send a GET request to a URL and print the response body"s,
        {
            urlArg,
        }
    };

    co_trya$(cmd.execAsync(ctx));

    co_return Ok();
}
