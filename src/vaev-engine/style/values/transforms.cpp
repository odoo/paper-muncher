export module Vaev.Engine:style.values.transforms;

import Karm.Core;
import Karm.Math;

import :css.values.numeric;
import :css.values.transforms;
import :style.values.backgrounds;
import :style.values.numeric;
import :style.values.keywords;
import :style.values.base;

using namespace Karm::Math::Literals;

using namespace Karm;

namespace Vaev::Style {

// https://www.w3.org/TR/css-transforms-1/#funcdef-transform-matrix
export struct MatrixTransform {
    Array<f64, 6> values;

    void repr(Io::Emit& e) const {
        e("matrix({}, {}, {}, {}, {}, {})", values[0], values[1], values[2], values[3], values[4], values[5]);
    }
};

// https://www.w3.org/TR/css-transforms-1/#funcdef-transform-translate
export struct TranslateTransform {
    PxPercent x;
    PxPercent y;

    void repr(Io::Emit& e) const {
        e("translate({}, {})", x, y);
    }
};

// https://www.w3.org/TR/css-transforms-1/#funcdef-transform-scale
export struct ScaleTransform {
    f64 x;
    f64 y;

    void repr(Io::Emit& e) const {
        e("scale({}, {})", x, y);
    }
};

// https://www.w3.org/TR/css-transforms-1/#funcdef-transform-rotate
export struct RotateTransform {
    Deg value;

    void repr(Io::Emit& e) const {
        e("rotate({})", value);
    }
};

// https://www.w3.org/TR/css-transforms-1/#funcdef-transform-skew
export struct SkewTransform {
    Deg x;
    Deg y;

    void repr(Io::Emit& e) const {
        e("skew({}, {})", x, y);
    }
};

// https://www.w3.org/TR/css-transforms-1/#funcdef-transform-skewx
export struct SkewXTransform {
    Deg value;

    void repr(Io::Emit& e) const {
        e("skewX({})", value);
    }
};

// https://www.w3.org/TR/css-transforms-1/#funcdef-transform-skewy
export struct SkewYTransform {
    Deg value;

    void repr(Io::Emit& e) const {
        e("skewY({})", value);
    }
};

export using TransformFunction = Union<
    MatrixTransform,
    TranslateTransform,
    ScaleTransform,
    RotateTransform,
    SkewTransform,
    SkewXTransform,
    SkewYTransform>;

export using TransformList = Vec<TransformFunction>;

export using Transform = Union<Keywords::None, TransformList>;

// https://drafts.csswg.org/css-transforms/#transform-property
export template <>
struct ValueComputer<Css::TransformFunction> {
    static TransformFunction compute(Css::TransformFunction const& value, LengthContext auto const& ctx) {
        return value.visit(
            [](Css::MatrixTransform const& t) -> TransformFunction {
                return MatrixTransform{{
                    computeValue(t.values[0]),
                    computeValue(t.values[1]),
                    computeValue(t.values[2]),
                    computeValue(t.values[3]),
                    computeValue(t.values[4]),
                    computeValue(t.values[5]),
                }};
            },
            [&](Css::TranslateTransform const& t) -> TransformFunction {
                return TranslateTransform{computeValue(t.x, ctx), computeValue(t.y, ctx)};
            },
            [](Css::ScaleTransform const& t) -> TransformFunction {
                return ScaleTransform{computeValue(t.x), computeValue(t.y)};
            },
            [](Css::RotateTransform const& t) -> TransformFunction {
                return RotateTransform{computeValue(t.value)};
            },
            [](Css::SkewTransform const& t) -> TransformFunction {
                return SkewTransform{computeValue(t.x), computeValue(t.y)};
            },
            [](Css::SkewXTransform const& t) -> TransformFunction {
                return SkewXTransform{computeValue(t.value)};
            },
            [](Css::SkewYTransform const& t) -> TransformFunction {
                return SkewYTransform{computeValue(t.value)};
            }
        );
    }

    static Css::TransformFunction specify(TransformFunction const& value) {
        auto number = [](f64 n) {
            return Css::number(n);
        };

        return value.visit(
            [&](MatrixTransform const& t) -> Css::TransformFunction {
                return Css::MatrixTransform{{
                    number(t.values[0]),
                    number(t.values[1]),
                    number(t.values[2]),
                    number(t.values[3]),
                    number(t.values[4]),
                    number(t.values[5]),
                }};
            },
            [](TranslateTransform const& t) -> Css::TransformFunction {
                return Css::TranslateTransform{specifyValue<Css::LengthPercentage>(t.x), specifyValue<Css::LengthPercentage>(t.y)};
            },
            [&](ScaleTransform const& t) -> Css::TransformFunction {
                return Css::ScaleTransform{number(t.x), number(t.y)};
            },
            [](RotateTransform const& t) -> Css::TransformFunction {
                return Css::RotateTransform{specifyValue<Css::Angle>(t.value)};
            },
            [](SkewTransform const& t) -> Css::TransformFunction {
                return Css::SkewTransform{specifyValue<Css::Angle>(t.x), specifyValue<Css::Angle>(t.y)};
            },
            [](SkewXTransform const& t) -> Css::TransformFunction {
                return Css::SkewXTransform{specifyValue<Css::Angle>(t.value)};
            },
            [](SkewYTransform const& t) -> Css::TransformFunction {
                return Css::SkewYTransform{specifyValue<Css::Angle>(t.value)};
            }
        );
    }
};

export using TransformBox = Css::TransformBox;

// https://drafts.csswg.org/css-transforms/#transform-origin-property
// Computed value: list, each item a pair of offsets (horizontal and vertical) from the top left
//                 origin each given as a computed <length-percentage> value
export template <>
struct ValueComputer<Css::TransformOrigin> {
    static Position compute(Css::TransformOrigin const& value, LengthContext auto const& ctx) {
        return {
            value.xOffset.visit(
                [](Keywords::Left) -> PxPercent {
                    return Percent{0};
                },
                [](Keywords::Center) -> PxPercent {
                    return Percent{50};
                },
                [](Keywords::Right) -> PxPercent {
                    return Percent{100};
                },
                [&](Css::LengthPercentage const& length) -> PxPercent {
                    return computeValue(length, ctx);
                }
            ),
            value.yOffset.visit(
                [](Keywords::Top) -> PxPercent {
                    return Percent{0};
                },
                [](Keywords::Center) -> PxPercent {
                    return Percent{50};
                },
                [](Keywords::Bottom) -> PxPercent {
                    return Percent{100};
                },
                [&](Css::LengthPercentage const& length) -> PxPercent {
                    return computeValue(length, ctx);
                }
            ),
        };
    }
};

} // namespace Vaev::Style
