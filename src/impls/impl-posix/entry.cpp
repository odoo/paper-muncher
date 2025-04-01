#include <karm-base/backtrace.h>
#include <karm-base/panic.h>
#include <karm-sys/chan.h>
#include <stdio.h>
#include <stdlib.h>

void __panicHandler(marK::PanicKind kind, char const* msg) {
    fprintf(stderr, "%s: %s\n", kind == marK::PanicKind::PANIC ? "panic" : "debug", msg);

    // NOTE: We hare calling backinto the framework here, it might cause another
    //       panic, this is why we are keeping track of nested panics
    static isize _panicDepth = 1;
    _panicDepth++;
    if (_panicDepth == 1) {
        auto bt = marK::Backtrace::capture();
        if (bt.status() == marK::Backtrace::Status::CAPTURED)
            Sys::println("backtrace:\n{}", bt);
    }

    if (kind == marK::PanicKind::PANIC) {
        abort();
        __builtin_unreachable();
    }
    _panicDepth--;
}
