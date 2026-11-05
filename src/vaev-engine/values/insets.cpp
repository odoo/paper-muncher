export module Vaev.Engine:values.insets;

import Karm.Core;
import Karm.Math;

import :css;
import :values.base;
import :values.calc2;
import :values.keywords;
import :values.percent;

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

export using Margins = Math::Insets<Union<Keywords::Auto, LengthPercentage>>;

export using Paddings = Math::Insets<LengthPercentage>;

// https://www.w3.org/TR/css-position-3/#propdef-inset
export using BoxInsets = Math::Insets<Union<Keywords::Auto, LengthPercentage>>;

namespace Style {

export using Margins = Math::Insets<Union<Keywords::Auto, PixelPercentage>>;

export using Paddings = Math::Insets<PixelPercentage>;

export using BoxInsets = Math::Insets<Union<Keywords::Auto, PixelPercentage>>;

} // namespace Style

export Union<Keywords::Auto, PixelPercentage> resolve(Union<Keywords::Auto, LengthPercentage> const& value, LengthContext auto const& ctx) {
    return value.visit([&](auto const& v) -> Union<Keywords::Auto, PixelPercentage> { return resolve(v, ctx); });
}

export Union<Keywords::Auto, LengthPercentage> toSpecified(Union<Keywords::Auto, PixelPercentage> const& value) {
    return value.visit([](auto const& v) -> Union<Keywords::Auto, LengthPercentage> { return toSpecified(v); });
}

export template <typename T>
auto resolve(Math::Insets<T> const& value, LengthContext auto const& ctx) -> Math::Insets<decltype(resolve(value.top, ctx))> {
    return {resolve(value.top, ctx), resolve(value.end, ctx), resolve(value.bottom, ctx), resolve(value.start, ctx)};
}

export template <typename T>
auto toSpecified(Math::Insets<T> const& value) -> Math::Insets<decltype(toSpecified(value.top))> {
    return {toSpecified(value.top), toSpecified(value.end), toSpecified(value.bottom), toSpecified(value.start)};
}

export template <typename T>
struct ValueParser<Math::Insets<T>> {
    static Res<Math::Insets<T>> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        eatWhitespace(c);
        auto top = parseValue<T>(c);
        if (not top)
            return top.none();

        eatWhitespace(c);
        auto right = parseValue<T>(c);
        if (not right)
            return Ok(Math::Insets<T>{top.take()});

        eatWhitespace(c);
        auto bottom = parseValue<T>(c);
        if (not bottom)
            return Ok(Math::Insets<T>{top.take(), right.take()});

        eatWhitespace(c);
        auto left = parseValue<T>(c);
        if (not left)
            return Ok(Math::Insets<T>{top.take(), right.take(), bottom.take()});

        return Ok(Math::Insets<T>{top.take(), right.take(), bottom.take(), left.take()});
    }
};

// https://drafts.csswg.org/css-align-3/#column-row-gap
export using Gap = Union<
    Keywords::Normal,
    LengthPercentage>;

namespace Style {

export using Gap = Union<
    Keywords::Normal,
    PixelPercentage>;

} // namespace Style

export Style::Gap resolve(Gap const& value, LengthContext auto const& ctx) {
    return value.visit([&](auto const& v) -> Style::Gap { return resolve(v, ctx); });
}

export Gap toSpecified(Style::Gap const& value) {
    return value.visit([](auto const& v) -> Gap { return toSpecified(v); });
}

export struct Gaps {
    Gap row = Keywords::NORMAL;
    Gap col = Keywords::NORMAL;

    void repr(Io::Emit& e) const {
        e("(gaps {} {})", row, col);
    }
};

namespace Style {

export struct Gaps {
    Style::Gap row = Keywords::NORMAL;
    Style::Gap col = Keywords::NORMAL;
};

} // namespace Style
} // namespace Vaev
