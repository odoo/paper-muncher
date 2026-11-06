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

struct Fonts {
    Vec<Css::FontFamily> families = {"sans-serif"_sym};
    Gfx::FontWeight weight = Gfx::FontWeight::REGULAR;
    Css::FontWidth width = Css::FontWidth::NORMAL;
    Css::FontStyle style = Css::FontStyle::NORMAL;
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
    Opt<Css::SvgViewBox> viewBox = NONE;
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
    Css::Align alignContent;

    // https://drafts.csswg.org/css-align-3/#propdef-justify-content
    Css::Align justifyContent;

    // https://drafts.csswg.org/css-align-3/#propdef-justify-self
    Css::Align justifySelf;

    // https://drafts.csswg.org/css-align-3/#propdef-align-self
    Css::Align alignSelf;

    // https://drafts.csswg.org/css-align-3/#propdef-justify-items
    Css::Align justifyItems;

    // https://drafts.csswg.org/css-align-3/#propdef-align-items
    Css::Align alignItems;
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

    operator Css::SpecifiedBorder() const {
        return {toSpecified(width), style, toSpecified(color)};
    }
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
    Vec<Css::CounterReset> reset;
    Vec<Css::CounterIncrement> increment;
    Vec<Css::CounterSet> set;
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
    Css::ListImage image = Keywords::NONE;
    Css::ListType type = CustomIdent{"disc"_sym};
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
    Css::ContentValue content = Keywords::NORMAL;
    Integer order;
    Aligns aligns;
    Css::Display display;
    f32 opacity;

    // Small Field
    Float float_ = Keywords::NONE;
    Clear clear = Keywords::NONE;
    Visibility visibility = Keywords::VISIBLE;
    WritingMode writingMode = Keywords::HORIZONTAL_TB;
    Direction direction = Keywords::LTR;
    Vaev::Position position = Keywords::STATIC;
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
