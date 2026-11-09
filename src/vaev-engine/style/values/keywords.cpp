export module Vaev.Engine:style.values.keywords;

import Karm.Core;

import :css.values.keywords;

using namespace Karm;

namespace Vaev::Style {

// Keywords compute as specified.
export using Css::Keyword;
export namespace Keywords = Css::Keywords;

export template <StrLit K>
Keyword<K> toComputed(Keyword<K> const& value, auto const&) {
    return value;
}

export template <StrLit K>
Keyword<K> toSpecified(Keyword<K> const& value) {
    return value;
}

} // namespace Vaev::Style
