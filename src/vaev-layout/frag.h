#pragma once

#include <karm-image/picture.h>
#include <karm-text/run.h>
#include <vaev-markup/dom.h>
#include <vaev-style/computer.h>

#include "base.h"

namespace Vaev::Layout {

// MARK: Float Manager ---------------------------------------------------------

struct FloatManager {
    Vec<Frag *> leftFloatElements, rightFloatElements;
    Map<Frag *, RectPx> placedFloat;
    Map<Frag *, usize> floatId;

    void init(Frag &f);

    FloatManager() {
    }

    void remove(Frag &f);

    void insertIfMissing(Frag *f, RectPx marginBox, Vec<Frag *> &v);

    RectPx placeFloat(Tree &t, Frag &c, Input floatChildInput);
};

// MARK: Frag ------------------------------------------------------------------

using Content = Union<
    None,
    Vec<Frag>,
    Karm::Text::Run,
    Image::Picture>;

struct Frag : public Meta::NoCopy {
    Strong<Style::Computed> style;
    Strong<Karm::Text::Fontface> fontFace;
    Content content = NONE;
    Layout layout;

    // TODO: consider refactor this to "HTML attributes" once other attributes need to be considered
    Cow<TableSpan> tableSpan;

    Frag(Strong<Style::Computed> style, Strong<Karm::Text::Fontface> fontFace);

    Frag(Strong<Style::Computed> style, Strong<Karm::Text::Fontface> fontFace, Content content);

    Slice<Frag> children() const;

    MutSlice<Frag> children();

    void add(Frag &&frag);

    void repr(Io::Emit &e) const;
};

struct Tree {
    Frag root;
    Viewport viewport;
    FloatManager floatManager = FloatManager();
};

// MARK: Build -----------------------------------------------------------------

void _buildNode(Style::Computer &c, Markup::Node const &n, Frag &parent);

Frag build(Style::Computer &c, Markup::Document const &doc);

// MARK: Layout ----------------------------------------------------------------

InsetsPx computeMargins(Tree &t, Frag &f, Input input);
InsetsPx computeBorders(Tree &t, Frag &f);

Output layout(Tree &t, Frag &f, Input input);

Px measure(Tree &t, Frag &f, Axis axis, IntrinsicSize intrinsic, Px availableSpace);

void wireframe(Frag &frag, Gfx::Canvas &g);

} // namespace Vaev::Layout
