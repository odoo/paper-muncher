export module Vaev.Engine:values.shapes;

import Karm.Core;

import :values.keywords;

using namespace Karm;

namespace Vaev {

// https://drafts.csswg.org/css-masking/#typedef-geometry-box
export using ShapeBox = Union<
    Keywords::BorderBox,
    Keywords::PaddingBox,
    Keywords::ContentBox,
    Keywords::MarginBox>;

export using GeometryBox = Union<
    Keywords::BorderBox,
    Keywords::PaddingBox,
    Keywords::ContentBox,
    Keywords::MarginBox,
    Keywords::FillBox,
    Keywords::StrokeBox,
    Keywords::ViewBox>;

export using FillRule = Union<
    Keywords::Nonzero,
    Keywords::Evenodd>;

} // namespace Vaev
