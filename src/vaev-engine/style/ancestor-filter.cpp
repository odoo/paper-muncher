export module Vaev.Engine:style.ancestorFilter;

import Karm.Core;
import Karm.Gc;

import :dom.element;

using namespace Karm;

namespace Vaev::Style {

export struct AncestorFilter {
    enum class Category : u8 {
        ID,
        CLASS,
        TYPE,
        ATTR,
    };
    using enum Category;

    static constexpr u16 BLOOM_SIZE = 8192;

    Array<u8, BLOOM_SIZE / 8> _bloomBuf;
    Vec<u16> _journal;
    Vec<usize> _journalMarks;

    static u16 hashEntry(Category category, Str name) {
        DefaultHasher h;
        Karm::hash(h, category);
        Karm::hash(h, name);
        return static_cast<u16>(h.finish());
    }

    static u16 _index(u16 hash) {
        return hash & (BLOOM_SIZE - 1);
    }

    void _add(u16 hash, usize& journalEntrySize) {
        auto bloom = MutBits{_bloomBuf.mutBytes()};
        auto index = _index(hash);

        if (not bloom.get(index)) {
            _journal.pushBack(index);
            journalEntrySize++;
        }

        bloom.set(index, true);
    }

    void push(Gc::Ref<Dom::Element> el) {
        auto& journalEntrySize = _journalMarks.emplaceBack();

        _add(hashEntry(TYPE, el->qualifiedName.name.str()), journalEntrySize);

        if (auto [id] = el->id()) {
            _add(hashEntry(ID, id), journalEntrySize);
        }

        for (auto const& class_ : el->classList._tokens) {
            _add(hashEntry(CLASS, class_), journalEntrySize);
        }

        for (auto const& attr : el->attributes) {
            if (not oneOf(attr.qualifiedName, Html::ID_ATTR, Html::CLASS_ATTR, Html::STYLE_ATTR)) {
                _add(hashEntry(ATTR, attr.qualifiedName.name.str()), journalEntrySize);
            }
        }
    }

    void pop() {
        auto bloom = MutBits{_bloomBuf.mutBytes()};

        usize counter = _journalMarks.popBack();
        while (counter > 0) {
            bloom.set(_journal.popBack(), false);
            counter--;
        }
    }

    bool rejects(Slice<u16> ancestorHashes) const {
        auto bloom = Bits{_bloomBuf.bytes()};
        for (auto hash : ancestorHashes)
            if (not bloom.get(_index(hash)))
                return true;
        return false;
    }
};

} // namespace Vaev::Style
