#pragma once

// https://url.spec.whatwg.org/

#include <karm-base/string.h>
#include <karm-base/vec.h>
#include <karm-io/expr.h>
#include <karm-io/fmt.h>
#include <karm-io/sscan.h>

namespace marK::Mime {

// MARK: Path ------------------------------------------------------------------

static inline Str suffixOf(Str str) {
    auto dotIndex = lastIndexOf(str, '.');
    if (not dotIndex.has())
        return "";
    return next(str, *dotIndex + 1);
}

struct Path {
    static constexpr auto SEP = '/';

    bool rooted = false;
    Vec<String> _parts;

    static Path parse(Io::SScan& s, bool inUrl = false, bool stopAtWhitespace = false);

    static Path parse(Str str, bool inUrl = false, bool stopAtWhitespace = false);

    void normalize();

    Str basename() const;

    Path join(Path const& other) const;

    Path join(Str other) const;

    void append(Str part) {
        _parts.pushBack(part);
    }

    Path parent(usize n = 0) const;

    bool isParentOf(Path const& other) const;

    Res<> unparse(Io::TextWriter& writer) const;

    String str() const;

    auto iter() const {
        return ::iter(_parts);
    }

    auto operator[](usize i) const {
        return _parts[i];
    }

    auto len() const {
        return _parts.len();
    }

    auto operator<=>(Path const&) const = default;

    Str suffix() const {
        if (not _parts.len())
            return "";
        auto dotIndex = lastIndexOf(last(_parts), '.');
        if (not dotIndex.has())
            return "";
        return next(last(_parts), *dotIndex + 1);
    }
};

} // namespace Karm::Mime

inline auto operator""_path(char const* str, usize len) {
    return marK::Mime::Path::parse({str, len});
}

inline auto operator/(marK::Mime::Path const& path, marK::Mime::Path const& other) {
    return path.join(other);
}

inline auto operator/(marK::Mime::Path const& path, Str other) {
    return path.join(other);
}

template <>
struct marK::Io::Formatter<marK::Mime::Path> {
    Res<> format(Io::TextWriter& writer, marK::Mime::Path const& path) {
        return path.unparse(writer);
    }
};
