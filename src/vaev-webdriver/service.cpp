module;

#include <karm/macros>

export module Vaev.Webdriver:service;

import Karm.Core;
import Karm.Http;
import Karm.Logger;

import :driver;

using namespace Karm;

namespace Vaev::WebDriver {

export Rc<Http::Handler> createService(Rc<WebDriver> webdriver) {
    auto router = makeRc<Http::Router>();

    router->get("/", [](Rc<Http::Request>, Rc<Http::ResponseWriter> resp, Async::CancellationToken ct) -> Async::Task<> {
        co_trya$(resp->writeFileAsync("bundle://vaev-webdriver/index.html"_url, ct));
        co_return Ok();
    });

    // MARK: 8. Sessions -------------------------------------------------------
    // https://www.w3.org/TR/webdriver2/#sessions

    // https://www.w3.org/TR/webdriver2/#new-session
    router->post(
        "/session",
        [webdriver](Rc<Http::Request> req, Rc<Http::ResponseWriter> resp, Async::CancellationToken ct) mutable -> Async::Task<> {
            auto data = co_trya$(req->readJsonAsync(ct));

            auto sessionId = co_trya$(webdriver->newSessionAsync(ct));
            co_trya$(_sendSuccessAsync(
                resp,
                Serde::Object{
                    {"sessionId"s, sessionId.unparsed()},
                    {"capabilities"s, data.getOr("capabilities"s, NONE)},
                },
                ct
            ));

            co_return Ok();
        }
    );

    // https://www.w3.org/TR/webdriver2/#delete-session
    router->delete_(
        "/session/{sessionId}",
        [webdriver](Rc<Http::Request> req, Rc<Http::ResponseWriter> resp, Async::CancellationToken ct) mutable -> Async::Task<> {
            Ref::Uuid sessionId = co_try$(Ref::Uuid::parse(co_try$(req->routeParams.tryGet("sessionId"s))));

            co_try$(webdriver->deleteSession(sessionId));
            co_trya$(_sendSuccessAsync(resp, {}, ct));

            co_return Ok();
        }
    );

    // https://www.w3.org/TR/webdriver2/#status
    router->get(
        "/status",
        [webdriver](Rc<Http::Request>, Rc<Http::ResponseWriter> resp, Async::CancellationToken ct) mutable -> Async::Task<> {
            auto status = co_try$(webdriver->status());

            co_trya$(_sendSuccessAsync(
                resp,
                Serde::Object{
                    {"ready"s, status.ready},
                    {"message"s, status.message},
                },
                ct
            ));

            co_return Ok();
        }
    );

    // MARK: 9. Timeout --------------------------------------------------------
    // https://www.w3.org/TR/webdriver2/#timeouts

    // https://www.w3.org/TR/webdriver2/#get-timeouts
    router->get(
        "/session/{sessionId}/timeouts",
        [webdriver](Rc<Http::Request> req, Rc<Http::ResponseWriter> resp, Async::CancellationToken ct) mutable -> Async::Task<> {
            Ref::Uuid sessionId = co_try$(Ref::Uuid::parse(co_try$(req->routeParams.tryGet("sessionId"s))));
            auto timeouts = co_try$(webdriver->getTimeouts(sessionId));

            co_trya$(_sendSuccessAsync(
                resp,
                Serde::Object{
                    {"script"s, timeouts.script.toMSecs()},
                    {"pageLoad"s, timeouts.pageLoad.toMSecs()},
                    {"implicit"s, timeouts.implicit.toMSecs()},
                },
                ct
            ));

            co_return Ok();
        }
    );

    // https://www.w3.org/TR/webdriver2/#set-timeouts
    router->post(
        "/session/{sessionId}/timeouts",
        [webdriver](Rc<Http::Request> req, Rc<Http::ResponseWriter> resp, Async::CancellationToken ct) mutable -> Async::Task<> {
            Ref::Uuid sessionId = co_try$(Ref::Uuid::parse(co_try$(req->routeParams.tryGet("sessionId"s))));
            Serde::Value timeouts = co_trya$(req->readJsonAsync(ct));
            TimeoutConfiguration configuration{};

            if (auto value = timeouts.getOr("script", NONE); value.isInt()) {
                configuration.script = Duration::fromMSecs(value.asInt());
            }
            if (auto value = timeouts.getOr("pageLoad", NONE); value.isInt()) {
                configuration.pageLoad = Duration::fromMSecs(value.asInt());
            }
            if (auto value = timeouts.getOr("implicit", NONE); value.isInt()) {
                configuration.implicit = Duration::fromMSecs(value.asInt());
            }

            co_try$(webdriver->setTimeouts(sessionId, configuration));
            co_trya$(_sendSuccessAsync(resp, {}, ct));

            co_return Ok();
        }
    );

    // MARK: 10. Navigation ----------------------------------------------------
    // https://www.w3.org/TR/webdriver2/#navigation

    // https://www.w3.org/TR/webdriver2/#navigate-to
    router->post(
        "/session/{sessionId}/url",
        [webdriver](Rc<Http::Request> req, Rc<Http::ResponseWriter> resp, Async::CancellationToken ct) mutable -> Async::Task<> {
            Ref::Uuid sessionId = co_try$(Ref::Uuid::parse(co_try$(req->routeParams.tryGet("sessionId"s))));
            Serde::Value parameters = co_trya$(req->readJsonAsync(ct));

            Ref::Url url;
            if (auto value = parameters.getOr("url", NONE); value.isStr()) {
                url = Ref::Url::parse(value.asStr());
            } else {
                co_return Error::invalidInput("missing url key");
            }

            co_trya$(webdriver->navigateToAsync(sessionId, url, ct));
            co_trya$(_sendSuccessAsync(resp, {}, ct));

            co_return Ok();
        }
    );

    // https://www.w3.org/TR/webdriver2/#get-current-url
    router->get(
        "/session/{sessionId}/url",
        [webdriver](Rc<Http::Request> req, Rc<Http::ResponseWriter> resp, Async::CancellationToken ct) mutable -> Async::Task<> {
            Ref::Uuid sessionId = co_try$(Ref::Uuid::parse(co_try$(req->routeParams.tryGet("sessionId"s))));

            auto url = co_try$(webdriver->getCurrentUrl(sessionId));
            co_trya$(_sendSuccessAsync(resp, url.str(), ct));

            co_return Ok();
        }
    );

    // https://www.w3.org/TR/webdriver2/#refresh
    router->post(
        "/session/{sessionId}/refresh",
        [webdriver](Rc<Http::Request> req, Rc<Http::ResponseWriter> resp, Async::CancellationToken ct) mutable -> Async::Task<> {
            Ref::Uuid sessionId = co_try$(Ref::Uuid::parse(co_try$(req->routeParams.tryGet("sessionId"s))));

            co_trya$(webdriver->refreshAsync(sessionId, ct));
            co_trya$(_sendSuccessAsync(resp, {}, ct));

            co_return Ok();
        }
    );

    // https://www.w3.org/TR/webdriver2/#get-title
    router->get(
        "/session/{sessionId}/title",
        [webdriver](Rc<Http::Request> req, Rc<Http::ResponseWriter> resp, Async::CancellationToken ct) mutable -> Async::Task<> {
            Ref::Uuid sessionId = co_try$(Ref::Uuid::parse(co_try$(req->routeParams.tryGet("sessionId"s))));

            auto title = co_try$(webdriver->getTitle(sessionId));
            co_trya$(_sendSuccessAsync(resp, title, ct));

            co_return Ok();
        }
    );

    // MARK: 11. Context -------------------------------------------------------
    // https://www.w3.org/TR/webdriver2/#contexts

    // https://www.w3.org/TR/webdriver2/#get-window-handle
    router->get(
        "/session/{sessionId}/window",
        [webdriver](Rc<Http::Request> req, Rc<Http::ResponseWriter> resp, Async::CancellationToken ct) mutable -> Async::Task<> {
            Ref::Uuid sessionId = co_try$(Ref::Uuid::parse(co_try$(req->routeParams.tryGet("sessionId"s))));

            auto windowHandle = co_try$(webdriver->getWindowHandle(sessionId));
            co_trya$(_sendSuccessAsync(resp, windowHandle.unparsed(), ct));

            co_return Ok();
        }
    );

    // https://www.w3.org/TR/webdriver2/#close-window
    router->delete_(
        "/session/{sessionId}/window",
        [webdriver](Rc<Http::Request> req, Rc<Http::ResponseWriter> resp, Async::CancellationToken ct) mutable -> Async::Task<> {
            Ref::Uuid sessionId = co_try$(Ref::Uuid::parse(co_try$(req->routeParams.tryGet("sessionId"s))));

            Serde::Array handles =
                iter(co_try$(webdriver->closeWindow(sessionId))) | Select([](Ref::Uuid const& i) -> Serde::Value {
                    return i.unparsed();
                }) |
                Collect<Serde::Array>();

            co_trya$(_sendSuccessAsync(resp, std::move(handles), ct));
            co_return Ok();
        }
    );

    // https://www.w3.org/TR/webdriver2/#switch-to-window
    router->post(
        "/session/{sessionId}/window",
        [webdriver](Rc<Http::Request> req, Rc<Http::ResponseWriter> resp, Async::CancellationToken ct) mutable -> Async::Task<> {
            Ref::Uuid sessionId = co_try$(Ref::Uuid::parse(co_try$(req->routeParams.tryGet("sessionId"s))));
            Serde::Value parameters = co_trya$(req->readJsonAsync(ct));

            Ref::Uuid handle;
            if (auto value = parameters.getOr("handle", NONE); value.isStr()) {
                handle = co_try$(Ref::Uuid::parse(value.asStr()));
            } else {
                co_return Error::invalidInput("missing handle key");
            }

            co_try$(webdriver->switchToWindow(sessionId, handle));

            co_trya$(_sendSuccessAsync(resp, {}, ct));
            co_return Ok();
        }
    );

    // https://www.w3.org/TR/webdriver2/#get-window-handles
    router->get(
        "/session/{sessionId}/window/handles",
        [webdriver](Rc<Http::Request> req, Rc<Http::ResponseWriter> resp, Async::CancellationToken ct) mutable -> Async::Task<> {
            Ref::Uuid sessionId = co_try$(Ref::Uuid::parse(co_try$(req->routeParams.tryGet("sessionId"s))));

            Serde::Array handles =
                iter(co_try$(webdriver->getWindowHandles(sessionId))) |
                Select([](Ref::Uuid const& i) -> Serde::Value {
                    return i.unparsed();
                }) |
                Collect<Serde::Array>();

            co_trya$(_sendSuccessAsync(resp, std::move(handles), ct));
            co_return Ok();
        }
    );

    // https://www.w3.org/TR/webdriver2/#new-window
    router->post(
        "/session/{sessionId}/window/new",
        [webdriver](Rc<Http::Request> req, Rc<Http::ResponseWriter> resp, Async::CancellationToken ct) mutable -> Async::Task<> {
            Ref::Uuid sessionId = co_try$(Ref::Uuid::parse(co_try$(req->routeParams.tryGet("sessionId"s))));

            auto windowHandle = co_trya$(webdriver->newWindowAsync(sessionId, ct));

            co_trya$(_sendSuccessAsync(
                resp,
                Serde::Object{
                    {"handle"s, windowHandle.unparsed()},
                    {"type"s, "window"s},
                },
                ct
            ));

            co_return Ok();
        }
    );

    // MARK: 11.8 Resizing and positioning windows -----------------------------
    // https://www.w3.org/TR/webdriver2/#resizing-and-positioning-windows

    // https://www.w3.org/TR/webdriver2/#get-window-rect
    router->get(
        "/session/{sessionId}/window/rect",
        [webdriver](Rc<Http::Request> req, Rc<Http::ResponseWriter> resp, Async::CancellationToken ct) mutable -> Async::Task<> {
            Ref::Uuid sessionId = co_try$(Ref::Uuid::parse(co_try$(req->routeParams.tryGet("sessionId"s))));

            auto windowRect = co_try$(webdriver->getWindowRect(sessionId));
            co_trya$(_sendSuccessAsync(
                resp,
                Serde::Object{
                    {"x"s, windowRect.x.cast<f64>()},
                    {"y"s, windowRect.y.cast<f64>()},
                    {"width"s, windowRect.width.cast<f64>()},
                    {"height"s, windowRect.height.cast<f64>()},
                },
                ct
            ));

            co_return Ok();
        }
    );

    // https://www.w3.org/TR/webdriver2/#set-window-rect
    router->post(
        "/session/{sessionId}/window/rect",
        [webdriver](Rc<Http::Request> req, Rc<Http::ResponseWriter> resp, Async::CancellationToken ct) mutable -> Async::Task<> {
            Ref::Uuid sessionId = co_try$(Ref::Uuid::parse(co_try$(req->routeParams.tryGet("sessionId"s))));

            Serde::Value parameters = co_trya$(req->readJsonAsync(ct));

            RectAu rect = co_try$(webdriver->getWindowRect(sessionId));

            if (auto value = parameters.getOr("width", NONE); value.isInt())
                rect.width = Au(value.asInt());

            if (auto value = parameters.getOr("height", NONE); value.isInt())
                rect.height = Au(value.asInt());

            if (auto value = parameters.getOr("x", NONE); value.isInt())
                rect.x = Au(value.asInt());

            if (auto value = parameters.getOr("y", NONE); value.isInt())
                rect.y = Au(value.asInt());

            auto windowRect = co_try$(webdriver->setWindowRect(sessionId, rect));
            co_trya$(_sendSuccessAsync(
                resp,
                Serde::Object{
                    {"x"s, windowRect.x.cast<f64>()},
                    {"y"s, windowRect.y.cast<f64>()},
                    {"width"s, windowRect.width.cast<f64>()},
                    {"height"s, windowRect.height.cast<f64>()},
                },
                ct
            ));

            co_return Ok();
        }
    );

    // MARK: 13. Document ------------------------------------------------------
    // https://www.w3.org/TR/webdriver2/#document

    // https://www.w3.org/TR/webdriver2/#get-page-source
    router->get(
        "/session/{sessionId}/source",
        [webdriver](Rc<Http::Request> req, Rc<Http::ResponseWriter> resp, Async::CancellationToken ct) mutable -> Async::Task<> {
            Ref::Uuid sessionId = co_try$(Ref::Uuid::parse(co_try$(req->routeParams.tryGet("sessionId"s))));

            auto source = co_try$(webdriver->getPageSource(sessionId));
            co_trya$(_sendSuccessAsync(resp, source, ct));

            co_return Ok();
        }
    );

    // https://www.w3.org/TR/webdriver2/#execute-script
    router->post(
        "/session/{sessionId}/execute/sync",
        [webdriver](Rc<Http::Request> req, Rc<Http::ResponseWriter> resp, Async::CancellationToken ct) mutable -> Async::Task<> {
            Ref::Uuid sessionId = co_try$(Ref::Uuid::parse(co_try$(req->routeParams.tryGet("sessionId"s))));

            Serde::Value parameters = co_trya$(req->readJsonAsync(ct));
            String script;
            if (auto value = parameters.getOr("script", NONE); value.isStr()) {
                script = value.asStr();
            } else {
                co_return Error::invalidInput("missing script key");
            }

            auto result = co_try$(webdriver->executeScript(sessionId, script));
            co_trya$(_sendSuccessAsync(resp, result, ct));

            co_return Ok();
        }
    );

    // https://www.w3.org/TR/webdriver2/#execute-async-script
    router->post(
        "/session/{sessionId}/execute/async",
        [webdriver](Rc<Http::Request> req, Rc<Http::ResponseWriter> resp, Async::CancellationToken ct) mutable -> Async::Task<> {
            Ref::Uuid sessionId = co_try$(Ref::Uuid::parse(co_try$(req->routeParams.tryGet("sessionId"s))));

            Serde::Value parameters = co_trya$(req->readJsonAsync(ct));
            String script;
            if (auto value = parameters.getOr("script", NONE); value.isStr()) {
                script = value.asStr();
            } else {
                co_return Error::invalidInput("missing script key");
            }

            auto result = co_try$(webdriver->executeScript(sessionId, script));
            co_trya$(_sendSuccessAsync(resp, std::move(result), ct));

            co_return Ok();
        }
    );

    // MARK: 17. Screen capture ------------------------------------------------
    // https://www.w3.org/TR/webdriver2/#screen-capture

    // https://www.w3.org/TR/webdriver2/#take-screenshot
    router->get(
        "/session/{sessionId}/screenshot",
        [webdriver](Rc<Http::Request> req, Rc<Http::ResponseWriter> resp, Async::CancellationToken ct) mutable -> Async::Task<> {
            Ref::Uuid sessionId = co_try$(Ref::Uuid::parse(co_try$(req->routeParams.tryGet("sessionId"s))));

            auto screenshot = co_try$(webdriver->takeScreenshot(sessionId));
            co_trya$(_sendSuccessAsync(resp, std::move(screenshot), ct));

            co_return Ok();
        }
    );

    // MARK: 18. Print ---------------------------------------------------------
    // https://www.w3.org/TR/webdriver2/#print

    // https://www.w3.org/TR/webdriver2/#print-page
    router->post(
        "/session/{sessionId}/print",
        [webdriver](Rc<Http::Request> req, Rc<Http::ResponseWriter> resp, Async::CancellationToken ct) mutable -> Async::Task<> {
            Ref::Uuid sessionId = co_try$(Ref::Uuid::parse(co_try$(req->routeParams.tryGet("sessionId"s))));

            Serde::Value parameters = co_trya$(req->readJsonAsync(ct));

            // Let orientation be the result of getting a property with default named "orientation" and with default "portrait" from parameters.
            auto orientation = parameters.getOr("orientation", "portrait"s);
            // If orientation is not a String or does not have one of the values "landscape" or "portrait", return error with error code invalid argument.
            if (not orientation.isStr() or (orientation.asStr() != "landscape"s and orientation.asStr() != "portrait"s)) {
                co_return Error::invalidInput("invalid argument");
            }

            // Let scale be the result of getting a property with default named "scale" and with default 1 from parameters.
            auto scale = parameters.getOr("scale", 1);

            // If scale is not a Number, or is less than 0.1 or greater than 2 return error with error code invalid argument.
            // TODO

            // Let background be the result of getting a property with default named "background" and with default false from parameters.
            auto background = parameters.getOr("background", false);

            // If background is not a Boolean return error with error code invalid argument.
            // TODO

            // Let page be the result of getting a property with default named "page" and with a default of an empty Object from parameters.
            auto page = parameters.getOr("page", Serde::Object{});

            // Let pageWidth be the result of getting a property with default named "width" and with a default of 21.59 from page.
            auto pageWidth = page.getOr("width", 21.59);

            // Let pageHeight be the result of getting a property with default named "height" and with a default of 27.94 from page.
            auto pageHeight = page.getOr("height", 27.94);

            // If either of pageWidth or pageHeight is not a Number, or is less than (2.54 / 72), return error with error code invalid argument.
            // TODO

            // Let margin be the result of getting a property with default named "margin" and with a default of an empty Object from parameters.
            auto margin = parameters.getOr("margin", Serde::Object{});

            // Let marginTop be the result of getting a property with default named "top" and with a default of 1 from margin.
            auto marginTop = margin.getOr("top", 1);

            // Let marginBottom be the result of getting a property with default named "bottom" and with a default of 1 from margin.
            auto marginBottom = margin.getOr("bottom", 1);

            // Let marginLeft be the result of getting a property with default named "left" and with a default of 1 from margin.
            auto marginLeft = margin.getOr("left", 1);

            // Let marginRight be the result of getting a property with default named "right" and with a default of 1 from margin.
            auto marginRight = margin.getOr("right", 1);

            // If any of marginTop, marginBottom, marginLeft, or marginRight is not a Number, or is less then 0, return error with error code invalid argument.
            // TODO

            // Let shrinkToFit be the result of getting a property with default named "shrinkToFit" and with default true from parameters.
            auto shrinkToFit = parameters.getOr("shrinkToFit", true);

            // If shrinkToFit is not a Boolean return error with error code invalid argument.
            // TODO

            // Let pageRanges be the result of getting a property with default named "pageRanges" from parameters with default of an empty Array.
            // TODO

            // If pageRanges is not an Array return error with error code invalid argument.
            // TODO

            auto pdf = co_try$(webdriver->printPage(
                sessionId,
                {
                    .orientation = orientation.asStr() == "portrait" ? Print::Orientation::PORTRAIT : Print::Orientation::LANDSCAPE,
                    .scale = scale.asFloat(),
                    .background = background.asBool(),
                    .paper = {
                        pageWidth.asFloat(),
                        pageHeight.asFloat(),
                    },
                    .margins = {
                        marginTop.asFloat(),
                        marginRight.asFloat(),
                        marginBottom.asFloat(),
                        marginLeft.asFloat(),
                    },
                }
            ));

            co_trya$(_sendSuccessAsync(resp, std::move(pdf), ct));

            co_return Ok();
        }
    );

    struct ErrorHandler : Http::Handler {
        Rc<Handler> _next;

        ErrorHandler(Rc<Handler> next)
            : _next(next) {}

        Async::Task<> handleAsync(Rc<Http::Request> req, Rc<Http::ResponseWriter> resp, Async::CancellationToken ct) override {
            auto result = co_await _next->handleAsync(req, resp, ct);
            if (not result) {
                logWarn("webdriver error: {}", result);
                co_return co_await _sendErrorAsync(resp, result.none(), {}, ct);
            }
            co_return Ok();
        }
    };

    return makeRc<ErrorHandler>(router);
}

} // namespace Vaev::WebDriver
