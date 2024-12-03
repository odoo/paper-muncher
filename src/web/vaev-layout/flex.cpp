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

bool isMinMaxIntrinsicSize(IntrinsicSize intrinsic) {
    return intrinsic == IntrinsicSize::MIN_CONTENT or intrinsic == IntrinsicSize::MAX_CONTENT;
}

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

    // TODO: only implementing borders after border-box is finished
    // InsetsPx borders;

    FlexItem(Tree &tree, Box &box, bool isRowOriented, Vec2Px containingBlock)
        : box(&box), flexItemProps(*box.style->flex), fa(isRowOriented) {
        speculateValues(tree, Input{Commit::NO});
        // TODO: not always we will need min/max content sizes,
        //       this can be lazy computed for performance gains
        computeContentSizes(tree, containingBlock);
    }

    void commit() {
        box->layout.margin.top = margin.top.unwrapOr(speculativeMargin.top);
        box->layout.margin.start = margin.start.unwrapOr(speculativeMargin.start);
        box->layout.margin.end = margin.end.unwrapOr(speculativeMargin.end);
        box->layout.margin.bottom = margin.bottom.unwrapOr(speculativeMargin.bottom);
    }

    void computeContentSizes(Tree &tree, Vec2Px containingBlock) {
        minContentSize = computeIntrinsicSize(
            tree,
            *box,
            IntrinsicSize::MIN_CONTENT,
            containingBlock
        );
        maxContentSize = computeIntrinsicSize(
            tree,
            *box,
            IntrinsicSize::MAX_CONTENT,
            containingBlock
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

    void _speculateValues(Tree &tree, Input input, Vec2Px &speculativeSize) {
        Output out = layout(tree, *box, input);
        speculativeSize = out.size;
    }

    void speculateValues(Tree &t, Input input) {
        _speculateValues(t, input, speculativeSize);
        speculativeMargin = computeMargins(
            t,
            *box,
            {
                .commit = Commit::NO,
                .containingBlock = input.availableSpace, // FIXME
            }
        );
    }

    // https://www.w3.org/TR/css-flexbox-1/#valdef-flex-basis-auto
    void computeFlexBaseSize(Tree &tree, Px mainContainerSize, IntrinsicSize containerSizing) {
        // A NONE return here indicates a CONTENT case for the flex basis
        auto getDefiniteFlexBasisSize = [](FlexProps &flexItemProps, FlexAxis &fa, Box *box) -> Opt<CalcValue<PercentOr<Length>>> {
            if (flexItemProps.basis.type != FlexBasis::WIDTH)
                return NONE;

            if (flexItemProps.basis.width.type == Width::Type::VALUE)
                return flexItemProps.basis.width.value;

            if (fa.mainAxis(box->style->sizing).type == Size::Type::AUTO)
                return NONE;

            return fa.mainAxis(box->style->sizing).value;
        };

        if (auto flexBasisDefiniteSize = getDefiniteFlexBasisSize(flexItemProps, fa, box)) {
            flexBaseSize = resolve(
                tree,
                *box,
                flexBasisDefiniteSize.unwrap(),
                mainContainerSize
            );
            return;
        }

        if (isMinMaxIntrinsicSize(containerSizing)) {
            flexBaseSize = fa.mainAxis(
                containerSizing == IntrinsicSize::MIN_CONTENT
                    ? minContentSize
                    : maxContentSize
            );
            return;
        }

        // TODO:
        //  https://www.w3.org/TR/css-flexbox-1/#algo-main-item
        //  - intrinsic aspect ratio case
        //  - what does it mean: "depends on its available space"?
        flexBaseSize = fa.mainAxis(maxContentSize);
    }

    void computeHypotheticalMainSize(Tree &tree, Vec2Px containerSize) {
        hypoMainSize = clamp(
            flexBaseSize,
            getMinMaxPrefferedSize(tree, fa.isRowOriented, true, containerSize),
            getMinMaxPrefferedSize(tree, fa.isRowOriented, false, containerSize)
        );
    }

    // https://www.w3.org/TR/css-flexbox-1/#min-size-auto
    Px getMinAutoPrefMainSize(Tree &tree, Vec2Px containerSize) const {

        Opt<Px> definiteMaxMainSize;
        auto maxMainSize = box->style->sizing->maxSize(fa.isRowOriented ? Axis::HORIZONTAL : Axis::VERTICAL);
        if (maxMainSize.type == Size::Type::LENGTH) {
            definiteMaxMainSize = resolve(
                tree,
                *box,
                maxMainSize.value,
                fa.mainAxis(containerSize)
            );
        }

        Px contentSizeSuggestion = fa.mainAxis(minContentSize);
        // TODO: clamped by cross size if there is an aspect ratio
        if (definiteMaxMainSize)
            contentSizeSuggestion = min(contentSizeSuggestion, definiteMaxMainSize.unwrap());

        if (fa.mainAxis(box->style->sizing).type == Size::Type::LENGTH) {
            Px specifiedSizeSuggestion = resolve(
                tree,
                *box,
                fa.mainAxis(box->style->sizing).value,
                fa.mainAxis(containerSize)
            );

            if (definiteMaxMainSize)
                specifiedSizeSuggestion = min(specifiedSizeSuggestion, definiteMaxMainSize.unwrap());

            return min(contentSizeSuggestion, specifiedSizeSuggestion);
            // TODO: else if(aspect ratio)
        } else {
            return contentSizeSuggestion;
        }
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
            if (not isMin)
                panic("AUTO is an invalid value for max-width");

            // used cross min sizes are resolved to 0 whereas main sizes have specific method
            return isWidth == fa.isRowOriented
                       ? getMinAutoPrefMainSize(tree, containerSize)
                       : 0_px;
        case Size::NONE:
            if (isMin)
                panic("NONE is an invalid value for min-width");
            return Limits<Px>::MAX;
        }
    }

    // https://www.w3.org/TR/css-flexbox-1/#intrinsic-item-contributions
    Px getMainSizeMinMaxContentContribution(Tree &tree, bool isMin, Vec2Px containerSize) {
        Px contentContribution = fa.mainAxis(isMin ? minContentSize : maxContentSize) + getMargin(BOTH_MAIN);

        if (fa.mainAxis(box->style->sizing).type == Size::Type::LENGTH) {
            contentContribution = max(
                contentContribution,
                resolve(
                    tree,
                    *box,
                    fa.mainAxis(box->style->sizing).value,
                    fa.mainAxis(containerSize)
                ) + getMargin(BOTH_MAIN)
            );
        } else if (fa.mainAxis(box->style->sizing).type == Size::Type::MIN_CONTENT) {
            contentContribution = max(
                contentContribution,
                fa.mainAxis(minContentSize) + getMargin(BOTH_MAIN)
            );
        } else if (fa.mainAxis(box->style->sizing).type == Size::Type::AUTO and not isMin) {
            contentContribution = max(contentContribution, fa.mainAxis(speculativeSize));
            // contentContribution = max(contentContribution, fa.mainAxis(speculativeSize) + getMargin(BOTH_MAIN));
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

    // unofficial
    Px getCrossSizeMinMaxContentContribution(Tree &tree, bool isMin, Vec2Px containerSize) {
        Px contentContribution =
            getMargin(BOTH_MAIN) +
            fa.crossAxis(
                isMin
                    ? minContentSize
                    : maxContentSize
            );

        if (fa.crossAxis(box->style->sizing).type == Size::Type::LENGTH) {
            contentContribution = max(
                contentContribution,
                resolve(
                    tree,
                    *box,
                    fa.crossAxis(box->style->sizing).value,
                    fa.crossAxis(containerSize)
                ) + getMargin(BOTH_CROSS)
            );
        }

        return clamp(
            contentContribution,
            getMinMaxPrefferedSize(tree, not fa.isRowOriented, true, containerSize),
            getMinMaxPrefferedSize(tree, not fa.isRowOriented, false, containerSize)
        );
    }

    void alignCrossFlexStart() {

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
                2_px;
            fa.crossAxis(position) = startOfBlock + getMargin(START_CROSS);
        }
    }

    void alignCrossStretch(Tree &tree, Input input, Px lineCrossSize) {
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
                {
                    .commit = input.commit,
                    .knownSize = fa.extractMainAxisAndFillOptOther(usedSize, elementSpeculativeCrossSize),
                }
            );
        }

        fa.crossAxis(position) = getMargin(START_CROSS);
    }

    void alignItem(Tree &tree, Input input, Px lineCrossSize, Style::Align::Keywords parentAlignItems) {
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
            alignCrossStretch(tree, input, lineCrossSize);
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

        Px currPositionMainAxis{gapSize / 2_px};
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
        Px currPositionMainAxis{(mainSize - occupiedSize) / 2_px};
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

    void _generateAnonymousFlexItems(Tree &tree, Box &box, Vec2Px containingBlock) {
        _items.ensure(box.children().len());
        for (auto &c : box.children())
            _items.emplaceBack(tree, c, _flex.isRowOriented(), containingBlock);
    }

    // 2. MARK: Available main and cross space for the flex items --------------
    // https://www.w3.org/TR/css-flexbox-1/#algo-available

    Vec2Px availableSpace = {};

    void _determineCrossSpaceForIntrinsicSizes(Tree &tree, Input input) {

        // https://www.w3.org/TR/css-flexbox-1/#intrinsic-cross-sizes
        if (not _flex.isRowOriented() and _flex.wrap == FlexWrap::WRAP) {
            auto newInitiallyAvailableCrossSpace = Limits<Px>::MIN;
            for (auto &item : _items) {
                newInitiallyAvailableCrossSpace = max(
                    newInitiallyAvailableCrossSpace,
                    item.getCrossSizeMinMaxContentContribution(
                        tree,
                        input.intrinsic == IntrinsicSize::MIN_CONTENT,
                        availableSpace
                    )
                );
            }
            fa.crossAxis(availableSpace) = newInitiallyAvailableCrossSpace;
        }
    }

    void _determineAvailableMainAndCrossSpace(
        Tree &t, Input input
    ) {
        availableSpace = {
            input.knownSize.x.unwrapOr(input.availableSpace.x),
            input.knownSize.y.unwrapOr(input.availableSpace.y),
        };

        if (isMinMaxIntrinsicSize(input.intrinsic))
            _determineCrossSpaceForIntrinsicSizes(t, input);
    }

    // 3. MARK: Flex base size and hypothetical main size of each item ---------
    // https://www.w3.org/TR/css-flexbox-1/#algo-main-item

    void _determineFlexBaseSizeAndHypotheticalMainSize(Tree &tree, IntrinsicSize containerSize) {
        for (auto &i : _items) {
            i.computeFlexBaseSize(
                tree,
                fa.mainAxis(availableSpace),
                containerSize
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

    Vec<Px> _computeFlexFractions(Tree &t, Input input) {
        Vec<Px> flexFraction;
        for (auto &flexItem : _items) {
            Px contribution = flexItem.getMainSizeMinMaxContentContribution(
                t,
                input.intrinsic == IntrinsicSize::MIN_CONTENT,
                availableSpace
            );

            auto itemFlexFraction =
                contribution -
                flexItem.flexBaseSize -
                flexItem.getMargin(FlexItem::BOTH_MAIN);

            if (itemFlexFraction > 0_px)
                itemFlexFraction = itemFlexFraction / max(1_px, Px{flexItem.flexItemProps.grow});

            else if (itemFlexFraction < 0_px)
                itemFlexFraction = itemFlexFraction / max(1_px, Px{flexItem.flexItemProps.shrink});

            flexFraction.pushBack(itemFlexFraction);
        }
        return flexFraction;
    }

    // https://www.w3.org/TR/css-flexbox-1/#intrinsic-main-sizes
    void _computeIntrinsicSize(Tree &t, Vec<Px> flexFraction) {

        usize globalIdx = 0;

        Px largestFraction{Limits<Px>::MIN};

        // TODO: gonna need this when fragmenting
        for (usize i = 0; i < _items.len(); ++i) {
            largestFraction = max(largestFraction, flexFraction[globalIdx]);
            globalIdx++;
        }

        Px sumOfProducts{0};
        for (auto &flexItem : _items) {
            Px product{flexItem.flexBaseSize};

            if (largestFraction < 0_px) {
                product += largestFraction / flexItem.getScaledFlexShrinkFactor();
            } else {
                product += largestFraction * Px{flexItem.flexItemProps.grow};
            }

            auto minPrefferedSize = flexItem.getMinMaxPrefferedSize(
                t,
                fa.isRowOriented,
                true,
                availableSpace
            );
            auto maxPrefferedSize = flexItem.getMinMaxPrefferedSize(
                t,
                fa.isRowOriented,
                false,
                availableSpace
            );

            sumOfProducts += clamp(product, minPrefferedSize, maxPrefferedSize) +
                             flexItem.getMargin(FlexItem::BOTH_MAIN);
        }
        _usedMainSize = max(_usedMainSize, sumOfProducts);
    }

    // https://github.com/w3c/csswg-drafts/issues/8884
    void _computeIntrinsicSizeRowNoWrap(Tree &t) {
        _usedMainSize = 0_px;
        for (auto &flexItem : _items) {
            auto minContrib = flexItem.getMainSizeMinMaxContentContribution(
                t,
                true,
                availableSpace
            );

            bool cantMove = flexItem.flexItemProps.grow == 0 and flexItem.flexBaseSize > minContrib;
            cantMove |= flexItem.flexItemProps.shrink == 0 and flexItem.flexBaseSize < minContrib;

            if (cantMove and true)
                _usedMainSize += flexItem.flexBaseSize;
            else
                _usedMainSize += minContrib;
        }
    }

    void _computeIntrinsicSizeMinMultilineColumn(Tree &tree) {
        Px largestMinContentContrib = Limits<Px>::MIN;
        for (usize i = 0; i < _items.len(); ++i) {
            auto &flexItem = _items[i];

            largestMinContentContrib = max(
                largestMinContentContrib,
                flexItem.getMainSizeMinMaxContentContribution(
                    tree,
                    true,
                    availableSpace
                )
            );
        }

        _usedMainSize = max(_usedMainSize, largestMinContentContrib);
    }

    void _determineMainSize(Tree &t, Input input, Box &box) {
        _usedMainSize =
            _flex.isRowOriented()
                ? input.knownSize.x.unwrapOr(0_px)
                : input.knownSize.y.unwrapOr(0_px);

        if (input.intrinsic == IntrinsicSize::MAX_CONTENT) {
            if (fa.isRowOriented) {
                if (_flex.wrap == FlexWrap::NOWRAP)
                    _computeIntrinsicSizeRowNoWrap(t);
                else {
                    // NOTE: would be _computeIntrinsicSize(t, _computeFlexFractions(t, input)) per specs, but found
                    // different behaviour on Chrome
                    _computeIntrinsicSizeRowNoWrap(t);
                }
            } else
                _computeIntrinsicSize(t, _computeFlexFractions(t, input));
        }
        if (input.intrinsic == IntrinsicSize::MIN_CONTENT) {
            if (fa.isRowOriented) {
                if (_flex.wrap == FlexWrap::NOWRAP)
                    _computeIntrinsicSizeRowNoWrap(t);
                else
                    _computeIntrinsicSizeMinMultilineColumn(t);
            } else {
                if (_flex.wrap == FlexWrap::NOWRAP)
                    _computeIntrinsicSize(t, _computeFlexFractions(t, input));
                else {
                    _computeIntrinsicSize(t, _computeFlexFractions(t, input));
                    // Chrome's behaviour is to put all items in the same line, even if it is multiline column; thus we
                    // are not following specs and wont run _computeIntrinsicSizeMinMultilineColumn(t);
                }
            }
        } else if (not fa.isRowOriented) {
            auto heightWrapSizing = box.style->sizing->height;

            if (heightWrapSizing == Size::MAX_CONTENT or
                heightWrapSizing == Size::MIN_CONTENT or
                heightWrapSizing == Size::AUTO)
                _computeIntrinsicSize(t, _computeFlexFractions(t, input));
        }
    }

    // 5. MARK: Collect flex items into flex lines -----------------------------
    // https://www.w3.org/TR/css-flexbox-1/#algo-line-break

    Vec<FlexLine>
        _lines = {};

    // https://www.w3.org/TR/css-flexbox-1/#intrinsic-main-sizes
    void _collectFlexItemsInfoFlexLinesWrap() {

        usize si = 0;
        while (si < _items.len()) {
            usize ei = si;

            Px currLineSize = 0_px;
            while (ei < _items.len()) {

                Px itemContribution = _items[ei].hypoMainSize;

                itemContribution += _items[ei].getMargin(FlexItem::BOTH_MAIN);
                // TODO: ignoring breaks for now
                if (currLineSize + itemContribution <= _usedMainSize + Px{0.01} or currLineSize == 0_px) {
                    currLineSize += itemContribution;
                    ei++;
                } else
                    break;
            }

            _lines.emplaceBack(mutSub(_items, si, ei), fa.isRowOriented);

            si = ei;
        }
    }

    void _collectFlexItemsIntoFlexLines() {
        if (_flex.wrap == FlexWrap::NOWRAP) {
            _lines.emplaceBack(_items, fa.isRowOriented);
        } else {
            _collectFlexItemsInfoFlexLinesWrap();
        }

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
                    (flexItem.flexItemProps.grow == 0 and flexCaseIsGrow) or
                    (flexItem.flexItemProps.shrink == 0 and not flexCaseIsGrow)
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
                    auto minSizeFlooringContentSizeAt0 = 0_px;

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

                if (totalViolation == 0_px) {
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
                    if (totalViolation < 0_px) {
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
        for (auto &i : _items) {
            Px availableCrossSpace = fa.crossAxis(availableSpace) - i.getMargin(FlexItem::BOTH_CROSS);

            if (fa.mainAxis(i.box->style->sizing) == Size::AUTO || fa.crossAxis(i.box->style->sizing) == Size::AUTO)
                input.intrinsic = IntrinsicSize::STRETCH_TO_FIT;

            i.speculateValues(
                tree,
                {
                    .commit = Commit::NO,
                    .knownSize = fa.extractMainAxisAndFillOptOther(i.usedSize),
                    .availableSpace = fa.extractMainAxisAndFillOther(i.usedSize, availableCrossSpace),
                }
            );
        }
    }

    // 8. MARK: Calculate the cross size of each flex line ---------------------

    // https://www.w3.org/TR/css-flexbox-1/#intrinsic-cross-sizes
    void _calculateCrossSizeOfEachFlexLineIntrinsicSize(Tree &tree, Input input) {

        for (auto &flexLine : _lines) {
            if (input.intrinsic == IntrinsicSize::MIN_CONTENT)
                for (auto &flexItem : flexLine.items) {
                    flexLine.crossSize = max(
                        flexLine.crossSize,
                        flexItem.getCrossSizeMinMaxContentContribution(
                            tree,
                            true,
                            availableSpace
                        )
                    );
                }
            else
                for (auto &flexItem : flexLine.items) {
                    flexLine.crossSize = max(
                        flexLine.crossSize,
                        flexItem.getCrossSizeMinMaxContentContribution(
                            tree,
                            false,
                            availableSpace
                        )
                    );
                }
        }
    }

    // https://www.w3.org/TR/css-flexbox-1/#algo-cross-line
    void _calculateCrossSizeOfEachFlexLineNonIntrinsicSize(Input input) {
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
                    maxDistBaselineCrossStartEdge = max(maxDistBaselineCrossStartEdge, 0_px);
                    maxDistBaselineCrossEndEdge = max(maxDistBaselineCrossEndEdge, 0_px);
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

    void _calculateCrossSizeOfEachFlexLine(Tree &tree, Input input) {
        if (isMinMaxIntrinsicSize(input.intrinsic)) {
            _calculateCrossSizeOfEachFlexLineIntrinsicSize(tree, input);
            // TODO: follow specs
        } else {
            _calculateCrossSizeOfEachFlexLineNonIntrinsicSize(input);
        }
    }

    // 9. MARK: Handle 'align-content: stretch' --------------------------------
    // https://www.w3.org/TR/css-flexbox-1/#algo-line-stretch

    void _handleAlignContentStretch(Input input, Box &box) {
        // FIXME: If the flex container has a definite cross size <=?=> f.style->sizing->height.type != Size::Type::AUTO
        if (
            not(input.intrinsic == IntrinsicSize::MIN_CONTENT) and
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

    void _determineUsedCrossSize(Tree &tree, Box &box, Input input) {
        for (auto &flexLine : _lines) {
            for (auto &flexItem : flexLine.items) {
                Style::Align itemAlign = flexItem.box->style->aligns.alignSelf;
                if (itemAlign == Style::Align::AUTO)
                    itemAlign = box.style->aligns.alignItems;

                if (
                    itemAlign == Style::Align::STRETCH and
                    fa.crossAxis(flexItem.box->style->sizing).type == Size::AUTO and
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
                } else if (input.intrinsic == IntrinsicSize::MIN_CONTENT) {
                    fa.crossAxis(flexItem.usedSize) = flexItem.getCrossSizeMinMaxContentContribution(
                        tree,
                        true,
                        availableSpace
                    );
                } else if (input.intrinsic == IntrinsicSize::MAX_CONTENT) {
                    fa.crossAxis(flexItem.usedSize) = flexItem.getCrossSizeMinMaxContentContribution(
                        tree,
                        false,
                        availableSpace
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
                        fa.startMainAxis(flexItem.margin) = 0_px;
                    if (fa.endMainAxis(*flexItem.box->style->margin) == Width::AUTO)
                        fa.endMainAxis(flexItem.margin) = 0_px;
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
                                fa.startCrossAxis(i.margin) = freeCrossSpace / 2_px;
                        }
                        fa.crossAxis(i.position) = i.getMargin(FlexItem::START_CROSS);
                    } else {
                        if (i.box->style->margin->top == Width::Type::AUTO)
                            fa.startCrossAxis(i.margin) = 0_px;

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
                    input,
                    flexLine.crossSize,
                    box.style->aligns.alignItems.keyword
                );
            }
        }
    }

    // 15. MARK: Determine the flex container’s used cross size ----------------
    // https://www.w3.org/TR/css-flexbox-1/#algo-cross-container
    // https://www.w3.org/TR/css-flexbox-1/#intrinsic-cross-sizes

    Px _usedCrossSizeByLines{0};
    Px _usedCrossSize{0};

    void _determineFlexContainerUsedCrossSize(Input input, Box &box) {
        for (auto &flexLine : _lines) {
            _usedCrossSizeByLines += flexLine.crossSize;
        }

        if (isMinMaxIntrinsicSize(input.intrinsic) or
            fa.crossAxis(box.style->sizing) == Size::Type::AUTO)
            _usedCrossSize = _usedCrossSizeByLines;
        else if (fa.crossAxis(input.knownSize))
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
            Px currPositionCross{availableCrossSpace / 2_px};
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
            if (availableCrossSpace < 0_px) {
                alignContentCenter();
            } else {
                Px gapSize = availableCrossSpace / Px{_lines.len()};

                Px currPositionCross{gapSize / 2_px};
                for (auto &flexLine : _lines) {
                    fa.crossAxis(flexLine.position) = currPositionCross;
                    currPositionCross += flexLine.crossSize + gapSize;
                }
            }
            break;
        }

        case Style::Align::SPACE_BETWEEN: {
            if (availableCrossSpace < 0_px or _lines.len() == 1)
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
        _generateAnonymousFlexItems(tree, box, input.containingBlock);

        // 2. Determine the available main and cross space for the flex items.
        _determineAvailableMainAndCrossSpace(tree, input);

        // 3. Determine the flex base size and hypothetical main size of each item
        _determineFlexBaseSizeAndHypotheticalMainSize(tree, input.intrinsic);

        // 4. Determine the main size of the flex container
        _determineMainSize(tree, input, box);

        // 5. Collect flex items into flex lines
        _collectFlexItemsIntoFlexLines();

        // 6. Resolve the flexible lengths
        _resolveFlexibleLengths(tree);

        // 7. Determine the hypothetical cross size of each item
        _determineHypotheticalCrossSize(tree, input);

        // 8. Calculate the cross size of each flex line
        _calculateCrossSizeOfEachFlexLine(tree, input);

        // 9. Handle 'align-content: stretch'.
        _handleAlignContentStretch(input, box);

        // 10. Collapse visibility:collapse items.
        _collapseVisibilityCollapseItems();

        // 11. Determine the used cross size of each flex item.
        _determineUsedCrossSize(tree, box, input);

        // 12. Distribute any remaining free space
        _distributeRemainingFreeSpace(box, input);

        // 13. Resolve cross-axis auto margins.
        _resolveCrossAxisAutoMargins();

        // 14. Align all flex items along the cross-axis.
        _alignAllFlexItems(tree, box, input);

        // 15. Determine the flex container's used cross size
        _determineFlexContainerUsedCrossSize(input, box);

        // 16. Align all flex lines
        _alignAllFlexLines(box);

        // XX. Commit
        if (input.commit == Commit::YES)
            _commit(tree, input);

        return Output::fromSize(fa.buildPair(_usedMainSize, _usedCrossSize));
    }
};

Output flexLayout(Tree &tree, Box &box, Input input) {
    FlexFormatingContext fc = {
        ._flex = *box.style->flex,
        .fa = box.style->flex->isRowOriented(),
    };

    return fc.run(tree, box, input);
}

} // namespace Vaev::Layout