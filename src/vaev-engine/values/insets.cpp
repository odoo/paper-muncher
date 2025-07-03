module;

#include <karm-io/emit.h>
#include <karm-math/insets.h>

export module Vaev.Engine:values.insets;

import :css;
import :values.base;
import :values.width;

namespace Vaev {

// MARK: Position --------------------------------------------------------------

struct RunningPosition {
    String customIdent;

    explicit RunningPosition(Str customIdent) : customIdent(customIdent) {}

    void repr(Io::Emit& e) const {
        e("running '{}'", customIdent);
    }
};

// https://www.w3.org/TR/CSS22/visuren.html#propdef-position
export using Position = Union<Keywords::Static, Keywords::Relative, Keywords::Absolute, Keywords::Fixed, Keywords::Sticky, RunningPosition>;

export bool impliesRemovingFromFlow(Position position) {
    return position == Keywords::ABSOLUTE || position == Keywords::FIXED;
}

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
            auto content = cur.peek().token.data.str();
            c.next();
            return Ok(RunningPosition{content});

        } else
            return Error::invalidData("expected position");
    }
};

export using Margin = Math::Insets<Width>;

export using Padding = Math::Insets<CalcValue<PercentOr<Length>>>;

// https://www.w3.org/TR/CSS22/visuren.html#propdef-top
// https://www.w3.org/TR/CSS22/visuren.html#propdef-right
// https://www.w3.org/TR/CSS22/visuren.html#propdef-bottom
// https://www.w3.org/TR/CSS22/visuren.html#propdef-left
export using Offsets = Math::Insets<Width>;

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

export using Gap = Union<
    Keywords::Normal,
    CalcValue<PercentOr<Length>>>;

export struct Gaps {
    Gap x = Keywords::NORMAL;
    Gap y = Keywords::NORMAL;

    void repr(Io::Emit& e) const {
        e("(gaps {} {})", x, y);
    }
};

} // namespace Vaev
