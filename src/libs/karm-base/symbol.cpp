#include "symbol.h"

namespace Karm {

static Set<Rc<_SymbolBuf>>& _symboleRegistry() {
    static Set<Rc<_SymbolBuf>> _registry;
    return _registry;
}

Symbol Symbol::from(Str str) {
    auto& registry = _symboleRegistry();
    registry.ensureForInsert();
    auto* slot = registry.lookup(str);
    if (slot->state) {
        return {slot->unwrap()};
    }

    auto buf = _SymbolBuf::from(str);
    registry.put(buf);
    return {buf};
}

} // namespace Karm
