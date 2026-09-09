export module Vaev.Idl:platformObject;

import Karm.Core;
import Karm.Gc;

using namespace Karm;

namespace Vaev::Idl {

export struct PlatformObject {
    virtual ~PlatformObject() = default;

    virtual bool is(Meta::Id id) const {
        return id == Meta::idOf<PlatformObject>();
    }

    template <Meta::Derive<PlatformObject> T>
    bool is() {
        return is(Meta::idOf<T>());
    }

    template <Meta::Derive<PlatformObject> T>
    Gc::Ptr<T> as() {
        if (is(Meta::idOf<T>()))
            return static_cast<T&>(*this);
        return nullptr;
    }

    template <Meta::Derive<PlatformObject> T>
    Gc::Ptr<T const> as() const {
        if (is(Meta::idOf<T>()))
            return static_cast<T const&>(*this);
        return nullptr;
    }
};

} // namespace Vaev::Idl
