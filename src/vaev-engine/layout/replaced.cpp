export module Vaev.Engine:layout.replaced;

import Karm.Image;
import Karm.Gfx;
import Karm.Math;
import Karm.Logger;
import Karm.Scene;
import Karm.Core;

import :values;
import :layout.base;
import :layout.layout;

namespace Vaev::Layout {

struct ReplacedFormatingContext : FormatingContext {
    Output run(Tree& tree, Box& box, Input input, [[maybe_unused]] usize startAt, [[maybe_unused]] Opt<usize> stopAt) override {
        tree.fc.enterMonolithicBox();
        Defer _ = [&] {
            tree.fc.leaveMonolithicBox();
        };

        Vec2Au size = {};

        if (auto image = box.content.is<Rc<Scene::Node>>()) {
            size = (*image)->bound().size().cast<Au>();
        } else {
            panic("unsupported replaced content");
        }

        if (tree.fc.allowBreak() and
            not tree.fc.acceptsFit(
                input.position.y,
                size.y,
                input.pendingVerticalSizes
            )) {
            return {
                .size = {},
                .completelyLaidOut = false,
                .breakpoint = _Breakpoint::lastResort(),
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
