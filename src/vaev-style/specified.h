#pragma once

#include <karm-base/cow.h>
#include <vaev-css/parser.h>
#include <vaev-values/align.h>
#include <vaev-values/background.h>
#include <vaev-values/baseline.h>
#include <vaev-values/basic-shape.h>
#include <vaev-values/borders.h>
#include <vaev-values/break.h>
#include <vaev-values/color.h>
#include <vaev-values/display.h>
#include <vaev-values/flex.h>
#include <vaev-values/float.h>
#include <vaev-values/font.h>
#include <vaev-values/insets.h>
#include <vaev-values/line.h>
#include <vaev-values/outline.h>
#include <vaev-values/overflow.h>
#include <vaev-values/sizing.h>
#include <vaev-values/table.h>
#include <vaev-values/text.h>
#include <vaev-values/transform.h>
#include <vaev-values/visibility.h>
#include <vaev-values/z-index.h>

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

// https://www.w3.org/TR/css-cascade/#specified
struct SpecifiedValues {
    static SpecifiedValues const& initial();

    Gfx::Color color;
    Number opacity;
    String content = ""s;

    AlignProps aligns;
    Cow<Gaps> gaps;

    Cow<BackgroundProps> backgrounds;
    Cow<BorderProps> borders;
    Cow<Margin> margin = makeCow<Margin>(Width(CalcValue<PercentOr<Length>>(Length(0_au)))); // FIXME
    Cow<Outline> outline;
    Cow<Padding> padding = makeCow<Padding>(Length(0_au)); // FIXME
    BoxSizing boxSizing;
    Cow<SizingProps> sizing;
    Overflows overflows;
    Opt<BasicShape> clip;

    // CSS Inline Layout Module Level 3
    // https://drafts.csswg.org/css-inline-3/
    Cow<Baseline> baseline;

    // 9.3 Positioning schemes
    // https://www.w3.org/TR/CSS22/visuren.html#positioning-scheme
    Position position;
    Cow<Offsets> offsets = makeCow<Offsets>(Width(Keywords::AUTO)); // FIXME

    // CSS Writing Modes Level 3
    // https://www.w3.org/TR/css-writing-modes-3
    WritingMode writingMode;
    Direction direction;

    // CSS Display Module Level 3
    // https://www.w3.org/TR/css-display-3
    Display display;
    Integer order;
    Visibility visibility;
    Cow<TransformProps> transform;
    // https://w3.org/TR/css-tables-3/#table-structure
    Cow<TableProps> table;

    // CSS Fonts Module Level 4
    // https://www.w3.org/TR/css-fonts-4/
    Cow<FontProps> font;
    Cow<TextProps> text;

    Cow<FlexProps> flex;
    Cow<BreakProps> break_;

    Cow<Map<String, Css::Content>> variables;

    Float float_ = Float::NONE;
    Clear clear = Clear::NONE;

    // https://drafts.csswg.org/css2/#z-index
    ZIndex zIndex = Keywords::AUTO;

    void inherit(SpecifiedValues const& parent);

    void repr(Io::Emit& e) const;

    void setCustomProp(Str varName, Css::Content value);

    Css::Content getCustomProp(Str varName) const;
};

} // namespace Vaev::Style
