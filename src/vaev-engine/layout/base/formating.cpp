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

// https://www.w3.org/TR/css-layout-api-1/#intrinsic-sizes
export struct IntrinsicSizes {
    Au minContentSize;
    Au maxContentSize;
};

struct FormatingContext {
    virtual ~FormatingContext() = default;

    virtual void build(Tree&, Box&) {};

    virtual IntrinsicSizes intrinsicSizes(Tree&, Box& box, Input) {
        panic("intrinsicSizes not implemented for formatting context");
    }

    virtual Output run(Tree& tree, Box& box, Input input, usize startAt, Opt<usize> stopAt) = 0;
};

} // namespace Vaev::Layout
