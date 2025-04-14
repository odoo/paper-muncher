module;

#include <karm-ui/anim.h>
#include <karm-ui/input.h>
#include <karm-ui/layout.h>
#include <karm-ui/popover.h>
#include <karm-ui/scroll.h>

export module Karm.Kira:select;

import Mdi;

namespace Karm::Kira {

export Ui::Child select(Ui::Child value, Ui::Slots slots) {
    return Ui::button(
        [slots = std::move(slots)](auto& n) {
            Ui::showPopover(
                n,
                n.bound().bottomStart(),
                Ui::vflow(
                    slots()
                ) |
                    Ui::vscroll() |
                    Ui::sizing({n.bound().width, Ui::UNCONSTRAINED}, {Ui::UNCONSTRAINED, 160}) |
                    Ui::box({
                        .borderRadii = 6,
                        .borderWidth = 1,
                        .borderFill = Ui::GRAY800,
                        .backgroundFill = Ui::GRAY900,
                        .shadowStyle = Gfx::BoxShadow::elevated(4),
                    }) |
                    Ui::scaleIn()
            );
        },
        Ui::ButtonStyle::outline(),
        Ui::hflow(
            8,
            Math::Align::CENTER,
            value | Ui::grow(),
            Ui::icon(Mdi::CHEVRON_DOWN)
        ) | Ui::insets({6, 12, 6, 16}) |
            Ui::minSize({Ui::UNCONSTRAINED, 36})
    );
}

export Ui::Child selectValue(String text) {
    return Ui::labelMedium(text);
}

export Ui::Child selectLabel(String text) {
    return Ui::labelMedium(Ui::GRAY400, text) |
           Ui::insets({12, 6, 3, 14});
}

export Ui::Child selectItem(Ui::OnPress onPress, String t) {
    return Ui::hflow(
               12,
               Math::Align::CENTER,
               Ui::text(t)
           ) |
           Ui::insets({6, 6, 6, 10}) |
           Ui::minSize({Ui::UNCONSTRAINED, 28}) |
           Ui::button(
               [onPress = std::move(onPress)](auto& n) {
                   onPress(n);
                   Ui::closePopover(n);
               },
               Ui::ButtonStyle::subtle()
           ) |
           Ui::insets(4);
}

export Ui::Child selectGroup(Ui::Children children) {
    return Ui::vflow(children);
}

} // namespace Karm::Kira
