export module Vaev.Engine:layout.fragment;

import Karm.Core;
import Karm.Math;

import :layout.svg;
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
struct SVGRootFrag;

namespace SVG {

struct GroupFrag : SVG::Frag {
    using Element = Union<SVG::ShapeFrag, SVGRootFrag, ::Box<Vaev::Layout::Frag>, GroupFrag>;
    Vec<Element> elements = {};

    RectAu _objectBoundingBox{};
    RectAu _strokeBoundingBox{};

    Karm::Cursor<Group> box;

    GroupFrag(Karm::Cursor<Group> group)
        : box(group) {}

    static void computeBoundingBoxes(SVG::GroupFrag* group);

    RectAu objectBoundingBox() override {
        return _objectBoundingBox;
    }

    RectAu strokeBoundingBox() override {
        return _strokeBoundingBox;
    }

    Style::SpecifiedValues const& style() override {
        return *box->style;
    }

    void add(Element&& element);

    void repr(Io::Emit& e) const {
        e("(GroupFrag)");
    }
};

} // namespace SVG

struct SVGRootFrag : SVG::GroupFrag {
    // NOTE: SVG viewports have these intrinsic transformations; choosing to store these transforms is more compliant
    // and somewhat rendering-friendly but makes it harder to debug
    Math::Trans2f transf;
    SVG::Rectangle<Au> boundingBox;

    SVGRootFrag(Karm::Cursor<SVG::Group> group, Math::Trans2f transf, SVG::Rectangle<Au> boundingBox)
        : SVG::GroupFrag(group), transf(transf), boundingBox(boundingBox) {
    }

    static SVGRootFrag build(SVGRoot const& box, Vec2Au position, Vec2Au viewportSize) {
        SVG::Rectangle<Karm::Au> rect{position.x, position.y, viewportSize.x, viewportSize.y};

        Math::Trans2f transf =
            box.viewBox ? SVG::computeEquivalentTransformOfSVGViewport(*box.viewBox, position, viewportSize)
                        : Math::Trans2f::translate(position.cast<f64>());

        return SVGRootFrag{&box, transf, rect};
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
    SVGRootFrag>;

export struct Frag {
    MutCursor<Box> box;
    Metrics metrics;
    FragContent content = Vec<Frag>{};

    Frag(MutCursor<Box> box) : box{std::move(box)} {}

    Frag() : box{nullptr} {}

    Style::SpecifiedValues const& style() const {
        return *box->style;
    }

    // https://drafts.csswg.org/css-overflow-3/#scrollable
    RectAu scrollableOverflow() const {
        // NOSPEC: This is just an approximation of the spec
        auto bound = metrics.borderBox();
        for (auto c : children())
            bound = bound.mergeWith(c.scrollableOverflow());
        return bound;
    }

    /// Offset the position of this fragment and its subtree.
    void offset(Vec2Au d) {
        metrics.position = metrics.position + d;

        if (auto children = content.is<Vec<Frag>>()) {
            for (auto& c : *children)
                c.offset(d);
        } else if (auto svg = content.is<SVGRootFrag>()) {
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

void SVG::GroupFrag::add(Element&& el) {
    elements.pushBack(std::move(el));
}

void SVGRootFrag::offsetBoxFrags(Vec2Au d) {
    for (auto& element : elements) {
        if (auto frag = element.is<::Box<Vaev::Layout::Frag>>()) {
            (*frag)->offset(d);
        } else if (auto nestedRoot = element.is<SVGRootFrag>()) {
            nestedRoot->offsetBoxFrags(d);
        }
    }
}

void SVG::GroupFrag::computeBoundingBoxes(SVG::GroupFrag* group) {
    if (group->elements.len() == 0)
        return;

    // FIXME: this could be implemented in the Union type
    auto upcast = [&](Element const& element, auto upcaster) {
        return element.visit(upcaster);
    };

    auto toSVGFrag = [&]<typename T>(T const& el) -> SVG::Frag* {
        if constexpr (Meta::Derive<T, SVG::Frag>) {
            return (SVG::Frag*)(&el);
        }
        return nullptr;
    };

    auto toSVGGroupFrag = [&]<typename T>(T const& el) -> SVG::GroupFrag* {
        if constexpr (Meta::Derive<T, SVG::GroupFrag>) {
            return (SVG::GroupFrag*)(&el);
        }
        return nullptr;
    };

    auto getElementBoundingBoxes = [&](Element const& element) -> Pair<RectAu> {
        if (auto frag = element.is<::Box<Vaev::Layout::Frag>>()) {
            return {
                (*frag)->metrics.borderBox(),
                (*frag)->metrics.borderBox()
            };
        } else if (auto svgFrag = upcast(element, toSVGFrag)) {
            if (auto svgGroupFrag = upcast(element, toSVGGroupFrag)) {
                computeBoundingBoxes(svgGroupFrag);
            }
            return {
                svgFrag->objectBoundingBox(),
                svgFrag->strokeBoundingBox()
            };
        } else
            unreachable();
    };

    auto [objectBoundingBox, strokeBoundingBox] = getElementBoundingBoxes(group->elements[0]);
    for (usize i = 1; i < group->elements.len(); i++) {
        auto [nextObjectBoundingBox, nextStrokeBoundingBox] = getElementBoundingBoxes(group->elements[i]);
        objectBoundingBox = objectBoundingBox.mergeWith(nextObjectBoundingBox);
        strokeBoundingBox = strokeBoundingBox.mergeWith(nextStrokeBoundingBox);
    }

    group->_objectBoundingBox = objectBoundingBox;
    group->_strokeBoundingBox = strokeBoundingBox;
}

} // namespace Vaev::Layout
