#include <karm-core/macros.h>
#include <karm-sys/entry.h>

import Karm.Core;
import Karm.Http;
import Karm.Cli;
import Vaev.Engine;

using namespace Karm;

namespace Vaev::Webdriver {

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
    Ref::Uuid uuid;
    Rc<Dom::Window> window;
};

struct Webdriver {
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
        co_return session->window->loadLocationAsync(url);
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

} // namespace Vaev::Webdriver

Async::Task<> entryPointAsync(Sys::Context& ctx) {
    auto portOption = Cli::option<isize>('p', "port"s, "TCP port to listen to."s);
    Cli::Section serverSection = {"Server Options"s, {portOption}};

    Cli::Command cmd{
        "vaev-webdriver"s,
        "Webdriver protocol implementation for vaev."s,
        {serverSection}
    };

    co_trya$(cmd.execAsync(ctx));
    if (not cmd)
        co_return Ok();

    auto webdriver = makeRc<Vaev::Webdriver::Webdriver>();
    auto router = makeRc<Http::Router>();

    router->route("GET /", [webdriver](Rc<Http::Request>, Rc<Http::Response::Writer> resp) -> Async::Task<> {
        co_trya$(resp->writeStrAsync(R"html(
            <!DOCTYPE html>
            <html>
                <body>
                    <h1>It works!</h1>
                    <p>Vaev WebDriver</p>
                </body>
            </html>
        )html"s));
        co_return Ok();
    });

    router->route("POST *.smnx.sh/assets/{path...}", [webdriver](Rc<Http::Request> req, Rc<Http::Response::Writer> resp) -> Async::Task<> {
        co_return Ok();
    });

    router->route("DELETE /session/{id}", [webdriver](Rc<Http::Request> req, Rc<Http::Response::Writer> resp) -> Async::Task<> {
        auto payload = co_trya$(req->readJsonAsync());
        co_trya$(resp->writeJsonAsync(payload));
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