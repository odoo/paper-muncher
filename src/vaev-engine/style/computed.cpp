export module Vaev.Engine:style.computed;

import Karm.Core;
import Karm.Gfx;
import Karm.Math;

import :css;
import :values;

using namespace Karm;

namespace Vaev::Style {

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

using ClipProps = Opt<BasicShape>;

export struct InlineProps {
    BaselineSource baselineSource = Keywords::AUTO;
    AlignmentBaseline alignmentBaseline = Keywords::BASELINE;
    BaselineShift baselineShift = Calc<PercentOr<Length>>(Length{});
};

export struct OutlineProps {
    LineWidth width = Keywords::MEDIUM;
    Calc<Length> offset = 0_au;
    Union<Keywords::Auto, Gfx::BorderStyle> style = Gfx::BorderStyle::NONE;
    Color color = Gfx::BLUE500;

    operator SpecifiedOutline() const {
        return {
            width,
            offset,
            style,
            Color{color}
        };
    }
};

export struct InheritedProps {
    // TABLE
    CaptionSide captionSide = CaptionSide::TOP;
    BorderSpacing borderSpacing = {0_au, 0_au};
    BorderCollapse borderCollapse = BorderCollapse::SEPARATE;

    // INLINE
    DominantBaseline dominantBaseline = Keywords::AUTO;

    // TEXT
    TextAlign textAlign = TextAlign::START;
    TextTransform textTransform = TextTransform::NONE;
    WhiteSpace whiteSpace = WhiteSpace::NORMAL;

    // LIST
    ListImage listImage = Keywords::NONE;
    ListType listType = CustomIdent{"disc"_sym};
    ListPosition listPosition = Keywords::OUTSIDE;
    MarkerSide markerSide = Keywords::MATCH_SELF;

    // FONT
    Vec<FontFamily> fontFamilies = {"sans-serif"_sym};
    Gfx::FontWeight fontWeight = Gfx::FontWeight::REGULAR;
    FontWidth fontWidth = FontWidth::NORMAL;
    FontStyle fontStyle = FontStyle::NORMAL;
};

// https://www.w3.org/TR/css-cascade/#computed
export struct ComputedValues {
    Cow<InheritedProps> inherited;
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
    Cow<FlexProps> flex;
    Cow<BreakProps> break_;
    Cow<SvgProps> svg;
    Cow<SvgPaintProps> svgPaint;
    Cow<CounterProps> counters;

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
    Au fontSize;

    // Inline inherit fields
    // FIXME: Reduce
    ComputedLineHeight lineHeight = Keywords::NORMAL;

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
