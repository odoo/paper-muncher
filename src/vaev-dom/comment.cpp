export module Vaev.Dom:comment;

import :character_data;

namespace Vaev::Dom {

// https://dom.spec.whatwg.org/#interface-comment
export struct Comment : CharacterData {
    using CharacterData::CharacterData;

    static constexpr auto TYPE = NodeType::COMMENT;

    NodeType nodeType() const override {
        return TYPE;
    }
};

} // namespace Vaev::Dom
