export module Vaev.Engine:style.namespace_;

import Karm.Core;
import :dom.names;

using namespace Karm;

namespace Vaev::Style {

export struct Namespace {
    Symbol default_ = Html::NAMESPACE;

    // Whether `default_` came from an actual `@namespace` declaration rather than
    // from the fallback above. Only an explicitly declared default namespace gets
    // applied to the universal selector implied by type-less compound selectors.
    bool explicitDefault = false;

    Map<Symbol, Symbol> prefixes = {};

    Res<Symbol> lookup(Symbol sym) const {
        auto maybeRes = prefixes.lookup(sym);
        if (maybeRes == NONE)
            return Error::invalidInput("unknown namespace prefix");
        return Ok(*maybeRes);
    }
};

} // namespace Vaev::Style
