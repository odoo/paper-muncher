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
struct SvgRootFrag;

namespace Svg {

struct GroupFrag : Svg::Frag {
    using Element = Union<Svg::ShapeFrag, SvgRootFrag, ::Box<Vaev::Layout::Frag>, GroupFrag>;
    Vec<Element> elements = {};

    RectAu _objectBoundingBox{};
    RectAu _strokeBoundingBox{};

    Karm::Cursor<Group> box;

    GroupFrag(Karm::Cursor<Group> group)
        : box(group) {}

    static void computeBoundingBoxes(Svg::GroupFrag* group);

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
} // namespace Svg

struct SvgRootFrag : Svg::GroupFrag {
    // NOTE: SVG viewports have these intrinsic transformations; choosing to store these transforms is more compliant
    // and somewhat rendering-friendly but makes it harder to debug
    Math::Trans2f transf;
    Svg::Rectangle<Au> boundingBox;

    SvgRootFrag(Karm::Cursor<Svg::Group> group, Math::Trans2f transf, Svg::Rectangle<Au> boundingBox)
        : Svg::GroupFrag(group), transf(transf), boundingBox(boundingBox) {
    }

    static SvgRootFrag build(SvgRoot const& box, Vec2Au position, Vec2Au viewportSize) {
        Svg::Rectangle<Karm::Au> rect{position.x, position.y, viewportSize.x, viewportSize.y};

        Math::Trans2f transf =
            box.viewBox ? Svg::computeEquivalentTransformOfSVGViewport(*box.viewBox, position, viewportSize)
                        : Math::Trans2f::translate(position.cast<f64>());

        return SvgRootFrag{&box, transf, rect};
    }

    void repr(Io::Emit& e) const {
        e("(SvgRootFrag)");
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

void Svg::GroupFrag::add(Element&& el) {
    elements.pushBack(std::move(el));
}

void SvgRootFrag::offsetBoxFrags(Vec2Au d) {
    for (auto& element : elements) {
        if (auto frag = element.is<::Box<Vaev::Layout::Frag>>()) {
            (*frag)->offset(d);
        } else if (auto nestedRoot = element.is<SvgRootFrag>()) {
            nestedRoot->offsetBoxFrags(d);
        }
    }
}

void Svg::GroupFrag::computeBoundingBoxes(Svg::GroupFrag* group) {
    if (group->elements.len() == 0)
        return;

    // FIXME: this could be implemented in the Union type
    auto upcast = [&](Element const& element, auto upcaster) {
        return element.visit(upcaster);
    };

    auto toSVGFrag = [&]<typename T>(T const& el) -> Svg::Frag* {
        if constexpr (Meta::Derive<T, Svg::Frag>) {
            return (Svg::Frag*)(&el);
        }
        return nullptr;
    };

    auto toSVGGroupFrag = [&]<typename T>(T const& el) -> Svg::GroupFrag* {
        if constexpr (Meta::Derive<T, Svg::GroupFrag>) {
            return (Svg::GroupFrag*)(&el);
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
