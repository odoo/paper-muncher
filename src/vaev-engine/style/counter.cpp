export module Vaev.Engine:style.counter;

import Karm.Core;
import :values.primitives;

using namespace Karm;

namespace Vaev::Style {

struct CounterSet {
    struct Counter {
        bool reversed;
        Integer value;
    };

    Map<CustomIdent, Integer> _counters;

    // https://drafts.csswg.org/css-lists/#propdef-counter-increment
    void increment(CustomIdent counter, Opt<int> by) {
    }

    // https://drafts.csswg.org/css-lists/#propdef-counter-set
    void set(CustomIdent counter) {}

    // https://drafts.csswg.org/css-lists/#counter-reset
    void reset(CustomIdent counter) {}
};

} // namespace Vaev::Style