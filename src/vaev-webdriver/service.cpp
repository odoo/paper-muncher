module;

#include <karm-core/macros.h>

export module Vaev.Webdriver:service;

import Karm.Core;
import Karm.Http;
import Karm.Logger;

import :driver;

using namespace Karm;

namespace Vaev::WebDriver {

export Rc<Http::Service> createService(Rc<WebDriver> webdriver) {
    auto service = makeRc<Http::Router>();

    service->route("GET /", [webdriver](Rc<Http::Request>, Rc<Http::Response::Writer> resp) -> Async::Task<> {
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

    service->route("DELETE /session/{id}", [webdriver](Rc<Http::Request>, Rc<Http::Response::Writer>) -> Async::Task<> {
        co_return Ok();
    });

    service->route("POST /session/{id}/url", [webdriver](Rc<Http::Request>, Rc<Http::Response::Writer>) -> Async::Task<> {
        co_return Ok();
    });

    service->route("GET /session/{id}/screenshot", [webdriver](Rc<Http::Request>, Rc<Http::Response::Writer>) -> Async::Task<> {
        co_return Ok();
    });

    service->route("GET /session/{id}/window", [webdriver](Rc<Http::Request>, Rc<Http::Response::Writer>) -> Async::Task<> {
        co_return Ok();
    });

    service->route("POST /session/{id}/window/rect", [webdriver](Rc<Http::Request>, Rc<Http::Response::Writer>) -> Async::Task<> {
        co_return Ok();
    });

    service->route("POST /session/{id}/timeout", [webdriver](Rc<Http::Request>, Rc<Http::Response::Writer>) -> Async::Task<> {
        co_return Ok();
    });

    service->route("POST /session/{id}/execute/async", [webdriver](Rc<Http::Request>, Rc<Http::Response::Writer>) -> Async::Task<> {
        co_return Ok();
    });

    return service;
}

} // namespace Vaev::WebDriver
