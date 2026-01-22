export module Vaev.Engine:props.registry;

import Karm.Core;

import :props.align;
import :props.background;
import :props.base;
import :props.baseline;
import :props.border;
import :props.breaks;
import :props.display;
import :props.flex;
import :props.floats;
import :props.fonts;
import :props.margin;
import :props.outline;
import :props.overflow;
import :props.padding;
import :props.positioned;
import :props.sizing;
import :props.svg;
import :props.table;
import :props.text;
import :props.transform;
import :props.visibility;

using namespace Karm;

namespace Vaev::Style {

export PropertyRegistry defaultRegistry() {
    PropertyRegistry registry;

    // Align
    registry.registerProperty<AlignContentProperty>();
    registry.registerProperty<JustifyContentProperty>();
    registry.registerProperty<JustifySelfProperty>();
    registry.registerProperty<AlignSelfProperty>();
    registry.registerProperty<JustifyItemsProperty>();
    registry.registerProperty<AlignItemsProperty>();

    // Gaps
    registry.registerProperty<RowGapProperty>();
    registry.registerProperty<ColumnGapProperty>();

    // Baseline
    registry.registerProperty<DominantBaselineProperty>();
    registry.registerProperty<BaselineSourceProperty>();
    registry.registerProperty<AlignmentBaselineProperty>();

    // Background
    registry.registerProperty<BackgroundAttachmentProperty>();
    registry.registerProperty<BackgroundColorProperty>();
    registry.registerProperty<BackgroundImageProperty>();
    registry.registerProperty<BackgroundPositionProperty>();
    registry.registerProperty<BackgroundRepeatProperty>();
    registry.registerProperty<BackgroundProperty>();
    registry.registerProperty<ColorProperty>();
    registry.registerProperty<DisplayProperty>();
    registry.registerProperty<TableLayoutProperty>();
    registry.registerProperty<CaptionSideProperty>();

    // Transform
    registry.registerProperty<TransformOriginProperty>();
    registry.registerProperty<TransformBoxProperty>();
    registry.registerProperty<TransformProperty>();

    // Visibility
    registry.registerProperty<VisibilityProperty>();

    // Borders – Colors
    registry.registerProperty<BorderTopColorProperty>();
    registry.registerProperty<BorderRightColorProperty>();
    registry.registerProperty<BorderBottomColorProperty>();
    registry.registerProperty<BorderLeftColorProperty>();
    registry.registerProperty<BorderColorProperty>();

    // Borders – Widths
    registry.registerProperty<BorderTopWidthProperty>();
    registry.registerProperty<BorderRightWidthProperty>();
    registry.registerProperty<BorderBottomWidthProperty>();
    registry.registerProperty<BorderLeftWidthProperty>();

    // Borders – Styles
    registry.registerProperty<BorderStyleProperty>();
    registry.registerProperty<BorderTopStyleProperty>();
    registry.registerProperty<BorderRightStyleProperty>();
    registry.registerProperty<BorderBottomStyleProperty>();
    registry.registerProperty<BorderLeftStyleProperty>();

    // Borders – Radius
    registry.registerProperty<BorderRadiusTopRightProperty>();
    registry.registerProperty<BorderRadiusTopLeftProperty>();
    registry.registerProperty<BorderRadiusBottomRightProperty>();
    registry.registerProperty<BorderRadiusBottomLeftProperty>();
    registry.registerProperty<BorderRadiusProperty>();

    // Borders – Shorthand
    registry.registerProperty<BorderTopProperty>();
    registry.registerProperty<BorderRightProperty>();
    registry.registerProperty<BorderBottomProperty>();
    registry.registerProperty<BorderLeftProperty>();
    registry.registerProperty<BorderProperty>();

    registry.registerProperty<BorderWidthProperty>();

    // Table Borders
    registry.registerProperty<BorderCollapseProperty>();
    registry.registerProperty<BorderSpacingProperty>();

    // Clip
    registry.registerProperty<ClipPathProperty>();

    // Content
    registry.registerProperty<ContentProperty>();

    // Breaks
    registry.registerProperty<BreakAfterProperty>();
    registry.registerProperty<BreakBeforeProperty>();
    registry.registerProperty<BreakInsideProperty>();

    // Flex
    registry.registerProperty<FlexBasisProperty>();
    registry.registerProperty<FlexDirectionProperty>();
    registry.registerProperty<FlexGrowProperty>();
    registry.registerProperty<FlexShrinkProperty>();
    registry.registerProperty<FlexWrapProperty>();
    registry.registerProperty<FlexFlowProperty>();
    registry.registerProperty<FlexProperty>();

    // Float & Clear
    registry.registerProperty<FloatProperty>();
    registry.registerProperty<ClearProperty>();

    // Font
    registry.registerProperty<FontFamilyProperty>();
    registry.registerProperty<FontWeightProperty>();
    registry.registerProperty<FontWidthProperty>();

    registry.registerProperty<FontStyleProperty>();
    registry.registerProperty<FontSizeProperty>();
    registry.registerProperty<FontProperty>();

    // Line
    registry.registerProperty<LineHeightProperty>();

    // Margin
    registry.registerProperty<MarginTopProperty>();
    registry.registerProperty<MarginRightProperty>();
    registry.registerProperty<MarginBottomProperty>();
    registry.registerProperty<MarginLeftProperty>();
    registry.registerProperty<MarginProperty>();

    registry.registerProperty<MarginInlineStartProperty>();
    registry.registerProperty<MarginInlineEndProperty>();
    registry.registerProperty<MarginInlineProperty>();

    registry.registerProperty<MarginBlockStartProperty>();
    registry.registerProperty<MarginBlockEndProperty>();
    registry.registerProperty<MarginBlockProperty>();

    // Outline
    registry.registerProperty<OutlineProperty>();
    registry.registerProperty<OutlineColorProperty>();
    registry.registerProperty<OutlineOffsetProperty>();
    registry.registerProperty<OutlineStyleProperty>();
    registry.registerProperty<OutlineWidthProperty>();

    // Overflow
    registry.registerProperty<OverflowXProperty>();
    registry.registerProperty<OverflowYProperty>();
    registry.registerProperty<OverflowBlockProperty>();
    registry.registerProperty<OverflowInlineProperty>();
    registry.registerProperty<OverflowProperty>();

    registry.registerProperty<OpacityProperty>();

    // Padding
    registry.registerProperty<PaddingTopProperty>();
    registry.registerProperty<PaddingRightProperty>();
    registry.registerProperty<PaddingBottomProperty>();
    registry.registerProperty<PaddingLeftProperty>();
    registry.registerProperty<PaddingInlineStartProperty>();
    registry.registerProperty<PaddingInlineEndProperty>();
    registry.registerProperty<PaddingProperty>();

    // Positioning
    registry.registerProperty<PositionProperty>();
    registry.registerProperty<TopProperty>();
    registry.registerProperty<RightProperty>();
    registry.registerProperty<BottomProperty>();
    registry.registerProperty<LeftProperty>();

    // Sizing
    registry.registerProperty<BoxSizingProperty>();
    registry.registerProperty<WidthProperty>();
    registry.registerProperty<HeightProperty>();
    registry.registerProperty<MinWidthProperty>();
    registry.registerProperty<MinHeightProperty>();
    registry.registerProperty<MaxWidthProperty>();
    registry.registerProperty<MaxHeightProperty>();

    // Text
    registry.registerProperty<TextAlignProperty>();
    registry.registerProperty<TextTransformProperty>();
    registry.registerProperty<WhiteSpaceProperty>();

    // ZIndex
    registry.registerProperty<ZIndexProperty>();

    // SVG
    registry.registerProperty<SvgXProperty>();
    registry.registerProperty<SvgYProperty>();
    registry.registerProperty<SvgCXProperty>();
    registry.registerProperty<SvgCYProperty>();
    registry.registerProperty<SvgRProperty>();
    registry.registerProperty<SvgFillProperty>();
    registry.registerProperty<SvgDProperty>();
    registry.registerProperty<SvgStrokeProperty>();
    registry.registerProperty<SvgStrokeOpacityProperty>();
    registry.registerProperty<SvgViewBoxProperty>();
    registry.registerProperty<FillOpacityProperty>();
    registry.registerProperty<StrokeWidthProperty>();

    return registry;
}

} // namespace Vaev::Style
