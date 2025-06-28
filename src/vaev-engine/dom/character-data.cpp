module;

#include <karm-base/string.h>
#include <karm-io/emit.h>

export module Vaev.Engine:dom.character_data;

import :dom.node;

namespace Vaev::Dom {

// https://dom.spec.whatwg.org/#interface-characterdata
export struct CharacterData : Node {
    StringBuilder _data;

    CharacterData() = default;

    CharacterData(String data)
        : _data(std::move(data)) {
    }

    void appendData(Str s) {
        _data.append(s);
    }

    void appendData(Rune rune) {
        _data.append(rune);
    }

    Str data() const {
        return _data.str();
    }

    void _repr(Io::Emit& e) const override {
        e(" data={#}", data());
    }
};

} // namespace Vaev::Dom
