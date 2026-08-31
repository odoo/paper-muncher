export module Vaev.Engine:layout.fragment;

import Karm.Core;
import Karm.Math;

import :values.length;
import :layout.box;

using namespace Karm;

namespace Vaev::Layout {

// https://drafts.csswg.org/css-position-4/#out-of-band-outline
export struct OutOfBandOutline {
    Gfx::Outline outline;
    Math::Rectf rect;
    Math::Radiif radii;

    void paint(Gfx::Canvas& g) {
        outline.paint(g, rect, radii);
    }
};

export struct Fragment {
    enum struct Options : u8 {
        OUT_OF_FLOW = 1 << 0,
    };

    using enum Options;

    Box& _box;
    Vec<Rc<Fragment>> _children;
    Flags<Options> _flags = {};

    Fragment(Box& box, Vec<Rc<Fragment>> children)
        : _box(box), _children(std::move(children)) {}

    virtual ~Fragment() = default;

    Flags<Options>& flags() {
        return _flags;
    }

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

    Style::ComputedValues const& style() const {
        return *_box.style;
    }

    Box& originatingBox() const {
        return _box;
    }

    Opt<Dom::OriginatingElement> const& originatingElement() const {
        return _box.origin;
    }

    virtual void offset(Vec2Au d) {
        for (auto& c : _children)
            c->offset(d);
    }

    MutSlice<Rc<Fragment>> children() {
        return _children;
    }

    Slice<Rc<Fragment>> children() const {
        return _children;
    }

    virtual void repr(Io::Emit& e) const = 0;

    virtual Opt<Math::Rectf> intrinsicViewBox() const {
        return NONE;
    }

    virtual Math::Trans2f intrinsicTransform() const {
        return Math::Trans2f::identity();
    }

    // https://www.w3.org/TR/SVG11/intro.html#TermSVGViewport
    virtual Opt<RectAu> intrinsicClip() const {
        return NONE;
    }

    // https://www.w3.org/TR/css-transforms-1/#transform-box
    // NOTE: Elements with an associated CSS layout box resolve fill-box,
    //       stroke-box and view-box against their own boxes, while SVG
    //       elements without one resolve them against their bounding boxes
    //       and the nearest SVG viewport.
    virtual bool hasCssLayoutBox() const {
        return true;
    }

    // https://drafts.csswg.org/css-position-4/#paint-a-blocks-decorations
    virtual void paintDecoration([[maybe_unused]] Gfx::Canvas& g) {}

    // 8. Otherwise
    // https://drafts.csswg.org/css-position-4/#paint-a-stacking-context
    virtual void paintContent([[maybe_unused]] Gfx::Canvas& g, [[maybe_unused]] Vec<OutOfBandOutline>& outOfBandOutlines) {}

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

export struct PlaceholderFragment : Fragment {
    Opt<Rc<Fragment>> fragment = NONE;

    // https://www.w3.org/TR/css-position-3/#staticpos-rect
    RectAu staticPosRect;

    PlaceholderFragment(Box& box, RectAu staticPositionRectangle)
        : Fragment(box, {}), staticPosRect(staticPositionRectangle) {}

    RectAu borderBox() const override {
        unreachable();
    }

    RectAu paddingBox() const override {
        unreachable();
    }

    RectAu contentBox() const override {
        unreachable();
    }

    RectAu marginBox() const override {
        unreachable();
    }

    void repr(Io::Emit& e) const override {
        e("(placeholder-frag)");
    }

    void paintOwnWireframe(Gfx::Canvas&) const override {
        unreachable();
    }

    void paintOwnOverlay(Gfx::Canvas&) const override {
        unreachable();
    }
};

// MARK: Svg -------------------------------------------------------------------

export using SvgShape = Union<
    RectAu,
    EllipseAu,
    Math::Path>;

export struct SvgShapeFragment : Fragment {
    SvgShape shape;
    Au strokeWidth;

    SvgShapeFragment(Box& box, SvgShape shape, Au strokeWidth, Vec<Rc<Fragment>> children = {})
        : Fragment(box, std::move(children)), shape(shape), strokeWidth(strokeWidth) {}

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
        // NOTE: Blanket implementation, because in most
        //       cases the strokeBoundingBox behave like
        //       the margin box of the shape.
        return strokeBoundingBox();
    }

    RectAu borderBox() const override {
        // NOTE: Blanket implementation, because in most
        //       cases the strokeBoundingBox behave like
        //       the border box of the shape.
        return strokeBoundingBox();
    }

    RectAu paddingBox() const override {
        // NOTE: Blanket implementation, because in most
        //       cases the objectBoundingBox behave like
        //       the padding box of the shape.
        return objectBoundingBox();
    }

    RectAu contentBox() const override {
        // NOTE: Blanket implementation, because in most
        //       cases the objectBoundingBox behave like
        //       the content box of the shape.
        return objectBoundingBox();
    }

    Opt<Gfx::Fill> resolveFill(SvgProps const& svg) {
        if (Math::epsilonEq(svg.fillOpacity, 0.))
            return NONE;

        if (not svg.fill)
            return NONE;

        if (auto [color] = resolve(svg.fill, style().color)) {
            color = color.withOpacity(svg.fillOpacity);
            if (color.transparent())
                return NONE;
            return Some(Gfx::Fill{color});
        }

        return NONE;
    }

    Opt<Gfx::Stroke> resolveStroke(SvgProps const& svg) {
        if (Math::epsilonEq(svg.strokeOpacity, 0.))
            return NONE;

        if (strokeWidth == 0_au)
            return NONE;

        if (not svg.stroke)
            return NONE;

        if (auto [color] = resolve(svg.stroke, style().color)) {
            color = color.withOpacity(svg.strokeOpacity);
            if (color.transparent())
                return NONE;

            return Some(Gfx::Stroke{
                .fill = color,
                .width = static_cast<f64>(strokeWidth),
                // FIXME: 'stroke-linejoin' is not implemented yet, so we
                //        always use its initial value.
                .join = Gfx::MITER_JOIN,
            });
        }

        return NONE;
    }

    void paintContent(Gfx::Canvas& g, Vec<OutOfBandOutline>& outOfBandOutlines) override {
        (void)outOfBandOutlines;

        auto const& style = *originatingBox().style->svg;

        Opt<Gfx::Fill> resolvedFill = resolveFill(style);
        Opt<Gfx::Stroke> resolvedStroke = resolveStroke(style);

        if (not(resolvedFill or resolvedStroke))
            return;

        g.beginPath();
        shape.visit(
            [&](RectAu const& rect) {
                g.rect(rect.cast<f64>());
            },
            [&](EllipseAu const& ellipse) {
                g.ellipse(ellipse.cast<f64>());
            },
            [&](Math::Path const& path) {
                g.path(path);
            }
        );

        if (auto& [fill] = resolvedFill)
            g.fill(fill);

        if (auto& [stroke] = resolvedStroke)
            g.stroke(stroke);
    }

    bool hasCssLayoutBox() const override {
        return false;
    }

    void repr(Io::Emit& e) const override {
        e("(svg-shape-frag {} {})", shape, strokeWidth);
    }
};

export struct SvgGroupFragment : Fragment {
    SvgGroupFragment(Box& box, Vec<Rc<Fragment>> children = {})
        : Fragment(box, std::move(children)) {}

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
        // NOTE: Blanket implementation, because in most
        //       cases the strokeBoundingBox behave like
        //       the margin box of the shape.
        return strokeBoundingBox();
    }

    RectAu borderBox() const override {
        // NOTE: Blanket implementation, because in most
        //       cases the strokeBoundingBox behave like
        //       the border box of the shape.
        return strokeBoundingBox();
    }

    RectAu paddingBox() const override {
        // NOTE: Blanket implementation, because in most
        //       cases the objectBoundingBox behave like
        //       the padding box of the shape.
        return objectBoundingBox();
    }

    RectAu contentBox() const override {
        // NOTE: Blanket implementation, because in most
        //       cases the objectBoundingBox behave like
        //       the content box of the shape.
        return objectBoundingBox();
    }

    bool hasCssLayoutBox() const override {
        return false;
    }

    void repr(Io::Emit& e) const override {
        e("(svg-group-frag children:{})", _children);
    }
};

export struct SvgRootFragment : Fragment {
    // NOTE: SVG viewports have these intrinsic transformations; choosing
    //       to store these transforms is more compliant and somewhat
    //       rendering-friendly but makes it harder to debug
    Math::Trans2f transform;
    RectAu boundingBox;

    SvgRootFragment(Box& box, Math::Trans2f transf, RectAu boundingBox, Vec<Rc<Fragment>> children = {})
        : Fragment(box, std::move(children)), transform(transf), boundingBox(boundingBox) {
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

    // NOTE: Maps the viewBox onto the viewport, this applies to the
    //       content of the viewport, inside of any CSS transform.
    Math::Trans2f intrinsicTransform() const override {
        return transform;
    }

    Opt<RectAu> intrinsicClip() const override {
        return Some(boundingBox);
    }

    // https://www.w3.org/TR/SVG11/intro.html#TermSVGViewport
    Opt<Math::Rectf> intrinsicViewBox() const override {
        if (auto& [viewBox] = style().svg->viewBox)
            return Some(Math::Rectf{
                viewBox.width,
                viewBox.height,
            });
        return Some(objectBoundingBox().cast<f64>());
    }

    void repr(Io::Emit& e) const override {
        e("(svg-root-frag transform:{} boundingBox:{} children:{})", transform, boundingBox, _children);
    }
};

// MARK: Regular Box -----------------------------------------------------------

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
    Opt<UsedBorders> usedBorders;

    BoxFragment(Box& box, BoxMetrics metrics = {}, Vec<Rc<Fragment>> children = {})
        : Fragment(box, std::move(children)), metrics(metrics) {}

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

    // https://www.w3.org/TR/SVG2/embedded.html#ForeignObjectElement
    // NOTE: A foreign object establishes a new viewport, its content is
    //       clipped to it.
    Opt<RectAu> intrinsicClip() const override {
        if (originatingBox().isSvgForeignObjectBox())
            return Some(borderBox());
        return NONE;
    }

    static Opt<Gfx::Borders> buildBorders(BoxMetrics const& metrics, Style::ComputedValues const& style) {
        if (metrics.borders.zero())
            return NONE;

        Gfx::Borders borders;

        auto const& bordersLayout = metrics.borders;
        borders.widths.top = bordersLayout.top.cast<f64>();
        borders.widths.bottom = bordersLayout.bottom.cast<f64>();
        borders.widths.start = bordersLayout.start.cast<f64>();
        borders.widths.end = bordersLayout.end.cast<f64>();

        auto const& bordersStyle = *style.borders;
        borders.styles[0] = bordersStyle.top.style;
        borders.styles[1] = bordersStyle.end.style;
        borders.styles[2] = bordersStyle.bottom.style;
        borders.styles[3] = bordersStyle.start.style;

        borders.fills[0] = resolve(bordersStyle.top.color, style.color);
        borders.fills[1] = resolve(bordersStyle.end.color, style.color);
        borders.fills[2] = resolve(bordersStyle.bottom.color, style.color);
        borders.fills[3] = resolve(bordersStyle.start.color, style.color);

        return Some(borders);
    }

    static Opt<Gfx::Borders> buildBorders(BoxMetrics const& metrics, UsedBorders const& border) {
        if (metrics.borders.zero())
            return NONE;

        Gfx::Borders borders;

        borders.widths.top = metrics.borders.top.cast<f64>();
        borders.widths.bottom = metrics.borders.bottom.cast<f64>();
        borders.widths.start = metrics.borders.start.cast<f64>();
        borders.widths.end = metrics.borders.end.cast<f64>();

        borders.styles[0] = border.top.style;
        borders.styles[1] = border.end.style;
        borders.styles[2] = border.bottom.style;
        borders.styles[3] = border.start.style;

        borders.fills[0] = border.top.color;
        borders.fills[1] = border.end.color;
        borders.fills[2] = border.bottom.color;
        borders.fills[3] = border.start.color;

        return Some(borders);
    }

    // https://drafts.csswg.org/css-position-4/#paint-a-blocks-decorations
    void paintDecoration(Gfx::Canvas& g) override {
        auto const& background = style().backgrounds;

        auto color = resolve(background->color, style().color);

        Math::Rectf bound = borderBox().round().cast<f64>();
        auto radii = metrics.radii.cast<f64>();

        if (not color.transparent()) {
            // NOTE: Clearing the canvas is only sound for an opaque background,
            //       a translucent one must be blended with what is underneath.
            if (originatingBox().isRootElementPrincipalBox() and color.alpha == 255) {
                g.clear(color);
            } else {
                g.fillStyle(color);
                g.fill(bound, radii);
            }
        }

        auto bordersWithoutRadii =
            usedBorders
                ? buildBorders(metrics, *usedBorders)
                : buildBorders(metrics, style());

        if (bordersWithoutRadii) {
            auto borders = bordersWithoutRadii
                               ? bordersWithoutRadii.take()
                               : Gfx::Borders{};

            borders.radii = metrics.radii.cast<f64>();
            borders.paint(g, bound);
        }
    }

    void paintContent(Gfx::Canvas& g, Vec<OutOfBandOutline>& outOfBandOutlines) override {
        // https://drafts.csswg.org/css-position-4/#paint-a-blocks-decorations:~:text=and%20canvas.-,Otherwise,-First%20for%20root
        // If the box is a replaced element, paint the replaced content into canvas, atomically.
        if (originatingBox().isReplaced()) {
            auto& image = originatingBox().content.unwrap<Gfx::Snapshot>();
            auto trans = Math::Trans2f::map(
                image.size().cast<f64>(),
                contentBox().cast<f64>()
            );

            g.push();
            g.transform(trans);
            if (not metrics.radii.zero()) {
                g.beginPath();
                g.rect(
                    contentBox().size().cast<f64>(),
                    metrics.radii.cast<f64>()
                );
                g.clip();
            }
            (void)image.replay(g);
            g.pop();
        }

        // Otherwise, for each line box of the box, paint a box in a line box given the box, the line box, and canvas.
        else if (originatingBox().content.is<Rc<Gfx::Prose>>()) {
            auto& prose = originatingBox().content.unwrap<Rc<Gfx::Prose>>();

            g.push();
            g.origin(contentBox().topStart().cast<f64>());
            g.fill(prose);
            g.pop();
        }

        // If the UA uses in-band outlines, paint the outlines of the box into canvas.
        if (metrics.outlineWidth != 0_au) {
            Gfx::Outline outline;
            outline.width = metrics.outlineWidth.cast<f64>();
            outline.offset = metrics.outlineOffset.cast<f64>();
            auto const& outlineStyle = *style().outline;

            if (outlineStyle.style.is<Keywords::Auto>()) {
                outline.style = Gfx::BorderStyle::SOLID;
            } else {
                outline.style = outlineStyle.style.unwrap<Gfx::BorderStyle>();
            }

            outline.fill = resolve(outlineStyle.color, style().color);

            outOfBandOutlines.pushBack(OutOfBandOutline{
                .outline = outline,
                .rect = borderBox().round().cast<f64>(),
                .radii = metrics.radii.cast<f64>(),
            });
        }
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
        e("(box-frag matrics:{} children:{})", metrics, _children);
    }
};

} // namespace Vaev::Layout
