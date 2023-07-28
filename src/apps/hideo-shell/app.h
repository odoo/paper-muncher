#pragma once

#include <karm-ui/box.h>
#include <karm-ui/layout.h>
#include <karm-ui/node.h>

#include "model.h"

namespace Shell {

inline auto panel(Math::Vec2i size = {500, 400}) {
    return Ui::pinSize(size) |
           Ui::box({
               .padding = 8,
               .borderRadius = 8,
               .borderWidth = 1,
               .borderPaint = Gfx::ZINC800,
               .backgroundPaint = Gfx::ZINC950,
           });
}

Ui::Child lock(State const &state);

Ui::Child appsPanel();

Ui::Child appsFlyout();

Ui::Child sysPanel(State const &state);

Ui::Child sysFlyout(State const &state);

Ui::Child keyboardFlyout();

Ui::Child powerDialog();

} // namespace Shell
