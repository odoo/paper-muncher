export module Vaev.Engine:style.computed;

import Karm.Core;
import Karm.Gfx;
import Karm.Math;

import :css;
import :values;

using namespace Karm;

namespace Vaev::Style {

struct FontProps {
    Vec<FontFamily> families = {"sans-serif"_sym};
    Gfx::FontWeight weight = Gfx::FontWeight::REGULAR;
    FontWidth width = FontWidth::NORMAL;
    FontStyle style = FontStyle::NORMAL;
    Au size;
};

struct TransformProps {
    ComputedTransform transform = Keywords::NONE;
    ComputedPosition origin = {Percent{0}, Percent{0}};
    TransformBox box = Keywords::BORDER_BOX;

    bool has() const {
        return not transform.is<Keywords::None>();
    }
};

export struct TableProps {
    CaptionSide captionSide = CaptionSide::TOP;
    ComputedBorderSpacing spacing;
    BorderCollapse collapse = BorderCollapse::SEPARATE;
};

export struct SvgProps {
    PixelPercentage x = Pixel{0};
    PixelPercentage y = Pixel{0};
    PixelPercentage cx = Pixel{0};
    PixelPercentage cy = Pixel{0};
    PixelPercentage r = Pixel{0};

    Union<String, None> d = NONE;
    Opt<SvgViewBox> viewBox = NONE;
};

export struct SvgPaintProps {
    f64 fillOpacity = 1;
    PixelPercentage strokeWidth = Pixel{1};
    f64 strokeOpacity = 1;
    SvgPaint fill = Some(Gfx::BLACK);
    SvgPaint stroke = NONE;
};

using ClipProps = Opt<ComputedBasicShape>;

export struct Inherited {};

// https://www.w3.org/TR/css-cascade/#computed
export struct ComputedValues {
    Cow<ComputedGaps> gaps;
    Cow<BackgroundProps> backgrounds;
    Cow<BorderProps> borders;
    Cow<ComputedMargins> margin = ComputedMargins(Keywords::AUTO);
    Cow<OutlineProps> outline;
    Cow<ComputedPaddings> padding = ComputedPaddings(Pixel{});
    Cow<SizingProps> sizing;
    Cow<InlineProps> inline_;
    Cow<ComputedBoxInsets> insets = ComputedBoxInsets(Keywords::AUTO);
    Cow<ClipProps> clip;
    Cow<TransformProps> transform;
    Cow<TableProps> table;
    Cow<FontProps> font;
    Cow<TextProps> text;
    Cow<FlexProps> flex;
    Cow<BreakProps> break_;
    Cow<SvgProps> svg;
    Cow<SvgPaintProps> svgPaint;
    Cow<CounterProps> counters;
    Cow<ListProps> list;
    Cow<ColumnProps> column;

    Cow<Map<Symbol, Css::Content>> customProps;
    Rc<Gfx::Fontface> fontFace;

    // Inlined fields
    ZIndex zIndex = Keywords::AUTO;
    Overflows overflows;
    Gfx::Color color;
    Content content = Keywords::NORMAL;
    Integer order;
    AlignProps aligns;
    Display display;
    f32 opacity;

    // Small Field
    Float float_ = Float::NONE;
    Clear clear = Clear::NONE;
    Visibility visibility;
    WritingMode writingMode = Keywords::HORIZONTAL_TB;
    Direction direction = Keywords::LTR;
    Position position = Keywords::STATIC;
    BoxSizing boxSizing = Keywords::CONTENT_BOX;
    TableLayout tableLayout = TableLayout::AUTO;

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
