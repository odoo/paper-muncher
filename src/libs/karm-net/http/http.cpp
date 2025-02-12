#include <karm-io/funcs.h>

#include "http.h"

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
        Str key, value;

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

// MARK: Request ---------------------------------------------------------------

Res<Request> Request::parse(Io::SScan& s) {
    Request req;

    req.method = try$(parseMethod(s));

    if (not s.skip(' '))
        return Error::invalidData("Expected space");

    req.path = Mime::Path::parse(s, true, true);
    req.path.rooted = true;
    req.path.normalize();
    req.path.rooted = false;

    if (not s.skip(' '))
        return Error::invalidData("Expected space");

    req.version = try$(Version::parse(s));

    if (not s.skip("\r\n"))
        return Error::invalidData("Expected \"\\r\\n\"");

    try$(req.header.parse(s));

    return Ok(req);
}

Res<> Request::unparse(Io::TextWriter& w) {
    // Start line

    path.rooted = true;
    try$(Io::format(w, "{} {} ", toStr(method), path));
    path.rooted = false;

    try$(version.unparse(w));
    try$(w.writeStr("\r\n"s));

    // Headers and empty line
    try$(header.unparse(w));

    return Ok();
}

Res<Response> Response::parse(Io::SScan& s) {
    Response res;

    res.version = try$(Version::parse(s));

    if (not s.skip(' '))
        return Error::invalidData("Expected space");

    res.code = try$(parseCode(s));

    if (not s.skip(' '))
        return Error::invalidData("Expected space");

    s.skip(Re::untilAndConsume("\r\n"_re));

    try$(res.header.parse(s));

    return Ok(res);
}

Res<Response> Response::read(Io::Reader& r) {
    Io::BufferWriter bw;
    while (true) {
        auto [read, reachedDelim] = try$(Io::readLine(
            r, bw, bytes("\r\n"s)
        ));

        if (not reachedDelim)
            return Error::invalidInput("input stream ended with incomplete http header");

        if (read == 0)
            break;
    }

    Io::SScan scan{bw.bytes().cast<char>()};
    return parse(scan);
}

Res<Opt<Buf<Byte>>> Response::readBody(Io::Reader& r) {
    auto contentLengthValue = header.tryGet("Content-Length"s);
    if (not contentLengthValue)
        return Ok(NONE);

    auto contentLength = try$(Io::atou(contentLengthValue.unwrap().str()));

    Io::BufferWriter bodyBytes;
    auto read = try$(Io::copy(r, bodyBytes, contentLength));

    if (read < contentLength)
        return Error::invalidInput("read body length is smaller than Content-Length header value");

    return Ok(bodyBytes.take());
}
} // namespace Karm::Net::Http
