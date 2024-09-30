#pragma once

#include "base.h"

namespace Vaev::Paint {

struct Image : public Node {
    void repr(Io::Emit &e) const override {
        e("(image z:{})", zIndex);
    }
};

} // namespace Vaev::Paint
