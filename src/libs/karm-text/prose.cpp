#include "prose.h"

namespace Karm::Text {

Prose::Prose(ProseStyle style, Str str) : _style(style) {
    clear();
    _spaceWidth = _style.font.advance(_style.font.glyph(' '));
    auto m = _style.font.metrics();
    _lineHeight = m.ascend;
    append(str);
}

// MARK: Prose --------------------------------------------------------------

void Prose::_beginBlock() {
    _blocks.pushBack({
        .prose = this,
        .runeRange = {_runes.len(), 0},
        .cellRange = {_cells.len(), 0},
    });
}

void Prose::_append(Cell&& cell) {
    if (any(_blocks) and last(_blocks).newline())
        _beginBlock();

    if (any(_blocks) and last(_blocks).spaces())
        _beginBlock();

    _cells.pushBack(std::move(cell));

    last(_blocks).cellRange.size++;
    last(_blocks).runeRange.end(_runes.len());
}

void Prose::append(Rc<StrutCell> strut) {
    // FIXME: _append encasulates _cell but we are using its size to find the index
    _structCellsIndexes.pushBack(_cells.len());
    _runes.pushBack(0);
    _append({
        .prose = this,
        .span = _currentSpan,
        .runeRange = {_runes.len(), 1},
        ._content = strut,
    });
}

void Prose::append(Rune rune) {
    auto glyph = _style.font.glyph(rune == '\n' ? ' ' : rune);

    Cell cell{
        .prose = this,
        .span = _currentSpan,
        .runeRange = {_runes.len(), 1},
        ._content = makeRc<RuneCell>(glyph),
    };

    _runes.pushBack(rune);

    _append(std::move(cell));
}

void Prose::clear() {
    _runes.clear();
    _cells.clear();
    _blocks.clear();
    _blocksMeasured = false;
    _lines.clear();

    _beginBlock();
}

void Prose::copySpanStack(Prose const& prose) {
    Vec<Box<Text::Prose::Span>> spans;
    auto currSpan = prose._currentSpan;
    while (currSpan) {
        spans.pushBack(*currSpan);
        currSpan = currSpan->parent;
    }

    reverse(mutSub(spans));
    _spans = std::move(spans);

    for (usize i = 0; i + 1 < _spans.len(); ++i) {
        _spans[i + 1]->parent = &*_spans[i];
    }

    if (_spans.len())
        _currentSpan = &*last(_spans);
}

void Prose::append(Slice<Rune> runes) {
    _runes.ensure(_runes.len() + runes.len());
    for (auto rune : runes) {
        append(rune);
    }
}

// MARK: Layout -------------------------------------------------------------

void Prose::_wrapLines(Au width) {
    _lines.clear();

    Line line{this, {}, {}};
    bool first = true;
    Au adv = 0_au;
    for (usize i = 0; i < _blocks.len(); i++) {
        auto& block = _blocks[i];
        if (adv + block.width > width and _style.wordwrap and _style.multiline and not first) {
            _lines.pushBack(line);
            line = {this, block.runeRange, {i, 1}};
            adv = block.width;

            if (block.newline()) {
                _lines.pushBack(line);
                line = {
                    this,
                    {block.runeRange.end(), 0},
                    {i + 1, 0},
                };
                adv = 0_au;
            }
        } else {
            line.blockRange.size++;
            line.runeRange.end(block.runeRange.end());

            if (block.newline() and _style.multiline) {
                _lines.pushBack(line);
                line = {
                    this,
                    {block.runeRange.end(), 0},
                    {i + 1, 0},
                };
                adv = 0_au;
            } else {
                adv += block.width;
            }
        }
        first = false;
    }

    _lines.pushBack(line);
}

Au Prose::_layoutVerticaly() {
    auto m = _style.font.metrics();
    Au baseline = Au{Math::ceil(m.linegap / 2)};
    for (auto& line : _lines) {
        baseline += Au{Math::ceil(m.ascend)};
        line.baseline = baseline;
        baseline += Au{Math::ceil(m.linegap + m.descend)};
    }
    return baseline - Au{Math::ceil(m.linegap / 2)};
}

Au Prose::_layoutHorizontaly(Au width) {
    Au maxWidth = 0_au;
    for (auto& line : _lines) {
        if (not line.blockRange.any())
            continue;

        Au pos = 0_au;
        for (auto& block : line.blocks()) {
            block.pos = pos;
            pos += block.width;
        }

        auto lastBlock = _blocks[line.blockRange.end() - 1];
        line.width = lastBlock.pos + lastBlock.width;
        maxWidth = max(maxWidth, line.width);
        auto free = width - line.width;

        switch (_style.align) {
        case TextAlign::LEFT:
            break;

        case TextAlign::CENTER:
            for (auto& block : line.blocks())
                block.pos += free / 2_au;
            break;

        case TextAlign::RIGHT:
            for (auto& block : line.blocks())
                block.pos += free;
            break;
        }
    }

    return maxWidth;
}

} // namespace Karm::Text
