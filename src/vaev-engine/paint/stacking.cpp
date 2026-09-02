export module Vaev.Engine:paint.stacking;

import Karm.Gfx;
import :layout.fragment;
import :layout.values;
import :paint.background;
import :paint.clip;
import :paint.transform;

namespace Vaev::Paint {

// https://drafts.csswg.org/css-position-4/#painting-order
enum struct PaintingOrder {
    POSITIONED_NEGATIVE_Z, //< 5. positioned descendants with negative z-indices
    FLOAT,                 //< 7. non-positioned floating descendants
    IN_FLOW,               //< 8. in-flow, non-positioned, block-level descendants
    POSITIONED_ZERO_Z,     //< 9. positioned descendants with 'z-index: auto' or 'z-index: 0'
    POSITIONED_POSITIVE_Z, //< 10. positioned descendants with z-indices greater than or equal to 1

    _LEN,
};

// https://drafts.csswg.org/css-position-4/#paint-a-stacking-context
export struct StackingContext {
    struct Layer {
        PaintingOrder paintingOrder;
        Integer zIndex;
        Union<Rc<Layout::Fragment>, Rc<StackingContext>> content;

        void paintStackingContext(Gfx::Canvas& g, Math::Rectf viewBox) {
            if (auto stackingContext = content.is<Rc<StackingContext>>())
                (*stackingContext)->paintNested(g, viewBox);
        }

        void paintBoxDecoration(Gfx::Canvas& g) {
            if (auto fragment = content.is<Rc<Layout::Fragment>>())
                if ((*fragment)->style().visibility != Visibility::HIDDEN)
                    (*fragment)->paintDecoration(g);
        }

        void paintBoxContent(Gfx::Canvas& g, Vec<Layout::OutOfBandOutline>& outOfBandOutlines) {
            if (auto fragment = content.is<Rc<Layout::Fragment>>())
                if ((*fragment)->style().visibility != Visibility::HIDDEN)
                    (*fragment)->paintContent(g, outOfBandOutlines);
        }

        void paintWireframe(Gfx::Canvas& g, Math::Rectf viewBox) {
            return content.visit(
                [&](Rc<Layout::Fragment>& f) {
                    return f->paintWireframe(g);
                },
                [&](Rc<StackingContext>& sc) {
                    return sc->paintWireframe(g, viewBox);
                }
            );
        }

        void paintOverlay(Gfx::Canvas& g, Dom::OriginatingElement of, Math::Rectf viewport) {
            return content.visit(
                [&](Rc<Layout::Fragment>& f) {
                    return f->paintOverlay(g, of, viewport);
                },
                [&](Rc<StackingContext>& sc) {
                    return sc->paintOverlay(g, of, viewport);
                }
            );
        }

        Opt<Rc<Layout::Fragment>> hitest(Math::Vec2f pos, Math::Rectf viewBox) {
            return content.visit(
                [&](Rc<Layout::Fragment>& f) -> Opt<Rc<Layout::Fragment>> {
                    if (f->borderBox().contains(pos.cast<Au>())) {
                        return Some(f);
                    }
                    return NONE;
                },
                [&](Rc<StackingContext>& sc) {
                    return sc->hitest(pos, viewBox);
                }
            );
        }

        RectAu scrollableOverflow() {
            return content.visit(
                [](Rc<Layout::Fragment>& f) {
                    return f->borderBox();
                },
                [](Rc<StackingContext>& sc) {
                    return sc->scrollableOverflow();
                }
            );
        }

        void repr(Io::Emit& e) const {
            e("({} {} ", paintingOrder, zIndex);
            if (auto stackingContext = content.is<Rc<StackingContext>>()) {
                e("{}", *stackingContext);
            } else {
                e("(fragment)");
            }
            e(")");
        }
    };

    Rc<Layout::Fragment> fragment;
    Vec<Layer> layers = {};
    bool sorted = false;

    StackingContext(Rc<Layout::Fragment> fragment)
        : fragment(fragment) {}

    // MARK: Dispatch --------------------------------------------------------------------------------------------------

    static Rc<StackingContext> establishStackingContext(Rc<Layout::Fragment> fragment) {
        auto stackingContext = makeRc<StackingContext>(fragment);
        stackingContext->dispatchChildren(fragment, *stackingContext);
        return stackingContext;
    }

    void addStackingLayer(PaintingOrder paintingOrder, Integer zIndex, Union<Rc<Layout::Fragment>, Rc<StackingContext>> content) {
        layers.emplaceBack(paintingOrder, zIndex, content);
    }

    // https://drafts.csswg.org/css-position-4/#paint-a-stacking-container
    void addStackingContainer(PaintingOrder paintingOrder, Integer zIndex, Rc<Layout::Fragment> fragment) {
        // Created a new stacking context, but any positioned descendants and
        // descendants which actually create a new stacking context should be
        // considered part of the parent stacking context
        auto stackingContext = makeRc<StackingContext>(fragment);
        addStackingLayer(paintingOrder, zIndex, stackingContext);
        stackingContext->dispatchChildren(fragment, *this);
    }

    void dispatchChildren(Rc<Layout::Fragment> fragment, StackingContext& parentStackingContext) {
        for (auto& c : fragment->children()) {
            if (c->flags().has(Layout::Fragment::OUT_OF_FLOW))
                continue;

            if (auto ph = c.is<Layout::PlaceholderFragment>()) {
                if (ph->fragment)
                    dispatchStackingLayer(*ph->fragment, parentStackingContext);
                continue;
            }

            dispatchStackingLayer(c, parentStackingContext);
        }
    }

    // https://drafts.csswg.org/css-position-4/#paint-a-stacking-context
    void dispatchStackingLayer(Rc<Layout::Fragment> fragment, StackingContext& parentStackingContext) {
        auto& box = fragment->originatingBox();
        auto zIndex = box.style->zIndex;

        // https://drafts.csswg.org/css2/#z-index
        // NOTE: 'z-index' only applies to positioned boxes, non-positioned ones
        //       always paint in tree order.
        auto zIndexNumber = box.isPositioned() ? zIndex.unwrapOr<Integer>(0) : 0;

        // 5. positioned descendants with negative (non-zero) z-index values
        if (box.isPositioned() and zIndexNumber < 0) {
            parentStackingContext.addStackingLayer(
                PaintingOrder::POSITIONED_NEGATIVE_Z,
                zIndexNumber,
                establishStackingContext(fragment)
            );
        }

        // 7. non-positioned floating descendants, in tree order
        else if (not box.isPositioned() and box.isFloating()) {
            parentStackingContext.addStackingContainer(
                PaintingOrder::FLOAT,
                zIndexNumber,
                fragment
            );
        }

        // 8. in-flow, non-positioned, block-level descendant boxes
        else if (not box.isPositioned() and not box.isFloating()) {
            if (box.impliesNewStackingContext()) {
                parentStackingContext.addStackingLayer(
                    PaintingOrder::IN_FLOW,
                    zIndexNumber,
                    establishStackingContext(fragment)
                );
            } else {
                addStackingLayer(PaintingOrder::IN_FLOW, zIndexNumber, fragment);
                dispatchChildren(fragment, parentStackingContext);
            }
        }

        // 9. positioned descendants with z-index: auto or z-index: 0
        else if (box.isPositioned() and zIndexNumber == 0) {
            // descendant has z-index: auto
            if (zIndex == Keywords::AUTO) {
                parentStackingContext.addStackingContainer(
                    PaintingOrder::POSITIONED_ZERO_Z,
                    zIndexNumber,
                    fragment
                );
            }

            // descendant has z-index: 0
            else {
                parentStackingContext.addStackingLayer(
                    PaintingOrder::POSITIONED_ZERO_Z,
                    zIndexNumber,
                    establishStackingContext(fragment)
                );
            }
        }
        // 10. positioned descendants with positive (non-zero) z-index values
        else if (box.isPositioned() and zIndexNumber >= 1) {
            parentStackingContext.addStackingLayer(
                PaintingOrder::POSITIONED_POSITIVE_Z,
                zIndexNumber,
                establishStackingContext(fragment)
            );
        }
    }

    // MARK: Z Sorting ---------------------------------------------------------

    void ensureSorted() {
        if (sorted)
            return;
        stableSort(layers, [](auto const& a, auto const& b) {
            if (auto c = a.paintingOrder <=> b.paintingOrder; c != 0)
                return c;
            return a.zIndex <=> b.zIndex;
        });
        sorted = true;
    }

    // MARK: Paint -------------------------------------------------------------

    void paintNested(Gfx::Canvas& g, Math::Rectf viewBox) {
        ensureSorted();

        g.push();

        if (fragment->style().clip->has()) {
            applyClip(fragment, g, viewBox);
        }

        if (fragment->style().transform->has()) {
            applyTransform(fragment, g, viewBox);
        }

        if (fragment->style().opacity != 1.0) {
            g.opacity(fragment->style().opacity);
        }

        // NOTE: SVG viewports clip their content and map their viewBox onto the
        //       viewport, both apply to their content, inside of any CSS transform
        if (auto clip = fragment->intrinsicClip())
            g.clip(clip->cast<f64>());

        g.transform(fragment->intrinsicTransform());

        // NOTE: From here on, descendants live in this fragment's viewport
        viewBox = fragment->intrinsicViewBox().unwrapOr(viewBox);

        Vec<Layout::OutOfBandOutline> outOfBandOutlines;

        // 4. If root is a block-level box, paint a block’s decorations given root and canvas.
        if (fragment->style().visibility != Visibility::HIDDEN)
            fragment->paintDecoration(g);

        for (auto& e : layers) {
            // 5. positioned descendants with negative (non-zero) z-index values
            if (e.paintingOrder == PaintingOrder::POSITIONED_NEGATIVE_Z)
                e.paintStackingContext(g, viewBox);

            // 6. non-positioned, block-level descendants, paint a block’s decorations
            if (e.paintingOrder == PaintingOrder::IN_FLOW)
                e.paintBoxDecoration(g);

            // 7. non-positioned floating descendants, in tree order
            if (e.paintingOrder == PaintingOrder::FLOAT)
                e.paintStackingContext(g, viewBox);
        }

        // 8. Otherwise, paint root’s own content, then the in-flow,
        //    non-positioned, block-level descendants. Descendants that
        //    established their own stacking context are painted atomically,
        //    in tree order.
        if (fragment->style().visibility != Visibility::HIDDEN)
            fragment->paintContent(g, outOfBandOutlines);

        for (auto& e : layers) {
            if (e.paintingOrder == PaintingOrder::IN_FLOW) {
                e.paintBoxContent(g, outOfBandOutlines);
                e.paintStackingContext(g, viewBox);
            }

            // 9. positioned descendants with 'z-index: auto' or 'z-index: 0'
            if (e.paintingOrder == PaintingOrder::POSITIONED_ZERO_Z)
                e.paintStackingContext(g, viewBox);

            // 10. positioned descendants with z-indices greater than or equal to 1
            if (e.paintingOrder == PaintingOrder::POSITIONED_POSITIVE_Z)
                e.paintStackingContext(g, viewBox);
        }

        // 11. draw all of root’s outlines into canvas
        for (auto& outline : outOfBandOutlines)
            outline.paint(g);

        g.pop();
    }

    void paintRoot(Gfx::Canvas& g) {
        paintNested(g, {});
    }

    void paintWireframe(Gfx::Canvas& g, Math::Rectf viewBox) {
        ensureSorted();

        g.push();

        if (fragment->style().transform->has()) {
            applyTransform(fragment, g, viewBox);
        }

        // NOTE: From here on, descendants live in this fragment's viewport
        g.transform(fragment->intrinsicTransform());

        viewBox = fragment->intrinsicViewBox().unwrapOr(viewBox);

        fragment->paintWireframe(g);
        for (auto& l : layers)
            l.paintWireframe(g, viewBox);

        g.pop();
    }

    void paintOverlay(Gfx::Canvas& g, Dom::OriginatingElement of, Math::Rectf viewport) {
        ensureSorted();
        g.push();
        fragment->paintOverlay(g, of, viewport);
        for (auto& l : layers)
            l.paintOverlay(g, of, viewport);

        g.pop();
    }

    Opt<Rc<Layout::Fragment>> hitest(Math::Vec2f pos, Math::Rectf viewBox) {
        ensureSorted();

        pos = fragment->intrinsicTransform().inverse().apply(pos);

        if (fragment->style().transform->has())
            pos = resolveTransform(fragment, viewBox).inverse().apply(pos);

        for (auto& l : mutIterRev(layers))
            if (auto hit = l.hitest(pos, viewBox)) {
                return hit;
            }

        if (fragment->borderBox().contains(pos.cast<Au>())) {
            return Some(fragment);
        }

        return NONE;
    }

    // https://drafts.csswg.org/css-overflow-3/#scrollable
    RectAu scrollableOverflow() {
        auto bound = fragment->borderBox();
        for (auto& c : layers)
            bound = bound.mergeWith(c.scrollableOverflow());
        return bound;
    }

    void repr(Io::Emit& e) const {
        e("(stacking-context");
        e.indentNewline();
        for (auto const& c : layers)
            e("{}\n", c);
        e.deindent();
        e(")");
    }
};

} // namespace Vaev::Paint
