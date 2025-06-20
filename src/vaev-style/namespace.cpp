module;

#include <karm-base/map.h>
#include <karm-base/symbol.h>

export module Vaev.Style:namespace_;

namespace Vaev::Style {

export struct Namespace {
    Symbol default_ = Html::NAMESPACE;
    Map<Symbol, Symbol> prefixes = {};
};

} // namespace Vaev::Style
