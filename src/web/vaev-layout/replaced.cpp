module;

#include <karm-image/picture.h>
#include <karm-logger/logger.h>
#include <karm-math/au.h>
#include <karm-scene/base.h>
#include <vaev-dom/tags.h>

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
            // https://svgwg.org/svg2-draft/geometry.html#Sizing
            // if the size isnt define, it is auto or something else; something else is not accepcted, so it is auto
            // auto is 100% os parent
            // SPEC: The value auto for width and height on the ‘svg’ element is treated as 100%.
            // if (input.knownSize.x and input.knownSize.y) {
            //     size = {
            //         input.knownSize.x.unwrap(),
            //         input.knownSize.y.unwrap(),
            //     };
            // } else {
            // logDebug("xxx: {}", svg->viewBox);
            if (svg->viewBox) {
                size = {
                    Au{svg->viewBox->width},
                    Au{svg->viewBox->height}
                };
            } else if(not input.knownSize.x){
                size = input.containingBlock;
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
                .breakpoint = Breakpoint::overflow()
            };
        }

        return {
            .size = size,
            .completelyLaidOut = true,
        };
    }
};

export Rc<FormatingContext> constructReplacedFormatingContext(Box&) {
    return makeRc<ReplacedFormatingContext>();
}

} // namespace Vaev::Layout
