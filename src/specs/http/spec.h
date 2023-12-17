#pragma once

#include <karm-base/distinct.h>
#include <karm-base/map.h>
#include <karm-base/std.h>
#include <url/url.h>

namespace Http {

#define FOREACH_CODE(CODE)                     \
    CODE(100, CONTINUE)                        \
    CODE(101, SWITCHING_PROTOCOLS)             \
    CODE(102, PROCESSING)                      \
    CODE(103, EARLY_HINTS)                     \
    CODE(200, OK)                              \
    CODE(201, CREATED)                         \
    CODE(202, ACCEPTED)                        \
    CODE(203, NON_AUTHORITATIVE_INFORMATION)   \
    CODE(204, NO_CONTENT)                      \
    CODE(205, RESET_CONTENT)                   \
    CODE(206, PARTIAL_CONTENT)                 \
    CODE(207, MULTI_STATUS)                    \
    CODE(208, ALREADY_REPORTED)                \
    CODE(226, IM_USED)                         \
    CODE(300, MULTIPLE_CHOICES)                \
    CODE(301, MOVED_PERMANENTLY)               \
    CODE(302, FOUND)                           \
    CODE(303, SEE_OTHER)                       \
    CODE(304, NOT_MODIFIED)                    \
    CODE(305, USE_PROXY)                       \
    CODE(306, UNUSED)                          \
    CODE(307, TEMPORARY_REDIRECT)              \
    CODE(308, PERMANENT_REDIRECT)              \
    CODE(400, BAD_REQUEST)                     \
    CODE(401, UNAUTHORIZED)                    \
    CODE(402, PAYMENT_REQUIRED)                \
    CODE(403, FORBIDDEN)                       \
    CODE(404, NOT_FOUND)                       \
    CODE(405, METHOD_NOT_ALLOWED)              \
    CODE(406, NOT_ACCEPTABLE)                  \
    CODE(407, PROXY_AUTHENTICATION_REQUIRED)   \
    CODE(408, REQUEST_TIMEOUT)                 \
    CODE(409, CONFLICT)                        \
    CODE(410, GONE)                            \
    CODE(411, LENGTH_REQUIRED)                 \
    CODE(412, PRECONDITION_FAILED)             \
    CODE(413, PAYLOAD_TOO_LARGE)               \
    CODE(414, URI_TOO_LONG)                    \
    CODE(415, UNSUPPORTED_MEDIA_TYPE)          \
    CODE(416, RANGE_NOT_SATISFIABLE)           \
    CODE(417, EXPECTATION_FAILED)              \
    CODE(418, IM_A_TEAPOT)                     \
    CODE(421, MISDIRECTED_REQUEST)             \
    CODE(422, UNPROCESSABLE_ENTITY)            \
    CODE(423, LOCKED)                          \
    CODE(424, FAILED_DEPENDENCY)               \
    CODE(425, TOO_EARLY)                       \
    CODE(426, UPGRADE_REQUIRED)                \
    CODE(428, PRECONDITION_REQUIRED)           \
    CODE(429, TOO_MANY_REQUESTS)               \
    CODE(431, REQUEST_HEADER_FIELDS_TOO_LARGE) \
    CODE(451, UNAVAILABLE_FOR_LEGAL_REASONS)   \
    CODE(500, INTERNAL_SERVER_ERROR)           \
    CODE(501, NOT_IMPLEMENTED)                 \
    CODE(502, BAD_GATEWAY)                     \
    CODE(503, SERVICE_UNAVAILABLE)             \
    CODE(504, GATEWAY_TIMEOUT)                 \
    CODE(505, HTTP_VERSION_NOT_SUPPORTED)      \
    CODE(506, VARIANT_ALSO_NEGOTIATES)         \
    CODE(507, INSUFFICIENT_STORAGE)            \
    CODE(508, LOOP_DETECTED)                   \
    CODE(510, NOT_EXTENDED)                    \
    CODE(511, NETWORK_AUTHENTICATION_REQUIRED)

enum struct Code : uint16_t {
#define ITER(CODE, NAME) NAME = CODE,
    FOREACH_CODE(ITER)
#undef ITER
};

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

enum struct Method : uint8_t {
#define ITER(NAME) NAME,
    FOREACH_METHOD(ITER)
#undef ITER
};

struct Version {
    uint8_t major;
    uint8_t minor;
};

struct Request {
    Method method;
    Url::Path path;
    Version version;
    Map<Str, Str> headers;

    static Res<Request> parse(Io::SScan &s);
};

} // namespace Http
