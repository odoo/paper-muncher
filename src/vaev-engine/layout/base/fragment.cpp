export module Vaev.Engine:layout.fragment;

import Karm.Core;
import Karm.Math;

import :values.length;
import :layout.box;

using namespace Karm;

namespace Vaev::Layout {

struct SvgRootFragment;

struct Fragment {
    Box const& _box;
    Vec<Rc<Fragment>> _children;

    Fragment(Box const& box)
        : _box(box) {}

    virtual ~Fragment() = default;

    // https://www.w3.org/TR/SVG2/coords.html#TermObjectBoundingBox
    virtual RectAu objectBoundingBox() const {
        return borderBox();
    }

    // https://www.w3.org/TR/SVG2/coords.html#TermStrokeBoundingBox
    virtual RectAu strokeBoundingBox() const {
        return borderBox();
    }

    // https://www.w3.org/TR/css-box-3/#border-box
    virtual RectAu borderBox() const = 0;

    // https://www.w3.org/TR/css-box-3/#padding-box
    virtual RectAu paddingBox() const = 0;

    // https://www.w3.org/TR/css-box-3/#content-box
    virtual RectAu contentBox() const = 0;

    // https://www.w3.org/TR/css-box-3/#margin-box
    virtual RectAu marginBox() const = 0;

    // https://drafts.csswg.org/css-overflow-3/#scrollable
    RectAu scrollableOverflow() const {
        auto bound = borderBox();
        for (auto const& c : _children)
            bound = bound.mergeWith(c->scrollableOverflow());
        return bound;
    }

    Style::ComputedValues const& style() const {
        return *_box.style;
    }

    Box const& originatingBox() const {
        return _box;
    }

    Opt<Dom::OriginatingElement> const& originatingElement() const {
        return _box.origin;
    }

    virtual void offset(Vec2Au d) {
        for (auto& c : _children)
            c->offset(d);
    }

    void add(Rc<Fragment>&& frag) {
        _children.pushBack(std::move(frag));
    }

    MutSlice<Rc<Fragment>> children() {
        return _children;
    }

    Slice<Rc<Fragment>> children() const {
        return _children;
    }

    virtual void repr(Io::Emit& e) const = 0;

    virtual void paintOwnWireframe(Gfx::Canvas& g) const {
        g.strokeStyle({
            .fill = Gfx::GREEN,
            .width = 1,
            .align = Gfx::INSIDE_ALIGN,
        });
        g.stroke(borderBox().cast<f64>());
    }

    void paintWireframe(Gfx::Canvas& g) {
        for (auto& c : children())
            c->paintWireframe(g);
        paintOwnWireframe(g);
    }

    virtual void paintOwnOverlay(Gfx::Canvas& g) const {
        g.fillStyle(Gfx::BLUE.withOpacity(0.5));
        g.fill(borderBox().cast<f64>());
    }

    void _paintContentBoxGuides(Gfx::Canvas& g, Math::Rectf viewport) {
        auto cb = contentBox().cast<f64>();
        g.strokeStyle(
            Gfx::stroke(Gfx::CYAN)
                .withDash({2, 2})
        );
        g.stroke(Math::Edgef{
            cb.start(),
            viewport.top(),
            cb.start(),
            viewport.bottom(),
        });
        g.stroke(Math::Edgef{
            cb.end(),
            viewport.top(),
            cb.end(),
            viewport.bottom(),
        });
        g.stroke(Math::Edgef{
            viewport.start(),
            cb.top(),
            viewport.end(),
            cb.top(),
        });
        g.stroke(Math::Edgef{
            viewport.start(),
            cb.bottom(),
            viewport.end(),
            cb.bottom(),
        });
    }

    void paintOverlay(Gfx::Canvas& g, Dom::OriginatingElement of, Math::Rectf viewport) {
        if (originatingElement() == of) {
            _paintContentBoxGuides(g, viewport);
            paintOwnOverlay(g);
        }

        for (auto& c : children())
            c->paintOverlay(g, of, viewport);
    }
};

using SvgShape = Union<RectAu, EllipseAu, Math::Path>;

struct SvgShapeFragment : Fragment {
    SvgShape shape;
    Au strokeWidth;

    SvgShapeFragment(Box& box, SvgShape shape, Au strokeWidth)
        : Fragment(box), shape(shape), strokeWidth(strokeWidth) {}

    RectAu objectBoundingBox() const override {
        return shape.visit(
            [](auto const& s) {
                return s.bound().template cast<Au>();
            }
        );
    }

    RectAu strokeBoundingBox() const override {
        return objectBoundingBox().grow(Au{strokeWidth / 2_au});
    }

    RectAu marginBox() const override {
        // NOTE: Blanket implementation, because it most
        //       cases the strokeBoundingBox behave like
        //       the margin box of the shape.
        return strokeBoundingBox();
    }

    RectAu borderBox() const override {
        // NOTE: Blanket implementation, because it most
        //       cases the strokeBoundingBox behave like
        //       the border box of the shape.
        return strokeBoundingBox();
    }

    RectAu paddingBox() const override {
        // NOTE: Blanket implementation, because it most
        //       cases the objectBoundingBox behave like
        //       the padding box of the shape.
        return objectBoundingBox();
    }

    RectAu contentBox() const override {
        // NOTE: Blanket implementation, because it most
        //       cases the objectBoundingBox behave like
        //       the content box of the shape.
        return objectBoundingBox();
    }

    void repr(Io::Emit& e) const override {
        e("(svg-shape-frag {} {})", shape, strokeWidth);
    }
};

struct SvgGroupFragment : Fragment {
    SvgGroupFragment(Box& box)
        : Fragment(box) {}

    RectAu objectBoundingBox() const override {
        if (not _children)
            return {};
        auto bound = _children[0]->objectBoundingBox();
        for (auto const& c : next(_children, 1))
            bound = bound.mergeWith(c->objectBoundingBox());
        return bound;
    }

    RectAu strokeBoundingBox() const override {
        if (not _children)
            return {};
        auto bound = _children[0]->strokeBoundingBox();
        for (auto const& c : next(_children, 1))
            bound = bound.mergeWith(c->strokeBoundingBox());
        return bound;
    }

    RectAu marginBox() const override {
        // NOTE: Blanket implementation, because it most
        //       cases the strokeBoundingBox behave like
        //       the margin box of the shape.
        return strokeBoundingBox();
    }

    RectAu borderBox() const override {
        // NOTE: Blanket implementation, because it most
        //       cases the strokeBoundingBox behave like
        //       the border box of the shape.
        return strokeBoundingBox();
    }

    RectAu paddingBox() const override {
        // NOTE: Blanket implementation, because it most
        //       cases the objectBoundingBox behave like
        //       the padding box of the shape.
        return objectBoundingBox();
    }

    RectAu contentBox() const override {
        // NOTE: Blanket implementation, because it most
        //       cases the objectBoundingBox behave like
        //       the content box of the shape.
        return objectBoundingBox();
    }

    void repr(Io::Emit& e) const override {
        e("(svg-group-frag  children:{})", _children);
    }
};

struct SvgRootFragment : Fragment {
    // NOTE: SVG viewports have these intrinsic transformations; choosing to store these transforms is more compliant
    // and somewhat rendering-friendly but makes it harder to debug
    Math::Trans2f transform;
    RectAu boundingBox;

    SvgRootFragment(Box& box, Math::Trans2f transf, RectAu boundingBox)
        : Fragment(box), transform(transf), boundingBox(boundingBox) {
    }

    RectAu objectBoundingBox() const override {
        return boundingBox;
    }

    RectAu strokeBoundingBox() const override {
        return boundingBox;
    }

    RectAu marginBox() const override {
        return boundingBox;
    }

    RectAu borderBox() const override {
        return boundingBox;
    }

    RectAu paddingBox() const override {
        return boundingBox;
    }

    RectAu contentBox() const override {
        return boundingBox;
    }

    void offset(Vec2Au d) override {
        transform = transform.translated(d.cast<f64>());
        Fragment::offset(d);
    }

    void repr(Io::Emit& e) const override {
        e("(svg-root-frag transform:{} boundingBox:{} children:{})", transform, boundingBox, _children);
    }
};

// https://www.w3.org/TR/css-box-3/#box-model
export struct BoxMetrics {
    InsetsAu padding{};
    InsetsAu borders{};
    Au outlineOffset{};
    Au outlineWidth{};
    Vec2Au position; //< Position relative to the content box of the containing block
    Vec2Au borderSize;
    InsetsAu margin{};
    RadiiAu radii{};

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

    void repr(Io::Emit& e) const {
        e("(layout paddings: {} borders: {} position: {} borderSize: {} margin: {} radii: {})",
          padding, borders, position, borderSize, margin, radii);
    }
};

export struct BoxFragment : Fragment {
    BoxMetrics metrics;

    BoxFragment(Box& box, BoxMetrics metrics = {})
        : Fragment(box), metrics(metrics) {}

    RectAu borderBox() const override {
        return metrics.borderBox();
    }

    RectAu paddingBox() const override {
        return metrics.paddingBox();
    }

    RectAu contentBox() const override {
        return metrics.contentBox();
    }

    RectAu marginBox() const override {
        return metrics.marginBox();
    }

    void offset(Vec2Au d) override {
        metrics.position = metrics.position + d;
        Fragment::offset(d);
    }

    void paintOwnOverlay(Gfx::Canvas& g) const override {
        Gfx::Borders border;

        // Margins
        border.widths = metrics.margin.cast<f64>();
        border.withFill(Gfx::YELLOW800.withOpacity(0.5));
        border.withStyle(Gfx::BorderStyle::SOLID);
        border.paint(g, metrics.marginBox().cast<f64>());

        // Borders
        border.widths = metrics.borders.cast<f64>();
        border.withFill(Gfx::YELLOW500.withOpacity(0.5));
        border.withStyle(Gfx::BorderStyle::SOLID);
        border.paint(g, metrics.borderBox().cast<f64>());

        // Paddings
        border.widths = metrics.padding.cast<f64>();
        border.withFill(Gfx::GREEN500.withOpacity(0.5));
        border.withStyle(Gfx::BorderStyle::SOLID);
        border.paint(g, metrics.paddingBox().cast<f64>());

        // Content Box
        g.fillStyle(Gfx::BLUE.withOpacity(0.5));
        g.fill(metrics.contentBox().cast<f64>());
    }

    void repr(Io::Emit& e) const override {
        e("(box-frag matrics: {} children: {})", metrics, _children);
    }
};

} // namespace Vaev::Layout
