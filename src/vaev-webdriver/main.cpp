#include <karm-sys/entry.h>

import Karm.Core;
import Karm.Http;
import Vaev.Engine;

using namespace Karm;

namespace Vaev::Webdriver {

struct Uuid {
};

// MARK: 6. Protocol -----------------------------------------------------------
// https://www.w3.org/TR/webdriver2/#protocol

void _sendSuccess();

void _sendError();

// MARK: Readiness State -------------------------------------------------------
// https://www.w3.org/TR/webdriver2/#dfn-readiness-state
struct ReadinessState {
    bool ready;
    String message;
};

struct Session {
    Rc<Dom::Window> window;
};

struct Webdriver {
    Map<Uuid, Session> _sessions;

    // MARK: 7. Capabilities ---------------------------------------------------

    // MARK: 8. Sessions -------------------------------------------------------
    // https://www.w3.org/TR/webdriver2/#sessions

    // https://www.w3.org/TR/webdriver2/#new-session
    Res<Uuid> newSession();

    // https://www.w3.org/TR/webdriver2/#delete-session
    Res<> deleteSession();

    // https://www.w3.org/TR/webdriver2/#status
    Res<ReadinessState> status();

    // MARK: 9. Timeout --------------------------------------------------------
    // https://www.w3.org/TR/webdriver2/#timeouts

    // https://www.w3.org/TR/webdriver2/#get-timeouts
    Res<> setTimeout();

    // https://www.w3.org/TR/webdriver2/#set-timeouts
    Res<> getTimeout();

    // MARK: 10. Navigation ----------------------------------------------------
    // https://www.w3.org/TR/webdriver2/#navigation

    // https://www.w3.org/TR/webdriver2/#navigate-to
    Res<> navigateTo();

    // https://www.w3.org/TR/webdriver2/#get-current-url
    Res<> getCurrentUrl();

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

} // namespace Vaev::Webdriver

Async::Task<> entryPointAsync(Sys::Context&) {
    auto webdriver = makeRc<Vaev::Webdriver::Webdriver>();
    auto router = makeRc<Http::Router>();

    router->route("GET /debug", [webdriver](Rc<Http::Request>, Rc<Http::Response::Writer>) -> Async::Task<> {
        co_return Ok();
    });

    router->route("POST /session", [webdriver](Rc<Http::Request>, Rc<Http::Response::Writer>) -> Async::Task<> {
        co_return Ok();
    });

    router->route("DELETE /session/{id}", [webdriver](Rc<Http::Request>, Rc<Http::Response::Writer>) -> Async::Task<> {
        co_return Ok();
    });

    router->route("POST /session/{id}/url", [webdriver](Rc<Http::Request>, Rc<Http::Response::Writer>) -> Async::Task<> {
        co_return Ok();
    });

    router->route("GET /session/{id}/screenshot", [webdriver](Rc<Http::Request>, Rc<Http::Response::Writer>) -> Async::Task<> {
        co_return Ok();
    });

    router->route("GET /session/{id}/window", [webdriver](Rc<Http::Request>, Rc<Http::Response::Writer>) -> Async::Task<> {
        co_return Ok();
    });

    router->route("POST /session/{id}/window/rect", [webdriver](Rc<Http::Request>, Rc<Http::Response::Writer>) -> Async::Task<> {
        co_return Ok();
    });

    router->route("POST /session/{id}/timeout", [webdriver](Rc<Http::Request>, Rc<Http::Response::Writer>) -> Async::Task<> {
        co_return Ok();
    });

    router->route("POST /session/{id}/execute/async", [webdriver](Rc<Http::Request>, Rc<Http::Response::Writer>) -> Async::Task<> {
        co_return Ok();
    });

    router->route("POST /session/{id}/execute/async", [webdriver](Rc<Http::Request>, Rc<Http::Response::Writer>) -> Async::Task<> {
        co_return Ok();
    });

    co_return co_await Http::servAsync(router);
}