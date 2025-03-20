#pragma once

#include <karm-base/union.h>
#include <karm-base/vec.h>
#include <karm-gfx/types.h>
#include <vaev-base/calc.h>
#include <vaev-base/length.h>
#include <vaev-base/percent.h>

#include "keywords.h"

namespace Vaev {

// https://drafts.fxtf.org/css-masking/#typedef-geometry-box

// using Box = Union<Keywords::BorderBox, Keywords::PaddingBox, Keywords::ContentBox>;

using ShapeBox = FlatUnion<Keywords::BorderBox, Keywords::PaddingBox, Keywords::ContentBox, Keywords::MarginBox>;

using GeometryBox = FlatUnion<ShapeBox, Keywords::FillBox, Keywords::StrokeBox, Keywords::ViewBox>;

using FillRule = Union<Keywords::Nonzero, Keywords::Evenodd>;
inline Gfx::FillRule fillRuleToGfx(FillRule rule) {
    return rule.visit(Visitor {
        [](Keywords::Nonzero&) {
            return Gfx::FillRule::NONZERO;
        },
        [](Keywords::Evenodd&) {
            return Gfx::FillRule::EVENODD;
        }
    });
}

struct Circle {};

struct Ellipse {};

struct Inset {};

struct Path {};

struct Polygon {
    Gfx::FillRule fillRule = Gfx::FillRule::NONZERO;
    Vec<Pair<CalcValue<PercentOr<Length>>>> points;

    void repr(Io::Emit& e) const {
        e("(polygon {} {})", fillRule, points);
    }
};

struct Rect {};

struct Xywh {};

using BasicShapeFunction = Union</*Circle, Ellipse, Inset, Path,*/ Polygon/*, Rect, Xywh*/>;

struct BasicShape {
    Opt<BasicShapeFunction> shape;
    GeometryBox referenceBox = Keywords::CONTENT_BOX;

    void repr(Io::Emit& e) const {
        e("(basic-shape {} {})", shape, referenceBox);
    }
};

} // namespace Vaev
