export module Vaev.Engine:layout.formating;

import Karm.Math;
import :values;
import :layout.box;
import :layout.breaks;
import :layout.input;
import :layout.output;

using namespace Karm;

namespace Vaev::Layout {

export struct Viewport {
    Resolution dpi = Resolution::fromDpi(96);
    // https://drafts.csswg.org/css-values/#small-viewport-size
    RectAu small;
    // https://drafts.csswg.org/css-values/#large-viewport-size
    RectAu large = small;
    // https://drafts.csswg.org/css-values/#dynamic-viewport-size
    RectAu dynamic = small;
};

export struct Tree {
    Box root;
    Viewport viewport = {};
    Fragmentainer fc = {};
};

struct FormatingContext {
    virtual ~FormatingContext() = default;

    virtual void build(Tree&, Box&) {};

    virtual Output run(Tree& tree, Box& box, Input input, usize startAt, Opt<usize> stopAt) = 0;
};

} // namespace Vaev::Layout
