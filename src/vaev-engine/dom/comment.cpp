export module Vaev.Engine:dom.comment;

import :dom.characterData;

namespace Vaev::Dom {

// https://dom.spec.whatwg.org/#interface-comment
export struct Comment : CharacterData {
    using CharacterData::CharacterData;

    static constexpr auto TYPE = NodeType::COMMENT;

    NodeType nodeType() const override {
        return TYPE;
    }

    bool is(Meta::Id id) const override {
        return id == Meta::idOf<Comment>() or PlatformObject::is(id);
    }
};

} // namespace Vaev::Dom
