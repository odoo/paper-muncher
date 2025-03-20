#pragma once

#include "base.h"

namespace Karm::Scene {

struct Stack : public Node {
    Vec<Rc<Node>> _children;

    void add(Rc<Node> child) {
        _children.pushBack(child);
    }

    virtual void prepare() override {
        stableSort(_children, [](auto& a, auto& b) {
            return a->zIndex <=> b->zIndex;
        });

        for (auto& child : _children)
            child->prepare();
    }

    virtual Math::Rectf bound() override {
        Math::Rectf rect;
        for (auto& child : _children)
            rect = rect.mergeWith(child->bound());

        return rect;
    }

    virtual void paint(Gfx::Canvas& g, Math::Rectf r, PaintOptions o) override {
        if (not bound().colide(r))
            return;

        for (auto& child : _children)
            child->paint(g, r, o);
    }

    virtual void repr(Io::Emit& e) const override {
        e("(stack z:{}", zIndex);
        if (_children) {
            e.indentNewline();
            for (auto& child : _children) {
                child->repr(e);
                e.newline();
            }
            e.deindent();
        }
        e(")");
    }
};

} // namespace Karm::Scene
