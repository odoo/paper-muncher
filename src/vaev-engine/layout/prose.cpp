module;

#include <karm-gfx/borders.h>
#include <karm-logger/logger.h>
#include <karm-math/au.h>
#include <karm-text/base.h>
#include <karm-text/font.h>

export module Vaev.Engine:layout.prose;

import :style;
import :values.text;

constexpr bool DEBUG_PROSE = true;

namespace Vaev::Layout {

struct Span {
    Rc<Style::SpecifiedValues> style;
    Rc<Text::Fontface> fontFace;
    urange runeRange;

    void repr(Io::Emit& e) const {
        e("(span {} {})", style->color, runeRange);
    }
};

bool hasInsets(Rc<Style::SpecifiedValues> style) {
    auto const& borders = style->borders;

    if (
        borders->top.style != Gfx::BorderStyle::NONE ||
        borders->end.style != Gfx::BorderStyle::NONE ||
        borders->bottom.style != Gfx::BorderStyle::NONE ||
        borders->start.style != Gfx::BorderStyle::NONE
    )
        return true;

    // Math::Insets<CalcValue<PercentOr<Length>>> const& padding = *style->padding;

    // CalcValue<PercentOr<Length>> const zeroPadding{Length{0_au}};

    // if (
    //     padding.top != zeroPadding ||
    //     padding.end != zeroPadding ||
    //     padding.bottom != zeroPadding ||
    //     padding.start != zeroPadding
    // )
    //     return true;

    return false;
}

bool canMergeInto(Rc<Style::SpecifiedValues> styleA, Rc<Style::SpecifiedValues> styleB) {
    if (styleA->color != styleB->color) {
        return false;
    }
    return *styleA->font == *styleB->font and not hasInsets(styleB);
}

/*
Style from Prose:
text-align


Style from Span:
white-space
color
font-family
word-spacing
*/

struct Prose {
    Rc<Style::SpecifiedValues> style;

    Prose(Rc<Style::SpecifiedValues> style, Rc<Text::Fontface> fontFace)
        : style(style) {
        _spans.pushBack(makeRc<Span>(style, fontFace, urange{0, 0}));
        _spansStack.pushBack(last(_spans));
        _runes.pushBack(0);
    }

    struct TextRun {
        Text::Font font;
        Rc<Style::SpecifiedValues> style;
        Vec<Tuple<urange, Karm::Gfx::Color>> paintedSegment;
        urange runeRange;

        usize bidiLevel = 0;

        Vec<Text::Glyph> glyphs{};

        // this is a cache and should be cleaned after each layout
        Vec<Au> glyphAdvances{};

        static TextRun fromSpanAndRange(Rc<Span> span, urange range) {
            Vec<Tuple<urange, Karm::Gfx::Color>> paintedSegment;
            paintedSegment.pushBack({range, span->style->color});
            return {
                {
                    span->fontFace,
                    16.0, // FIXME: This should be resolved from the style
                },
                span->style,
                std::move(paintedSegment),
                range,
            };
        }

        static TextRun fromSpan(Rc<Span> span) {
            return fromSpanAndRange(span, span->runeRange);
        }

        void buildGlyphs(Slice<Rune> runes) {
            // TODO: use bidi level here to see if mirroing and try to apply ligature too
            glyphs.ensure(runeRange.size);
            for (usize i = runeRange.start; i < runeRange.end(); ++i) {
                if (runes[i] == 0) {
                    glyphs.pushBack(Text::Glyph::TOFU);
                    glyphAdvances.pushBack(0_au);
                } else {
                    glyphs.pushBack(font.glyph(runes[i]));
                    glyphAdvances.pushBack(Au{font.advance(last(glyphs))});
                }
            }
        }

        Opt<Text::Glyph> glyph(usize i) const {
            if (i < runeRange.start or i >= runeRange.end())
                return NONE;
            return glyphs[i - runeRange.start];
        }

        Au adv(usize i) const {
            return glyphAdvances[i - runeRange.start];
        }

        void repr(Io::Emit& e) const {
            e("(text-run {} {})", runeRange, paintedSegment);
        }
    };

    // -------------------------------------------- Building API --------------------------------------

    Vec<Rune> _runes;

    Vec<Rc<Span>> _spans;
    Vec<Rc<Span>> _spansStack;

    template <typename E>
    void append(_Str<E> str) {
        for (auto rune : iterRunes(str))
            append(rune);
    }

    void append(Slice<Rune> runes) {
        for (auto rune : runes)
            append(rune);
    }

    bool empty() {
        return _runes.len() == 1;
    }

    bool _isSegmentBreak(Rune rune) {
        return rune == '\n' or rune == '\r' or rune == '\f' or rune == '\v';
    }

    // https://www.w3.org/TR/css-text-3/#propdef-white-space
    // https://www.w3.org/TR/css-text-3/#line-break-transform
    void _appendWhitespace(Rune r) {
        if (empty() or not isAsciiSpace(last(_runes))) {
            _runes.pushBack(r);
            return;
        }

        auto const& whitespace = last(_spansStack)->style->text->whiteSpace;

        // When white-space is pre, pre-wrap, break-spaces, or pre-line, segment breaks are not collapsible and are
        // instead transformed into a preserved line feed (U+000A).
        if (_isSegmentBreak(r))
            r = '\n';
        else // Every collapsible tab is converted to a collapsible space (U+0020).
            r = ' ';

        // FIXME: simplify this
        if (whitespace == WhiteSpace::NORMAL) {
            // Do nothing.
        } else if (whitespace == WhiteSpace::PRE) {
            _runes.pushBack(r);
        } else if (whitespace == WhiteSpace::NOWRAP) {
            // Do nothing.
        } else if (whitespace == WhiteSpace::PRE_WRAP) {
            _runes.pushBack(r);
        } else if (whitespace == WhiteSpace::BREAK_SPACES) {
            _runes.pushBack(r);
        } else if (whitespace == WhiteSpace::PRE_LINE) {
            if (_isSegmentBreak(r)) {
                if (_isSegmentBreak(last(_runes)))
                    _runes.pushBack(r);
                else
                    last(_runes) = r;
            }
        }
    }

    void append(Rune rune) {
        if (isAsciiSpace(rune)) {
            _appendWhitespace(rune);
            return;
        }

        _runes.pushBack(rune);
    }

    // void append(StrutCell&& strut);

    void pushSpan(Rc<Style::SpecifiedValues> spanStyle, Rc<Text::Fontface> fontFace) {
        _spans.pushBack(makeRc<Span>(spanStyle, fontFace, urange{_runes.len(), _runes.len()}));
        _runes.pushBack(0);
        _spansStack.pushBack(last(_spans));
    }

    void popSpan() {
        _runes.pushBack(0);
        last(_spansStack)->runeRange = urange::fromStartEnd(last(_spansStack)->runeRange.start, _runes.len());
        _spansStack.popBack();
    }

    Vec<TextRun> _textRuns;

    Vec<Tuple<urange, Rc<Span>>> _flattenSpans() {
        Vec<Rc<Span>> spanStack = {_spans[0]};
        Vec<Tuple<urange, Rc<Span>>> flattenedSpans;
        Karm::logDebugIf(DEBUG_PROSE, "Will now flatten spans {}", _spans);

        usize lastEnd = 0;
        for (usize i = 1; i < _spans.len(); ++i) {
            auto span = _spans[i];
            // if the next span starts before the current ends, we finalize the flattneed span for the active spans in the
            // stack
            while (span->runeRange.start > last(spanStack)->runeRange.end()) {
                flattenedSpans.pushBack({
                    urange::fromStartEnd(
                        // this flattneed will start after the previous one ends and end with its own span
                        last(flattenedSpans).v0.end(),
                        last(spanStack)->runeRange.end()
                    ),
                    last(spanStack),
                });
                spanStack.popBack();
            }

            flattenedSpans.pushBack({
                urange::fromStartEnd(
                    last(spanStack)->runeRange.start,
                    span->runeRange.start
                ),
                last(spanStack),
            });
            spanStack.pushBack(span);
            lastEnd = span->runeRange.start;

            Karm::logDebugIf(DEBUG_PROSE, "Found flattened spans: {}", flattenedSpans);
        }

        Karm::logDebugIf(DEBUG_PROSE, "Found flattened spans: {}", flattenedSpans);

        Karm::logDebugIf(DEBUG_PROSE, "span stack {}, {}", spanStack, lastEnd);

        while (spanStack.len()) {
            flattenedSpans.pushBack({
                urange::fromStartEnd(
                    // this flattneed will start after the previous one ends and end with its own span
                    lastEnd,
                    last(spanStack)->runeRange.end()
                ),
                last(spanStack),
            });

            lastEnd = last(spanStack)->runeRange.end();
            spanStack.popBack();
        }

        Karm::logDebugIf(DEBUG_PROSE, "Found flattened spans: {}", flattenedSpans);
        return flattenedSpans;
    }

    Vec<TextRun> _textRunsFromSpans() {
        auto flattenedSpans = _flattenSpans();

        Vec<TextRun> textRunsFromSpans = {
            TextRun::fromSpanAndRange(flattenedSpans[0].v1, flattenedSpans[0].v0)
        };

        for (usize i = 1; i < flattenedSpans.len(); i++) {
            auto& prev = last(textRunsFromSpans);
            auto& current = flattenedSpans[i];
            if (canMergeInto(prev.style, current.v1->style)) {
                // merge the two spans
                prev.paintedSegment.pushBack({current.v1->runeRange, current.v1->style->color});
                prev.runeRange = urange::fromStartEnd(prev.runeRange.start, current.v1->runeRange.end());
            } else {
                // create a new text run
                textRunsFromSpans.pushBack(TextRun::fromSpanAndRange(current.v1, current.v0));
            }
        }

        return textRunsFromSpans;
    }

    void _splitIntoTextRuns(Vec<urange> const& levelRuns, Vec<usize> const& hardWrapOpportunities) {
        auto textRunsFromSpans = _textRunsFromSpans();

        usize hardWrapOpportunitiesIndex = 0;
        usize levelRunIndex = 0;
        usize textRunIndex = 0;
        usize lastPos = 0;
        while (levelRunIndex < levelRuns.len() and textRunIndex < textRunsFromSpans.len()) {
            auto& levelRun = levelRuns[levelRunIndex];
            auto& textRun = textRunsFromSpans[textRunIndex];

            auto end = min(levelRun.end(), textRun.runeRange.end());
            if (hardWrapOpportunitiesIndex < hardWrapOpportunities.len() and (hardWrapOpportunities[hardWrapOpportunitiesIndex] + 1) <= end) {
                end = hardWrapOpportunities[hardWrapOpportunitiesIndex] + 1;
                hardWrapOpportunitiesIndex++;
            }

            _textRuns.pushBack(TextRun{
                textRun.font,
                textRun.style,
                textRun.paintedSegment,
                urange::fromStartEnd(lastPos, end),
                textRun.bidiLevel,
            });

            lastPos = end;
            while (levelRunIndex < levelRuns.len() and levelRuns[levelRunIndex].end() <= lastPos)
                levelRunIndex++;
            while (textRunIndex < textRunsFromSpans.len() and textRunsFromSpans[textRunIndex].runeRange.end() <= lastPos)
                textRunIndex++;
        }

        Karm::logDebugIf(DEBUG_PROSE, "Split prose into text runs {}", _textRuns);
    }

    Vec<urange> _extractLevelRuns() {
        Vec<usize> levels;
        for (usize i = 0; i < _runes.len(); ++i) {
            levels.pushBack(0);
        }

        Vec<urange> levelRuns = {urange{0, 0}};
        for (usize i = 1; i < levels.len(); ++i) {
            if (levels[i] != levels[i - 1]) {
                levelRuns.pushBack({i, i});
            } else {
                last(levelRuns) = urange::fromStartEnd(last(levelRuns).start, i + 1);
            }
        }
        last(levelRuns) = urange::fromStartEnd(last(levelRuns).start, _runes.len());

        Karm::logDebugIf(DEBUG_PROSE, "Found level runs {}", levelRuns);

        return levelRuns;
    }

    void finalizeBuild() {
        _runes.pushBack(0);
        first(_spans)->runeRange = urange{0, _runes.len()};

        Karm::logDebugIf(DEBUG_PROSE, "Building prose with {} runes and {} spans", _runes.len(), _spans.len());

        // Run unicode normalization

        // Apply text transformations
        if (style->text->transform == TextTransform::UPPERCASE) {
            for (auto& rune : _runes) {
                rune = toAsciiUpper(rune);
            }
        } else if (style->text->transform == TextTransform::LOWERCASE) {
            for (auto& rune : _runes) {
                rune = toAsciiLower(rune);
            }
        }

        // Separate into paragraphs

        // Compute unicode bidi levels
        auto levels = _extractLevelRuns();

        // Find hard wrap opportunities
        Vec<usize> hardWrapOpportunities;
        for (usize i = 0; i < _runes.len(); ++i) {
            if (_runes[i] == '\n') {
                hardWrapOpportunities.pushBack(i);
            }
        }

        // Paragraph is split in Text Runs
        _textRuns.clear();
        _splitIntoTextRuns(levels, hardWrapOpportunities);

        // Glyphs are computed for each Text Run
        for (auto& textRun : _textRuns) {
            textRun.buildGlyphs(_runes);
        }
    }

    // -------------------------------------------- Layout -------------------------------------

    struct TextRunSlice {
        TextRun& run;
        urange runeRange;
        Au width;

        Slice<Text::Glyph> glyphs() const {
            if (runeRange.size == 0)
                return {};
            return sub(run.glyphs, 0, runeRange.size);
        }

        void repr(Io::Emit& e) const {
            e("(text-run-frag {} {})", runeRange, width);
        }
    };

    using Line = Vec<TextRunSlice>;

    Vec<Line> _wrapLines(usize, Au availableWidth, Opt<usize>) {
        Vec<Line> lines = {Line{}};

        bool allowSoftWrap =
            style->text->whiteSpace != WhiteSpace::PRE and style->text->whiteSpace != WhiteSpace::NOWRAP;

        // runeStart = 0;
        // maxLines = NONE;

        // TextRun* startingTextRun = &first(_textRuns); // TODO: adapt to runeStart
        // usize startingRuneIndex = runeStart;

        // we are greedy
        Au totalWidthInLastLine = 0_au; // flushed

        // we should keep a buffer that holds a list of text run slices that are not flushed yet
        // and their total width
        Au totalOngoingWidth = 0_au;

        struct Buffer {
            Vec<TextRunSlice> textRunSlices;
            Au totalWidth = 0_au;

            void startNewSlice(TextRun& run, usize runeStart) {
                textRunSlices.pushBack(TextRunSlice{
                    .run = run,
                    .runeRange = urange{runeStart, 0},
                    .width = 0_au,
                });
            }

            void pushBack(Au advance) {
                last(textRunSlices).runeRange.size++;
                last(textRunSlices).width += advance;
                totalWidth += advance;
            }

            void clear() {
                textRunSlices.clear();
                totalWidth = 0_au;
            }

            void flushToLine(Line& line) {
                if (textRunSlices.len() == 0) {
                    return;
                }

                usize startFrom = 0;

                if (line.len() and &last(line).run == &first(textRunSlices).run) {
                    startFrom++;

                    last(line).runeRange = urange::fromStartEnd(
                        last(line).runeRange.start,
                        first(textRunSlices).runeRange.end()
                    );
                    last(line).width += first(textRunSlices).width;
                }

                for (usize i = startFrom; i < textRunSlices.len(); ++i) {
                    line.pushBack(std::move(textRunSlices[i]));
                }

                clear();
            }
        };

        Buffer buffer;

        // INVARIANT: totalOngoingWidth = buffer.totalWidth + totalWidthInLastLine;
        for (auto& textRun : _textRuns) {
            // initiate current text run in the buffer
            buffer.startNewSlice(textRun, textRun.runeRange.start);
            for (usize i = textRun.runeRange.start; i < textRun.runeRange.end(); ++i) {
                // after clear, if there is something to process, we must start a new text run slice in the buffer
                if (buffer.textRunSlices.len() == 0)
                    buffer.startNewSlice(textRun, i);

                auto adv = textRun.adv(i);

                // are we allowed to flush?
                if (_isSegmentBreak(_runes[i]) or (allowSoftWrap and Karm::isAsciiSpace(_runes[i]))) {
                    buffer.pushBack(adv);

                    totalWidthInLastLine = totalOngoingWidth = totalOngoingWidth - buffer.totalWidth;
                    buffer.flushToLine(last(lines));
                }

                // we must change the line we are going to flushing into
                // either because we had a forced line break
                // or because we have an "overflow" and the line is not empty
                if (
                    _isSegmentBreak(_runes[i]) or
                    (allowSoftWrap and totalOngoingWidth + adv > availableWidth and totalOngoingWidth > 0_au)
                ) {
                    lines.pushBack(Line{});
                    totalOngoingWidth = totalOngoingWidth - totalWidthInLastLine;
                }

                totalOngoingWidth += adv;

                // if we just flushed the buffer, we already added the whitespace to the line from it
                if (buffer.textRunSlices.len())
                    buffer.pushBack(adv);
            }
        }
        buffer.flushToLine(last(lines));

        // FIXME: adapt rune ranges to remove white spaces from beginning and end of lines

        Karm::logDebugIf(DEBUG_PROSE, "Found lines: {}", lines);
        return lines;
    }

    void _reorderLinesFromBidi(Vec<Vec<TextRunSlice>>& lines) {
        for (auto const& line : lines) {
            Vec<usize> levels(line.len());
            for (auto const& textRunSlice : line) {
                levels.pushBack(textRunSlice.run.bidiLevel);
            }

            // auto permutationFromLevels = Karm::Text::bidiPermutationFromLevels(levels);
            // permute elements inside line
        }
    }

    Vec<Line> wrapLinesFrom(usize runeStart, Au availableWidth, Opt<usize> maxLines = NONE) {
        if (runeStart >= _runes.len()) {
            return {};
        }

        auto lines = _wrapLines(runeStart, availableWidth, maxLines);
        _reorderLinesFromBidi(lines);

        // reverse glyphs from text runs (they should be reversed independent of the line (confirm this))

        return lines;
    }

    Vec<Line> layout(Au availableWidth, Opt<usize> maxLines = NONE) {
        return wrapLinesFrom(0, availableWidth, maxLines);
    }
};

struct TextRunFrag : Prose::TextRunSlice {
    Vec2Au position; // Position of the baseline of the text run

    TextRunFrag(Prose::TextRunSlice textRunSlice, Vec2Au position = {})
        : Prose::TextRunSlice(textRunSlice), position(position) {}
};

struct ProseFrag {
    Vec<Vec<TextRunFrag>> lines;
    Vec2Au size;
};

} // namespace Vaev::Layout
