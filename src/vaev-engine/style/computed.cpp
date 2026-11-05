export module Vaev.Engine:style.computed;

import Karm.Core;
import Karm.Gfx;
import Karm.Math;

import :css;
import :values;

using namespace Karm;

namespace Vaev::Style {

struct Fonts {
    Vec<FontFamily> families = {"sans-serif"_sym};
    Gfx::FontWeight weight = Gfx::FontWeight::REGULAR;
    FontWidth width = FontWidth::NORMAL;
    FontStyle style = FontStyle::NORMAL;
    Au size;
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
    Overflows overflows;
    Gfx::Color color;
    Content content = Keywords::NORMAL;
    Integer order;
    Style::Aligns aligns;
    Display display;
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
