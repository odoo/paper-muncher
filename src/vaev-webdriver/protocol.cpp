export module Vaev.Webdriver:protocol;

import Karm.Core;

using namespace Karm;

namespace Vaev::WebDriver {

// MARK: 6. Protocol -----------------------------------------------------------
// https://www.w3.org/TR/webdriver2/#protocol

void _sendSuccess();

void _sendError();

// MARK: Readiness State -------------------------------------------------------
// https://www.w3.org/TR/webdriver2/#dfn-readiness-state

struct ReadinessState {
    bool ready;
    String message = ""s;
};

} // namespace Vaev::WebDriver
