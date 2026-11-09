export module Vaev.Engine:style.values.transforms;

import Karm.Core;
import Karm.Math;

import :css.values.numeric;
import :css.values.transforms;
import :style.values.numeric;
import :style.values.keywords;

using namespace Karm::Math::Literals;

using namespace Karm;

namespace Vaev::Style {

export struct MatrixTransform {
    Array<f64, 6> values;

    void repr(Io::Emit& e) const {
        e("matrix({}, {}, {}, {}, {}, {})", values[0], values[1], values[2], values[3], values[4], values[5]);
    }
};

export struct TranslateTransform {
    PxPercent x;
    PxPercent y;

    void repr(Io::Emit& e) const {
        e("translate({}, {})", x, y);
    }
};

export struct ScaleTransform {
    f64 x;
    f64 y;

    void repr(Io::Emit& e) const {
        e("scale({}, {})", x, y);
    }
};

export struct RotateTransform {
    Deg value;

    void repr(Io::Emit& e) const {
        e("rotate({})", value);
    }
};

export struct SkewTransform {
    Deg x;
    Deg y;

    void repr(Io::Emit& e) const {
        e("skew({}, {})", x, y);
    }
};

export struct SkewXTransform {
    Deg value;

    void repr(Io::Emit& e) const {
        e("skewX({})", value);
    }
};

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
// Computed value: as specified, but with lengths made absolute.
export TransformFunction toComputed(Css::TransformFunction const& value, LengthContext auto const& ctx) {
    return value.visit(
        [](Css::MatrixTransform const& t) -> TransformFunction {
            return MatrixTransform{{
                toComputed(t.values[0]),
                toComputed(t.values[1]),
                toComputed(t.values[2]),
                toComputed(t.values[3]),
                toComputed(t.values[4]),
                toComputed(t.values[5]),
            }};
        },
        [&](Css::TranslateTransform const& t) -> TransformFunction {
            return TranslateTransform{toComputed(t.x, ctx), toComputed(t.y, ctx)};
        },
        [](Css::ScaleTransform const& t) -> TransformFunction {
            return ScaleTransform{toComputed(t.x), toComputed(t.y)};
        },
        [](Css::RotateTransform const& t) -> TransformFunction {
            return RotateTransform{toComputed(t.value)};
        },
        [](Css::SkewTransform const& t) -> TransformFunction {
            return SkewTransform{toComputed(t.x), toComputed(t.y)};
        },
        [](Css::SkewXTransform const& t) -> TransformFunction {
            return SkewXTransform{toComputed(t.value)};
        },
        [](Css::SkewYTransform const& t) -> TransformFunction {
            return SkewYTransform{toComputed(t.value)};
        }
    );
}

export Css::TransformFunction toSpecified(TransformFunction const& value) {
    auto number = [](f64 n) {
        return Css::Number::fromCanonical(n);
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
            return Css::TranslateTransform{toSpecified(t.x), toSpecified(t.y)};
        },
        [&](ScaleTransform const& t) -> Css::TransformFunction {
            return Css::ScaleTransform{number(t.x), number(t.y)};
        },
        [](RotateTransform const& t) -> Css::TransformFunction {
            return Css::RotateTransform{toSpecified(t.value)};
        },
        [](SkewTransform const& t) -> Css::TransformFunction {
            return Css::SkewTransform{toSpecified(t.x), toSpecified(t.y)};
        },
        [](SkewXTransform const& t) -> Css::TransformFunction {
            return Css::SkewXTransform{toSpecified(t.value)};
        },
        [](SkewYTransform const& t) -> Css::TransformFunction {
            return Css::SkewYTransform{toSpecified(t.value)};
        }
    );
}

export using TransformBox = Css::TransformBox;

} // namespace Vaev::Style
