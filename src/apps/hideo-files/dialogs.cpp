#include <karm-kira/dialog.h>
#include <karm-ui/layout.h>

#include "dialogs.h"
#include "widgets.h"

namespace Hideo::Files {

Ui::Child openDialog(OnFile onFile) {
    return Ui::reducer<Model>(
        {"location://home"_url},
        [onFile](State const& s) {
            auto maybeDir = Sys::Dir::open(s.currentUrl());

            return Kr::dialogContent({
                Kr::dialogTitleBar("Open File…"s),
                toolbar(s),
                (maybeDir
                     ? directoryListing(s, maybeDir.unwrap())
                     : alert(
                           s,
                           "Can't access this location"s,
                           Io::toStr(maybeDir.none())
                       )
                ) | Ui::pinSize({400, 260}),
                Ui::separator(),
                Kr::dialogFooter({
                    Ui::grow(NONE),
                    Kr::dialogCancel(),
                    Kr::dialogAction(
                        [&, onFile](auto& n) {
                            onFile(n, s.currentUrl());
                        },
                        "Open"s
                    ),
                }),
            });
        }
    );
}

Ui::Child saveDialog(OnFile onFile) {
    return Ui::reducer<Model>(
        {"location://home"_url},
        [onFile](State const& s) {
            auto maybeDir = Sys::Dir::open(s.currentUrl());

            return Kr::dialogContent({
                Kr::dialogTitleBar("Save As…"s),
                toolbar(s),
                (maybeDir
                     ? directoryListing(s, maybeDir.unwrap())
                     : alert(
                           s,
                           "Can't access this location"s,
                           Io::toStr(maybeDir.none())
                       )
                ) | Ui::pinSize({400, 260}),
                Ui::separator(),
                Kr::dialogFooter({
                    Ui::grow(NONE),
                    Kr::dialogCancel(),
                    Kr::dialogAction(
                        [&, onFile](auto& n) {
                            onFile(n, s.currentUrl());
                        },
                        "Save"s
                    ),
                }),
            });
        }
    );
}

Ui::Child directoryDialog(OnFile onFile) {
    return Ui::reducer<Model>(
        {"location://home"_url},
        [onFile](auto const& d) {
            auto maybeDir = Sys::Dir::open(d.currentUrl());

            return Kr::dialogContent({
                Kr::dialogTitleBar("Select Directory…"s),
                toolbar(d),
                (maybeDir
                     ? directoryListing(d, maybeDir.unwrap())
                     : alert(
                           d,
                           "Can't access this location"s,
                           Io::toStr(maybeDir.none())
                       )
                ) | Ui::pinSize({400, 260}),
                Ui::separator(),
                Kr::dialogFooter({
                    Ui::grow(NONE),
                    Kr::dialogCancel(),
                    Kr::dialogAction(
                        [&, onFile](auto& n) {
                            onFile(n, d.currentUrl());
                        },
                        "Select"s
                    ),
                }),
            });
        }
    );
}

} // namespace Hideo::Files
