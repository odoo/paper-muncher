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
    Transform transform = Keywords::NONE;
    TransformOrigin origin = {
        .xOffset = Calc<PercentOr<Length>>{Percent{0}},
        .yOffset = Calc<PercentOr<Length>>{Percent{0}},
    };
    TransformBox box = Keywords::BORDER_BOX;

    bool has() const {
        return transform != Keywords::NONE;
    }
};


export struct TableProps {
    TableLayout tableLayout = TableLayout::AUTO;
    usize span = 1;
    usize rowSpan = 1;
    usize colSpan = 1;
};

export struct TableInheritedProps {
    CaptionSide captionSide = CaptionSide::TOP;
    BorderSpacing spacing = {0_au, 0_au};
    BorderCollapse collapse = BorderCollapse::SEPARATE;
};

export struct SvgProps {
    PercentOr<Length> x = Length{0_au};
    PercentOr<Length> y = Length{0_au};
    PercentOr<Length> cx = Length{0_au};
    PercentOr<Length> cy = Length{0_au};
    PercentOr<Length> r = Length{0_au};

    Union<String, None> d = NONE;
    Opt<SvgViewBox> viewBox = NONE;
};

export struct SvgPaintProps {
    Number fillOpacity = 1;
    PercentOr<Length> strokeWidth = Length{1_au};
    Number strokeOpacity = 1;
    SvgPaint fill = Some(Gfx::BLACK);
    SvgPaint stroke = NONE;
};

using ClipProps = Opt<BasicShape>;

export struct Inherited {};

// https://www.w3.org/TR/css-cascade/#computed
export struct ComputedValues {
    Cow<Gaps> gaps;
    Cow<BackgroundProps> backgrounds;
    Cow<BorderProps> borders;
    Cow<Margin> margin = Margin(Calc<PercentOr<Length>>(Length())); // FIXME
    Cow<OutlineProps> outline;
    Cow<Padding> padding = Padding(Length{}); // FIXME
    Cow<SizingProps> sizing;
    Cow<InlineProps> inline_;
    Cow<BoxInsets> insets = BoxInsets(Keywords::AUTO); // FIXME
    Cow<ClipProps> clip;
    Cow<TransformProps> transform;
    Cow<TableProps> table;
    Cow<TableInheritedProps> tableInherited;
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
    BoxSizing boxSizing;

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
