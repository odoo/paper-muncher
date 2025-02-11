#include <karm-kira/print-dialog.h>
#include <vaev-driver/print.h>

#include "dialog.h"

namespace Vaev::View {

Ui::Child printDialog(Driver::Fetcher& fetcher, Gc::Ref<Dom::Document> dom) {
    return Kr::printDialog([&fetcher, dom](Print::Settings const& settings) {
        return Driver::print(fetcher, dom, settings);
    });
}

} // namespace Vaev::View
