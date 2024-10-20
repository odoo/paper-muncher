#include "block.h"

#include "float.h"
#include "frag.h"

namespace Vaev::Layout {

static Px _blockLayoutDetermineWidth(Tree &t, Frag &f, Input input) {
    Px width = Px{0};
    for (auto &c : f.children()) {
        auto ouput = layout(
            t,
            c,
            {
                .commit = Commit::NO,
                .intrinsic = input.intrinsic,
            }
        );

        width = max(width, ouput.size.x);
    }

    return width;
}

bool intersect(Px a0, Px a1, Px b0, Px b1) {
    // a and b intersect?
    if (a0 > a1)
        std::swap(a0, a1);
    if (b0 > b1)
        std::swap(b0, b1);
    if (a0 > b0) {
        std::swap(a0, b0);
        std::swap(a1, b1);
    }

    return b0 <= a1;
}

bool inside(Px x, Px a, Px b) {
    return x >= min(a, b) and x <= max(a, b);
}

struct PossiblePositions {
    Vec<Px> positions;
    usize pos = 0;

    PossiblePositions(FloatManager &floatManager, usize floatIdsBelow) {
        // lowest top and all bottoms below this top
        Px lowestTop{0};
        for (auto el : floatManager.leftFloatElements) {
            if (floatManager.floatId.get(el) >= floatIdsBelow)
                continue;
            lowestTop = max(lowestTop, floatManager.placedFloat.get(el).top());
        }
        for (auto el : floatManager.rightFloatElements) {
            if (floatManager.floatId.get(el) >= floatIdsBelow)
                continue;
            lowestTop = max(lowestTop, floatManager.placedFloat.get(el).top());
        }

        positions.pushBack(lowestTop);
        for (auto el : floatManager.leftFloatElements) {
            if (floatManager.floatId.get(el) >= floatIdsBelow)
                continue;
            if (floatManager.placedFloat.get(el).bottom() > lowestTop)
                positions.pushBack(floatManager.placedFloat.get(el).bottom() + Px{1});
        }
        for (auto el : floatManager.rightFloatElements) {
            if (floatManager.floatId.get(el) >= floatIdsBelow)
                continue;
            if (floatManager.placedFloat.get(el).bottom() > lowestTop)
                positions.pushBack(floatManager.placedFloat.get(el).bottom() + Px{1});
        }

        std::sort(positions.buf(), positions.buf() + positions.len());
    }

    bool empty() {
        return pos == positions.len();
    }

    Opt<Px> next() {
        if (pos < positions.len())
            return positions[pos++];
        return NONE;
    }

    Opt<Px> get() {
        if (pos < positions.len())
            return positions[pos];
        return NONE;
    }
};

struct Block {
    FloatManager &floatManager;

    Opt<RectPx> lastPlacedInline;
    Px lowerOuterBottomFromBlock, lowerOuterTopFromBlock;
    Px inlineSize{0}, lowerYposition{0}, lowerYFloatPosition{0};
    Input defaultChildInput, defaultSpeculativeChildInput;

    Block(Input input, Px inlineSize, FloatManager &floatManager) : floatManager(floatManager),
                                                                    lowerOuterBottomFromBlock(input.position.y),
                                                                    lowerOuterTopFromBlock(input.position.y),
                                                                    inlineSize(inlineSize),
                                                                    lowerYposition(input.position.y) {
        defaultChildInput = {
            .commit = input.commit,
            .availableSpace = {inlineSize, Px{0}},
            .containingBlock = {inlineSize, Px{0}},
        };

        defaultSpeculativeChildInput = Input{defaultChildInput};
        defaultSpeculativeChildInput.commit = Commit::NO;
    }

    Input prepareChildInput(Frag &c, Vec2Px marginBoxPosition, InsetsPx &margin) {
        Input childInput{defaultChildInput};

        Opt<Px> childInlineSize = NONE;
        if (c.style->sizing->width == Size::AUTO and c.style->display != Display::Outside::INLINE)
            childInlineSize = inlineSize;

        if (c.style->position != Position::ABSOLUTE) {
            childInput.knownSize.width = childInlineSize;
        }

        childInput.position = marginBoxPosition + margin.topStart();

        return childInput;
    }

    Px nextYPositionFromBlockAndInline() {

        if (lastPlacedInline == NONE)
            return lowerOuterBottomFromBlock;

        return max(lastPlacedInline.unwrap().bottom(), lowerOuterBottomFromBlock);
    }

    Tuple<Px, Px> occupiedHorForAndBetweenAllFloats(Input input, Px top, Px bottom, usize floatIdsBelow) {
        // this assumes floats occupy a continuos piece on beggining and end
        Px start{input.position.x};
        for (auto placedEl : floatManager.leftFloatElements) {
            if (floatManager.floatId.get(placedEl) >= floatIdsBelow)
                continue;

            auto placedFloat = floatManager.placedFloat.get(placedEl);

            if (intersect(top, bottom, placedFloat.top(), placedFloat.bottom())) {
                start = max(placedFloat.end() + Px{1}, start);
            }
        }

        Px end{input.position.x + inlineSize};
        for (auto placedEl : floatManager.rightFloatElements) {
            if (floatManager.floatId.get(placedEl) >= floatIdsBelow)
                continue;
            auto placedFloat = floatManager.placedFloat.get(placedEl);
            if (intersect(top, bottom, placedFloat.top(), placedFloat.bottom())) {
                end = min(placedFloat.start() - Px{1}, end);
            }
        }

        return {start, end};
    }

    Px availableSpaceForPositionFromAllFloats(Input input, Px top, Px bottom, usize floatIdsBelow) {
        auto [start, end] = occupiedHorForAndBetweenAllFloats(input, top, bottom, floatIdsBelow);
        return end - start + Px{1};
    };

    Opt<Px> bottomEdgeFloats(usize floatIdsBelow, Vec<Frag *> &floats) {
        Opt<Px> ret;

        for (auto placedEl : floats) {
            if (floatManager.floatId.get(placedEl) >= floatIdsBelow)
                continue;

            auto placedFloat = floatManager.placedFloat.get(placedEl);

            ret = max(ret.unwrapOr(Px{0}), placedFloat.bottom());
        }
        return ret;
    }

    Opt<Px> bottomEdgeRightFloats(usize floatIdsBelow) {
        return bottomEdgeFloats(floatIdsBelow, floatManager.rightFloatElements);
    }

    Opt<Px> getClearConstraintPosition(Clear clear, usize floatIdsBelow) {
        Opt<Px> out;
        if (clear == Clear::LEFT or clear == Clear::BOTH)
            out = bottomEdgeFloats(floatIdsBelow, floatManager.leftFloatElements);
        if (clear == Clear::RIGHT or clear == Clear::BOTH) {
            out = max(out.unwrapOr(Px{0}), bottomEdgeFloats(floatIdsBelow, floatManager.rightFloatElements));
        }
        return out;
    }

    Vec2Px computePositionForFloat(Frag &c, Vec2Px size, Input input, InsetsPx margin, bool isLeft) {
        // although a new block can be placed at the same height as a prev float, the converse is not true i.e. a new float
        // goes to the bottom of the prev block
        PossiblePositions floatPositions(floatManager, floatManager.floatId.get(&c));

        Px positionY = max(
            lastPlacedInline ? lastPlacedInline.unwrap().bottom() + Px{1} : input.position.y,
            lowerOuterBottomFromBlock + Px{1},
            floatPositions.next().unwrapOr(input.position.y)
        );

        auto floatId = floatManager.floatId.get(&c);

        auto marginBoxHeight = size.y + margin.vertical();
        auto marginBoxWidth = size.x + margin.horizontal();
        while (true) {

            Px availableWidth = availableSpaceForPositionFromAllFloats(input, positionY, positionY + size.y + margin.vertical(), floatId);
            if (marginBoxWidth > availableWidth) {
                if (floatPositions.empty())
                    panic("xii...");
                positionY = floatPositions.next().unwrapOr(Px{0});

            } else
                break;
        }

        Px startPositionX{input.position.x};

        auto [start, end] = occupiedHorForAndBetweenAllFloats(input, positionY, positionY + marginBoxHeight, floatId);
        if (isLeft)
            startPositionX = start + Px{1};
        else
            startPositionX = end - marginBoxWidth;

        return {startPositionX, positionY};
    }

    Output placeInline(Tree &t, Frag &c, Vec2Px position, InsetsPx margin) {
        Input inlineChildInput = prepareChildInput(c, position, margin);

        auto output = layout(
            t,
            c,
            inlineChildInput
        );

        lowerYposition = max(
            lowerYposition,
            inlineChildInput.position.y + output.size.y + margin.bottom
        );

        lastPlacedInline = RectPx{position, output.size + margin.all()};
        return output;
    }

    void placeBlock(Tree &t, Frag &c, Vec2Px position, InsetsPx &margin) {
        Input blockChildInput = prepareChildInput(c, position, margin);

        auto output = layout(
            t,
            c,
            blockChildInput
        );

        lowerYposition = max(
            lowerYposition,
            blockChildInput.position.y + output.size.y + margin.bottom
        );

        lowerOuterBottomFromBlock = blockChildInput.position.y + output.size.y + margin.bottom;
        lowerOuterTopFromBlock = blockChildInput.position.y - margin.top;
    }
};

Output blockLayout(Tree &t, Frag &f, Input input) {

    Px inlineSize = f.style->display == Display::Outside::INLINE
                        ? _blockLayoutDetermineWidth(t, f, input)
                        : input.knownSize.width.unwrapOrElse([&] {
                              return _blockLayoutDetermineWidth(t, f, input);
                          });

    Block block(input, inlineSize, t.floatManager);

    t.floatManager.remove(f);

    auto children = f.children();
    usize i = 0;

    while (i < children.len()) {
        auto &c = children[i];

        auto margin = computeMargins(t, c, block.defaultChildInput);

        if (c.style->position == Position::ABSOLUTE) {
            Input absChildInput = block.prepareChildInput(c, Vec2Px{input.position.x, block.nextYPositionFromBlockAndInline()}, margin);
            layout(
                t,
                c,
                absChildInput
            );
        } else if (c.style->float_ != Float::NONE) {

            auto size = layout(t, c, block.defaultSpeculativeChildInput).size;

            Vec2Px position = block.computePositionForFloat(c, size, input, margin, c.style->float_ == Float::LEFT);

            auto marginBox = t.floatManager.placeFloat(t, c, block.prepareChildInput(c, position, margin));

            block.lowerYFloatPosition = max(block.lowerYFloatPosition, marginBox.bottom());

        } else if (c.style->display == Display::Outside::BLOCK) {
            Px positionY = max(
                block.lastPlacedInline ? block.lastPlacedInline.unwrap().bottom() + Px{1} : input.position.y,
                block.lowerOuterBottomFromBlock + Px{1}
            );

            auto id = t.floatManager.floatId.get(&c);
            positionY = max(positionY, block.getClearConstraintPosition(c.style->clear, id).unwrapOr(positionY));

            block.placeBlock(t, c, {input.position.x, positionY}, margin);

        } else if (c.style->display == Display::Outside::INLINE) {
            auto size = layout(t, c, block.defaultSpeculativeChildInput).size;
            // either we try to put it after an already placed inline element
            //     dont worry, its assured that no float would be placed here
            // or we add it in a new line

            if (block.lastPlacedInline and block.lastPlacedInline.unwrap().top() >= block.lowerOuterBottomFromBlock + Px{1}) {
                // try to add after prev inline
                Px positionY = block.lastPlacedInline.unwrap().top();

                Px verticalSize{size.y + margin.vertical()};
                auto neededHorizontalSpace = size.x + margin.horizontal();

                auto [start, end] = block.occupiedHorForAndBetweenAllFloats(input, positionY, positionY + verticalSize, t.floatManager.floatId.get(&c));
                start = max(start, block.lastPlacedInline.unwrap().end());
                if (neededHorizontalSpace < end - start + Px{1}) {
                    block.placeInline(t, c, {start, positionY}, margin);

                    i++;
                    continue;
                }
            }

            Px positionY = block.lowerOuterBottomFromBlock + Px{1};

            Px verticalSize{size.y + margin.vertical()};

            auto neededHorizontalSpace = size.x + margin.horizontal();

            if (block.availableSpaceForPositionFromAllFloats(input, positionY, positionY + verticalSize, t.floatManager.floatId.get(&c)) < neededHorizontalSpace) {
                Px toppestPositionY = Limits<Px>::MAX;
                for (auto placedEl : t.floatManager.leftFloatElements) {
                    auto placedFloat = t.floatManager.placedFloat.get(placedEl);
                    Px suggestion = placedFloat.bottom() + Px{1};
                    if (block.availableSpaceForPositionFromAllFloats(input, suggestion, suggestion + verticalSize, t.floatManager.floatId.get(&c)) < neededHorizontalSpace) {
                        if (suggestion >= positionY)
                            toppestPositionY = min(toppestPositionY, suggestion);
                    }
                }

                for (auto placedEl : t.floatManager.rightFloatElements) {
                    auto placedFloat = t.floatManager.placedFloat.get(placedEl);
                    Px suggestion = placedFloat.bottom() + Px{1};
                    if (block.availableSpaceForPositionFromAllFloats(input, suggestion, suggestion + verticalSize, t.floatManager.floatId.get(&c)) < neededHorizontalSpace) {
                        if (suggestion >= positionY)
                            toppestPositionY = min(toppestPositionY, suggestion);
                    }
                }

                if (toppestPositionY != Limits<Px>::MAX)
                    positionY = toppestPositionY;
            }

            Px availableSpace = block.availableSpaceForPositionFromAllFloats(input, positionY, positionY + verticalSize, t.floatManager.floatId.get(&c));

            // after position is found, we may lay other blocks, as long as their width fit or they are not blocks
            Px inlineAccuSizes{neededHorizontalSpace};
            usize j = i + 1;
            while (j < children.len()) {
                auto &cj = children[j];

                auto sizeHanging = layout(t, cj, block.defaultSpeculativeChildInput).size;
                auto marginHanging = computeMargins(t, cj, block.defaultChildInput);

                auto verticalSizeHanging{sizeHanging.y + marginHanging.vertical()};

                auto neededHorizontalSpace = sizeHanging.x + marginHanging.horizontal();

                if (cj.style->position == Position::ABSOLUTE) {
                    j++;
                    continue;
                } else if (cj.style->float_ != Float::NONE) {

                    auto position = block.computePositionForFloat(
                        cj,
                        size,
                        input,
                        marginHanging,
                        children[j].style->float_ == Float::LEFT
                    );

                    auto occupiedPositions = block.occupiedHorForAndBetweenAllFloats(input, position.y, verticalSizeHanging, t.floatManager.floatId.get(&cj));

                    if (inlineAccuSizes + neededHorizontalSpace > occupiedPositions.v1 - occupiedPositions.v0) {
                        break;
                    }

                    if (intersect(position.y, position.y + verticalSizeHanging, positionY, positionY + verticalSize)) {
                        if (inlineAccuSizes + neededHorizontalSpace <= availableSpace) {
                            t.floatManager.placeFloat(t, cj, block.prepareChildInput(cj, position, marginHanging));
                        } else {
                            // not putting j
                            break;
                        }
                    } else {
                        t.floatManager.placeFloat(t, cj, block.prepareChildInput(cj, position, marginHanging));
                    }
                } else if (cj.style->display == Display::Outside::BLOCK) {
                    // not putting on the same line
                    break;
                } else if (cj.style->display == Display::Outside::INLINE) {
                    // actually the available space for the n-th inline can be smaller then the availablespace for the
                    // first inline

                    if (inlineAccuSizes + neededHorizontalSpace > availableSpace)
                        break;
                    inlineAccuSizes += neededHorizontalSpace;
                }
                j++;
            }

            Px currPositionX{input.position.x};
            for (auto placedEl : t.floatManager.leftFloatElements) {
                // TODO: filter ID
                auto placedFloat = t.floatManager.placedFloat.get(placedEl);

                if (intersect(positionY, positionY + size.y + margin.vertical(), placedFloat.top(), placedFloat.bottom())) {
                    currPositionX = max(placedFloat.end() + Px{1}, currPositionX);
                }
            }

            while (i < j) {
                auto margin = computeMargins(t, children[i], block.defaultChildInput);

                if (children[i].style->display != Display::Outside::INLINE) {
                    i++;
                    continue;
                }

                auto output = block.placeInline(t, children[i], Vec2Px{currPositionX, positionY}, margin);

                currPositionX += output.size.x + margin.horizontal();

                i++;
            }

            i--;
        } else {
            logWarn("not implemented");
        }

        i++;
    }

    Px lowerYPosition = f.style->float_ == Float::NONE ? block.lowerYposition : block.lowerYFloatPosition;

    return Output::fromSize({
        inlineSize,
        max(input.knownSize.y.unwrapOr(Px{0}), lowerYPosition - input.position.y),
    });
}

} // namespace Vaev::Layout
