export module Vaev.Engine:layout.fragment;

import Karm.Core;
import Karm.Math;

import :values.length;
import :layout.box;

using namespace Karm;

namespace Vaev::Layout {

// MARK: Fragment --------------------------------------------------------------

export struct Metrics {
    InsetsAu padding{};
    InsetsAu borders{};
    Au outlineOffset{};
    Au outlineWidth{};
    Vec2Au position; //< Position relative to the content box of the containing block
    Vec2Au borderSize;
    InsetsAu margin{};
    RadiiAu radii{};

    void repr(Io::Emit& e) const {
        e("(layout paddings: {} borders: {} position: {} borderSize: {} margin: {} radii: {})",
          padding, borders, position, borderSize, margin, radii);
    }

    RectAu borderBox() const {
        return RectAu{position, borderSize};
    }

    RectAu paddingBox() const {
        return borderBox().shrink(borders);
    }

    RectAu contentBox() const {
        return paddingBox().shrink(padding);
    }

    RectAu marginBox() const {
        return borderBox().grow(margin);
    }
};

export struct Frag;
struct SvgRootFrag;

struct SvgFrag {
    virtual ~SvgFrag() = default;
    virtual RectAu objectBoundingBox() = 0;
    virtual RectAu strokeBoundingBox() = 0;
    virtual Style::ComputedValues const& style() = 0;
};

struct SvgShapeFrag : SvgFrag {
    using _SvgShape = Union<RectAu, EllipseAu, Rc<Math::Path>>;
    _SvgShape shape;
    Opt<Box&> box;
    Au strokeWidth;

    SvgShapeFrag(_SvgShape shape, Opt<Box&> box, Au strokeWidth)
        : shape(shape), box(box), strokeWidth(strokeWidth) {}

    RectAu objectBoundingBox() override {
        if (auto rect = shape.is<RectAu>()) {
            return *rect;
        } else if (auto circle = shape.is<EllipseAu>()) {
            return circle->bound().cast<Au>();
        } else if (auto path = shape.is<Rc<Math::Path>>()) {
            return (*path)->bound().cast<Au>();
        } else {
            unreachable();
        }
    }

    RectAu strokeBoundingBox() override {
        return objectBoundingBox().grow(Au{strokeWidth / 2_au});
    }

    Style::ComputedValues const& style() override {
        return *box->style;
    }

    void repr(Io::Emit& e) const {
        e("(ShapeFrag {} {})", shape, strokeWidth);
    }
};

struct SvgGroupFrag : SvgFrag {
    using Element = Union<SvgShapeFrag, SvgRootFrag, Karm::Box<Frag>, SvgGroupFrag>;

    Vec<Element> elements = {};
    RectAu _objectBoundingBox{};
    RectAu _strokeBoundingBox{};

    Opt<Box const&> box;

    SvgGroupFrag(Opt<Box const&> box)
        : box(box) {}

    void computeBoundingBoxes();

    RectAu objectBoundingBox() override {
        return _objectBoundingBox;
    }

    RectAu strokeBoundingBox() override {
        return _strokeBoundingBox;
    }

    Style::ComputedValues const& style() override {
        return *box->style;
    }

    void add(Element&& element);

    void repr(Io::Emit& e) const {
        e("(GroupFrag)");
    }
};

struct SvgRootFrag : SvgGroupFrag {
    // NOTE: SVG viewports have these intrinsic transformations; choosing to store these transforms is more compliant
    // and somewhat rendering-friendly but makes it harder to debug
    Math::Trans2f transf;
    RectAu boundingBox;

    SvgRootFrag(Opt<Box const&> box, Math::Trans2f transf, RectAu boundingBox)
        : SvgGroupFrag(box), transf(transf), boundingBox(boundingBox) {
    }

    void repr(Io::Emit& e) const {
        e("(SVGRootFrag)");
    }

    void offsetBoxFrags(Vec2Au d);

    void offset(Vec2Au d) {
        transf = transf.translated(d.cast<f64>());
        offsetBoxFrags(d);
    }
};

export using FragContent = Union<
    Vec<Frag>,
    SvgRootFrag>;

export struct Frag {
    Opt<Box&> box;
    Metrics metrics;
    FragContent content = Vec<Frag>{};

    Frag(Opt<Box&> box = NONE)
        : box(box) {}

    Style::ComputedValues const& style() const {
        return *box->style;
    }

    // https://drafts.csswg.org/css-overflow-3/#scrollable
    RectAu scrollableOverflow() const {
        // NOSPEC: This is just an approximation of the spec
        auto bound = metrics.borderBox();
        for (auto const& c : children())
            bound = bound.mergeWith(c.scrollableOverflow());
        return bound;
    }

    /// Offset the position of this fragment and its subtree.
    void offset(Vec2Au d) {
        metrics.position = metrics.position + d;

        if (auto children = content.is<Vec<Frag>>()) {
            for (auto& c : *children)
                c.offset(d);
        } else if (auto svg = content.is<SvgRootFrag>()) {
            svg->offset(d);
        }
    }

    Slice<Frag> children() const {
        if (auto children = content.is<Vec<Frag>>()) {
            return *children;
        }
        return {};
    }

    MutSlice<Frag> children() {
        if (auto children = content.is<Vec<Frag>>()) {
            return *children;
        }
        return {};
    }

    /// Add a child fragment.
    void add(Frag&& frag) {
        if (auto children = content.is<Vec<Frag>>()) {
            children->pushBack(std::move(frag));
        }
    }

    void repr(Io::Emit& e) const {
        e("(frag matrics: {} content: {})", metrics, content);
    }
};

void SvgGroupFrag::add(Element&& el) {
    elements.pushBack(std::move(el));
}

void SvgRootFrag::offsetBoxFrags(Vec2Au d) {
    for (auto& element : elements) {
        if (auto frag = element.is<::Box<Frag>>()) {
            (*frag)->offset(d);
        } else if (auto nestedRoot = element.is<SvgRootFrag>()) {
            nestedRoot->offsetBoxFrags(d);
        }
    }
}

void SvgGroupFrag::computeBoundingBoxes() {
    if (elements.len() == 0)
        return;

    // FIXME: this could be implemented in the Union type
    auto upcast = [&](Element& element, auto upcaster) {
        return element.visit(upcaster);
    };

    auto toSVGFrag = [&]<typename T>(T& el) -> SvgFrag* {
        if constexpr (Meta::Derive<T, SvgFrag>) {
            return static_cast<SvgFrag*>(&el);
        }
        return nullptr;
    };

    auto toSVGGroupFrag = [&]<typename T>(T& el) -> SvgGroupFrag* {
        if constexpr (Meta::Derive<T, SvgGroupFrag>) {
            return static_cast<SvgGroupFrag*>(&el);
        }
        return nullptr;
    };

    auto getElementBoundingBoxes = [&](Element& element) -> Pair<RectAu> {
        if (auto frag = element.is<::Box<Frag>>()) {
            return {
                (*frag)->metrics.borderBox(),
                (*frag)->metrics.borderBox()
            };
        } else if (auto svgFrag = upcast(element, toSVGFrag)) {
            if (auto svgGroupFrag = upcast(element, toSVGGroupFrag)) {
                svgGroupFrag->computeBoundingBoxes();
            }
            return {
                svgFrag->objectBoundingBox(),
                svgFrag->strokeBoundingBox()
            };
        } else
            unreachable();
    };

    auto [objectBoundingBox, strokeBoundingBox] = getElementBoundingBoxes(elements[0]);
    for (usize i = 1; i < elements.len(); i++) {
        auto [nextObjectBoundingBox, nextStrokeBoundingBox] = getElementBoundingBoxes(elements[i]);
        objectBoundingBox = objectBoundingBox.mergeWith(nextObjectBoundingBox);
        strokeBoundingBox = strokeBoundingBox.mergeWith(nextStrokeBoundingBox);
    }

    _objectBoundingBox = objectBoundingBox;
    _strokeBoundingBox = strokeBoundingBox;
}

} // namespace Vaev::Layout
