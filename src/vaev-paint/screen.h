#pragma once

#include "stack.h"

namespace Vaev::Paint {

struct Screen : public Stack {
    void repr(Io::Emit &e) const override {
        e("(screen");
        if (_children) {
            e.indentNewline();
            for (auto &child : _children) {
                child->repr(e);
                e.newline();
            }
            e.deindent();
        }
        e(")");
    }
};

} // namespace Vaev::Paint
