module;

#include <karm-core/macros.h>

export module Vaev.Webdriver:driver;

import Karm.Core;
import Karm.Http;
import Karm.Ref;
import Vaev.Engine;

import :protocol;

using namespace Karm;

namespace Vaev::WebDriver {

export struct Session {
    Ref::Uuid uuid;
    Rc<Dom::Window> window;
};

export struct WebDriver {
    Map<Ref::Uuid, Rc<Session>> _sessions;

    // MARK: 7. Capabilities ---------------------------------------------------

    // MARK: 8. Sessions -------------------------------------------------------
    // https://www.w3.org/TR/webdriver2/#sessions

    // https://www.w3.org/TR/webdriver2/#new-session
    Res<Ref::Uuid> newSession() {
        auto uuid = try$(Ref::Uuid::v4());
        _sessions.put(
            uuid,
            makeRc<Session>(
                uuid,
                Dom::Window::create(Http::defaultClient())
            )
        );
        return Ok(uuid);
    }

    // https://www.w3.org/TR/webdriver2/#delete-session
    Res<> deleteSession(Ref::Uuid uuid) {
        _sessions.del(uuid);
        return Ok();
    }

    // https://www.w3.org/TR/webdriver2/#status
    Res<ReadinessState> status() {
        return Ok(ReadinessState{
            .ready = true,
        });
    }

    // MARK: 9. Timeout --------------------------------------------------------
    // https://www.w3.org/TR/webdriver2/#timeouts

    // https://www.w3.org/TR/webdriver2/#get-timeouts
    Res<> setTimeout();

    // https://www.w3.org/TR/webdriver2/#set-timeouts
    Res<> getTimeout();

    // MARK: 10. Navigation ----------------------------------------------------
    // https://www.w3.org/TR/webdriver2/#navigation

    // https://www.w3.org/TR/webdriver2/#navigate-to
    Async::Task<> navigateTo(Ref::Uuid sessionId, Ref::Url url) {
        auto session = co_try$(_sessions.tryGet(sessionId));
        co_return co_await session->window->loadLocationAsync(url);
    }

    // https://www.w3.org/TR/webdriver2/#get-current-url
    Res<Ref::Url> getCurrentUrl(Ref::Uuid sessionId) {
        auto session = try$(_sessions.tryGet(sessionId));
        return Ok(session->window->location());
    }

    // https://www.w3.org/TR/webdriver2/#back
    Res<> back();

    // https://www.w3.org/TR/webdriver2/#back
    Res<> forward();

    // https://www.w3.org/TR/webdriver2/#refresh
    Res<> refresh();

    // https://www.w3.org/TR/webdriver2/#get-title
    Res<> getTitle();

    // MARK: 11. Context -------------------------------------------------------
    // https://www.w3.org/TR/webdriver2/#contexts

    Res<> getWindowHandle();

    Res<> closeWindow();

    Res<> switchToWindow();

    Res<> getWindowHandles();

    Res<> newWindow();

    Res<> switchToFrame();

    Res<> switchToParentFrame();

    // MARK: 11.8 Resizing and positioning windows -----------------------------
    // https://www.w3.org/TR/webdriver2/#resizing-and-positioning-windows

    Res<> getWindowRect();

    Res<> setWindowRect();

    Res<> maximizeWindow();

    Res<> minimizeWindow();

    Res<> fullScreenWindow();

    // MARK: 17. Screen capture ------------------------------------------------
    // https://www.w3.org/TR/webdriver2/#screen-capture

    // https://www.w3.org/TR/webdriver2/#take-screenshot
    Res<> takeScreenshot();

    // https://www.w3.org/TR/webdriver2/#take-element-screenshot
    Res<> takeElementScreenshot();

    // MARK: 18. Print ---------------------------------------------------------
    // https://www.w3.org/TR/webdriver2/#print

    Res<> printPage();
};

export Rc<WebDriver> createWebDriver() {
    return makeRc<WebDriver>();
}

} // namespace Vaev::WebDriver
