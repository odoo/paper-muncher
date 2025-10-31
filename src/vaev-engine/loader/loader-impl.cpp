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
import Karm.Gfx;
import Karm.Sys;
import Karm.Font;

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
        // https://www.w3.org/TR/SVG2/coords.html#SizingSVGInCSS
        window->changeMedia(Style::Media::forRender({}, Resolution::fromDppx(1)));
        co_trya$(window->loadLocationAsync(url));
        window->computeStyle();
        auto root = window->document()->documentElement();
        Layout::Resolver resolver;

        // NOSPEC: The spec references a “default object size” but does not define explicit values.
        //         Historically, browsers (including Chrome) default to 300x150, as mentioned in older drafts:
        //         https://www.w3.org/TR/2011/WD-css3-images-20110908/#default-object-size
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

void _evalFontfaceRules(Style::Rule const& rule, Vec<Style::FontFace>& fontFaces) {
    rule.visit(Visitor{
        [&](Style::FontFaceRule const& r) {
            auto& fontFace = fontFaces.emplaceBack();
            for (auto const& decl : r.descs)
                decl.apply(fontFace);
        },
        [&](auto const&) {
            // Ignore other rule types
        },
    });
}

Async::Task<Rc<Gfx::Fontface>> _loadFontfaceAsync(Http::Client& client, Ref::Url url) {
    auto resp = co_trya$(client.getAsync(url));
    if (not resp->body)
        co_return Error::notFound("could not load image");
    auto body = resp->body.unwrap();
    auto data = co_trya$(Aio::readAllAsync(*body));

    // FIXME: Make this more streamline and avoid the extra copy once we reworked karm-font.
    auto size = alignUp(data.len(), Sys::pageSize());
    auto mem = co_try$(Sys::mutMmap(NONE, {.size = size}));
    copy(sub(data), mem.mutBytes());
    co_return Font::loadFontface(mem.seal());
}

Async::_Task<Rc<Font::Database>> _loadFontfacesAsync(Http::Client& client, Style::StyleSheetList const& stylesheets) {
    auto fontDatabase = makeRc<Font::Database>();
    if (auto result = fontDatabase->loadSystemFonts(); not result)
        logWarn("Failed to load system fonts: {}", result);

    for (auto const& sheet : stylesheets.styleSheets) {
        Vec<Style::FontFace> fontFaces;
        for (auto const& rule : sheet.rules)
            _evalFontfaceRules(rule, fontFaces);

        for (auto const& ff : fontFaces) {
            if (not ff.valid())
                continue;

            for (auto const& src : ff.sources) {
                if (not src.identifier.is<Ref::Url>()) {
                    auto result = fontDatabase->queryExact(src.identifier.unwrap<FontFamily>().name);
                    if (not result) {
                        logWarn("Failed to assets font {}", src.identifier.unwrap<FontFamily>().name);
                        continue;
                    }
                    break;
                }

                auto fontUrl = src.identifier.unwrap<Ref::Url>();

                auto resolvedUrl = Ref::Url::resolveReference(sheet.href, fontUrl);
                if (not resolvedUrl) {
                    logWarn("Cannot resolve urls when loading fonts: {} {}", ff.family, sheet.href);
                    continue;
                }

                auto fontface = co_await _loadFontfaceAsync(client, resolvedUrl.unwrap());
                if (not fontface) {
                    logWarn("Failed to load font {}", ff.family);
                    continue;
                }

                fontDatabase->add({
                    .url = resolvedUrl.unwrap(),
                    .attrs = ff.attributes(),
                    .face = fontface.unwrap(),
                    .adjust = ff.adjustments(),
                });
            }
        }
    }

    co_return fontDatabase;
}

} // namespace Vaev::Loader
