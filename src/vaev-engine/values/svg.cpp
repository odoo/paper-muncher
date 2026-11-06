module;

#include <karm/macros>

export module Vaev.Engine:values.svg;

import Karm.Core;

using namespace Karm;

namespace Vaev {

// https://svgwg.org/svg2-draft/coords.html#ViewBoxAttribute
export struct SvgViewBox {
    f64 minX;
    f64 minY;
    f64 width;
    f64 height;

    void repr(Io::Emit& e) const {
        e("(ViewBox minX={} minY={} width={} height={})", minX, minY, width, height);
    }
};

} // namespace Vaev
