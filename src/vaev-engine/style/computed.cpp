export module Vaev.Engine:style.computed;

import Karm.Core;
import Karm.Gfx;
import Karm.Math;

import :css;
import :css.values;
import :style.values;

using namespace Karm;

namespace Vaev::Style {

export struct ComputedValues {
    struct Fonts {
        Vec<FontFamily> families = {"sans-serif"_sym};
        f64 weight = 0;
        Percent width = Percent{100};
        FontStyle style;
        Px size;
    };

    struct Transforms {
        Transform transform = Keywords::NONE;
        Position origin = {Percent{0}, Percent{0}};
        TransformBox box = Keywords::BORDER_BOX;
    };

    struct Table {
        CaptionSide captionSide = Keywords::TOP;
        BorderSpacing spacing;
        BorderCollapse collapse = Keywords::SEPARATE;
    };

    struct SvgGeometry {
        PxPercent x = 0_px;
        PxPercent y = 0_px;
        PxPercent cx = 0_px;
        PxPercent cy = 0_px;
        PxPercent r = 0_px;
        Union<Keywords::Auto, PxPercent> rx = Keywords::AUTO;
        Union<Keywords::Auto, PxPercent> ry = Keywords::AUTO;

        Union<String, None> d = NONE;
        Opt<SvgViewBox> viewBox = NONE;
    };

    struct SvgPainting {
        f64 fillOpacity = 1;
        PxPercent strokeWidth = 1_px;
        f64 strokeOpacity = 1;
        SvgPaint fill = Some(Gfx::BLACK);
        SvgPaint stroke = NONE;
    };

    struct Aligns {
        Align alignContent;
        Align justifyContent;
        Align justifySelf;
        Align alignSelf;
        Align justifyItems;
        Align alignItems;
    };

    struct Backgrounds {
        Color color = Gfx::ALPHA;
        Vec<BackgroundLayer> layers = {};
    };

    struct Borders {
        Border top, start, bottom, end;
        Math::Radii<PxPercent> radii = {0_px};
    };

    struct Breaks {
        BreakBetween before = Keywords::AUTO;
        BreakBetween after = Keywords::AUTO;
        BreakInside inside = Keywords::AUTO;
        Integer orphans = 2;
        Integer widows = 2;
    };

    struct Columns {
        Union<Keywords::Auto, Px> width = Keywords::AUTO;
        Union<Keywords::Auto, Px> height = Keywords::AUTO;
        ColumnCount count = Keywords::AUTO;
    };

    struct Counters {
        Vec<CounterReset> reset;
        Vec<CounterIncrement> increment;
        Vec<CounterIncrement> set;
    };

    struct Flex {
        FlexDirection direction = Keywords::ROW;
        FlexWrap wrap = Keywords::NOWRAP;
        FlexBasis basis = Keywords::AUTO;
        f64 grow = 0;
        f64 shrink = 1;
    };

    struct Gaps {
        Gap row = Keywords::NORMAL;
        Gap col = Keywords::NORMAL;
    };

    struct Inline {
        BaselineSource baselineSource = Keywords::AUTO;
        AlignmentBaseline alignmentBaseline = Keywords::BASELINE;
        DominantBaseline dominantBaseline = Keywords::AUTO;
        BaselineShift baselineShift = PxPercent{0_px};
        LineHeight lineHeight = Keywords::NORMAL;
    };

    struct List {
        ListImage image = Keywords::NONE;
        ListType type = CustomIdent{"disc"_sym};
        ListPosition position = Keywords::OUTSIDE;
        MarkerSide markerSide = Keywords::MATCH_SELF;
    };

    struct Outline {
        Px width = 3_px;
        Px offset = 0_px;
        Union<Keywords::Auto, Gfx::BorderStyle> style = Gfx::BorderStyle::NONE;
        Color color = Keywords::CURRENT_COLOR;
    };

    struct Sizing {
        Size width = Keywords::AUTO, height = Keywords::AUTO;
        Size minWidth = Keywords::AUTO, minHeight = Keywords::AUTO;
        MaxSize maxWidth = Keywords::NONE, maxHeight = Keywords::NONE;
    };

    struct Text {
        TextAlign align = Keywords::START;
        TextTransform transform = Keywords::NONE;
        WhiteSpace whiteSpace = Keywords::NORMAL;
    };

    struct Overflows {
        Overflow x = Keywords::VISIBLE;
        Overflow y = Keywords::VISIBLE;
        Overflow block = Keywords::VISIBLE;
        Overflow inline_ = Keywords::VISIBLE;
    };

    Cow<Gaps> gaps;
    Cow<Backgrounds> backgrounds;
    Cow<Borders> borders;
    Cow<Margins> margin = Margins(Keywords::AUTO);
    Cow<Outline> outline;
    Cow<Paddings> padding = Paddings(0_px);
    Cow<Sizing> sizing;
    Cow<Inline> inline_;
    Cow<BoxInsets> insets = BoxInsets(Keywords::AUTO);
    Cow<ClipPath> clip;
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
    Rc<Gfx::Fontface> fontFace = Gfx::Fontface::fallback();

    // Inlined fields
    ZIndex zIndex = Keywords::AUTO;
    Overflows overflows;
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
    PositionScheme position = Keywords::STATIC;
    BoxSizing boxSizing = Keywords::CONTENT_BOX;
    TableLayout tableLayout = Keywords::AUTO;

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
