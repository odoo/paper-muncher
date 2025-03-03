#include <karm-io/expr.h>

#include "header.h"

namespace Karm::Net::Http {

// MARK: Version ---------------------------------------------------------------

Res<Version> Version::parse(Io::SScan& s) {
    if (not s.skip("HTTP/"))
        return Error::invalidData("Expected \"HTTP/\"");
    Version v;
    v.major = try$(atou(s));
    s.skip('.');
    v.minor = try$(atou(s));
    return Ok(v);
}

Res<> Version::unparse(Io::TextWriter& w) {
    try$(Io::format(w, "HTTP/{}.{}", major, minor));
    return Ok();
}

// MARK: Header ----------------------------------------------------------------

void Header::add(Str const& key, Str value) {
    put(key, std::move(value));
}

Res<> Header::parse(Io::SScan& s) {
    while (not s.ended()) {
        Str key;
        Str value;

        auto RE_ENDLINE =
            Re::zeroOrMore(' '_re) & "\r\n"_re;

        auto RE_SEPARATOR =
            Re::separator(':'_re);

        auto RE_KEY_VALUE =
            Re::token(
                key,
                Re::until(RE_SEPARATOR)
            ) &
            RE_SEPARATOR &
            Re::token(
                value,
                Re::until(RE_ENDLINE)
            ) &
            RE_ENDLINE;

        if (s.skip("\r\n"))
            break;

        if (not s.skip(RE_KEY_VALUE))
            return Error::invalidData("Expected header");

        put(key, value);
    }

    return Ok();
}

Res<> Header::unparse(Io::TextWriter& w) {
    for (auto& [key, value] : iter()) {
        try$(Io::format(w, "{}: {}\r\n", key, value));
    }

    try$(w.writeStr("\r\n"s));

    return Ok();
}

} // namespace Karm::Http
