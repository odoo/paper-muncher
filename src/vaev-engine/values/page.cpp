export module Vaev.Engine:values.page;

import Karm.Core;
import Karm.Print;

import :css;
import :values.base;
import :values.length;
import :values.keywords;

using namespace Karm;

namespace Vaev {

// MARK: Size -------------------------------------------------------------
// https://www.w3.org/TR/css-page-3/#page-size-prop

struct PageStockWithOrientation {
    Opt<Print::PaperStock> stock;
    Opt<Print::Orientation> orientation;

    void repr(Io::Emit& e) const {
        e("({} {})", stock, orientation);
    }
};

export using _PageSize = Union<Pair<Length>, Keywords::Auto, PageStockWithOrientation>;

export struct PageSize : _PageSize {
    using _PageSize::_PageSize;

    Vec2Au toComputed(ComputationContext& ctx) const {
        return visit(Visitor{
            [&](Keywords::Auto) -> Vec2Au {
                return ctx.displayArea;
            },
            [&](Pair<Length> const& dim) -> Vec2Au {
                return {dim.v0.toComputed(ctx), dim.v1.toComputed(ctx)};
            },
            [&](PageStockWithOrientation const& page) -> Vec2Au {
                if (page.stock and page.orientation) {
                    return page.stock->size(*page.orientation).cast<Au>();
                }

                if (page.stock) {
                    return page.stock->size(Print::Orientation::PORTRAIT);
                }

                if (*page.orientation == Print::Orientation::PORTRAIT) {
                    return {
                        min(ctx.displayArea.width, ctx.displayArea.height),
                        max(ctx.displayArea.width, ctx.displayArea.height),
                    };
                } else {
                    return {
                        max(ctx.displayArea.width, ctx.displayArea.height),
                        min(ctx.displayArea.width, ctx.displayArea.height),
                    };
                }
            },
        });
    }

    void repr(Io::Emit& e) const {
        visit([&](auto&& v) {
            e("(page-size {})", v);
        });
    }
};

export template <>
struct ValueParser<PageSize> {
    static Res<PageSize> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (auto kw = parseValue<Keywords::Auto>(c)) {
            return kw;
        }

        if (auto firstLength = parseValue<Length>(c)) {
            if (auto secondLength = parseValue<Length>(c)) {
                return Ok(Pair{firstLength.unwrap(), secondLength.unwrap()});
            }

            return Ok(Pair{firstLength.unwrap(), firstLength.unwrap()});
        }

        return parseValue<PageStockWithOrientation>(c);
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

export struct PageProps {
    PageSize size = Keywords::AUTO;
};

} // namespace Vaev
