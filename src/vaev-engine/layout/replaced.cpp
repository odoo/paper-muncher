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
    IntrinsicSizes intrinsicSizes(Tree&, Box& box, Input) {
        if (auto image = box.content.is<Rc<Scene::Node>>()) {
            auto inlineSize = inlineSizeOf((*image)->bound().size().cast<Au>(), box.style->writingMode);

            return {
                .minContentSize = inlineSize,
                .maxContentSize = inlineSize,
            };
        } else {
            panic("unsupported replaced content");
        }
    }

    Output run(Tree& tree, Box& box, Input input, [[maybe_unused]] usize startAt, [[maybe_unused]] Opt<usize> stopAt) override {
        tree.fc.enterMonolithicBox();
        Defer _ = [&] {
            tree.fc.leaveMonolithicBox();
        };

        Vec2Au size = {};

        if (auto image = box.content.is<Rc<Scene::Node>>()) {
            auto naturalSize = (*image)->bound().size().cast<Au>();

            auto naturalDimensions = ObjectNaturalDimensions{
                .size = {naturalSize.width, naturalSize.height},
                .aspectRatio = naturalSize.width / naturalSize.height,
            };

            auto inlineSize = resolveInlineLength(tree, box, input.containingBlock, [&](IntrinsicSize) -> Opt<Au> {
                return naturalDimensions.size.width;
            });

            auto blockSize = resolveBlockLength(tree, box, input.containingBlock, [&](IntrinsicSize) -> Opt<Au> {
                return naturalDimensions.size.height;
            });

            auto specifiedSize = Math::Vec2{inlineSize, blockSize};

            auto tentativeSize = resolveObjectDefaultSizing(naturalDimensions, specifiedSize);
            size = applyReplacedSizeConstraints(tree, box, tentativeSize, input.containingBlock, specifiedSize);
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
                .breakpoint = Breakpoint::overflow(),
                .firstBaselineSet = BaselinePositionsSet::fromSinglePosition(size.y),
                .lastBaselineSet = BaselinePositionsSet::fromSinglePosition(size.y),
            };
        }

        return {
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
