#pragma once

#include "box.h"

namespace Vaev::Layout {

struct Metrics {
    InsetsPx padding{};
    InsetsPx borders{};
    Vec2Px position; //< Position relative to the content box of the containing block
    Vec2Px borderSize;
    InsetsPx margin{};
    RadiiPx radii{};

    void repr(Io::Emit &e) const {
        e("(layout paddings: {} borders: {} position: {} borderSize: {} margin: {} radii: {})",
          padding, borders, position, borderSize, margin, radii);
    }

    RectPx borderBox() const {
        return RectPx{position, borderSize};
    }

    RectPx paddingBox() const {
        return borderBox().shrink(borders);
    }

    RectPx contentBox() const {
        return paddingBox().shrink(padding);
    }

    RectPx marginBox() const {
        return borderBox().grow(margin);
    }
};

struct Frag {
    Cursor<Box> box;
    Metrics metrics;
    Vec<Frag> children;

    Style::Computed const &style() const {
        return *box->style;
    }

    void offset(Vec2Px d) {
        metrics.position = metrics.position + d;
        for (auto &c : children)
            c.offset(d);
    }
};

} // namespace Vaev::Layout
