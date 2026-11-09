export module Vaev.Engine:css.values.images;

import Karm.Ref;
import Karm.Gfx;
import Karm.Math;

import :css.parser;
import :css.values.base;
import :css.values.color;
import :css.values.keywords;
import :css.values.primitives;
import :css.values.numeric;

namespace Vaev::Css {

export struct Image;

// https://www.w3.org/TR/css-images-4/#linear-gradients
export struct LinearGradient {
    struct Stop {
        Gfx::Color color;
        Percentage position;

        void repr(Io::Emit& e) const {
            e("({} {})", color, position);
        }
    };

    Angle angle;
    ColorSpace colorSpace = ColorSpace::SRGB;
    Vec<Stop> stops;

    void repr(Io::Emit& e) const {
        e("(linear-gradient {} {} {}", angle, colorSpace, stops);
    }
};

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

// https://www.w3.org/TR/css-images-4/#conic-gradients
export struct ConicGradient {
    struct Stop {
        Gfx::Color color;
        Angle angle;

        void repr(Io::Emit& e) const {
            e("({} {})", color, angle);
        }
    };

    Angle angle;
    Math::Vec2f position;
    ColorSpace colorSpace = ColorSpace::SRGB;
    Vec<LinearGradient::Stop> stops;

    void repr(Io::Emit& e) const {
        e("(conic-gradient {} {} {} {}", angle, position, colorSpace, stops);
    }
};

// https://www.w3.org/TR/css-images-4/#cross-fade-function
export struct CrossFade {
    struct Layer {
        Box<Image> image;
        Percentage opacity;

        void repr(Io::Emit& e) const {
            e("({} {})", image, opacity);
        }
    };

    Vec<Layer> layers;

    void repr(Io::Emit& e) const {
        e("(cross-fade {}", layers);
    }
};

// https://www.w3.org/TR/css-images-4/#typedef-color-stripe
export struct ColorStripe {
    Gfx::Color color;
    Union<Percentage, Flex> thickness;

    void repr(Io::Emit& e) const {
        e("({} {})", color, thickness);
    }
};

// https://www.w3.org/TR/css-images-4/#stripes
export struct Stripes {
    Vec<ColorStripe> stripes;

    void repr(Io::Emit& e) const {
        e("(stripes {}", stripes);
    }
};

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
