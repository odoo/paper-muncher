module;

#include <karm-image/picture.h>
#include <karm-math/au.h>

export module Vaev.Layout:replaced;

import :base;
import :layout;

namespace Vaev::Layout {

struct ReplacedFormatingContext : FormatingContext {
    Output run(Tree& tree, Box& box, Input input, [[maybe_unused]] usize startAt, [[maybe_unused]] Opt<usize> stopAt) override {
        Vec2Au size = {};

        if (auto image = box.content.is<Karm::Image::Picture>()) {
            size = image->bound().size().cast<Au>();
        } else if (auto svg = box.content.is<SVGRoot>()) {
            auto aspectRatio = SVG::intrinsicAspectRatio(box.style->svg->viewBox, box.style->sizing->width, box.style->sizing->height);

            if (input.knownSize.x and input.knownSize.y)
                size = {*input.knownSize.x, *input.knownSize.y};
            else if (input.knownSize.x) {
                size.x = *input.knownSize.x;
                if (aspectRatio)
                    size.y = size.x / Au{*aspectRatio};
                else
                    size.y = input.containingBlock.y;
            } else if (input.knownSize.y) {
                size.y = *input.knownSize.y;
                if (aspectRatio)
                    size.x = Au{*aspectRatio} * size.y;
                else
                    size.x = input.containingBlock.x;
            } else {
                // https://svgwg.org/svg2-draft/geometry.html#Sizing
                // FIXME
                size.x = input.containingBlock.x;
                size.y = size.x / Au{*aspectRatio};
            }

            for (auto& foreignObjectBox : svg->foreignObjectBoxes) {
                layout(tree, foreignObjectBox, Input{.fragment = input.fragment});
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
                .firstBaselineSet = size.y,
                .lastBaselineSet = size.y,
            };
        }

        return {
            .size = size,
            .completelyLaidOut = true,
            .firstBaselineSet = size.y,
            .lastBaselineSet = size.y,
        };
    }
};

export Rc<FormatingContext> constructReplacedFormatingContext(Box&) {
    return makeRc<ReplacedFormatingContext>();
}

} // namespace Vaev::Layout
