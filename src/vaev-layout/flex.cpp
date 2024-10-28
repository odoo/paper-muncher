#include "flex.h"

#include "box.h"
#include "layout.h"
#include "values.h"

namespace Vaev::Layout {

// FIXME: refrain from saving this on every flex item/line to decrease their sizes
struct FlexAxis {
    bool isRowOriented;

    FlexAxis(bool isRowOriented) : isRowOriented(isRowOriented) {}

    template <typename T>
    T &mainAxis(Math::Vec2<T> &value) const {
        return isRowOriented ? value.x : value.y;
    }

    template <typename T>
    T mainAxis(Math::Vec2<T> const &value) const {
        return isRowOriented ? value.x : value.y;
    }

    template <typename T>
    T &crossAxis(Math::Vec2<T> &value) const {
        return isRowOriented ? value.y : value.x;
    }

    template <typename T>
    T crossAxis(Math::Vec2<T> const &value) const {
        return isRowOriented ? value.y : value.x;
    }

    template <typename T>
    T mainAxis(Math::Insets<T> const &value) const {
        return isRowOriented ? value.horizontal() : value.vertical();
    }

    template <typename T>
    T &startMainAxis(Math::Insets<T> &value) const {
        return isRowOriented ? value.start : value.top;
    }

    template <typename T>
    T startMainAxis(Math::Insets<T> const &value) const {
        return isRowOriented ? value.start : value.top;
    }

    template <typename T>
    T &startCrossAxis(Math::Insets<T> &value) const {
        return isRowOriented ? value.top : value.start;
    }

    template <typename T>
    T startCrossAxis(Math::Insets<T> const &value) const {
        return isRowOriented ? value.top : value.start;
    }

    template <typename T>
    T &endMainAxis(Math::Insets<T> &value) const {
        return isRowOriented ? value.end : value.bottom;
    }

    template <typename T>
    T endMainAxis(Math::Insets<T> const &value) const {
        return isRowOriented ? value.end : value.bottom;
    }

    template <typename T>
    T &endCrossAxis(Math::Insets<T> &value) const {
        return isRowOriented ? value.bottom : value.end;
    }

    template <typename T>
    T endCrossAxis(Math::Insets<T> const &value) const {
        return isRowOriented ? value.bottom : value.end;
    }

    template <typename T>
    T crossAxis(Math::Insets<T> value) const {
        return isRowOriented ? value.vertical() : value.horizontal();
    }

    Size mainAxis(Cow<SizingProps> sizing) const {
        return isRowOriented ? sizing->width : sizing->height;
    }

    Size crossAxis(Cow<SizingProps> sizing) const {
        return isRowOriented ? sizing->height : sizing->width;
    }

    Vec2Px extractMainAxisAndFillOther(Vec2Px base, Px other) const {
        if (isRowOriented) {
            return {base.x, other};
        } else {
            return {other, base.y};
        }
    }

    Math::Vec2<Opt<Px>> extractMainAxisAndFillOptOther(Vec2Px base, Opt<Px> other = NONE) const {
        if (isRowOriented) {
            return {base.x, other};
        } else {
            return {other, base.y};
        }
    }

    template <typename T>
    Math::Vec2<T> buildPair(T main, T cross) const {
        if (isRowOriented) {
            return {main, cross};
        } else {
            return {cross, main};
        }
    }

    Math::Vec2<Opt<Px>> buildOptPairWithMainAndOther(Px mainValue, Opt<Px> other = NONE) const {
        if (isRowOriented) {
            return {mainValue, other};
        } else {
            return {other, mainValue};
        }
    }
};

struct FlexItem {
    Box *box;
    FlexProps flexItemProps;
    FlexAxis fa;

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

    // TODO: only implementing borders after border-box is finished
    // InsetsPx borders;

    FlexItem(Tree &tree, Box &box, bool isRowOriented)
        : box(&box), flexItemProps(*box.style->flex), fa(isRowOriented) {
        speculateValues(tree, Input{Commit::NO});
        // TODO: not always we will need min/max content sizes,
        //       this can be lazy computed for performance gains
        computeContentSizes(tree);
    }

    void commit() {
        box->layout.margin.top = margin.top.unwrapOr(speculativeMargin.top);
        box->layout.margin.start = margin.start.unwrapOr(speculativeMargin.start);
        box->layout.margin.end = margin.end.unwrapOr(speculativeMargin.end);
        box->layout.margin.bottom = margin.bottom.unwrapOr(speculativeMargin.bottom);
    }

    void computeContentSizes(Tree &t) {
        _speculateValues(
            t,
            {
                .intrinsic = IntrinsicSize::MAX_CONTENT,
                .knownSize = {NONE, NONE},
            },
            maxContentSize,
            maxContentMargin
        );
        _speculateValues(
            t,
            {
                .intrinsic = IntrinsicSize::MIN_CONTENT,
                .knownSize = {NONE, NONE},
            },
            minContentSize,
            minContentMargin
        );
    }

    enum OuterPosition {
        START_CROSS,
        START_MAIN,
        END_MAIN,
        END_CROSS,
        BOTH_MAIN,
        BOTH_CROSS
    };

    Px getMargin(OuterPosition position) const {
        // FIXME: when should we consider borders and when we shouldnt?

        switch (position) {
        case START_CROSS:
            return fa.startCrossAxis(margin).unwrapOr(fa.startCrossAxis(speculativeMargin));

        case START_MAIN:
            return fa.startMainAxis(margin).unwrapOr(fa.startMainAxis(speculativeMargin));

        case END_MAIN:
            return fa.endMainAxis(margin).unwrapOr(fa.endMainAxis(speculativeMargin));

        case END_CROSS:
            return fa.endCrossAxis(margin).unwrapOr(fa.endCrossAxis(speculativeMargin));

        case BOTH_MAIN:
            return fa.startMainAxis(margin).unwrapOr(fa.startMainAxis(speculativeMargin)) +
                   fa.endMainAxis(margin).unwrapOr(fa.endMainAxis(speculativeMargin));

        case BOTH_CROSS:
            return fa.startCrossAxis(margin).unwrapOr(fa.startCrossAxis(speculativeMargin)) +
                   fa.endCrossAxis(margin).unwrapOr(fa.endCrossAxis(speculativeMargin));
        }
    }

    bool hasAnyCrossMarginAuto() const {
        return (fa.startCrossAxis(*box->style->margin) == Width::Type::AUTO) or
               (fa.endCrossAxis(*box->style->margin) == Width::Type::AUTO);
    }

    Px getScaledFlexShrinkFactor() const {
        return flexBaseSize * Px{flexItemProps.shrink};
    }

    void _speculateValues(Tree &tree, Input input, Vec2Px &speculativeSize, InsetsPx &speculativeMargin) {
        Output out = layout(tree, *box, input);
        speculativeSize = out.size;
        speculativeMargin = computeMargins(
            tree,
            *box,
            {
                .containingBlock = speculativeSize,
            }
        );
    }

    void speculateValues(Tree &tree, Input input) {
        _speculateValues(tree, input, speculativeSize, speculativeMargin);
    }

    void computeFlexBaseSize(Tree &tree, Px mainContainerSize) {
        // TODO: check specs
        if (flexItemProps.basis.type == FlexBasis::WIDTH) {
            if (flexItemProps.basis.width.type == Width::Type::VALUE) {
                flexBaseSize = resolve(
                    tree,
                    *box,
                    flexItemProps.basis.width.value,
                    mainContainerSize
                );
            } else if (flexItemProps.basis.width.type == Width::Type::AUTO) {
                flexBaseSize = fa.mainAxis(speculativeSize);
            }
        }

        if (flexItemProps.basis.type == FlexBasis::Type::CONTENT and
            box->style->sizing->height.type == Size::Type::LENGTH /* and
            intrinsic aspect ratio*/
        ) {
            // TODO: placehold value, check specs
            Px aspectRatio{1};
            auto crossSize = resolve(tree, *box, box->style->sizing->height.value, Px{0});
            flexBaseSize = (crossSize)*aspectRatio;
        }

        if (false) {
            // TODO: other flex base size cases
            logWarn("not implemented flex base size case");
        }
    }

    void computeHypotheticalMainSize(Tree &tree, Vec2Px containerSize) {
        hypoMainSize = clamp(
            flexBaseSize,
            getMinMaxPrefferedSize(tree, flexItemProps.isRowOriented(), true, containerSize),
            getMinMaxPrefferedSize(tree, flexItemProps.isRowOriented(), false, containerSize)
        );
        hypoMainSize = flexBaseSize;
    }

    Px getMinMaxPrefferedSize(Tree &tree, bool isWidth, bool isMin, Vec2Px containerSize) const {
        Size sizeToResolve;
        if (isWidth and isMin)
            sizeToResolve = box->style->sizing->minWidth;
        else if (isWidth and not isMin)
            sizeToResolve = box->style->sizing->maxWidth;
        else if (not isWidth and isMin)
            sizeToResolve = box->style->sizing->minHeight;
        else
            sizeToResolve = box->style->sizing->maxHeight;

        switch (sizeToResolve.type) {
        case Size::LENGTH:
            return resolve(
                tree,
                *box,
                sizeToResolve.value,
                isWidth
                    ? containerSize.x
                    : containerSize.y
            );
        case Size::MIN_CONTENT:
            return isWidth ? minContentSize.x : minContentSize.y;
        case Size::MAX_CONTENT:
            return isWidth ? maxContentSize.x : maxContentSize.y;
        case Size::FIT_CONTENT:
            logWarn("not implemented");
        case Size::AUTO:
            if (isMin) {
                // https://www.w3.org/TR/css-flexbox-1/#min-size-auto
                Size specifiedSizeToResolve{isWidth ? box->style->sizing->width : box->style->sizing->height};

                if (specifiedSizeToResolve.type == Size::Type::LENGTH) {
                    auto resolvedSpecifiedSize = resolve(
                        tree,
                        *box,
                        specifiedSizeToResolve.value,
                        isWidth
                            ? containerSize.x
                            : containerSize.y
                    );
                    return min(resolvedSpecifiedSize, isWidth ? minContentSize.x : minContentSize.y);
                } else {
                    return isWidth ? minContentSize.x : minContentSize.y;
                }

            } else {
                panic("AUTO is an invalid value for max-width");
            }
        case Size::NONE:
            if (isMin)
                panic("NONE is an invalid value for min-width");
            return Limits<Px>::MAX;
        }
    }

    // https://www.w3.org/TR/css-flexbox-1/#intrinsic-item-contributions
    Px getMainSizeMinMaxContentContribution(Tree &tree, bool isMin, Vec2Px containerSize) {
        Px contentContribution;
        if (isMin)
            contentContribution = fa.mainAxis(minContentSize) + fa.mainAxis(minContentMargin);
        else
            contentContribution = fa.mainAxis(maxContentSize) + fa.mainAxis(maxContentMargin);

        if (fa.mainAxis(box->style->sizing).type == Size::Type::LENGTH) {
            contentContribution = max(
                contentContribution,
                resolve(
                    tree,
                    *box,
                    fa.mainAxis(box->style->sizing).value,
                    fa.mainAxis(containerSize)
                )
            );
        } else if (fa.mainAxis(box->style->sizing).type == Size::Type::MIN_CONTENT) {
            contentContribution = max(contentContribution, fa.mainAxis(minContentSize) + fa.mainAxis(minContentMargin));
        } else if (fa.mainAxis(box->style->sizing).type == Size::Type::AUTO and not isMin) {
            contentContribution = fa.mainAxis(speculativeSize);
        } else {
            logWarn("not implemented");
        }

        if (flexItemProps.grow == 0)
            contentContribution = min(contentContribution, flexBaseSize);

        if (flexItemProps.shrink == 0)
            contentContribution = max(contentContribution, flexBaseSize);

        return clamp(
            contentContribution,
            getMinMaxPrefferedSize(tree, fa.isRowOriented, true, containerSize),
            getMinMaxPrefferedSize(tree, fa.isRowOriented, false, containerSize)
        );
    }

    void
    alignCrossFlexStart() {

        if (not hasAnyCrossMarginAuto()) {
            fa.crossAxis(position) = getMargin(START_CROSS);
        }
    }

    void alignCrossFlexEnd(Px lineCrossSize) {
        if (not hasAnyCrossMarginAuto()) {
            fa.crossAxis(position) =
                lineCrossSize -
                fa.crossAxis(usedSize) -
                getMargin(END_CROSS);
        }
    }

    void alignCrossCenter(Px lineCrossSize) {
        if (not hasAnyCrossMarginAuto()) {
            Px startOfBlock =
                (lineCrossSize -
                 fa.crossAxis(usedSize) -
                 getMargin(BOTH_CROSS)
                ) /
                Px{2};
            fa.crossAxis(position) = startOfBlock + getMargin(START_CROSS);
        }
    }

    void alignCrossStretch(Tree &tree, Commit commit, Px lineCrossSize) {
        if (
            fa.crossAxis(box->style->sizing).type == Size::Type::AUTO and
            fa.startCrossAxis(*box->style->margin) != Width::Type::AUTO and
            fa.endCrossAxis(*box->style->margin) != Width::Type::AUTO
        ) {
            /* Its used value is the length necessary to make the cross size of the item’s margin box as close to
            the same size as the line as possible, while still respecting the constraints imposed by
            min-height/min-width/max-height/max-width.*/

            auto elementSpeculativeCrossSize = lineCrossSize - getMargin(BOTH_CROSS);
            speculateValues(
                tree,
                {.commit = commit,
                 .knownSize = fa.extractMainAxisAndFillOptOther(usedSize, elementSpeculativeCrossSize),
                 // TODO: not really sure of these arguments, check specs
                 .availableSpace = fa.extractMainAxisAndFillOther(usedSize, elementSpeculativeCrossSize)
                }
            );

            fa.crossAxis(position) = getMargin(START_CROSS);
        }
    }

    void alignItem(Tree &tree, Commit commit, Px lineCrossSize, Style::Align::Keywords parentAlignItems) {
        auto align = box->style->aligns.alignSelf.keyword;

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
    FlexAxis fa;
    Vec2Px position;

    FlexLine(MutSlice<FlexItem> items, bool isRowOriented)
        : items(items), crossSize(0), fa(isRowOriented) {}

    void alignMainFlexStart() {
        Px currPositionMainAxis{0};
        for (auto &flexItem : items) {
            fa.mainAxis(flexItem.position) =
                currPositionMainAxis + flexItem.getMargin(FlexItem::START_MAIN);
            currPositionMainAxis +=
                fa.mainAxis(flexItem.usedSize) + flexItem.getMargin(FlexItem::BOTH_MAIN);
        }
    }

    void alignMainFlexEnd(Px mainSize, Px occupiedSize) {
        Px currPositionMainAxis{mainSize - occupiedSize};
        for (auto &flexItem : items) {
            fa.mainAxis(flexItem.position) =
                currPositionMainAxis + flexItem.getMargin(FlexItem::START_MAIN);
            currPositionMainAxis +=
                fa.mainAxis(flexItem.usedSize) + flexItem.getMargin(FlexItem::BOTH_MAIN);
        }
    }

    void alignMainSpaceAround(Px mainSize, Px occupiedSize) {
        Px gapSize = (mainSize - occupiedSize) / Px{items.len()};

        Px currPositionMainAxis{gapSize / Px{2}};
        for (auto &flexItem : items) {
            fa.mainAxis(flexItem.position) =
                currPositionMainAxis + flexItem.getMargin(FlexItem::START_MAIN);
            currPositionMainAxis +=
                fa.mainAxis(flexItem.usedSize) + flexItem.getMargin(FlexItem::BOTH_MAIN) + gapSize;
        }
    }

    void alignMainSpaceBetween(Px mainSize, Px occupiedSize) {
        Px gapSize = (mainSize - occupiedSize) / Px{items.len() - 1};

        Px currPositionMainAxis{0};
        for (auto &flexItem : items) {
            fa.mainAxis(flexItem.position) =
                currPositionMainAxis + flexItem.getMargin(FlexItem::START_MAIN);
            currPositionMainAxis +=
                fa.mainAxis(flexItem.usedSize) + flexItem.getMargin(FlexItem::BOTH_MAIN) + gapSize;
        }
    }

    void alignMainCenter(Px mainSize, Px occupiedSize) {
        Px currPositionMainAxis{(mainSize - occupiedSize) / Px{2}};
        for (auto &flexItem : items) {
            fa.mainAxis(flexItem.position) =
                currPositionMainAxis + flexItem.getMargin(FlexItem::START_MAIN);
            currPositionMainAxis +=
                fa.mainAxis(flexItem.usedSize) + flexItem.getMargin(FlexItem::BOTH_MAIN);
        }
    }

    void justifyContent(Style::Align::Keywords justifyContent, Px mainSize, Px occupiedSize) {
        switch (justifyContent) {
        case Style::Align::SPACE_AROUND:
            if (occupiedSize > mainSize or items.len() == 1)
                alignMainCenter(mainSize, occupiedSize);
            else
                alignMainSpaceAround(mainSize, occupiedSize);
            break;

        case Style::Align::CENTER:
            alignMainCenter(mainSize, occupiedSize);
            break;

        case Style::Align::FLEX_END:
            alignMainFlexEnd(mainSize, occupiedSize);
            break;

        case Style::Align::SPACE_BETWEEN:
            alignMainSpaceBetween(mainSize, occupiedSize);
            break;

        case Style::Align::FLEX_START:
        default:
            alignMainFlexStart();
            break;
        }
    }
};

struct FlexFormatingContext {
    FlexProps _flex;
    FlexAxis fa{_flex.isRowOriented()};

    // https://www.w3.org/TR/css-flexbox-1/#layout-algorithm

    // 1. MARK: Generate anonymous flex items ----------------------------------
    // https://www.w3.org/TR/css-flexbox-1/#algo-anon-box

    Vec<FlexItem> _items = {};

    void _generateAnonymousFlexItems(Tree &tree, Box &box) {
        _items.ensure(box.children().len());
        for (auto &c : box.children())
            _items.emplaceBack(tree, c, fa.isRowOriented);
    }

    // 2. MARK: Available main and cross space for the flex items --------------
    // https://www.w3.org/TR/css-flexbox-1/#algo-available

    Vec2Px availableSpace = {};

    void _determineAvailableMainAndCrossSpace(
        Input input
    ) {
        availableSpace = {
            input.knownSize.x.unwrapOr(input.availableSpace.x),
            input.knownSize.y.unwrapOr(input.availableSpace.y),
        };
    }

    // 3. MARK: Flex base size and hypothetical main size of each item ---------
    // https://www.w3.org/TR/css-flexbox-1/#algo-main-item

    void _determineFlexBaseSizeAndHypotheticalMainSize(
        Tree &tree
    ) {
        for (auto &i : _items) {
            i.computeFlexBaseSize(
                tree,
                fa.mainAxis(availableSpace)
            );

            i.computeHypotheticalMainSize(tree, availableSpace);

            // Speculate margins before following steps
            i.speculateValues(
                tree,
                {
                    .commit = Commit::NO,
                    .knownSize = fa.extractMainAxisAndFillOptOther(i.flexBaseSize),
                    .availableSpace = availableSpace,
                }
            );
        }
    }

    // 4. MARK: Determine the main size of the flex container ------------------
    // https://www.w3.org/TR/css-flexbox-1/#algo-main-container

    Px _usedMainSize = {};

    void _determineMainSize(Input input) {
        // TODO: we should have the knownSize of the main size; not clear what to do in case it isnt there
        _usedMainSize =
            _flex.isRowOriented()
                ? input.knownSize.x.unwrapOr(Px{0})
                : input.knownSize.y.unwrapOr(Px{0});
    }

    // 5. MARK: Collect flex items into flex lines -----------------------------
    // https://www.w3.org/TR/css-flexbox-1/#algo-line-break

    Vec<FlexLine> _lines = {};

    void _collectFlexItemsInfoFlexLinesNowWrap(Tree &tree, Input input) {
        _lines.emplaceBack(_items, fa.isRowOriented);

        if (input.intrinsic == IntrinsicSize::MIN_CONTENT or
            input.intrinsic == IntrinsicSize::MAX_CONTENT) {

            Vec<Px> flexFraction;
            for (auto &flexItem : _items) {
                Px contribution = flexItem.getMainSizeMinMaxContentContribution(
                    tree,
                    input.intrinsic == IntrinsicSize::MIN_CONTENT,
                    availableSpace
                );

                auto itemFlexFraction = (contribution - flexItem.flexBaseSize - flexItem.getMargin(FlexItem::BOTH_MAIN));
                if (itemFlexFraction > Px{0})
                    itemFlexFraction = itemFlexFraction / Px{flexItem.flexItemProps.grow};

                else if (itemFlexFraction < Px{0})
                    itemFlexFraction = itemFlexFraction / Px{flexItem.flexItemProps.shrink};

                flexFraction.pushBack(itemFlexFraction);
            }

            Px largestSum = Limits<Px>::MIN;
            usize globalIdx = 0;
            for (auto &flexLine : _lines) {
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
                        product = largestFraction * Px{flexItem.flexItemProps.grow};
                    }

                    // then clamp that result by the max main size floored by the min main size.
                    // QUESTION: how can i know these values?
                    // sumOfProducts += clamp(product, );
                }

                largestSum = max(largestSum, sumOfProducts);
            }

            _usedMainSize = largestSum;
        }
    }

    void _collectFlexItemsInfoFlexLinesWrap(Tree &tree, Input input) {
        if (input.intrinsic == IntrinsicSize::MIN_CONTENT) {
            _lines.ensure(_items.len());
            Px largestMinContentContrib = Limits<Px>::MIN;

            for (usize i = 0; i < _items.len(); ++i) {
                largestMinContentContrib = max(
                    largestMinContentContrib,
                    _items[i].getMainSizeMinMaxContentContribution(
                        tree,
                        true,
                        availableSpace
                    )
                );
                _lines.emplaceBack(mutSub(_items, i, i + 1), fa.isRowOriented);
            }

            _usedMainSize = largestMinContentContrib;
        } else {
            usize si = 0;
            while (si < _items.len()) {
                usize ei = si;

                Px currLineSize = Px{0};
                while (ei < _items.len()) {
                    Px itemContribution = _items[ei].hypoMainSize + _items[ei].getMargin(FlexItem::BOTH_MAIN);
                    // TODO: ignoring breaks for now
                    if (currLineSize + itemContribution <= fa.mainAxis(availableSpace) or currLineSize == Px{0}) {
                        currLineSize += itemContribution;
                        ei++;
                    } else
                        break;
                }

                _lines.pushBack({
                    mutSub(_items, si, ei),
                    fa.isRowOriented,
                });

                si = ei;
            }
        }
    }

    void _collectFlexItemsIntoFlexLines(Tree &tree, Input input) {
        if (_flex.wrap == FlexWrap::NOWRAP or input.intrinsic == IntrinsicSize::MAX_CONTENT)
            _collectFlexItemsInfoFlexLinesNowWrap(tree, input);
        else
            _collectFlexItemsInfoFlexLinesWrap(tree, input);

        if (_flex.direction == FlexDirection::ROW_REVERSE)
            for (auto &flexLine : _lines)
                reverse(flexLine.items);
    }

    // 6. MARK: Resolve the flexible lengths -----------------------------------
    // https://www.w3.org/TR/css-flexbox-1/#algo-flex

    void _resolveFlexibleLengths(Tree &t) {
        for (auto &flexLine : _lines) {
            Px sumItemsHypotheticalMainSizes{0};
            for (auto const &flexItem : flexLine.items) {
                sumItemsHypotheticalMainSizes +=
                    flexItem.hypoMainSize + flexItem.getMargin(FlexItem::BOTH_MAIN);
            }

            bool matchedSize = sumItemsHypotheticalMainSizes == _usedMainSize;
            bool flexCaseIsGrow = sumItemsHypotheticalMainSizes < _usedMainSize;

            Vec<FlexItem *> unfrozenItems{flexLine.items.len()};
            Vec<FlexItem *> frozenItems{flexLine.items.len()};

            Px sumFrozenOuterSizes{0};
            for (auto &flexItem : flexLine.items) {
                if (
                    matchedSize or
                    (flexCaseIsGrow and flexItem.flexBaseSize > flexItem.hypoMainSize) or
                    (not flexCaseIsGrow and flexItem.flexBaseSize < flexItem.hypoMainSize) or
                    (flexItem.flexItemProps.grow == 0 and flexItem.flexItemProps.shrink == 0)
                ) {
                    fa.mainAxis(flexItem.usedSize) = flexItem.hypoMainSize;
                    frozenItems.pushBack(&flexItem);
                    sumFrozenOuterSizes +=
                        fa.mainAxis(flexItem.usedSize) +
                        flexItem.getMargin(FlexItem::BOTH_MAIN);
                } else {
                    fa.mainAxis(flexItem.usedSize) = flexItem.flexBaseSize;
                    unfrozenItems.pushBack(&flexItem);
                }
            }

            auto computeStats = [&]() {
                Px sumOfUnfrozenOuterSizes{0};
                Number sumUnfrozenFlexFactors{0};
                for (auto *flexItem : unfrozenItems) {
                    sumOfUnfrozenOuterSizes +=
                        flexItem->flexBaseSize + flexItem->getMargin(FlexItem::BOTH_MAIN);

                    sumUnfrozenFlexFactors +=
                        flexCaseIsGrow
                            ? flexItem->flexItemProps.grow
                            : flexItem->flexItemProps.shrink;
                }

                return Tuple<Px, Number>(sumOfUnfrozenOuterSizes, sumUnfrozenFlexFactors);
            };

            auto [sumUnfrozenOuterSizes, _] = computeStats();
            // FIXME: weird types of spaces and sizes here, since free space can be negative
            Number initialFreeSpace = Number{_usedMainSize} - Number{sumUnfrozenOuterSizes + sumFrozenOuterSizes};

            while (unfrozenItems.len()) {
                auto [sumUnfrozenOuterSizes, sumUnfrozenFlexFactors] = computeStats();
                auto freeSpace = Number{_usedMainSize} - Number{sumUnfrozenOuterSizes + sumFrozenOuterSizes};

                if (
                    sumUnfrozenFlexFactors < 1 and
                    Math::abs(initialFreeSpace * sumUnfrozenFlexFactors) < Math::abs(freeSpace)
                )
                    freeSpace = initialFreeSpace * sumUnfrozenFlexFactors;

                if (flexCaseIsGrow) {
                    for (auto *flexItem : unfrozenItems) {
                        Number ratio = flexItem->flexItemProps.grow / sumUnfrozenFlexFactors;

                        fa.mainAxis(flexItem->usedSize) = flexItem->flexBaseSize + Px{ratio * freeSpace};
                    }
                } else {
                    Px sumScaledFlexShrinkFactor{0};
                    for (auto *flexItem : unfrozenItems) {
                        sumScaledFlexShrinkFactor += flexItem->getScaledFlexShrinkFactor();
                    }

                    for (auto *flexItem : unfrozenItems) {
                        Px ratio = flexItem->getScaledFlexShrinkFactor() / sumScaledFlexShrinkFactor;
                        fa.mainAxis(flexItem->usedSize) =
                            flexItem->flexBaseSize - ratio * Px{Math::abs(freeSpace)};
                    }
                }

                Px totalViolation{0};

                auto clampAndFloorContentBox = [&](FlexItem *flexItem) {
                    auto clampedSize = clamp(
                        fa.mainAxis(flexItem->usedSize),
                        flexItem->getMinMaxPrefferedSize(
                            t,
                            fa.isRowOriented,
                            true,
                            availableSpace
                        ),
                        flexItem->getMinMaxPrefferedSize(
                            t,
                            fa.isRowOriented,
                            false,
                            availableSpace
                        )
                    );

                    // TODO: should consider padding and border so content size is not negative
                    auto minSizeFlooringContentSizeAt0 = Px{0};

                    return max(clampedSize, minSizeFlooringContentSizeAt0);
                };

                // TODO: assuming row orientation here

                for (auto *flexItem : unfrozenItems) {
                    Px clampedSize = clampAndFloorContentBox(flexItem);
                    totalViolation += clampedSize - fa.mainAxis(flexItem->usedSize);
                }
                for (auto *flexItem : frozenItems) {
                    Px clampedSize = clampAndFloorContentBox(flexItem);
                    totalViolation += clampedSize - fa.mainAxis(flexItem->usedSize);
                }

                if (totalViolation == Px{0}) {
                    for (usize i = 0; i < unfrozenItems.len(); ++i) {
                        auto *flexItem = unfrozenItems[i];
                        Px clampedSize = clampAndFloorContentBox(flexItem);
                        fa.mainAxis(flexItem->usedSize) = clampedSize;
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

                            if (clampedSize < fa.mainAxis(flexItem->usedSize))
                                indexesToFreeze.pushBack(i);
                        }
                    } else {
                        for (usize i = 0; i < unfrozenItems.len(); ++i) {
                            auto *flexItem = unfrozenItems[i];
                            Px clampedSize = clampAndFloorContentBox(flexItem);

                            if (clampedSize > fa.mainAxis(flexItem->usedSize))
                                indexesToFreeze.pushBack(i);
                        }
                    }
                    for (usize i = 0; i < unfrozenItems.len(); ++i) {
                        auto *flexItem = unfrozenItems[i];
                        Px clampedSize = clampAndFloorContentBox(flexItem);
                        fa.mainAxis(flexItem->usedSize) = clampedSize;
                    }

                    for (int j = indexesToFreeze.len() - 1; j >= 0; j--) {
                        usize i = indexesToFreeze[j];

                        sumFrozenOuterSizes +=
                            fa.mainAxis(unfrozenItems[i]->usedSize) +
                            unfrozenItems[i]->getMargin(FlexItem::BOTH_MAIN);
                        frozenItems.pushBack(unfrozenItems[i]);

                        // fast delete of unordered buf
                        std::swap(unfrozenItems[i], unfrozenItems[unfrozenItems.len() - 1]);
                        unfrozenItems.popBack();
                    }
                }
            }
        }
    }

    // 7. MARK: Determine the hypothetical cross size of each item -------------
    // https://www.w3.org/TR/css-flexbox-1/#algo-cross-item

    void _determineHypotheticalCrossSize(Tree &tree, Input input) {
        // TODO: once again, this was coded assuming a ROW orientation
        for (auto &i : _items) {
            Px availableCrossSpace = fa.crossAxis(availableSpace) - i.getMargin(FlexItem::BOTH_CROSS);

            if (fa.mainAxis(i.box->style->sizing) == Size::AUTO)
                input.intrinsic = IntrinsicSize::STRETCH_TO_FIT;

            i.speculateValues(
                tree,
                {
                    .commit = input.commit,
                    .knownSize = fa.extractMainAxisAndFillOptOther(i.usedSize),
                    .availableSpace = fa.extractMainAxisAndFillOther(i.usedSize, availableCrossSpace),
                }
            );
        }
    }

    // 8. MARK: Calculate the cross size of each flex line ---------------------

    // https://www.w3.org/TR/css-flexbox-1/#algo-cross-line

    void _calculateCrossSizeOfEachFlexLine(Input input) {
        if (_lines.len() == 1 and fa.crossAxis(input.knownSize)) {
            first(_lines).crossSize = fa.crossAxis(input.knownSize).unwrap();
            return;
        }

        // FIXME: this is a workaround since I still dont get the concepts required to implement it; also the workaround
        // is not considering the vertical margin, which I don't know if it should or not
        for (auto &flexLine : _lines) {
            Px maxOuterHypCrossSize{0};
            Px maxDistBaselineCrossStartEdge{0};
            Px maxDistBaselineCrossEndEdge{0};

            for (auto &flexItem : flexLine.items) {
                if (
                    flexItem.box->style->aligns.alignSelf == Style::Align::FIRST_BASELINE and
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
                    maxOuterHypCrossSize = max(
                        maxOuterHypCrossSize,
                        fa.crossAxis(flexItem.speculativeSize) +
                            flexItem.getMargin(FlexItem::BOTH_CROSS)
                    );
                }
            }
            flexLine.crossSize = max(
                maxOuterHypCrossSize,
                maxDistBaselineCrossStartEdge + maxDistBaselineCrossEndEdge
            );
        }
    }

    // 9. MARK: Handle 'align-content: stretch' --------------------------------
    // https://www.w3.org/TR/css-flexbox-1/#algo-line-stretch

    void _handleAlignContentStretch(Box &box, Input input) {
        // FIXME: If the flex container has a definite cross size <=?=> f.style->sizing->height.type != Size::Type::AUTO
        if (
            (fa.crossAxis(box.style->sizing).type != Size::Type::AUTO or fa.crossAxis(input.knownSize)) and
            box.style->aligns.alignContent == Style::Align::STRETCH
        ) {
            Px sumOfCrossSizes{0};
            for (auto &flexLine : _lines)
                sumOfCrossSizes += flexLine.crossSize;
            if (fa.crossAxis(availableSpace) > sumOfCrossSizes) {
                Px toDistribute = (fa.crossAxis(availableSpace) - sumOfCrossSizes) / Px{_lines.len()};
                for (auto &flexLine : _lines) {
                    flexLine.crossSize += toDistribute;
                }
            }
        }
    }

    // 10. MARK: Collapse visibility:collapse items ----------------------------
    // https://www.w3.org/TR/css-flexbox-1/#algo-visibility

    void _collapseVisibilityCollapseItems() {
        // TODO: simplify first try (assume not the case)
    }

    // 11. MARK: Determine the used cross size of each flex item ---------------
    // https://www.w3.org/TR/css-flexbox-1/#algo-stretch

    void _determineUsedCrossSize(Tree &tree, Box &box) {
        for (auto &flexLine : _lines) {
            for (auto &flexItem : flexLine.items) {
                Style::Align itemAlign = flexItem.box->style->aligns.alignSelf;
                if (itemAlign == Style::Align::AUTO)
                    itemAlign = box.style->aligns.alignItems;

                if (
                    itemAlign == Style::Align::STRETCH and
                    flexItem.box->style->sizing->height.type == Size::AUTO and
                    not flexItem.hasAnyCrossMarginAuto()
                ) {
                    fa.crossAxis(flexItem.usedSize) =
                        flexLine.crossSize - flexItem.getMargin(FlexItem::BOTH_CROSS);

                    fa.crossAxis(flexItem.usedSize) = clamp(
                        fa.crossAxis(flexItem.usedSize),
                        flexItem.getMinMaxPrefferedSize(
                            tree,
                            not fa.isRowOriented,
                            true,
                            availableSpace
                        ),
                        flexItem.getMinMaxPrefferedSize(
                            tree,
                            not fa.isRowOriented,
                            false,
                            availableSpace
                        )
                    );
                } else {
                    fa.crossAxis(flexItem.usedSize) = fa.crossAxis(flexItem.speculativeSize);
                }
            }
        }
    }

    // 12. MARK: Distribute any remaining free space ---------------------------
    // https://www.w3.org/TR/css-flexbox-1/#algo-main-align

    void _distributeRemainingFreeSpace(Box &box, Input input) {
        for (auto &flexLine : _lines) {
            Px occupiedMainSize{0};
            for (auto &flexItem : flexLine.items) {
                occupiedMainSize +=
                    fa.mainAxis(flexItem.usedSize) + flexItem.getMargin(FlexItem::BOTH_MAIN);
            }

            bool usedAutoMargins = false;
            if (_usedMainSize > occupiedMainSize) {
                usize countOfAutos = 0;

                for (auto &flexItem : flexLine.items) {
                    countOfAutos += (fa.startMainAxis(*flexItem.box->style->margin) == Width::AUTO);
                    countOfAutos += (fa.endMainAxis(*flexItem.box->style->margin) == Width::AUTO);
                }

                if (countOfAutos) {
                    Px marginsSize = (_usedMainSize - occupiedMainSize) / Px{countOfAutos};
                    for (auto &flexItem : flexLine.items) {
                        if (fa.startMainAxis(*flexItem.box->style->margin) == Width::AUTO)
                            fa.startMainAxis(flexItem.margin) = marginsSize;
                        if (fa.endMainAxis(*flexItem.box->style->margin) == Width::AUTO)
                            fa.endMainAxis(flexItem.margin) = marginsSize;
                    }

                    usedAutoMargins = true;
                    occupiedMainSize = _usedMainSize;
                }
            }

            if (not usedAutoMargins) {
                for (auto &flexItem : flexLine.items) {
                    if (fa.startMainAxis(*flexItem.box->style->margin) == Width::AUTO)
                        fa.startMainAxis(flexItem.margin) = Px{0};
                    if (fa.endMainAxis(*flexItem.box->style->margin) == Width::AUTO)
                        fa.endMainAxis(flexItem.margin) = Px{0};
                }
            }

            if (input.commit == Commit::YES) {
                // This is done after any flexible lengths and any auto margins have been resolved.
                // NOTE: justifying doesnt change sizes/margins, thus will only run when committing and setting positions
                auto justifyContent = box.style->aligns.justifyContent.keyword;
                if (_flex.direction == FlexDirection::ROW_REVERSE) {
                    if (justifyContent == Style::Align::FLEX_START)
                        justifyContent = Style::Align::FLEX_END;
                    else if (justifyContent == Style::Align::FLEX_END)
                        justifyContent = Style::Align::FLEX_START;
                }
                flexLine.justifyContent(
                    justifyContent,
                    _usedMainSize,
                    occupiedMainSize
                );
            }
        }
    }

    // 13. MARK: Resolve cross-axis auto margins -------------------------------
    // https://www.w3.org/TR/css-flexbox-1/#algo-cross-margins

    void _resolveCrossAxisAutoMargins() {
        for (auto &l : _lines) {
            for (auto &i : l.items) {

                auto marginStyle = *i.box->style->margin;

                bool startCrossMarginIsAuto = fa.startCrossAxis(marginStyle) == Width::AUTO;
                bool endCrossMarginIsAuto = fa.endCrossAxis(marginStyle) == Width::AUTO;

                if (startCrossMarginIsAuto or endCrossMarginIsAuto) {
                    if (fa.crossAxis(i.usedSize) + i.getMargin(FlexItem::BOTH_CROSS) < l.crossSize) {
                        if (not startCrossMarginIsAuto and endCrossMarginIsAuto) {
                            auto startMargin = i.getMargin(FlexItem::START_CROSS);
                            Px freeCrossSpace = l.crossSize - fa.crossAxis(i.usedSize) - startMargin;
                            fa.endCrossAxis(i.margin) = freeCrossSpace;

                        } else if (startCrossMarginIsAuto and not endCrossMarginIsAuto) {
                            auto endMargin = i.getMargin(FlexItem::END_CROSS);
                            Px freeCrossSpace = l.crossSize - fa.crossAxis(i.usedSize) - endMargin;
                            fa.startCrossAxis(i.margin) = freeCrossSpace;
                        } else {
                            Px freeCrossSpace = l.crossSize - fa.crossAxis(i.usedSize);
                            fa.endCrossAxis(i.margin) =
                                fa.startCrossAxis(i.margin) = freeCrossSpace / Px{2};
                        }
                        fa.crossAxis(i.position) = i.getMargin(FlexItem::START_CROSS);
                    } else {
                        if (i.box->style->margin->top == Width::Type::AUTO)
                            fa.startCrossAxis(i.margin) = Px{0};

                        // FIXME: not sure if the following is what the specs want
                        if (l.crossSize > fa.crossAxis(i.usedSize))
                            fa.endCrossAxis(i.margin) = l.crossSize - fa.crossAxis(i.usedSize);
                    }
                }
            }
        }
    }

    // 14. MARK: Align all flex items ------------------------------------------
    // https://www.w3.org/TR/css-flexbox-1/#algo-cross-align

    void _alignAllFlexItems(Tree &tree, Box &box, Input input) {
        for (auto &flexLine : _lines) {
            for (auto &flexItem : flexLine.items) {
                flexItem.alignItem(
                    tree,
                    input.commit,
                    flexLine.crossSize,
                    box.style->aligns.alignItems.keyword
                );
            }
        }
    }

    // 15. MARK: Determine the flex container’s used cross size ----------------
    // https://www.w3.org/TR/css-flexbox-1/#algo-cross-container

    Px _usedCrossSizeByLines{0};
    Px _usedCrossSize{0};

    void _determineFlexContainerUsedCrossSize(Input input) {
        for (auto &flexLine : _lines)
            _usedCrossSizeByLines += flexLine.crossSize;

        if (fa.crossAxis(input.knownSize))
            _usedCrossSize = fa.crossAxis(input.knownSize).unwrap();
        else
            _usedCrossSize = _usedCrossSizeByLines;

        // TODO: clamp usedCrossSize
    }

    // 16. MARK: Align all flex lines ------------------------------------------
    // https://www.w3.org/TR/css-flexbox-1/#algo-line-align

    void _alignAllFlexLines(Box &box) {
        Px availableCrossSpace = fa.crossAxis(availableSpace) - _usedCrossSizeByLines;

        auto alignContentFlexStart = [&]() {
            Px currPositionCross{0};
            for (auto &flexLine : _lines) {
                fa.crossAxis(flexLine.position) = currPositionCross;
                currPositionCross += flexLine.crossSize;
            }
        };

        auto alignContentCenter = [&]() {
            Px currPositionCross{availableCrossSpace / Px{2}};
            for (auto &flexLine : _lines) {
                fa.crossAxis(flexLine.position) = currPositionCross;
                currPositionCross += flexLine.crossSize;
            }
        };

        switch (box.style->aligns.alignContent.keyword) {
        case Style::Align::FLEX_END: {
            Px currPositionCross{availableCrossSpace};
            for (auto &flexLine : _lines) {
                fa.crossAxis(flexLine.position) = currPositionCross;
                currPositionCross += flexLine.crossSize;
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
                Px gapSize = availableCrossSpace / Px{_lines.len()};

                Px currPositionCross{gapSize / Px{2}};
                for (auto &flexLine : _lines) {
                    fa.crossAxis(flexLine.position) = currPositionCross;
                    currPositionCross += flexLine.crossSize + gapSize;
                }
            }
            break;
        }

        case Style::Align::SPACE_BETWEEN: {
            if (availableCrossSpace < Px{0} or _lines.len() == 1)
                alignContentFlexStart();
            else {
                Px gapSize = availableCrossSpace / Px{_lines.len() - 1};

                Px currPositionCross{0};
                for (auto &flexLine : _lines) {
                    fa.crossAxis(flexLine.position) = currPositionCross;
                    currPositionCross += flexLine.crossSize + gapSize;
                }
            }
            break;
        }

        case Style::Align::FLEX_START:
        default:
            alignContentFlexStart();
        }
    }

    // XX. MARK: Commit --------------------------------------------------------

    void _commit(Tree &tree, Input input) {
        // NOTE: Flex items positions are relative to their flex lines;
        //       however, since flex lines are virtual elements,
        //       items positions need to be adapted before committing
        //       to refer to the flex container. This however wont change
        //       the sizes, and will be done only when committing
        for (auto &flexLine : _lines) {
            for (auto &flexItem : flexLine.items) {
                flexItem.position = flexItem.position + flexLine.position + input.position;

                auto out = layout(
                    tree,
                    *flexItem.box,
                    {
                        .commit = Commit::YES,
                        .knownSize = {flexItem.usedSize.x, flexItem.usedSize.y},
                        .position = flexItem.position,
                        .availableSpace = flexItem.usedSize,
                    }
                );
                flexItem.commit();
            }
        }
    }

    // MARK: Public API --------------------------------------------------------

    // FIXME: auto, min and max content values for flex container dimensions are not working as in Chrome; add tests
    Output run(Tree &tree, Box &box, Input input) {
        // 1. Generate anonymous flex items
        _generateAnonymousFlexItems(tree, box);

        // 2. Determine the available main and cross space for the flex items.
        _determineAvailableMainAndCrossSpace(input);

        // 3. Determine the flex base size and hypothetical main size of each item
        _determineFlexBaseSizeAndHypotheticalMainSize(tree);

        // 4. Determine the main size of the flex container
        _determineMainSize(input);

        // 5. Collect flex items into flex lines
        _collectFlexItemsIntoFlexLines(tree, input);

        // 6. Resolve the flexible lengths
        _resolveFlexibleLengths(tree);

        // 7. Determine the hypothetical cross size of each item
        _determineHypotheticalCrossSize(tree, input);

        // 8. Calculate the cross size of each flex line
        _calculateCrossSizeOfEachFlexLine(input);

        // 9. Handle 'align-content: stretch'.
        _handleAlignContentStretch(box, input);

        // 10. Collapse visibility:collapse items.
        _collapseVisibilityCollapseItems();

        // 11. Determine the used cross size of each flex item.
        _determineUsedCrossSize(tree, box);

        // 12. Distribute any remaining free space
        _distributeRemainingFreeSpace(box, input);

        // 13. Resolve cross-axis auto margins.
        _resolveCrossAxisAutoMargins();

        // 14. Align all flex items along the cross-axis.
        _alignAllFlexItems(tree, box, input);

        // 15. Determine the flex container's used cross size
        _determineFlexContainerUsedCrossSize(input);

        // 16. Align all flex lines
        _alignAllFlexLines(box);

        // XX. Commit
        if (input.commit == Commit::YES)
            _commit(tree, input);

        return Output::fromSize(fa.buildPair(_usedMainSize, _usedCrossSize));
    }
};

Output flexLayout(Tree &tree, Box &box, Input input) {
    FlexFormatingContext fc = {*box.style->flex};
    return fc.run(tree, box, input);
}

} // namespace Vaev::Layout
