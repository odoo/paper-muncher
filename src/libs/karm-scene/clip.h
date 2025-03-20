#pragma once

#include "base.h"

namespace Karm::Scene {

struct Clip : public Stack {
    Math::Path _path;

    Clip() = delete;

    Clip(Math::Path path)
        : Stack(), _path(path) {}

    void paint(Gfx::Canvas& g, Math::Rectf r, PaintOptions o) override {
        g.push();

        g.beginPath();
        g.path(_path);
        g.clip();

        Stack::paint(g, r, o);

        g.pop();
    }

    void repr(Io::Emit& e) const override {
        e("(clip");
        Stack::repr(e);
        e(")");
    }
};

} // namespace Karm::Scene
