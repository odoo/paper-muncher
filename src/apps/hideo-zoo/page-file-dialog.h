#pragma once

#include <hideo-files/dialogs.h>
#include <karm-ui/dialog.h>
#include <karm-ui/input.h>
#include <karm-ui/layout.h>
#include <mdi/file-search.h>

#include "model.h"

namespace Hideo::Zoo {

static inline Page PAGE_FILE_DIALOG{
    Mdi::FILE_SEARCH,
    "File Dialog",
    "A window overlaid on either the primary window or another dialog window, rendering the content underneath inert.",
    [] {
        return Ui::vflow(
                   6,
                   Ui::button(
                       [](auto& n) {
                           Ui::showDialog(
                               n,
                               Hideo::Files::openDialog([](auto&, auto url) {
                                   logInfo("selected file: {}", url);
                               })
                           );
                       },
                       "Open File..."
                   ),
                   Ui::button(
                       [](auto& n) {
                           Ui::showDialog(
                               n,
                               Hideo::Files::saveDialog([](auto&, auto url) {
                                   logInfo("selected file: {}", url);
                               })
                           );
                       },
                       "Save As..."
                   ),
                   Ui::button(
                       [](auto& n) {
                           Ui::showDialog(
                               n,
                               Hideo::Files::directoryDialog([](auto&, auto url) {
                                   logInfo("selected directory: {}", url);
                               })
                           );
                       },
                       "Open Directory..."
                   )
               ) |
               Ui::center();
    },
};

} // namespace Hideo::Zoo
