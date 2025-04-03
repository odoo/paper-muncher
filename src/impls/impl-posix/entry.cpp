#include <stdio.h>
#include <stdlib.h>

//
#include <karm-base/backtrace.h>
#include <karm-base/panic.h>
#include <karm-sys/chan.h>
#include <karm-sys/context.h>

void __panicHandler(Karm::PanicKind kind, char const* msg) {
    fprintf(stderr, "%s: %s\n", kind == Karm::PanicKind::PANIC ? "panic" : "debug", msg);

    // NOTE: We hare calling backinto the framework here, it might cause another
    //       panic, this is why we are keeping track of nested panics
    static isize _panicDepth = 1;
    _panicDepth++;
    if (_panicDepth == 1) {
        auto bt = Karm::Backtrace::capture();
        if (bt.status() == Karm::Backtrace::Status::CAPTURED)
            Sys::println("backtrace:\n{}", bt);
    }

    if (kind == Karm::PanicKind::PANIC) {
        abort();
        __builtin_unreachable();
    }
    _panicDepth--;
}

int __entryPointWraper(int argc, char const** argv, Sys::EntryPointAsync* entry) {
    Karm::registerPanicHandler(__panicHandler);

    auto& ctx = Sys::globalContext();
    ctx.add<Sys::ArgsHook>(argc, argv);
    Res<> code = Sys::run(entry(ctx));
    if (not code) {
        Karm::Sys::errln("{}: {}", argv[0], code);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
