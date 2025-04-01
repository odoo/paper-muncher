#pragma once

#include <karm-sys/chan.h>
#include <karm-sys/context.h>

void __panicHandler(marK::PanicKind kind, char const* msg);

int main(int argc, char const** argv) {
    marK::registerPanicHandler(__panicHandler);

    auto& ctx = Sys::globalContext();
    ctx.add<Sys::ArgsHook>(argc, argv);
    Res<> code = Sys::run(entryPointAsync(ctx));
    if (not code) {
        marK::Sys::errln("{}: {}", argv[0], code);
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
