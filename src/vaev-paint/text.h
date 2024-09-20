#pragma once

#include <karm-text/run.h>

#include "base.h"

namespace Vaev::Paint {

struct Text : public Node {
    Math::Vec2f baseline;
    Strong<Karm::Text::Run> run;
    Gfx::Fill fill;

    Text(Math::Vec2f baseline, Strong<Karm::Text::Run> run, Gfx::Fill fill)
        : baseline(baseline), run(run), fill(fill) {}

    void paint(Gfx::Canvas &g) override {
        g.push();
        g.fillStyle(fill);
        g.fill(run->_font, *run, baseline);

        // g.beginPath();
        // g.line(Math::Edgef{baseline, baseline + Math::Vec2f{run->_width, 0}});
        // g.stroke(Gfx::Stroke{
        //     .fill = Gfx::PINK,
        //     .width = 1,
        // });
        g.pop();
    }

    void repr(Io::Emit &e) const override {
        e("(text {})", baseline);
    }
};

} // namespace Vaev::Paint
