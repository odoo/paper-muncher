export module Vaev.Engine:layout.formating;

import Karm.Math;
import :style.values;
import :layout.box;
import :layout.breaks;
import :layout.input;
import :layout.output;

using namespace Karm;

namespace Vaev::Layout {

struct FormatingContext {
    virtual ~FormatingContext() = default;

    virtual void build(Box&) {};

    virtual Output run(Box& box, Input input, usize startAt, Opt<usize> stopAt) = 0;
};

} // namespace Vaev::Layout
