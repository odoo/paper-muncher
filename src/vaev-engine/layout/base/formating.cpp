export module Vaev.Engine:layout.formating;

import Karm.Math;
import :values;
import :layout.box;
import :layout.breaks;
import :layout.input;
import :layout.output;

using namespace Karm;

namespace Vaev::Layout {

export struct Tree {
    Box root;
    Style::Viewport viewport = {};
    Fragmentainer fc = {};
};

struct FormatingContext {
    virtual ~FormatingContext() = default;

    virtual void build(Tree&, Box&) {};

    virtual Output run(Tree& tree, Box& box, Input input, usize startAt, Opt<usize> stopAt) = 0;

    virtual IntrinsicSizes intrinsicInlineContentSizes(Tree&, Box&) {
        panic("intrinsicInlineContentSizes not implemented for formating context");
    }
};

} // namespace Vaev::Layout
