#include <karm-base/ctype.h>
#include <karm-io/aton.h>

#include "url.h"

namespace Karm::Mime {

static auto const RE_COMPONENT =
    Re::alpha() &
    Re::zeroOrMore(
        Re::alnum() | '+'_re | '-'_re | '.'_re
    );

static auto const RE_SCHEME = RE_COMPONENT & ':'_re;

Url Url::parse(Io::SScan& s, Opt<Url> baseUrl) {
    Url url;

    if (s.ahead(RE_SCHEME)) {
        url.scheme = s.token(RE_COMPONENT);
        s.skip(':');
    } else if (baseUrl) {
        url.scheme = baseUrl->scheme;
        url.userInfo = baseUrl->userInfo;
        url.host = baseUrl->host;
        url.port = baseUrl->port;
        url.path = baseUrl->path;
    }

    if (s.skip("//")) {
        auto maybeHost = s.token(RE_COMPONENT);

        if (s.skip('@')) {
            url.userInfo = maybeHost;
            maybeHost = s.token(RE_COMPONENT);
        }

        url.host = maybeHost;

        if (s.skip(':')) {
            url.port = atou(s);
        }
    }

    url.path = Path::parse(s, true);
    if (not url.path.rooted and baseUrl)
        url.path = baseUrl->path.join(url.path);

    if (s.skip('?'))
        url.query = s.token(Re::until('#'_re));

    if (s.skip('#'))
        url.fragment = s.token(Re::until(Re::eof()));

    return url;
}

Url Url::parse(Str str, Opt<Url> origin) {
    Io::SScan s{str};
    return parse(s, origin);
}

bool Url::isUrl(Str str) {
    Io::SScan s{str};

    return s.skip(RE_COMPONENT) and
           s.skip(':');
}

Url Url::join(Path const& other) const {
    Url url = *this;
    url.path = url.path.join(other);
    return url;
}

Url Url::join(Str other) const {
    return join(Path::parse(other));
}

Str Url::basename() const {
    return path.basename();
}

Url Url::parent(usize n) const {
    Url url = *this;
    url.path = url.path.parent(n);
    return url;
}

bool Url::isParentOf(Url const& other) const {
    bool same = scheme == other.scheme and
                host == other.host and
                port == other.port;

    return same and path.isParentOf(other.path);
}

Res<> Url::unparse(Io::TextWriter& writer) const {
    if (scheme.len() > 0)
        try$(Io::format(writer, "{}:", scheme));

    if (userInfo.len() > 0 or host.len() > 0)
        try$(writer.writeStr("//"s));

    if (userInfo.len() > 0)
        try$(Io::format(writer, "{}@", userInfo));

    if (host.len() > 0)
        try$(writer.writeStr(host.str()));

    if (port)
        try$(Io::format(writer, ":{}", port.unwrap()));

    try$(path.unparse(writer));

    if (query.len() > 0)
        try$(Io::format(writer, "?{}", query));

    if (fragment.len() > 0)
        try$(Io::format(writer, "#{}", fragment));

    return Ok();
}

String Url::str() const {
    Io::StringWriter writer;
    unparse(writer).unwrap("unparse error");
    return writer.str();
}

Url parseUrlOrPath(Str str, Opt<Url> baseUrl) {
    if (Url::isUrl(str)) {
        return Url::parse(str, baseUrl);
    }

    Url url = baseUrl.unwrapOr(""_url);
    url.path = url.path.join(Path::parse(str));

    return url;
}

} // namespace Karm::Mime
