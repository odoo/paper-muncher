#include <karm/entry>

import Karm.Core;
import Karm.Cli;

import Html5LibTest;

using namespace Karm;

enum struct Suite {
    TOKENIZER,
    TREE_CONSTRUCTION,

    _LEN,
};

Async::Task<> entryPointAsync(Sys::Env& env, [[maybe_unused]] Async::CancellationToken ct) {
    auto suiteArg = Cli::option<Suite>('s', "suite"s, "The type of test to run"s);
    auto inputArg = Cli::operand<Str>("input"s, "Input file (default: stdin)"s, {"-"s});

    auto mainSection = Cli::Section{
        "Runner Options"s,
        {suiteArg, inputArg},
    };

    Cli::Command cmd{
        "html5lib-tests-runner"s,
        "Runner for the html5lib test suite format."s,
        {
            mainSection,
        }
    };

    co_trya$(cmd.execAsync(env));
    if (not cmd)
        co_return Ok();

    if (not suiteArg.has())
        co_return Error::invalidInput("test suite required");

    auto input = inputArg.value() == "-" ? "fd:stdin"_url : Ref::parseUrlOrPath(inputArg.value(), env.cwd());
    auto inputString = co_try$(Sys::readAllUtf8(input));

    Html5LibTest::Result result;
    if (suiteArg.value() == Suite::TOKENIZER) {
        result = co_try$(Html5LibTest::Tokenizer::run(inputString));
    } else if (suiteArg.value() == Suite::TREE_CONSTRUCTION) {
        result = co_try$(Html5LibTest::TreeConstruction::run(inputString));
    } else {
        unreachable();
    }

    Serde::Object json = {
        {"reference"s, std::move(result.reference)},
        {"actual"s, std::move(result.actual)},
    };
    co_return Json::unparse(Sys::out(), json);
}
