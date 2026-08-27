export module Vaev.Browser:dialogs;

import Karm.Core;
import Karm.Gc;
import Karm.Gfx;
import Karm.Kira;
import Karm.Print;
import Karm.Print.Dialog;
import Karm.Ui;
import Vaev.Engine;

using namespace Karm;

namespace Vaev::View {

export Ui::Child printDialog(Rc<Dom::Window> window) {
    return Print::printDialog(
        [window](Print::Settings const& settings) -> Vec<Gfx::Snapshot> {
            return window->print(settings) | Collect<Vec<Gfx::Snapshot>>();
        }
    );
}

} // namespace Vaev::View
