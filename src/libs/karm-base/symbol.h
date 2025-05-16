#pragma once

#include "rc.h"
#include "set.h"
#include "string.h"

namespace Karm {

struct _SymbolBuf {
    using Inner = char;
    usize _len;
    [[no_unique_address]] char _buf[0];

    usize len() const {
        return _len;
    }

    char const* buf() const {
        return _buf;
    }

    char const& operator[](usize i) const {
        if (i >= _len) [[unlikely]]
            panic("index out of bounds");
        return _buf[i];
    }

    char* buf() {
        return _buf;
    }

    char& operator[](usize i) {
        if (i >= _len) [[unlikely]]
            panic("index out of bounds");
        return _buf[i];
    }

    static Rc<_SymbolBuf> from(Str str) {
        using _StorageCell = Cell<NoLock, _SymbolBuf>;
        Rc<_SymbolBuf> buf = {
            MOVE,
            new (reinterpret_cast<_StorageCell*>(new u8[sizeof(_StorageCell) + str.len() + 1])) _StorageCell(str.len()),
        };
        copy(str, mutSub(*buf));
        buf->_buf[str.len()] = 0;
        return buf;
    }

    bool operator==(_SymbolBuf const& other) const {
        return Str{*this} == Str{other};
    }

    bool operator==(Str const& other) const {
        return Str(*this) == other;
    }

    u64 hash() const {
        return ::hash(Str{*this});
    }
};

struct Symbol {
    static Set<Rc<_SymbolBuf>> _registry;
    Rc<_SymbolBuf> _buf;

    static Symbol from(Str str);

    bool operator==(Symbol const& other) const {
        return _buf._cell == other._buf._cell;
    }

    Str str() const {
        return Str(*_buf);
    }
};

} // namespace Karm
