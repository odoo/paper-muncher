#include <karm-gfx/canvas.h>
#include <karm-logger/logger.h>

#include "font.h"
#include "vga.h"

namespace Karm::Text {

Rc<Fontface> Fontface::fallback() {
    return makeRc<VgaFontface>();
}

Font Font::fallback() {
    return {
        .fontface = Fontface::fallback(),
        .fontsize = 8,
    };
}

FontMetrics Font::metrics() const {
    auto m = fontface->metrics();

    m.advance *= fontsize;
    m.ascend *= fontsize;
    m.captop *= fontsize;
    m.descend *= fontsize;
    m.linegap *= fontsize;

    // Spread linegap evenly between lines
    m.linegap += (lineheight - 1) * (m.ascend + m.descend);

    return m;
}

BaselineSet Font::baselineSet() {
    return fontface->baselineSet().scale(fontsize);
}

Glyph Font::glyph(Rune rune) {
    return fontface->glyph(rune);
}

f64 Font::advance(Glyph glyph) {
    return fontface->advance(glyph) * fontsize;
}

f64 Font::kern(Glyph prev, Glyph curr) {
    return fontface->kern(prev, curr) * fontsize;
}

FontMeasure Font::measure(Glyph r) {
    auto m = metrics();
    auto adv = advance(r);

    return {
        .capbound = {adv, m.captop + m.descend},
        .linebound = {adv, m.ascend + m.descend + m.linegap},
        .baseline = {0, m.ascend},
    };
}

f64 Font::fontSize() {
    return fontsize;
}

f64 Font::xHeight() {
    // FIXME: capbound height as it is here is a font metric, not a glyph metric
    return measure(glyph('x')).capbound.height;
}

f64 Font::capHeight() {
    return measure(glyph('H')).capbound.height;
}

f64 Font::zeroAdvance() {
    return advance(glyph('0'));
}

f64 Font::lineHeight() {
    return metrics().lineheight();
}

} // namespace Karm::Text
