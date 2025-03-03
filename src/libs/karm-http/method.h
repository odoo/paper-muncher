#pragma once

#include <karm-io/aton.h>
#include <karm-io/fmt.h>

namespace Karm::Net::Http {

#define FOREACH_METHOD(METHOD) \
    METHOD(GET)                \
    METHOD(HEAD)               \
    METHOD(POST)               \
    METHOD(PUT)                \
    METHOD(DELETE)             \
    METHOD(CONNECT)            \
    METHOD(OPTIONS)            \
    METHOD(TRACE)              \
    METHOD(PATCH)

enum struct Method : u8 {
#define ITER(NAME) NAME,
    FOREACH_METHOD(ITER)
#undef ITER
};

using enum Method;

static inline Res<Method> parseMethod(Io::SScan& s) {
#define ITER(NAME)     \
    if (s.skip(#NAME)) \
        return Ok(Method::NAME);
    FOREACH_METHOD(ITER)
#undef ITER
    return Error::invalidData("Expected method");
}

static inline Str toStr(Method method) {
    switch (method) {
#define ITER(NAME)     \
    case Method::NAME: \
        return #NAME;
        FOREACH_METHOD(ITER)
#undef ITER
    }
    return "UNKNOWN";
}

} // namespace Karm::Net::Http

template <>
struct Karm::Io::Formatter<Karm::Net::Http::Method> {
    Res<> format(Io::TextWriter& writer, Karm::Net::Http::Method method) {
        return writer.writeStr(Karm::Net::Http::toStr(method));
    }
};
