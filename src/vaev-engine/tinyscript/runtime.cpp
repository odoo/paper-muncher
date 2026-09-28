export module Vaev.Engine:tinyscript.runtime;

import Karm.Core;

using namespace Karm;

namespace Vaev::TinyScript {

export struct Realm {
    virtual ~Realm() = default;

    virtual void eval(Str source) = 0;
};

export struct Agent {
    virtual ~Agent() = default;
};

} // namespace Vaev::TinyScript
