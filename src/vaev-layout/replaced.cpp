module;

#include <karm-image/picture.h>
#include <karm-math/au.h>

export module Vaev.Layout:replaced;

import :base;

namespace Vaev::Layout {

struct ReplacedFormatingContext : FormatingContext {
    Output run(Tree& tree, Box& box, Input input, [[maybe_unused]] usize startAt, [[maybe_unused]] Opt<usize> stopAt) override {
        Vec2Au size = {};

        if (auto image = box.content.is<Karm::Image::Picture>()) {
            size = image->bound().size().cast<Au>();
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
