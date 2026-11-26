module;

#include <karm/macros>

export module Vaev.Engine:layout2.block;

import Karm.Logger;

import :layout2.layout;
import :layout2.box;
import :layout2.fragBuilder;
import :layout.layout;

namespace Vaev::Layout2 {

export struct BlockFormattingContext {
    static IntrinsicSizes intrinsicSizes(Layout::Box const& box) {
        auto sizes = IntrinsicSizes{};

        for (auto const& child : box.children()) {
            auto contributions = intrinsicInlineContributions(child);
            sizes.minContentSize = max(sizes.minContentSize, contributions.min);
            sizes.maxContentSize = max(sizes.maxContentSize, contributions.max);
        }

        return sizes;
    }

    // https://www.w3.org/TR/CSS22/visudet.html#Computing_widths_and_margins
    static Vec2Au solveInlineMargins(Opt<Au> inlineSize, Au inlineCb, Opt<Au> inlineStartMargin, Opt<Au> inlineEndMargin) {
        // NOTE: inlineSize := 'border-left-width' + 'padding-left' + 'width' + 'padding-right' + 'border-right-width'

        // The following constraints must hold among the used values of the other properties:
        // 'margin-left' + 'border-left-width' + 'padding-left' + 'width' + 'padding-right' + 'border-right-width' + 'margin-right'
        // = width of containing block

        // If all of the above have a computed value other than 'auto', the values are said to be "over-constrained" and one
        // of the used values will have to be different from its computed value.
        if (inlineSize and inlineStartMargin and inlineEndMargin) {
            // TODO: Only ltr is implemented for now.
            // If the 'direction' property of the containing block has the value 'ltr', the specified value of 'margin-right' is
            // ignored and the value is calculated so as to make the equality true.
            // If the value of 'direction' is 'rtl', this happens to 'margin-left' instead.
            return {
                inlineCb - *inlineSize - *inlineEndMargin,
                *inlineEndMargin,
            };
        }

        // If 'width' is not 'auto' and 'border-left-width' + 'padding-left' + 'width' + 'padding-right' + 'border-right-width' (plus
        // any of 'margin-left' or 'margin-right' that are not 'auto') is larger than the width of the containing block,
        if (inlineSize and *inlineSize + inlineStartMargin.unwrapOr(0_au) + inlineEndMargin.unwrapOr(0_au) > inlineCb) {
            // then any 'auto' values for 'margin-left' or 'margin-right' are, for the following rules, treated as zero.
            return {
                inlineStartMargin.unwrapOr(0_au),
                inlineEndMargin.unwrapOr(0_au),
            };
        }

        // If 'width' is set to 'auto', any other 'auto' values become '0' and 'width' follows from the resulting equality.
        if (not inlineSize) {
            return {
                inlineStartMargin.unwrapOr(0_au),
                inlineEndMargin.unwrapOr(0_au),
            };
        }

        // If both 'margin-left' and 'margin-right' are 'auto', their used values are equal. This horizontally centers the element
        // with respect to the edges of the containing block.
        if (not inlineStartMargin and not inlineEndMargin) {
            return {(inlineCb - *inlineSize) / 2};
        }

        // If there is exactly one value specified as 'auto', its used value follows from the equality.
        if (not inlineStartMargin) {
            return {
                inlineCb - *inlineSize - *inlineEndMargin,
                *inlineEndMargin,
            };
        } else if (inlineEndMargin) {
            return {
                *inlineStartMargin,
                inlineCb - *inlineSize - *inlineStartMargin,
            };
        }

        return {
            *inlineStartMargin,
            *inlineEndMargin,
        };
    }

    Output run(Layout::Box& box, Input const& input) {
        auto const& [constraints, metrics, opportunity, continuation] = input;

        auto fragBuilder = FragBuilder{box};

        auto inlineSizeBehavior = input.constraints.inlineAutoSizeBehavior;

        Au autoInlineSize = 0_au;
        if (inlineSizeBehavior == AutoSizeBehavior::STRETCH_FIT and constraints.containingBlock.inline_) {
            // https://www.w3.org/TR/css-sizing-3/#stretch-fit-sizing
            auto [inlineStartMargin, inlineEndMargin] = resolveInlineMargins(*box.style, constraints.containingBlock);
            auto inlineMargins = inlineStartMargin.unwrapOr(0_au) + inlineEndMargin.unwrapOr(0_au);
            autoInlineSize = max(metrics.borders.inlineSum() + metrics.paddings.inlineSum(), *constraints.containingBlock.inline_ - inlineMargins);
            autoInlineSize = clamp(autoInlineSize, metrics.minSize.inline_.unwrapOr(0_au), metrics.maxSize.inline_.unwrapOr(Limits<Au>::MAX));
        } else {
            autoInlineSize = metrics.borders.inlineSum() + metrics.paddings.inlineSum();
        }

        PendingMargin pendingMargin = constraints.pendingMargin;

        if (constraints.truncateBlockStartMargin) {
            pendingMargin = {};
        } else {
            // FIXME: Is this correct?
            pendingMargin.add(constraints.margins.blockStart.unwrapOr(0_au));
        }

        Opt<Au> blockOffset = NONE;

        auto initialBreakBefore = box.style->break_->before;

        Opt<BreakOpportunity> bestBreakOpportunity = NONE;

        auto overrideBestBreakOpportunityIfBetter = [&](BreakOpportunity&& other) {
            if (auto& [old] = bestBreakOpportunity) {
                if (other.appeal >= old.appeal) {
                    logInfo("New best break opportunity: {}", other);
                    bestBreakOpportunity = Some(std::move(other));
                }
            } else {
                logInfo("New best break opportunity: {}", other);
                bestBreakOpportunity = Some(std::move(other));
            }
        };

        if (not constraints.collapseMargins or box.establishesFc or metrics.paddings.blockStart != 0_au or metrics.borders.blockStart != 0_au) {
            blockOffset = Some(pendingMargin.sum());
            pendingMargin = PendingMargin{};
        }

        // FIXME: Express in logical units instead
        Vec2Au cursor = {
            metrics.borders.inlineStart + metrics.paddings.inlineStart,
            metrics.borders.blockStart + metrics.paddings.blockStart
        };

        Style::BreakBetween previousBreakAfter = Keywords::AUTO;

        usize start = 0;
        if (continuation) {
            start = continuation->_inner.is<BreakNode::BlockResumeData>()->startChildIndex;
        }

        usize end = box.children().len();
        if (opportunity)
            end = opportunity->inner ? opportunity->index + 1 : opportunity->index;

        auto childCbInline = input.metrics.size.inline_.unwrapOr(autoInlineSize) -
                             metrics.borders.inlineSum() -
                             metrics.paddings.inlineSum();

        for (usize i = start; i < end; i++) {
            auto& child = box.children()[i];

            if (child.isRemovedFromFlow())
                continue;

            auto childSpecifiedMargins = resolveMargins(*child.style, {Some(childCbInline), NONE});

            Opt<Fragmentainer> childFragmentainer = NONE;

            if (auto const& [fragmentainer] = constraints.fragmentainer) {
                childFragmentainer = Some(fragmentainer.at(blockOffset.unwrapOr(0_au) + cursor.y));
            }

            auto const childConstraints = Constraints{
                .containingBlock = {
                    Some(childCbInline),
                    NONE,
                },
                .pendingMargin = pendingMargin,
                .margins = childSpecifiedMargins,
                .truncateBlockStartMargin = continuation and not continuation->preservedMargin and isEmpty(fragBuilder._children),
                .fragmentainer = childFragmentainer,
            };

            Opt<BreakNode const&> childContinuation = NONE;
            if (continuation and not isEmpty(continuation->_children)) {
                childContinuation = Some(continuation->_children[0]);
            }

            // FIXME: Rename bestOpportunity
            Opt<BreakOpportunity const&> childOpportunity = NONE;
            if (opportunity and opportunity->inner and i == opportunity->index)
                childOpportunity = Some(**opportunity->inner);

            auto childOutput = layout(
                child,
                childConstraints,
                childOpportunity,
                childContinuation
            );

            Opt<BreakNode> breakTree = NONE;

            auto placed = childOutput.visit(
                [](Placed& placed) -> Placed {
                    return std::move(placed);
                },
                [&](NeedsEarlierBreak& needsEarlierBreak) -> Placed {
                    logInfo("Relayout: {}", needsEarlierBreak.opportunity);

                    auto relayoutOutput = layout(
                        child,
                        childConstraints,
                        Some(needsEarlierBreak.opportunity)
                    );

                    return relayoutOutput.take<Placed>();
                }
            );

            if (isEmpty(fragBuilder._children)) {
                initialBreakBefore = max(initialBreakBefore, placed.initialBreakBefore);
            }

            auto forcedBreak = [](Style::BreakBetween b, Fragmentainer const& f) {
                if (b == Keywords::REGION) {
                    return f.hasContext(Fragmentainer::Type::REGION);
                } else if (b == Keywords::COLUMN) {
                    return f.hasContext(Fragmentainer::Type::COLUMN);
                } else if (oneOf(b, Keywords::PAGE, Keywords::LEFT, Keywords::RIGHT, Keywords::VERSO)) {
                    return f.hasContext(Fragmentainer::Type::PAGE);
                } else {
                    return false;
                }
            };

            // FIXME: Inaccurate tie breaking model.
            auto maybeForcedBreak = max(placed.initialBreakBefore, previousBreakAfter);

            if (constraints.fragmentainer and not isEmpty(fragBuilder._children) and forcedBreak(maybeForcedBreak, *constraints.fragmentainer)) {
                return Placed{
                    .fragment = fragBuilder.buildBox(input, {autoInlineSize, cursor.height}),
                    .blockOffset = blockOffset,
                    .initialBreakBefore = initialBreakBefore,
                    .breakState = Broke{
                        .tree = BreakNode{
                            // https://www.w3.org/TR/css-break-3/#break-margins
                            // When a forced break occurs there, adjoining margins before the break are truncated,
                            // but margins after the break are preserved.
                            .preservedMargin = true,
                            ._inner = BreakNode::BlockResumeData{
                                .startChildIndex = i,
                            },
                            ._children = {},
                        },
                        .appeal = BreakAppeal::PERFECT,
                        .originatingBreak = maybeForcedBreak,
                        .forced = true,
                    },
                };
            }

            if (auto const& [childOffset] = placed.blockOffset) {
                if (not blockOffset) {
                    blockOffset = Some(childOffset);
                } else {
                    cursor.y += childOffset;
                }
            }

            // FIXME: Unsure that is handles all the edge cases.
            // Can the parent break before the child?
            if (constraints.fragmentainer and not isEmpty(fragBuilder._children)) {
                // TODO: Support other fragmentainers
                auto between = max(previousBreakAfter, placed.initialBreakBefore);

                auto appeal = BreakAppeal::PERFECT;
                if (oneOf(between, Keywords::AVOID, Keywords::AVOID_PAGE)) {
                    appeal = BreakAppeal::DROPS_AVOID;
                }

                overrideBestBreakOpportunityIfBetter(BreakOpportunity{
                    .inner = NONE,
                    .index = i,
                    .consumedBlockSize = 0_au, // FIXME: Is this really useful?
                    .originatingBreak = between,
                    .appeal = appeal,
                });

                if (constraints.fragmentainer->wouldOverflow(blockOffset.unwrapOr(0_au) + cursor.y)) {
                    if (bestBreakOpportunity and bestBreakOpportunity->appeal > appeal) {
                        return NeedsEarlierBreak{.opportunity = bestBreakOpportunity.take()};
                    }

                    auto size = LogicalSize{
                        input.metrics.size.inline_.unwrapOr(autoInlineSize + metrics.paddings.inlineEnd + metrics.borders.inlineEnd),
                        input.metrics.size.block.unwrapOr(cursor.height + metrics.paddings.blockEnd + metrics.borders.blockEnd),
                    };

                    logInfo("[{i}] Broke because of overflow with appeal={}", i, appeal);

                    return Placed{
                        .fragment = fragBuilder.buildBox(input, size),
                        .blockOffset = blockOffset,
                        .initialBreakBefore = initialBreakBefore,
                        .breakState = Broke{
                            .tree = BreakNode{
                                // https://www.w3.org/TR/css-break-3/#break-margins
                                // When a forced break occurs there, adjoining margins before the break are truncated,
                                // but margins after the break are preserved.
                                .preservedMargin = false,
                                ._inner = BreakNode::BlockResumeData{
                                    .startChildIndex = i,
                                },
                                ._children = {},
                            },
                            .appeal = appeal,
                            .originatingBreak = between,
                            .forced = false,
                        }
                    };
                }
            }

            auto childSize = LogicalSize<Au>::fromPhysical(placed.fragment->borderBox().size(), box.style->writingMode);

            auto inlineMargins = solveInlineMargins(
                Some(childSize.inline_),
                // FIXME: This doesn't look correct.
                childCbInline,
                childSpecifiedMargins.inlineStart,
                childSpecifiedMargins.inlineEnd
            );

            logInfo("inlineMargins: {}", inlineMargins);

            LogicalInsets usedMargins = {
                .inlineStart = inlineMargins.x,
                .inlineEnd = inlineMargins.y,
                .blockStart = childSpecifiedMargins.blockStart.unwrapOr(0_au),
                .blockEnd = childSpecifiedMargins.blockEnd.unwrapOr(0_au),
            };

            fragBuilder.addChild(placed.fragment, cursor + Vec2Au{usedMargins.inlineStart, 0_au}, usedMargins);

            if (auto [completed] = placed.breakState.is<Completed>()) {
                previousBreakAfter = completed.finalBreakAfter;
                pendingMargin = completed.pendingMargin;

                if (completed.bestBreakOpportunity) {
                    auto originatingBreak = completed.bestBreakOpportunity->originatingBreak;
                    auto appeal = completed.bestBreakOpportunity->appeal;

                    overrideBestBreakOpportunityIfBetter(BreakOpportunity{
                        .inner = Some(makeBox<BreakOpportunity>(completed.bestBreakOpportunity.take())),
                        .index = i,
                        .consumedBlockSize = 0_au, // TODO
                        .originatingBreak = originatingBreak,
                        .appeal = appeal,
                    });
                }
            }

            if (auto broke = placed.breakState.is<Broke>()) {
                if (bestBreakOpportunity and broke->appeal < bestBreakOpportunity->appeal) {
                    logInfo("[{}] Returned NeedsEarlierBreak", i);
                    return NeedsEarlierBreak{
                        .opportunity = bestBreakOpportunity.take(),
                    };
                }

                auto b = std::move(broke->tree);
                auto c = Vec<BreakNode>{};
                c.pushBack(std::move(b));

                logInfo("[{i}] Bubbling Placed::Broke", i);
                return Placed{
                    .fragment = fragBuilder.buildBox(input, {autoInlineSize, cursor.height}),
                    .blockOffset = blockOffset,
                    .initialBreakBefore = initialBreakBefore,
                    .breakState = Broke{
                        .tree = BreakNode{
                            // https://www.w3.org/TR/css-break-3/#break-margins
                            // When a forced break occurs there, adjoining margins before the break are truncated,
                            // but margins after the break are preserved.
                            .preservedMargin = broke->forced,
                            ._inner = BreakNode::BlockResumeData{
                                .startChildIndex = i,
                            },
                            ._children = std::move(c),
                        },
                        .appeal = broke->appeal,
                        .originatingBreak = broke->originatingBreak,
                        .forced = broke->forced,
                    },
                };
            }

            cursor.y += childSize.block;

            // FIXME: Rework and bring back in.
            // if (inlineSizeBehavior == AutoSizeBehavior::FIT_CONTENT or not constraints.containingBlock.inline_) {
            //     autoInlineSize = max(autoInlineSize, childOutput.size.inline_);
            // }
        }

        if (opportunity and not opportunity->inner) {
            auto size = LogicalSize{
                input.metrics.size.inline_.unwrapOr(autoInlineSize + metrics.paddings.inlineEnd + metrics.borders.inlineEnd),
                input.metrics.size.block.unwrapOr(cursor.height + metrics.paddings.blockEnd + metrics.borders.blockEnd),
            };

            logInfo("Broke at chosen opportunity {}", *opportunity);

            return Placed{
                .fragment = fragBuilder.buildBox(input, size),
                .blockOffset = blockOffset,
                .initialBreakBefore = initialBreakBefore,
                .breakState = Broke{
                    .tree = BreakNode{
                        .preservedMargin = false,
                        ._inner = BreakNode::BlockResumeData{
                            .startChildIndex = opportunity->index,
                        },
                        ._children = {},
                    },
                    .appeal = opportunity->appeal,
                    .originatingBreak = opportunity->originatingBreak,
                    .forced = false,
                },
            };
        }

        // FIXME: Check for fixed height too.
        if (blockOffset and (box.establishesFc or metrics.paddings.blockEnd != 0_au or metrics.borders.blockEnd != 0_au)) {
            cursor.y += pendingMargin.sum();
            pendingMargin = PendingMargin{};
        }

        auto finalBreakAfter = max(previousBreakAfter, box.style->break_->after);

        auto size = LogicalSize{
            input.metrics.size.inline_.unwrapOr(autoInlineSize + metrics.paddings.inlineEnd + metrics.borders.inlineEnd),
            input.metrics.size.block.unwrapOr(cursor.height + metrics.paddings.blockEnd + metrics.borders.blockEnd),
        };

        if (not blockOffset and size.block > 0_au) {
            blockOffset = Some(pendingMargin.sum());
            pendingMargin = PendingMargin{};
        }

        // FIXME: Are we allowed to unwrap?
        pendingMargin.add(constraints.margins.blockEnd.unwrapOr(0_au));

        logInfo("Returned Placed::Completed {}", size);

        return Placed{
            .fragment = fragBuilder.buildBox(input, size),
            .blockOffset = blockOffset,
            .initialBreakBefore = initialBreakBefore,
            .breakState = Completed{
                .pendingMargin = pendingMargin,
                .finalBreakAfter = finalBreakAfter,
                .bestBreakOpportunity = std::move(bestBreakOpportunity),
            }
        };
    }
};

} // namespace Vaev::Layout2
