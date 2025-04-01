#include <karm-ui/layout.h>
#include <mdi/close.h>

#include "side-panel.h"

namespace marK::Kira {

Ui::Child sidePanelContent(Ui::Children children) {
    return Ui::vflow(children) |
           Ui::pinSize({128, Ui::UNCONSTRAINED});
}

Ui::Child sidePanelTitle(Str title) {
    return Ui::hflow(
               Ui::labelLarge(title),
               Ui::grow(NONE)
           ) |
           Ui::insets(6);
}

Ui::Child sidePanelTitle(Ui::OnPress onClose, Str title) {
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

} // namespace marK::Kira
