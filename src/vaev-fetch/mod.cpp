export module Vaev.Fetch;

import Karm.Core;
import Vaev.Idl;
import Vaev.Script;
import Karm.Http;

using namespace Karm;

// https://fetch.spec.whatwg.org
namespace Vaev::Fetch {

// https://fetch.spec.whatwg.org/#fetch-controller
export struct FetchController {
};

export struct FetchAlgorithm {
    implements_rtti;

    enum struct Options : u8 {
        USE_PARALLEL_QUEUE,
        PROCESS_REQUEST_BODY_CHUNK_LENGTH,
        PROCESS_REQUEST_END_OF_BODY,
        PROCESS_EARLY_HINTS_RESPONSE,
        PROCESS_RESPONSE,
        PROCESS_RESPONSE_END_OF_BODY,
        PROCESS_RESPONSE_CONSUME_BODY,
    };

    using enum Options;
    Flags<Options> options;

    virtual usize processRequestBodyChunkLength() {
        notImplemented();
    }

    virtual void processRequestEndOfBody() {
        notImplemented();
    }

    virtual void processEarlyHintsResponse(Http::Response& resp) {
        notImplemented();
    }

    virtual void processResponse(Http::Response& resp) {
        notImplemented();
    }

    virtual void processResponseEndOfBody(Http::Response& resp) {
        notImplemented();
    }

    virtual void processResponseConsumeBody(Http::Response& resp) {
        notImplemented();
    }
};

// https://fetch.spec.whatwg.org/#concept-fetch
Async::Task<> fetch(Rc<Http::Client> client, Rc<FetchOptionalAlgorithms> optionalAlgorithms) {
    // 1. Assert: request’s mode is "navigate" or processEarlyHintsResponse is null.
    // NOTE: Processing of early hints (responses whose status is 103) is only vetted for navigations.
    // TODO

    // 2. Let taskDestination be null.

    // 3. Let crossOriginIsolatedCapability be false.

    // 4. Populate request from client given request.

    // 5. If request’s client is non-null, then:

    // Set taskDestination to request’s client’s global object.

    // Set crossOriginIsolatedCapability to request’s client’s cross-origin isolated capability.

    // If useParallelQueue is true, then set taskDestination to the result of starting a new parallel queue.

    // Let timingInfo be a new fetch timing info whose start time and post-redirect start time are the coarsened shared current time given crossOriginIsolatedCapability, and render-blocking is set to request’s render-blocking.

    // Let fetchParams be a new fetch params whose request is request, timing info is timingInfo, process request body chunk length is processRequestBodyChunkLength, process request end-of-body is processRequestEndOfBody, process early hints response is processEarlyHintsResponse, process response is processResponse, process response consume body is processResponseConsumeBody, process response end-of-body is processResponseEndOfBody, task destination is taskDestination, and cross-origin isolated capability is crossOriginIsolatedCapability.

    // If request’s body is a byte sequence, then set request’s body to request’s body as a body.

    // Run the WebDriver BiDi clone network request body steps with request.

    // If all of the following conditions are true:

    // request’s URL’s scheme is an HTTP(S) scheme

    // request’s mode is "same-origin", "cors", or "no-cors"

    // request’s client is not null, and request’s client’s global object is a Window object

    // request’s method is `GET`

    // request’s unsafe-request flag is not set or request’s header list is empty

    // then:

    // Assert: request’s origin is same origin with request’s client’s origin.

    // Let onPreloadedResponseAvailable be an algorithm that runs the following step given a response response: set fetchParams’s preloaded response candidate to response.

    // Let foundPreloadedResource be the result of invoking consume a preloaded resource for request’s client, given request’s URL, request’s destination, request’s mode, request’s credentials mode, request’s integrity metadata, and onPreloadedResponseAvailable.

    // If foundPreloadedResource is true and fetchParams’s preloaded response candidate is null, then set fetchParams’s preloaded response candidate to "pending".

    // If request’s header list does not contain `Accept`, then:

    // Let value be `*/*`.

    // If request’s initiator is "prefetch", then set value to the document `Accept` header value.

    // Otherwise, the user agent should set value to the first matching statement, if any, switching on request’s destination:

    // "document"
    // "frame"
    // "iframe"
    // the document `Accept` header value
    // "image"
    // `image/png,image/svg+xml,image/*;q=0.8,*/*;q=0.5`
    // "json"
    // `application/json,*/*;q=0.5`
    // "style"
    // `text/css,*/*;q=0.1`
    // "text"
    // `text/plain,*/*;q=0.5`
    // Append (`Accept`, value) to request’s header list.

    // If request’s header list does not contain `Accept-Language` and request’s client is non-null:

    // Let emulatedLanguage be the WebDriver BiDi emulated language for request’s client.

    // If emulatedLanguage is non-null:

    // Let encodedEmulatedLanguage be emulatedLanguage, isomorphic encoded.

    // Append (`Accept-Language`, encodedEmulatedLanguage) to request’s header list.

    // If request’s header list does not contain `Accept-Language`, then user agents should append (`Accept-Language, an appropriate header value) to request’s header list.

    // If request’s internal priority is null, then use request’s priority, initiator, destination, and render-blocking in an implementation-defined manner to set request’s internal priority to an implementation-defined object.

    // The implementation-defined object could encompass stream weight and dependency for HTTP/2, priorities used in Extensible Prioritization Scheme for HTTP for transports where it applies (including HTTP/3), and equivalent information used to prioritize dispatch and processing of HTTP/1 fetches. [RFC9218]

    // If request is a subresource request:

    // Let record be a new fetch record whose request is request and controller is fetchParams’s controller.

    // Append record to request’s client’s fetch group’s fetch records.

    // Run main fetch given fetchParams.

    // Return fetchParams’s controller.
}

// https://fetch.spec.whatwg.org/#dom-global-fetch
void fetchMethod(RequestInit init);

} // namespace Vaev::Fetch