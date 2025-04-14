module;

#include <karm-ui/input.h>
#include <karm-ui/layout.h>

export module Karm.Kira:sidePanel;

import Mdi;

namespace Karm::Kira {

export Ui::Child sidePanelContent(Ui::Children children) {
    return Ui::vflow(children) |
           Ui::pinSize({128, Ui::UNCONSTRAINED});
}

export Ui::Child sidePanelTitle(Str title) {
    return Ui::hflow(
               Ui::labelLarge(title),
               Ui::grow(NONE)
           ) |
           Ui::insets(6);
}

export Ui::Child sidePanelTitle(Ui::OnPress onClose, Str title) {
    return Ui::hflow(
               Ui::labelLarge(title),
               Ui::grow(NONE),
               Ui::button(
                   std::move(onClose),
                   Ui::ButtonStyle::subtle(),
                   Ui::icon(Mdi::CLOSE) | Ui::center()
               )
           ) |
           Ui::insets(6);
}

} // namespace Karm::Kira
