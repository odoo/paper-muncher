module;

#include <karm/macros>

export module Vaev.Engine:values.transform;

import Karm.Math;
import Karm.Logger;

import :css;
import :values.base;
import :values.angle;
import :values.background;
import :values.calc2;
import :values.length;
import :values.keywords;
import :values.percent;
import :values.primitives;
import :values.unitValue;

using namespace Karm::Math::Literals;

namespace Vaev::Css {

// https://drafts.csswg.org/css-transforms
// TODO: Add support for 3d transforms and perspective.

} // namespace Vaev::Css

namespace Vaev::Css {

// MARK: Transform Box ---------------------------------------------------------
// https://drafts.csswg.org/css-transforms/#transform-box

export using TransformBox = Union<
    Keywords::ContentBox,
    Keywords::BorderBox,
    Keywords::FillBox,
    Keywords::StrokeBox,
    Keywords::ViewBox>;

// https://drafts.csswg.org/css-transforms/#funcdef-transform-rotate
// rotate(), skew(), skewX() and skewY() accept <angle> | <zero>.
Res<Angle> _parseAngleOrZero(Cursor<Sst>& c) {
    if (c.skip(Token::number("0")))
        return Ok(Angle::fromCanonical(0));
    return parseValue<Angle>(c);
}

// MARK: Transform Functions ---------------------------------------------------
// https://drafts.csswg.org/css-transforms/#transform-functions

export struct SpecifiedTransformTypes {
    using LengthPercentageType = LengthPercentage;
    using NumberType = Number;
    using AngleType = Angle;
};

} // namespace Vaev::Css

namespace Vaev::Style {

export struct TransformTypes {
    using LengthPercentageType = PixelPercentage;
    using NumberType = f64;
    using AngleType = Css::Rad;
};

} // namespace Vaev::Style

namespace Vaev::Css {

// https://www.w3.org/TR/css-transforms-1/#funcdef-transform-matrix
export template <typename T>
struct MatrixTransformT {
    Array<typename T::NumberType, 6> values;

    void repr(Io::Emit& e) const {
        e("matrix({}, {}, {}, {}, {}, {})", values[0], values[1], values[2], values[3], values[4], values[5]);
    }
};

export using MatrixTransform = MatrixTransformT<SpecifiedTransformTypes>;

} // namespace Vaev::Css

namespace Vaev::Style {

export using MatrixTransform = Css::MatrixTransformT<Style::TransformTypes>;

} // namespace Vaev::Style

namespace Vaev::Css {

export template <>
struct ValueParser<MatrixTransform> {
    static Res<MatrixTransform> parse(Cursor<Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c->prefix != Token::function("matrix("))
            return Error::invalidData("expected matrix function");

        Cursor<Sst> content = c->content;
        eatWhitespace(content);

        auto value = [&](bool last) -> Res<Number> {
            if (content.ended())
                return Error::invalidData("unexpected end of input");

            auto value = try$(parseValue<Number>(content));
            if (not last)
                skipOmmitableComma(content);
            return Ok(value);
        };

        auto a = try$(value(false));
        auto b = try$(value(false));
        auto c_ = try$(value(false));
        auto d = try$(value(false));
        auto e = try$(value(false));
        auto f = try$(value(true));

        c.next(); // consume the function token
        return Ok(MatrixTransform{{a, b, c_, d, e, f}});
    }
};

// https://www.w3.org/TR/css-transforms-1/#funcdef-transform-translate
export template <typename T>
struct TranslateTransformT {
    typename T::LengthPercentageType x;
    typename T::LengthPercentageType y;

    void repr(Io::Emit& e) const {
        e("translate({}, {})", x, y);
    }
};

export using TranslateTransform = TranslateTransformT<SpecifiedTransformTypes>;

} // namespace Vaev::Css

namespace Vaev::Style {

export using TranslateTransform = Css::TranslateTransformT<Style::TransformTypes>;

} // namespace Vaev::Style

namespace Vaev::Css {

export template <>
struct ValueParser<TranslateTransform> {
    static Res<TranslateTransform> parse(Cursor<Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c->prefix == Token::function("translateX(")) {
            Cursor<Sst> content = c->content;
            eatWhitespace(content);
            auto tx = try$(parseValue<LengthPercentage>(content));
            eatWhitespace(content);
            if (not content.ended()) {
                return Error::invalidData("unexpected content after translateX function");
            }
            c.next(); // consume the function token
            return Ok(TranslateTransform{std::move(tx), LengthPercentage::fromCanonical(0)});
        }

        if (c->prefix == Token::function("translateY(")) {
            Cursor<Sst> content = c->content;
            eatWhitespace(content);
            auto ty = try$(parseValue<LengthPercentage>(content));
            eatWhitespace(content);
            if (not content.ended())
                return Error::invalidData("unexpected content after translateY function");
            c.next(); // consume the function token
            return Ok(TranslateTransform{LengthPercentage::fromCanonical(0), std::move(ty)});
        }

        if (c->prefix != Token::function("translate("))
            return Error::invalidData("expected translate function");

        Cursor<Sst> content = c->content;
        eatWhitespace(content);

        auto tx = try$(parseValue<LengthPercentage>(content));
        skipOmmitableComma(content);

        if (content.ended()) {
            // f <ty> is not provided, ty has zero as a value.
            c.next(); // consume the function token
            return Ok(TranslateTransform{std::move(tx), LengthPercentage::fromCanonical(0)});
        }

        auto ty = try$(parseValue<LengthPercentage>(content));
        eatWhitespace(content);
        if (not content.ended())
            return Error::invalidData("unexpected content after scale function");

        c.next(); // consume the function token
        return Ok(TranslateTransform{std::move(tx), std::move(ty)});
    }
};

// https://www.w3.org/TR/css-transforms-1/#funcdef-transform-scale
export template <typename T>
struct ScaleTransformT {
    typename T::NumberType x;
    typename T::NumberType y;

    void repr(Io::Emit& e) const {
        e("scale({}, {})", x, y);
    }
};

export using ScaleTransform = ScaleTransformT<SpecifiedTransformTypes>;

} // namespace Vaev::Css

namespace Vaev::Style {

export using ScaleTransform = Css::ScaleTransformT<Style::TransformTypes>;

} // namespace Vaev::Style

namespace Vaev::Css {

export template <>
struct ValueParser<ScaleTransform> {
    static Res<ScaleTransform> parse(Cursor<Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c->prefix == Token::function("scaleX(")) {
            Cursor<Sst> content = c->content;
            eatWhitespace(content);
            auto sx = try$(parseValue<Number>(content));
            eatWhitespace(content);
            if (not content.ended()) {
                return Error::invalidData("unexpected content after scaleX function");
            }
            c.next(); // consume the function token
            return Ok(ScaleTransform{std::move(sx), Number::fromCanonical(1)});
        }

        if (c->prefix == Token::function("scaleY(")) {
            Cursor<Sst> content = c->content;
            eatWhitespace(content);
            auto sy = try$(parseValue<Number>(content));
            eatWhitespace(content);
            if (not content.ended())
                return Error::invalidData("unexpected content after scaleY function");
            c.next(); // consume the function token
            return Ok(ScaleTransform{Number::fromCanonical(1), std::move(sy)});
        }

        if (c->prefix != Token::function("scale("))
            return Error::invalidData("expected scale function");

        Cursor<Sst> content = c->content;
        eatWhitespace(content);

        auto x = try$(parseValue<Number>(content));
        skipOmmitableComma(content);

        // If the second parameter is not provided, it takes a value equal to the first.
        if (content.ended()) {
            auto y = x;
            c.next(); // consume the function token
            return Ok(ScaleTransform{std::move(x), std::move(y)});
        }

        auto y = try$(parseValue<Number>(content));
        eatWhitespace(content);
        if (not content.ended())
            return Error::invalidData("unexpected content after scale function");

        c.next(); // consume the function token
        return Ok(ScaleTransform{std::move(x), std::move(y)});
    }
};

// https://www.w3.org/TR/css-transforms-1/#funcdef-transform-rotate
export template <typename T>
struct RotateTransformT {
    typename T::AngleType value;

    void repr(Io::Emit& e) const {
        e("rotate({})", value);
    }
};

export using RotateTransform = RotateTransformT<SpecifiedTransformTypes>;

} // namespace Vaev::Css

namespace Vaev::Style {

export using RotateTransform = Css::RotateTransformT<Style::TransformTypes>;

} // namespace Vaev::Style

namespace Vaev::Css {

export template <>
struct ValueParser<RotateTransform> {
    static Res<RotateTransform> parse(Cursor<Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c->prefix != Token::function("rotate("))
            return Error::invalidData("expected rotate function");

        Cursor<Sst> content = c->content;
        eatWhitespace(content);

        auto value = try$(_parseAngleOrZero(content));
        eatWhitespace(content);

        if (not content.ended())
            return Error::invalidData("unexpected content after rotate function");

        c.next(); // consume the function token
        return Ok(RotateTransform{std::move(value)});
    }
};

// https://www.w3.org/TR/css-transforms-1/#funcdef-transform-skew
export template <typename T>
struct SkewTransformT {
    typename T::AngleType x;
    typename T::AngleType y;

    void repr(Io::Emit& e) const {
        e("skew({}, {})", x, y);
    }
};

export using SkewTransform = SkewTransformT<SpecifiedTransformTypes>;

} // namespace Vaev::Css

namespace Vaev::Style {

export using SkewTransform = Css::SkewTransformT<Style::TransformTypes>;

} // namespace Vaev::Style

namespace Vaev::Css {

export template <>
struct ValueParser<SkewTransform> {
    static Res<SkewTransform> parse(Cursor<Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c->prefix != Token::function("skew("))
            return Error::invalidData("expected skew function");

        Cursor<Sst> content = c->content;
        eatWhitespace(content);

        auto x = try$(_parseAngleOrZero(content));
        skipOmmitableComma(content);

        // If the second parameter is not provided, it has a zero value.
        if (content.ended()) {
            c.next(); // consume the function token
            return Ok(SkewTransform{std::move(x), Angle::fromCanonical(0)});
        }

        auto y = try$(_parseAngleOrZero(content));
        eatWhitespace(content);

        if (not content.ended())
            return Error::invalidData("unexpected content after skew function");

        c.next(); // consume the function token
        return Ok(SkewTransform{std::move(x), std::move(y)});
    }
};

// https://www.w3.org/TR/css-transforms-1/#funcdef-transform-skewx
export template <typename T>
struct SkewXTransformT {
    typename T::AngleType value;

    void repr(Io::Emit& e) const {
        e("skewX({})", value);
    }
};

export using SkewXTransform = SkewXTransformT<SpecifiedTransformTypes>;

} // namespace Vaev::Css

namespace Vaev::Style {

export using SkewXTransform = Css::SkewXTransformT<Style::TransformTypes>;

} // namespace Vaev::Style

namespace Vaev::Css {

export template <>
struct ValueParser<SkewXTransform> {
    static Res<SkewXTransform> parse(Cursor<Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c->prefix != Token::function("skewX("))
            return Error::invalidData("expected skewX function");

        Cursor<Sst> content = c->content;
        eatWhitespace(content);

        auto value = try$(_parseAngleOrZero(content));
        eatWhitespace(content);

        if (not content.ended())
            return Error::invalidData("unexpected content after skewX function");

        c.next(); // consume the function token
        return Ok(SkewXTransform{std::move(value)});
    }
};

// https://www.w3.org/TR/css-transforms-1/#funcdef-transform-skewy
export template <typename T>
struct SkewYTransformT {
    typename T::AngleType value;

    void repr(Io::Emit& e) const {
        e("skewY({})", value);
    }
};

export using SkewYTransform = SkewYTransformT<SpecifiedTransformTypes>;

} // namespace Vaev::Css

namespace Vaev::Style {

export using SkewYTransform = Css::SkewYTransformT<Style::TransformTypes>;

} // namespace Vaev::Style

namespace Vaev::Css {

export template <>
struct ValueParser<SkewYTransform> {
    static Res<SkewYTransform> parse(Cursor<Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c->prefix != Token::function("skewY("))
            return Error::invalidData("expected skewY function");

        Cursor<Sst> content = c->content;
        eatWhitespace(content);

        auto value = try$(_parseAngleOrZero(content));
        eatWhitespace(content);

        if (not content.ended())
            return Error::invalidData("unexpected content after skewY function");

        c.next(); // consume the function token
        return Ok(SkewYTransform{std::move(value)});
    }
};

export template <typename T>
using TransformFunctionT = Union<
    MatrixTransformT<T>,
    TranslateTransformT<T>,
    ScaleTransformT<T>,
    RotateTransformT<T>,
    SkewTransformT<T>,
    SkewXTransformT<T>,
    SkewYTransformT<T>>;

export using TransformFunction = TransformFunctionT<SpecifiedTransformTypes>;

} // namespace Vaev::Css

namespace Vaev::Style {

export using TransformFunction = Css::TransformFunctionT<Style::TransformTypes>;

} // namespace Vaev::Style

namespace Vaev::Css {

// MARK: Transform -------------------------------------------------------------
// https://drafts.csswg.org/css-transforms/#propdef-transform

export using TransformList = Vec<TransformFunction>;

} // namespace Vaev::Css

namespace Vaev::Style {

export using TransformList = Vec<Style::TransformFunction>;

export using Transform = Union<Keywords::None, Style::TransformList>;

// https://drafts.csswg.org/css-transforms/#transform-property
// Computed value: as specified, but with lengths made absolute.
export Style::TransformFunction toComputed(Css::TransformFunction const& value, Css::LengthContext auto const& ctx) {
    return value.visit(
        [](Css::MatrixTransform const& t) -> Style::TransformFunction {
            return Style::MatrixTransform{{
                toComputed(t.values[0]),
                toComputed(t.values[1]),
                toComputed(t.values[2]),
                toComputed(t.values[3]),
                toComputed(t.values[4]),
                toComputed(t.values[5]),
            }};
        },
        [&](Css::TranslateTransform const& t) -> Style::TransformFunction {
            return Style::TranslateTransform{toComputed(t.x, ctx), toComputed(t.y, ctx)};
        },
        [](Css::ScaleTransform const& t) -> Style::TransformFunction {
            return Style::ScaleTransform{toComputed(t.x), toComputed(t.y)};
        },
        [](Css::RotateTransform const& t) -> Style::TransformFunction {
            return Style::RotateTransform{toComputed(t.value)};
        },
        [](Css::SkewTransform const& t) -> Style::TransformFunction {
            return Style::SkewTransform{toComputed(t.x), toComputed(t.y)};
        },
        [](Css::SkewXTransform const& t) -> Style::TransformFunction {
            return Style::SkewXTransform{toComputed(t.value)};
        },
        [](Css::SkewYTransform const& t) -> Style::TransformFunction {
            return Style::SkewYTransform{toComputed(t.value)};
        }
    );
}

export Css::TransformFunction toSpecified(Style::TransformFunction const& value) {
    auto number = [](f64 n) {
        return Css::Number::fromCanonical(n);
    };

    return value.visit(
        [&](Style::MatrixTransform const& t) -> Css::TransformFunction {
            return Css::MatrixTransform{{
                number(t.values[0]),
                number(t.values[1]),
                number(t.values[2]),
                number(t.values[3]),
                number(t.values[4]),
                number(t.values[5]),
            }};
        },
        [](Style::TranslateTransform const& t) -> Css::TransformFunction {
            return Css::TranslateTransform{toSpecified(t.x), toSpecified(t.y)};
        },
        [&](Style::ScaleTransform const& t) -> Css::TransformFunction {
            return Css::ScaleTransform{number(t.x), number(t.y)};
        },
        [](Style::RotateTransform const& t) -> Css::TransformFunction {
            return Css::RotateTransform{toSpecified(t.value)};
        },
        [](Style::SkewTransform const& t) -> Css::TransformFunction {
            return Css::SkewTransform{toSpecified(t.x), toSpecified(t.y)};
        },
        [](Style::SkewXTransform const& t) -> Css::TransformFunction {
            return Css::SkewXTransform{toSpecified(t.value)};
        },
        [](Style::SkewYTransform const& t) -> Css::TransformFunction {
            return Css::SkewYTransform{toSpecified(t.value)};
        }
    );
}

} // namespace Vaev::Style

