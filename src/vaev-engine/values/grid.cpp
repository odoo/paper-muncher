module;

#include <karm-base/symbol.h>

export module Vaev.Engine:values.grid;

import :values.primitives;
import :values.keywords;

namespace Vaev {

struct GridTrackList;

// https://www.w3.org/TR/css-grid-1/#repeat-syntax

// <track-repeat> = repeat( [ <integer [1,∞]> ] , [ <line-names>? <track-size> ]+ <line-names>? )
// <auto-repeat>  = repeat( [ auto-fill | auto-fit ] , [ <line-names>? <fixed-size> ]+ <line-names>? )
// <fixed-repeat> = repeat( [ <integer [1,∞]> ] , [ <line-names>? <fixed-size> ]+ <line-names>? )

using GridRepeatNumber = Union<Integer, Keywords::AutoFill, Keywords::AutoFit>;

struct GridRepeat {
    GridRepeatNumber number;
    Box<GridTrackList> tracks;
};

// https://www.w3.org/TR/css-grid-1/#track-list

// <track-size>          = <track-breadth> | minmax( <inflexible-breadth> , <track-breadth> ) | fit-content( <length-percentage [0,∞ ]> )
// <fixed-size>          = <fixed-breadth> | minmax( <fixed-breadth> , <track-breadth> ) | minmax( <inflexible-breadth> , <fixed-breadth> )
// <track-breadth>       = <length-percentage [0,∞ ]> | <flex [0,∞ ]> | min-content | max-content | auto
// <inflexible-breadth>  = <length-percentage [0,∞ ]> |                 min-content | max-content | auto
// <fixed-breadth>       = <length-percentage [0,∞ ]>
// <line-names>          = '[' <custom-ident>* ']'
struct GridFlex {
};

struct GridMinMax {
};

struct GridTrackSize {
};

// <track-list>          = [ <line-names>? [ <track-size> | <track-repeat> ] ]+ <line-names>?
// <auto-track-list>     = [ <line-names>? [ <fixed-size> | <fixed-repeat> ] ]* <line-names>? <auto-repeat>
//                         [ <line-names>? [ <fixed-size> | <fixed-repeat> ] ]* <line-names>?
// <explicit-track-list> = [ <line-names>? <track-size> ]+ <line-names>?
struct GridTrackList {
    struct Item {
        Opt<Symbol> name;
        Union<GridTrackSize, GridRepeat> size;
    };

    GridRepeat autoRepeat;
};

// https://www.w3.org/TR/css-grid-1/#grid-auto-flow-property
export struct GridAutoFlow {
    enum struct PlaceItem {
        ROW,
        COLUMN,
    };

    PlaceItem place;
    bool dense;

    bool sparse() const {
        return not dense;
    }
};

export struct GridLine {};

export struct GridAreaRow {};

export struct ExplicitGridProps {
    Union<Keywords::None, Vec<GridAreaRow>> areas;
    Union<Keywords::None, GridTrackList> rows, columns;
};

export struct ImplicitGridProps {
    GridAutoFlow flow;
    Union<Keywords::Auto, GridTrackSize> rows, columns;
};

export struct GridProps {
    ExplicitGridProps explict;
    ImplicitGridProps implicit;
    GridLine rowStart, rowEnd;
    GridLine columnStart, columnEnd;
};

} // namespace Vaev