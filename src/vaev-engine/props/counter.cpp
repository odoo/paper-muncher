module;

#include <karm/macros>

export module Vaev.Engine:props.counter;

import :props.base;
import :style.counter;

using namespace Karm;

namespace Vaev::Style {

// https://drafts.csswg.org/css-lists/#counter-reset
struct CounterResetProperty : Property {
    struct Registration : Property::Registration {
        Symbol name() const override {
            return Properties::COUNTER_RESET;
        }
    };

    Vec<Counter> _value;
};

// https://drafts.csswg.org/css-lists/#propdef-counter-increment
struct CounterIncrementProperty : Property {
    struct Registration : Property::Registration {
        Symbol name() const override {
            return Properties::COUNTER_INCREMENT;
        }
    };
};

// https://drafts.csswg.org/css-lists/#propdef-counter-set
struct CounterSetProperty : Property {
    struct Registration : Property::Registration {
        Symbol name() const override {
            return Properties::COUNTER_SET;
        }
    };
};

} // namespace Vaev::Style