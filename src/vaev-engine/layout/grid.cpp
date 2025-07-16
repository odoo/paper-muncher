module;

#include <karm-base/rc.h>

export module Vaev.Engine:layout.grid;

import :values.grid;
import :layout.block;

namespace Vaev::Layout {

struct GridItem {
    Integer order;
};

struct GridLine {
    Symbol name;
    Au pos = 0_au;
};

struct GridTrack {
    enum struct Flags {
        /// A track that has no in-flow grid items placed into or spanning across it.
        /// (see https://www.w3.org/TR/css-grid-1/#collapsed-grid-track)
        EMPTY = (1 << 0),

        /// A track that has been generated as part of `repeat(auto-fill)` / `repeat(auto-fit)`
        FILL = (1 << 1)
    };

    using enum Flags;

    Karm::Flags<Flags> flags = {EMPTY};
    Au size = 0_au;
    Au startGutter = 0_au, endGutter = 0_au;
    GridTrackSize sizing = 0_au; //< https://www.w3.org/TR/css-grid-1/#grid-template-rows-track-sizing-function

    // https://www.w3.org/TR/css-grid-1/#collapsed-grid-track
    void collapse() {
        sizing = 0_au;
        startGutter = 0_au;
        endGutter = 0_au;
    }
};

struct GridTracks {
    Vec<GridTrack> tracks;
    Vec<GridLine> lines;

    void clear() {
        tracks.clear();
        lines.clear();
    }
};

struct GridArea {
};

// https://www.w3.org/TR/css-grid-1/
struct GridFormatingContext : FormatingContext {
    Vec<GridItem> _items;
    GridTracks _rows;
    GridTracks _columns;
    Map<Symbol, GridArea> _areas;
    GridAutoFlow _autoFlow;

    // MARK: Define The Grid ---------------------------------------------------

    // https://www.w3.org/TR/css-grid-1/#grid-items
    void _generateGridItems(Box& box) {
        _items.clear();
        // Each in-flow child of a grid container becomes a grid item
        for (auto& child : box.children()) {
            auto& style = *child.style;
            // Is the box in flow?
            if (style.position == Position::ABSOLUTE)
                continue;
            _items.pushBack({
                .order = style.order,
            });
        }
    }

    // https://www.w3.org/TR/css-grid-1/#valdef-repeat-auto-fill
    void _defineAutoFillTracks() {
        notImplemented();
    }

    // https://www.w3.org/TR/css-grid-1/#valdef-repeat-auto-fit
    void _colapseEmptyAutoFitTracks(GridTracks& tracks) {
        for (auto& t : tracks.tracks) {
            if (t.flags.has({GridTrack::EMPTY, GridTrack::FIT}))
                t.collapse();
        }
    }

    // https://www.w3.org/TR/css-grid-1/#explicit-grids
    void _defineExplicitTracks(GridTrackList& tracks, GridTracks& out) {
        notImplemented();

        for (GridTrackList::Track& t : tracks) {
            _defineAutoFillTracks();
        }
    }

    void _defineExplicitGrid(GridProps& props) {
        _rows.clear();
        if (auto it = props.explict.rows.is<GridTrackList>())
            _defineExplicitTracks(*it, _rows);

        _columns.clear();
        if (auto it = props.explict.columns.is<GridTrackList>())
            _defineExplicitTracks(*it, _columns);
    }

    // https://www.w3.org/TR/css-grid-1/#implicit-grid
    void _defineImplicitGrid() {
        notImplemented();
    }

    // https://www.w3.org/TR/css-grid-1/#grid-definition
    void _defineGrid(Box& box) {
        _generateGridItems(box);
        _defineExplicitGrid();
        _defineImplicitGrid();
    }

    // MARK: Grid Items Placement ----------------------------------------------

    void _placePositionedItem();

    // https://www.w3.org/TR/css-grid-1/#grid-item-placement-algorith
    void _placeGridItems() {
        notImplemented();
        // 0. Generate anonymous grid items as described in §6 Grid Items.

        // 1. Position anything that’s not auto-positioned.
        _placePositionedItem();

        // 2. Process the items locked to a given row.
        //     For each grid item with a definite row position (that is, the grid-row-start and grid-row-end properties define a definite grid position), in order-modified document order
        //      - “sparse” packing (default behavior)
        if (_autoFlow.sparse()) {
            // Set the column-start line of its placement to the earliest (smallest positive index) line index that ensures this item’s grid area will not overlap any occupied grid cells and that is past any grid items previously placed in this row by this step.
        }
        //      - “dense” packing (dense specified)
        else {
            // Set the column-start line of its placement to the earliest (smallest positive index) line index that ensures this item’s grid area will not overlap any occupied grid cells.
        }

        // 3. Determine the columns in the implicit grid.
        //    Create columns in the implicit grid:
        //     1. Start with the columns from the explicit grid.
        //     2. Among all the items with a definite column position (explicitly positioned items, items positioned in the previous step, and items not yet positioned but with a definite column) add columns to the beginning and end of the implicit grid as necessary to accommodate those items.
        //     3. If the largest column span among all the items without a definite column position is larger than the width of the implicit grid, add columns to the end of the implicit grid to accommodate that column span.

        // 4. Position the remaining grid items.

        //    The auto-placement cursor defines the current “insertion point” in the grid, specified as a pair of row and column grid lines. Initially the auto-placement cursor is set to the start-most row and column lines in the implicit grid.
        struct {
            usize row, col;
        } cursor;

        //    The grid-auto-flow value in use determines how to position the items:
        if (_autoFlow.sparse()) {
            // For each grid item that hasn’t been positioned by the previous steps, in order-modified document order:
            for (auto& item : _items) {
                // If the item has a definite column position:
                if (false) {
                    // 1. Set the column position of the cursor to the grid item’s column-start line. If this is less than the previous column position of the cursor, increment the row position by 1.

                    // 2. Increment the cursor’s row position until a value is found where the grid item does not overlap any occupied grid cells (creating new rows in the implicit grid as necessary).

                    // 3. Set the item’s row-start line to the cursor’s row position, and set the item’s row-end line according to its span from that position.
                }

                // If the item has an automatic grid position in both axes:
                if (false) {
                    // 1. Increment the column position of the auto-placement cursor until either this item’s grid area does not overlap any occupied grid cells, or the cursor’s column position, plus the item’s column span, overflow the number of columns in the implicit grid, as determined earlier in this algorithm.

                    // 2. If a non-overlapping position was found in the previous step, set the item’s row-start and column-start lines to the cursor’s position. Otherwise, increment the auto-placement cursor’s row position (creating new rows in the implicit grid as necessary), set its column position to the start-most column line in the implicit grid, and return to the previous step.
                }
            }
        }
        // “dense” packing (dense specified)
        else {
            // For each grid item that hasn’t been positioned by the previous steps, in order-modified document order:
            for (auto& item : _items) {
                // If the item has a definite column position:
                if (false) {
                    // 1. Set the row position of the cursor to the start-most row line in the implicit grid. Set the column position of the cursor to the grid item’s column-start line.

                    // 2. Increment the auto-placement cursor’s row position until a value is found where the grid item does not overlap any occupied grid cells (creating new rows in the implicit grid as necessary).

                    // 3. Set the item’s row-start line index to the cursor’s row position. (Implicitly setting the item’s row-end line according to its span, as well.)
                }

                // If the item has an automatic grid position in both axes:
                if (false) {
                    // 1. Set the cursor’s row and column positions to start-most row and column lines in the implicit grid.

                    // 2. Increment the column position of the auto-placement cursor until either this item’s grid area does not overlap any occupied grid cells, or the cursor’s column position, plus the item’s column span, overflow the number of columns in the implicit grid, as determined earlier in this algorithm.

                    // 3. If a non-overlapping position was found in the previous step, set the item’s row-start and column-start lines to the cursor’s position. Otherwise, increment the auto-placement cursor’s row position (creating new rows in the implicit grid as necessary), reset its column position to the start-most column line in the implicit grid, and return to the previous step.
                }
            }
        }
    }

    // MARK: Find Grid Size ----------------------------------------------------

    // https://www.w3.org/TR/css-grid-1/#intrinsic-sizes
    void _findGridContainerSize() {
        notImplemented();
        // The max-content size (min-content size) of a grid container is the sum of the grid container’s track sizes (including gutters) in the appropriate axis, when the grid is sized under a max-content constraint (min-content constraint).
    }

    // MARK: Grid Sizing -------------------------------------------------------

    // https://www.w3.org/TR/css-grid-1/#algo-grid-sizing
    void _gridSizing() {
        notImplemented();

        // 1. First, the track sizing algorithm is used to resolve the sizes of the grid columns.

        // 2. Next, the track sizing algorithm resolves the sizes of the grid rows.

        // 3. Then, if the min-content contribution of any grid item has changed based on the row sizes and alignment calculated in step 2, re-resolve the sizes of the grid columns with the new min-content and max-content contributions (once only).

        // 4. Next, if the min-content contribution of any grid item has changed based on the column sizes and alignment calculated in step 3, re-resolve the sizes of the grid rows with the new min-content and max-content contributions (once only).

        // 5. Finally, align the tracks within the grid container according to the align-content and justify-content properties.
    }

    // https://www.w3.org/TR/css-grid-1/#algo-track-sizing
    void _trackSizing() {
        notImplemented();

        // 1. Initialize Track Sizes

        // 2. Resolve Intrinsic Track Sizes

        // 3. Maximize Tracks

        // 4. Expand Flexible Tracks

        // 5. Expand Stretched auto Tracks
    }

    // MARK: Layout Grid Items -------------------------------------------------

    void _layoutGridItems() {
        notImplemented();
    }

    // MARK: Layout Algorithm --------------------------------------------------

    // https://www.w3.org/TR/css-grid-1/#layout-algorithm
    void _layoutGrid() {
        notImplemented();

        // 1. Run the Grid Item Placement Algorithm to resolve the placement of all grid items in the grid.
        // NOTE: Call moved up

        // 2. Find the size of the grid container, per § 5.2 Sizing Grid Containers.
        _findGridContainerSize();

        // 3. Given the resulting grid container size, run the Grid Sizing Algorithm to size the grid.
        _gridSizing();

        // 4. Lay out the grid items into their respective containing blocks. Each grid area’s width and height are considered definite for this purpose.
        _layoutGridItems();
    }

    // https://www.w3.org/TR/css-grid-1/#fragmentation-alg
    void _fragmentGrid() {
        notImplemented();

        // 1. Layout the grid following the § 11 Grid Layout Algorithm by using the fragmentation container’s inline size and assume unlimited block size. During this step all grid-row auto and fr values must be resolved.
        _layoutGrid();

        // 2. Layout the grid container using the values resolved in the previous step.

        // 3. If a grid area’s size changes due to fragmentation (do not include items that span rows in this decision),
        if (false) {
            // increase the grid row size as necessary for rows that either:
            //  - have a content min track sizing function.
            //  - are in a grid that does not have an explicit height and the grid row is flexible.
        }

        // 4. If the grid height is auto, the height of the grid should be the sum of the final row sizes.
        if (false) {
        }

        // 5. If a grid area overflows the grid container due to margins being collapsed during fragmentation
        if (false) {
            // extend the grid container to contain this grid area (this step is necessary in order to avoid circular layout dependencies due to fragmentation).
        }
    }

    Output run(Tree& tree, Box& box, Input input, usize startAt, Opt<usize> stopAt) override {
        _defineGrid(box);
        _placeGridItems();
        _colapseEmptyAutoFitTracks(_rows);
        _colapseEmptyAutoFitTracks(_columns);

        if (false) {
            _layoutGrid();
        } else {
            _fragmentGrid();
        }
    }
};

export Rc<FormatingContext> constructGridFormatingContext(Box& box) {
    return makeRc<GridFormatingContext>();
}

} // namespace Vaev::Layout
