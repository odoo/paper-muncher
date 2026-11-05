export module Vaev.Engine:style.computed;

import Karm.Core;
import Karm.Gfx;
import Karm.Math;

import :css;
import :values;

using namespace Karm;

namespace Vaev::Style {

struct Fonts {
    Vec<Css::FontFamily> families = {"sans-serif"_sym};
    Gfx::FontWeight weight = Gfx::FontWeight::REGULAR;
    Css::FontWidth width = Css::FontWidth::NORMAL;
    Css::FontStyle style = Css::FontStyle::NORMAL;
    Au size;
};

struct Transforms {
    Transform transform = Keywords::NONE;
    Position origin = {Percent{0}, Percent{0}};
    Css::TransformBox box = Keywords::BORDER_BOX;

    bool has() const {
        return not transform.is<Keywords::None>();
    }
};

export struct Table {
    Css::CaptionSide captionSide = Keywords::TOP;
    BorderSpacing spacing;
    Css::BorderCollapse collapse = Keywords::SEPARATE;
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
    Css::ZIndex zIndex = Keywords::AUTO;
    Css::Overflows overflows;
    Gfx::Color color;
    Css::ContentValue content = Keywords::NORMAL;
    Css::Integer order;
    Style::Aligns aligns;
    Css::Display display;
    f32 opacity;

    // Small Field
    Css::Float float_ = Keywords::NONE;
    Css::Clear clear = Keywords::NONE;
    Css::Visibility visibility = Keywords::VISIBLE;
    Css::WritingMode writingMode = Keywords::HORIZONTAL_TB;
    Css::Direction direction = Keywords::LTR;
    Css::Position position = Keywords::STATIC;
    Css::BoxSizing boxSizing = Keywords::CONTENT_BOX;
    Css::TableLayout tableLayout = Keywords::AUTO;

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
