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

export Ui::Child printDialog(Rc<WebView> webview) {
    return Print::printDialog(
        [webview](Print::Settings const& settings) -> Vec<Gfx::Snapshot> {
            return webview->print(settings) | Collect<Vec<Gfx::Snapshot>>();
        }
    );
}

} // namespace Vaev::View
