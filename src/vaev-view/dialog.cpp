export module Vaev.View:dialog;

import Hideo.Printers;
import Karm.Core;
import Karm.Gc;
import Karm.Kira;
import Karm.Print;
import Karm.Ui;
import Vaev.Engine;

using namespace Karm;

namespace Vaev::View {

export Ui::Child printDialog(Rc<Dom::Window> window) {
    return Hideo::Printers::printDialog([window](Print::Settings const& settings) -> Vec<Print::Page> {
        return window->print(settings) | Collect<Vec<Print::Page>>();
    });
}

} // namespace Vaev::View
