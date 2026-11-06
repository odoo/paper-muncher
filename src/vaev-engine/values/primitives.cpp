export module Vaev.Engine:values.primitives;

import Karm.Core;

using namespace Karm;
using namespace Karm::Literals;

namespace Vaev {

export using Integer = isize;

export struct CustomIdent {
    Symbol _symbol = ""_sym;

    bool operator==(CustomIdent const& other) const = default;

    auto operator<=>(CustomIdent const& other) const = default;

    explicit operator bool() const {
        return static_cast<bool>(_symbol);
    }

    Str str() const {
        return _symbol.str();
    }

    void hash(Meta::Derive<Hasher> auto& h) const {
        Karm::hash(h, _symbol);
    }

    void repr(Io::Emit& e) const {
        e("(custom-ident '{}')", _symbol);
    }
};

} // namespace Vaev
