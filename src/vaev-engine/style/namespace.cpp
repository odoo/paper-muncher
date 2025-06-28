module;

#include <karm-base/map.h>
#include <karm-base/res.h>
#include <karm-base/symbol.h>

export module Vaev.Engine:style.namespace_;

import :dom;

namespace Vaev::Style {

export struct Namespace {
    Symbol default_ = Html::NAMESPACE;
    Map<Symbol, Symbol> prefixes = {};

    Res<Symbol> lookup(Symbol sym) const {
        auto maybeRes = prefixes.tryGet(sym);
        if (maybeRes == NONE)
            return Error::invalidInput("unknown namespace prefix");
        return Ok(*maybeRes);
    }
};

} // namespace Vaev::Style
