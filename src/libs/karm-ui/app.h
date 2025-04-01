#pragma once

#include <karm-cli/args.h>
#include <karm-sys/context.h>

#include "node.h"

namespace marK::Ui {

Child inspector(Child child);

Async::Task<> runAsync(Sys::Context& ctx, Child root);

void mountApp(Cli::Command& cmd, Slot rootSlot);

} // namespace Karm::Ui
