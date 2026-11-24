export module Vaev.Engine:layout2.layout;

import :layout.box;
import :layout.formating;
import :layout2.input;
import :layout2.output;

namespace Vaev::Layout2 {

export Output layout(Layout::Tree& tree, Layout::Box& box, Constraints const& constraints);

} // namespace Vaev::Layout2
