#include "flex.h"

#include "frag.h"
#include "layout.h"
#include "values.h"

namespace Vaev::Layout {

struct FlexDimensionHelper {
    bool isRowOriented;

    FlexDimensionHelper(bool isRowOriented) : isRowOriented(isRowOriented) {}

    template <typename T>
    T &mainAxis(Math::Vec2<T> &value) const {
        return isRowOriented ? value.x : value.y;
    }

    template <typename T>
    T &crossAxis(Math::Vec2<T> &value) const {
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

    Size mainAxis(Cow<Sizing> sizing) const {
        return isRowOriented ? sizing->width : sizing->height;
    }

    Size crossAxis(Cow<Sizing> sizing) const {
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

    Vec2Px buildPair(Px main, Px cross) const {
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
    Frag *frag;
    Flex flexItemProps;
    FlexDimensionHelper fdh;

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

    FlexItem(Tree &t, Frag &f, bool isRowOriented)
        : frag(&f), flexItemProps(*f.style->flex), fdh(isRowOriented) {
        speculateValues(t, Input{Commit::NO});
        // TODO: not always we will need min/max content sizes,
        //       this can be lazy computed for performance gains
        computeContentSizes(t);
    }

    void commit() {
        frag->layout.margin.top = margin.top.unwrapOr(speculativeMargin.top);
        frag->layout.margin.start = margin.start.unwrapOr(speculativeMargin.start);
        frag->layout.margin.end = margin.end.unwrapOr(speculativeMargin.end);
        frag->layout.margin.bottom = margin.bottom.unwrapOr(speculativeMargin.bottom);
    }

    void computeContentSizes(Tree &t) {
        _speculateValues(
            t,
            {
                .commit = Commit::NO,
                .intrinsic = {IntrinsicSize::MAX_CONTENT, IntrinsicSize::AUTO},
                .knownSize = {NONE, NONE},
            },
            maxContentSize,
            maxContentMargin
        );
        _speculateValues(
            t,
            {
                .commit = Commit::NO,
                .intrinsic = {IntrinsicSize::MIN_CONTENT, IntrinsicSize::AUTO},
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
            return fdh.startCrossAxis(margin).unwrapOr(fdh.startCrossAxis(speculativeMargin));

        case START_MAIN:
            return fdh.startMainAxis(margin).unwrapOr(fdh.startMainAxis(speculativeMargin));

        case END_MAIN:
            return fdh.endMainAxis(margin).unwrapOr(fdh.endMainAxis(speculativeMargin));

        case END_CROSS:
            return fdh.endCrossAxis(margin).unwrapOr(fdh.endCrossAxis(speculativeMargin));

        case BOTH_MAIN:
            return fdh.startMainAxis(margin).unwrapOr(fdh.startMainAxis(speculativeMargin)) +
                   fdh.endMainAxis(margin).unwrapOr(fdh.endMainAxis(speculativeMargin));

        case BOTH_CROSS:
            return fdh.startCrossAxis(margin).unwrapOr(fdh.startCrossAxis(speculativeMargin)) +
                   fdh.endCrossAxis(margin).unwrapOr(fdh.endCrossAxis(speculativeMargin));
        }
    }

    bool hasAnyCrossMarginAuto() const {
        return (fdh.startCrossAxis(*frag->style->margin) == Width::Type::AUTO) or
               (fdh.endCrossAxis(*frag->style->margin) == Width::Type::AUTO);
    }

    Px getScaledFlexShrinkFactor() const {
        return flexBaseSize * Px{flexItemProps.shrink};
    }

    void _speculateValues(Tree &t, Input input, Vec2Px &speculativeSize, InsetsPx &speculativeMargin) {
        Output out = layout(t, *frag, input);
        speculativeSize = out.size;
        speculativeMargin = computeMargins(
            t,
            *frag,
            {
                .commit = Commit::NO,
                .containingBlock = speculativeSize,
            }
        );
    }

    void speculateValues(Tree &t, Input input) {
        _speculateValues(t, input, speculativeSize, speculativeMargin);
    }

    void computeFlexBaseSize(Tree &t, Frag &f, Px mainContainerSize) {
        // TODO: check specs
        if (flexItemProps.basis.type == FlexBasis::WIDTH) {
            if (flexItemProps.basis.width.type == Width::Type::VALUE) {
                flexBaseSize = resolve(t, f, flexItemProps.basis.width.value, mainContainerSize);
            } else if (flexItemProps.basis.width.type == Width::Type::AUTO) {
                flexBaseSize = fdh.mainAxis(speculativeSize);
            }
        }

        if (flexItemProps.basis.type == FlexBasis::Type::CONTENT and
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
            getMinMaxPrefferedSize(t, flexItemProps.isRowOriented(), true, containerSize),
            getMinMaxPrefferedSize(t, flexItemProps.isRowOriented(), false, containerSize)
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
            return resolve(
                t,
                *frag,
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
            contentContribution = fdh.mainAxis(minContentSize) + fdh.mainAxis(minContentMargin);
        else
            contentContribution = fdh.mainAxis(maxContentSize) + fdh.mainAxis(maxContentMargin);

        if (fdh.mainAxis(frag->style->sizing).type == Size::Type::LENGTH) {
            contentContribution = max(
                contentContribution,
                resolve(
                    t,
                    *frag,
                    fdh.mainAxis(frag->style->sizing).value,
                    fdh.mainAxis(containerSize)
                )
            );
        } else if (fdh.mainAxis(frag->style->sizing).type == Size::Type::MIN_CONTENT) {
            contentContribution = max(contentContribution, fdh.mainAxis(minContentSize) + fdh.mainAxis(minContentMargin));
        } else if (fdh.mainAxis(frag->style->sizing).type == Size::Type::AUTO and not isMin) {
            contentContribution = fdh.mainAxis(speculativeSize);
        } else {
            logWarn("not implemented");
        }

        if (flexItemProps.grow == 0)
            contentContribution = min(contentContribution, flexBaseSize);

        if (flexItemProps.shrink == 0)
            contentContribution = max(contentContribution, flexBaseSize);

        return clamp(
            contentContribution,
            getMinMaxPrefferedSize(t, fdh.isRowOriented, true, containerSize),
            getMinMaxPrefferedSize(t, fdh.isRowOriented, false, containerSize)
        );
    }

    void alignCrossFlexStart() {
        if (not hasAnyCrossMarginAuto()) {
            fdh.crossAxis(position) = getMargin(START_CROSS);
        }
    }

    void alignCrossFlexEnd(Px lineCrossSize) {
        if (not hasAnyCrossMarginAuto()) {
            fdh.crossAxis(position) = lineCrossSize - fdh.crossAxis(usedSize) - getMargin(END_CROSS);
        }
    }

    void alignCrossCenter(Px lineCrossSize) {
        if (not hasAnyCrossMarginAuto()) {
            Px startOfBlock = (lineCrossSize - fdh.crossAxis(usedSize) - getMargin(BOTH_CROSS)) / Px{2};
            fdh.crossAxis(position) = startOfBlock + getMargin(START_CROSS);
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

            auto elementSpeculativeCrossSize = lineCrossSize - getMargin(BOTH_CROSS);
            speculateValues(
                tree,
                {.commit = commit,
                 .knownSize = fdh.extractMainAxisAndFillOptOther(usedSize, elementSpeculativeCrossSize),
                 // TODO: not really sure of these arguments, check specs
                 .availableSpace = fdh.extractMainAxisAndFillOther(usedSize, elementSpeculativeCrossSize)
                }
            );

            fdh.crossAxis(position) = getMargin(START_CROSS);
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
    FlexDimensionHelper fdh;
    Vec2Px position;

    FlexLine(MutSlice<FlexItem> items, bool isRowOriented)
        : items(items), crossSize(0), fdh(isRowOriented) {}

    FlexLine(MutSlice<FlexItem> items)
        : items(items), crossSize(0), fdh(items[0].fdh.isRowOriented) {}

    void alignMainFlexStart() {
        Px currPositionMainAxis{0};
        for (auto &flexItem : items) {
            fdh.mainAxis(flexItem.position) = currPositionMainAxis + flexItem.getMargin(FlexItem::START_MAIN);
            currPositionMainAxis += fdh.mainAxis(flexItem.usedSize) + flexItem.getMargin(FlexItem::BOTH_MAIN);
        }
    }

    void alignMainFlexEnd(Px mainSize, Px occupiedSize) {
        Px currPositionMainAxis{mainSize - occupiedSize};
        for (auto &flexItem : items) {
            fdh.mainAxis(flexItem.position) = currPositionMainAxis + flexItem.getMargin(FlexItem::START_MAIN);
            currPositionMainAxis += fdh.mainAxis(flexItem.usedSize) + flexItem.getMargin(FlexItem::BOTH_MAIN);
        }
    }

    void alignMainSpaceAround(Px mainSize, Px occupiedSize) {
        Px gapSize = (mainSize - occupiedSize) / Px{items.len()};

        Px currPositionMainAxis{gapSize / Px{2}};
        for (auto &flexItem : items) {
            fdh.mainAxis(flexItem.position) = currPositionMainAxis + flexItem.getMargin(FlexItem::START_MAIN);
            currPositionMainAxis += fdh.mainAxis(flexItem.usedSize) + flexItem.getMargin(FlexItem::BOTH_MAIN) + gapSize;
        }
    }

    void alignMainSpaceBetween(Px mainSize, Px occupiedSize) {
        Px gapSize = (mainSize - occupiedSize) / Px{items.len() - 1};

        Px currPositionMainAxis{0};
        for (auto &flexItem : items) {
            fdh.mainAxis(flexItem.position) = currPositionMainAxis + flexItem.getMargin(FlexItem::START_MAIN);
            currPositionMainAxis += fdh.mainAxis(flexItem.usedSize) + flexItem.getMargin(FlexItem::BOTH_MAIN) + gapSize;
        }
    }

    void alignMainCenter(Px mainSize, Px occupiedSize) {
        Px currPositionMainAxis{(mainSize - occupiedSize) / Px{2}};
        for (auto &flexItem : items) {
            fdh.mainAxis(flexItem.position) = currPositionMainAxis + flexItem.getMargin(FlexItem::START_MAIN);
            currPositionMainAxis += fdh.mainAxis(flexItem.usedSize) + flexItem.getMargin(FlexItem::BOTH_MAIN);
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
    Flex _flex;
    FlexDimensionHelper fdh{_flex.isRowOriented()};

    // https://www.w3.org/TR/css-flexbox-1/#layout-algorithm

    // 1. MARK: Generate anonymous flex items ----------------------------------
    // https://www.w3.org/TR/css-flexbox-1/#algo-anon-box

    Vec<FlexItem> _items = {};

    void _generateAnonymousFlexItems(Tree &t, Frag &f) {
        _items.ensure(f.children().len());
        for (auto &c : f.children())
            _items.emplaceBack(t, c, _flex.isRowOriented());
    }

    // 2. MARK: Available main and cross space for the flex items --------------
    // https://www.w3.org/TR/css-flexbox-1/#algo-available

    Px _availableMainSpace = {};
    Px _initiallyAvailableCrossSpace = {};

    void _determineAvailableMainAndCrossSpace(
        Input input
    ) {
        // TODO: Consider refactor orientation after checking karm-math/Flow.h
        if (_flex.isRowOriented()) {
            _availableMainSpace = input.knownSize.x.unwrapOr(input.availableSpace.x);
            _initiallyAvailableCrossSpace = input.knownSize.y.unwrapOr(input.availableSpace.y);
        } else {
            // TODO: Implement other orientation cases
            logWarn("column orientation or reverse-row still not implemented");
            _availableMainSpace = input.knownSize.y.unwrapOr(input.availableSpace.y);
            _initiallyAvailableCrossSpace = input.knownSize.x.unwrapOr(input.availableSpace.x);
        }
    }

    // 3. MARK: Flex base size and hypothetical main size of each item ---------
    // https://www.w3.org/TR/css-flexbox-1/#algo-main-item

    void _determineFlexBaseSizeAndHypotheticalMainSize(
        Tree &t, Frag &f, Input input
    ) {
        for (auto &i : _items) {
            i.computeFlexBaseSize(
                t,
                f,
                _availableMainSpace
            );

            i.computeHypotheticalMainSize(
                t,
                {
                    _availableMainSpace,
                    _initiallyAvailableCrossSpace,
                }
            );

            // Speculate margins before following steps
            i.speculateValues(
                t,
                {
                    .commit = Commit::NO,
                    .knownSize = fdh.extractMainAxisAndFillOptOther(i.flexBaseSize),
                    .availableSpace = _flex.isRowOriented()
                                          ? Vec2Px{
                                                i.flexBaseSize,
                                                input.knownSize.y.unwrapOr(Px{0}),
                                            }
                                          : Vec2Px{
                                                input.knownSize.x.unwrapOr(Px{0}),
                                                i.flexBaseSize,
                                            },
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

    void _collectFlexItemsInfoFlexLinesNowWrap(Tree &t, Input input) {
        _lines.emplaceBack(_items);

        if (fdh.mainAxis(input.intrinsic) == IntrinsicSize::MIN_CONTENT or
            fdh.mainAxis(input.intrinsic) == IntrinsicSize::MAX_CONTENT) {

            Vec<Px> flexFraction;
            for (auto &flexItem : _items) {
                Px contribution = flexItem.getMainSizeMinMaxContentContribution(
                    t,
                    fdh.mainAxis(input.intrinsic) == IntrinsicSize::MIN_CONTENT,
                    fdh.buildPair(_availableMainSpace, _initiallyAvailableCrossSpace)
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

    void _collectFlexItemsInfoFlexLinesWrap(Tree &t, Input input) {
        if (fdh.mainAxis(input.intrinsic) == IntrinsicSize::MIN_CONTENT) {
            _lines.ensure(_items.len());
            Px largestMinContentContrib = Limits<Px>::MIN;
            usize si = 0;
            for (auto &flexItem : _items) {
                largestMinContentContrib = max(
                    largestMinContentContrib,
                    flexItem.getMainSizeMinMaxContentContribution(
                        t,
                        true,
                        {_availableMainSpace, _initiallyAvailableCrossSpace}
                    )
                );
                _lines.emplaceBack(mutSub(_items, si, si + 1));
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
                    if (currLineSize + itemContribution <= _availableMainSpace or currLineSize == Px{0}) {
                        currLineSize += itemContribution;
                        ei++;
                    } else
                        break;
                }

                _lines.pushBack({
                    mutSub(_items, si, ei),
                    fdh.isRowOriented,
                });

                si = ei;
            }
        }
    }

    void _collectFlexItemsIntoFlexLines(Tree &t, Input input) {
        if (_flex.wrap == FlexWrap::NOWRAP or fdh.mainAxis(input.intrinsic) == IntrinsicSize::MAX_CONTENT)
            _collectFlexItemsInfoFlexLinesNowWrap(t, input);
        else
            _collectFlexItemsInfoFlexLinesWrap(t, input);

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
                sumItemsHypotheticalMainSizes += flexItem.hypoMainSize + flexItem.getMargin(FlexItem::BOTH_MAIN);
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
                    fdh.mainAxis(flexItem.usedSize) = flexItem.hypoMainSize;
                    frozenItems.pushBack(&flexItem);
                    sumFrozenOuterSizes += fdh.mainAxis(flexItem.usedSize) + flexItem.getMargin(FlexItem::BOTH_MAIN);
                } else {
                    fdh.mainAxis(flexItem.usedSize) = flexItem.flexBaseSize;
                    unfrozenItems.pushBack(&flexItem);
                }
            }

            auto computeStats = [&]() {
                Px sumOfUnfrozenOuterSizes{0};
                Number sumUnfrozenFlexFactors{0};
                for (auto *flexItem : unfrozenItems) {
                    sumOfUnfrozenOuterSizes += flexItem->flexBaseSize + flexItem->getMargin(FlexItem::BOTH_MAIN);
                    sumUnfrozenFlexFactors += flexCaseIsGrow ? flexItem->flexItemProps.grow : flexItem->flexItemProps.shrink;
                }

                return Tuple<Px, Number>(sumOfUnfrozenOuterSizes, sumUnfrozenFlexFactors);
            };

            auto [sumUnfrozenOuterSizes, _] = computeStats();
            // FIXME: weird types of spaces and sizes here, since free space can be negative
            Number initialFreeSpace = Number{_usedMainSize} - Number{sumUnfrozenOuterSizes + sumFrozenOuterSizes};

            while (unfrozenItems.len()) {
                auto [sumUnfrozenOuterSizes, sumUnfrozenFlexFactors] = computeStats();
                auto freeSpace = Number{_usedMainSize} - Number{sumUnfrozenOuterSizes + sumFrozenOuterSizes};

                if (sumUnfrozenFlexFactors < 1 and Math::abs(initialFreeSpace * sumUnfrozenFlexFactors) < Math::abs(freeSpace))
                    freeSpace = initialFreeSpace * sumUnfrozenFlexFactors;

                if (flexCaseIsGrow) {
                    for (auto *flexItem : unfrozenItems) {
                        Number ratio = flexItem->flexItemProps.grow / sumUnfrozenFlexFactors;

                        fdh.mainAxis(flexItem->usedSize) = flexItem->flexBaseSize + Px{ratio * freeSpace};
                    }
                } else {
                    Px sumScaledFlexShrinkFactor{0};
                    for (auto *flexItem : unfrozenItems) {
                        sumScaledFlexShrinkFactor += flexItem->getScaledFlexShrinkFactor();
                    }

                    for (auto *flexItem : unfrozenItems) {
                        Px ratio = flexItem->getScaledFlexShrinkFactor() / sumScaledFlexShrinkFactor;
                        fdh.mainAxis(flexItem->usedSize) = flexItem->flexBaseSize - ratio * Px{Math::abs(freeSpace)};
                    }
                }

                Px totalViolation{0};

                auto clampAndFloorContentBox = [&](FlexItem *flexItem) {
                    // FIXME: it seems that the browser sets minSize = max(minSize, textSize)
                    auto clampedSize = clamp(
                        fdh.mainAxis(flexItem->usedSize),
                        flexItem->getMinMaxPrefferedSize(t, fdh.isRowOriented, true, fdh.buildPair(_availableMainSpace, _initiallyAvailableCrossSpace)),
                        flexItem->getMinMaxPrefferedSize(t, fdh.isRowOriented, false, fdh.buildPair(_availableMainSpace, _initiallyAvailableCrossSpace))
                    );

                    // TODO: should consider padding and border so content size is not negative
                    auto minSizeFlooringContentSizeAt0 = Px{0};

                    return max(clampedSize, minSizeFlooringContentSizeAt0);
                };

                // TODO: assuming row orientation here

                for (auto *flexItem : unfrozenItems) {
                    Px clampedSize = clampAndFloorContentBox(flexItem);
                    totalViolation += clampedSize - fdh.mainAxis(flexItem->usedSize);
                }
                for (auto *flexItem : frozenItems) {
                    Px clampedSize = clampAndFloorContentBox(flexItem);
                    totalViolation += clampedSize - fdh.mainAxis(flexItem->usedSize);
                }

                if (totalViolation == Px{0}) {
                    Px soma{0};
                    for (usize i = 0; i < unfrozenItems.len(); ++i) {
                        auto *flexItem = unfrozenItems[i];
                        Px clampedSize = clampAndFloorContentBox(flexItem);
                        fdh.mainAxis(flexItem->usedSize) = clampedSize;
                        soma += clampedSize + flexItem->getMargin(FlexItem::BOTH_MAIN);
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

                            if (clampedSize < fdh.mainAxis(flexItem->usedSize))
                                indexesToFreeze.pushBack(i);
                        }
                    } else {
                        for (usize i = 0; i < unfrozenItems.len(); ++i) {
                            auto *flexItem = unfrozenItems[i];
                            Px clampedSize = clampAndFloorContentBox(flexItem);

                            if (clampedSize > fdh.mainAxis(flexItem->usedSize))
                                indexesToFreeze.pushBack(i);
                        }
                    }
                    for (usize i = 0; i < unfrozenItems.len(); ++i) {
                        auto *flexItem = unfrozenItems[i];
                        Px clampedSize = clampAndFloorContentBox(flexItem);
                        fdh.mainAxis(flexItem->usedSize) = clampedSize;
                    }

                    for (int j = indexesToFreeze.len() - 1; j >= 0; j--) {
                        usize i = indexesToFreeze[j];

                        sumFrozenOuterSizes += fdh.mainAxis(unfrozenItems[i]->usedSize) + unfrozenItems[i]->getMargin(FlexItem::BOTH_MAIN);
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

    void _determineHypotheticalCrossSize(Tree &t, Input input) {
        // TODO: once again, this was coded assuming a ROW orientation
        for (auto &i : _items) {
            Px availableCrossSpace = fdh.crossAxis(input.knownSize).unwrapOr(Px{0}) - i.getMargin(FlexItem::BOTH_CROSS);

            if (fdh.mainAxis(i.frag->style->sizing) == Size::AUTO)
                fdh.mainAxis(input.intrinsic) = IntrinsicSize::STRETCH_TO_FIT;

            i.speculateValues(
                t,
                {
                    .commit = input.commit,
                    .knownSize = fdh.extractMainAxisAndFillOptOther(i.usedSize),
                    .availableSpace = fdh.extractMainAxisAndFillOther(i.usedSize, availableCrossSpace),
                }
            );
        }
    }

    // 8. MARK: Calculate the cross size of each flex line ---------------------
    // https://www.w3.org/TR/css-flexbox-1/#algo-cross-line

    void _calculateCrossSizeOfEachFlexLine(Input input) {
        if (_lines.len() == 1 and fdh.crossAxis(input.knownSize)) {
            first(_lines).crossSize = fdh.crossAxis(input.knownSize).unwrap();
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
                    maxOuterHypCrossSize = max(maxOuterHypCrossSize, fdh.crossAxis(flexItem.speculativeSize) + flexItem.getMargin(FlexItem::BOTH_CROSS));
                }
            }
            flexLine.crossSize = max(maxOuterHypCrossSize, maxDistBaselineCrossStartEdge + maxDistBaselineCrossEndEdge);
        }
    }

    // 9. MARK: Handle 'align-content: stretch' --------------------------------
    // https://www.w3.org/TR/css-flexbox-1/#algo-line-stretch

    void _handleAlignContentStretch(Frag &f) {
        // FIXME: If the flex container has a definite cross size <=?=> f.style->sizing->height.type != Size::Type::AUTO
        if (f.style->sizing->height.type != Size::Type::AUTO and
            f.style->aligns.alignContent == Style::Align::STRETCH) {
            Px sumOfCrossSizes{0};
            for (auto &flexLine : _lines)
                sumOfCrossSizes += flexLine.crossSize;
            if (_initiallyAvailableCrossSpace > sumOfCrossSizes) {
                Px toDistribute = (_initiallyAvailableCrossSpace - sumOfCrossSizes) / Px{_lines.len()};
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

    void _determineUsedCrossSize(Tree &t, Frag &f) {
        for (auto &flexLine : _lines) {
            for (auto &flexItem : flexLine.items) {
                Style::Align itemAlign = flexItem.frag->style->aligns.alignSelf;
                if (itemAlign == Style::Align::AUTO)
                    itemAlign = f.style->aligns.alignItems;

                if (
                    itemAlign == Style::Align::STRETCH and
                    flexItem.frag->style->sizing->height.type == Size::AUTO and
                    not flexItem.hasAnyCrossMarginAuto()
                ) {
                    fdh.crossAxis(flexItem.usedSize) = flexLine.crossSize - flexItem.getMargin(FlexItem::BOTH_CROSS);

                    fdh.crossAxis(flexItem.usedSize) = clamp(
                        fdh.crossAxis(flexItem.usedSize),
                        flexItem.getMinMaxPrefferedSize(
                            t,
                            not fdh.isRowOriented,
                            true,
                            fdh.buildPair(_availableMainSpace, _initiallyAvailableCrossSpace)
                        ),
                        flexItem.getMinMaxPrefferedSize(
                            t,
                            not fdh.isRowOriented,
                            false,
                            fdh.buildPair(_availableMainSpace, _initiallyAvailableCrossSpace)
                        )
                    );
                } else {
                    fdh.crossAxis(flexItem.usedSize) = fdh.crossAxis(flexItem.speculativeSize);
                }
            }
        }
    }

    // 12. MARK: Distribute any remaining free space ---------------------------
    // https://www.w3.org/TR/css-flexbox-1/#algo-main-align

    void _distributeRemainingFreeSpace(Frag &f, Input input) {
        for (auto &flexLine : _lines) {
            Px occupiedMainSize{0};
            for (auto &flexItem : flexLine.items) {
                occupiedMainSize += fdh.mainAxis(flexItem.usedSize) + flexItem.getMargin(FlexItem::BOTH_MAIN);
            }

            bool usedAutoMargins = false;
            if (_usedMainSize > occupiedMainSize) {
                usize countOfAutos = 0;

                for (auto &flexItem : flexLine.items) {
                    countOfAutos += (fdh.startMainAxis(*flexItem.frag->style->margin) == Width::AUTO);
                    countOfAutos += (fdh.endMainAxis(*flexItem.frag->style->margin) == Width::AUTO);
                }

                if (countOfAutos) {
                    Px marginsSize = (_usedMainSize - occupiedMainSize) / Px{countOfAutos};
                    for (auto &flexItem : flexLine.items) {
                        if (fdh.startMainAxis(*flexItem.frag->style->margin) == Width::AUTO)
                            fdh.startMainAxis(flexItem.margin) = marginsSize;
                        if (fdh.endMainAxis(*flexItem.frag->style->margin) == Width::AUTO)
                            fdh.endMainAxis(flexItem.margin) = marginsSize;
                    }

                    usedAutoMargins = true;
                    occupiedMainSize = _usedMainSize;
                }
            }

            if (not usedAutoMargins) {
                for (auto &flexItem : flexLine.items) {
                    if (fdh.startMainAxis(*flexItem.frag->style->margin) == Width::AUTO)
                        fdh.startMainAxis(flexItem.margin) = Px{0};
                    if (fdh.endMainAxis(*flexItem.frag->style->margin) == Width::AUTO)
                        fdh.endMainAxis(flexItem.margin) = Px{0};
                }
            }

            if (input.commit == Commit::YES) {
                // This is done after any flexible lengths and any auto margins have been resolved.
                // NOTE: justifying doesnt change sizes/margins, thus will only run when committing and setting positions
                auto justifyContent = f.style->aligns.justifyContent.keyword;
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

                auto marginStyle = *i.frag->style->margin;

                bool startCrossMarginIsAuto = fdh.startCrossAxis(marginStyle) == Width::AUTO;
                bool endCrossMarginIsAuto = fdh.endCrossAxis(marginStyle) == Width::AUTO;

                if (startCrossMarginIsAuto or endCrossMarginIsAuto) {
                    if (fdh.crossAxis(i.usedSize) + i.getMargin(FlexItem::BOTH_CROSS) < l.crossSize) {
                        if (not startCrossMarginIsAuto and endCrossMarginIsAuto) {
                            auto startMargin = i.getMargin(FlexItem::START_CROSS);
                            Px freeCrossSpace = l.crossSize - fdh.crossAxis(i.usedSize) - startMargin;
                            fdh.endCrossAxis(i.margin) = freeCrossSpace;

                        } else if (startCrossMarginIsAuto and not endCrossMarginIsAuto) {
                            auto endMargin = i.getMargin(FlexItem::END_CROSS);
                            Px freeCrossSpace = l.crossSize - fdh.crossAxis(i.usedSize) - endMargin;
                            fdh.startCrossAxis(i.margin) = freeCrossSpace;
                        } else {
                            Px freeCrossSpace = l.crossSize - fdh.crossAxis(i.usedSize);
                            fdh.endCrossAxis(i.margin) = fdh.startCrossAxis(i.margin) = freeCrossSpace / Px{2};
                        }
                        fdh.crossAxis(i.position) = i.getMargin(FlexItem::START_CROSS);
                    } else {
                        if (i.frag->style->margin->top == Width::Type::AUTO)
                            fdh.startCrossAxis(i.margin) = Px{0};

                        // FIXME: not sure if the following is what the specs want
                        if (l.crossSize > fdh.crossAxis(i.usedSize))
                            fdh.endCrossAxis(i.margin) = l.crossSize - fdh.crossAxis(i.usedSize);
                    }
                }
            }
        }
    }

    // 14. MARK: Align all flex items ------------------------------------------
    // https://www.w3.org/TR/css-flexbox-1/#algo-cross-align

    void _alignAllFlexItems(Tree &t, Frag &f, Input input) {
        for (auto &flexLine : _lines) {
            for (auto &flexItem : flexLine.items) {
                flexItem.alignItem(
                    t,
                    input.commit,
                    flexLine.crossSize,
                    f.style->aligns.alignItems.keyword
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

        if (fdh.crossAxis(input.knownSize))
            _usedCrossSize = fdh.crossAxis(input.knownSize).unwrap();
        else
            _usedCrossSize = _usedCrossSizeByLines;

        // TODO: clamp usedCrossSize
    }

    // 16. MARK: Align all flex lines ------------------------------------------
    // https://www.w3.org/TR/css-flexbox-1/#algo-line-align

    void _alignAllFlexLines(Frag &f) {
        Px availableCrossSpace = _initiallyAvailableCrossSpace - _usedCrossSizeByLines;

        auto alignContentFlexStart = [&]() {
            Px currPositionCross{0};
            for (auto &flexLine : _lines) {
                fdh.crossAxis(flexLine.position) = currPositionCross;
                currPositionCross += flexLine.crossSize;
            }
        };

        auto alignContentCenter = [&]() {
            Px currPositionCross{availableCrossSpace / Px{2}};
            for (auto &flexLine : _lines) {
                fdh.crossAxis(flexLine.position) = currPositionCross;
                currPositionCross += flexLine.crossSize;
            }
        };

        switch (f.style->aligns.alignContent.keyword) {
        case Style::Align::FLEX_END: {
            Px currPositionCross{availableCrossSpace};
            for (auto &flexLine : _lines) {
                fdh.crossAxis(flexLine.position) = currPositionCross;
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
                    fdh.crossAxis(flexLine.position) = currPositionCross;
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
                    fdh.crossAxis(flexLine.position) = currPositionCross;
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

    void _commit(Tree &t, Input input) {
        // NOTE: Flex items positions are relative to their flex lines;
        //       however, since flex lines are virtual elements,
        //       items positions need to be adapted before committing
        //       to refer to the flex container. This however wont change
        //       the sizes, and will be done only when committing
        for (auto &flexLine : _lines) {
            for (auto &flexItem : flexLine.items) {
                flexItem.position = flexItem.position + flexLine.position + input.position;

                auto out = layout(
                    t,
                    *flexItem.frag,
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
    Output run(Tree &t, Frag &f, Input input) {
        // 1. Generate anonymous flex items
        _generateAnonymousFlexItems(t, f);

        // 2. Determine the available main and cross space for the flex items.
        _determineAvailableMainAndCrossSpace(input);

        // 3. Determine the flex base size and hypothetical main size of each item
        _determineFlexBaseSizeAndHypotheticalMainSize(t, f, input);

        // 4. Determine the main size of the flex container
        _determineMainSize(input);

        // 5. Collect flex items into flex lines
        _collectFlexItemsIntoFlexLines(t, input);

        // 6. Resolve the flexible lengths
        _resolveFlexibleLengths(t);

        // 7. Determine the hypothetical cross size of each item
        _determineHypotheticalCrossSize(t, input);

        // 8. Calculate the cross size of each flex line
        _calculateCrossSizeOfEachFlexLine(input);

        // 9. Handle 'align-content: stretch'.
        _handleAlignContentStretch(f);

        // 10. Collapse visibility:collapse items.
        _collapseVisibilityCollapseItems();

        // 11. Determine the used cross size of each flex item.
        _determineUsedCrossSize(t, f);

        // 12. Distribute any remaining free space
        _distributeRemainingFreeSpace(f, input);

        // 13. Resolve cross-axis auto margins.
        _resolveCrossAxisAutoMargins();

        // 14. Align all flex items along the cross-axis.
        _alignAllFlexItems(t, f, input);

        // 15. Determine the flex container's used cross size
        _determineFlexContainerUsedCrossSize(input);

        // 16. Align all flex lines
        _alignAllFlexLines(f);

        // XX. Commit
        if (input.commit == Commit::YES)
            _commit(t, input);

        return Output::fromSize({_usedMainSize, _usedCrossSize});
    }
};

Output flexLayout(Tree &t, Frag &f, Input input) {
    FlexFormatingContext fc = {*f.style->flex};
    return fc.run(t, f, input);
}

} // namespace Vaev::Layout
