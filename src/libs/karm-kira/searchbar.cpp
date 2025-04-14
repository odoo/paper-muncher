module;

#include <karm-ui/focus.h>
#include <karm-ui/layout.h>

export module Karm.Kira:searchbar;

import Mdi;

namespace Karm::Kira {

export Ui::Child searchbar(String text) {
    return Ui::hflow(
               8,
               Math::Align::VCENTER | Math::Align::START,
               Ui::stack(
                   text ? Ui::empty() : Ui::labelMedium(Gfx::ZINC600, "Search…"),
                   Ui::input(Ui::TextStyles::labelMedium(), text, NONE)
               ) | Ui::grow(),
               Ui::icon(Mdi::MAGNIFY)
           ) |
           Ui::box({
               .padding = {6, 12, 6, 12},
               .borderRadii = 4,
               .borderWidth = 1,
               .borderFill = Ui::GRAY800,
           }) |
           Ui::minSize({Ui::UNCONSTRAINED, 36}) |
           Ui::focusable() |
           Ui::keyboardShortcut(App::Key::F, App::KeyMod::CTRL);
}

} // namespace Karm::Kira
