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
    Computed<Transform> transform = Keywords::NONE;
    Computed<TransformOrigin> origin = {
        .xOffset = Calc<Au, Percent>{Percent{0}},
        .yOffset = Calc<Au, Percent>{Percent{0}},
    };
    TransformBox box = Keywords::BORDER_BOX;

    bool has() const {
        return transform != Keywords::NONE;
    }
};

export struct TableProps {
    CaptionSide captionSide = CaptionSide::TOP;
    Computed<BorderSpacing> spacing = {0_au, 0_au};
    BorderCollapse collapse = BorderCollapse::SEPARATE;
};

export struct SvgProps {
    Computed<Calc<Length, Percent>> x = 0_au;
    Computed<Calc<Length, Percent>> y = 0_au;
    Computed<Calc<Length, Percent>> cx = 0_au;
    Computed<Calc<Length, Percent>> cy = 0_au;
    Computed<Calc<Length, Percent>> r = 0_au;

    Union<String, None> d = NONE;
    Opt<SvgViewBox> viewBox = NONE;
};

export struct SvgPaintProps {
    Number fillOpacity = 1;
    Computed<Calc<Length, Percent>> strokeWidth = 1_au;
    Number strokeOpacity = 1;
    SvgPaint fill = Some(Gfx::BLACK);
    SvgPaint stroke = NONE;
};

using ClipProps = Opt<Computed<BasicShape>>;

export struct Inherited {};

// https://www.w3.org/TR/css-cascade/#computed
export struct ComputedValues {
    Cow<Computed<Gaps>> gaps;
    Cow<BackgroundProps> backgrounds;
    Cow<BorderProps> borders;
    Cow<Computed<Margin>> margin = Computed<Margin>(Calc<Au, Percent>(0_au));
    Cow<OutlineProps> outline;
    Cow<Computed<Padding>> padding = Computed<Padding>(0_au);
    Cow<SizingProps> sizing;
    Cow<InlineProps> inline_;
    Cow<Computed<BoxInsets>> insets = Computed<BoxInsets>(Keywords::AUTO);
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
    WritingMode writingMode;
    Direction direction;
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
