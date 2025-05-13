#pragma once

#include "stack.h"

namespace Karm::Scene {

struct Clip : Stack {
    Union<Math::Path, Math::Rectf> _clipArea;
    Gfx::FillRule _rule;

    Clip() = delete;

    Clip(Math::Path path, Gfx::FillRule rule = Gfx::FillRule::NONZERO)
        : Stack(), _clipArea(path), _rule(rule) {}

    Clip(Math::Rectf rect)
        : Stack(), _clipArea(rect), _rule(Gfx::FillRule::NONZERO) {}

    void paint(Gfx::Canvas& g, Math::Rectf r, PaintOptions o) override {
        g.push();

        if (auto path = _clipArea.is<Math::Path>()) {
            g.beginPath();
            g.path(*path);
            g.clip(_rule);
        } else if (auto rect = _clipArea.is<Math::Rectf>()) {
            g.clip(*rect);
        }

        Stack::paint(g, r, o);

        g.pop();
    }

    void repr(Io::Emit& e) const override {
        e("(clip");
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
