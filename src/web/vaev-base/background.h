#pragma once

#include <karm-gfx/fill.h>
#include <karm-mime/url.h>

#include "color.h"
#include "image.h"
#include "length.h"
#include "percent.h"

namespace Vaev {

enum struct BackgroundAttachment {
    SCROLL,
    FIXED,
    LOCAL,

    _LEN
};

struct BackgroundPosition {
    enum struct HorizontalAnchor {
        LEFT,
        CENTER,
        RIGHT,

        _LEN0
    };

    enum struct VerticalPosition {
        TOP,
        CENTER,
        BOTTOM,

        _LEN1
    };

    HorizontalAnchor horizontalAnchor;
    PercentOr<Length> horizontal;
    VerticalPosition verticalAnchor;
    PercentOr<Length> vertical;

    constexpr BackgroundPosition()
        : horizontalAnchor(HorizontalAnchor::LEFT),
          horizontal(Percent{0}),
          verticalAnchor(VerticalPosition::TOP),
          vertical(Percent{0}) {
    }

    constexpr BackgroundPosition(PercentOr<Length> horizontal, PercentOr<Length> vertical)
        : horizontalAnchor(HorizontalAnchor::LEFT), horizontal(horizontal), verticalAnchor(VerticalPosition::TOP), vertical(vertical) {
    }

    constexpr BackgroundPosition(HorizontalAnchor horizontalAnchor, PercentOr<Length> horizontal, VerticalPosition verticalAnchor, PercentOr<Length> vertical)
        : horizontalAnchor(horizontalAnchor), horizontal(horizontal), verticalAnchor(verticalAnchor), vertical(vertical) {
    }

    void repr(Io::Emit& e) const {
        switch (horizontalAnchor) {
        case HorizontalAnchor::LEFT:
            e("left");
            break;
        case HorizontalAnchor::CENTER:
            e("center");
            break;
        case HorizontalAnchor::RIGHT:
            e("right");
            break;

        default:
            unreachable();
        }

        e(" ");

        switch (verticalAnchor) {
        case VerticalPosition::TOP:
            e("top");
            break;
        case VerticalPosition::CENTER:
            e("center");
            break;
        case VerticalPosition::BOTTOM:
            e("bottom");
            break;

        default:
            unreachable();
        }

        e(" ");
        e("{}", horizontal);
        e(" ");
        e("{}", vertical);
    }
};

struct BackgroundRepeat {
    enum _Val : u8 {
        NO = 0,
        X = 1 << 0,
        Y = 1 << 1,
        REPEAT = X | Y,
    };

    u8 _val;

    constexpr BackgroundRepeat(_Val val = REPEAT)
        : _val(static_cast<u8>(val)) {
    }

    void repr(Io::Emit& e) const {
        if (_val == NO) {
            e("no-repeat");
        } else if (_val == REPEAT) {
            e("repeat");
        } else {
            if (_val & X)
                e("repeat-x");
            if (_val & Y)
                e("repeat-y");
        }
    }
};

struct BackgroundLayer {
    Opt<Image> image;
    BackgroundAttachment attachment;
    BackgroundPosition position;
    BackgroundRepeat repeat;

    void repr(Io::Emit& e) const {
        e("(background");
        e(" image={}", image);
        e(" attachment={}", attachment);
        e(" position={}", position);
        e(" repeat={}", repeat);
        e(")");
    }
};

struct BackgroundProps {
    Color color = TRANSPARENT;
    Vec<BackgroundLayer> layers = {};

    void repr(Io::Emit& e) const {
        e("(background");
        e(" color={}", color);
        e(" layers={}", layers);
        e(")");
    }
};

} // namespace Vaev
