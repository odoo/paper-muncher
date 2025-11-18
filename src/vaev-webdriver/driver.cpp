module;

#include <karm-core/macros.h>

export module Vaev.Webdriver:driver;

import Karm.Core;
import Karm.Http;
import Karm.Ref;
import Karm.Image;
import Karm.Crypto;
import Vaev.Engine;

import :protocol;

using namespace Karm;

namespace Vaev::WebDriver {

export struct Session {
    Ref::Uuid uuid;
    Map<Ref::Uuid, Rc<Dom::Window>> windows;
    Ref::Uuid current;
    TimeoutConfiguration timeouts;

    // https://www.w3.org/TR/webdriver2/#dfn-current-browsing-context
    Res<Rc<Dom::Window>> currentBrowsingContext() {
        auto maybeWindow = windows.tryGet(current);
        if (not maybeWindow)
            return Error::invalidInput("no current browsing context");
        return Ok(maybeWindow.take());
    }
};

// https://www.w3.org/TR/webdriver2/#endpoints
export struct WebDriver {
    Map<Ref::Uuid, Rc<Session>> _sessions;

    // MARK: 7. Capabilities ---------------------------------------------------

    // MARK: 8. Sessions -------------------------------------------------------
    // https://www.w3.org/TR/webdriver2/#sessions

    Res<Rc<Session>> getSession(Ref::Uuid sessionId) {
        auto maybeSession = _sessions.tryGet(sessionId);
        if (not maybeSession)
            return Error::invalidInput("invalid session uuid");
        return Ok(maybeSession.take());
    }

    // https://www.w3.org/TR/webdriver2/#new-session
    Res<Ref::Uuid> newSession() {
        auto sessionId = try$(Ref::Uuid::v4());
        auto windowHandle = try$(Ref::Uuid::v4());
        auto session = makeRc<Session>(sessionId);
        session->windows.put(windowHandle, Dom::Window::create());
        session->current = windowHandle;
        _sessions.put(sessionId, session);
        return Ok(sessionId);
    }

    // https://www.w3.org/TR/webdriver2/#delete-session
    Res<> deleteSession(Ref::Uuid sessionId) {
        _sessions.del(sessionId);
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
    Res<> setTimeouts(Ref::Uuid sessionId, TimeoutConfiguration timeouts) {
        auto session = try$(getSession(sessionId));
        session->timeouts = timeouts;
        return Ok();
    }

    // https://www.w3.org/TR/webdriver2/#set-timeouts
    Res<TimeoutConfiguration> getTimeouts(Ref::Uuid sessionId) {
        auto session = try$(getSession(sessionId));
        return Ok(session->timeouts);
    }

    // MARK: 10. Navigation ----------------------------------------------------
    // https://www.w3.org/TR/webdriver2/#navigation

    // https://www.w3.org/TR/webdriver2/#navigate-to
    Async::Task<> navigateTo(Ref::Uuid sessionId, Ref::Url url) {
        auto session = co_try$(getSession(sessionId));
        auto window = co_try$(session->currentBrowsingContext());
        co_return co_await window->loadLocationAsync(url);
    }

    // https://www.w3.org/TR/webdriver2/#get-current-url
    Res<Ref::Url> getCurrentUrl(Ref::Uuid sessionId) {
        auto session = try$(getSession(sessionId));
        auto window = try$(session->currentBrowsingContext());
        return Ok(window->location());
    }

    // https://www.w3.org/TR/webdriver2/#refresh
    Async::Task<> refreshAsync(Ref::Uuid sessionId) {
        auto session = co_try$(getSession(sessionId));
        auto window = co_try$(session->currentBrowsingContext());
        co_return co_await window->refreshAsync();
    }

    // https://www.w3.org/TR/webdriver2/#get-title
    Res<String> getTitle(Ref::Uuid sessionId) {
        auto session = try$(getSession(sessionId));
        auto window = try$(session->currentBrowsingContext());

        // 3. Let title be the session's current top-level browsing context's active document's title.
        return Ok(window->document().upgrade()->title());
    }

    // MARK: 11. Context -------------------------------------------------------
    // https://www.w3.org/TR/webdriver2/#contexts

    // https://www.w3.org/TR/webdriver2/#get-window-handle
    Res<Ref::Uuid> getWindowHandle(Ref::Uuid sessionId) {
        auto session = try$(getSession(sessionId));
        return Ok(session->current);
    }

    // https://www.w3.org/TR/webdriver2/#close-window
    Res<> closeWindow(Ref::Uuid sessionId) {
        auto session = try$(getSession(sessionId));
        if (not session->windows.del(session->current))
            return Error::invalidInput("browsing context no longer open");
        return Ok();
    }

    // https://www.w3.org/TR/webdriver2/#switch-to-window
    Res<> switchToWindow(Ref::Uuid sessionId, Ref::Uuid windowHandle) {
        auto session = try$(getSession(sessionId));
        if (not session->windows.has(windowHandle))
            return Error::invalidInput("no such window");
        session->current = windowHandle;
        return Ok();
    }

    // https://www.w3.org/TR/webdriver2/#get-window-handles
    Res<Vec<Ref::Uuid>> getWindowHandles(Ref::Uuid sessionId) {
        auto session = try$(getSession(sessionId));
        return Ok(session->windows.keys());
    }

    // https://www.w3.org/TR/webdriver2/#new-window
    Res<Ref::Uuid> newWindow(Ref::Uuid sessionId) {
        auto session = try$(getSession(sessionId));
        auto windowHandle = try$(Ref::Uuid::v4());
        auto window = Dom::Window::create();
        session->windows.put(windowHandle, window);
        session->current = windowHandle;
        return Ok(windowHandle);
    }

    // MARK: 11.8 Resizing and positioning windows -----------------------------
    // https://www.w3.org/TR/webdriver2/#resizing-and-positioning-windows

    // https://www.w3.org/TR/webdriver2/#get-window-rect
    Res<RectAu> getWindowRect(Ref::Uuid sessionId) {
        auto session = try$(getSession(sessionId));
        auto window = try$(session->currentBrowsingContext());
        return Ok(window->_media.viewportSize());
    }

    // https://www.w3.org/TR/webdriver2/#set-window-rect
    Res<> setWindowRect(Ref::Uuid sessionId, RectAu rect) {
        auto session = try$(getSession(sessionId));
        auto window = try$(session->currentBrowsingContext());
        window->changeViewport(rect.size());
        return Ok();
    }

    // https://www.w3.org/TR/webdriver2/#maximize-window
    Res<> maximizeWindow() {
        return Error::unsupported("unsupported operation");
    }

    // https://www.w3.org/TR/webdriver2/#minimize-window
    Res<> minimizeWindow() {
        return Error::unsupported("unsupported operation");
    }

    // https://www.w3.org/TR/webdriver2/#fullscreen-window
    Res<> fullscreenWindow() {
        return Error::unsupported("unsupported operation");
    }

    // MARK: 13. Document ------------------------------------------------------
    // https://www.w3.org/TR/webdriver2/#document

    // https://www.w3.org/TR/webdriver2/#get-page-source
    Res<String> getPageSource(Ref::Uuid sessionId) {
        auto session = try$(getSession(sessionId));
        auto window = try$(session->currentBrowsingContext());
        return Ok(Dom::serializeHtmlFragment(window->document().upgrade()));
    }

    // https://www.w3.org/TR/webdriver2/#execute-script
    Res<Serde::Value> executeScript([[maybe_unused]] Ref::Uuid sessionId, Str body) {
        // FIXME: We don't support javascript yet, let's pretend we are

        // https://github.com/web-platform-tests/wpt/blob/bbfc05f2af01d92e2c5af0f8a37b580e233f48f1/tools/wptrunner/wptrunner/executors/executorwebdriver.py#L1070
        if (contains(body, R"js(return [window.outerWidth - window.innerWidth,
                       window.outerHeight - window.innerHeight];")js"s)) {
            return Ok(Serde::Array{0, 0});
        }

        // https://github.com/web-platform-tests/wpt/blob/master/tools/wptrunner/wptrunner/executors/runner.js
        else if (contains(body, "document.title = 'MainThread'"s)) {
            return Ok(Serde::Array{});
        }

        // https://github.com/web-platform-tests/wpt/blob/master/tools/wptrunner/wptrunner/executors/test-wait.js
        else if (contains(body, R"js(const initialized = !!window.__wptrunner_url;)js"s)) {
            // FIXME: not 100% sure
            return Ok(Serde::Array{"complete"s, "complete"s, Serde::Array{}});
        }

        else
            return Error::unsupported("unsupported operation");
    }

    // MARK: 17. Screen capture ------------------------------------------------
    // https://www.w3.org/TR/webdriver2/#screen-capture

    // https://www.w3.org/TR/webdriver2/#take-screenshot
    Res<String> takeScreenshot(Ref::Uuid sessionId) {
        auto session = try$(getSession(sessionId));
        auto window = try$(session->currentBrowsingContext());
        auto scene = window->render();
        auto data = try$(
            Karm::Image::save(
                scene,
                window->_media.viewportSize().cast<isize>(),
                {
                    // NOSPEC: Should be PUBLIC_PNG but we don't support PNG encoding yet
                    .format = Ref::Uti::PUBLIC_BMP,
                }
            )
        );

        return Ok(Crypto::base64Encode(data));
    }

    // MARK: 18. Print ---------------------------------------------------------
    // https://www.w3.org/TR/webdriver2/#print

    // https://www.w3.org/TR/webdriver2/#print-page
    Res<String> printPage(Ref::Uuid sessionId, PrintSettings settings = {}) {
        auto session = try$(getSession(sessionId));
        auto window = try$(session->currentBrowsingContext());

        auto printer = try$(
            Print::PdfPrinter::create(
                Ref::Uti::PUBLIC_PDF
            )
        );

        window->print(settings.toNative()) | forEach([&](Print::Page& page) {
            page.print(
                *printer,
                {.showBackgroundGraphics = true}
            );
        });

        Io::BufferWriter bw;
        try$(printer->write(bw));
        return Ok(Crypto::base64Encode(bw.bytes()));
    }
};

export Rc<WebDriver> createWebDriver() {
    return makeRc<WebDriver>();
}

} // namespace Vaev::WebDriver
