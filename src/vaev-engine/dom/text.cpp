export module Vaev.Engine:dom.text;

import :dom.characterData;

namespace Vaev::Dom {

// https://dom.spec.whatwg.org/#text
export struct Text : CharacterData {
    static constexpr auto TYPE = NodeType::TEXT;

    using CharacterData::CharacterData;

    NodeType nodeType() const override {
        return TYPE;
    }
};

} // namespace Vaev::Dom
