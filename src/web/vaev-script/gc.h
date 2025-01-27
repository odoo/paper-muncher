#pragma once

#include <karm-base/base.h>
#include <karm-base/panic.h>
#include <karm-io/emit.h>

namespace Karm::Gc {

template <typename T>
struct Ref {
    T* _ptr = nullptr;

    Ref() = default;

    Ref(T* ptr) : _ptr{ptr} {
        if (not _ptr)
            panic("null pointer");
    }

    T const* operator->() const {
        return _ptr;
    }

    T* operator->() {
        return _ptr;
    }

    T const& operator*() const {
        return *_ptr;
    }

    T& operator*() {
        return *_ptr;
    }

    void repr(Io::Emit& e) const {
        e("{}", *_ptr);
    }

    bool checkIdentity(Ref const& other) const {
        return _ptr == other._ptr;
    }
};

// Nullable reference
template <typename T>
struct Ptr {
    T* _ptr = nullptr;

    Ptr() = default;

    Ptr(T* ptr) : _ptr{ptr} {
    }

    T const* operator->() const {
        if (not _ptr)
            panic("trying to dereference a null pointer");
        return _ptr;
    }

    T* operator->() {
        if (not _ptr)
            panic("trying to dereference a null pointer");
        return _ptr;
    }

    T const& operator*() const {
        if (not _ptr)
            panic("trying to dereference a null pointer");
        return *_ptr;
    }

    T& operator*() {
        if (not _ptr)
            panic("trying to dereference a null pointer");
        return *_ptr;
    }

    bool operator==(None) const {
        return not _ptr;
    }

    void repr(Io::Emit& e) const {
        if (not _ptr)
            e("nullptr");
        else
            e("{}", *_ptr);
    }

    explicit operator bool() const {
        return _ptr != nullptr;
    }

    bool checkIdentity(Ptr const& other) const {
        return _ptr == other._ptr;
    }

    Gc::Ref<T> upgrade() const {
        if (not _ptr)
            panic("trying to upgrade a null pointer");
        return _ptr;
    }
};

struct Gc {
    template <typename T, typename... Args>
    Ref<T> alloc(Args&&... args) {
        return new T{std::forward<Args>(args)...};
    }
};

} // namespace Karm::Gc
