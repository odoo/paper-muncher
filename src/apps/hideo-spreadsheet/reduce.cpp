#include "app.h"

namespace Spreadsheet {

State reduce(State s, Actions a) {
    a.visit(
        Visitor{
            [&](UpdateSelection &u) {
                s.selection = u.range;
            },
            [&](ToggleProperties &) {
                s.propertiesVisible = not s.propertiesVisible;
            },
            [&](SwitchSheet &u) {
                s.active = u.index;
                s.selection = NONE;
            },
            [&](auto &) {
                debug("Unhandled action");
            },
        });

    s.activeSheet().recompute();
    return s;
}

} // namespace Spreadsheet
