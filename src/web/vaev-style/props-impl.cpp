module;

#include <karm-logger/logger.h>

module Vaev.Style:props_impl;

import :decls;
import :computed;

namespace Vaev::Style {

static constexpr bool DEBUG_PROPS = false;

void DeferredProp::apply(Computed const& parent, Computed& c) const {
    Css::Sst decl{Css::Sst::DECL};
    decl.token = Css::Token::ident(propName);
    Cursor<Css::Sst> cursor = value;
    _expandContent(cursor, *c.variables, decl.content);

    // Parse the expanded content
    Res<StyleProp> computed = parseDeclaration<StyleProp>(decl, false);
    if (not computed) {
        logWarnIf(DEBUG_PROPS, "failed to parse declaration: {}: {}", decl, computed);
    } else {
        computed.unwrap().apply(parent, c);
    }
}

} // namespace Vaev::Style
