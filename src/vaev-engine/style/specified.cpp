export module Vaev.Engine:style.specified;

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
        .xOffset = CalcValue<PercentOr<Length>>{Percent{0}},
        .yOffset = CalcValue<PercentOr<Length>>{Percent{0}},
    };
    TransformBox box = Keywords::BORDER_BOX;

    bool has() const {
        return transform != Keywords::NONE;
    }
};

using ClipProps = Opt<BasicShape>;

// https://www.w3.org/TR/css-cascade/#specified
export struct SpecifiedValues {
    static SpecifiedValues const& initial();

    SpecifiedValues() : fontFace(Gfx::Fontface::fallback()) {}

    Cow<Gaps> gaps;
    Cow<BackgroundProps> backgrounds;
    Cow<BorderProps> borders;
    Cow<Margin> margin = makeCow<Margin>(Width(CalcValue<PercentOr<Length>>(Length(0_au)))); // FIXME
    Cow<Outline> outline;
    Cow<Padding> padding = makeCow<Padding>(Length(0_au)); // FIXME
    Cow<SizingProps> sizing;
    Cow<Baseline> baseline;
    Cow<Offsets> offsets = makeCow<Offsets>(Width(Keywords::AUTO)); // FIXME
    Cow<ClipProps> clip;
    Cow<TransformProps> transform;
    Cow<TableProps> table;
    Cow<FontProps> font;
    Cow<TextProps> text;
    Cow<FlexProps> flex;
    Cow<BreakProps> break_;
    Cow<SVGProps> svg;

    Cow<Map<Symbol, Css::Content>> variables;
    Rc<Gfx::Fontface> fontFace;

    // Inlined fields
    ZIndex zIndex = Keywords::AUTO;
    Overflows overflows;
    Gfx::Color color;
    String content = ""s;
    Integer order;
    AlignProps aligns;
    Display display;
    f16 opacity;

    // Small Field
    Float float_ = Float::NONE;
    Clear clear = Clear::NONE;
    Visibility visibility;
    WritingMode writingMode;
    Direction direction;
    Position position;
    BoxSizing boxSizing;

    void inherit(SpecifiedValues const& parent) {
        color = parent.color;
        font = parent.font;
        text = parent.text;
        variables = parent.variables;
        visibility = parent.visibility;

        if (not parent.svg.defaulted()) {
            svg.cow().fillOpacity = parent.svg->fillOpacity;
            svg.cow().strokeWidth = parent.svg->strokeWidth;
            svg.cow().fill = parent.svg->fill;
            svg.cow().stroke = parent.svg->stroke;
        }
    }

    void setCustomProp(Str varName, Css::Content value) {
        setCustomProp(Symbol::from(varName), value);
    }

    void setCustomProp(Symbol varName, Css::Content value) {
        variables.cow().put(varName, value);
    }

    Css::Content getCustomProp(Str varName) const {
        auto value = variables->access(Symbol::from(varName));
        if (value)
            return *value;
        return {};
    }

    void repr(Io::Emit& e) const {
        e("(computed");
        e(" color: {}", color);
        e(" opacity: {}", opacity);
        e(" aligns: {}", aligns);
        e(" gaps: {}", gaps);
        e(" backgrounds: {}", backgrounds);
        e(" baseline: {}", baseline);
        e(" borders: {}", borders);
        e(" margin: {}", margin);
        e(" padding: {}", padding);
        e(" boxSizing: {}", boxSizing);
        e(" sizing: {}", sizing);
        e(" overflows: {}", overflows);
        e(" position: {}", position);
        e(" offsets: {}", offsets);
        e(" writingMode: {}", writingMode);
        e(" direction: {}", direction);
        e(" display: {}", display);
        e(" order: {}", order);
        e(" visibility: {}", visibility);
        e(" table: {}", table);
        e(" font: {}", font);
        e(" text: {}", text);
        e(" flex: {}", flex);
        e(" break: {}", break_);
        e(" float: {}", float_);
        e(" clear: {}", clear);
        e(" svg: {}", svg);
        e(" zIndex: {}", zIndex);
        e(" variables: {}", variables);
        e(")");
    }
};

} // namespace Vaev::Style
