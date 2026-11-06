export module Vaev.Engine:css.values.image;

import Karm.Ref;
import Karm.Gfx;
import Karm.Math;

import :css.parser;
import :css.values.base;
import :css.values.color;
import :css.values.keywords;
import :css.values.length;
import :css.values.primitives;
import :css.values.unitValue;

// https://www.w3.org/TR/css-images-4/

namespace Vaev::Css {

export struct Image;

// MARK: Linear Gradient -------------------------------------------------------
// https://www.w3.org/TR/css-images-4/#linear-gradients

export struct LinearGradient {
    struct Stop {
        Gfx::Color color;
        PurePercentage position;

        void repr(Io::Emit& e) const {
            e("({} {})", color, position);
        }
    };

    PureAngle angle;
    ColorSpace colorSpace = ColorSpace::SRGB;
    Vec<Stop> stops;

    void repr(Io::Emit& e) const {
        e("(linear-gradient {} {} {}", angle, colorSpace, stops);
    }
};

// MARK: Radial Gradient -------------------------------------------------------
// https://www.w3.org/TR/css-images-4/#radial-gradients

export struct RadialGradient {
    enum struct Shape {
        CIRCLE,
        ELLIPSE,

        _LEN,
    };

    Math::Vec2f size;
    Math::Vec2f position;
    ColorSpace colorSpace = ColorSpace::SRGB;
    Vec<LinearGradient::Stop> stops;

    void repr(Io::Emit& e) const {
        e("(radial-gradient {} {} {} {}", size, position, colorSpace, stops);
    }
};

// MARK: Conic Gradient --------------------------------------------------------
// https://www.w3.org/TR/css-images-4/#conic-gradients

export struct ConicGradient {
    struct Stop {
        Gfx::Color color;
        PureAngle angle;

        void repr(Io::Emit& e) const {
            e("({} {})", color, angle);
        }
    };

    PureAngle angle;
    Math::Vec2f position;
    ColorSpace colorSpace = ColorSpace::SRGB;
    Vec<LinearGradient::Stop> stops;

    void repr(Io::Emit& e) const {
        e("(conic-gradient {} {} {} {}", angle, position, colorSpace, stops);
    }
};

// MARK: Cross Fade ------------------------------------------------------------
// https://www.w3.org/TR/css-images-4/#cross-fade-function

export struct CrossFade {
    struct Layer {
        Box<Image> image;
        PurePercentage opacity;

        void repr(Io::Emit& e) const {
            e("({} {})", image, opacity);
        }
    };

    Vec<Layer> layers;

    void repr(Io::Emit& e) const {
        e("(cross-fade {}", layers);
    }
};

// MARK: Stripes ---------------------------------------------------------------

// https://www.w3.org/TR/css-images-4/#typedef-color-stripe
export struct ColorStrip {
    Gfx::Color color;
    Union<PurePercentage, PureFlex> thickness;

    void repr(Io::Emit& e) const {
        e("({} {})", color, thickness);
    }
};

// https://www.w3.org/TR/css-images-4/#stripes
export struct Stripes {
    Vec<ColorStrip> stripes;

    void repr(Io::Emit& e) const {
        e("(stripes {}", stripes);
    }
};

// MARK: Image -----------------------------------------------------------------
// https://www.w3.org/TR/css-images-4/#typedef-image

using _Image = Union<
    Color,
    LinearGradient,
    RadialGradient,
    ConicGradient,
    CrossFade,
    Stripes,
    Ref::Url>;

export struct Image : _Image {
    using _Image::_Image;

    void repr(Io::Emit& e) const {
        visit([&](auto const& i) {
            e("{}", i);
        });
    }
};

export template <>
struct ValueParser<Image> {
    static Res<Image> parse(Cursor<Sst>& c) {
        return parseValue<Ref::Url>(c);
    }
};

} // namespace Vaev::Css
