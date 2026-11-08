export module Vaev.Engine:style.computed;

import Karm.Core;
import Karm.Gfx;
import Karm.Math;

import :css;
import :values;
import :css.values;
import :style.values;

using namespace Karm;

namespace Vaev::Style {

// MARK: Computed value types --------------------------------------------------

// https://drafts.csswg.org/css-fonts-4/#font-style-prop
// Computed value: the keyword specified, plus angle in degrees if specified
export struct FontStyle {
    Gfx::FontStyle val = Gfx::FontStyle::NORMAL;
    // The lack of an <angle> represents 14deg.
    Deg obliqueAngle = Deg{14};

    bool operator==(FontStyle const&) const = default;

    void repr(Io::Emit& e) const {
        e("{}", val);
        if (val == Gfx::FontStyle::OBLIQUE)
            e(" {}", obliqueAngle);
    }
};

// https://www.w3.org/TR/css-lists-3/#image-markers
export using ListImage = Union<Keywords::None, Css::Image>;

export using LineHeight = Union<Keywords::Normal, f64, Pixel>;

export using Gap = Union<
    Keywords::Normal,
    PixelPercentage>;

// https://www.w3.org/TR/css-inline-3/#baseline-source
export using BaselineSource = Union<Keywords::Auto, Keywords::First, Keywords::Last>;

// https://www.w3.org/TR/css-inline-3/#alignment-baseline-property
export using AlignmentBaseline = Union<
    Keywords::Baseline,
    Keywords::TextBottom,
    Keywords::Alphabetic,
    Keywords::Ideographic,
    Keywords::Middle,
    Keywords::Central,
    Keywords::Mathematical,
    Keywords::TextTop>;

// https://www.w3.org/TR/css-inline-3/#dominant-baseline-property
export using DominantBaseline = Union<
    Keywords::Auto,
    Keywords::Baseline,
    Keywords::TextBottom,
    Keywords::Alphabetic,
    Keywords::Ideographic,
    Keywords::Middle,
    Keywords::Central,
    Keywords::Mathematical,
    Keywords::TextTop>;

export using BreakBetween = Union<
    Keywords::Auto,
    Keywords::Avoid,
    Keywords::AvoidPage,
    Keywords::Page,
    Keywords::Left,
    Keywords::Right,
    Keywords::Recto,
    Keywords::Verso,
    Keywords::AvoidColumn,
    Keywords::Column,
    Keywords::AvoidRegion,
    Keywords::Region>;

export using BreakInside = Union<
    Keywords::Auto,
    Keywords::Avoid,
    Keywords::AvoidPage,
    Keywords::AvoidColumn,
    Keywords::AvoidRegion>;

export using FlexDirection = Union<
    Keywords::Row,
    Keywords::RowReverse,
    Keywords::Column,
    Keywords::ColumnReverse>;

export using FlexWrap = Union<
    Keywords::Nowrap,
    Keywords::Wrap,
    Keywords::WrapReverse>;

export using Float = Union<
    Keywords::None,
    Keywords::InlineStart,
    Keywords::InlineEnd,
    Keywords::Left,
    Keywords::Right>;

export using Clear = Union<
    Keywords::None,
    Keywords::Left,
    Keywords::Right,
    Keywords::Both,
    Keywords::InlineStart,
    Keywords::InlineEnd>;

// https://www.w3.org/TR/css-lists-3/#list-style-position-property
export using ListPosition = Union<Keywords::Inside, Keywords::Outside>;

// https://www.w3.org/TR/css-lists-3/#marker-side
export using MarkerSide = Union<Keywords::MatchSelf, Keywords::MatchParent>;

// https://www.w3.org/TR/css-lists-3/#text-markers
export using ListType = Union<Keywords::None, CustomIdent, CounterSymbolsFunc, String>;

// https://drafts.csswg.org/css-multicol-2/#cc
// FIXME: Enforce the [1,∞] range.
export using ColumnCount = Union<
    Keywords::Auto,
    Integer>;

// MARK: Overflow
// https://www.w3.org/TR/css-overflow/#overflow-control
export using Overflow = Union<
    Keywords::Visible,
    Keywords::Hidden,
    Keywords::Scroll,
    Keywords::Auto>;

// https://www.w3.org/TR/css-sizing-3/#box-sizing
export using BoxSizing = Union<Keywords::ContentBox, Keywords::BorderBox>;

export using TableLayout = Union<
    Keywords::Auto,
    Keywords::Fixed>;

export using CaptionSide = Union<
    Keywords::Top,
    Keywords::Bottom>;

export using BorderCollapse = Union<
    Keywords::Separate,
    Keywords::Collapse>;

export using TextAlign = Union<
    Keywords::Start,
    Keywords::End,
    Keywords::Left,
    Keywords::Right,
    Keywords::Center,
    Keywords::Justify,
    Keywords::MatchParent,
    Keywords::JustifyAll,
    Keywords::VaevBlockCenter>;

export using TextTransform = Union<
    Keywords::None,
    Keywords::Uppercase,
    Keywords::Lowercase>;

export using WhiteSpace = Union<
    Keywords::Normal,
    Keywords::Pre,
    Keywords::Nowrap,
    Keywords::PreWrap,
    Keywords::BreakSpaces,
    Keywords::PreLine>;

export using TransformBox = Union<
    Keywords::ContentBox,
    Keywords::BorderBox,
    Keywords::FillBox,
    Keywords::StrokeBox,
    Keywords::ViewBox>;

export using Visibility = Union<
    Keywords::Visible,
    Keywords::Hidden,
    Keywords::Collapse>;

export using ZIndex = Union<Keywords::Auto, Integer>;

export struct RunningPosition {
    CustomIdent customIdent;

    void repr(Io::Emit& e) const {
        e("running '{}'", customIdent);
    }
};

// https://www.w3.org/TR/CSS22/visuren.html#propdef-position
export using PositionScheme = Union<Keywords::Static, Keywords::Relative, Keywords::Absolute, Keywords::Fixed, Keywords::Sticky, RunningPosition>;

// https://drafts.csswg.org/css-writing-modes-4/#propdef-writing-mode
export using WritingMode = Union<Keywords::HorizontalTb, Keywords::VerticalRl, Keywords::VerticalLr, Keywords::SidewaysRl, Keywords::SidewaysLr>;

// https://drafts.csswg.org/css-writing-modes-4/#propdef-direction
export using Direction = Union<Keywords::Ltr, Keywords::Rtl>;

// https://svgwg.org/svg2-draft/coords.html#ViewBoxAttribute
export struct SvgViewBox {
    f64 minX;
    f64 minY;
    f64 width;
    f64 height;

    void repr(Io::Emit& e) const {
        e("(ViewBox minX={} minY={} width={} height={})", minX, minY, width, height);
    }
};

// https://www.w3.org/TR/css-lists-3/#counter-reset
export struct CounterReset {
    CustomIdent name;
    bool reversed;
    Opt<Integer> value = Some(0);

    void repr(Io::Emit& e) const {
        e("(counter-reset {}{} {})", reversed ? "reversed " : "", name, value);
    }
};

// https://www.w3.org/TR/css-lists-3/#increment-set
export struct CounterIncrement {
    CustomIdent name;
    Opt<Integer> value;

    void repr(Io::Emit& e) const {
        e("(counter-increment {} {})", name, value);
    }
};

export using ContentValue = Union<
    Keywords::Normal,
    Keywords::None,
    String,
    ElementFunc,
    CounterFunc,
    CountersFunc>;

export using Size = Union<Keywords::Auto, PixelPercentage, Keywords::Stretch, Keywords::MinContent, Keywords::MaxContent, Keywords::FitContent>;

export using MaxSize = Union<Keywords::None, PixelPercentage, Keywords::Stretch, Keywords::MinContent, Keywords::MaxContent, Keywords::FitContent>;

export using BaselineShift = Union<
    PixelPercentage,
    Keywords::Sub,
    Keywords::Super,
    Keywords::Top,
    Keywords::Center,
    Keywords::Bottom>;

export using FlexBasis = Union<
    Keywords::Auto,
    Keywords::Content,
    Keywords::MinContent,
    Keywords::MaxContent,
    Keywords::FitContent,
    PixelPercentage>;

struct Fonts {
    Vec<FontFamily> families = {"sans-serif"_sym};
    Gfx::FontWeight weight = Gfx::FontWeight::REGULAR;
    Percent width = Percent{100};
    FontStyle style;
    Pixel size;
};

struct Transforms {
    Transform transform = Keywords::NONE;
    Position origin = {Percent{0}, Percent{0}};
    TransformBox box = Keywords::BORDER_BOX;

    bool has() const {
        return not transform.is<Keywords::None>();
    }
};

export struct BorderSpacing {
    Pixel horizontal = Pixel{0};
    Pixel vertical = Pixel{0};
};

export struct Table {
    CaptionSide captionSide = Keywords::TOP;
    BorderSpacing spacing;
    BorderCollapse collapse = Keywords::SEPARATE;
};

export struct SvgGeometry {
    PixelPercentage x = Pixel{0};
    PixelPercentage y = Pixel{0};
    PixelPercentage cx = Pixel{0};
    PixelPercentage cy = Pixel{0};
    PixelPercentage r = Pixel{0};

    Union<String, None> d = NONE;
    Opt<SvgViewBox> viewBox = NONE;
};

export struct SvgPainting {
    f64 fillOpacity = 1;
    PixelPercentage strokeWidth = Pixel{1};
    f64 strokeOpacity = 1;
    SvgPaint fill = Some(Gfx::BLACK);
    SvgPaint stroke = NONE;
};

using Clip = Opt<BasicShape>;

export struct Inherited {};

// https://www.w3.org/TR/css-cascade/#computed
// MARK: Groupings -----------------------------------------------------------
// Bundles of computed longhands sharing storage in ComputedValues.

export struct Aligns {
    // https://drafts.csswg.org/css-align-3/#propdef-align-content
    Align alignContent;

    // https://drafts.csswg.org/css-align-3/#propdef-justify-content
    Align justifyContent;

    // https://drafts.csswg.org/css-align-3/#propdef-justify-self
    Align justifySelf;

    // https://drafts.csswg.org/css-align-3/#propdef-align-self
    Align alignSelf;

    // https://drafts.csswg.org/css-align-3/#propdef-justify-items
    Align justifyItems;

    // https://drafts.csswg.org/css-align-3/#propdef-align-items
    Align alignItems;
};

export struct Backgrounds {
    Color color = Gfx::ALPHA;
    Vec<BackgroundLayer> layers = {};

    bool imageIsNone() const {
        return iter(layers) |
               All([](auto const& l) {
                   return l.image == NONE;
               });
    }
};

// FIXME: cant we have this for karm/insets?
export enum struct BorderEdge {
    TOP,
    START,
    BOTTOM,
    END,
};

export struct Border {
    Pixel width = 3_px;
    Gfx::BorderStyle style = Gfx::BorderStyle::NONE;
    Color color = Gfx::BLACK;
};

export struct Borders {
    Border top, start, bottom, end;
    Math::Radii<PixelPercentage> radii = {Pixel{0}};

    Border const& get(BorderEdge edge) const {
        switch (edge) {
        case BorderEdge::TOP:
            return top;
        case BorderEdge::START:
            return start;
        case BorderEdge::BOTTOM:
            return bottom;
        case BorderEdge::END:
            return end;
        }
    }
};

export struct Breaks {
    BreakBetween before = Keywords::AUTO;
    BreakBetween after = Keywords::AUTO;
    BreakInside inside = Keywords::AUTO;

    // Breaks Between Lines: orphans, widows
    // https://www.w3.org/TR/css-break-3/#widows-orphans
    Integer orphans = 2;
    Integer widows = 2;
};

export struct Columns {
    Union<Keywords::Auto, Pixel> width = Keywords::AUTO;
    Union<Keywords::Auto, Pixel> height = Keywords::AUTO;
    ColumnCount count = Keywords::AUTO;
};

export struct Counters {
    Vec<CounterReset> reset;
    Vec<CounterIncrement> increment;
    Vec<CounterIncrement> set;
};

export struct Flex {
    // FlexContainer
    FlexDirection direction = Keywords::ROW;
    FlexWrap wrap = Keywords::NOWRAP;

    // FlexItem
    FlexBasis basis = Keywords::AUTO;
    f64 grow = 0;
    f64 shrink = 1;

    bool isRowOriented() const {
        return direction == Keywords::ROW or
               direction == Keywords::ROW_REVERSE;
    }

    bool isReverse() const {
        return direction == Keywords::ROW_REVERSE or
               direction == Keywords::COLUMN_REVERSE;
    }
};

export struct Gaps {
    Gap row = Keywords::NORMAL;
    Gap col = Keywords::NORMAL;
};

export struct Inline {
    BaselineSource baselineSource = Keywords::AUTO;
    AlignmentBaseline alignmentBaseline = Keywords::BASELINE;
    DominantBaseline dominantBaseline = Keywords::AUTO;
    BaselineShift baselineShift = PixelPercentage{Pixel{}};
    LineHeight lineHeight = Keywords::NORMAL;
};

export struct List {
    ListImage image = Keywords::NONE;
    ListType type = CustomIdent{"disc"_sym};
    ListPosition position = Keywords::OUTSIDE;
    MarkerSide markerSide = Keywords::MATCH_SELF;
};

export using Margins = Math::Insets<Union<Keywords::Auto, PixelPercentage>>;

export struct Outline {
    Pixel width = 3_px;
    Pixel offset = Pixel{0};
    Union<Keywords::Auto, Gfx::BorderStyle> style = Gfx::BorderStyle::NONE;
    Gfx::Color color = Gfx::BLUE500;
};

export using Paddings = Math::Insets<PixelPercentage>;

export using BoxInsets = Math::Insets<Union<Keywords::Auto, PixelPercentage>>;

export struct Sizing {
    Size width = Keywords::AUTO, height = Keywords::AUTO;
    Size minWidth = Keywords::AUTO, minHeight = Keywords::AUTO;
    MaxSize maxWidth = Keywords::NONE, maxHeight = Keywords::NONE;

    Size& size(Axis axis) {
        return axis == Axis::HORIZONTAL ? width : height;
    }

    Size const& size(Axis axis) const {
        return axis == Axis::HORIZONTAL ? width : height;
    }

    Size& minSize(Axis axis) {
        return axis == Axis::HORIZONTAL ? minWidth : minHeight;
    }

    Size const& minSize(Axis axis) const {
        return axis == Axis::HORIZONTAL ? minWidth : minHeight;
    }

    MaxSize& maxSize(Axis axis) {
        return axis == Axis::HORIZONTAL ? maxWidth : maxHeight;
    }

    MaxSize const& maxSize(Axis axis) const {
        return axis == Axis::HORIZONTAL ? maxWidth : maxHeight;
    }
};

export struct Text {
    TextAlign align = Keywords::START;
    TextTransform transform = Keywords::NONE;
    WhiteSpace whiteSpace = Keywords::NORMAL;
};

export struct Overflows {
    Overflow x = Keywords::VISIBLE;
    Overflow y = Keywords::VISIBLE;
    Overflow block = Keywords::VISIBLE;
    Overflow inline_ = Keywords::VISIBLE;

    void repr(Io::Emit& e) const {
        e("(overflows");
        e(" x={}", x);
        e(" y={}", y);
        e(" block={}", block);
        e(" inline={}", inline_);
        e(")");
    }
};

// MARK: Computed Values -------------------------------------------------------

export struct ComputedValues {
    Cow<Gaps> gaps;
    Cow<Backgrounds> backgrounds;
    Cow<Borders> borders;
    Cow<Margins> margin = Margins(Keywords::AUTO);
    Cow<Outline> outline;
    Cow<Paddings> padding = Paddings(Pixel{});
    Cow<Sizing> sizing;
    Cow<Inline> inline_;
    Cow<BoxInsets> insets = BoxInsets(Keywords::AUTO);
    Cow<Clip> clip;
    Cow<Transforms> transform;
    Cow<Table> table;
    Cow<Fonts> font;
    Cow<Text> text;
    Cow<Flex> flex;
    Cow<Breaks> break_;
    Cow<SvgGeometry> svg;
    Cow<SvgPainting> svgPaint;
    Cow<Counters> counters;
    Cow<List> list;
    Cow<Columns> column;

    Cow<Map<Symbol, Css::Content>> customProps;
    Rc<Gfx::Fontface> fontFace;

    // Inlined fields
    ZIndex zIndex = Keywords::AUTO;
    Style::Overflows overflows;
    Gfx::Color color;
    ContentValue content = Keywords::NORMAL;
    Integer order;
    Aligns aligns;
    Display display;
    f32 opacity;

    // Small Field
    Float float_ = Keywords::NONE;
    Clear clear = Keywords::NONE;
    Visibility visibility = Keywords::VISIBLE;
    WritingMode writingMode = Keywords::HORIZONTAL_TB;
    Direction direction = Keywords::LTR;
    Style::PositionScheme position = Keywords::STATIC;
    BoxSizing boxSizing = Keywords::CONTENT_BOX;
    TableLayout tableLayout = Keywords::AUTO;

    ComputedValues() : fontFace(Gfx::Fontface::fallback()) {}

    void setCustomProp(Str name, Css::Content value) {
        setCustomProp(Symbol::from(name), value);
    }

    void setCustomProp(Symbol name, Css::Content value) {
        customProps.cow().put(name, value);
    }

    Opt<Css::Content const&> getCustomProp(Symbol name) const {
        return customProps->lookup(name);
    }

    bool hasCustomProp(Symbol name) const {
        return customProps->contains(name);
    }
};
} // namespace Vaev::Style
