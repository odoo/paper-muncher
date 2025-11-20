module;

#include <karm-core/macros.h>

export module Vaev.Webdriver:service;

import Karm.Core;
import Karm.Http;
import Karm.Logger;

import :driver;

using namespace Karm;

namespace Vaev::WebDriver {

export Rc<Http::Handler> createService(Rc<WebDriver> webdriver) {
    auto service = makeRc<Http::Router>();

    service->get("/", [webdriver](Rc<Http::Request>, Rc<Http::Response::Writer> resp) -> Async::Task<> {
        co_trya$(resp->writeStrAsync(R"html(
            <!DOCTYPE html>
            <html>
                <body>
                    <h1>Vaev WebDriver</h1>
                </body>
            </html>
        )html"s));
        co_return Ok();
    });

    // MARK: 8. Sessions -------------------------------------------------------
    // https://www.w3.org/TR/webdriver2/#sessions

    // https://www.w3.org/TR/webdriver2/#new-session
    service->post(
        "/session",
        [webdriver](Rc<Http::Request> req, Rc<Http::Response::Writer> resp) mutable -> Async::Task<> {
            auto data = co_trya$(req->readJsonAsync());

            auto maybeSessionId = webdriver->newSession();
            if (not maybeSessionId)
                co_return co_await _sendErrorAsync(resp, maybeSessionId.none());

            Serde::Object body{
                {"sessionId"s, maybeSessionId.unwrap().unparsed()},
                {"capabilities"s, data.getOr("capabilities"s, NONE)},
            };
            co_trya$(_sendSuccessAsync(resp, std::move(body)));

            co_return Ok();
        }
    );

    // https://www.w3.org/TR/webdriver2/#delete-session
    service->delete_(
        "/session/{sessionId}",
        [webdriver](Rc<Http::Request> req, Rc<Http::Response::Writer> resp) mutable -> Async::Task<> {
            Ref::Uuid sessionId = co_try$(Ref::Uuid::parse(co_try$(req->routeParams.tryGet("sessionId"s))));

            auto result = webdriver->deleteSession(sessionId);

            if (not result)
                co_return co_await _sendErrorAsync(resp, result.none());

            co_return Ok();
        }
    );

    // https://www.w3.org/TR/webdriver2/#status
    service->get(
        "/status",
        [webdriver](Rc<Http::Request>, Rc<Http::Response::Writer> resp) mutable -> Async::Task<> {
            auto maybeStatus = webdriver->status();
            if (not maybeStatus)
                co_return co_await _sendErrorAsync(resp, maybeStatus.none());

            Serde::Object body{
                {"ready"s, maybeStatus.unwrap().ready},
                {"message"s, maybeStatus.unwrap().message},
            };
            co_trya$(_sendSuccessAsync(resp, std::move(body)));

            co_return Ok();
        }
    );

    // MARK: 9. Timeout --------------------------------------------------------
    // https://www.w3.org/TR/webdriver2/#timeouts

    // https://www.w3.org/TR/webdriver2/#get-timeouts
    service->get(
        "/session/{sessionId}/timeouts",
        [webdriver](Rc<Http::Request> req, Rc<Http::Response::Writer> resp) mutable -> Async::Task<> {
            Ref::Uuid sessionId = co_try$(Ref::Uuid::parse(co_try$(req->routeParams.tryGet("sessionId"s))));
            auto maybeTimeouts = webdriver->getTimeouts(sessionId);
            if (not maybeTimeouts)
                co_return co_await _sendErrorAsync(resp, maybeTimeouts.none());

            Serde::Object serialized{
                {"script"s, maybeTimeouts.unwrap().script.toMSecs()},
                {"pageLoad"s, maybeTimeouts.unwrap().pageLoad.toMSecs()},
                {"implicit"s, maybeTimeouts.unwrap().implicit.toMSecs()},
            };
            co_trya$(_sendSuccessAsync(resp, std::move(serialized)));

            co_return Ok();
        }
    );

    // https://www.w3.org/TR/webdriver2/#set-timeouts
    service->post(
        "/session/{sessionId}/timeouts",
        [webdriver](Rc<Http::Request> req, Rc<Http::Response::Writer> resp) mutable -> Async::Task<> {
            Ref::Uuid sessionId = co_try$(Ref::Uuid::parse(co_try$(req->routeParams.tryGet("sessionId"s))));
            Serde::Value timeouts = co_trya$(req->readJsonAsync());
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

            auto result = webdriver->setTimeouts(sessionId, configuration);
            if (not result)
                co_return co_await _sendErrorAsync(resp, result.none());

            co_trya$(_sendSuccessAsync(resp));
            co_return Ok();
        }
    );

    // MARK: 10. Navigation ----------------------------------------------------
    // https://www.w3.org/TR/webdriver2/#navigation

    // https://www.w3.org/TR/webdriver2/#navigate-to
    service->post(
        "/session/{sessionId}/url",
        [webdriver](Rc<Http::Request> req, Rc<Http::Response::Writer> resp) mutable -> Async::Task<> {
            Ref::Uuid sessionId = co_try$(Ref::Uuid::parse(co_try$(req->routeParams.tryGet("sessionId"s))));
            Serde::Value parameters = co_trya$(req->readJsonAsync());

            Ref::Url url;
            if (auto value = parameters.getOr("url", NONE); value.isStr()) {
                url = Ref::Url::parse(value.asStr());
            } else {
                co_return co_await _sendErrorAsync(resp, Error::invalidInput("missing url key"));
            }

            auto result = co_await webdriver->navigateTo(sessionId, url);
            if (not result)
                co_return co_await _sendErrorAsync(resp, result.none());

            co_trya$(_sendSuccessAsync(resp));
            co_return Ok();
        }
    );

    // https://www.w3.org/TR/webdriver2/#get-current-url
    service->get(
        "/session/{sessionId}/url",
        [webdriver](Rc<Http::Request> req, Rc<Http::Response::Writer> resp) mutable -> Async::Task<> {
            Ref::Uuid sessionId = co_try$(Ref::Uuid::parse(co_try$(req->routeParams.tryGet("sessionId"s))));

            auto maybeUrl = webdriver->getCurrentUrl(sessionId);
            if (not maybeUrl)
                co_return co_await _sendErrorAsync(resp, maybeUrl.none());

            co_trya$(_sendSuccessAsync(resp, maybeUrl.unwrap().str()));
            co_return Ok();
        }
    );

    // https://www.w3.org/TR/webdriver2/#refresh
    service->post(
        "/session/{sessionId}/refresh",
        [webdriver](Rc<Http::Request> req, Rc<Http::Response::Writer> resp) mutable -> Async::Task<> {
            Ref::Uuid sessionId = co_try$(Ref::Uuid::parse(co_try$(req->routeParams.tryGet("sessionId"s))));

            auto result = co_await webdriver->refreshAsync(sessionId);
            if (not result)
                co_return co_await _sendErrorAsync(resp, result.none());
            co_trya$(_sendSuccessAsync(resp));

            co_return Ok();
        }
    );

    // https://www.w3.org/TR/webdriver2/#get-title
    service->get(
        "/session/{sessionId}/title",
        [webdriver](Rc<Http::Request> req, Rc<Http::Response::Writer> resp) mutable -> Async::Task<> {
            Ref::Uuid sessionId = co_try$(Ref::Uuid::parse(co_try$(req->routeParams.tryGet("sessionId"s))));

            auto maybeTitle = webdriver->getTitle(sessionId);
            if (not maybeTitle)
                co_return co_await _sendErrorAsync(resp, maybeTitle.none());

            co_trya$(_sendSuccessAsync(resp, maybeTitle.unwrap().str()));
            co_return Ok();
        }
    );

    // MARK: 11. Context -------------------------------------------------------
    // https://www.w3.org/TR/webdriver2/#contexts

    // https://www.w3.org/TR/webdriver2/#get-window-handle
    service->get(
        "/session/{sessionId}/window",
        [webdriver](Rc<Http::Request> req, Rc<Http::Response::Writer> resp) mutable -> Async::Task<> {
            Ref::Uuid sessionId = co_try$(Ref::Uuid::parse(co_try$(req->routeParams.tryGet("sessionId"s))));
            auto maybeWindowHandle = webdriver->getWindowHandle(sessionId);
            if (not maybeWindowHandle)
                co_return co_await _sendErrorAsync(resp, maybeWindowHandle.none());

            co_trya$(_sendSuccessAsync(resp, maybeWindowHandle.unwrap().unparsed()));
            co_return Ok();
        }
    );

    // https://www.w3.org/TR/webdriver2/#close-window
    service->delete_(
        "/session/{sessionId}/window",
        [webdriver](Rc<Http::Request> req, Rc<Http::Response::Writer> resp) mutable -> Async::Task<> {
            Ref::Uuid sessionId = co_try$(Ref::Uuid::parse(co_try$(req->routeParams.tryGet("sessionId"s))));

            auto result = webdriver->closeWindow(sessionId);
            if (not result)
                co_return co_await _sendErrorAsync(resp, result.none());

            co_trya$(_sendSuccessAsync(resp));
            co_return Ok();
        }
    );

    // https://www.w3.org/TR/webdriver2/#switch-to-window
    service->post(
        "/session/{sessionId}/window",
        [webdriver](Rc<Http::Request> req, Rc<Http::Response::Writer> resp) mutable -> Async::Task<> {
            Ref::Uuid sessionId = co_try$(Ref::Uuid::parse(co_try$(req->routeParams.tryGet("sessionId"s))));
            Serde::Value parameters = co_trya$(req->readJsonAsync());
            Ref::Uuid handle;
            if (auto value = parameters.getOr("handle", NONE); value.isStr()) {
                handle = co_try$(Ref::Uuid::parse(value.asStr()));
            } else {
                co_return co_await _sendErrorAsync(resp, Error::invalidInput("missing handle key"));
            }

            auto result = webdriver->switchToWindow(sessionId, handle);
            if (not result)
                co_return co_await _sendErrorAsync(resp, result.none());

            co_trya$(_sendSuccessAsync(resp));
            co_return Ok();
        }
    );

    // https://www.w3.org/TR/webdriver2/#get-window-handles
    service->get(
        "/session/{sessionId}/window/handles",
        [webdriver](Rc<Http::Request> req, Rc<Http::Response::Writer> resp) mutable -> Async::Task<> {
            Ref::Uuid sessionId = co_try$(Ref::Uuid::parse(co_try$(req->routeParams.tryGet("sessionId"s))));

            auto maybeHandles = webdriver->getWindowHandles(sessionId);
            if (not maybeHandles)
                co_return co_await _sendErrorAsync(resp, maybeHandles.none());

            Serde::Array handles;
            for (auto& h : maybeHandles.unwrap())
                handles.pushBack(h.unparsed());
            co_trya$(_sendSuccessAsync(resp, std::move(handles)));
            co_return Ok();
        }
    );

    // https://www.w3.org/TR/webdriver2/#new-window
    service->post(
        "/session/{sessionId}/window/new",
        [webdriver](Rc<Http::Request> req, Rc<Http::Response::Writer> resp) mutable -> Async::Task<> {
            Ref::Uuid sessionId = co_try$(Ref::Uuid::parse(co_try$(req->routeParams.tryGet("sessionId"s))));

            auto maybeWindowHandle = webdriver->newWindow(sessionId);
            if (not maybeWindowHandle)
                co_return co_await _sendErrorAsync(resp, maybeWindowHandle.none());

            Serde::Object result{
                {"handle"s, maybeWindowHandle.take().unparsed()},
                {"type"s, "window"s},
            };
            co_trya$(_sendSuccessAsync(resp, std::move(result)));

            co_return Ok();
        }
    );

    // MARK: 11.8 Resizing and positioning windows -----------------------------
    // https://www.w3.org/TR/webdriver2/#resizing-and-positioning-windows

    // https://www.w3.org/TR/webdriver2/#get-window-rect
    service->get(
        "/session/{sessionId}/window/rect",
        [webdriver](Rc<Http::Request> req, Rc<Http::Response::Writer> resp) mutable -> Async::Task<> {
            Ref::Uuid sessionId = co_try$(Ref::Uuid::parse(co_try$(req->routeParams.tryGet("sessionId"s))));

            auto maybeRect = webdriver->getWindowRect(sessionId);
            if (not maybeRect)
                co_return co_await _sendErrorAsync(resp, maybeRect.none());

            Serde::Object result{
                {"x"s, maybeRect.unwrap().x.cast<f64>()},
                {"y"s, maybeRect.unwrap().y.cast<f64>()},
                {"width"s, maybeRect.unwrap().width.cast<f64>()},
                {"height"s, maybeRect.unwrap().height.cast<f64>()},
            };
            co_trya$(_sendSuccessAsync(resp, std::move(result)));
            co_return Ok();
        }
    );

    // https://www.w3.org/TR/webdriver2/#set-window-rect
    service->post(
        "/session/{sessionId}/window/rect",
        [webdriver](Rc<Http::Request> req, Rc<Http::Response::Writer> resp) mutable -> Async::Task<> {
            Ref::Uuid sessionId = co_try$(Ref::Uuid::parse(co_try$(req->routeParams.tryGet("sessionId"s))));

            Serde::Value parameters = co_trya$(req->readJsonAsync());

            RectAu rect;

            if (auto value = parameters.getOr("width", NONE); value.isInt())
                rect.width = Au(value.asInt());

            if (auto value = parameters.getOr("height", NONE); value.isInt())
                rect.height = Au(value.asInt());

            if (auto value = parameters.getOr("x", NONE); value.isInt())
                rect.x = Au(value.asInt());

            if (auto value = parameters.getOr("y", NONE); value.isInt())
                rect.y = Au(value.asInt());

            auto result = webdriver->setWindowRect(sessionId, rect);
            if (not result)
                co_return co_await _sendErrorAsync(resp, result.none());

            co_trya$(_sendSuccessAsync(resp));

            co_return Ok();
        }
    );

    // MARK: 13. Document ------------------------------------------------------
    // https://www.w3.org/TR/webdriver2/#document

    // https://www.w3.org/TR/webdriver2/#get-page-source
    service->get(
        "/session/{sessionId}/source",
        [webdriver](Rc<Http::Request> req, Rc<Http::Response::Writer> resp) mutable -> Async::Task<> {
            Ref::Uuid sessionId = co_try$(Ref::Uuid::parse(co_try$(req->routeParams.tryGet("sessionId"s))));

            auto maybeSource = webdriver->getPageSource(sessionId);
            if (not maybeSource)
                co_return co_await _sendErrorAsync(resp, maybeSource.none());

            co_trya$(_sendSuccessAsync(resp, maybeSource.take()));
            co_return Ok();
        }
    );

    // https://www.w3.org/TR/webdriver2/#execute-script
    service->post(
        "/session/{sessionId}/execute/sync",
        [webdriver](Rc<Http::Request> req, Rc<Http::Response::Writer> resp) mutable -> Async::Task<> {
            Ref::Uuid sessionId = co_try$(Ref::Uuid::parse(co_try$(req->routeParams.tryGet("sessionId"s))));

            Serde::Value parameters = co_trya$(req->readJsonAsync());
            String script;
            if (auto value = parameters.getOr("script", NONE); value.isStr()) {
                script = value.asStr();
            } else {
                co_return co_await _sendErrorAsync(resp, Error::invalidInput("missing script key"));
            }

            auto scriptResult = webdriver->executeScript(sessionId, script);
            if (not scriptResult)
                co_return co_await _sendErrorAsync(resp, scriptResult.none());

            co_trya$(_sendSuccessAsync(resp, scriptResult.take()));
            co_return Ok();
        }
    );

    // https://www.w3.org/TR/webdriver2/#execute-async-script
    service->post(
        "/session/{sessionId}/execute/async",
        [webdriver](Rc<Http::Request> req, Rc<Http::Response::Writer> resp) mutable -> Async::Task<> {
            Ref::Uuid sessionId = co_try$(Ref::Uuid::parse(co_try$(req->routeParams.tryGet("sessionId"s))));

            Serde::Value parameters = co_trya$(req->readJsonAsync());
            String script;
            if (auto value = parameters.getOr("script", NONE); value.isStr()) {
                script = value.asStr();
            } else {
                co_return co_await _sendErrorAsync(resp, Error::invalidInput("missing script key"));
            }

            auto scriptResult = webdriver->executeScript(sessionId, script);
            if (not scriptResult)
                co_return co_await _sendErrorAsync(resp, scriptResult.none());

            co_trya$(_sendSuccessAsync(resp, scriptResult.take()));
            co_return Ok();
        }
    );

    // MARK: 17. Screen capture ------------------------------------------------
    // https://www.w3.org/TR/webdriver2/#screen-capture

    // https://www.w3.org/TR/webdriver2/#take-screenshot
    service->post(
        "/session/{sessionId}/screenshot",
        [webdriver](Rc<Http::Request> req, Rc<Http::Response::Writer> resp) mutable -> Async::Task<> {
            Ref::Uuid sessionId = co_try$(Ref::Uuid::parse(co_try$(req->routeParams.tryGet("sessionId"s))));

            auto maybeScreenshot = webdriver->takeScreenshot(sessionId);
            if (not maybeScreenshot)
                co_return co_await _sendErrorAsync(resp, maybeScreenshot.none());

            co_trya$(_sendSuccessAsync(resp, maybeScreenshot.take()));
            co_return Ok();
        }
    );

    // MARK: 18. Print ---------------------------------------------------------
    // https://www.w3.org/TR/webdriver2/#print

    // https://www.w3.org/TR/webdriver2/#print-page
    service->post(
        "/session/{sessionId}/print",
        [webdriver](Rc<Http::Request> req, Rc<Http::Response::Writer> resp) mutable -> Async::Task<> {
            Ref::Uuid sessionId = co_try$(Ref::Uuid::parse(co_try$(req->routeParams.tryGet("sessionId"s))));

            Serde::Value parameters = co_trya$(req->readJsonAsync());

            // Let orientation be the result of getting a property with default named "orientation" and with default "portrait" from parameters.
            auto orientation = parameters.getOr("orientation", "portrait"s);
            // If orientation is not a String or does not have one of the values "landscape" or "portrait", return error with error code invalid argument.
            if (not orientation.isStr() or (orientation.asStr() != "landscape"s and orientation.asStr() != "portrait"s)) {
                co_trya$(_sendErrorAsync(resp, Error::invalidInput("invalid argument")));
                co_return Ok();
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

            PrintSettings settings{
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
            };

            auto maybePdf = webdriver->printPage(sessionId, settings);
            if (not maybePdf)
                co_return co_await _sendErrorAsync(resp, maybePdf.none());

            co_trya$(_sendSuccessAsync(resp, maybePdf.take()));
            co_return Ok();
        }
    );

    return service;
}

} // namespace Vaev::WebDriver
