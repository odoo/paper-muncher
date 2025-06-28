module;

#include <karm-gc/ptr.h>
#include <karm-print/page.h>

export module Vaev.View:dialog;

import Hideo.Printers;
import Karm.Kira;
import Karm.Ui;
import Vaev.Engine;

namespace Vaev::View {

export Ui::Child printDialog(Gc::Ref<Dom::Document> dom) {
    return Hideo::Printers::printDialog([dom](Print::Settings const& settings) -> Vec<Print::Page> {
        return Driver::print(dom, settings) | collect<Vec<Print::Page>>();
    });
}

} // namespace Vaev::View
