export module Vaev.Engine:tinyscript.element;

import Karm.Core;

using namespace Karm;

namespace Vaev::TinyScript {

using DomString = _Str<Utf16>;

struct EventTarget {
    // TODO
};

struct Document {
};

struct Window : EventTarget {
        Document* document;
};

struct Node : EventTarget {
    // TODO
};

struct Element : Node {
};

} // namespace Vaev::TinyScript
