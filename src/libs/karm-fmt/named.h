#pragma once

#include <karm-base/string.h>

namespace Karm::Fmt {

struct Name {
    Str name;
};

template <typename T>
struct Named {
    T const& named;
    Str name;
};

template <typename T>
Named<T> operator|(T const& t, Name n) {
    return Named<T>{t, n.name};
}

} // namespace Karm::Fmt
