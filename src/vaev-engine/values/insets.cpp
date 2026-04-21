export module Vaev.Engine:values.insets;

import Karm.Core;
import Karm.Math;

import :css;
import :values.base;
import :values.width;

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
struct ValueTraits<Position> : DefaultValueTraits<Position> {
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

export using Margin = Math::Insets<Width>;

export using Padding = Math::Insets<CalcValue<PercentOr<Length>>>;

// https://www.w3.org/TR/CSS22/visuren.html#propdef-top
// https://www.w3.org/TR/CSS22/visuren.html#propdef-right
// https://www.w3.org/TR/CSS22/visuren.html#propdef-bottom
// https://www.w3.org/TR/CSS22/visuren.html#propdef-left
export using Offsets = Math::Insets<Width>;

export template <typename T>
struct ValueTraits<Math::Insets<T>> {
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

export template <>
struct ValueTraits<Gap> {
    // Computed value: specified keyword, else a computed <length-percentage> value
    using ComputedType = FlatUnion<Keywords::Normal, Computed<CalcValue<PercentOr<Length>>>>;

    static Res<Gap> parse(Cursor<Css::Sst>& c) {
        return parseOneOf<Gap>(c);
    }

    static ComputedType compute(Gap const& gap, ComputationContext const& ctx) {
        return gap.visit([&](auto&& v) -> ComputedType {
            return computeValue(v, ctx);
        });
    }

    // static Gap fromComputed(ComputedType const& computed) {
    //     return computed.visit(Visitor{
    //         [](Computed<CalcValue<PercentOr<Length>>> const& calc) -> Gap {
    //             return valueFromComputed<CalcValue<PercentOr<Length>>>(calc);
    //         },
    //         [](auto&& val) -> Gap {
    //             return val;
    //         }
    //     });
    // }
};

export struct Gaps {
    Gap row = Keywords::NORMAL;
    Gap col = Keywords::NORMAL;

    void repr(Io::Emit& e) const {
        e("(gaps {} {})", row, col);
    }
};

export struct ComputedGaps {
    Computed<Gap> row = Keywords::NORMAL;
    Computed<Gap> col = Keywords::NORMAL;

    void repr(Io::Emit& e) const {
        e("(gaps {} {})", row, col);
    }
};

export template <>
struct ValueTraits<Gaps> {
    using ComputedType = ComputedGaps;

    static ComputedType compute(Gaps const& gaps, ComputationContext const& ctx) {
        return ComputedGaps {
            .row = computeValue(gaps.row, ctx),
            .col = computeValue(gaps.col, ctx)
        };
    }

    // static Gaps fromComputed(ComputedType const& computed) {
    //     return Gaps {
    //         .row = valueFromComputed<Gap>(computed.row),
    //         .col = valueFromComputed<Gap>(computed.col)
    //     };
    // }
};


} // namespace Vaev
