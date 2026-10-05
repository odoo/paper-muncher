export module Vaev.Engine:layout.flex2;

import Karm.Core;
import Karm.Math;
import :layout.base;
import :layout.values;
import :layout.sizing;

using namespace Karm;

namespace Vaev::Layout::Unstable {

// https://www.w3.org/TR/css-flexbox-1/#flex-items
struct FlexItem {
    Box& box;
    Au baseSize = 0_au;
    Au hypotheticalMainSize = 0_au;
};

struct Flex2FormatingContent : FormatingContext {
    Vec<FlexItem> _items = {};
    Axis _mainAxis = Axis::HORIZONTAL;

    void build(Tree&, Box& box) override {
        // https://www.w3.org/TR/css-flexbox-1/#axis-mapping
        if (oneOf(box.style->flex->direction, FlexDirection::ROW, FlexDirection::ROW_REVERSE)) {
            _mainAxis = Axis::HORIZONTAL;
        } else {
            _mainAxis = Axis::VERTICAL;
        }
    }

    // https://www.w3.org/TR/css-flexbox-1/#algo-anon-box
    void _generateAnonymousFlexItems(Box& box) {
        // Each in-flow child of a flex container becomes a flex item, and each
        // child text sequence is wrapped in an anonymous block container flex item.
        // However, if the entire text sequences contains only document white space
        // characters (i.e. characters that can be affected by the white-space property)
        // it is instead not rendered (just as if its text nodes were display:none).
        // FIXME: Ensure that the text merging and whitespace pruning already done
        //        during box building.

        _items.ensure(box.children().len());

        for (auto& child : box.children()) {
            // TODO:
            // - float and clear do not create floating or clearance of flex item,
            //   and do not take it out-of-flow.
            if (not box.isRemovedFromFlow()) {
                _items.pushBack(FlexItem{.box = child});
            }
        }
    }

    // https://www.w3.org/TR/css-flexbox-1/#algo-available
    AvailableSpace _determineAvailableSpaceForFlexItems(Tree& tree, Box& box, Input const& input) {
        auto const& style = *box.style;

        AvailableSpace availableSize = {};

        if (auto [inline_] = input.knownSize.x) {
            availableSize.width = inline_;
        } else {
            availableSize.width = style.sizing->width.visit(
                // if that dimension of the flex container’s content box is a definite size, use that;
                [&](Calc<PercentOr<Length>> const& calc) -> AvailableSpaceAxis {
                    // FIXME: Correctly handle indefinite percents.
                    return resolve(tree, box, calc, input.containingBlock.width.unwrapOr(0_au));
                },

                // if that dimension of the flex container is being sized under a min or max-content constraint,
                // the available space in that dimension is that constraint;
                [&](Keywords::MinContent) -> AvailableSpaceAxis {
                    return MIN_CONTENT;
                },
                [&](Keywords::MaxContent) -> AvailableSpaceAxis {
                    return MAX_CONTENT;
                },

                // otherwise, subtract the flex container’s margin, border, and padding from the space available
                // to the flex container in that dimension and use that value.
                [&](auto const&) -> AvailableSpaceAxis {
                    return input.availableSpace.width.visit(
                        [&](Au definite) -> AvailableSpaceAxis {
                            Au margin = input.usedSpacings.margin.horizontal();
                            Au border = input.usedSpacings.borders.horizontal();
                            Au padding = input.usedSpacings.padding.horizontal();

                            return definite - margin - border - padding;
                        },
                        [](auto const& other) -> AvailableSpaceAxis {
                            return other;
                        }
                    );
                }
            );
        }

        if (auto [height] = input.knownSize.height) {
            availableSize.height = height;
        } else {
            availableSize.height = style.sizing->height.visit(
                // if that dimension of the flex container’s content box is a definite size, use that;
                [&](Calc<PercentOr<Length>> const& calc) -> AvailableSpaceAxis {
                    // FIXME: Correctly handle indefinite percents.
                    return resolve(tree, box, calc, input.containingBlock.height.unwrapOr(0_au));
                },

                // if that dimension of the flex container is being sized under a min or max-content constraint,
                // the available space in that dimension is that constraint;
                [&](Keywords::MinContent) -> AvailableSpaceAxis {
                    return MIN_CONTENT;
                },
                [&](Keywords::MaxContent) -> AvailableSpaceAxis {
                    return MAX_CONTENT;
                },

                // otherwise, subtract the flex container’s margin, border, and padding from the space available
                // to the flex container in that dimension and use that value.
                [&](auto const&) -> AvailableSpaceAxis {
                    return input.availableSpace.height.visit(
                        [&](Au definite) -> AvailableSpaceAxis {
                            Au margin = input.usedSpacings.margin.vertical();
                            Au border = input.usedSpacings.borders.vertical();
                            Au padding = input.usedSpacings.padding.vertical();

                            return definite - margin - border - padding;
                        },
                        [](auto const& other) -> AvailableSpaceAxis {
                            return other;
                        }
                    );
                }
            );
        }

        return availableSize;
    }

    Axis main() const {
        return _mainAxis;
    }

    template <typename T>
    T main(Math::Vec2<T> vec) const {
        if (_mainAxis == Axis::HORIZONTAL) {
            return vec.x;
        } else {
            return vec.y;
        }
    }

    template <typename T>
    T const& main(T const& horizontal, T const& vertical) const {
        if (_mainAxis == Axis::HORIZONTAL) {
            return horizontal;
        } else {
            return vertical;
        }
    }

    Axis cross() const {
        return _mainAxis.cross();
    }

    template <typename T>
    T cross(Math::Vec2<T> const& vec) const {
        if (_mainAxis == Axis::HORIZONTAL) {
            return vec.y;
        } else {
            return vec.x;
        }
    }

    template <typename T>
    T const& cross(T const& horizontal, T const& vertical) const {
        return main(horizontal, vertical);
        if (_mainAxis == Axis::HORIZONTAL) {
            return vertical;
        } else {
            return horizontal;
        }
    }

    void _determineFlexBaseSizeAndHypotheticalMainSize(FlexItem& item, Tree& tree, Input const& input) {
        auto const& style = *item.box.style;

        // A. If the item has a definite used flex basis, that’s the flex base size.

        bool resolveAsContent = style.flex->basis.visit(
            [&](Keywords::Auto) {
                // When specified on a flex item, the auto keyword retrieves the value of the main size property as the used flex-basis.
                // If that value is itself auto, then the used value is content.

                // FIXME: This should be extracted to an helper.
                auto borders = computeBorders(tree, item.box);

                // FIXME: Is it the right containingBlock ?
                if (main() == Axis::HORIZONTAL) {
                    Au paddingStart = resolve(tree, item.box, style.padding->start, input.containingBlock.width.unwrapOr(0_au));
                    Au paddingEnd = resolve(tree, item.box, style.padding->end, input.containingBlock.width.unwrapOr(0_au));
                    auto usedWidth = computeSpecifiedBorderBoxWidth(
                        tree, item.box,
                        style.sizing->width,
                        input.containingBlock,
                        borders.horizontal() + paddingStart + paddingEnd
                    );

                    if (auto [mainSize] = usedWidth) {
                        item.baseSize = mainSize;
                        return false;
                    }
                } else {
                    Au paddingTop = resolve(tree, item.box, style.padding->top, input.containingBlock.width.unwrapOr(0_au));
                    Au paddingBottom = resolve(tree, item.box, style.padding->bottom, input.containingBlock.width.unwrapOr(0_au));
                    auto usedHeight = computeSpecifiedBorderBoxHeight(
                        tree, item.box,
                        style.sizing->height,
                        input.containingBlock,
                        borders.vertical() + paddingTop + paddingBottom
                    );

                    if (auto [mainSize] = usedHeight) {
                        item.baseSize = mainSize;
                        return false;
                    }
                }
                return true;
            },
            [&](Keywords::Content) {
                return true;
            },
            [&](Calc<PercentOr<Length>> const& calc) {
                item.baseSize = resolve(tree, item.box, calc, main(input.containingBlock).unwrapOr(0_au));
                return false;
            },
            [](auto const&) {
                logWarn("could not compute flex base size");
                return false;
            }
        );

        // TODO:
        // B. If the flex item has ...
        // - a preferred aspect ratio,
        // - a used flex basis of content, and
        // - a definite cross size,
        // then the flex base size is calculated from its used cross size and the flex item’s aspect ratio.

        // C. If the used flex basis is content or depends on its available space, and the flex container
        //    is being sized under a min-content or max-content constraint (e.g. when performing automatic
        //    table layout [CSS2]), size the item under that constraint. The flex base size is the item’s
        //    resulting main size.
        if (resolveAsContent and main(input.availableSpace).isMinMaxContent()) {
            item.baseSize = measure(
                tree, item.box,
                _mainAxis,
                {NONE, NONE},
                {NONE, NONE},
                input.availableSpace,
                SizingMode::SIZE
            );
        }

        // TODO:
        // D. Otherwise, if the used flex basis is content or depends on its available space, the available main
        //    size is infinite, and the flex item’s inline axis is parallel to the main axis, lay the item out
        //    using the rules for a box in an orthogonal flow [CSS3-WRITING-MODES]. The flex base size is the item’s max-content main size.
        // TODO:
        // E. Otherwise, size the item into the available space using its used flex basis in place of its main size,
        //    treating a value of content as max-content. If a cross size is needed to determine the main size
        //    (e.g. when the flex item’s main size is in its block axis, or when it has a preferred aspect ratio) and
        //    the flex item’s cross size is auto and not definite, in this calculation use fit-content as the flex item’s cross size.
        //    The flex base size is the item’s resulting main size.
        else if (resolveAsContent) {
            logWarn("could not compute flex base size");
        }

        // When determining the flex base size, the item’s min and max main sizes are ignored (no clamping occurs).
        // Furthermore, the sizing calculations that floor the content box size at zero when applying box-sizing are also ignored.
        // (For example, an item with a specified size of zero, positive padding, and box-sizing: border-box will have an outer
        // flex base size of zero—and hence a negative inner flex base size.)

        // The hypothetical main size is the item’s flex base size clamped according to its used min and max main sizes
        // (and flooring the content box size at zero).
        auto minMainSize = main(style.sizing->minWidth, style.sizing->minHeight);
        auto maxMainSize = main(style.sizing->maxWidth, style.sizing->maxHeight);

        // FIXME: Avoid computing if not necessary.
        auto minContentInline = minContentInlineContribution(tree, item.box);
        auto minContentBlock = minContentBlockContribution(tree, item.box, minContentInline);
        auto maxContentInline = maxContentInlineContribution(tree, item.box);
        auto maxContentBlock = maxContentBlockContribution(tree, item.box, maxContentInline);

        // FIXME: Extract the generic case to an helper.
        Au usedMinMainSize = minMainSize.visit(
            [](Keywords::Auto) -> Au {
                return 0_au;
            },
            [&](Calc<PercentOr<Length>> const& calc) -> Au {
                // FIXME: Correctly handle indefinite percents.
                return resolve(tree, item.box, calc, main(input.containingBlock).unwrapOr(0_au));
            },

            // if that dimension of the flex container is being sized under a min or max-content constraint,
            // the available space in that dimension is that constraint;
            [&](Keywords::MinContent) -> Au {
                return main(minContentInline, minContentBlock);
            },
            [&](Keywords::MaxContent) -> Au {
                return main(maxContentInline, maxContentBlock);
            },
            [&](Keywords::Stretch) -> Au {
                return stretchFit(tree, item.box, input.containingBlock, _mainAxis).unwrapOr(0_au);
            },
            [&](Keywords::FitContent) {
                Au stretch = stretchFit(tree, item.box, input.containingBlock, _mainAxis).unwrapOr(0_au);
                Au min = main(minContentInline, minContentBlock);
                Au max = main(maxContentInline, maxContentBlock);
                return clamp(stretch, min, max);
            }
        );

        // FIXME: Extract the generic case to an helper.
        Au usedMaxMainSize = maxMainSize.visit(
            [](Keywords::None) -> Au {
                return INFINITE;
            },
            [&](Calc<PercentOr<Length>> const& calc) -> Au {
                // FIXME: Correctly handle indefinite percents.
                return resolve(tree, item.box, calc, main(input.containingBlock).unwrapOr(0_au));
            },

            // if that dimension of the flex container is being sized under a min or max-content constraint,
            // the available space in that dimension is that constraint;
            [&](Keywords::MinContent) -> Au {
                return main(minContentInline, minContentBlock);
            },
            [&](Keywords::MaxContent) -> Au {
                return main(maxContentInline, maxContentBlock);
            },
            [&](Keywords::Stretch) -> Au {
                return stretchFit(tree, item.box, input.containingBlock, _mainAxis).unwrapOr(INFINITE);
            },
            [&](Keywords::FitContent) {
                Au stretch = stretchFit(tree, item.box, input.containingBlock, _mainAxis).unwrapOr(INFINITE);
                Au min = main(minContentInline, minContentBlock);
                Au max = main(maxContentInline, maxContentBlock);
                return clamp(stretch, min, max);
            }
        );

        item.hypotheticalMainSize = clamp(item.baseSize, usedMinMainSize, usedMaxMainSize);

        logDebug("[flex-item]: base-size={} hypothetical-main-size={}", item.baseSize, item.hypotheticalMainSize);
    }

    Output run(Tree& tree, Box& box, Input input, usize, Opt<usize>) override {
        // 1. Generate anonymous flex items.
        _generateAnonymousFlexItems(box);

        // 2. Determine the available main and cross space for the flex items.
        auto availableSpace = _determineAvailableSpaceForFlexItems(tree, box, input);

        // 3. Determine the flex base size and hypothetical main size of each item.
        for (auto& item : _items) {
            _determineFlexBaseSizeAndHypotheticalMainSize(item, tree, input);
        }

        // 4.

        return Output{};
    }
};

} // namespace Vaev::Layout::Unstable

namespace Vaev::Layout {

export Rc<FormatingContext> constructFlex2FormatingContext(Box&) {
    return makeRc<Unstable::Flex2FormatingContent>();
}

} // namespace Vaev::Layout
