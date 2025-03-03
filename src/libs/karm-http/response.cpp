#include <karm-io/expr.h>
#include <karm-io/funcs.h>

#include "response.h"

namespace Karm::Net::Http {

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

} // namespace Karm::Http
