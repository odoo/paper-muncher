#include <karm-ui/anim.h>
#include <karm-ui/layout.h>
#include <karm-ui/popover.h>

#include "checkbox.h"
#include "context-menu.h"

namespace marK::Kira {

struct ContextMenu : public Ui::ProxyNode<ContextMenu> {
    Ui::Slot _menu;

    ContextMenu(Ui::Child child, Ui::Slot menu)
        : Ui::ProxyNode<ContextMenu>(child),
          _menu(std::move(menu)) {
    }

    void reconcile(ContextMenu& o) override {
        Ui::ProxyNode<ContextMenu>::reconcile(o);
        _menu = std::move(o._menu);
    }

    void event(App::Event& event) override {
        Ui::ProxyNode<ContextMenu>::event(event);

        if (event.accepted())
            return;

        if (auto e = event.is<App::MouseEvent>()) {
            if (e->type == App::MouseEvent::PRESS and
                e->button == App::MouseButton::RIGHT and
                bound().contains(e->pos)) {
                Ui::showPopover(*this, e->pos, _menu());
                event.accept();
            }
        }
    }
};

Ui::Child contextMenu(Ui::Child child, Ui::Slot menu) {
    return makeRc<ContextMenu>(child, std::move(menu));
}

Ui::Child contextMenuContent(Ui::Children children) {
    return Ui::vflow(
               children
           ) |
           Ui::minSize({200, Ui::UNCONSTRAINED}) |
           Ui::box({
               .margin = 4,
               .borderRadii = 6,
               .borderWidth = 1,
               .borderFill = Ui::GRAY800,
               .backgroundFill = Ui::GRAY900,
               .shadowStyle = Gfx::BoxShadow::elevated(4),
           }) |
           Ui::scaleIn();
}

Ui::Child contextMenuItem(Ui::OnPress onPress, Opt<Mdi::Icon> i, Str t) {
    return Ui::hflow(
               12,
               Math::Align::CENTER,
               i ? Ui::icon(*i) : Ui::empty(18),
               Ui::text(t)
           ) |
           Ui::insets({6, 6, 6, 10}) |
           Ui::minSize({Ui::UNCONSTRAINED, 36}) |
           Ui::button(
               [onPress = std::move(onPress)](auto& n) {
                   onPress(n);
                   Ui::closePopover(n);
               },
               Ui::ButtonStyle::subtle()
           ) |
           Ui::insets(4);
}

Ui::Child contextMenuCheck(Ui::OnPress onPress, bool checked, Str t) {
    return Ui::hflow(
               12,
               Math::Align::CENTER,
               checkbox(checked, NONE),
               Ui::text(t)
           ) |
           Ui::insets({6, 6, 6, 10}) |
           Ui::minSize({Ui::UNCONSTRAINED, 36}) |
           Ui::button(
               [onPress = std::move(onPress)](auto& n) {
                   onPress(n);
                   Ui::closePopover(n);
               },
               Ui::ButtonStyle::subtle()
           ) |
           Ui::insets(4);
}

Ui::Child contextMenuDock(Ui::Children children) {
    return Ui::hflow(
               2,
               Math::Align::CENTER,
               children
           ) |
           Ui::insets(4);
}

Ui::Child contextMenuIcon(Ui::OnPress onPress, Mdi::Icon i) {
    if (onPress) {
        onPress = [onPress = std::move(onPress)](auto& n) {
            onPress(n);
            Ui::closePopover(n);
        };
    }

    return Ui::button(
        std::move(onPress),
        Ui::ButtonStyle::subtle(),
        i
    );
}

} // namespace marK::Kira
