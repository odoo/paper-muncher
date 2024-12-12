#pragma once

#include <karm-image/picture.h>
#include <karm-text/prose.h>
#include <vaev-markup/dom.h>
#include <vaev-style/computer.h>

#include "base.h"

namespace Vaev::Layout {

// MARK: Box ------------------------------------------------------------------

struct Box;

using Content = Union<
    None,
    Vec<Box>,
    Strong<Text::Prose>,
    Karm::Image::Picture>;

struct Attrs {
    usize span = 1;
    usize rowSpan = 1;
    usize colSpan = 1;

    void repr(Io::Emit &e) const {
        e("(attrs span: {} rowSpan: {} colSpan: {})", span, rowSpan, colSpan);
    }
};

struct Box : public Meta::NoCopy {
    Strong<Style::Computed> style;
    Strong<Text::Fontface> fontFace;
    Content content = NONE;
    Attrs attrs;

    Box(Strong<Style::Computed> style, Strong<Karm::Text::Fontface> fontFace);

    Box(Strong<Style::Computed> style, Strong<Karm::Text::Fontface> fontFace, Content content);

    Slice<Box> children() const;

    MutSlice<Box> children();

    void add(Box &&box);

    void repr(Io::Emit &e) const;
};

} // namespace Vaev::Layout
