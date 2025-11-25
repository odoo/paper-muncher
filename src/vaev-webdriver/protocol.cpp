module;

#include <karm-core/macros.h>

export module Vaev.Webdriver:protocol;

import Karm.Core;
import Karm.Math;
import Karm.Print;
import Karm.Http;

using namespace Karm;

namespace Vaev::WebDriver {

// MARK: 6. Protocol -----------------------------------------------------------
// https://www.w3.org/TR/webdriver2/#protocol

Async::Task<> _sendSuccessAsync(Rc<Http::Response::Writer> resp, Serde::Value data = NONE) {
    co_trya$(resp->writeJsonAsync(Serde::Object{
        {"value"s, data},
    }));
    co_return Ok();
}

Async::Task<> _sendErrorAsync(Rc<Http::Response::Writer> resp, Error err, Serde::Value data = {}) {
    co_trya$(resp->writeHeaderAsync(Http::Code::BAD_REQUEST));
    co_trya$(resp->writeJsonAsync(Serde::Object{
        {"error"s, Str{err.msg()}},
        {"data"s, data},
    }));
    co_return Ok();
}

// MARK: Readiness State -------------------------------------------------------
// https://www.w3.org/TR/webdriver2/#dfn-readiness-state

struct ReadinessState {
    bool ready;
    String message = ""s;
};

// MARK: 9. Timeouts -----------------------------------------------------------
// https://www.w3.org/TR/webdriver2/#timeouts

// https://www.w3.org/TR/webdriver2/#dfn-timeouts-configuration
struct TimeoutConfiguration {
    Duration script = Duration::fromMSecs(30000);
    Duration pageLoad = Duration::fromMSecs(300000);
    Duration implicit = Duration::fromMSecs(0);
};

// MARK: 18. Print -------------------------------------------------------------

struct PrintSettings {
    Print::Orientation orientation = Print::Orientation::PORTRAIT;
    f64 scale = 1.0;
    bool background = false;
    bool shrinkToFit = true;
    Math::Vec2f paper = {
        21.59,
        27.94
    };
    Math::Insetsf margins = 1.0;
    Vec<urange> pageRanges{};

    static PrintSettings defaults() {
        return {};
    }

    Print::Settings toNative() const {
        return {
            .paper = {
                .name = "custom"s,
                .width = paper.width * 10 * Print::UNIT,
                .height = paper.height * 10 * Print::UNIT,
            },
            .margins = Math::Insetsf{
                margins.top * 10 * Print::UNIT,
                margins.end * 10 * Print::UNIT,
                margins.bottom * 10 * Print::UNIT,
                margins.start * 10 * Print::UNIT,
            },
            .orientation = orientation,
            .scale = scale,
            .backgroundGraphics = background,
        };
    }
};

} // namespace Vaev::WebDriver
