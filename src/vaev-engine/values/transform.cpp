export module Vaev.Engine:values.transform;

import :values.keywords;

namespace Vaev {

export using TransformBox = Union<
    Keywords::ContentBox,
    Keywords::BorderBox,
    Keywords::FillBox,
    Keywords::StrokeBox,
    Keywords::ViewBox>;

} // namespace Vaev
