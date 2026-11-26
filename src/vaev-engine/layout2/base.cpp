export module Vaev.Engine:layout2.base;

import :layout;

namespace Vaev::Layout2 {

using Au = Layout::Au;
using Vec2Au = Layout::Vec2Au;
using InsetsAu = Layout::InsetsAu;

// https://www.w3.org/TR/css-layout-api-1/#intrinsic-sizes
struct IntrinsicSizes {
    Au minContentSize;
    Au maxContentSize;
};

// https://www.w3.org/TR/css-sizing-3/#contributions
struct IntrinsicContributions {
    Au min;
    Au max;
};

}

