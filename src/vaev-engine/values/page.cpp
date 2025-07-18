module;

#include <karm-math/au.h>

export module Vaev.Engine:values.page;

import Karm.Core;
import Karm.Print;

import :css;
import :values.base;
import :values.length;
import :values.keywords;

using namespace Karm;

namespace Vaev {

// MARK: Orientation -----------------------------------------------------------
// https://drafts.csswg.org/mediaqueries/#orientation

export template <>
struct ValueParser<Print::Orientation> {
    static Res<Print::Orientation> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c.skip(Css::Token::ident("portrait")))
            return Ok(Print::Orientation::PORTRAIT);
        else if (c.skip(Css::Token::ident("landscape")))
            return Ok(Print::Orientation::LANDSCAPE);
        else
            return Error::invalidData("expected orientation");
    }
};

// https://www.w3.org/TR/css-page-3/#valdef-page-size-landscape
// https://www.w3.org/TR/css-page-3/#valdef-page-size-portrait
Math::Vec2f resolve(
    Print::Orientation orientation,
    Math::Vec2f size
) {
    if (orientation == Print::Orientation::LANDSCAPE) {
        return {
            max(size.x, size.y),
            min(size.x, size.y)
        };
    } else {
        return {
            min(size.x, size.y),
            max(size.x, size.y)
        };
    }
}

// MARK: Size -------------------------------------------------------------
// https://www.w3.org/TR/css-page-3/#page-size-prop

export template <>
struct ValueParser<Print::PaperStock> {
    static Res<Print::PaperStock> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c.skip(Css::Token::ident("A5")))
            return Ok(Print::A5);
        else if (c.skip(Css::Token::ident("A4")))
            return Ok(Print::A4);
        else if (c.skip(Css::Token::ident("A3")))
            return Ok(Print::A3);
        else if (c.skip(Css::Token::ident("B5")))
            return Ok(Print::B5);
        else if (c.skip(Css::Token::ident("B4")))
            return Ok(Print::B4);
        else if (c.skip(Css::Token::ident("JIS-B5")))
            return Ok(Print::JIS_B5);
        else if (c.skip(Css::Token::ident("JIS-B4")))
            return Ok(Print::JIS_B4);
        else if (c.skip(Css::Token::ident("letter")))
            return Ok(Print::LETTER);
        else if (c.skip(Css::Token::ident("legal")))
            return Ok(Print::LEGAL);
        else if (c.skip(Css::Token::ident("ledger")))
            return Ok(Print::LEDGER);
        else
            return Error::invalidData("expected paper stock");
    }
};

export struct PageStockWithOrientation {
    // https://www.w3.org/TR/css-page-3/#typedef-page-size-page-size
    Opt<Print::PaperStock> stock;

    // https://www.w3.org/TR/css-page-3/#valdef-page-size-landscape
    // https://www.w3.org/TR/css-page-3/#valdef-page-size-portrait
    Opt<Print::Orientation> orientation;

    void repr(Io::Emit& e) const {
        e("({} {})", stock, orientation);
    }
};

export template <>
struct ValueParser<PageStockWithOrientation> {
    static Res<PageStockWithOrientation> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        PageStockWithOrientation res;

        auto maybeStock = parseValue<Print::PaperStock>(c);
        if (maybeStock) {
            res.stock = maybeStock.take();
        }

        auto maybeOrientation = parseValue<Print::Orientation>(c);
        if (maybeOrientation) {
            res.orientation = maybeOrientation.take();
        }

        maybeStock = parseValue<Print::PaperStock>(c);
        if (maybeStock) {
            res.stock = maybeStock.take();
        }

        return Ok(res);
    }
};

export using PageSize = Union<
    Pair<Length>,
    Keywords::Auto,
    PageStockWithOrientation>;

Vec2Au resolve(PageSize const& pageSize, Vec2Au const& mediaSize) {
    if (pageSize.is<Keywords::Auto>()) {
        // https://www.w3.org/TR/css-page-3/#valdef-page-size-auto
        // The page box will be set to a size and orientation chosen by the UA.
        return mediaSize;
    } else if (auto length = pageSize.is<Pair<Length>>()) {
        // https://www.w3.org/TR/css-page-3/#valdef-page-size-length
        return {resolveAbsoluteLength(length->v0), resolveAbsoluteLength(length->v1)};
    } else if (auto pageStockWithOrientation = pageSize.is<PageStockWithOrientation>()) {
        if (pageStockWithOrientation->stock and pageStockWithOrientation->orientation) {
            return resolve(
                       *pageStockWithOrientation->orientation,
                       pageStockWithOrientation->stock->size()
            )
                .cast<Au>();
        } else if (pageStockWithOrientation->orientation) {
            // If a <page-size> is not specified, the size of the page sheet is chosen by the UA.
            return resolve(*pageStockWithOrientation->orientation, mediaSize.cast<f64>()).cast<Au>();
        } else if (pageStockWithOrientation->stock) {
            return pageStockWithOrientation->stock->size().cast<Au>();
        }
    }

    unreachable();
}

} // namespace Vaev
