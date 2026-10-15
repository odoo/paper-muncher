export module Vaev.Engine:values.insets;

import Karm.Core;
import Karm.Math;

import :css;
import :values.base;
import :values.calc;
import :values.keywords;
import :values.percent;
import :values.resolved;

using namespace Karm;

namespace Vaev {

// MARK: Position --------------------------------------------------------------

export struct RunningPosition {
    CustomIdent customIdent;

    void repr(Io::Emit& e) const {
        e("running '{}'", customIdent);
    }
};

// https://www.w3.org/TR/CSS22/visuren.html#propdef-position
export using Position = Union<Keywords::Static, Keywords::Relative, Keywords::Absolute, Keywords::Fixed, Keywords::Sticky, RunningPosition>;

export template <>
struct ValueParser<Position> {
    // https://drafts.csswg.org/css-position-3/#propdef-position
    static Res<Position> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c.skip(Css::Token::ident("static")))
            return Ok(Keywords::STATIC);
        else if (c.skip(Css::Token::ident("relative")))
            return Ok(Keywords::RELATIVE);
        else if (c.skip(Css::Token::ident("absolute")))
            return Ok(Keywords::ABSOLUTE);
        else if (c.skip(Css::Token::ident("fixed")))
            return Ok(Keywords::FIXED);
        else if (c.skip(Css::Token::ident("sticky")))
            return Ok(Keywords::STICKY);
        else if (c->type == Css::Sst::FUNC and c->prefix == Css::Token::function("running(")) {
            Cursor<Css::Sst> cur = c->content;
            auto res = parseValue<CustomIdent>(cur);
            if (not res) {
                return Error::invalidData("ill formed custom-ident in running position");
            }
            c.next();
            return Ok(RunningPosition{res.take()});

        } else
            return Error::invalidData("expected position");
    }
};

export using Margin = Math::Insets<Union<Keywords::Auto, Calc<Length, Percent>>>;

export using Padding = Math::Insets<Calc<Length, Percent>>;

// https://www.w3.org/TR/css-position-3/#propdef-inset
export using BoxInsets = Math::Insets<Union<Keywords::Auto, Calc<Length, Percent>>>;

export template <typename T>
struct ValueParser<Math::Insets<T>> {
    static Res<Math::Insets<T>> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        auto top = parseValue<T>(c);
        if (not top)
            return top.none();

        auto right = parseValue<T>(c);
        if (not right)
            return Ok(Math::Insets<T>{top.take()});

        auto bottom = parseValue<T>(c);
        if (not bottom)
            return Ok(Math::Insets<T>{top.take(), right.take()});

        auto left = parseValue<T>(c);
        if (not left)
            return Ok(Math::Insets<T>{top.take(), right.take(), bottom.take()});

        return Ok(Math::Insets<T>{top.take(), right.take(), bottom.take(), left.take()});
    }
};

export template <typename T>
struct _Computed<Math::Insets<T>> {
    using Type = Math::Insets<Computed<T>>;
};

export template <typename T>
Math::Insets<Computed<T>> compute(Math::Insets<T> const& value, auto const& ctx) {
    return value.map([&](T const& v) {
        return compute(v, ctx);
    });
}

export template <typename T>
struct _Specify<Math::Insets<T>> {
    static Math::Insets<T> from(Math::Insets<Computed<T>> const& value) {
        return value.map([](Computed<T> const& v) {
            return specified<T>(v);
        });
    }
};

export using Gap = Union<
    Keywords::Normal,
    Calc<Length, Percent>>;

export template <template <typename> typename S>
struct _Gaps {
    S<Gap> row = Keywords::NORMAL;
    S<Gap> col = Keywords::NORMAL;

    void repr(Io::Emit& e) const {
        e("(gaps {} {})", row, col);
    }
};

export using Gaps = _Gaps<Specified>;

export template <>
struct _Computed<Gaps> {
    using Type = _Gaps<Computed>;
};

export Computed<Gaps> compute(Gaps const& value, auto const& ctx) {
    return {compute(value.row, ctx), compute(value.col, ctx)};
}

export template <>
struct _Specify<Gaps> {
    static Gaps from(Computed<Gaps> const& value) {
        return {specified<Gap>(value.row), specified<Gap>(value.col)};
    }
};

} // namespace Vaev
