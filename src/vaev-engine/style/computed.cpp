export module Vaev.Engine:style.computed;

import Karm.Core;
import Karm.Gfx;
import Karm.Math;

import :css;
import :css.values;
import :style.values;

using namespace Karm;

namespace Vaev::Style {

// https://www.w3.org/TR/css-cascade/#computed
// MARK: Groupings -----------------------------------------------------------
// Bundles of computed longhands sharing storage in ComputedValues.

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

export struct Table {
    CaptionSide captionSide = Keywords::TOP;
    BorderSpacing spacing;
    BorderCollapse collapse = Keywords::SEPARATE;
};

export struct SvgGeometry {
    PixelPercentage x = 0_px;
    PixelPercentage y = 0_px;
    PixelPercentage cx = 0_px;
    PixelPercentage cy = 0_px;
    PixelPercentage r = 0_px;

    Union<String, None> d = NONE;
    Opt<SvgViewBox> viewBox = NONE;
};

export struct SvgPainting {
    f64 fillOpacity = 1;
    PixelPercentage strokeWidth = 1_px;
    f64 strokeOpacity = 1;
    SvgPaint fill = Some(Gfx::BLACK);
    SvgPaint stroke = NONE;
};

using Clip = Opt<BasicShape>;

export struct Inherited {};

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
    Math::Radii<PixelPercentage> radii = {0_px};

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
    BaselineShift baselineShift = PixelPercentage{0_px};
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
    Pixel offset = 0_px;
    Union<Keywords::Auto, Gfx::BorderStyle> style = Gfx::BorderStyle::NONE;
    Gfx::Color color = Gfx::BLUE500;
};

export using Paddings = Math::Insets<PixelPercentage>;

export using BoxInsets = Math::Insets<Union<Keywords::Auto, PixelPercentage>>;

export struct Sizing {
    Size width = Keywords::AUTO, height = Keywords::AUTO;
    Size minWidth = Keywords::AUTO, minHeight = Keywords::AUTO;
    MaxSize maxWidth = Keywords::NONE, maxHeight = Keywords::NONE;
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
    Cow<Paddings> padding = Paddings(0_px);
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
