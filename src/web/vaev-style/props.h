#pragma once

#include <karm-io/emit.h>
#include <karm-mime/url.h>
#include <vaev-base/basic-shape.h>
#include <vaev-base/color.h>
#include <vaev-base/font.h>

#include "base.h"
#include "computed.h"
#include "values.h"

// https://www.w3.org/TR/CSS22/propidx.html

namespace Vaev::Style {

// MARK: Props -----------------------------------------------------------------

// NOTE: This list should be kept alphabetically sorted.

// MARK: Align -----------------------------------------------------------------
// https://drafts.csswg.org/css-align-3

// https://drafts.csswg.org/css-align-3/#propdef-align-content
struct AlignContentProp {
    Align value = initial();

    static constexpr Str name() { return "align-content"; }

    static constexpr Align initial() { return Align::Keywords::STRETCH; }

    void apply(ComputedStyle& c) const {
        c.aligns.alignContent = value;
    }

    static Align load(ComputedStyle const& c) {
        return c.aligns.alignContent;
    }

    Res<> parse(Cursor<Css::Sst>& c) {
        value = try$(parseValue<Align>(c));
        return Ok();
    }
};

// https://drafts.csswg.org/css-align-3/#propdef-justify-content
struct JustifyContentProp {
    Align value = initial();

    static constexpr Str name() { return "justify-content"; }

    static constexpr Align initial() { return Align::Keywords::FLEX_START; }

    void apply(ComputedStyle& c) const {
        c.aligns.justifyContent = value;
    }

    static Align load(ComputedStyle const& c) {
        return c.aligns.justifyContent;
    }

    Res<> parse(Cursor<Css::Sst>& c) {
        value = try$(parseValue<Align>(c));
        return Ok();
    }
};

// https://drafts.csswg.org/css-align-3/#propdef-justify-self
struct JustifySelfProp {
    Align value = initial();

    static constexpr Str name() { return "justify-self"; }

    static constexpr Align initial() { return {}; }

    void apply(ComputedStyle& c) const {
        c.aligns.justifySelf = value;
    }

    static Align load(ComputedStyle const& c) {
        return c.aligns.justifySelf;
    }

    Res<> parse(Cursor<Css::Sst>& c) {
        value = try$(parseValue<Align>(c));
        return Ok();
    }
};

// https://drafts.csswg.org/css-align-3/#propdef-align-self
struct AlignSelfProp {
    Align value = initial();

    static constexpr Str name() { return "align-self"; }

    static constexpr Align initial() { return Align::Keywords::AUTO; }

    void apply(ComputedStyle& c) const {
        c.aligns.alignSelf = value;
    }

    static Align load(ComputedStyle const& c) {
        return c.aligns.alignSelf;
    }

    Res<> parse(Cursor<Css::Sst>& c) {
        value = try$(parseValue<Align>(c));
        return Ok();
    }
};

// https://drafts.csswg.org/css-align-3/#propdef-justify-items
struct JustifyItemsProp {
    Align value = initial();

    static constexpr Str name() { return "justify-items"; }

    static constexpr Align initial() { return {}; }

    void apply(ComputedStyle& c) const {
        c.aligns.justifyItems = value;
    }

    static Align load(ComputedStyle const& c) {
        return c.aligns.justifyItems;
    }

    Res<> parse(Cursor<Css::Sst>& c) {
        value = try$(parseValue<Align>(c));
        return Ok();
    }
};

// https://drafts.csswg.org/css-align-3/#propdef-align-items
struct AlignItemsProp {
    Align value = initial();

    static constexpr Str name() { return "align-items"; }

    static constexpr Align initial() { return Align::Keywords::STRETCH; }

    void apply(ComputedStyle& c) const {
        c.aligns.alignItems = value;
    }

    static Align load(ComputedStyle const& c) {
        return c.aligns.alignItems;
    }

    Res<> parse(Cursor<Css::Sst>& c) {
        value = try$(parseValue<Align>(c));
        return Ok();
    }
};

// https://drafts.csswg.org/css-align-3/#column-row-gap
struct RowGapProp {
    Gap value = initial();

    static constexpr Str name() { return "row-gap"; }

    static constexpr Keywords::Normal initial() { return Keywords::NORMAL; }

    void apply(ComputedStyle& c) const {
        c.gaps.cow().y = value;
    }

    static Gap load(ComputedStyle const& c) {
        return c.gaps->y;
    }

    Res<> parse(Cursor<Css::Sst>& c) {
        value = try$(parseValue<Gap>(c));
        return Ok();
    }
};

// https://drafts.csswg.org/css-align-3/#column-row-gap
struct ColumnGapProp {
    Gap value = initial();

    static constexpr Str name() { return "column-gap"; }

    static constexpr Keywords::Normal initial() { return Keywords::NORMAL; }

    void apply(ComputedStyle& c) const {
        c.gaps.cow().x = value;
    }

    static Gap load(ComputedStyle const& c) {
        return c.gaps->x;
    }

    Res<> parse(Cursor<Css::Sst>& c) {
        value = try$(parseValue<Gap>(c));
        return Ok();
    }
};

// MARK: Baselines ------------------------------------------------------

// https://www.w3.org/TR/css-inline-3/#dominant-baseline-property
struct DominantBaselineProp {
    DominantBaseline value = initial();

    static constexpr Str name() { return "dominant-baseline"; }

    static constexpr Keywords::Auto initial() { return Keywords::AUTO; }

    void apply(ComputedStyle& c) const {
        c.baseline.cow().dominant = value;
    }

    static DominantBaseline load(ComputedStyle const& c) {
        return c.baseline->dominant;
    }

    Res<> parse(Cursor<Css::Sst>& c) {
        value = try$(parseValue<DominantBaseline>(c));
        return Ok();
    }
};

// https://www.w3.org/TR/css-inline-3/#baseline-source
struct BaselineSourceProp {
    BaselineSource value = initial();

    static constexpr Str name() { return "baseline-source"; }

    static constexpr Keywords::Auto initial() { return Keywords::AUTO; }

    void apply(ComputedStyle& c) const {
        c.baseline.cow().source = value;
    }

    static BaselineSource load(ComputedStyle const& c) {
        return c.baseline->source;
    }

    Res<> parse(Cursor<Css::Sst>& c) {
        value = try$(parseValue<BaselineSource>(c));
        return Ok();
    }
};

// https://www.w3.org/TR/css-inline-3/#alignment-baseline-property
struct AlignmentBaselineProp {
    AlignmentBaseline value = initial();

    static constexpr Str name() { return "alignment-baseline"; }

    static constexpr Keywords::Baseline initial() { return Keywords::BASELINE; }

    void apply(ComputedStyle& c) const {
        c.baseline.cow().alignment = value;
    }

    static AlignmentBaseline load(ComputedStyle const& c) {
        return c.baseline->alignment;
    }

    Res<> parse(Cursor<Css::Sst>& c) {
        value = try$(parseValue<AlignmentBaseline>(c));
        return Ok();
    }
};

// MARK: Background Color ------------------------------------------------------

// https://www.w3.org/TR/CSS22/colors.html#propdef-background-color
struct BackgroundColorProp {
    Color value = initial();

    static constexpr Str name() { return "background-color"; }

    static constexpr Color initial() { return TRANSPARENT; }

    void apply(ComputedStyle& c) const {
        c.backgrounds.cow().color = value;
    }

    static Color load(ComputedStyle const& c) {
        return c.backgrounds->color;
    }

    Res<> parse(Cursor<Css::Sst>& c) {
        value = try$(parseValue<Color>(c));
        return Ok();
    }
};

// MARK: Background Image ------------------------------------------------------

// https://www.w3.org/TR/CSS22/colors.html#propdef-background-attachment
struct BackgroundAttachmentProp {
    Vec<BackgroundAttachment> value = initial();

    static constexpr Str name() { return "background-attachment"; }

    static constexpr Array<BackgroundAttachment, 1> initial() {
        return {BackgroundAttachment::SCROLL};
    }

    void apply(ComputedStyle& c) const {
        auto& layers = c.backgrounds.cow().layers;
        layers.resize(max(layers.len(), value.len()));
        for (usize i = 0; i < value.len(); ++i)
            layers[i].attachment = value[i];
    }

    static Vec<BackgroundAttachment> load(ComputedStyle const& c) {
        Vec<BackgroundAttachment> layers;
        for (auto const& l : c.backgrounds->layers)
            layers.pushBack(l.attachment);
        return layers;
    }
};

// https://www.w3.org/TR/CSS22/colors.html#propdef-background-image
struct BackgroundImageProp {
    Vec<Image> value = initial();

    static constexpr Str name() { return "background-image"; }

    static Vec<Image> initial() { return {}; }

    void apply(ComputedStyle&) const {
        // TODO
    }

    static Vec<Image> load(ComputedStyle const&) {
        return {};
    }

    Res<> parse(Cursor<Css::Sst>&) {
        // TODO
        return Ok();
    }
};

// https://www.w3.org/TR/CSS22/colors.html#propdef-background-position
struct BackgroundPositionProp {
    Vec<BackgroundPosition> value = initial();

    static constexpr Str name() { return "background-position"; }

    static constexpr Vec<BackgroundPosition> initial() {
        return {};
    }

    void apply(ComputedStyle&) const {
        // TODO
    }

    static Vec<BackgroundPosition> load(ComputedStyle const&) {
        return {};
    }

    Res<> parse(Cursor<Css::Sst>&) {
        // TODO
        return Ok();
    }
};

// https://www.w3.org/TR/CSS22/colors.html#propdef-background-repeat
struct BackgroundRepeatProp {
    Vec<BackgroundRepeat> value = initial();

    static constexpr Str name() { return "background-repeat"; }

    static constexpr Array<BackgroundRepeat, 1> initial() {
        return {BackgroundRepeat::REPEAT};
    }

    void apply(ComputedStyle&) const {
        // TODO
    }

    static Vec<BackgroundRepeat> load(ComputedStyle const&) {
        return {};
    }

    Res<> parse(Cursor<Css::Sst>&) {
        // TODO
        return Ok();
    }
};

// https://www.w3.org/TR/CSS22/colors.html#x10
struct BackgroundProp {
    // FIXME: this should cover everything else
    BackgroundProps value = initial();

    static constexpr Str name() { return "background"; }

    static BackgroundProps initial() { return {TRANSPARENT}; }

    void apply(ComputedStyle& c) const {
        c.backgrounds.cow() = value;
    }

    static BackgroundProps load(ComputedStyle const& c) {
        return *c.backgrounds;
    }

    Res<> parse(Cursor<Css::Sst>& c) {
        value.color = try$(parseValue<Color>(c));
        return Ok();
    }
};

// https://www.w3.org/TR/CSS22/colors.html#propdef-color
struct ColorProp {
    Color value = initial();

    static constexpr Str name() { return "color"; }

    static constexpr Color initial() { return BLACK; }

    static void inherit(ComputedStyle const& parent, ComputedStyle& child) {
        child.color = parent.color;
    }

    void apply(ComputedStyle& c) const {
        c.color = resolve(value, Gfx::BLACK);
    }

    void apply(ComputedStyle const& parent, ComputedStyle& c) const {
        c.color = resolve(value, parent.color);
    }

    static Color load(ComputedStyle const& c) {
        return c.color;
    }

    Res<> parse(Cursor<Css::Sst>& c) {
        value = try$(parseValue<Color>(c));
        return Ok();
    }
};

// https://www.w3.org/TR/CSS22/visuren.html#propdef-display
struct DisplayProp {
    Display value = initial();

    static constexpr Str name() { return "display"; }

    static constexpr Display initial() { return {Display::FLOW, Display::INLINE}; }

    void apply(ComputedStyle& s) const {
        s.display = value;
    }

    static Display load(ComputedStyle const& s) {
        return s.display;
    }

    Res<> parse(Cursor<Css::Sst>& c) {
        value = try$(parseValue<Display>(c));
        return Ok();
    }
};

// https://www.w3.org/TR/CSS21/tables.html#propdef-table-layout
struct TableLayoutProp {
    TableLayout value = initial();

    static constexpr Str name() { return "table-layout"; }

    static constexpr TableLayout initial() { return TableLayout::AUTO; }

    void apply(ComputedStyle& s) const {
        s.table.cow().tableLayout = value;
    }

    static TableLayout load(ComputedStyle const& s) {
        return s.table->tableLayout;
    }

    Res<> parse(Cursor<Css::Sst>& c) {
        value = try$(parseValue<TableLayout>(c));
        return Ok();
    }
};

// https://www.w3.org/TR/CSS21/tables.html#caption-position
struct CaptionSideProp {
    CaptionSide value = initial();

    static constexpr Str name() { return "caption-side"; }

    static constexpr CaptionSide initial() { return CaptionSide::TOP; }

    void apply(ComputedStyle& s) const {
        s.table.cow().captionSide = value;
    }

    static CaptionSide load(ComputedStyle const& s) {
        return s.table->captionSide;
    }

    Res<> parse(Cursor<Css::Sst>& c) {
        value = try$(parseValue<CaptionSide>(c));
        return Ok();
    }
};

// MARK: Borders ---------------------------------------------------------------

// https://www.w3.org/TR/CSS22/box.html#propdef-border-color
struct BorderTopColorProp {
    Color value = initial();

    static constexpr Str name() { return "border-top-color"; }

    static constexpr Color initial() { return BLACK; }

    void apply(ComputedStyle& c) const {
        c.borders.cow().top.color = value;
    }

    static Color load(ComputedStyle const& c) {
        return c.borders->top.color;
    }

    Res<> parse(Cursor<Css::Sst>& c) {
        value = try$(parseValue<Color>(c));
        return Ok();
    }
};

// https://www.w3.org/TR/CSS22/box.html#propdef-border-color
struct BorderRightColorProp {
    Color value = initial();

    static constexpr Str name() { return "border-right-color"; }

    static constexpr Color initial() { return BLACK; }

    void apply(ComputedStyle& c) const {
        c.borders.cow().end.color = value;
    }

    static Color load(ComputedStyle const& c) {
        return c.borders->end.color;
    }

    Res<> parse(Cursor<Css::Sst>& c) {
        value = try$(parseValue<Color>(c));
        return Ok();
    }
};

// https://www.w3.org/TR/CSS22/box.html#propdef-border-color
struct BorderBottomColorProp {
    Color value = initial();

    static constexpr Str name() { return "border-bottom-color"; }

    static constexpr Color initial() { return BLACK; }

    void apply(ComputedStyle& c) const {
        c.borders.cow().bottom.color = value;
    }

    static Color load(ComputedStyle const& c) {
        return c.borders->bottom.color;
    }

    Res<> parse(Cursor<Css::Sst>& c) {
        value = try$(parseValue<Color>(c));
        return Ok();
    }
};

// https://www.w3.org/TR/CSS22/box.html#propdef-border-color
struct BorderLeftColorProp {
    Color value = initial();

    static constexpr Str name() { return "border-left-color"; }

    static constexpr Color initial() { return BLACK; }

    void apply(ComputedStyle& c) const {
        c.borders.cow().start.color = value;
    }

    static Color load(ComputedStyle const& c) {
        return c.borders->start.color;
    }

    Res<> parse(Cursor<Css::Sst>& c) {
        value = try$(parseValue<Color>(c));
        return Ok();
    }
};

struct BorderColorProp {
    Math::Insets<Color> value = initial();

    static constexpr Str name() { return "border-color"; }

    static constexpr Math::Insets<Color> initial() { return {BLACK}; }

    void apply(ComputedStyle& c) const {
        auto& borders = c.borders.cow();
        borders.start.color = value.start;
        borders.end.color = value.end;
        borders.top.color = value.top;
        borders.bottom.color = value.bottom;
    }

    static Math::Insets<Color> load(ComputedStyle const& c) {
        return {
            c.borders->start.color,
            c.borders->end.color,
            c.borders->top.color,
            c.borders->bottom.color,
        };
    }

    Res<> parse(Cursor<Css::Sst>& c) {
        value = try$(parseValue<Math::Insets<Color>>(c));
        return Ok();
    }
};

// https://www.w3.org/TR/CSS22/box.html#border-style-properties

struct BorderStyle {
    Math::Insets<Gfx::BorderStyle> value = initial();

    static constexpr Str name() { return "border-style"; }

    static constexpr Math::Insets<Gfx::BorderStyle> initial() {
        return {Gfx::BorderStyle::NONE};
    }

    void apply(ComputedStyle& c) const {
        c.borders.cow().start.style = value.start;
        c.borders.cow().end.style = value.end;
        c.borders.cow().top.style = value.top;
        c.borders.cow().bottom.style = value.bottom;
    }

    static Math::Insets<Gfx::BorderStyle> load(ComputedStyle const& c) {
        return {
            c.borders->start.style,
            c.borders->end.style,
            c.borders->top.style,
            c.borders->bottom.style,
        };
    }

    Res<> parse(Cursor<Css::Sst>& c) {
        value = try$(parseValue<Math::Insets<Gfx::BorderStyle>>(c));
        return Ok();
    }
};

// https://www.w3.org/TR/CSS22/box.html#border-style-properties
struct BorderLeftStyleProp {
    Gfx::BorderStyle value = initial();

    static constexpr Str name() { return "border-left-style"; }

    static constexpr Gfx::BorderStyle initial() { return Gfx::BorderStyle::NONE; }

    void apply(ComputedStyle& c) const {
        c.borders.cow().start.style = value;
    }

    static Gfx::BorderStyle load(ComputedStyle const& c) {
        return c.borders->start.style;
    }

    Res<> parse(Cursor<Css::Sst>& c) {
        value = try$(parseValue<Gfx::BorderStyle>(c));
        return Ok();
    }
};

// https://www.w3.org/TR/CSS22/box.html#border-style-properties
struct BorderTopStyleProp {
    Gfx::BorderStyle value = initial();

    static constexpr Str name() { return "border-top-style"; }

    static constexpr Gfx::BorderStyle initial() { return Gfx::BorderStyle::NONE; }

    void apply(ComputedStyle& c) const {
        c.borders.cow().top.style = value;
    }

    static Gfx::BorderStyle load(ComputedStyle const& c) {
        return c.borders->top.style;
    }

    Res<> parse(Cursor<Css::Sst>& c) {
        value = try$(parseValue<Gfx::BorderStyle>(c));
        return Ok();
    }
};

// https://www.w3.org/TR/CSS22/box.html#border-style-properties
struct BorderRightStyleProp {
    Gfx::BorderStyle value = initial();

    static constexpr Str name() { return "border-right-style"; }

    static constexpr Gfx::BorderStyle initial() { return Gfx::BorderStyle::NONE; }

    void apply(ComputedStyle& c) const {
        c.borders.cow().end.style = value;
    }

    static Gfx::BorderStyle load(ComputedStyle const& c) {
        return c.borders->end.style;
    }

    Res<> parse(Cursor<Css::Sst>& c) {
        value = try$(parseValue<Gfx::BorderStyle>(c));
        return Ok();
    }
};

// https://www.w3.org/TR/CSS22/box.html#border-style-properties
struct BorderBottomStyleProp {
    Gfx::BorderStyle value = initial();

    static constexpr Str name() { return "border-bottom-style"; }

    static constexpr Gfx::BorderStyle initial() { return Gfx::BorderStyle::NONE; }

    void apply(ComputedStyle& c) const {
        c.borders.cow().bottom.style = value;
    }

    static Gfx::BorderStyle load(ComputedStyle const& c) {
        return c.borders->bottom.style;
    }

    Res<> parse(Cursor<Css::Sst>& c) {
        value = try$(parseValue<Gfx::BorderStyle>(c));
        return Ok();
    }
};

// https://www.w3.org/TR/css-backgrounds-3/#border-width
struct BorderTopWidthProp {
    LineWidth value = initial();

    static constexpr Str name() { return "border-top-width"; }

    static constexpr LineWidth initial() { return Keywords::MEDIUM; }

    void apply(ComputedStyle& c) const {
        c.borders.cow().top.width = value;
    }

    static LineWidth load(ComputedStyle const& c) {
        return c.borders->top.width;
    }

    Res<> parse(Cursor<Css::Sst>& c) {
        value = try$(parseValue<LineWidth>(c));
        return Ok();
    }
};

// https://www.w3.org/TR/css-backgrounds-3/#border-width
struct BorderRightWidthProp {
    LineWidth value = initial();

    static constexpr Str name() { return "border-right-width"; }

    static constexpr LineWidth initial() { return Keywords::MEDIUM; }

    void apply(ComputedStyle& c) const {
        c.borders.cow().end.width = value;
    }

    static LineWidth load(ComputedStyle const& c) {
        return c.borders->end.width;
    }

    Res<> parse(Cursor<Css::Sst>& c) {
        value = try$(parseValue<LineWidth>(c));
        return Ok();
    }
};

// https://www.w3.org/TR/css-backgrounds-3/#border-width
struct BorderBottomWidthProp {
    LineWidth value = initial();

    static constexpr Str name() { return "border-bottom-width"; }

    static constexpr LineWidth initial() { return Keywords::MEDIUM; }

    void apply(ComputedStyle& c) const {
        c.borders.cow().bottom.width = value;
    }

    static LineWidth load(ComputedStyle const& c) {
        return c.borders->bottom.width;
    }

    Res<> parse(Cursor<Css::Sst>& c) {
        value = try$(parseValue<CalcValue<Length>>(c));
        return Ok();
    }
};

// https://www.w3.org/TR/css-backgrounds-3/#border-width
struct BorderLeftWidthProp {
    LineWidth value = initial();

    static constexpr Str name() { return "border-left-width"; }

    static constexpr LineWidth initial() { return Keywords::MEDIUM; }

    void apply(ComputedStyle& c) const {
        c.borders.cow().start.width = value;
    }

    static LineWidth load(ComputedStyle const& c) {
        return c.borders->start.width;
    }

    Res<> parse(Cursor<Css::Sst>& c) {
        value = try$(parseValue<LineWidth>(c));
        return Ok();
    }
};

// https://drafts.csswg.org/css-backgrounds/#the-border-radius
struct BorderRadiusTopRight {
    Array<CalcValue<PercentOr<Length>>, 2> value = initial();

    static constexpr Str name() { return "border-top-right-radius"; }

    static constexpr Array<CalcValue<PercentOr<Length>>, 2> initial() {
        return makeArray<CalcValue<PercentOr<Length>>, 2>(Length{});
    }

    void apply(ComputedStyle& c) const {
        c.borders.cow().radii.c = value[0];
        c.borders.cow().radii.d = value[1];
    }

    static Array<CalcValue<PercentOr<Length>>, 2> load(ComputedStyle const& c) {
        return {
            c.borders->radii.c,
            c.borders->radii.d,
        };
    }

    Res<> parse(Cursor<Css::Sst>& c) {
        value[0] = try$(parseValue<CalcValue<PercentOr<Length>>>(c));
        if (c.ended()) {
            value[1] = value[0];
        } else {
            value[1] = try$(parseValue<CalcValue<PercentOr<Length>>>(c));
        }

        return Ok();
    }
};

// https://drafts.csswg.org/css-backgrounds/#the-border-radius
struct BorderRadiusTopLeft {
    Array<CalcValue<PercentOr<Length>>, 2> value = initial();

    static constexpr Str name() { return "border-top-left-radius"; }

    static constexpr Array<CalcValue<PercentOr<Length>>, 2> initial() {
        return makeArray<CalcValue<PercentOr<Length>>, 2>(Length{});
    }

    void apply(ComputedStyle& c) const {
        c.borders.cow().radii.a = value[1];
        c.borders.cow().radii.b = value[0];
    }

    static Array<CalcValue<PercentOr<Length>>, 2> load(ComputedStyle const& c) {
        return {
            c.borders->radii.a,
            c.borders->radii.b,
        };
    }

    Res<> parse(Cursor<Css::Sst>& c) {
        value[0] = try$(parseValue<CalcValue<PercentOr<Length>>>(c));
        eatWhitespace(c);
        if (c.ended()) {
            value[1] = value[0];
        } else {
            value[1] = try$(parseValue<CalcValue<PercentOr<Length>>>(c));
        }

        return Ok();
    }
};

// https://drafts.csswg.org/css-backgrounds/#the-border-radius
struct BorderRadiusBottomRight {
    Array<CalcValue<PercentOr<Length>>, 2> value = initial();

    static constexpr Str name() { return "border-bottom-right-radius"; }

    static constexpr Array<CalcValue<PercentOr<Length>>, 2> initial() {
        return makeArray<CalcValue<PercentOr<Length>>, 2>(Length{});
    }

    void apply(ComputedStyle& c) const {
        c.borders.cow().radii.e = value[1];
        c.borders.cow().radii.f = value[0];
    }

    static Array<CalcValue<PercentOr<Length>>, 2> load(ComputedStyle const& c) {
        return {
            c.borders->radii.e,
            c.borders->radii.f,
        };
    }

    Res<> parse(Cursor<Css::Sst>& c) {
        value[0] = try$(parseValue<CalcValue<PercentOr<Length>>>(c));
        if (c.ended()) {
            value[1] = value[0];
        } else {
            value[1] = try$(parseValue<CalcValue<PercentOr<Length>>>(c));
        }

        return Ok();
    }
};

// https://drafts.csswg.org/css-backgrounds/#the-border-radius
struct BorderRadiusBottomLeft {
    Array<CalcValue<PercentOr<Length>>, 2> value = initial();

    static constexpr Str name() { return "border-bottom-left-radius"; }

    static constexpr Array<CalcValue<PercentOr<Length>>, 2> initial() {
        return makeArray<CalcValue<PercentOr<Length>>, 2>(Length{});
    }

    void apply(ComputedStyle& c) const {
        c.borders.cow().radii.g = value[0];
        c.borders.cow().radii.h = value[1];
    }

    static Array<CalcValue<PercentOr<Length>>, 2> load(ComputedStyle const& c) {
        return {
            c.borders->radii.g,
            c.borders->radii.h,
        };
    }

    Res<> parse(Cursor<Css::Sst>& c) {
        value[0] = try$(parseValue<CalcValue<PercentOr<Length>>>(c));
        if (c.ended()) {
            value[1] = value[0];
        } else {
            value[1] = try$(parseValue<CalcValue<PercentOr<Length>>>(c));
        }

        return Ok();
    }
};

// https://drafts.csswg.org/css-backgrounds/#the-border-radius
struct BorderRadius {
    Math::Radii<CalcValue<PercentOr<Length>>> value = initial();

    static constexpr Str name() { return "border-radius"; }

    static Math::Radii<CalcValue<PercentOr<Length>>> initial() { return {CalcValue<PercentOr<Length>>(Length{})}; }

    void apply(ComputedStyle& c) const {
        c.borders.cow().radii = value;
    }

    static Math::Radii<CalcValue<PercentOr<Length>>> load(ComputedStyle const& c) {
        return c.borders->radii;
    }

    Res<> parse(Cursor<Css::Sst>& c) {
        value = try$(parseValue<Math::Radii<CalcValue<PercentOr<Length>>>>(c));
        return Ok();
    }
};

// https://www.w3.org/TR/css-backgrounds-3/#border-shorthands
struct BorderTopProp {
    Border value;

    static constexpr Str name() { return "border-top"; }

    void apply(ComputedStyle& c) const {
        c.borders.cow().top = value;
    }

    static Border load(ComputedStyle const& c) {
        return c.borders->top;
    }

    Res<> parse(Cursor<Css::Sst>& c) {
        while (not c.ended()) {
            auto width = parseValue<CalcValue<Length>>(c);
            if (width) {
                value.width = width.unwrap();
                continue;
            }

            auto color = parseValue<Color>(c);
            if (color) {
                value.color = color.unwrap();
                continue;
            }

            auto style = parseValue<Gfx::BorderStyle>(c);
            if (style) {
                value.style = style.unwrap();
                continue;
            }

            break;
        }

        return Ok();
    }
};

// https://www.w3.org/TR/css-backgrounds-3/#border-shorthands
struct BorderRightProp {
    Border value;

    static constexpr Str name() { return "border-right"; }

    void apply(ComputedStyle& c) const {
        c.borders.cow().end = value;
    }

    static Border load(ComputedStyle const& c) {
        return c.borders->end;
    }

    Res<> parse(Cursor<Css::Sst>& c) {
        while (not c.ended()) {
            auto width = parseValue<CalcValue<Length>>(c);
            if (width) {
                value.width = width.unwrap();
                continue;
            }

            auto color = parseValue<Color>(c);
            if (color) {
                value.color = color.unwrap();
                continue;
            }

            auto style = parseValue<Gfx::BorderStyle>(c);
            if (style) {
                value.style = style.unwrap();
                continue;
            }

            break;
        }

        return Ok();
    }
};

// https://www.w3.org/TR/css-backgrounds-3/#border-shorthands
struct BorderBottomProp {
    Border value;

    static constexpr Str name() { return "border-bottom"; }

    void apply(ComputedStyle& c) const {
        c.borders.cow().bottom = value;
    }

    static Border load(ComputedStyle const& c) {
        return c.borders->bottom;
    }

    Res<> parse(Cursor<Css::Sst>& c) {
        while (not c.ended()) {
            auto width = parseValue<CalcValue<Length>>(c);
            if (width) {
                value.width = width.unwrap();
                continue;
            }

            auto color = parseValue<Color>(c);
            if (color) {
                value.color = color.unwrap();
                continue;
            }

            auto style = parseValue<Gfx::BorderStyle>(c);
            if (style) {
                value.style = style.unwrap();
                continue;
            }

            break;
        }

        return Ok();
    }
};

// https://www.w3.org/TR/css-backgrounds-3/#border-shorthands
struct BorderLeftProp {
    Border value;

    static constexpr Str name() { return "border-left"; }

    void apply(ComputedStyle& c) const {
        c.borders.cow().start = value;
    }

    static Border load(ComputedStyle const& c) {
        return c.borders->start;
    }

    Res<> parse(Cursor<Css::Sst>& c) {
        while (not c.ended()) {
            auto width = parseValue<CalcValue<Length>>(c);
            if (width) {
                value.width = width.unwrap();
                continue;
            }

            auto color = parseValue<Color>(c);
            if (color) {
                value.color = color.unwrap();
                continue;
            }

            auto style = parseValue<Gfx::BorderStyle>(c);
            if (style) {
                value.style = style.unwrap();
                continue;
            }

            break;
        }

        return Ok();
    }
};

// https://www.w3.org/TR/css-backgrounds-3/#border-shorthands
struct BorderProp {
    Border value;

    static constexpr Str name() { return "border"; }

    void apply(ComputedStyle& c) const {
        c.borders.cow().top = value;
        c.borders.cow().bottom = value;
        c.borders.cow().start = value;
        c.borders.cow().end = value;
    }

    static Border load(ComputedStyle const& c) {
        return c.borders->top;
    }

    Res<> parse(Cursor<Css::Sst>& c) {
        value = try$(parseValue<Border>(c));
        return Ok();
    }
};

// https://www.w3.org/TR/css-backgrounds-3/#border-width
struct BorderWidthProp {
    Math::Insets<LineWidth> value = LineWidth{Keywords::MEDIUM};

    static constexpr Str name() { return "border-width"; }

    void apply(ComputedStyle& c) const {
        auto& borders = c.borders.cow();
        borders.start.width = value.start;
        borders.end.width = value.end;
        borders.top.width = value.top;
        borders.bottom.width = value.bottom;
    }

    static Math::Insets<LineWidth> load(ComputedStyle const& c) {
        return {
            c.borders->start.width,
            c.borders->end.width,
            c.borders->top.width,
            c.borders->bottom.width,
        };
    }

    Res<> parse(Cursor<Css::Sst>& c) {
        value = try$(parseValue<Math::Insets<LineWidth>>(c));

        return Ok();
    }
};

// MARK: Content ---------------------------------------------------------------

// https://drafts.csswg.org/css-content/#content-property
struct ContentProp {
    String value = initial();

    static constexpr Str name() { return "content"; }

    static String initial() { return ""s; }

    void apply(ComputedStyle& c) const {
        c.content = value;
    }

    static String load(ComputedStyle const& c) {
        return c.content;
    }

    Res<> parse(Cursor<Css::Sst>& c) {
        value = try$(parseValue<String>(c));
        return Ok();
    }
};

// MARK: Clip Path -------------------------------------------------------------

// https://drafts.fxtf.org/css-masking/#the-clip-path
struct ClipPathProp {
    using Value = Union</* Url, */ BasicShape, Keywords::None>;
    Value value = initial();

    static constexpr Str name() { return "clip-path"; }

    static Keywords::None initial() { return Keywords::NONE; }

    void apply(ComputedStyle& c) const {
        if (auto clipShape = value.is<BasicShape>())
            c.clip = *clipShape;
        else
            c.clip = NONE;
    }

    static Value load(ComputedStyle const& c) {
        if (c.clip.has())
            return c.clip.unwrap();
        return Keywords::NONE;
    }

    Res<> parse(Cursor<Css::Sst>& c) {
        value = try$(parseValue<Value>(c));
        return Ok();
    }
};

// MARK: Borders - Table -------------------------------------------------------

// https://www.w3.org/TR/CSS22/tables.html#propdef-border-collapse
struct BorderCollapseProp {
    BorderCollapse value = initial();

    static constexpr Str name() { return "border-collapse"; }

    static constexpr BorderCollapse initial() { return BorderCollapse::SEPARATE; }

    void apply(ComputedStyle& c) const {
        c.table.cow().collapse = value;
    }

    static BorderCollapse load(ComputedStyle const& c) {
        return c.table->collapse;
    }

    Res<> parse(Cursor<Css::Sst>& c) {
        value = try$(parseValue<BorderCollapse>(c));
        return Ok();
    }
};

// https://www.w3.org/TR/CSS22/tables.html#propdef-border-spacing
struct BorderSpacingProp {
    BorderSpacing value = initial();

    static constexpr Str name() { return "border-spacing"; }

    static constexpr BorderSpacing initial() { return {0_au, 0_au}; }

    void apply(ComputedStyle& c) const {
        c.table.cow().spacing = value;
    }

    static BorderSpacing load(ComputedStyle const& c) {
        return c.table->spacing;
    }

    Res<> parse(Cursor<Css::Sst>& c) {
        value = try$(parseValue<BorderSpacing>(c));
        return Ok();
    }
};

// MARK: Breaks ----------------------------------------------------------------

// https://www.w3.org/TR/css-break-3/#propdef-break-after
struct BreakAfterProp {
    BreakBetween value = initial();

    static constexpr Str name() { return "break-after"; }

    static constexpr BreakBetween initial() { return BreakBetween::AUTO; }

    void apply(ComputedStyle& c) const {
        c.break_.cow().after = value;
    }

    Res<> parse(Cursor<Css::Sst>& c) {
        value = try$(parseValue<BreakBetween>(c));
        return Ok();
    }
};

// https://www.w3.org/TR/css-break-3/#propdef-break-before
struct BreakBeforeProp {
    BreakBetween value = initial();

    static constexpr Str name() { return "break-before"; }

    static constexpr BreakBetween initial() { return BreakBetween::AUTO; }

    void apply(ComputedStyle& c) const {
        c.break_.cow().before = value;
    }

    Res<> parse(Cursor<Css::Sst>& c) {
        value = try$(parseValue<BreakBetween>(c));
        return Ok();
    }
};

// https://www.w3.org/TR/css-break-3/#break-within
struct BreakInsideProp {
    BreakInside value = initial();

    static constexpr Str name() { return "break-inside"; }

    static constexpr BreakInside initial() { return BreakInside::AUTO; }

    void apply(ComputedStyle& c) const {
        c.break_.cow().inside = value;
    }

    Res<> parse(Cursor<Css::Sst>& c) {
        value = try$(parseValue<BreakInside>(c));
        return Ok();
    }
};

// MARK: Flex ------------------------------------------------------------------

// https://www.w3.org/TR/css-flexbox-1/#flex-basis-property
struct FlexBasisProp {
    FlexBasis value = initial();

    static constexpr Str name() { return "flex-basis"; }

    static FlexBasis initial() { return Keywords::AUTO; }

    void apply(ComputedStyle& c) const {
        c.flex.cow().basis = value;
    }

    static FlexBasis load(ComputedStyle const& c) {
        return c.flex->basis;
    }

    Res<> parse(Cursor<Css::Sst>& c) {
        value = try$(parseValue<FlexBasis>(c));
        return Ok();
    }
};

// https://www.w3.org/TR/css-flexbox-1/#propdef-flex-direction
struct FlexDirectionProp {
    FlexDirection value = initial();

    static constexpr Str name() { return "flex-direction"; }

    static constexpr FlexDirection initial() { return FlexDirection::ROW; }

    void apply(ComputedStyle& c) const {
        c.flex.cow().direction = value;
    }

    static FlexDirection load(ComputedStyle const& c) {
        return c.flex->direction;
    }

    Res<> parse(Cursor<Css::Sst>& c) {
        value = try$(parseValue<FlexDirection>(c));
        return Ok();
    }
};

// https://www.w3.org/TR/css-flexbox-1/#flex-grow-property
struct FlexGrowProp {
    Number value = initial();

    static constexpr Str name() { return "flex-grow"; }

    static constexpr f64 initial() { return 0; }

    void apply(ComputedStyle& c) const {
        c.flex.cow().grow = value;
    }

    static Number load(ComputedStyle const& c) {
        return c.flex->grow;
    }

    Res<> parse(Cursor<Css::Sst>& c) {
        value = try$(parseValue<Number>(c));
        return Ok();
    }
};

// https://www.w3.org/TR/css-flexbox-1/#propdef-flex-shrink
struct FlexShrinkProp {
    Number value = initial();

    static constexpr Str name() { return "flex-shrink"; }

    static constexpr Number initial() { return 1; }

    void apply(ComputedStyle& c) const {
        c.flex.cow().shrink = value;
    }

    static Number load(ComputedStyle const& c) {
        return c.flex->shrink;
    }

    Res<> parse(Cursor<Css::Sst>& c) {
        value = try$(parseValue<Number>(c));
        return Ok();
    }
};

// https://www.w3.org/TR/css-flexbox-1/#propdef-flex-wrap
struct FlexWrapProp {
    FlexWrap value = initial();

    static constexpr Str name() { return "flex-wrap"; }

    static constexpr FlexWrap initial() { return FlexWrap::NOWRAP; }

    void apply(ComputedStyle& c) const {
        c.flex.cow().wrap = value;
    }

    static FlexWrap load(ComputedStyle const& c) {
        return c.flex->wrap;
    }

    Res<> parse(Cursor<Css::Sst>& c) {
        value = try$(parseValue<FlexWrap>(c));
        return Ok();
    }
};

// https://www.w3.org/TR/css-flexbox-1/#propdef-flex-flow
struct FlexFlowProp {
    Tuple<FlexDirection, FlexWrap> value = initial();

    static Tuple<FlexDirection, FlexWrap> initial() {
        return {
            FlexDirection::ROW,
            FlexWrap::NOWRAP,
        };
    }

    static constexpr Str name() { return "flex-flow"; }

    void apply(ComputedStyle& c) const {
        c.flex.cow().direction = value.v0;
        c.flex.cow().wrap = value.v1;
    }

    static Tuple<FlexDirection, FlexWrap> load(ComputedStyle const& c) {
        return {
            c.flex->direction,
            c.flex->wrap,
        };
    }

    Res<> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        auto direction = parseValue<FlexDirection>(c);
        if (direction) {
            value.v0 = direction.unwrap();

            auto wrap = parseValue<FlexWrap>(c);
            if (wrap)
                value.v1 = wrap.unwrap();
        } else {
            auto wrap = parseValue<FlexWrap>(c);
            if (not wrap)
                return Error::invalidData("expected flex direction or wrap");
            value.v1 = wrap.unwrap();

            direction = parseValue<FlexDirection>(c);
            if (direction)
                value.v0 = direction.unwrap();
        }

        return Ok();
    }
};

// https://www.w3.org/TR/css-flexbox-1/#propdef-flex
struct FlexProp {
    FlexItemProps value = initial();

    static constexpr Str name() { return "flex"; }

    static FlexItemProps initial() {
        return {
            Keywords::AUTO,
            0,
            1,
        };
    }

    void apply(ComputedStyle& c) const {
        auto& flex = c.flex.cow();
        flex.basis = value.flexBasis;
        flex.grow = value.flexGrow;
        flex.shrink = value.flexShrink;
    }

    static FlexItemProps load(ComputedStyle const& c) {
        return {
            c.flex->basis,
            c.flex->grow,
            c.flex->shrink,
        };
    }

    Res<> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c.skip(Css::Token::ident("none"))) {
            value = {
                Keywords::AUTO,
                0,
                0,
            };
            return Ok();
        } else if (c.skip(Css::Token::ident("initial"))) {
            value = {
                Keywords::AUTO,
                0,
                1,
            };
            return Ok();
        }

        // deafult values if these parameters are omitted
        value.flexGrow = value.flexShrink = 1;
        value.flexBasis = CalcValue<PercentOr<Length>>(Length{});

        auto parseGrowShrink = [](Cursor<Css::Sst>& c, FlexItemProps& value) -> Res<> {
            auto grow = parseValue<Number>(c);
            if (not grow)
                return Error::invalidData("expected flex item grow");

            value.flexGrow = grow.unwrap();

            auto shrink = parseValue<Number>(c);
            if (shrink)
                value.flexShrink = shrink.unwrap();

            return Ok();
        };

        auto parsedGrowAndMaybeShrink = parseGrowShrink(c, value);
        if (parsedGrowAndMaybeShrink) {
            auto basis = parseValue<FlexBasis>(c);
            if (basis)
                value.flexBasis = basis.unwrap();
        } else {
            auto basis = parseValue<FlexBasis>(c);
            if (basis)
                value.flexBasis = basis.unwrap();
            else
                return Error::invalidData("expected flex item grow or basis");

            auto parsedGrowAndMaybeShrink = parseGrowShrink(c, value);
        }
        return Ok();
    }
};

// MARK: Float & Clear ---------------------------------------------------------

struct FloatProp {
    Float value = initial();

    static constexpr Str name() { return "float"; }

    static Float initial() { return Float::NONE; }

    void apply(ComputedStyle& c) const {
        c.float_ = value;
    }

    static Float load(ComputedStyle const& c) {
        return c.float_;
    }

    Res<> parse(Cursor<Css::Sst>& c) {
        value = try$(parseValue<Float>(c));
        return Ok();
    }
};

struct ClearProp {
    Clear value = initial();

    static constexpr Str name() { return "clear"; }

    static Clear initial() { return Clear::NONE; }

    void apply(ComputedStyle& c) const {
        c.clear = value;
    }

    static Clear load(ComputedStyle const& c) {
        return c.clear;
    }

    Res<> parse(Cursor<Css::Sst>& c) {
        value = try$(parseValue<Clear>(c));
        return Ok();
    }
};

// MARK: Fonts -----------------------------------------------------------------

// https://www.w3.org/TR/css-fonts-4/#font-family-prop
struct FontFamilyProp {
    Vec<Text::Family> value = initial();

    static constexpr Str name() { return "font-family"; }

    static Array<Text::Family, 1> initial() { return {Text::GenericFamily::SANS_SERIF}; }

    static void inherit(ComputedStyle const& parent, ComputedStyle& child) {
        if (not child.font.sameInstance(parent.font))
            child.font.cow().families = parent.font->families;
    }

    void apply(ComputedStyle& c) const {
        c.font.cow().families = value;
    }

    static Vec<Text::Family> load(ComputedStyle const& c) {
        return c.font->families;
    }

    Res<> parse(Cursor<Css::Sst>& c) {
        value = {};
        eatWhitespace(c);
        while (not c.ended()) {
            value.pushBack(try$(parseValue<Text::Family>(c)));

            eatWhitespace(c);
            c.skip(Css::Token::comma());
            eatWhitespace(c);
        }
        return Ok();
    }
};

// https://www.w3.org/TR/css-fonts-4/#font-weight-prop
struct FontWeightProp {
    FontWeight value = initial();

    static constexpr Str name() { return "font-weight"; }

    static FontWeight initial() { return Text::FontWeight::REGULAR; }

    static void inherit(ComputedStyle const& parent, ComputedStyle& child) {
        if (not child.font.sameInstance(parent.font))
            child.font.cow().weight = parent.font->weight;
    }

    void apply(ComputedStyle& child) const {
        child.font.cow().weight = value.resolve();
    }

    void apply(ComputedStyle const& parent, ComputedStyle& child) const {
        child.font.cow().weight = value.resolve(parent.font->weight);
    }

    static FontWeight load(ComputedStyle const& c) {
        return c.font->weight;
    }

    Res<> parse(Cursor<Css::Sst>& c) {
        value = try$(parseValue<FontWeight>(c));
        return Ok();
    }
};

// https://www.w3.org/TR/css-fonts-4/#font-width-prop
struct FontWidthProp {
    FontWidth value = initial();

    static constexpr Str name() { return "font-width"; }

    static constexpr FontWidth initial() { return FontWidth::NORMAL; }

    static void inherit(ComputedStyle const& parent, ComputedStyle& child) {
        if (not child.font.sameInstance(parent.font))
            child.font.cow().width = parent.font->width;
    }

    void apply(ComputedStyle& c) const {
        c.font.cow().width = value;
    }

    static FontWidth load(ComputedStyle const& c) {
        return c.font->width;
    }

    Res<> parse(Cursor<Css::Sst>& c) {
        value = try$(parseValue<FontWidth>(c));
        return Ok();
    }
};

// https://www.w3.org/TR/css-fonts-4/#font-style-prop
struct FontStyleProp {
    FontStyle value = initial();

    static constexpr Str name() { return "font-style"; }

    static constexpr FontStyle initial() { return FontStyle::NORMAL; }

    static void inherit(ComputedStyle const& parent, ComputedStyle& child) {
        if (not child.font.sameInstance(parent.font))
            child.font.cow().style = parent.font->style;
    }

    void apply(ComputedStyle& c) const {
        c.font.cow().style = value;
    }

    static FontStyle load(ComputedStyle const& c) {
        return c.font->style;
    }

    Res<> parse(Cursor<Css::Sst>& c) {
        value = try$(parseValue<FontStyle>(c));
        return Ok();
    }
};

// https://www.w3.org/TR/css-fonts-4/#font-prop
struct FontProp {
    FontProps value;
    Opt<FontWeight> unresolvedWeight;

    static constexpr Str name() { return "font"; }

    static void inherit(ComputedStyle const& parent, ComputedStyle& child) {
        child.font.cow() = *parent.font;
    }

    void apply(ComputedStyle& c) const {
        c.font.cow() = value;
        if (unresolvedWeight)
            c.font.cow().weight = unresolvedWeight->resolve();
    }

    void apply(ComputedStyle const& parent, ComputedStyle& child) const {
        child.font.cow() = value;
        if (unresolvedWeight)
            child.font.cow().weight = unresolvedWeight->resolve(parent.font->weight);
    }

    Res<> parse(Cursor<Css::Sst>& c) {
        // TODO: system family name

        while (true) {
            auto fontStyle = parseValue<FontStyle>(c);
            if (fontStyle) {
                value.style = fontStyle.unwrap();
                continue;
            }

            auto fontWeight = parseValue<FontWeight>(c);
            if (fontWeight) {
                unresolvedWeight = fontWeight.unwrap();
                continue;
            }

            // TODO: font variant https://www.w3.org/TR/css-fonts-4/#font-variant-css21-values

            auto fontWidth = parseValue<FontWidth>(c);
            if (fontWidth) {
                value.width = fontWidth.unwrap();
                continue;
            }

            auto fontSize = parseValue<FontSize>(c);
            if (fontSize) {
                value.size = fontSize.unwrap();
                break;
            }

            return Error::invalidData("expected font-style, font-weight, font-width or font-size");
        }

        if (c.skip(Css::Token::delim("/"))) {
            auto lh = Ok(parseValue<LineHeight>(c));
            // TODO: use lineheight parsed value
        }

        value.families = {try$(parseValue<Text::Family>(c))};

        return Ok();
    }
};

// https://www.w3.org/TR/css-fonts-4/#font-size-prop
struct FontSizeProp {
    FontSize value = initial();

    static constexpr Str name() { return "font-size"; }

    static constexpr FontSize initial() { return FontSize::MEDIUM; }

    static void inherit(ComputedStyle const& parent, ComputedStyle& child) {
        if (not child.font.sameInstance(parent.font))
            child.font.cow().size = parent.font->size;
    }

    void apply(ComputedStyle& c) const {
        c.font.cow().size = value;
    }

    static FontSize load(ComputedStyle const& c) {
        return c.font->size;
    }

    Res<> parse(Cursor<Css::Sst>& c) {
        value = try$(parseValue<FontSize>(c));
        return Ok();
    }
};

// MARK: Line ------------------------------------------------------------------

struct LineHeightProp {
    LineHeight value = initial();

    static constexpr Str name() { return "line-height"; }

    static LineHeight initial() { return LineHeight::NORMAL; }

    void apply(ComputedStyle&) const {
        // TODO
    }

    static LineHeight load(ComputedStyle const&) {
        return initial(); // TODO
    }

    Res<> parse(Cursor<Css::Sst>& c) {
        value = try$(parseValue<LineHeight>(c));
        return Ok();
    }
};

// MARK: Margin ----------------------------------------------------------------

// https://www.w3.org/TR/css-box-3/#propdef-margin

struct MarginTopProp {
    Width value = initial();

    static Str name() { return "margin-top"; }

    static Width initial() { return CalcValue<PercentOr<Length>>(Length{}); }

    void apply(ComputedStyle& c) const {
        c.margin.cow().top = value;
    }

    static Width load(ComputedStyle const& c) {
        return c.margin->top;
    }

    Res<> parse(Cursor<Css::Sst>& c) {
        value = try$(parseValue<Width>(c));
        return Ok();
    }
};

struct MarginRightProp {
    Width value = initial();

    static Str name() { return "margin-right"; }

    static Width initial() { return CalcValue<PercentOr<Length>>(Length{}); }

    void apply(ComputedStyle& c) const {
        c.margin.cow().end = value;
    }

    static Width load(ComputedStyle const& c) {
        return c.margin->end;
    }

    Res<> parse(Cursor<Css::Sst>& c) {
        value = try$(parseValue<Width>(c));
        return Ok();
    }
};

struct MarginBottomProp {
    Width value = initial();

    static constexpr Str name() { return "margin-bottom"; }

    static Width initial() { return CalcValue<PercentOr<Length>>(Length{}); }

    void apply(ComputedStyle& c) const {
        c.margin.cow().bottom = value;
    }

    static Width load(ComputedStyle const& c) {
        return c.margin->bottom;
    }

    Res<> parse(Cursor<Css::Sst>& c) {
        value = try$(parseValue<Width>(c));
        return Ok();
    }
};

struct MarginLeftProp {
    Width value = initial();

    static Str name() { return "margin-left"; }

    static Width initial() { return CalcValue<PercentOr<Length>>(Length{}); }

    void apply(ComputedStyle& c) const {
        c.margin.cow().start = value;
    }

    static Width load(ComputedStyle const& c) {
        return c.margin->start;
    }

    Res<> parse(Cursor<Css::Sst>& c) {
        value = try$(parseValue<Width>(c));
        return Ok();
    }
};

struct MarginProp {
    Math::Insets<Width> value = initial();

    static Str name() { return "margin"; }

    static Math::Insets<Width> initial() { return {CalcValue<PercentOr<Length>>(Length{})}; }

    void apply(ComputedStyle& c) const {
        c.margin.cow() = value;
    }

    static Math::Insets<Width> load(ComputedStyle const& c) {
        return c.margin->start;
    }

    Res<> parse(Cursor<Css::Sst>& c) {
        value = try$(parseValue<Math::Insets<Width>>(c));
        return Ok();
    }
};

// https://drafts.csswg.org/css-logical/#margin-properties

struct MarginInlineStartProp {
    Width value = initial();

    static Str name() { return "margin-inline-start"; }

    static Width initial() { return CalcValue<PercentOr<Length>>(Length{}); }

    void apply(ComputedStyle& c) const {
        // FIXME: Take writing mode into account
        c.margin.cow().start = value;
    }

    static Width load(ComputedStyle const& c) {
        return c.margin->start;
    }

    Res<> parse(Cursor<Css::Sst>& c) {
        value = try$(parseValue<Width>(c));
        return Ok();
    }
};

struct MarginInlineEndProp {
    Width value = initial();

    static Str name() { return "margin-inline-end"; }

    static Width initial() { return CalcValue<PercentOr<Length>>(Length{}); }

    void apply(ComputedStyle& c) const {
        // FIXME: Take writing mode into account
        c.margin.cow().end = value;
    }

    static Width load(ComputedStyle const& c) {
        return c.margin->end;
    }

    Res<> parse(Cursor<Css::Sst>& c) {
        value = try$(parseValue<Width>(c));
        return Ok();
    }
};

struct MarginInlineProp {
    Math::Insets<Width> value = initial();

    static Str name() { return "margin-inline"; }

    static Math::Insets<Width> initial() { return {CalcValue<PercentOr<Length>>(Length{})}; }

    void apply(ComputedStyle& c) const {
        // FIXME: Take writing mode into account
        c.margin.cow().start = value.start;
        c.margin.cow().end = value.end;
    }

    static Math::Insets<Width> load(ComputedStyle const& c) {
        return {
            c.margin->start,
            c.margin->end,
        };
    }

    Res<> parse(Cursor<Css::Sst>& c) {
        value = try$(parseValue<Math::Insets<Width>>(c));
        return Ok();
    }
};

struct MarginBlockStartProp {
    Width value = initial();

    static Str name() { return "margin-block-start"; }

    static Width initial() { return CalcValue<PercentOr<Length>>(Length{}); }

    void apply(ComputedStyle& c) const {
        // FIXME: Take writing mode into account
        c.margin.cow().top = value;
    }

    static Width load(ComputedStyle const& c) {
        return c.margin->top;
    }

    Res<> parse(Cursor<Css::Sst>& c) {
        value = try$(parseValue<Width>(c));
        return Ok();
    }
};

struct MarginBlockEndProp {
    Width value = initial();

    static Str name() { return "margin-block-end"; }

    static Width initial() { return CalcValue<PercentOr<Length>>(Length{}); }

    void apply(ComputedStyle& c) const {
        // FIXME: Take writing mode into account
        c.margin.cow().bottom = value;
    }

    static Width load(ComputedStyle const& c) {
        return c.margin->bottom;
    }

    Res<> parse(Cursor<Css::Sst>& c) {
        value = try$(parseValue<Width>(c));
        return Ok();
    }
};

struct MarginBlockProp {
    Math::Insets<Width> value = initial();

    static Str name() { return "margin-block"; }

    static Math::Insets<Width> initial() { return {CalcValue<PercentOr<Length>>(Length{})}; }

    void apply(ComputedStyle& c) const {
        // FIXME: Take writing mode into account
        c.margin.cow().top = value.top;
        c.margin.cow().bottom = value.bottom;
    }

    static Math::Insets<Width> load(ComputedStyle const& c) {
        return {
            c.margin->top,
            c.margin->bottom,
        };
    }

    Res<> parse(Cursor<Css::Sst>& c) {
        value = try$(parseValue<Math::Insets<Width>>(c));
        return Ok();
    }
};

// https://www.w3.org/TR/css-color-4/#propdef-opacity

struct OpacityProp {
    Number value = initial();

    static Str name() { return "opacity"; }

    static f64 initial() { return 1; }

    void apply(ComputedStyle& c) const {
        c.opacity = value;
    }

    static f64 load(ComputedStyle const& c) {
        return c.opacity;
    }

    Res<> parse(Cursor<Css::Sst>& c) {
        auto maybePercent = parseValue<Percent>(c);
        if (maybePercent) {
            value = maybePercent.unwrap().value() / 100;
        } else {
            value = try$(parseValue<Number>(c));
        }
        return Ok();
    }
};

// MARK: Outline --------------------------------------------------------------

// https://drafts.csswg.org/css-ui/#outline
struct OutlineProp {
    Outline value;

    static Str name() { return "outline"; }

    void apply(ComputedStyle& c) const {
        c.outline.cow() = value;
    }

    static Outline load(ComputedStyle const& c) {
        return *c.outline;
    }

    Res<> parse(Cursor<Css::Sst>& c) {
        bool styleSet = false;
        while (not c.ended()) {
            auto width = parseValue<CalcValue<Length>>(c);
            if (width) {
                value.width = width.unwrap();
                continue;
            }

            auto color = parseValue<Color>(c);
            if (color) {
                value.color = color.unwrap();
                continue;
            }

            auto style = parseValue<Gfx::BorderStyle>(c);
            if (style) {
                value.style = style.unwrap();
                styleSet = true;
                continue;
            }

            if (c.skip(Css::Token::ident("auto"))) {
                if (not styleSet)
                    value.style = Keywords::AUTO;
                value.color = Keywords::AUTO;
                continue;
            }

            break;
        }

        return Ok();
    }
};

// https://drafts.csswg.org/css-ui/#outline-width
struct OutlineWidthProp {
    LineWidth value = initial();

    static Str name() { return "outline-width"; }

    static LineWidth initial() { return Keywords::MEDIUM; }

    void apply(ComputedStyle& c) const {
        c.outline.cow().width = value;
    }

    static LineWidth load(ComputedStyle const& c) {
        return c.outline->width;
    }

    Res<> parse(Cursor<Css::Sst>& c) {
        value = try$(parseValue<LineWidth>(c));
        return Ok();
    }
};

// https://drafts.csswg.org/css-ui/#outline-style
struct OutlineStyleProp {
    using Value = Union<Keywords::Auto, Gfx::BorderStyle>;
    Value value = initial();

    static Str name() { return "outline-style"; }

    static Gfx::BorderStyle initial() { return Gfx::BorderStyle::NONE; }

    void apply(ComputedStyle& c) const {
        c.outline.cow().style = value;
    }

    static Value load(ComputedStyle const& c) {
        return c.outline->style;
    }

    Res<> parse(Cursor<Css::Sst>& c) {
        value = try$(parseValue<Value>(c));
        return Ok();
    }
};

// https://drafts.csswg.org/css-ui/#outline-color
struct OutlineColorProp {
    using Value = Union<Keywords::Auto, Color>;
    Value value = initial();

    static Str name() { return "outline-color"; }

    static Keywords::Auto initial() { return Keywords::AUTO; }

    void apply(ComputedStyle& c) const {
        c.outline.cow().color = value;
    }

    static Value load(ComputedStyle const& c) {
        return c.outline->color;
    }

    Res<> parse(Cursor<Css::Sst>& c) {
        value = try$(parseValue<Value>(c));
        return Ok();
    }
};

// https://drafts.csswg.org/css-ui/#outline-offset
struct OutlineOffsetProp {
    CalcValue<Length> value = initial();

    static Str name() { return "outline-offset"; }

    static Length initial() { return 0_au; }

    void apply(ComputedStyle& c) const {
        c.outline.cow().offset = value;
    }

    static CalcValue<Length> load(ComputedStyle const& c) {
        return c.outline->offset;
    }

    Res<> parse(Cursor<Css::Sst>& c) {
        value = try$(parseValue<CalcValue<Length>>(c));
        return Ok();
    }
};

// MARK: Overflow --------------------------------------------------------------

// https://www.w3.org/TR/css-overflow/#overflow-control
struct OverflowXProp {
    Overflow value = initial();

    static Str name() { return "overflow-x"; }

    static Overflow initial() { return Overflow::VISIBLE; }

    void apply(ComputedStyle& c) const {
        c.overflows.x = value;
    }

    static Overflow load(ComputedStyle const& c) {
        return c.overflows.x;
    }

    Res<> parse(Cursor<Css::Sst>& c) {
        value = try$(parseValue<Overflow>(c));
        return Ok();
    }
};

// https://www.w3.org/TR/css-overflow/#overflow-control
struct OverflowYProp {
    Overflow value = initial();

    static Str name() { return "overflow-y"; }

    static Overflow initial() { return Overflow::VISIBLE; }

    void apply(ComputedStyle& c) const {
        c.overflows.y = value;
    }

    static Overflow load(ComputedStyle const& c) {
        return c.overflows.y;
    }

    Res<> parse(Cursor<Css::Sst>& c) {
        value = try$(parseValue<Overflow>(c));
        return Ok();
    }
};

// https://www.w3.org/TR/css-overflow/#overflow-block
struct OverflowBlockProp {
    Overflow value = initial();

    static Str name() { return "overflow-block"; }

    static Overflow initial() { return Overflow::VISIBLE; }

    void apply(ComputedStyle& c) const {
        c.overflows.block = value;
    }

    static Overflow load(ComputedStyle const& c) {
        return c.overflows.block;
    }

    Res<> parse(Cursor<Css::Sst>& c) {
        value = try$(parseValue<Overflow>(c));
        return Ok();
    }
};

// https://www.w3.org/TR/css-overflow/#overflow-inline
struct OverflowInlineProp {
    Overflow value = initial();

    static Str name() { return "overflow-inline"; }

    static Overflow initial() { return Overflow::VISIBLE; }

    void apply(ComputedStyle& c) const {
        c.overflows.inline_ = value;
    }

    static Overflow load(ComputedStyle const& c) {
        return c.overflows.inline_;
    }

    Res<> parse(Cursor<Css::Sst>& c) {
        value = try$(parseValue<Overflow>(c));
        return Ok();
    }
};

// https://www.w3.org/TR/css-overflow-3/#propdef-overflow
struct OverflowProp {
    Pair<Overflow> value = initial();

    static Str name() { return "overflow"; }

    static Pair<Overflow> initial() { return {Overflow::VISIBLE, Overflow::VISIBLE}; }

    void apply(ComputedStyle& c) const {
        c.overflows.x = value.v0;
        c.overflows.y = value.v1;
    }

    static Pair<Overflow> load(ComputedStyle const& c) {
        return {c.overflows.x, c.overflows.y};
    }

    Res<> parse(Cursor<Css::Sst>& c) {
        eatWhitespace(c);
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        value.v0 = try$(parseValue<Overflow>(c));

        eatWhitespace(c);
        if (c.ended()) {
            value.v1 = value.v0;
        } else {
            value.v1 = try$(parseValue<Overflow>(c));
        }

        return Ok();
    }
};

// MARK: Padding ---------------------------------------------------------------

// https://www.w3.org/TR/css-box-3/#propdef-padding

struct PaddingTopProp {
    CalcValue<PercentOr<Length>> value = initial();

    static Str name() { return "padding-top"; }

    static Length initial() { return Length{}; }

    void apply(ComputedStyle& c) const {
        c.padding.cow().top = value;
    }

    static CalcValue<PercentOr<Length>> load(ComputedStyle const& c) {
        return c.padding->top;
    }

    Res<> parse(Cursor<Css::Sst>& c) {
        value = try$(parseValue<CalcValue<PercentOr<Length>>>(c));
        return Ok();
    }
};

struct PaddingRightProp {
    CalcValue<PercentOr<Length>> value = initial();

    static Str name() { return "padding-right"; }

    static Length initial() { return Length{}; }

    void apply(ComputedStyle& c) const {
        c.padding.cow().end = value;
    }

    static CalcValue<PercentOr<Length>> load(ComputedStyle const& c) {
        return c.padding->end;
    }

    Res<> parse(Cursor<Css::Sst>& c) {
        value = try$(parseValue<CalcValue<PercentOr<Length>>>(c));
        return Ok();
    }
};

struct PaddingBottomProp {
    CalcValue<PercentOr<Length>> value = initial();

    static Str name() { return "padding-bottom"; }

    static Length initial() { return Length{}; }

    void apply(ComputedStyle& c) const {
        c.padding.cow().bottom = value;
    }

    static CalcValue<PercentOr<Length>> load(ComputedStyle const& c) {
        return c.padding->bottom;
    }

    Res<> parse(Cursor<Css::Sst>& c) {
        value = try$(parseValue<CalcValue<PercentOr<Length>>>(c));
        return Ok();
    }
};

struct PaddingLeftProp {
    CalcValue<PercentOr<Length>> value = initial();

    static Str name() { return "padding-left"; }

    static Length initial() { return {}; }

    void apply(ComputedStyle& c) const {
        c.padding.cow().start = value;
    }

    static CalcValue<PercentOr<Length>> load(ComputedStyle const& c) {
        return c.padding->start;
    }

    Res<> parse(Cursor<Css::Sst>& c) {
        value = try$(parseValue<CalcValue<PercentOr<Length>>>(c));
        return Ok();
    }
};

struct PaddingProp {
    Math::Insets<CalcValue<PercentOr<Length>>> value = initial();

    static Str name() { return "padding"; }

    static Math::Insets<CalcValue<PercentOr<Length>>> initial() { return {Length{}}; }

    void apply(ComputedStyle& c) const {
        c.padding.cow() = value;
    }

    static Math::Insets<CalcValue<PercentOr<Length>>> load(ComputedStyle const& c) {
        return *c.padding;
    }

    Res<> parse(Cursor<Css::Sst>& c) {
        value = try$(parseValue<Math::Insets<CalcValue<PercentOr<Length>>>>(c));
        return Ok();
    }
};

// https://www.w3.org/TR/css-display-3/#order-property
struct OrderProp {
    Integer value = initial();

    static Str name() { return "order"; }

    static Integer initial() { return 0; }

    void apply(ComputedStyle& c) const {
        c.order = value;
    }

    static Integer load(ComputedStyle const& c) {
        return c.order;
    }

    Res<> parse(Cursor<Css::Sst>& c) {
        value = try$(parseValue<Integer>(c));
        return Ok();
    }
};

// MARK: Positioning -----------------------------------------------------------

// https://www.w3.org/TR/CSS22/visuren.html#positioning-scheme
struct PositionProp {
    Position value = initial();

    static Str name() { return "position"; }

    static Position initial() { return Position::STATIC; }

    void apply(ComputedStyle& c) const {
        c.position = value;
    }

    static Position load(ComputedStyle const& c) {
        return c.position;
    }

    Res<> parse(Cursor<Css::Sst>& c) {
        value = try$(parseValue<Position>(c));
        return Ok();
    }
};

// https://www.w3.org/TR/CSS22/visuren.html#propdef-top
struct TopProp {
    Width value = initial();

    static Str name() { return "top"; }

    static Width initial() { return Keywords::AUTO; }

    void apply(ComputedStyle& c) const {
        c.offsets.cow().top = value;
    }

    static Width load(ComputedStyle const& c) {
        return c.offsets->top;
    }

    Res<> parse(Cursor<Css::Sst>& c) {
        value = try$(parseValue<Width>(c));
        return Ok();
    }
};

// https://www.w3.org/TR/CSS22/visuren.html#propdef-right
struct RightProp {
    Width value = initial();

    static Str name() { return "right"; }

    static Width initial() { return Keywords::AUTO; }

    void apply(ComputedStyle& c) const {
        c.offsets.cow().end = value;
    }

    static Width load(ComputedStyle const& c) {
        return c.offsets->end;
    }

    Res<> parse(Cursor<Css::Sst>& c) {
        value = try$(parseValue<Width>(c));
        return Ok();
    }
};

// https://www.w3.org/TR/CSS22/visuren.html#propdef-bottom
struct BottomProp {
    Width value = initial();

    static Str name() { return "bottom"; }

    static Width initial() { return Keywords::AUTO; }

    void apply(ComputedStyle& c) const {
        c.offsets.cow().bottom = value;
    }

    static Width load(ComputedStyle const& c) {
        return c.offsets->bottom;
    }

    Res<> parse(Cursor<Css::Sst>& c) {
        value = try$(parseValue<Width>(c));
        return Ok();
    }
};

// https://www.w3.org/TR/CSS22/visuren.html#propdef-left
struct LeftProp {
    Width value = initial();

    static Str name() { return "left"; }

    static Width initial() { return Keywords::AUTO; }

    void apply(ComputedStyle& c) const {
        c.offsets.cow().start = value;
    }

    static Width load(ComputedStyle const& c) {
        return c.offsets->start;
    }

    Res<> parse(Cursor<Css::Sst>& c) {
        value = try$(parseValue<Width>(c));
        return Ok();
    }
};

// MARK: Sizing ----------------------------------------------------------------
// https://www.w3.org/TR/css-sizing-3

// https://www.w3.org/TR/css-sizing-3/#box-sizing
struct BoxSizingProp {
    BoxSizing value = initial();

    static constexpr Str name() { return "box-sizing"; }

    static constexpr BoxSizing initial() { return BoxSizing::CONTENT_BOX; }

    void apply(ComputedStyle& c) const {
        c.boxSizing = value;
    }

    static BoxSizing load(ComputedStyle const& c) {
        return c.boxSizing;
    }

    Res<> parse(Cursor<Css::Sst>& c) {
        if (c.skip(Css::Token::ident("border-box")))
            value = BoxSizing::BORDER_BOX;
        else if (c.skip(Css::Token::ident("content-box")))
            value = BoxSizing::CONTENT_BOX;
        else
            return Error::invalidData("expected 'border-box' or 'content-box'");

        return Ok();
    }
};

// https://www.w3.org/TR/css-sizing-3/#propdef-width

struct WidthProp {
    Size value = initial();

    static constexpr Str name() { return "width"; }

    static Size initial() { return Keywords::AUTO; }

    void apply(ComputedStyle& c) const {
        c.sizing.cow().width = value;
    }

    static Size load(ComputedStyle const& c) {
        return c.sizing->width;
    }

    Res<> parse(Cursor<Css::Sst>& c) {
        value = try$(parseValue<Size>(c));
        return Ok();
    }
};

// https://www.w3.org/TR/css-sizing-3/#propdef-height

struct HeightProp {
    Size value = initial();

    static constexpr Str name() { return "height"; }

    static Size initial() { return Keywords::AUTO; }

    void apply(ComputedStyle& c) const {
        c.sizing.cow().height = value;
    }

    static Size load(ComputedStyle const& c) {
        return c.sizing->height;
    }

    Res<> parse(Cursor<Css::Sst>& c) {
        value = try$(parseValue<Size>(c));
        return Ok();
    }
};

// https://www.w3.org/TR/css-sizing-3/#propdef-min-width

struct MinWidthProp {
    Size value = initial();

    static constexpr Str name() { return "min-width"; }

    static Size initial() { return Keywords::AUTO; }

    void apply(ComputedStyle& c) const {
        c.sizing.cow().minWidth = value;
    }

    static Size load(ComputedStyle const& c) {
        return c.sizing->minWidth;
    }

    Res<> parse(Cursor<Css::Sst>& c) {
        value = try$(parseValue<Size>(c));
        return Ok();
    }
};

// https://www.w3.org/TR/css-sizing-3/#propdef-min-height

struct MinHeightProp {
    Size value = initial();

    static constexpr Str name() { return "min-height"; }

    static Size initial() { return Keywords::AUTO; }

    void apply(ComputedStyle& c) const {
        c.sizing.cow().minHeight = value;
    }

    static Size load(ComputedStyle const& c) {
        return c.sizing->minHeight;
    }

    Res<> parse(Cursor<Css::Sst>& c) {
        value = try$(parseValue<Size>(c));
        return Ok();
    }
};

// https://www.w3.org/TR/css-sizing-3/#propdef-max-width

struct MaxWidthProp {
    MaxSize value = initial();

    static constexpr Str name() { return "max-width"; }

    static MaxSize initial() { return Keywords::NONE; }

    void apply(ComputedStyle& c) const {
        c.sizing.cow().maxWidth = value;
    }

    static MaxSize load(ComputedStyle const& c) {
        return c.sizing->maxWidth;
    }

    Res<> parse(Cursor<Css::Sst>& c) {
        value = try$(parseValue<MaxSize>(c));
        return Ok();
    }
};

// https://www.w3.org/TR/css-sizing-3/#propdef-max-height

struct MaxHeightProp {
    MaxSize value = initial();

    static constexpr Str name() { return "max-height"; }

    static MaxSize initial() { return Keywords::NONE; }

    void apply(ComputedStyle& c) const {
        c.sizing.cow().maxHeight = value;
    }

    static MaxSize load(ComputedStyle const& c) {
        return c.sizing->maxHeight;
    }

    Res<> parse(Cursor<Css::Sst>& c) {
        value = try$(parseValue<MaxSize>(c));
        return Ok();
    }
};

// MARK: Text
// https://drafts.csswg.org/css-text-4

// https://drafts.csswg.org/css-text/#text-align-property

struct TextAlignProp {
    TextAlign value = initial();

    static constexpr Str name() { return "text-align"; }

    static TextAlign initial() { return TextAlign::LEFT; }

    void apply(ComputedStyle& c) const {
        c.text.cow().align = value;
    }

    static TextAlign load(ComputedStyle const& c) {
        return c.text->align;
    }

    Res<> parse(Cursor<Css::Sst>& c) {
        if (c.skip(Css::Token::ident("left"))) {
            value = TextAlign::LEFT;
        } else if (c.skip(Css::Token::ident("right"))) {
            value = TextAlign::RIGHT;
        } else if (c.skip(Css::Token::ident("center"))) {
            value = TextAlign::CENTER;
        } else if (c.skip(Css::Token::ident("justify"))) {
            value = TextAlign::JUSTIFY;
        } else if (c.skip(Css::Token::ident("start"))) {
            value = TextAlign::START;
        } else if (c.skip(Css::Token::ident("end"))) {
            value = TextAlign::END;
        } else {
            return Error::invalidData("expected text-align");
        }
        return Ok();
    }
};

// https://drafts.csswg.org/css-text-4/#text-transform-property

struct TextTransformProp {
    TextTransform value = initial();

    static constexpr Str name() { return "text-transform"; }

    static TextTransform initial() { return TextTransform::NONE; }

    void apply(ComputedStyle& c) const {
        c.text.cow().transform = value;
    }

    static TextTransform load(ComputedStyle const& c) {
        return c.text->transform;
    }

    Res<> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c.skip(Css::Token::ident("none"))) {
            value = TextTransform::NONE;
        } else if (c.skip(Css::Token::ident("uppercase"))) {
            value = TextTransform::UPPERCASE;
        } else if (c.skip(Css::Token::ident("lowsercase"))) {
            value = TextTransform::LOWERCASE;
        } else {
            return Error::invalidData("expected text-transform");
        }

        return Ok();
    }
};

// https://drafts.csswg.org/css-display/#visibility
struct VisibilityProp {
    Visibility value = initial();

    static constexpr Str name() { return "visibility"; }

    static Visibility initial() { return Visibility::VISIBLE; }

    void apply(ComputedStyle& c) const {
        c.visibility = value;
    }

    static void inherit(ComputedStyle const& parent, ComputedStyle& child) {
        child.visibility = parent.visibility;
    }

    Res<> parse(Cursor<Css::Sst>& c) {
        if (c.skip(Css::Token::ident("visible"))) {
            value = Visibility::VISIBLE;
        } else if (c.skip(Css::Token::ident("hidden"))) {
            value = Visibility::HIDDEN;
        } else if (c.skip(Css::Token::ident("collapse"))) {
            value = Visibility::COLLAPSE;
        } else {
            return Error::invalidData("expected visibility");
        }

        return Ok();
    }
};

// https://drafts.csswg.org/css-text/#white-space-property

struct WhiteSpaceProp {
    WhiteSpace value = initial();

    static constexpr Str name() { return "white-space"; }

    static WhiteSpace initial() { return WhiteSpace::NORMAL; }

    void apply(ComputedStyle& c) const {
        c.text.cow().whiteSpace = value;
    }

    static WhiteSpace load(ComputedStyle const& c) {
        return c.text->whiteSpace;
    }

    Res<> parse(Cursor<Css::Sst>& c) {
        if (c.skip(Css::Token::ident("normal"))) {
            value = WhiteSpace::NORMAL;
        } else if (c.skip(Css::Token::ident("nowrap"))) {
            value = WhiteSpace::NOWRAP;
        } else if (c.skip(Css::Token::ident("pre"))) {
            value = WhiteSpace::PRE;
        } else if (c.skip(Css::Token::ident("pre-wrap"))) {
            value = WhiteSpace::PRE_WRAP;
        } else if (c.skip(Css::Token::ident("pre-line"))) {
            value = WhiteSpace::PRE_LINE;
        } else if (c.skip(Css::Token::ident("break-spaces"))) {
            value = WhiteSpace::BREAK_SPACES;
        } else {
            return Error::invalidData("expected white-space");
        }

        return Ok();
    }
};

// https://drafts.csswg.org/css2/#z-index

struct ZIndexProp {
    ZIndex value = initial();

    static constexpr Str name() { return "z-index"; }

    static constexpr ZIndex initial() { return Keywords::AUTO; }

    void apply(ComputedStyle& c) const {
        c.zIndex = value;
    }

    static ZIndex load(ComputedStyle const& c) {
        return c.zIndex;
    }

    Res<> parse(Cursor<Css::Sst>& c) {
        value = try$(parseValue<ZIndex>(c));
        return Ok();
    }
};

// MARK: OTHER -----------------------------------------------------------------
// These are no specs or behave differently than the others, you can find more details for each one in the comments.

// https://drafts.csswg.org/css-variables/#defining-variables
// this symbolizes a custom property, it starts with `--` and can be used to store a value that can be reused in the stylesheet
struct CustomProp {
    String varName;
    Css::Content value;

    CustomProp(String varName, Css::Content value)
        : varName(varName), value(value) {
    }

    static constexpr Str name() { return "custom prop"; }

    void apply(ComputedStyle& c) const {
        c.setCustomProp(varName, value);
    }

    void repr(Io::Emit& e) const {
        e("(var {#} = {})", varName, value);
    }
};

// NOTE: A property that could not be parsed, it's used to store the value
//       as-is and apply it with the cascade and custom properties
struct DeferredProp {
    String propName;
    Css::Content value;

    static constexpr Str name() { return "deferred prop"; }

    static bool _expandVariable(Cursor<Css::Sst>& c, Map<String, Css::Content> const& env, Css::Content& out);

    static bool _expandFunction(Cursor<Css::Sst>& c, Map<String, Css::Content> const& env, Css::Content& out);

    static void _expandContent(Cursor<Css::Sst>& c, Map<String, Css::Content> const& env, Css::Content& out);

    // static void inherit(Computed const &parent, Computed &child) {
    //     child.variables = parent.variables;
    // }

    void apply(ComputedStyle const& parent, ComputedStyle& c) const;

    void repr(Io::Emit& e) const {
        e("(Deffered {#} = {})", propName, value);
    }
};

enum struct Default {
    INITIAL, //< represents the value defined as the property’s initial value.
    INHERIT, //< represents the property’s computed value on the parent element.
    UNSET,   //< acts as either inherit or initial, depending on whether the property is inherited or not.
    REVERT,  //< rolls back the cascade to the cascaded value of the earlier origin.

    _LEN,
};

struct DefaultedProp {
    String propName;
    Default value;

    static constexpr Str name() { return "defaulted prop"; }

    void apply(ComputedStyle const& parent, ComputedStyle& c) const;

    void repr(Io::Emit&) const;
};

// MARK: Style Property  -------------------------------------------------------

using _StyleProp = Union<
    // Align
    AlignContentProp,
    JustifyContentProp,
    JustifySelfProp,
    AlignSelfProp,
    JustifyItemsProp,
    AlignItemsProp,

    RowGapProp,
    ColumnGapProp,

    // Baseline
    DominantBaselineProp,
    BaselineSourceProp,
    AlignmentBaselineProp,

    // Background
    BackgroundAttachmentProp,
    BackgroundColorProp,
    BackgroundImageProp,
    BackgroundPositionProp,
    BackgroundRepeatProp,
    BackgroundProp,
    ColorProp,
    DisplayProp,
    TableLayoutProp,
    CaptionSideProp,
    VisibilityProp,

    // Borders
    BorderTopColorProp,
    BorderRightColorProp,
    BorderBottomColorProp,
    BorderLeftColorProp,
    BorderColorProp,

    BorderTopWidthProp,
    BorderRightWidthProp,
    BorderBottomWidthProp,
    BorderLeftWidthProp,

    BorderStyle,
    BorderTopStyleProp,
    BorderRightStyleProp,
    BorderBottomStyleProp,
    BorderLeftStyleProp,

    BorderRadiusTopRight,
    BorderRadiusTopLeft,
    BorderRadiusBottomRight,
    BorderRadiusBottomLeft,
    BorderRadius,

    BorderTopProp,
    BorderRightProp,
    BorderBottomProp,
    BorderLeftProp,
    BorderProp,

    BorderWidthProp,

    // Borders - Table
    BorderCollapseProp,
    BorderSpacingProp,

    // Clip
    ClipPathProp,

    // Content
    ContentProp,

    // Breaks
    BreakAfterProp,
    BreakBeforeProp,
    BreakInsideProp,

    // Flex
    FlexBasisProp,
    FlexDirectionProp,
    FlexGrowProp,
    FlexShrinkProp,
    FlexWrapProp,
    FlexFlowProp,
    FlexProp,

    // Float & Clear
    FloatProp,
    ClearProp,

    // Font
    FontFamilyProp,
    FontWeightProp,
    FontWidthProp,
    FontStyleProp,
    FontSizeProp,
    FontProp,

    // Line
    LineHeightProp,

    // Margin
    MarginTopProp,
    MarginRightProp,
    MarginBottomProp,
    MarginLeftProp,
    MarginProp,

    MarginInlineStartProp,
    MarginInlineEndProp,
    MarginInlineProp,

    MarginBlockStartProp,
    MarginBlockEndProp,
    MarginBlockProp,

    // Outline
    OutlineProp,
    OutlineColorProp,
    OutlineOffsetProp,
    OutlineStyleProp,
    OutlineWidthProp,

    // Overflow
    OverflowXProp,
    OverflowYProp,
    OverflowBlockProp,
    OverflowInlineProp,
    OverflowProp,

    OpacityProp,

    // Padding
    PaddingTopProp,
    PaddingRightProp,
    PaddingBottomProp,
    PaddingLeftProp,
    PaddingProp,

    // Positioning
    PositionProp,
    TopProp,
    RightProp,
    BottomProp,
    LeftProp,

    // Sizing
    BoxSizingProp,
    WidthProp,
    HeightProp,
    MinWidthProp,
    MinHeightProp,
    MaxWidthProp,
    MaxHeightProp,

    // Text
    TextAlignProp,
    TextTransformProp,
    WhiteSpaceProp,

    // ZIndex
    ZIndexProp,

    // Other
    CustomProp,
    DeferredProp,
    DefaultedProp

    /**/
    >;

enum struct Important {
    NO,
    YES,
};

struct StyleProp : _StyleProp {
    using _StyleProp::_StyleProp;
    Important important = Important::NO;

    static constexpr Array LEGACY_ALIAS = {
        // https://drafts.csswg.org/css-align-3/#gap-legacy
        Pair<Str>{"grid-row-gap", "row-gap"},
        Pair<Str>{"grid-column-gap", "column-gap"},
        Pair<Str>{"grid-gap", "gap"},
    };

    Str name() const;

    void inherit(ComputedStyle const& parent, ComputedStyle& child) const;

    void apply(ComputedStyle const& parent, ComputedStyle& c) const;

    void repr(Io::Emit& e) const;
};

} // namespace Vaev::Style
