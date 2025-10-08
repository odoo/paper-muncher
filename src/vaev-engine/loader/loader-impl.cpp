module;

#include <karm-core/macros.h>

module Vaev.Engine;

import Karm.Http;
import Karm.Core;
import Karm.Image;
import Karm.Scene;
import Karm.Math;
import Karm.Ref;
import Karm.Logger;

import :dom.window;
import :layout.values;

namespace Vaev::Loader {

Async::Task<Rc<Scene::Node>> _fetchImageContentAsync(Http::Client& client, Ref::Url url) {
    auto resp = co_trya$(client.getAsync(url));
    if (not resp->body)
        co_return Error::notFound("could not load image");

    auto body = resp->body.unwrap();

    auto data = co_trya$(Aio::readAllAsync(*body));
    if (resp->header.contentType().unwrapOr(Ref::sniffBytes(data)) == "image/svg+xml"_mime) {
        auto subClient = makeRc<Http::Client>(client._transport);
        subClient->userAgent = client.userAgent;
        auto window = Dom::Window::create(subClient);

        // FIXME: Properly determine the size of the SVG
        window->changeMedia(Style::Media::forRender({20_au, 20_au}, Resolution::fromDppx(1)));
        co_trya$(window->loadLocationAsync(url));
        window->computeStyle();
        auto root = window->document()->documentElement();
        Layout::Resolver resolver;
        auto width = resolver.resolve(root->specifiedValues()->sizing->width.unwrapOr<CalcValue<PercentOr<Length>>>(Length{300_au}), 300_au);
        auto height = resolver.resolve(root->specifiedValues()->sizing->height.unwrapOr<CalcValue<PercentOr<Length>>>(Length{300_au}), 300_au);
        window->changeMedia(Style::Media::forRender({width, height}, Resolution::fromDppx(1)));
        co_return Ok(window->render());
    } else {
        auto image = Karm::Image::load(data);
        if (not image)
            co_return image.none();
        co_return Ok(makeRc<Scene::Image>(image.unwrap()->bound().cast<f64>(), image.unwrap()));
    }
}

} // namespace Vaev::Loader
