module Vaev.Engine;

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

// MARK: Procedure Tables -----------------------------------------------------

using LonghandParseFn = Res<Rc<Property>>(Cursor<Css::Sst>&, ParsingMode);
using LonghandInitialFn = Rc<Property>();
using LonghandLoadFn = Rc<Property>(ComputedValues const&);
using LonghandInheritFn = void(ComputedValues const& parent, ComputedValues& child);

using ShorthandParseFn = Res<>(Cursor<Css::Sst>&, Vec<Rc<Property>>& longhands, ParsingMode);

struct PropertyTables {
    static constexpr usize LONGHAND_LEN = enumLen<LonghandId>();
    static constexpr usize SHORTHAND_LEN = enumLen<ShorthandId>();

    auto longhandParse = Array<Opt<LonghandParseFn const&>, LONGHAND_LEN>::fill(NONE);
    auto longhandInitial = Array<Opt<LonghandInitialFn const&>, LONGHAND_LEN>::fill(NONE);
    auto longhandLoad = Array<Opt<LonghandLoadFn const&>, LONGHAND_LEN>::fill(NONE);

    auto longhandInherit = Array<Opt<LonghandInheritFn const&>, LONGHAND_LEN>::fill(NONE);

    auto longhandPhase = Array<ComputationPhase, LONGHAND_LEN>::fill(ComputationPhase::NORMAL);
    auto longhandAliases = Array<Slice<Symbol>, LONGHAND_LEN>::fill(Slice<Symbol>{});

    auto shorthandParse = Array<Opt<ShorthandParseFn const&>, SHORTHAND_LEN>::fill(NONE);

    template <typename T>
    constexpr void registerLonghand() {
        auto id = toUnderlyingType(T::ID);

        longhandParse[id] = Some(T::parse);
        longhandInitial[id] = Some(T::initial);
        longhandLoad[id] = Some(T::load);
        if constexpr (requires { T::inherit; })
            longhandInherit[id] = Some(T::inherit);
        longhandPhase[id] = Some(T::PHASE);
    }

    template <typename T>
    constexpr void registerShorthand() {
        auto id = toUnderlyingType(T::ID);
        shorthandParse[id] = &T::parse;
    }
};

static constexpr PropertyTables _propertyTables = [] {
    PropertyTables tables;

    // Align
    tables.registerLonghand<AlignContentProperty>();
    tables.registerLonghand<JustifyContentProperty>();
    tables.registerLonghand<JustifySelfProperty>();
    tables.registerLonghand<AlignSelfProperty>();
    tables.registerLonghand<JustifyItemsProperty>();
    tables.registerLonghand<AlignItemsProperty>();

    // Gaps
    tables.registerLonghand<RowGapProperty>();
    tables.registerLonghand<ColumnGapProperty>();

    // Baseline
    tables.registerLonghand<DominantBaselineProperty>();
    tables.registerLonghand<BaselineSourceProperty>();
    tables.registerLonghand<AlignmentBaselineProperty>();
    tables.registerLonghand<BaselineShiftProperty>();
    // VerticalAlignProperty: shorthand

    // Background
    tables.registerLonghand<BackgroundAttachmentProperty>();
    tables.registerLonghand<BackgroundColorProperty>();
    tables.registerLonghand<BackgroundImageProperty>();
    tables.registerLonghand<BackgroundPositionProperty>();
    tables.registerLonghand<BackgroundRepeatProperty>();
    // BackgroundProperty: shorthand
    tables.registerLonghand<ColorProperty>();
    tables.registerLonghand<DisplayProperty>();
    tables.registerLonghand<TableLayoutProperty>();
    tables.registerLonghand<CaptionSideProperty>();

    // Transform
    tables.registerLonghand<TransformOriginProperty>();
    tables.registerLonghand<TransformBoxProperty>();
    tables.registerLonghand<TransformProperty>();

    // Visibility
    tables.registerLonghand<VisibilityProperty>();

    // Borders – Colors
    tables.registerLonghand<BorderTopColorProperty>();
    tables.registerLonghand<BorderRightColorProperty>();
    tables.registerLonghand<BorderBottomColorProperty>();
    tables.registerLonghand<BorderLeftColorProperty>();
    // BorderColorProperty: shorthand

    // Borders – Widths
    tables.registerLonghand<BorderTopWidthProperty>();
    tables.registerLonghand<BorderRightWidthProperty>();
    tables.registerLonghand<BorderBottomWidthProperty>();
    tables.registerLonghand<BorderLeftWidthProperty>();

    // Borders – Styles
    // BorderStyleProperty: shorthand
    tables.registerLonghand<BorderTopStyleProperty>();
    tables.registerLonghand<BorderRightStyleProperty>();
    tables.registerLonghand<BorderBottomStyleProperty>();
    tables.registerLonghand<BorderLeftStyleProperty>();

    // Borders – Radius
    tables.registerLonghand<BorderTopRightRadiusProperty>();
    tables.registerLonghand<BorderTopLeftRadiusProperty>();
    tables.registerLonghand<BorderBottomRightRadiusProperty>();
    tables.registerLonghand<BorderBottomLeftRadiusProperty>();
    // BorderRadiusProperty: shorthand

    // Borders – Shorthand
    // BorderTopProperty: shorthand
    // BorderRightProperty: shorthand
    // BorderBottomProperty: shorthand
    // BorderLeftProperty: shorthand
    // BorderProperty: shorthand

    // BorderWidthProperty: shorthand

    // Table Borders
    tables.registerLonghand<BorderCollapseProperty>();
    tables.registerLonghand<BorderSpacingProperty>();

    // Clip
    tables.registerLonghand<ClipPathProperty>();

    // Content
    tables.registerLonghand<ContentProperty>();

    // Breaks
    tables.registerLonghand<BreakAfterProperty>();
    tables.registerLonghand<BreakBeforeProperty>();
    tables.registerLonghand<BreakInsideProperty>();
    // PageBreakAfterProperty: shorthand
    // PageBreakBeforeProperty: shorthand
    // PageBreakInsideProperty: shorthand

    // Flex
    tables.registerLonghand<FlexBasisProperty>();
    tables.registerLonghand<FlexDirectionProperty>();
    tables.registerLonghand<FlexGrowProperty>();
    tables.registerLonghand<FlexShrinkProperty>();
    tables.registerLonghand<FlexWrapProperty>();
    // FlexFlowProperty: shorthand
    // FlexProperty: shorthand

    // Float & Clear
    tables.registerLonghand<FloatProperty>();
    tables.registerLonghand<ClearProperty>();

    // Font
    tables.registerLonghand<FontFamilyProperty>();
    tables.registerLonghand<FontWeightProperty>();
    tables.registerLonghand<FontWidthProperty>();

    tables.registerLonghand<FontStyleProperty>();
    tables.registerLonghand<FontSizeProperty>();
    // FontProperty: shorthand

    // Line
    tables.registerLonghand<LineHeightProperty>();

    // Margin
    tables.registerLonghand<MarginTopProperty>();
    tables.registerLonghand<MarginRightProperty>();
    tables.registerLonghand<MarginBottomProperty>();
    tables.registerLonghand<MarginLeftProperty>();
    // MarginShorthand: goes in ShorthandTables once it has a static ID and parse().

    tables.registerLonghand<MarginInlineStartProperty>();
    tables.registerLonghand<MarginInlineEndProperty>();
    // MarginInlineProperty: shorthand

    tables.registerLonghand<MarginBlockStartProperty>();
    tables.registerLonghand<MarginBlockEndProperty>();
    // MarginBlockProperty: shorthand

    // Outline
    // OutlineProperty: shorthand
    tables.registerLonghand<OutlineColorProperty>();
    tables.registerLonghand<OutlineOffsetProperty>();
    tables.registerLonghand<OutlineStyleProperty>();
    tables.registerLonghand<OutlineWidthProperty>();

    // Overflow
    tables.registerLonghand<OverflowXProperty>();
    tables.registerLonghand<OverflowYProperty>();
    tables.registerLonghand<OverflowBlockProperty>();
    tables.registerLonghand<OverflowInlineProperty>();
    // OverflowProperty: shorthand

    tables.registerLonghand<OpacityProperty>();

    // Padding
    tables.registerLonghand<PaddingTopProperty>();
    tables.registerLonghand<PaddingRightProperty>();
    tables.registerLonghand<PaddingBottomProperty>();
    tables.registerLonghand<PaddingLeftProperty>();
    tables.registerLonghand<PaddingInlineStartProperty>();
    tables.registerLonghand<PaddingInlineEndProperty>();
    // PaddingProperty: shorthand

    // Positioning
    tables.registerLonghand<PositionProperty>();
    tables.registerLonghand<TopProperty>();
    tables.registerLonghand<RightProperty>();
    tables.registerLonghand<BottomProperty>();
    tables.registerLonghand<LeftProperty>();

    // Sizing
    tables.registerLonghand<BoxSizingProperty>();
    tables.registerLonghand<WidthProperty>();
    tables.registerLonghand<HeightProperty>();
    tables.registerLonghand<MinWidthProperty>();
    tables.registerLonghand<MinHeightProperty>();
    tables.registerLonghand<MaxWidthProperty>();
    tables.registerLonghand<MaxHeightProperty>();

    // Text
    tables.registerLonghand<TextAlignProperty>();
    tables.registerLonghand<TextTransformProperty>();
    tables.registerLonghand<WhiteSpaceProperty>();

    // ZIndex
    tables.registerLonghand<ZIndexProperty>();

    // SVG
    tables.registerLonghand<SvgXProperty>();
    tables.registerLonghand<SvgYProperty>();
    tables.registerLonghand<SvgCXProperty>();
    tables.registerLonghand<SvgCYProperty>();
    tables.registerLonghand<SvgRProperty>();
    tables.registerLonghand<SvgRxProperty>();
    tables.registerLonghand<SvgRyProperty>();
    tables.registerLonghand<SvgFillProperty>();
    tables.registerLonghand<SvgDProperty>();
    tables.registerLonghand<SvgStrokeProperty>();
    tables.registerLonghand<SvgStrokeOpacityProperty>();
    tables.registerLonghand<SvgViewBoxProperty>();
    tables.registerLonghand<FillOpacityProperty>();
    tables.registerLonghand<StrokeWidthProperty>();

    // Counter
    tables.registerLonghand<CounterResetProperty>();
    tables.registerLonghand<CounterIncrementProperty>();
    tables.registerLonghand<CounterSetProperty>();

    // List
    tables.registerLonghand<ListStyleImageProperty>();
    tables.registerLonghand<ListStyleTypeProperty>();
    tables.registerLonghand<ListStylePositionProperty>();
    // ListStyleProperty: shorthand
    tables.registerLonghand<MarkerSideProperty>();

    // Writing
    tables.registerLonghand<WritingModeProperty>();
    tables.registerLonghand<DirectionProperty>();

    // Multi-column
    tables.registerLonghand<ColumnWidthProperty>();
    tables.registerLonghand<ColumnHeightProperty>();
    tables.registerLonghand<ColumnCountProperty>();
    // ColumnsProperty: shorthand

    // FIXME: Shorthands are registered here once they have a static ID and parse().

    return tables;
}();

static Map<Symbol, PropertyId> const& _propertyIds() {
    static auto ids = [] {
        Map<Symbol, PropertyId> ids;
        for (usize i = 0; i < enumLen<LonghandId>(); i++) {
            ids.put(Symbol::from(_longhandNames[i]), PropertyId{LonghandId(i)});
            for (auto alias : _propertyTables.longhandAliases[i])
                ids.put(Symbol::from(alias), PropertyId{LonghandId(i)});
        }
        for (usize i = 0; i < enumLen<ShorthandId>(); i++)
            ids.put(Symbol::from(_shorthandNames[i]), PropertyId{ShorthandId(i)});
        return ids;
    }();
    return ids;
}

Opt<PropertyId> lookupProperty(Symbol name) {
    return _propertyIds().lookup(name);
}

Rc<Property> initialProperty(LonghandId id) {
    return _propertyTables.longhandInitial[toUnderlyingType(id)].expect("unimplemented property")();
}

Rc<Property> loadProperty(LonghandId id, ComputedValues const& c) {
    return _propertyTables.longhandLoad[toUnderlyingType(id)].expect("unimplemented property")(c);
}

Res<Rc<Property>> parseProperty(LonghandId id, Cursor<Css::Sst>& c, ParsingMode mode) {
    return _propertyTables.longhandParse[toUnderlyingType(id)].expect("unimplemented property")(c, mode);
}

void inheritProperty(LonghandId id, ComputedValues const& parent, ComputedValues& child) {
    // NOTE: This is the slow fallback path for properties that are not
    //       commonly inherited. Any property marked with the INHERITED
    //       flag should override this method with a faster implementation.
    // if (flags.has(INHERITED))
    //     logFatal("property {:#} marked as INHERITED is using the slow fallback path. override inherit()!", name);

    // NOTE: Since we are copying computed values, we don't expect much new computation to happen
    ComputationContext dummy{};
    loadProperty(id, parent)->apply(parent, child, dummy);
}

} // namespace Vaev::Style
