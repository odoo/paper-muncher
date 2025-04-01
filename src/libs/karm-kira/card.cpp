#include <karm-ui/box.h>
#include <karm-ui/layout.h>

#include "card.h"

namespace marK::Kira {

Ui::Child card(Ui::Child child) {
    return Ui::box(
        {
            .borderRadii = 4,
            .backgroundFill = Ui::GRAY900,
        },
        child
    );
}

Ui::Child card(Ui::Children children) {
    return card(Ui::vflow(children));
}

} // namespace marK::Kira
