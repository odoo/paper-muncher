export module Vaev.Fetch;

import Karm.Core;
import Vaev.Idl;
import Vaev.Script;
import Karm.Http;

import :request;

using namespace Karm;

// https://fetch.spec.whatwg.org
namespace Vaev::Fetch {

// https://fetch.spec.whatwg.org/#fetch-controller
export struct FetchController {
    // https://fetch.spec.whatwg.org/#fetch-controller-state
    enum struct State {
        ONGOING,
        TERMINATED,
        ABORTED,
    };

    State state;
};

// https://fetch.spec.whatwg.org/#process-request-body
export struct FetchOptionalAlgorithms {
    Opt<Func<usize()>> processRequestBodyChunkLength;
    Opt<Func<void()>> processRequestEndOfBody;
    Opt<Func<void()>> processEarlyHintsResponse;
    Opt<Func<void(Http::Response& resp)>> processResponse;
    Opt<Func<void(Http::Response& resp)>> processResponseEndOfBody;
    Opt<Func<void(Http::Response& resp)>> processResponseConsumeBody;
    bool useParallelQueue = false;
};

} // namespace Vaev::Fetch
