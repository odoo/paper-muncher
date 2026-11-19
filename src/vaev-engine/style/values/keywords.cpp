export module Vaev.Engine:style.values.keywords;

import Karm.Core;

import :css.values.keywords;
import :style.values.base;

using namespace Karm;

namespace Vaev::Style {

export template <StrLit K>
struct ValueComputer<Keyword<K>> {
    static Keyword<K> compute(Keyword<K> const& value, auto const&...) {
        return value;
    }
};

} // namespace Vaev::Style
