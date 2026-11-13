export module Vaev.Engine:props.registry;

import Karm.Core;

import :props.align;
import :props.background;
import :props.base;
import :props.baseline;
import :props.border;
import :props.breaks;
import :props.counter;
import :props.display;
import :props.flex;
import :props.floats;
import :props.fonts;
import :props.list;
import :props.margin;
import :props.multicol;
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
import :props.writing;

using namespace Karm;

namespace Vaev::Style {

export struct ShorthandProcs {
    using ParseFn = Res<Rc<Property>>(Cursor<Css::Sst>& c, Vec<Rc<Property>>& longhands, ParsingMode mode);

    ParseFn const& parse;
};

export struct LonghandProcs {
    using ParseFn = Res<Rc<Property>>(Cursor<Css::Sst>& c, ParsingMode mode);
    using InitialFn = Res<Rc<Property>>();
    using LoadFn = Res<Rc<Property>>(ComputedValues const& style);

    ParseFn const& parse;
    InitialFn const& initial;
    LoadFn const& load;
};

export static auto _shorthandProcs = Array<Opt<ShorthandProcs>, enumLen<ShorthandId>()>::fill(NONE);
export static auto _longhandProcs = Array<Opt<LonghandProcs>, enumLen<LonghandId>()>::fill(NONE);

export template <typename T>
void registerShorthand() {
    _shorthandProcs[toUnderlyingType(T::ID)] = Some(ShorthandProcs{
        .parse = T::parse,
    });
}

export template <typename T>
void registerLonghand() {
    _longhandProcs[toUnderlyingType(T::ID)] = Some(LonghandProcs{
        .parse = T::parse,
        .initial = T::initial,
        .load = T::load,
    });
}

struct PropertyTablesInit {
    PropertyTablesInit() {
        // Align
        registerLonghand<AlignContentProperty>();
        registerLonghand<JustifyContentProperty>();
        registerLonghand<JustifySelfProperty>();
        registerLonghand<AlignSelfProperty>();
        registerLonghand<JustifyItemsProperty>();
        registerLonghand<AlignItemsProperty>();

        // Gaps
        registerLonghand<RowGapProperty>();
        registerLonghand<ColumnGapProperty>();

        // Baseline
        registerLonghand<DominantBaselineProperty>();
        registerLonghand<BaselineSourceProperty>();
        registerLonghand<AlignmentBaselineProperty>();
        registerLonghand<BaselineShiftProperty>();
        // registry.registerProperty<VerticalAlignProperty>();

        // Background
        registerLonghand<BackgroundAttachmentProperty>();
        registerLonghand<BackgroundColorProperty>();
        registerLonghand<BackgroundImageProperty>();
        registerLonghand<BackgroundPositionProperty>();
        registerLonghand<BackgroundRepeatProperty>();
        // registry.registerProperty<BackgroundProperty>();
        registerLonghand<ColorProperty>();
        registerLonghand<DisplayProperty>();
        registerLonghand<TableLayoutProperty>();
        registerLonghand<CaptionSideProperty>();

        // Transform
        registerLonghand<TransformOriginProperty>();
        registerLonghand<TransformBoxProperty>();
        registerLonghand<TransformProperty>();

        // Visibility
        registerLonghand<VisibilityProperty>();

        // Borders – Colors
        registerLonghand<BorderTopColorProperty>();
        registerLonghand<BorderRightColorProperty>();
        registerLonghand<BorderBottomColorProperty>();
        registerLonghand<BorderLeftColorProperty>();
        // registry.registerProperty<BorderColorProperty>();

        // Borders – Widths
        registerLonghand<BorderTopWidthProperty>();
        registerLonghand<BorderRightWidthProperty>();
        registerLonghand<BorderBottomWidthProperty>();
        registerLonghand<BorderLeftWidthProperty>();

        // Borders – Styles
        // registry.registerProperty<BorderStyleProperty>();
        registerLonghand<BorderTopStyleProperty>();
        registerLonghand<BorderRightStyleProperty>();
        registerLonghand<BorderBottomStyleProperty>();
        registerLonghand<BorderLeftStyleProperty>();

        // Borders – Radius
        registerLonghand<BorderTopRightRadiusProperty>();
        registerLonghand<BorderTopLeftRadiusProperty>();
        registerLonghand<BorderBottomRightRadiusProperty>();
        registerLonghand<BorderBottomLeftRadiusProperty>();
        // registry.registerProperty<BorderRadiusProperty>();

        // Borders – Shorthand
        // registry.registerProperty<BorderTopProperty>();
        // registry.registerProperty<BorderRightProperty>();
        // registry.registerProperty<BorderBottomProperty>();
        // registry.registerProperty<BorderLeftProperty>();
        // registry.registerProperty<BorderProperty>();

        // registry.registerProperty<BorderWidthProperty>();

        // Table Borders
        registerLonghand<BorderCollapseProperty>();
        registerLonghand<BorderSpacingProperty>();

        // Clip
        registerLonghand<ClipPathProperty>();

        // Content
        registerLonghand<ContentProperty>();

        // Breaks
        registerLonghand<BreakAfterProperty>();
        registerLonghand<BreakBeforeProperty>();
        registerLonghand<BreakInsideProperty>();
        // registry.registerProperty<PageBreakAfterProperty>();
        // registry.registerProperty<PageBreakBeforeProperty>();
        // registry.registerProperty<PageBreakInsideProperty>();

        // Flex
        registerLonghand<FlexBasisProperty>();
        registerLonghand<FlexDirectionProperty>();
        registerLonghand<FlexGrowProperty>();
        registerLonghand<FlexShrinkProperty>();
        registerLonghand<FlexWrapProperty>();
        // registry.registerProperty<FlexFlowProperty>();
        // registry.registerProperty<FlexProperty>();

        // Float & Clear
        registerLonghand<FloatProperty>();
        registerLonghand<ClearProperty>();

        // Font
        registerLonghand<FontFamilyProperty>();
        registerLonghand<FontWeightProperty>();
        registerLonghand<FontWidthProperty>();

        registerLonghand<FontStyleProperty>();
        registerLonghand<FontSizeProperty>();
        // registry.registerProperty<FontProperty>();

        // Line
        registerLonghand<LineHeightProperty>();

        // Margin
        registerLonghand<MarginTopProperty>();
        registerLonghand<MarginRightProperty>();
        registerLonghand<MarginBottomProperty>();
        registerLonghand<MarginLeftProperty>();
        registerLonghand<MarginShorthand>();

        registerLonghand<MarginInlineStartProperty>();
        registerLonghand<MarginInlineEndProperty>();
        // registry.registerProperty<MarginInlineProperty>();

        registerLonghand<MarginBlockStartProperty>();
        registerLonghand<MarginBlockEndProperty>();
        // registry.registerProperty<MarginBlockProperty>();

        // Outline
        // registry.registerProperty<OutlineProperty>();
        registerLonghand<OutlineColorProperty>();
        registerLonghand<OutlineOffsetProperty>();
        registerLonghand<OutlineStyleProperty>();
        registerLonghand<OutlineWidthProperty>();

        // Overflow
        registerLonghand<OverflowXProperty>();
        registerLonghand<OverflowYProperty>();
        registerLonghand<OverflowBlockProperty>();
        registerLonghand<OverflowInlineProperty>();
        // registry.registerProperty<OverflowProperty>();

        registerLonghand<OpacityProperty>();

        // Padding
        registerLonghand<PaddingTopProperty>();
        registerLonghand<PaddingRightProperty>();
        registerLonghand<PaddingBottomProperty>();
        registerLonghand<PaddingLeftProperty>();
        registerLonghand<PaddingInlineStartProperty>();
        registerLonghand<PaddingInlineEndProperty>();
        // registry.registerProperty<PaddingProperty>();

        // Positioning
        registerLonghand<PositionProperty>();
        registerLonghand<TopProperty>();
        registerLonghand<RightProperty>();
        registerLonghand<BottomProperty>();
        registerLonghand<LeftProperty>();

        // Sizing
        registerLonghand<BoxSizingProperty>();
        registerLonghand<WidthProperty>();
        registerLonghand<HeightProperty>();
        registerLonghand<MinWidthProperty>();
        registerLonghand<MinHeightProperty>();
        registerLonghand<MaxWidthProperty>();
        registerLonghand<MaxHeightProperty>();

        // Text
        registerLonghand<TextAlignProperty>();
        registerLonghand<TextTransformProperty>();
        registerLonghand<WhiteSpaceProperty>();

        // ZIndex
        registerLonghand<ZIndexProperty>();

        // SVG
        registerLonghand<SvgXProperty>();
        registerLonghand<SvgYProperty>();
        registerLonghand<SvgCXProperty>();
        registerLonghand<SvgCYProperty>();
        registerLonghand<SvgRProperty>();
        registerLonghand<SvgRxProperty>();
        registerLonghand<SvgRyProperty>();
        registerLonghand<SvgFillProperty>();
        registerLonghand<SvgDProperty>();
        registerLonghand<SvgStrokeProperty>();
        registerLonghand<SvgStrokeOpacityProperty>();
        registerLonghand<SvgViewBoxProperty>();
        registerLonghand<FillOpacityProperty>();
        registerLonghand<StrokeWidthProperty>();

        // Counter
        registerLonghand<CounterResetProperty>();
        registerLonghand<CounterIncrementProperty>();
        registerLonghand<CounterSetProperty>();

        // List
        registerLonghand<ListStyleImageProperty>();
        registerLonghand<ListStyleTypeProperty>();
        registerLonghand<ListStylePositionProperty>();
        // registry.registerProperty<ListStyleProperty>();
        registerLonghand<MarkerSideProperty>();

        // Writing
        registerLonghand<WritingModeProperty>();
        registerLonghand<DirectionProperty>();

        // Multi-column
        registerLonghand<ColumnWidthProperty>();
        registerLonghand<ColumnHeightProperty>();
        registerLonghand<ColumnCountProperty>();
        // registry.registerProperty<ColumnsProperty>();
    }
};

static PropertyTablesInit _propertyInit{};

} // namespace Vaev::Style
