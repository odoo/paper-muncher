#pragma once

// https://url.spec.whatwg.org/

#include <karm-io/expr.h>
#include <karm-io/fmt.h>

#include "path.h"

namespace marK::Mime {

struct Url {
    String scheme;
    String userInfo;
    String host;
    Opt<usize> port;
    Path path;
    String query;
    String fragment;

    static Url parse(Io::SScan& s, Opt<Url> baseUrl = NONE);

    static Url parse(Str str, Opt<Url> baseUrl = NONE);

    static bool isUrl(Str str);

    bool rooted() const {
        return path.rooted;
    }

    Url join(Path const& other) const;

    Url join(Str other) const;

    Str basename() const;

    void append(Str part) {
        path.append(part);
    }

    Url parent(usize n = 0) const;

    bool isParentOf(Url const& other) const;

    Res<> unparse(Io::TextWriter& writer) const;

    String str() const;

    auto iter() const {
        return path.iter();
    }

    auto len() const {
        return path.len();
    }

    auto operator<=>(Url const&) const = default;

    bool isRelative() const {
        return not scheme;
    }

    static Res<Url> resolveReference(Url const& baseUrl, Url const& referenceUrl, bool strict = true);
};

Url parseUrlOrPath(Str str, Opt<Url> baseUrl = NONE);

} // namespace Karm::Mime

inline Mime::Url operator""_url(char const* str, usize len) {
    return marK::Mime::Url::parse({str, len});
}

inline Mime::Url operator/(marK::Mime::Url const& url, Str path) {
    return url.join(path);
}

inline Mime::Url operator/(marK::Mime::Url const& url, marK::Mime::Path const& path) {
    return url.join(path);
}

template <>
struct marK::Io::Formatter<marK::Mime::Url> {
    Res<> format(Io::TextWriter& writer, marK::Mime::Url const& url) {
        return url.unparse(writer);
    }
};
