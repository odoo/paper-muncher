#include "flex.h"

#include "frag.h"
#include "values.h"

namespace Vaev::Layout {

struct FlexItem {
    Frag *frag;
    Flex flex;

    // these 2 sizes do NOT account margins
    Vec2Px usedSize;
    Math::Insets<Opt<Px>> margin;

    // position relative to its flex line
    Vec2Px position;

    Px flexBaseSize, hypoMainSize;

    Vec2Px speculativeSize;
    InsetsPx speculativeMargin;

    Vec2Px minContentSize, maxContentSize;
    InsetsPx minContentMargin, maxContentMargin;

    void commit() {
        frag->layout.margin.top = margin.top.unwrapOr(speculativeMargin.top);
        frag->layout.margin.start = margin.start.unwrapOr(speculativeMargin.start);
        frag->layout.margin.end = margin.end.unwrapOr(speculativeMargin.end);
        frag->layout.margin.bottom = margin.bottom.unwrapOr(speculativeMargin.bottom);
        frag->layout.position = position;
    }

    void computeContentSizes(Tree &t) {
        _speculateValues(t, {
                                .commit = Commit::NO,
                                .intrinsic = {IntrinsicSize::MAX_CONTENT, IntrinsicSize::AUTO},
                                .knownSize = {NONE, NONE},
                            },
                         maxContentSize, maxContentMargin);
        _speculateValues(t, {
                                .commit = Commit::NO,
                                .intrinsic = {IntrinsicSize::MIN_CONTENT, IntrinsicSize::AUTO},
                                .knownSize = {NONE, NONE},
                            },
                         minContentSize, minContentMargin);
    }

    // FIXME: bad way of defining enum?
    enum OUTER_POSITIONS {
        TOP,
        START,
        END,
        BOTTOM,
        HORIZONTAL,
        VERTICAL
    };

    Px getMargin(OUTER_POSITIONS position) const {
        // FIXME: when should we consider borders and when we shouldnt?
        switch (position) {
        case TOP:
            return margin.top.unwrapOr(speculativeMargin.top);
        case START:
            return margin.start.unwrapOr(speculativeMargin.start);
        case END:
            return margin.end.unwrapOr(speculativeMargin.end);
        case BOTTOM:
            return margin.bottom.unwrapOr(speculativeMargin.bottom);
        case HORIZONTAL:
            return margin.start.unwrapOr(speculativeMargin.start) + margin.end.unwrapOr(speculativeMargin.end);
        case VERTICAL:
            return margin.top.unwrapOr(speculativeMargin.top) + margin.bottom.unwrapOr(speculativeMargin.bottom);
        }
    }

    bool hasAnyCrossMarginAuto() const {
        return (frag->style->margin->top == Width::Type::AUTO) or (frag->style->margin->bottom == Width::Type::AUTO);
    }

    FlexItem(Tree &t, Frag *f) : frag(f), flex(*f->style->flex) {
        speculateValues(t, Input{Commit::NO});
        // TODO: not always we will need min/max content sizes, this can be lazy computed for performance gains
        computeContentSizes(t);
    }

    Px getScaledFlexShrinkFactor() const {
        return flexBaseSize * Px{flex.shrink};
    }

    void _speculateValues(Tree &t, Input input, Vec2Px &speculativeSize, InsetsPx &speculativeMargin) {
        Output out = layout(t, *frag, input);
        speculativeSize = out.size;
        speculativeMargin = computeMargins(t, *frag, Input{.commit = Commit::NO, .containingBlock = speculativeSize});
    }

    void speculateValues(Tree &t, Input input) {
        _speculateValues(t, input, speculativeSize, speculativeMargin);
    }

    void computeFlexBaseSize(Tree &t, Frag &f, Px mainContainerSize) {
        // TODO: check specs
        if (flex.basis.type == FlexBasis::WIDTH) {
            if (flex.basis.width.type == Width::Type::VALUE) {
                flexBaseSize = resolve(t, f, flex.basis.width.value, mainContainerSize);
            } else if (flex.basis.width.type == Width::Type::AUTO) {
                flexBaseSize = speculativeSize.x;
            }
        }

        if (flex.basis.type == FlexBasis::Type::CONTENT and
            frag->style->sizing->height.type == Size::Type::LENGTH /* and
            intrinsic aspect ratio*/
        ) {
            // TODO: placehold value, check specs
            Px aspectRatio{1};
            auto crossSize = resolve(t, f, frag->style->sizing->height.value, Px{0});
            flexBaseSize = (crossSize)*aspectRatio;
        }

        if (false) {
            // TODO: other flex base size cases
            logWarn("not implemented flex base size case");
        }
    }

    void computeHypotheticalMainSize(Tree &t, Vec2Px containerSize) {
        hypoMainSize = clamp(
            flexBaseSize,
            getMinMaxPrefferedSize(t, true, true, containerSize),
            getMinMaxPrefferedSize(t, true, false, containerSize)
        );
        hypoMainSize = flexBaseSize;
    }

    Px getMinMaxPrefferedSize(Tree &t, bool isWidth, bool isMin, Vec2Px containerSize) const {
        Size sizeToResolve;
        if (isWidth and isMin)
            sizeToResolve = frag->style->sizing->minSize(Axis::HORIZONTAL);
        else if (isWidth and not isMin)
            sizeToResolve = frag->style->sizing->maxSize(Axis::HORIZONTAL);
        else if (not isWidth and isMin)
            sizeToResolve = frag->style->sizing->minSize(Axis::VERTICAL);
        else
            sizeToResolve = frag->style->sizing->maxSize(Axis::VERTICAL);

        switch (sizeToResolve.type) {
        case Size::LENGTH:
            return resolve(t, *frag, sizeToResolve.value, isWidth ? containerSize.x : containerSize.y);
        case Size::MIN_CONTENT:
            return isWidth ? minContentSize.x : minContentSize.y;
        case Size::MAX_CONTENT:
            return isWidth ? maxContentSize.x : maxContentSize.y;
        case Size::FIT_CONTENT:
            logWarn("not implemented");
        case Size::AUTO:
            return isWidth ? speculativeSize.x : speculativeSize.y;
        case Size::NONE:
            if (isMin)
                panic("NONE is an invalid value for min-width");
            return Limits<Px>::MAX;
        }
    }

    Px getMainSizeMinMaxContentContribution(Tree &t, bool isMin, Vec2Px containerSize) {
        Px contentContribution;
        if (isMin)
            contentContribution = minContentSize.x + minContentMargin.horizontal();
        else
            contentContribution = maxContentSize.x + maxContentMargin.horizontal();

        if (frag->style->sizing->width.type == Size::Type::LENGTH) {
            contentContribution = max(
                contentContribution,
                resolve(t, *frag, frag->style->sizing->width.value, containerSize.x)
            );
        } else if (frag->style->sizing->width.type == Size::Type::MIN_CONTENT) {
            contentContribution = max(contentContribution, minContentSize.x + minContentMargin.horizontal());
        } else if (frag->style->sizing->width.type == Size::Type::AUTO and not isMin) {
            contentContribution = speculativeSize.x;
        } else {
            logWarn("not implemented");
        }

        if (flex.grow == 0)
            contentContribution = min(contentContribution, flexBaseSize);

        if (flex.shrink == 0)
            contentContribution = max(contentContribution, flexBaseSize);

        return clamp(
            contentContribution,
            getMinMaxPrefferedSize(t, true, true, containerSize),
            getMinMaxPrefferedSize(t, true, false, containerSize)
        );
    }

    void alignCrossFlexStart() {
        if (not hasAnyCrossMarginAuto()) {
            position.y = getMargin(FlexItem::TOP);
        }
    }

    void alignCrossFlexEnd(Px lineCrossSize) {
        if (not hasAnyCrossMarginAuto()) {
            position.y = lineCrossSize - usedSize.y - getMargin(FlexItem::BOTTOM);
        }
    }

    void alignCrossCenter(Px lineCrossSize) {
        if (not hasAnyCrossMarginAuto()) {
            Px startOfBlock = (lineCrossSize - usedSize.y - getMargin(FlexItem::VERTICAL)) / Px{2};
            position.y = startOfBlock + getMargin(FlexItem::TOP);
        }
    }

    void alignCrossStretch(Tree &tree, Commit commit, Px lineCrossSize) {
        if (
            frag->style->sizing->height.type == Size::Type::AUTO and
            frag->style->margin->bottom != Width::AUTO and
            frag->style->margin->top != Width::AUTO
        ) {
            /* Its used value is the length necessary to make the cross size of the item’s margin box as close to
            the same size as the line as possible, while still respecting the constraints imposed by
            min-height/min-width/max-height/max-width.*/

            speculateValues(tree, Input{
                                      .commit = commit,
                                      .knownSize = {usedSize.x, lineCrossSize - getMargin(FlexItem::VERTICAL)},
                                      // TODO: not really sure of these arguments, check specs
                                      .availableSpace = {usedSize.x, lineCrossSize - getMargin(FlexItem::VERTICAL)},
                                  });

            position.y = getMargin(FlexItem::TOP);
        }
    }

    void alignItem(Tree &tree, Commit commit, Px lineCrossSize, Style::Align::Keywords parentAlignItems) {
        auto align = frag->style->aligns.alignSelf.keyword;

        if (align == Style::Align::AUTO)
            align = parentAlignItems;

        switch (align) {
        case Style::Align::FLEX_END:
            alignCrossFlexEnd(lineCrossSize);
            return;

        case Style::Align::CENTER:
            alignCrossCenter(lineCrossSize);
            return;

        case Style::Align::FIRST_BASELINE:
            // TODO: complex case, ignoring for now
            return;

        case Style::Align::STRETCH:
            alignCrossStretch(tree, commit, lineCrossSize);
            return;

        default:
        case Style::Align::FLEX_START:
            alignCrossFlexStart();
            return;
        }
    }
};

struct FlexLine {
    MutSlice<FlexItem> items;
    Px crossSize;
    Vec2Px position;

    FlexLine(MutSlice<FlexItem> items) : items(items), crossSize(0) {}

    void alignMainFlexStart() {
        Px currPositionX{0};
        for (auto &flexItem : items) {
            flexItem.position.x = currPositionX + flexItem.getMargin(FlexItem::START);
            currPositionX += flexItem.usedSize.x + flexItem.getMargin(FlexItem::HORIZONTAL);
        }
    }

    void alignMainFlexEnd(Px mainSize, Px occupiedSize) {
        Px currPositionX{mainSize - occupiedSize};
        for (auto &flexItem : items) {
            flexItem.position.x = currPositionX + flexItem.getMargin(FlexItem::START);
            currPositionX += flexItem.usedSize.x + flexItem.getMargin(FlexItem::HORIZONTAL);
        }
    }

    void alignMainSpaceAround(Px mainSize, Px occupiedSize) {
        Px gapSize = (mainSize - occupiedSize) / Px{items.len()};

        Px currPositionX{gapSize / Px{2}};
        for (auto &flexItem : items) {
            flexItem.position.x = currPositionX + flexItem.getMargin(FlexItem::START);
            currPositionX += flexItem.usedSize.x + flexItem.getMargin(FlexItem::HORIZONTAL) + gapSize;
        }
    }

    void alignMainSpaceBetween(Px mainSize, Px occupiedSize) {
        Px gapSize = (mainSize - occupiedSize) / Px{items.len() - 1};

        Px currPositionX{0};
        for (auto &flexItem : items) {
            flexItem.position.x = currPositionX + flexItem.getMargin(FlexItem::START);
            currPositionX += flexItem.usedSize.x + flexItem.getMargin(FlexItem::HORIZONTAL) + gapSize;
        }
    }

    void alignMainCenter(Px mainSize, Px occupiedSize) {
        Px currPositionX{(mainSize - occupiedSize) / Px{2}};
        for (auto &flexItem : items) {
            flexItem.position.x = currPositionX + flexItem.getMargin(FlexItem::START);
            currPositionX += flexItem.usedSize.x + flexItem.getMargin(FlexItem::HORIZONTAL);
        }
    }

    void justifyContent(Style::Align::Keywords justifyContent, Px mainSize, Px occupiedSize) {
        switch (justifyContent) {
        case Style::Align::SPACE_AROUND:
            if (occupiedSize > mainSize or items.len() == 1)
                alignMainCenter(mainSize, occupiedSize);
            else
                alignMainSpaceAround(mainSize, occupiedSize);
            return;

        case Style::Align::CENTER:
            alignMainCenter(mainSize, occupiedSize);
            return;

        case Style::Align::FLEX_END:
            alignMainFlexEnd(mainSize, occupiedSize);
            return;

        case Style::Align::SPACE_BETWEEN:
            alignMainSpaceBetween(mainSize, occupiedSize);
            return;

        case Style::Align::FLEX_START:
        default:
            alignMainFlexStart();
            return;
        }
    }
};

// FIXME: auto, min and max content values for flex container dimensions are not working as in Chrome; add tests
Output flexLayout(Tree &t, Frag &f, Input input) {
    // https://www.w3.org/TR/css-flexbox-1/#layout-algorithm
    Flex const &flexContainer = *f.style->flex;
    bool isRowOriented = flexContainer.direction == FlexDirection::ROW or flexContainer.direction == FlexDirection::ROW_REVERSE;

    // 1. Generate anonymous flex items
    Vec<FlexItem> flexItems{f.children().len()};
    for (auto &c : f.children()) {
        flexItems.pushBack(FlexItem{t, &c});
    }

    // 2. Determine the available main and cross space for the flex items.
    // TODO: Consider refactor orientation after checking karm-math/Flow.h
    Px availableMainSpace, initiallyAvailableCrossSpace;
    if (isRowOriented) {
        availableMainSpace = input.knownSize.x.unwrapOr(input.availableSpace.x);
        initiallyAvailableCrossSpace = input.knownSize.y.unwrapOr(input.availableSpace.y);
    } else {
        // TODO: implement other orientation cases
        logWarn("column orientation or reverse-row still not implemented");
        availableMainSpace = input.knownSize.y.unwrapOr(input.availableSpace.y);
        initiallyAvailableCrossSpace = input.knownSize.x.unwrapOr(input.availableSpace.x);
    }

    // 3. Determine the flex base size and hypothetical main size of each item
    for (auto &flexItem : flexItems) {
        flexItem.computeFlexBaseSize(t, f, availableMainSpace);
        flexItem.computeHypotheticalMainSize(t, {availableMainSpace, initiallyAvailableCrossSpace});

        // Speculate margins before following steps
        flexItem.speculateValues(t, {
                                        .commit = Commit::NO,
                                        .knownSize = {flexItem.flexBaseSize, NONE},
                                        .availableSpace = {flexItem.flexBaseSize, input.knownSize.y.unwrapOr(Px{0})},
                                        .containingBlock = {flexItem.flexBaseSize, input.knownSize.y.unwrapOr(Px{0})},
                                    });
    }

    // 4. Determine the main size of the flex container
    // TODO: we should have the knownSize of the main size; not clear what to do in case it isnt there
    auto usedMainSize = isRowOriented
                            ? input.knownSize.x.unwrapOr(Px{0})
                            : input.knownSize.y.unwrapOr(Px{0});

    // 5. Collect flex items into flex lines
    Vec<FlexLine> flexLines;
    if (flexContainer.wrap == FlexWrap::NOWRAP or input.intrinsic.x == IntrinsicSize::MAX_CONTENT) {
        flexLines = Vec<FlexLine>{FlexLine{flexItems}};

        if (input.intrinsic.x == IntrinsicSize::MIN_CONTENT or input.intrinsic.x == IntrinsicSize::MAX_CONTENT) {

            Vec<Px> flexFraction;
            for (auto &flexItem : flexItems) {
                Px contribution = flexItem.getMainSizeMinMaxContentContribution(
                    t,
                    input.intrinsic.x == IntrinsicSize::MIN_CONTENT,
                    {availableMainSpace, initiallyAvailableCrossSpace}
                );
                auto itemFlexFraction = (contribution - flexItem.flexBaseSize - flexItem.getMargin(FlexItem::HORIZONTAL));
                if (itemFlexFraction > Px{0})
                    itemFlexFraction = itemFlexFraction / Px{flexItem.flex.grow};
                else if (itemFlexFraction < Px{0})
                    itemFlexFraction = itemFlexFraction / Px{flexItem.flex.shrink};

                flexFraction.pushBack(itemFlexFraction);
            }

            Px largestSum{Px::MIN};
            usize globalIdx = 0;
            for (auto &flexLine : flexLines) {
                Px largestFraction{0};

                for (usize i = 0; i < flexLine.items.len(); ++i) {
                    largestFraction = max(largestFraction, flexFraction[globalIdx]);

                    globalIdx++;
                }

                Px sumOfProducts{0};
                for (auto &flexItem : flexLine.items) {
                    Px product;

                    // TODO: will the compiler optimize this?
                    if (largestFraction < Px{0}) {
                        product = largestFraction * flexItem.getScaledFlexShrinkFactor();
                    } else {
                        product = largestFraction * Px{flexItem.flex.grow};
                    }

                    // then clamp that result by the max main size floored by the min main size.
                    // QUESTION: how can i know these values?
                    // sumOfProducts += clamp(product, );
                }

                largestSum = max(largestSum, sumOfProducts);
            }

            usedMainSize = largestSum;
        }

    } else {
        if (input.intrinsic.x == IntrinsicSize::MIN_CONTENT) {
            flexLines.ensure(flexItems.len());

            Px largestMinContentContrib{Px::MIN};
            for (auto &flexItem : flexItems) {
                largestMinContentContrib = max(
                    largestMinContentContrib,
                    flexItem.getMainSizeMinMaxContentContribution(
                        t,
                        true,
                        {availableMainSpace, initiallyAvailableCrossSpace}
                    )
                );
            }

            usedMainSize = largestMinContentContrib;
        } else {
            usize startItemIdx = 0;
            while (startItemIdx < flexItems.len()) {
                usize endItemIdx = startItemIdx;
                Px currLineSize = Px{0};
                while (endItemIdx < flexItems.len()) {
                    Px itemContribution = flexItems[endItemIdx].hypoMainSize + flexItems[endItemIdx].getMargin(FlexItem::HORIZONTAL);
                    //                     // TODO: ignoring breaks for now
                    if (currLineSize + itemContribution <= availableMainSpace || currLineSize == Px{0}) {
                        currLineSize += itemContribution;
                        endItemIdx++;
                    } else
                        break;
                }

                flexLines.pushBack(FlexLine{mutSub(flexItems, startItemIdx, endItemIdx)});

                startItemIdx = endItemIdx;
            }
        }
    }

    if (flexContainer.direction == FlexDirection::ROW_REVERSE) {
        for (auto &flexLine : flexLines) {
            reverse(flexLine.items);
        }
    }

    // 6. Resolve the flexible lengths
    for (auto &flexLine : flexLines) {
        Px sumItemsHypotheticalMainSizes{0};
        for (auto const &flexItem : flexLine.items) {
            sumItemsHypotheticalMainSizes += flexItem.hypoMainSize + flexItem.getMargin(FlexItem::HORIZONTAL);
        }

        bool matchedSize = sumItemsHypotheticalMainSizes == usedMainSize;
        bool flexCaseIsGrow = sumItemsHypotheticalMainSizes < usedMainSize;

        Vec<FlexItem *> unfrozenItems{flexLine.items.len()}, frozenItems{flexLine.items.len()};
        Px sumFrozenOuterSizes{0};
        for (auto &flexItem : flexLine.items) {
            if (
                matchedSize or
                (flexCaseIsGrow and flexItem.flexBaseSize > flexItem.hypoMainSize) or
                (!flexCaseIsGrow and flexItem.flexBaseSize < flexItem.hypoMainSize) or
                (flexItem.flex.grow == 0 and flexItem.flex.shrink == 0)
            ) {
                flexItem.usedSize.x = flexItem.hypoMainSize;
                frozenItems.pushBack(&flexItem);
                sumFrozenOuterSizes += flexItem.usedSize.x + flexItem.getMargin(FlexItem::HORIZONTAL);
            } else {
                flexItem.usedSize.x = flexItem.flexBaseSize;
                unfrozenItems.pushBack(&flexItem);
            }
        }

        auto computeStats = [&]() {
            Px sumOfUnfrozenOuterSizes{0};
            Number sumUnfrozenFlexFactors{0};
            for (auto *flexItem : unfrozenItems) {
                sumOfUnfrozenOuterSizes += flexItem->flexBaseSize + flexItem->getMargin(FlexItem::HORIZONTAL);
                sumUnfrozenFlexFactors += flexCaseIsGrow ? flexItem->flex.grow : flexItem->flex.shrink;
            }

            return Tuple<Px, Number>(sumOfUnfrozenOuterSizes, sumUnfrozenFlexFactors);
        };

        auto [sumUnfrozenOuterSizes, _] = computeStats();
        // FIXME: weird types of spaces and sizes here, since free space can be negative
        Number initialFreeSpace = Number{usedMainSize} - Number{sumUnfrozenOuterSizes + sumFrozenOuterSizes};

        while (unfrozenItems.len()) {
            auto [sumUnfrozenOuterSizes, sumUnfrozenFlexFactors] = computeStats();
            auto freeSpace = Number{usedMainSize} - Number{sumUnfrozenOuterSizes + sumFrozenOuterSizes};

            if (sumUnfrozenFlexFactors < 1 and abs(initialFreeSpace * sumUnfrozenFlexFactors) < abs(freeSpace))
                freeSpace = initialFreeSpace * sumUnfrozenFlexFactors;

            if (flexCaseIsGrow) {
                for (auto *flexItem : unfrozenItems) {
                    Number ratio = flexItem->flex.grow / sumUnfrozenFlexFactors;
                    flexItem->usedSize.x = flexItem->flexBaseSize + Px{ratio * freeSpace};
                }
            } else {
                Px sumScaledFlexShrinkFactor{0};
                for (auto *flexItem : unfrozenItems) {
                    sumScaledFlexShrinkFactor += flexItem->getScaledFlexShrinkFactor();
                }

                for (auto *flexItem : unfrozenItems) {
                    Px ratio = flexItem->getScaledFlexShrinkFactor() / sumScaledFlexShrinkFactor;
                    flexItem->usedSize.x = flexItem->flexBaseSize - ratio * Px{abs(freeSpace)};
                }
            }

            Px totalViolation{0};

            auto clampAndFloorContentBox = [&](FlexItem *flexItem) {
                // FIXME: it seems that the browser sets minSize = max(minSize, textSize)
                auto clampedSize = clamp(
                    flexItem->usedSize.x,
                    flexItem->getMinMaxPrefferedSize(t, true, true, {availableMainSpace, initiallyAvailableCrossSpace}),
                    flexItem->getMinMaxPrefferedSize(t, true, false, {availableMainSpace, initiallyAvailableCrossSpace})
                );

                // TODO: should consider padding and border so content size is not negative
                auto minSizeFlooringContentSizeAt0 = Px{0};

                return max(clampedSize, minSizeFlooringContentSizeAt0);
            };
            // TODO: assuming row orientation here

            for (auto *flexItem : unfrozenItems) {
                Px clampedSize = clampAndFloorContentBox(flexItem);
                totalViolation += clampedSize - flexItem->usedSize.x;
            }
            for (auto *flexItem : frozenItems) {
                Px clampedSize = clampAndFloorContentBox(flexItem);
                totalViolation += clampedSize - flexItem->usedSize.x;
            }

            if (totalViolation == Px{0}) {
                Px soma{0};
                for (usize i = 0; i < unfrozenItems.len(); ++i) {
                    auto *flexItem = unfrozenItems[i];
                    Px clampedSize = clampAndFloorContentBox(flexItem);
                    flexItem->usedSize.x = clampedSize;
                    soma += clampedSize + flexItem->getMargin(FlexItem::HORIZONTAL);
                }

                for (auto *flexItem : unfrozenItems)
                    frozenItems.pushBack(flexItem);
                unfrozenItems.clear();
            } else {
                Vec<usize> indexesToFreeze;
                if (totalViolation < Px{0}) {
                    for (usize i = 0; i < unfrozenItems.len(); ++i) {
                        auto *flexItem = unfrozenItems[i];
                        Px clampedSize = clampAndFloorContentBox(flexItem);

                        if (clampedSize < flexItem->usedSize.x)
                            indexesToFreeze.pushBack(i);
                    }
                } else {
                    for (usize i = 0; i < unfrozenItems.len(); ++i) {
                        auto *flexItem = unfrozenItems[i];
                        Px clampedSize = clampAndFloorContentBox(flexItem);

                        if (clampedSize > flexItem->usedSize.x)
                            indexesToFreeze.pushBack(i);
                    }
                }
                for (usize i = 0; i < unfrozenItems.len(); ++i) {
                    auto *flexItem = unfrozenItems[i];
                    Px clampedSize = clampAndFloorContentBox(flexItem);
                    flexItem->usedSize.x = clampedSize;
                }

                // TODO: reverse indexesToFreeze so we use foreach
                for (int j = indexesToFreeze.len() - 1; j >= 0; j--) {
                    usize i = indexesToFreeze[j];

                    sumFrozenOuterSizes += unfrozenItems[i]->usedSize.x + unfrozenItems[i]->getMargin(FlexItem::HORIZONTAL);
                    frozenItems.pushBack(unfrozenItems[i]);

                    // fast delete of unordered buf
                    std::swap(unfrozenItems[i], unfrozenItems[unfrozenItems.len() - 1]);
                    unfrozenItems.popBack();
                }
            }
        }

        for (size_t i = 0; i < flexLine.items.len(); ++i) {
            auto &flexItem = flexLine.items[i];
            // 7. Determine the hypothetical cross size of each item

            // TODO: once again, this was coded assuming a ROW orientation
            Px availableCrossSpace = input.knownSize.y.unwrapOr(Px{0}) - flexItem.getMargin(FlexItem::VERTICAL);
            Input itemInput{
                .commit = input.commit,
                .knownSize = {flexItem.usedSize.x, NONE},
                .availableSpace = {
                    flexItem.usedSize.x,
                    availableCrossSpace
                },
            };

            if (flexItem.frag->style->sizing->width == Size::AUTO)
                input.intrinsic.x = IntrinsicSize::STRETCH_TO_FIT;

            flexItem.speculateValues(t, itemInput);
        }
    }

    // 8. Calculate the cross size of each flex line
    if (flexLines.len() == 1 and input.knownSize.y) {
        flexLines[0].crossSize = input.knownSize.y.unwrap();
    } else {
        // FIXME: this is a workaround since I still dont get the concepts required to implement it; also the workaround
        // is not considering the vertical margin, which I don't know if it should or not
        for (auto &flexLine : flexLines) {
            Px maxOuterHypCrossSize{0};
            Px maxDistBaselineCrossStartEdge{0};
            Px maxDistBaselineCrossEndEdge{0};
            for (auto &flexItem : flexLine.items) {
                if (
                    flexItem.frag->style->aligns.alignSelf == Style::Align::FIRST_BASELINE and
                    not flexItem.hasAnyCrossMarginAuto() /* and
                    inline-axis is parallel to main-axis*/
                ) {
                    /* Find the largest of the distances between each item’s baseline and its hypothetical outer
                    cross-start edge, and the largest of the distances between each item’s baseline and its hypothetical
                    outer cross-end edge, and sum these two values.
                    TODO
                    */
                    maxDistBaselineCrossStartEdge = max(maxDistBaselineCrossStartEdge, Px{0});
                    maxDistBaselineCrossEndEdge = max(maxDistBaselineCrossEndEdge, Px{0});
                } else {
                    maxOuterHypCrossSize = max(maxOuterHypCrossSize, flexItem.speculativeSize.y + flexItem.getMargin(FlexItem::VERTICAL));
                }
            }
            flexLine.crossSize = max(maxOuterHypCrossSize, maxDistBaselineCrossStartEdge + maxDistBaselineCrossEndEdge);
        }
    }

    // 9. Handle 'align-content: stretch'.
    // FIXME: If the flex container has a definite cross size <=?=> f.style->sizing->height.type != Size::Type::AUTO
    if (f.style->sizing->height.type != Size::Type::AUTO and f.style->aligns.alignContent == Style::Align::STRETCH) {
        Px sumOfCrossSizes{0};
        for (auto &flexLine : flexLines)
            sumOfCrossSizes += flexLine.crossSize;
        if (initiallyAvailableCrossSpace > sumOfCrossSizes) {
            Px toDistribute = (initiallyAvailableCrossSpace - sumOfCrossSizes) / Px{flexLines.len()};
            for (auto &flexLine : flexLines) {
                flexLine.crossSize += toDistribute;
            }
        }
    }

    // 10. Collapse visibility:collapse items.
    // TODO: simplify first try (assume not the case)

    // 11. Determine the used cross size of each flex item.
    for (auto &flexLine : flexLines) {
        for (auto &flexItem : flexLine.items) {
            Style::Align itemAlign = flexItem.frag->style->aligns.alignSelf;
            if (itemAlign == Style::Align::AUTO)
                itemAlign = f.style->aligns.alignItems;

            if (
                itemAlign == Style::Align::STRETCH and
                flexItem.frag->style->sizing->height.type == Size::AUTO and
                not flexItem.hasAnyCrossMarginAuto()
            ) {
                flexItem.usedSize.y = flexLine.crossSize - flexItem.getMargin(FlexItem::VERTICAL);
                flexItem.usedSize.y = clamp(
                    flexItem.usedSize.y,
                    flexItem.getMinMaxPrefferedSize(t, false, true, {availableMainSpace, initiallyAvailableCrossSpace}),
                    flexItem.getMinMaxPrefferedSize(t, false, false, {availableMainSpace, initiallyAvailableCrossSpace})
                );
            } else {
                flexItem.usedSize.y = flexItem.speculativeSize.y;
            }
        }
    }

    // 12. Distribute any remaining free space
    for (auto &flexLine : flexLines) {
        Px occupiedMainSize{0};
        for (auto &flexItem : flexLine.items) {
            occupiedMainSize += flexItem.usedSize.x + flexItem.getMargin(FlexItem::HORIZONTAL);
        }

        bool usedAutoMargins = false;
        if (usedMainSize > occupiedMainSize) {
            usize countOfAutos = 0;

            for (auto &flexItem : flexLine.items) {
                countOfAutos += (flexItem.frag->style->margin->start == Width::AUTO);
                countOfAutos += (flexItem.frag->style->margin->end == Width::AUTO);
            }

            if (countOfAutos) {
                Px marginsSize = (usedMainSize - occupiedMainSize) / Px{countOfAutos};
                for (auto &flexItem : flexLine.items) {
                    if (flexItem.frag->style->margin->start == Width::AUTO)
                        flexItem.margin.start = marginsSize;
                    if (flexItem.frag->style->margin->end == Width::AUTO)
                        flexItem.margin.end = marginsSize;
                }

                usedAutoMargins = true;
                occupiedMainSize = usedMainSize;
            }
        }

        if (not usedAutoMargins) {
            for (auto &flexItem : flexLine.items) {
                if (flexItem.frag->style->margin->start == Width::AUTO)
                    flexItem.margin.start = Px{0};
                if (flexItem.frag->style->margin->end == Width::AUTO)
                    flexItem.margin.end = Px{0};
            }
        }

        if (input.commit == Commit::YES) {
            // This is done after any flexible lengths and any auto margins have been resolved.
            // NOTE: justifying doesnt change sizes/margins, thus will only run when committing and setting positions
            auto justifyContent = f.style->aligns.justifyContent.keyword;
            if (flexContainer.direction == FlexDirection::ROW_REVERSE) {
                if (justifyContent == Style::Align::FLEX_START)
                    justifyContent = Style::Align::FLEX_END;
                else if (justifyContent == Style::Align::FLEX_END)
                    justifyContent = Style::Align::FLEX_START;
            }
            flexLine.justifyContent(justifyContent, usedMainSize, occupiedMainSize);
        }
    }

    // 13. Resolve cross-axis auto margins.
    for (auto &flexLine : flexLines) {
        for (auto &flexItem : flexLine.items) {
            bool bottomMarginIsAuto = flexItem.frag->style->margin->bottom == Width::AUTO;
            bool topMarginIsAuto = flexItem.frag->style->margin->top == Width::AUTO;
            if (bottomMarginIsAuto or topMarginIsAuto) {
                if (flexItem.usedSize.y + flexItem.getMargin(FlexItem::VERTICAL) < flexLine.crossSize) {
                    if (bottomMarginIsAuto and not topMarginIsAuto) {
                        auto topMargin = flexItem.getMargin(FlexItem::TOP);
                        Px freeCrossSpace = flexLine.crossSize - flexItem.usedSize.y - topMargin;
                        flexItem.margin.bottom = freeCrossSpace;

                    } else if (not bottomMarginIsAuto and topMarginIsAuto) {
                        auto bottomMargin = flexItem.getMargin(FlexItem::BOTTOM);
                        Px freeCrossSpace = flexLine.crossSize - flexItem.usedSize.y - bottomMargin;
                        flexItem.margin.top = freeCrossSpace;
                    } else {
                        Px freeCrossSpace = flexLine.crossSize - flexItem.usedSize.y;
                        flexItem.margin.bottom = flexItem.margin.top = freeCrossSpace / Px{2};
                    }
                    flexItem.position.y = flexItem.getMargin(FlexItem::TOP);
                } else {
                    if (flexItem.frag->style->margin->top == Width::Type::AUTO)
                        flexItem.margin.top = Px{0};

                    // FIXME: not sure if the following is what the specs want
                    if (flexLine.crossSize > flexItem.usedSize.y)
                        flexItem.margin.bottom = flexLine.crossSize - flexItem.usedSize.y;
                }
            }
        }
    }

    // 14. Align all flex items along the cross-axis.
    for (auto &flexLine : flexLines) {
        for (auto &flexItem : flexLine.items) {
            flexItem.alignItem(t, input.commit, flexLine.crossSize, f.style->aligns.alignItems.keyword);
        }
    }

    // 15. Determine the flex container's used cross size
    Px usedCrossSizeByLines{0};
    for (auto &flexLine : flexLines) {
        usedCrossSizeByLines += flexLine.crossSize;
    }

    Px usedCrossSize{0};
    if (input.knownSize.y)
        usedCrossSize = input.knownSize.y.unwrap();
    else
        usedCrossSize = usedCrossSizeByLines;
    // TODO: clamp usedCrossSize

    // 16. Align all flex lines
    {
        Px availableCrossSpace = initiallyAvailableCrossSpace - usedCrossSizeByLines;

        auto alignContentFlexStart = [&]() {
            Px currPositionY{0};
            for (auto &flexLine : flexLines) {
                flexLine.position.y = currPositionY;
                currPositionY += flexLine.crossSize;
            }
        };

        auto alignContentCenter = [&]() {
            Px currPositionY{availableCrossSpace / Px{2}};
            for (auto &flexLine : flexLines) {
                flexLine.position.y = currPositionY;
                currPositionY += flexLine.crossSize;
            }
        };

        switch (f.style->aligns.alignContent.keyword) {
        case Style::Align::FLEX_END: {
            Px currPositionY{availableCrossSpace};
            for (auto &flexLine : flexLines) {
                flexLine.position.y = currPositionY;
                currPositionY += flexLine.crossSize;
            }
            break;
        }
        case Style::Align::CENTER: {
            alignContentCenter();
            break;
        }
        case Style::Align::SPACE_AROUND: {
            if (availableCrossSpace < Px{0}) {
                alignContentCenter();
            } else {
                Px gapSize = availableCrossSpace / Px{flexLines.len()};

                Px currPositionY{gapSize / Px{2}};
                for (auto &flexLine : flexLines) {
                    flexLine.position.y = currPositionY;
                    currPositionY += flexLine.crossSize + gapSize;
                }
            }
            break;
        }
        case Style::Align::SPACE_BETWEEN: {
            if (availableCrossSpace < Px{0} or flexLines.len() == 1)
                alignContentFlexStart();
            else {
                Px gapSize = availableCrossSpace / Px{flexLines.len() - 1};

                Px currPositionY{0};
                for (auto &flexLine : flexLines) {
                    flexLine.position.y = currPositionY;
                    currPositionY += flexLine.crossSize + gapSize;
                }
            }
            break;
        }
        case Style::Align::FLEX_START: {
        default:
            alignContentFlexStart();
        }
        }
    }

    // NOTE: Flex items positions are relative to their flex lines; however, since flex lines are virtual elements,
    // items positions need to be adapted before committing to refer to the flex container
    // This however wont change the sizes, and will be done only when committing
    if (input.commit == Commit::YES) {
        for (auto &flexLine : flexLines) {
            for (auto &flexItem : flexLine.items) {
                flexItem.position.x += flexLine.position.x + input.position.x;
                flexItem.position.y += flexLine.position.y + input.position.y;

                layout(t, *flexItem.frag, Input{
                                              .commit = Commit::YES,
                                              .knownSize = {flexItem.usedSize.x, flexItem.usedSize.y},
                                              .position = flexItem.position,
                                              .availableSpace = flexItem.usedSize,
                                          });
                flexItem.commit();
            }
        }
    }

    return Output::fromSize({usedMainSize, usedCrossSize});
}

} // namespace Vaev::Layout
