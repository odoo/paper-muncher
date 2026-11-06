module;

#include <karm/macros>

export module Vaev.Engine:values.content;

import Karm.Core;

import :values.counter;
import :values.keywords;
import :values.primitives;

using namespace Karm;

namespace Vaev {

// https://www.w3.org/TR/css-content-3/
// https://www.w3.org/TR/css-gcpm-3/#funcdef-element
export struct ElementFunc {
    enum struct Target {
        UNDEFINED,
        FIRST,
        START,
        LAST,
        FIRST_EXCEPT
    };

    CustomIdent customIdent = {""_sym};
    Target target = Target::UNDEFINED;

    explicit ElementFunc(CustomIdent customIdent, Target target = Target::UNDEFINED)
        : customIdent(customIdent), target(target) {}

    void repr(Io::Emit& e) const {
        e("element '{}'", customIdent);
    }
};

export using ContentValue = Union<
    Keywords::Normal,
    Keywords::None,
    String,
    ElementFunc,
    CounterFunc,
    CountersFunc>;

} // namespace Vaev
