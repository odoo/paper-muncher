module;

#include <karm-text/prose.h>

export module Vaev.Engine:layout.inline_;

import :values;
import :layout.base;
import :layout.layout;

namespace Vaev::Layout {

struct InlineFormatingContext : FormatingContext {

    // https://www.w3.org/TR/css-inline-3/#baseline-source
    BaselinePositionsSet getUsedBaselineFromBox(Box const& childBox, Output const& output) {
        if (childBox.style->baseline->source == Keywords::FIRST)
            return output.firstBaselineSet;
        if (childBox.style->baseline->source == Keywords::LAST)
            return output.lastBaselineSet;

        if (childBox.style->display == Display::INLINE and childBox.style->display == Display::FLOW_ROOT)
            return output.lastBaselineSet;
        return output.firstBaselineSet;
    }

    BaselinePositionsSet _computeBaselinePositions(InlineBox& inlineBox, Au baselinePosition) {
        auto metrics = inlineBox.prose->_textRuns[0].font.metrics();

        return BaselinePositionsSet{
            .alphabetic = Au{metrics.alphabeticBaseline()} + baselinePosition,
            .xHeight = Au{metrics.xHeight} + baselinePosition,
            .xMiddle = Au{metrics.xMiddleBaseline()} + baselinePosition,
            .capHeight = Au{metrics.captop} + baselinePosition,
        };
    }

    Au resolveBaselinePosition(Style::AlignmentBaseline alignmentBaseline, Text::FontMetrics metrics) {
        if (alignmentBaseline == Keywords::MIDDLE)
            return Au{metrics.xMiddleBaseline()};
        else if (alignmentBaseline == Keywords::TEXT_TOP)
            return Au{metrics.captop};
        else
            return Au{metrics.alphabeticBaseline()};
    }

    // https://www.w3.org/TR/css-inline-3/#line-layout
    // https://www.w3.org/TR/css-inline-3/#inline-height
    Au _layoutVerticallyWithinLineBox(Vec<TextRunFrag>& textRunFrags) {
        Au highestAscent = 0_au;
        Au lowestDescent = 0_au;
        for (auto& textRunFrag : textRunFrags) {
            // position text run frags based on their baseline
            auto& textRunFont = textRunFrag.run->font;

            auto fontAscent = textRunFont.metrics().ascend;
            auto fontDescend = textRunFont.metrics().descend;
            auto baselinePosition = resolveBaselinePosition(
                textRunFrag.run->style->baseline->alignment,
                textRunFont.metrics()
            );

            textRunFrag.baselinePosition.y = -baselinePosition;
            auto textRunAscent = Au{fontAscent} - baselinePosition;
            auto textRunDescend = Au{fontDescend} + baselinePosition;

            highestAscent = max(highestAscent, textRunAscent);
            lowestDescent = max(lowestDescent, textRunDescend);
        }

        return highestAscent + lowestDescent;
    }

    void _layoutHorizontallyWithinLineBox(Vec<TextRunFrag>& textRunFrags, Au inlineSize, TextAlign textAlign) {
        Au totalWidth = 0_au;
        for (auto const& textRunFrag : textRunFrags)
            totalWidth += textRunFrag.width;

        Au free = inlineSize - totalWidth;
        if (free <= 0_au or textAlign == TextAlign::LEFT) {
            Au currXpos = 0_au;
            for (auto& textRunFrag : textRunFrags) {
                textRunFrag.baselinePosition.x = currXpos;
                currXpos += textRunFrag.width;
            }
        } else if (textAlign == TextAlign::RIGHT) {
            Au currXpos = inlineSize;
            for (usize i = textRunFrags.len() - 1; i != usize(-1); --i) {
                auto& textRunFrag = textRunFrags[i];
                currXpos -= textRunFrag.width;
                textRunFrag.baselinePosition.x = currXpos;
            }
        } else if (textAlign == TextAlign::CENTER) {
            Au currXpos = free / 2_au;
            for (auto& textRunFrag : textRunFrags) {
                textRunFrag.baselinePosition.x = currXpos;
                currXpos += textRunFrag.width;
            }
        }
    }

    Au _layoutWithinLineBox(Vec<TextRunFrag>& textRunFrags, Au inlineSize, TextAlign textAlign, Vec2Au position, Au ascend) {
        auto lineHeight = _layoutVerticallyWithinLineBox(textRunFrags);
        _layoutHorizontallyWithinLineBox(textRunFrags, inlineSize, textAlign);

        // ascend is based on the dominant baseline
        for (auto& textRunFrag : textRunFrags) {
            textRunFrag.baselinePosition = textRunFrag.baselinePosition + position + ascend;
        }

        return lineHeight;
    }

    virtual Output run([[maybe_unused]] Tree& tree, Box& box, Input input, [[maybe_unused]] usize startAt, [[maybe_unused]] Opt<usize> stopAt) override {
        // NOTE: We are not supposed to get there if the content is not a prose
        auto& inlineBox = box.content.unwrap<InlineBox>("inlineLayout");

        auto inlineSize = input.knownSize.x.unwrapOrElse([&] {
            if (input.intrinsic == IntrinsicSize::MIN_CONTENT) {
                return 0_au;
            } else if (input.intrinsic == IntrinsicSize::MAX_CONTENT) {
                return Limits<Au>::MAX;
            } else {
                return input.availableSpace.x;
            }
        });

        auto& prose = inlineBox.prose;
        prose->finalizeBuild();

        auto lines = prose->layout(inlineSize);

        ProseFrag proseFrag;
        Vec2Au currPosition{input.position.x, input.position.y};
        for (auto const& line : lines) {
            // FIXME
            Vec<TextRunFrag> textRunsFrags;
            for (auto&& textRunSlice : line) {
                textRunsFrags.pushBack(TextRunFrag{std::move(textRunSlice)});
            }

            auto lineHeight = _layoutWithinLineBox(
                textRunsFrags,
                inlineSize,
                inlineBox.prose->style->text->align,
                currPosition,
                Au{first(lines)[0].run->font.metrics().ascend} // FIXME
            );

            currPosition.y += lineHeight;
            proseFrag.size.y += lineHeight;

            proseFrag.lines.pushBack(textRunsFrags);
        }

        // FIXME: a line that was only 1 word does not have this size
        proseFrag.size.x = inlineSize;

        input.fragment->content = proseFrag;

        if (tree.fc.allowBreak() and not tree.fc.acceptsFit(
                                         input.position.y,
                                         proseFrag.size.y,
                                         input.pendingVerticalSizes
                                     )) {
            return {
                .size = {},
                .completelyLaidOut = false,
                .breakpoint = Breakpoint::overflow()
            };
        }

        return {
            .size = proseFrag.size,
            .completelyLaidOut = true,
            // .firstBaselineSet = firstBaselineSet,
            // .lastBaselineSet = lastBaselineSet,
        };
    }
};

export Rc<FormatingContext> constructInlineFormatingContext(Box&) {
    return makeRc<InlineFormatingContext>();
}

} // namespace Vaev::Layout
