module;

#include <karm-io/aton.h>
#include <karm-io/fmt.h>

export module Karm.Http:method;

namespace marK::Http {

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

export enum struct Method : u8 {
#define ITER(NAME) NAME,
    FOREACH_METHOD(ITER)
#undef ITER
};

using enum Method;

export Res<Method> parseMethod(Io::SScan& s) {
#define ITER(NAME)     \
    if (s.skip(#NAME)) \
        return Ok(Method::NAME);
    FOREACH_METHOD(ITER)
#undef ITER
    return Error::invalidData("Expected method");
}

export Str toStr(Method method) {
    switch (method) {
#define ITER(NAME)     \
    case Method::NAME: \
        return #NAME;
        FOREACH_METHOD(ITER)
#undef ITER
    }
    return "UNKNOWN";
}

} // namespace marK::Http

template <>
struct marK::Io::Formatter<marK::Http::Method> {
    Res<> format(Io::TextWriter& writer, marK::Http::Method method) {
        return writer.writeStr(marK::Http::toStr(method));
    }
};
