module;

#include <karm-image/picture.h>
#include <karm-logger/logger.h>
#include <karm-math/au.h>
#include <vaev-values/length.h>
#include <vaev-values/percent.h>

export module Vaev.Layout:replaced;

import :base;
import :layout;

namespace Vaev::Layout {

struct ReplacedFormatingContext : FormatingContext {

    SVGRootFrag buildSVGFrag(Tree& tree, SVGRoot& root, Karm::Vec2Au position, Karm::Vec2Au size) {
        SVGRootFrag svgFrag = SVGRootFrag::build(root, position, size);

        // https://svgwg.org/svg2-draft/coords.html#Units
        // SPEC: For <percentage> values that are defined to be relative to the size of SVG viewport:
        // the value to use must be the percentage, in user units, of the * parameter of the ‘viewBox’ applied to that
        // viewport. If no ‘viewBox’ is specified, then the value to use must be the percentage, in user units, of the
        // * of the SVG viewport.
        size =
            root.viewBox
                ? Karm::Vec2Au{(Au)root.viewBox->width, (Au)root.viewBox->height}
                : svgFrag.transf.inverse().applyVector(size.cast<f64>()).cast<Au>();

        for (auto& element : root.elements) {
            if (auto shape = element.is<SVG::Shape>()) {
                svgFrag.add(SVG::ShapeFrag::fromShape(*shape, size));
            } else if (auto nestedRoot = element.is<SVGRoot>()) {
                auto resolvedRect = SVG::resolve(SVG::buildRectangle(*nestedRoot->style), size);

                svgFrag.add(buildSVGFrag(tree, *nestedRoot, {resolvedRect.x, resolvedRect.y}, {resolvedRect.width, resolvedRect.height}));
            } else if (auto box = element.is<::Box<Box>>()) {
                auto resolvedRect = SVG::resolve(SVG::buildRectangle(*(*box)->style), size);

                auto frag = Layout::Frag();
                Input input{
                    .fragment = &frag,
                    .knownSize = {resolvedRect.width, resolvedRect.height},
                    .position = {resolvedRect.x, resolvedRect.y},
                };

                layout(tree, *box, input);

                svgFrag.add(makeBox<Frag>(std::move(frag.children()[0])));
            }
        }

        return svgFrag;
    }

    // https://www.w3.org/TR/css-images-3/#default-sizing
    // NOTE: not fully compliant
    Karm::Vec2Au _defaultSizing(Math::Vec2<Opt<Au>> knownSize, Opt<f64> aspectRatio, Vec2Au defaultSize) {
        if (knownSize.x and knownSize.y)
            return {*knownSize.x, *knownSize.y};

        Karm::Vec2Au size = defaultSize;
        if (knownSize.x) {
            size.x = *knownSize.x;
            if (aspectRatio)
                size.y = size.x / Au{*aspectRatio};
        } else if (knownSize.y) {
            size.y = *knownSize.y;
            if (aspectRatio)
                size.x = Au{*aspectRatio} * size.y;
        } else if (aspectRatio) {
            size.y = size.x / Au{*aspectRatio};
        }
        return size;
    }

    Output run(Tree& tree, Box& box, Input input, [[maybe_unused]] usize startAt, [[maybe_unused]] Opt<usize> stopAt) override {
        Vec2Au size = {};

        if (auto image = box.content.is<Karm::Image::Picture>()) {
            size = image->bound().size().cast<Au>();
        } else if (auto svg = box.content.is<SVGRoot>()) {
            fillKnownSizeWithSpecifiedSizeIfEmpty(tree, box, input);

            auto aspectRatio = SVG::intrinsicAspectRatio(box.style->svg->viewBox, box.style->sizing->width, box.style->sizing->height);

            size = _defaultSizing(input.knownSize, aspectRatio, input.containingBlock);

            if (input.fragment) {
                input.fragment->content = buildSVGFrag(tree, *svg, input.position, size);
            }
        } else {
            panic("unsupported replaced content");
        }

        if (tree.fc.allowBreak() and not tree.fc.acceptsFit(
                                         input.position.y,
                                         size.y,
                                         input.pendingVerticalSizes
                                     )) {
            return {
                .size = {},
                .completelyLaidOut = false,
                .breakpoint = Breakpoint::overflow(),
                .firstBaselineSet = BaselinePositionsSet::fromSinglePosition(size.y),
                .lastBaselineSet = BaselinePositionsSet::fromSinglePosition(size.y),
            };
        }

        return {
            .size = size,
            .completelyLaidOut = true,
            .firstBaselineSet = BaselinePositionsSet::fromSinglePosition(size.y),
            .lastBaselineSet = BaselinePositionsSet::fromSinglePosition(size.y),
        };
    }
};

export Rc<FormatingContext> constructReplacedFormatingContext(Box&) {
    return makeRc<ReplacedFormatingContext>();
}

} // namespace Vaev::Layout
