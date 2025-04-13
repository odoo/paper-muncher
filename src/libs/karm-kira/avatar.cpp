module;

#include <karm-ui/box.h>
#include <karm-ui/layout.h>
#include <karm-ui/view.h>
#include <mdi/account.h>

export module Karm.Kira:avatar;

namespace Karm::Kira {

export Ui::Child avatar(String t) {
    Ui::BoxStyle boxStyle = {
        .borderRadii = 99,
        .backgroundFill = Ui::GRAY800,
        .foregroundFill = Ui::GRAY500
    };

    return Ui::labelLarge(t) |
           Ui::center() |
           Ui::pinSize(46) |
           Ui::box(boxStyle);
}

export Ui::Child avatar(Mdi::Icon i) {
    Ui::BoxStyle boxStyle = {
        .borderRadii = 99,
        .backgroundFill = Ui::GRAY800,
        .foregroundFill = Ui::GRAY400
    };

    return Ui::icon(i, 26) |
           Ui::center() |
           Ui::pinSize(46) |
           Ui::box(boxStyle);
}

export Ui::Child avatar() {
    return avatar(Mdi::ACCOUNT);
}

} // namespace Karm::Kira
