#include "table.h"

#include "frag.h"
#include "values.h"

namespace Vaev::Layout {

bool isHeadBodyFootOrRow(Display display) {
    return (
        display == Vaev::Display::Internal::TABLE_HEADER_GROUP or
        display == Vaev::Display::Internal::TABLE_ROW_GROUP or
        display == Vaev::Display::Internal::TABLE_FOOTER_GROUP or
        display == Vaev::Display::Internal::TABLE_ROW
    );
}

bool isHeadBodyFootRowOrColGroup(Display display) {
    return (
        isHeadBodyFootOrRow(display) or
        display == Vaev::Display::Internal::TABLE_COLUMN_GROUP
    );
}

void advanceUntil(MutCursor<Frag> &cursor, Func<bool(Display)> pred) {
    while (not cursor.ended() and not pred(cursor->style->display)) {
        cursor.next();
    }
}

struct Cell {
    Math::Vec2u anchorIdx;
    MutCursor<Frag> el = nullptr;

    bool operator==(Cell const &c) const {
        return el == c.el and anchorIdx == c.anchorIdx;
    }

    bool isOccupied() const {
        return el != nullptr;
    }
};

Cell const EMPTY_CELL{};

struct Axis {
    usize start, end;
    Frag &el;
};

struct Group {
    usize start, end;
    Frag &el;
};

struct Table {
    // General info
    struct {
        Vec<Vec<Cell>> rows;
        Math::Vec2u size = {0, 0};

        void increaseWidth(usize span = 1) {

            for (auto &row : rows) {
                for (usize i = 0; i < span; ++i)
                    row.pushBack(EMPTY_CELL);
            }
            size.x += span;
        }

        void increaseHeight(usize span = 1) {

            Vec<Cell> newRow{Buf<Cell>::init(size.x, EMPTY_CELL)};
            for (usize i = 0; i < span; ++i) {
                rows.pushBack(newRow);
            }
            size.y += span;
        }

        Cell &get(usize x, usize y) {
            // FIXME: maybe remove in prod? guaranteed that algo is correct
            if (x >= size.x or y >= size.y)
                panic("bad coordinates for table slot");
            return rows[y][x];
        }

        void set(usize x, usize y, Cell cell) {
            // FIXME: maybe remove in prod? guaranteed that algo is correct
            if (x >= size.x or y >= size.y)
                panic("bad coordinates for table slot");

            rows[y][x] = cell;
        }
    } slots;

    Vec<Axis> cols, rows;
    Vec<Group> rowGroups, colGroups;

    Vec<usize> captionsIdxs;
    Frag &wrapperBox, &box;

    // Table forming algorithm
    Math::Vec2u current;

    struct DownwardsGrowingCell {
        Math::Vec2u cellIdx;
        usize xpos, width;
    };

    Vec<DownwardsGrowingCell> downwardsGrowingCells;
    Vec<usize> pendingTfoots;

    struct {
        usize width;
        Vec<InsetsPx> borders;

        InsetsPx &get(usize i, usize j) {
            return borders[i * width + j];
        }
    } bordersGrid;

    InsetsPx boxBorder;
    Vec2Px spacing;

    // https://html.spec.whatwg.org/multipage/tables.html#algorithm-for-growing-downward-growing-cells
    void growDownwardGrowingCells() {
        for (auto &[cellIdx, cellx, width] : downwardsGrowingCells) {
            for (usize x = cellx; x < cellx + width; x++)
                slots.set(x, current.y, slots.get(cellIdx.x, cellIdx.y));
        }
    }

    // https://html.spec.whatwg.org/multipage/tables.html#algorithm-for-processing-rows
    void processRows(Frag &tableRowElement) {
        auto tableRowChildren = tableRowElement.children();
        MutCursor<Frag> tableRowCursor{tableRowChildren};

        if (slots.size.y == current.y)
            slots.increaseHeight();

        current.x = 0;

        growDownwardGrowingCells();

        advanceUntil(tableRowCursor, [](Display d) {
            return d == Display::TABLE_CELL;
        });

        while (not tableRowCursor.ended()) {
            while (current.x < slots.size.x and slots.get(current.x, current.y).isOccupied())
                current.x++;

            if (current.x == slots.size.x)
                slots.increaseWidth();

            usize rowSpan = tableRowCursor->tableSpan->row, colSpan = tableRowCursor->tableSpan->col;

            bool cellGrowsDownward;
            if (rowSpan == 0 and true /* TODO: and the table element's node document is not set to quirks mode, */) {
                cellGrowsDownward = true;
                rowSpan = 1;
            } else
                cellGrowsDownward = false;

            if (slots.size.x < current.x + colSpan) {
                slots.increaseWidth(current.x + colSpan - slots.size.x);
            }

            if (slots.size.y < current.y + rowSpan) {
                slots.increaseHeight(current.y + rowSpan - slots.size.y);
            }

            {

                Cell cell = {.anchorIdx = current, .el = tableRowCursor};

                for (usize x = current.x; x < current.x + colSpan; ++x) {
                    for (usize y = current.y; y < current.y + rowSpan; ++y) {
                        slots.set(x, y, cell);
                    }
                }

                if (cellGrowsDownward) {
                    downwardsGrowingCells.pushBack({.cellIdx = current, .xpos = current.x, .width = colSpan});
                }
            }

            current.x += colSpan;

            tableRowCursor.next();
            advanceUntil(tableRowCursor, [](Display d) {
                return d == Display::TABLE_CELL;
            });
        }
        current.y++;
    }

    // https://html.spec.whatwg.org/multipage/tables.html#algorithm-for-ending-a-row-group
    void endRowGroup() {
        while (current.y < slots.size.y) {
            growDownwardGrowingCells();
            current.y++;
            downwardsGrowingCells.clear();
        }
    }

    // https://html.spec.whatwg.org/multipage/tables.html#algorithm-for-processing-row-groups
    void processRowGroup(Frag &rowGroupElement) {
        auto rowGroupChildren = rowGroupElement.children();
        MutCursor<Frag> rowGroupCursor{rowGroupChildren};

        usize ystartRowGroup = slots.size.y;

        advanceUntil(rowGroupCursor, [](Display d) {
            return d == Display::TABLE_ROW;
        });
        while (not rowGroupCursor.ended()) {
            usize ystartRow = slots.size.y;
            processRows(*rowGroupCursor);
            rows.pushBack({.start = ystartRow, .end = slots.size.y - 1, .el = *rowGroupCursor});

            rowGroupCursor.next();
            advanceUntil(rowGroupCursor, [](Display d) {
                return d == Display::TABLE_ROW;
            });
        }

        if (slots.size.y > ystartRowGroup) {
            rowGroups.pushBack({.start = ystartRowGroup, .end = slots.size.y - 1, .el = rowGroupElement});
        }

        endRowGroup();
    }

    // https://html.spec.whatwg.org/multipage/tables.html#forming-a-table
    void run() {
        auto tableBoxChildren = box.children();
        MutCursor<Frag> tableBoxCursor{tableBoxChildren};

        if (tableBoxCursor.ended())
            return;

        advanceUntil(tableBoxCursor, [](Display d) {
            return isHeadBodyFootRowOrColGroup(d);
        });

        if (tableBoxCursor.ended())
            return;

        // MARK: Columns groups
        while (not tableBoxCursor.ended() and tableBoxCursor->style->display == Display::TABLE_COLUMN_GROUP) {
            auto columnGroupChildren = tableBoxCursor->children();
            MutCursor<Frag> columnGroupCursor = {columnGroupChildren};

            advanceUntil(columnGroupCursor, [](Display d) {
                return d == Vaev::Display::TABLE_COLUMN;
            });

            if (not columnGroupCursor.ended()) {
                usize startColRange = slots.size.x;

                // MARK: Columns
                while (not columnGroupCursor.ended()) {
                    auto span = columnGroupCursor->tableSpan->col;
                    slots.increaseWidth(span);

                    cols.pushBack({.start = slots.size.x - span, .end = slots.size.x - 1, .el = *columnGroupCursor});

                    columnGroupCursor.next();
                    advanceUntil(columnGroupCursor, [](Display d) {
                        return d == Vaev::Display::TABLE_COLUMN;
                    });
                }

                colGroups.pushBack({.start = startColRange, .end = slots.size.x - 1, .el = *tableBoxCursor});
            } else {
                auto span = tableBoxCursor->tableSpan->col;
                slots.increaseWidth(span);

                colGroups.pushBack({.start = slots.size.x - span + 1, .end = slots.size.x - 1, .el = *tableBoxCursor});
            }

            tableBoxCursor.next();
            advanceUntil(tableBoxCursor, [](Display d) {
                return isHeadBodyFootRowOrColGroup(d);
            });
        }

        current.y = 0;

        // MARK: rows
        while (true) {
            advanceUntil(tableBoxCursor, [](Display d) {
                return isHeadBodyFootOrRow(d);
            });

            if (tableBoxCursor.ended())
                break;

            if (tableBoxCursor->style->display == Display::TABLE_ROW) {
                processRows(*tableBoxCursor);
                tableBoxCursor.next();
                continue;
            }

            endRowGroup();

            if (tableBoxCursor->style->display == Display::TABLE_FOOTER_GROUP) {
                pendingTfoots.pushBack(tableBoxCursor - tableBoxChildren.begin());
                tableBoxCursor.next();
                continue;
            }

            // The current element is either a thead or a tbody.
            if (tableBoxCursor->style->display != Display::TABLE_HEADER_GROUP and
                tableBoxCursor->style->display != Display::TABLE_ROW_GROUP) {
                // FIXME: prod code should not fail, but ok for current dev scenario
                panic("current element should be thead or tbody");
            }

            processRowGroup(*tableBoxCursor);

            tableBoxCursor.next();
        }

        for (auto tfootIdx : pendingTfoots) {
            processRowGroup(tableBoxChildren[tfootIdx]);
        }
    }

    Frag &getTableBox(Frag &tableWrapperBox) {
        for (auto &child : tableWrapperBox.children()) {
            if (child.style->display != Display::Internal::TABLE_CAPTION) {
                return child;
            }
        }
        panic("table box not found in frag tree");
    }

    void buildBordersGrid(Tree &fragTree) {
        bordersGrid.borders = Vec<InsetsPx>{
            Buf<InsetsPx>::init(slots.size.x * slots.size.y, InsetsPx{})
        };
        bordersGrid.width = slots.size.x;

        for (usize i = 0; i < slots.size.y; ++i) {
            for (usize j = 0; j < slots.size.x; ++j) {
                auto &cell = slots.get(j, i);
                if (cell.anchorIdx != Math::Vec2u{j, i})
                    continue;

                usize rowSpan = cell.el->tableSpan->row, colSpan = cell.el->tableSpan->col;

                auto cellBorder = computeBorders(fragTree, *cell.el);

                // top and bottom borders
                for (usize k = 0; k < rowSpan; ++k) {
                    bordersGrid.get(i, j + k).top = cellBorder.top;
                    bordersGrid.get(i + rowSpan - 1, j + k).bottom = cellBorder.bottom;
                }

                // left and right borders
                for (usize k = 0; k < rowSpan; ++k) {
                    bordersGrid.get(i + k, j).start = cellBorder.start;
                    bordersGrid.get(i + k, j + colSpan - 1).end = cellBorder.end;
                }
            }
        }
    }

    Table(Tree &fragTree, Frag &tableWrapperBox) : wrapperBox(tableWrapperBox),
                                                   box(getTableBox(tableWrapperBox)),
                                                   boxBorder(computeBorders(fragTree, box)),
                                                   spacing(
                                                       {resolve(fragTree, box, box.style->table->spacing.horizontal),
                                                        resolve(fragTree, box, box.style->table->spacing.vertical)
                                                       }
                                                   ) {
        for (usize i = 0; i < tableWrapperBox.children().len(); ++i) {
            auto &child = tableWrapperBox.children()[i];
            if (child.style->display == Display::Internal::TABLE_CAPTION) {
                captionsIdxs.pushBack(i);
            }
        }

        run();
        buildBordersGrid(fragTree);
    }

    // https://www.w3.org/TR/CSS22/tables.html#borders
    Tuple<Vec<Px>, Px> getColumnBorders() {
        // Borders are only defined for cells and Table Box
        // Rows, columns, row groups, and column groups cannot have borders (i.e., user agents must ignore the border
        // properties for those elements).

        Vec<Px> borders;
        Px sumBorders{0};

        for (usize j = 0; j < slots.size.x; ++j) {
            Px columnBorder{0};

            for (usize i = 0; i < slots.size.y; ++i) {
                auto cell = slots.get(j, i);

                columnBorder = max(
                    columnBorder,
                    bordersGrid.get(i, j).horizontal()
                );
            }

            borders.pushBack(columnBorder);
            sumBorders += columnBorder;
        }
        return {borders, sumBorders};
    }

    // MARK: Fixed Table Layout
    // https://www.w3.org/TR/CSS22/tables.html#fixed-table-layout
    Tuple<Vec<Px>, Px> getFixedColWidths(Tree &t, Input &input) {
        // https://www.w3.org/TR/CSS22/tables.html#model
        // Percentages on 'width' and 'height' on the table (box) are relative to the table wrapper box's containing
        // block, not the table wrapper box itself.
        // https://www.w3.org/TR/CSS22/tables.html#width-layout
        // the table will not automatically size to fill its containing block

        auto tableUsedWidth = box.style->sizing->width == Size::Type::AUTO
                                  ? Px{0}
                                  : resolve(t, box, box.style->sizing->width.value, input.availableSpace.x);

        auto [columnBorders, sumBorders] = getColumnBorders();

        Px fixedWidthToAccount = boxBorder.horizontal() + Px{slots.size.x + 1} * spacing.x;

        Vec<Opt<Px>> colWidth{Buf<Opt<Px>>::init(slots.size.x, NONE)};
        for (auto &col : cols) {

            auto width = col.el.style->sizing->width;
            if (width == Size::Type::AUTO)
                continue;

            for (usize x = col.start; x <= col.end; ++x) {
                colWidth[x] = resolve(t, col.el, width.value, tableUsedWidth);
            }
        }

        // Using first row cells to define columns widths

        usize x = 0;
        while (x < slots.size.x) {
            auto cell = slots.get(x, 0);
            if (cell.el->style->sizing->width == Size::Type::AUTO) {
                x++;
                continue;
            }

            if (cell.anchorIdx != Math::Vec2u{x, 0})
                continue;

            auto cellWidth = resolve(t, *cell.el, cell.el->style->sizing->width.value, tableUsedWidth);
            auto colSpan = cell.el->tableSpan->col;

            for (usize j = 0; j < colSpan; ++j, x++) {
                // FIXME: not overriding values already computed, but should we subtract the already computed from
                // cellWidth before division?
                if (colWidth[x] == NONE)
                    colWidth[x] = cellWidth / Px{colSpan};
            }
        }

        Px sumColsWidths{0};
        usize emptyCols{0};
        for (usize i = 0; i < slots.size.x; ++i) {
            if (colWidth[i])
                sumColsWidths += colWidth[i].unwrap() + columnBorders[i];
            else
                emptyCols++;
        }

        if (emptyCols > 0) {
            if (sumColsWidths < tableUsedWidth - fixedWidthToAccount) {
                Px toDistribute = (tableUsedWidth - fixedWidthToAccount - sumColsWidths) / Px{emptyCols};
                for (auto &w : colWidth)
                    if (w == NONE)
                        w = toDistribute;
            }
        } else if (sumColsWidths < tableUsedWidth - fixedWidthToAccount) {
            Px toDistribute = (tableUsedWidth - fixedWidthToAccount - sumColsWidths);
            for (auto &w : colWidth) {
                w = w.unwrap() + (toDistribute * w.unwrap()) / sumColsWidths;
            }
        }

        Vec<Px> finalColWidths{colWidth.len()};
        for (usize i = 0; i < slots.size.x; ++i) {
            auto finalColWidth = colWidth[i].unwrapOr(Px{0});
            finalColWidths.pushBack(finalColWidth);
        }
        return {finalColWidths, tableUsedWidth};
    }

    // MARK: Auto Table Layout
    // https://www.w3.org/TR/CSS22/tables.html#auto-table-layout
    Tuple<Vec<Px>, Px>
    getAutoColWidths(Tree &t, Input &input) {

        /*
            FIXME: this is just a rough approximation of the algorithm. for instance, we should be able to discriminate
            percentage from fixed lengths: the formers are fixed and cant have extra space distributed over them, while
            the latter can. this can only be done if we have a predicate over sizes to discriminate percentages, which
            we still dont have.
            further, if we try to implement
            https://www.w3.org/TR/css-tables-3/#intrinsic-percentage-width-of-a-column-based-on-cells-of-span-up-to-1
            we will need a way to get the percentage value, which also is not implemented.
        */

        Px capmin{0};
        for (auto i : captionsIdxs) {
            auto captionOutput = layout(
                t,
                wrapperBox.children()[i],
                Input{
                    .commit = Commit::NO,
                    .intrinsic = {IntrinsicSize::MIN_CONTENT, IntrinsicSize::AUTO},
                    .knownSize = {NONE, NONE}
                }
            );
            capmin = max(capmin, captionOutput.size.x);
        }

        auto getCellMinMaxWidth = [](Tree &t, Frag &f, Input &input, Cell &cell) {
            // FIXME: what should be the parameter for intrinsic in the vertical axis?
            auto cellMinOutput = layout(
                t,
                *cell.el,
                Input{
                    .commit = Commit::NO,
                    .intrinsic = {IntrinsicSize::MIN_CONTENT, IntrinsicSize::AUTO},
                    .knownSize = {NONE, NONE}
                }
            );

            // FIXME: what should be the parameter for intrinsic in the vertical axis?
            auto cellMaxOutput = layout(
                t,
                *cell.el,
                Input{
                    .commit = Commit::NO,
                    .intrinsic = {IntrinsicSize::MAX_CONTENT, IntrinsicSize::AUTO},
                    .knownSize = {NONE, NONE}
                }
            );

            auto cellMinWidth = cellMinOutput.size.x;
            auto cellMaxWidth = cellMaxOutput.size.x;

            if (cell.el->style->sizing->width != Size::Type::AUTO) {
                auto cellPreferredWidth = resolve(
                    t,
                    f,
                    cell.el->style->sizing->width.value,
                    input.availableSpace.x
                );
                cellMinWidth = max(cellMinWidth, cellPreferredWidth);

                // TODO: it is not 100% from docs if we should also use 'width' (preffered width) for the maximum cell
                // width
                cellMaxWidth = max(cellMaxWidth, cellPreferredWidth);
            }

            return Tuple<Px, Px>{cellMinWidth, cellMaxWidth};
        };

        Vec<Px> minColWidth{Buf<Px>::init(slots.size.x, Px{0})};
        Vec<Px> maxColWidth{Buf<Px>::init(slots.size.x, Px{0})};

        for (usize i = 0; i < slots.size.y; ++i) {
            for (usize j = 0; j < slots.size.x; ++j) {
                auto cell = slots.get(j, i);

                if (cell.anchorIdx != Math::Vec2u{j, i})
                    continue;

                auto colSpan = cell.el->tableSpan->col;
                if (colSpan > 1)
                    continue;

                auto [cellMinWidth, cellMaxWidth] = getCellMinMaxWidth(t, *cell.el, input, cell);

                minColWidth[j] = max(minColWidth[j], cellMinWidth);
                maxColWidth[j] = max(maxColWidth[j], cellMaxWidth);
            }
        }

        Opt<Px> tableComputedWidth;
        if (box.style->sizing->width != Size::Type::AUTO) {
            tableComputedWidth = resolve(t, box, box.style->sizing->width.value, input.availableSpace.x);
        }

        for (auto &[start, end, el] : cols) {
            auto width = el.style->sizing->width;

            // FIXME: docs are not clear on what to do for columns with AUTO width
            if (width == Size::Type::AUTO)
                continue;

            auto widthValue = resolve(t, el, width.value, tableComputedWidth.unwrapOr(Px{0}));

            for (usize x = start; x <= end; ++x) {
                minColWidth[x] = max(minColWidth[x], widthValue);
                maxColWidth[x] = max(maxColWidth[x], widthValue);
            }
        }

        for (usize i = 0; i < slots.size.y; ++i) {
            for (usize j = 0; j < slots.size.x; ++j) {
                auto cell = slots.get(j, i);

                if (not(cell.anchorIdx == Math::Vec2u{j, i}))
                    continue;

                auto colSpan = cell.el->tableSpan->col;
                if (colSpan <= 1)
                    continue;

                auto [cellMinWidth, cellMaxWidth] = getCellMinMaxWidth(t, *cell.el, input, cell);

                Px currSumMinColWidth{0}, currSumMaxColWidth{0};
                for (usize k = 0; k < colSpan; ++k) {
                    currSumMinColWidth += minColWidth[j + k];
                    currSumMaxColWidth += maxColWidth[j + k];
                }

                if (cellMinWidth > currSumMinColWidth) {
                    auto cellMinWidthContribution = (cellMinWidth - currSumMinColWidth) / Px{colSpan};
                    for (usize k = 0; k < colSpan; ++k) {
                        minColWidth[j + k] += cellMinWidthContribution;
                    }
                }

                if (cellMaxWidth > currSumMaxColWidth) {
                    auto cellMaxWidthContribution = (cellMaxWidth - currSumMaxColWidth) / Px{colSpan};
                    for (usize k = 0; k < colSpan; ++k) {
                        maxColWidth[j + k] += cellMaxWidthContribution;
                    }
                }
            }
        }

        for (auto &group : colGroups) {

            auto columnGroupWidth = group.el.style->sizing->width;
            if (columnGroupWidth == Size::Type::AUTO)
                continue;

            Px currSumOfGroupWidth{0};
            for (usize x = group.start; x <= group.end; ++x) {
                currSumOfGroupWidth += minColWidth[x];
            }

            auto columnGroupWidthValue = resolve(t, group.el, columnGroupWidth.value, input.availableSpace.x);
            if (currSumOfGroupWidth >= columnGroupWidthValue)
                continue;

            Px toDistribute = (columnGroupWidthValue - currSumOfGroupWidth) / Px{group.end - group.start + 1};
            for (usize x = group.start; x <= group.end; ++x) {
                minColWidth[x] += toDistribute;
            }
        }

        Px sumMinColWidths{0}, sumMaxColWidths{0};
        for (auto x : minColWidth)
            sumMinColWidths += x;
        for (auto x : maxColWidth)
            sumMaxColWidths += x;

        // TODO: should minColWidth or maxColWidth be forcelly used if input is MIN_CONTENT or MAX_CONTENT respectivelly?
        if (box.style->sizing->width != Size::Type::AUTO) {
            // TODO: how to resolve percentage if case of table width?
            auto tableUsedWidth = max(capmin, max(tableComputedWidth.unwrap(), sumMinColWidths));

            // If the used width is greater than MIN, the extra width should be distributed over the columns.
            // NOTE: a bit obvious, but assuming that specs refers to MIN columns above
            if (sumMinColWidths < tableUsedWidth) {
                auto toDistribute = tableUsedWidth - sumMinColWidths;
                for (auto &w : minColWidth)
                    w += (toDistribute * w) / sumMaxColWidths;
            }
            return {minColWidth, tableUsedWidth};
        } else {
            // TODO: specs doesnt say if we should distribute extra width over columns; also would it be over min or max
            // columns?
            if (min(sumMaxColWidths, capmin) < input.containingBlock.x) {
                return {maxColWidth, max(sumMaxColWidths, capmin)};
            } else {
                return {minColWidth, max(sumMinColWidths, capmin)};
            }
        }
    }

    // https://www.w3.org/TR/CSS22/tables.html#height-layout
    Vec<Px> getRowHeights(Tree &t, Vec<Px> const &colWidth) {
        /*
        References:
            [A] https://www.w3.org/TR/CSS22/tables.html#height-layout,
            [B] https://www.w3.org/TR/css-tables-3/#computing-the-table-height
        [A] CSS 2.2 does not define how the height of table cells and table rows is calculated when their height is
        specified using percentage values.
        [B] if definite, percentages being considered 0px
        */

        Vec<Px> rowHeight{Buf<Px>::init(slots.size.y, Px{0})};

        for (auto &row : rows) {

            auto height = row.el.style->sizing->height;
            if (height == Size::Type::AUTO)
                continue;

            for (usize y = row.start; y <= row.end; ++y) {
                rowHeight[y] = resolve(t, row.el, height.value, Px{0});
            }
        }

        for (usize i = 0; i < slots.size.y; ++i) {
            for (usize j = 0; j < slots.size.x; ++j) {
                auto cell = slots.get(j, i);

                if (not(cell.anchorIdx == Math::Vec2u{j, i}))
                    continue;

                // [A] CSS 2.2 does not specify how cells that span more than one row affect row height calculations except
                // that the sum of the row heights involved must be great enough to encompass the cell spanning the rows.

                auto rowSpan = cell.el->tableSpan->row;
                if (cell.el->style->sizing->height != Size::Type::AUTO) {
                    auto computedHeight = resolve(
                        t,
                        *cell.el,
                        cell.el->style->sizing->height.value,
                        Px{0}
                    );

                    for (usize k = 0; k < rowSpan; k++) {
                        rowHeight[i + k] = max(rowHeight[i + k], Px{computedHeight / Px{rowSpan}});
                    }
                }

                auto cellOutput = layout(
                    t,
                    *cell.el,
                    Input{
                        .commit = Commit::NO,
                        .intrinsic = {IntrinsicSize::AUTO, IntrinsicSize::MIN_CONTENT},
                        .knownSize = {colWidth[j], NONE}
                    }
                );

                for (usize k = 0; k < rowSpan; k++) {
                    rowHeight[i + k] = max(rowHeight[i + k], Px{cellOutput.size.y / Px{rowSpan}});
                }
            }
        }

        for (usize i = 0; i < slots.size.y; ++i) {
            Px rowBorderHeight{0};
            for (usize j = 0; j < slots.size.x; ++j) {
                auto cellVertBorder = bordersGrid.get(i, j).vertical();
                rowBorderHeight = max(rowBorderHeight, cellVertBorder);
            }
            rowHeight[i] += rowBorderHeight;
        }

        return rowHeight;
    }
};

Output tableLayout(Tree &t, Frag &tableWrapperBox, Input input) {
    // TODO: vertical and horizontal alignment
    // TODO: borders collapse

    Table table(t, tableWrapperBox);

    // NOTE: in case "table-layout: fixed" but "width: auto", specs says the UA could run fixed after computing the
    // width with https://www.w3.org/TR/CSS22/visudet.html#blockwidth , but Chrome does not implement this exception and
    // nor are we
    bool shouldRunAutoAlgorithm = table.box.style->table->tableLayout == TableLayout::AUTO or
                                  table.box.style->sizing->width == Size::AUTO;

    auto [colWidth, tableUsedWidth] = shouldRunAutoAlgorithm
                                          ? table.getAutoColWidths(t, input)
                                          : table.getFixedColWidths(t, input);

    auto rowHeight = table.getRowHeights(t, colWidth);

    Px currPositionY{input.position.y}, captionsHeight{0};
    if (table.box.style->table->captionSide == CaptionSide::TOP) {
        for (auto i : table.captionsIdxs) {
            auto cellOutput = layout(
                t,
                tableWrapperBox.children()[i],
                Input{
                    .commit = input.commit,
                    .position = {input.position.x, currPositionY}
                }
            );
            captionsHeight += cellOutput.size.y;
            currPositionY += captionsHeight;
        }
    }

    auto buildPref = [](Vec<Px> const &v) {
        Vec<Px> pref(v);
        for (usize i = 1; i < v.len(); ++i) {
            pref[i] = pref[i - 1] + pref[i];
        }
        return pref;
    };
    auto queryPref = [](Vec<Px> const &pref, usize l, int r) -> Px {
        if (r < int(l))
            return Px{0};
        if (l == 0)
            return pref[r];
        return pref[r] - pref[l - 1];
    };

    auto colWidthPref = buildPref(colWidth), rowHeightPref = buildPref(rowHeight);

    auto tableBoxSize = Vec2Px{
        queryPref(colWidthPref, 0, table.slots.size.x - 1) + table.spacing.x * Px{table.slots.size.x + 1},
        queryPref(rowHeightPref, 0, table.slots.size.y - 1) + table.spacing.y * Px{table.slots.size.y + 1},
    };

    if (input.commit == Commit::YES) {
        Px currPositionX{input.position.x};

        // table box
        layout(
            t,
            table.box,
            Input{
                .commit = Commit::YES,
                .knownSize = {
                    tableBoxSize.x + table.boxBorder.horizontal(),
                    tableBoxSize.y + table.boxBorder.vertical(),
                },
                .position = {currPositionX, currPositionY},
            }
        );

        currPositionX += table.boxBorder.start + table.spacing.x;
        currPositionY += table.boxBorder.top + table.spacing.y;

        // column groups
        for (auto &group : table.colGroups) {
            layout(
                t,
                group.el,
                Input{
                    .commit = Commit::YES,
                    .knownSize = {
                        queryPref(colWidthPref, group.start, group.end),
                        tableBoxSize.y,
                    },
                    .position = {currPositionX + queryPref(colWidthPref, 0, (int)group.start - 1), currPositionY},
                }
            );
        }

        // columns
        for (auto &col : table.cols) {
            layout(
                t,
                col.el,
                Input{
                    .commit = Commit::YES,
                    .knownSize = {
                        queryPref(colWidthPref, col.start, col.end),
                        tableBoxSize.y,
                    },
                    .position = {currPositionX, currPositionY + queryPref(colWidthPref, 0, (int)col.start - 1)}
                }
            );
        }

        // row groups
        for (auto &group : table.rowGroups) {
            layout(
                t,
                group.el,
                Input{
                    .commit = Commit::YES,
                    .knownSize = {
                        tableBoxSize.x,
                        queryPref(rowHeightPref, group.start, group.end),
                    },
                    .position = {currPositionX, currPositionY + queryPref(rowHeightPref, 0, (int)group.start - 1)}
                }
            );
        }

        // rows
        for (auto &row : table.rows) {
            layout(
                t,
                row.el,
                Input{
                    .commit = Commit::YES,
                    .knownSize = {
                        tableBoxSize.x,
                        queryPref(rowHeightPref, row.start, row.end),
                    },
                    .position = {currPositionX, currPositionY + queryPref(rowHeightPref, 0, (int)row.start - 1)}
                }
            );
        }

        // cells
        for (usize i = 0; i < table.slots.size.y; currPositionY += rowHeight[i] + table.spacing.y, i++) {
            Px innnerCurrPositionX = Px{currPositionX};
            for (usize j = 0; j < table.slots.size.x; innnerCurrPositionX += colWidth[j] + table.spacing.x, j++) {
                auto cell = table.slots.get(j, i);

                if (cell.anchorIdx != Math::Vec2u{j, i})
                    continue;

                auto colSpan = cell.el->tableSpan->col;
                auto rowSpan = cell.el->tableSpan->row;

                // https://www.w3.org/TR/CSS22/tables.html#height-layout
                // TODO: In CSS 2.2, the height of a cell box is the minimum height required by the content.
                // The table cell's 'height' property can influence the height of the row (see above), but it does not
                // increase the height of the cell box.
                auto cellOutput = layout(
                    t,
                    *cell.el,
                    Input{
                        .commit = Commit::YES,
                        // .intrinsic{IntrinsicSize::AUTO, IntrinsicSize::MIN_CONTENT},
                        .knownSize = {
                            queryPref(colWidthPref, j, j + colSpan - 1) + table.spacing.x * Px{colSpan - 1},
                            queryPref(rowHeightPref, i, i + rowSpan - 1) + table.spacing.y * Px{rowSpan - 1}
                        },
                        .position{innnerCurrPositionX, currPositionY},
                    }
                );
            };
        }
    }

    if (table.box.style->table->captionSide == CaptionSide::BOTTOM) {
        for (auto i : table.captionsIdxs) {
            auto cellOutput = layout(
                t,
                tableWrapperBox.children()[i],
                Input{
                    .commit = input.commit,
                    .knownSize = {tableUsedWidth, NONE},
                    .position = {input.position.x, currPositionY},
                }
            );
            captionsHeight += cellOutput.size.y;
        }
    }

    return Output::fromSize(
        {tableUsedWidth + table.boxBorder.horizontal(),
         tableBoxSize.y + captionsHeight + table.boxBorder.vertical()
        }
    );
}

} // namespace Vaev::Layout
