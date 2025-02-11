#pragma once

#include <karm-io/text.h>

#include "named.h"

namespace Karm::Fmt {

struct _Item {
    Str name;

    virtual ~_Item() = default;

    virtual Res<> format(Io::TextWriter& w, Str format) const = 0;
};

template <typename T>
static inline auto _makeItem(T const& t) {
    struct Item : public _Item {
        T const& value;

        Item(T const& value) : value(value) {}

        Res<> format(Io::TextWriter& w, Str format) const override;
    };

    return Item{t};
}

template <typename T>
static inline auto _makeItem(Named<T> const& t) {
    auto item = _makeItem(t.named);
    item.name = t.name;
    return item;
}

template <typename T>
using _ItemFor = decltype(_makeItem(std::declval<T>()));

struct _Items {
    virtual ~_Items() = default;
    virtual _Item const* byIndex(usize index) const = 0;
    virtual _Item const* byName(Str name) const = 0;
};

template <typename... Ts>
struct Items : public _Items {
    Tuple<_ItemFor<Ts>...> _items;

    Items(Ts const&... ts) : _items{_makeItem(ts)...} {}

    _Item const* byIndex(usize index) const override {
        usize i = 0;
        _Item const* item = nullptr;
        _items.visit([&](auto const& t) {
            if (index == i) {
                item = &t;
                return false;
            }
            i++;
            return true;
        });
        return item;
    }

    _Item const* byName(Str name) const override {
        _Item const* item = nullptr;
        _items.visit([&](auto const& t) {
            if (t.name == name) {
                item = &t;
                return false;
            }
            return true;
        });
        return item;
    }
};

} // namespace Karm::Fmt
