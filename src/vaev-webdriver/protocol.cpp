module;

#include <karm/macros>

export module Vaev.Webdriver:protocol;

import Karm.Core;
import Karm.Math;
import Karm.Print;
import Karm.Http;

using namespace Karm;

namespace Vaev::WebDriver {

// MARK: 6. Protocol -----------------------------------------------------------
// https://www.w3.org/TR/webdriver2/#protocol

Async::Task<> _sendSuccessAsync(Rc<Http::ResponseWriter> resp, Serde::Value data, Async::CancellationToken ct) {
    co_trya$(resp->writeJsonAsync(
        Serde::Object{
            {"value"s, data},
        },
        ct
    ));
    co_return Ok();
}

Async::Task<> _sendErrorAsync(Rc<Http::ResponseWriter> resp, Error err, Serde::Value data, Async::CancellationToken ct) {
    resp->code = Http::Code::BAD_REQUEST;
    co_trya$(resp->writeJsonAsync(
        Serde::Object{
            {
                "value"s,
                Serde::Object{
                    {"error"s, Str{err.msg()}},
                    {"message"s, data},
                    {"stacktrace"s, ""s},
                },
            },
        },
        ct
    ));

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
    Vec2Au paper = {
        21.59_au,
        27.94_au
    };
    InsetsAu margins = 1.0_au;
    Vec<urange> pageRanges{};

    static PrintSettings defaults() {
        return {};
    }

    Print::Settings toNative() const {
        return {
            .size = Vec2Au{
                paper.width * 10_au * Print::UNIT,
                paper.height * 10_au * Print::UNIT,
            },
            .margins = InsetsAu{
                margins.top * 10_au * Print::UNIT,
                margins.end * 10_au * Print::UNIT,
                margins.bottom * 10_au * Print::UNIT,
                margins.start * 10_au * Print::UNIT,
            },
            .scale = scale,
            .backgroundGraphics = background,
        };
    }
};

} // namespace Vaev::WebDriver
