#include <karm/entry>

import Karm.Core;
import Karm.Cli;

import Html5LibTest;

using namespace Karm;

Async::Task<> entryPointAsync([[maybe_unused]] Sys::Context& ctx, [[maybe_unused]] Async::CancellationToken ct) {
    auto inputArg = Cli::operand<Str>("input"s, "Input file (default: stdin)"s, {"-"s});

    auto mainSection = Cli::Section{
        "Input"s,
        {inputArg},
    };

    Cli::Command cmd{
        "html5lib-tests-runner"s,
        "Runner for the html5lib test suite format."s,
        {
            mainSection,
        }
    };

    co_trya$(cmd.execAsync(ctx));
    if (not cmd)
        co_return Ok();

    auto input = inputArg.value() == "-" ? "fd:stdin"_url : Ref::parseUrlOrPath(inputArg.value(), co_try$(Sys::pwd()));
    auto inputString = co_try$(Sys::readAllUtf8(input));

    auto testResult = co_try$(Html5LibTest::run(inputString));

    auto output = co_try$(Json::unparse(Serde::Object{
        {"passed"s, testResult.passed},
        {"reference"s, std::move(testResult.reference)},
        {"actual"s, std::move(testResult.actual)},
    }));

    Sys::println("{}", output);

    co_return Ok();
}
