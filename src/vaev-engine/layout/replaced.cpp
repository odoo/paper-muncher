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
        auto fragBuilder = FragmentBuilder{tree, box};

        tree.fc.enterMonolithicBox();
        Defer _ = [&] {
            tree.fc.leaveMonolithicBox();
        };

        Vec2Au size = {};

        if (auto image = box.content.is<Rc<Scene::Node>>()) {
            auto naturalSize = (*image)->bound().size().cast<Au>();
            if (input.intrinsic == IntrinsicSize::AUTO) {
                auto naturalDimensions = ObjectNaturalDimensions{
                    .size = {naturalSize.width, naturalSize.height},
                    .aspectRatio = naturalSize.width / naturalSize.height,
                };

                auto specifiedSize = resolvePreferredSize(tree, box, input.containingBlock);
                auto tentativeSize = resolveObjectDefaultSizing(naturalDimensions, specifiedSize);
                size = applyReplacedMinMaxSizeConstraints(tree, box, tentativeSize, input.containingBlock, specifiedSize);
            } else {
                size = naturalSize;
            }
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
                .fragment = fragBuilder.buildBoxFromInput(input, {}),
                .size = {},
                .completelyLaidOut = false,
                .breakpoint = Breakpoint::overflow(),
                .firstBaselineSet = BaselinePositionsSet::fromSinglePosition(size.y),
                .lastBaselineSet = BaselinePositionsSet::fromSinglePosition(size.y),
            };
        }

        return {
            .fragment = fragBuilder.buildBoxFromInput(input, size),
            .size = size,
            .completelyLaidOut = true,
            .breakpoint = Breakpoint::bottomOfMonolithicBox(box),
            .firstBaselineSet = BaselinePositionsSet::fromSinglePosition(size.y),
            .lastBaselineSet = BaselinePositionsSet::fromSinglePosition(size.y),
        };
    }
};

export Rc<FormatingContext> constructReplacedFormatingContext(Box&) {
    return makeRc<ReplacedFormatingContext>();
}

} // namespace Vaev::Layout
