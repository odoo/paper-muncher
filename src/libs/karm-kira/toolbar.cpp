module;

#include <karm-ui/input.h>
#include <karm-ui/layout.h>

export module Karm.Kira:toolbar;

namespace Karm::Kira {

export Ui::Child toolbar(Ui::Children children) {
    return Ui::vflow(
        Ui::hflow(4, children) |
            Ui::insets(8) |
            Ui::box({.backgroundFill = Ui::GRAY900}),
        Ui::separator()
    );
}

export Ui::Child bottombar(Ui::Children children) {
    return Ui::vflow(
        Ui::separator(),
        Ui::hflow(4, children) |
            Ui::insets(8) |
            Ui::box({.backgroundFill = Ui::GRAY900})
    );
}

} // namespace Karm::Kira
