#include <karm-kira/print-dialog.h>
#include <vaev-driver/print.h>

#include "dialog.h"

namespace Vaev::View {

Ui::Child printDialog(Driver::Fetcher& fetcher, Gc::Ref<Dom::Document> dom) {
    return Kr::printDialog([&fetcher, dom](Print::Settings const& settings) -> Vec<Print::Page> {
        return Driver::print(fetcher, dom, settings) | collect<Vec<Print::Page>>();
    });
}

} // namespace Vaev::View
