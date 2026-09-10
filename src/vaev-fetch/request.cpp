export module Vaev.Fetch:request;

import Vaev.Idl;
import Vaev.Script;
import Karm.Core;

import :headers;

using namespace Karm;

namespace Vaev::Fetch {

// https://fetch.spec.whatwg.org/#requestmode
export enum struct RequestMode : u8 {
    NAVIGATE,
    SAME_ORIGIN,
    NO_CORS,
    CORS,
    _LEN
};

// https://fetch.spec.whatwg.org/#enumdef-requestpriority
export enum struct RequestPriority : u8 {
    HIGH,
    LOW,
    AUTO,
    _LEN
};

// https://fetch.spec.whatwg.org/#enumdef-requestduplex
export enum struct RequestDuplex : u8 {
    HALF,
    _LEN
};

// https://fetch.spec.whatwg.org/#requestredirect
export enum struct RequestRedirect : u8 {
    FOLLOW,
    ERROR,
    MANUAL,
    _LEN
};

// https://fetch.spec.whatwg.org/#requestcredentials
export enum struct RequestCredentials : u8 {
    OMIT,
    SAME_ORIGIN,
    INCLUDE,
    _LEN
};

// https://fetch.spec.whatwg.org/#requestcache
export enum struct RequestCache : u8 {
    DEFAULT,
    NO_STORE,
    RELOAD,
    NO_CACHE,
    FORCE_CACHE,
    ONLY_IF_CACHED,
    _LEN
};

// https://fetch.spec.whatwg.org/#requestdestination
export enum struct RequestDestination : u8 {
    EMPTY,
    AUDIO,
    AUDIOWORKLET,
    DOCUMENT,
    EMBED,
    FONT,
    FRAME,
    IFRAME,
    IMAGE,
    JSON,
    MANIFEST,
    OBJECT,
    PAINTWORKLET,
    REPORT,
    SCRIPT,
    SHAREDWORKER,
    STYLE,
    TEXT,
    TRACK,
    VIDEO,
    WORKER,
    XSLT,
    _LEN
};

// https://w3c.github.io/webappsec-referrer-policy/#enumdef-referrerpolicy
enum struct ReferrerPolicy : u8 {
    EMPTY,
    NO_REFERRER,
    NO_REFERRER_WHEN_DOWNGRADE,
    SAME_ORIGIN,
    ORIGIN,
    STRICT_ORIGIN,
    ORIGIN_WHEN_CROSS_ORIGIN,
    STRICT_ORIGIN_WHEN_CROSS_ORIGIN,
    UNSAFE_URL,
    _LEN
};

// https://fetch.spec.whatwg.org/#requestinit
export struct RequestInit : u8 {
    String method;
    HeadersInit headers;
    Idl::USVString referrer;
    ReferrerPolicy referrerPolicy;
    RequestMode mode;
    RequestCredentials credentials;
    RequestCache cache;
    RequestRedirect redirect;
    Idl::DOMString integrity;
    Script::Boolean keepalive;
    // TODO: AbortSignal? signal;
    RequestDuplex duplex;
    RequestPriority priority;
    Idl::Any window = Script::null; // can only be set to null
};

// https://fetch.spec.whatwg.org/#request
struct Request {
    Idl::BytesString method;
    Idl::USVString method;
    HeadersInit headers;
    RequestDestination destination;
};

// https://fetch.spec.whatwg.org/#requestinfo
export using RequestInfo = Union<Rc<Request>, Idl::USVString>;

} // namespace Vaev::Fetch