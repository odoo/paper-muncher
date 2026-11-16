module;

#include <karm/macros>

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

struct LonghandProcs {
    using ParseFn = Res<Rc<Property>>(Cursor<Css::Sst>&, ParsingMode);
    using InitialFn = Rc<Property>();
    using LoadFn = Rc<Property>(ComputedValues const&);
    using InheritFn = void(ComputedValues const& parent, ComputedValues& child);

    ParseFn const& parse;
    InitialFn const& initial;
    LoadFn const& load;
    Opt<InheritFn const&> inherit = NONE;
};

struct ShorthandProcs {
    using ParseFn = Res<>(Cursor<Css::Sst>&, Vec<Rc<Property>>& longhands, ParsingMode);

    ParseFn const& parse;
};

struct PropertyTables {
    static constexpr usize LONGHAND_LEN = enumLen<LonghandId>();
    static constexpr usize SHORTHAND_LEN = enumLen<ShorthandId>();

    auto longhandProcs = Array<Opt<LonghandProcs>, LONGHAND_LEN>::fill(NONE);
    auto shorthandProcs = Array<Opt<ShorthandProcs>, SHORTHAND_LEN>::fill(NONE);

    auto longhandPhase = Array<ComputationPhase, LONGHAND_LEN>::fill(ComputationPhase::NORMAL);
    auto longhandAliases = Array<Slice<Symbol>, LONGHAND_LEN>::fill(Slice<Symbol>{});

    auto shorthandLonghands = Array<Slice<LonghandId>, SHORTHAND_LEN>::fill(Slice<LonghandId>{});

    template <typename T>
    constexpr void registerLonghand() {
        auto id = toUnderlyingType(T::ID);

        auto procs = LonghandProcs{
            .parse = T::parse,
            .initial = T::initial,
            .load = T::load,
        };

        if constexpr (requires { T::inherit; })
            procs.inherit = Some(T::inherit);

        longhandProcs[id] = Some(procs);
        longhandPhase[id] = Some(T::PHASE);
    }

    template <typename T>
    constexpr void registerShorthand() {
        auto id = toUnderlyingType(T::ID);

        auto procs = ShorthandProcs{
            .parse = T::parse,
        };

        shorthandProcs[id] = Some(procs);
        shorthandLonghands[id] = T::LONGHANDS;
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
    tables.registerShorthand<MarginShorthand>();

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

LonghandProcs const& _expectProcs(LonghandId id) {
    _propertyTables.longhandProcs[toUnderlyingType(id)].expect("unimplemented property");
}

ShorthandProcs const& _expectProcs(ShorthandId id) {
    _propertyTables.shorthandProcs[toUnderlyingType(id)].expect("unimplemented shorthand");
}

Slice<LonghandId> expandShorthand(LonghandId id) {
    _propertyTables.shorthandLonghands[toUnderlyingType(id)];
}

Opt<PropertyId> lookupProperty(Symbol const& name) {
    return _propertyIds().lookup(name);
}

Rc<Property> initialProperty(LonghandId id) {
    return _expectProcs(id).initial();
}

Rc<Property> loadProperty(LonghandId id, ComputedValues const& c) {
    return _expectProcs(id).load(c);
}

static Res<Default> _parseDefaulted(Cursor<Css::Sst>& c) {
    Default value;
    if (c.skip(Css::Token::ident("initial"))) {
        return Ok(Default::INITIAL);
    } else if (c.skip(Css::Token::ident("inherit"))) {
        return Ok(Default::INHERIT);
    } else if (c.skip(Css::Token::ident("unset"))) {
        return Ok(Default::UNSET);
    } else {
        return Error::invalidData("unknown declaration");
    }
}

static bool _containsVariable(Cursor<Css::Sst> c) {
    while (not c.ended()) {
        if (c.peek() == Css::Sst::FUNC) {
            auto const& prefix = c.peek().prefix;
            auto prefixToken = prefix.expect()->token;
            if (prefixToken.data == "var(") {
                return true;
            }
        }

        c.next();
    }

    return false;
}

// FIXME: This looks very ad-hoc and hacky.
Res<Rc<Property>> mustParseLonghand(LonghandId id, Cursor<Css::Sst>& c, ParsingMode mode) {
    if (auto defaultValue = _parseDefaulted(c)) {
        eatWhitespace(c);
        if (not c.ended())
            return Error::invalidData("unexpected tokens");

        auto prop = makeRc<DefaultedProperty>(id, defaultValue.expect());
        return Ok(prop);
    }

    if (auto property = _expectProcs(id).parse(c, mode)) {
        eatWhitespace(c);
        if (not c.ended())
            return Error::invalidData("unexpected tokens");

        return Ok(property.take());
    }

    return Error::invalidData("invalid property");
}

Res<> parseProperty(PropertyId id, Cursor<Css::Sst>& c, Vec<Rc<Property>>& stylesheet, ParsingMode mode, bool important) {
    eatWhitespace(c);

    return id.visit(
        [&](LonghandId longhandId) -> Res<> {
            Opt<LonghandProcs> procs = _propertyTables.longhandProcs[toUnderlyingType(longhandId)];

            if (not procs)
                return Error::notImplemented("unimplemented property");

            if (auto defaultValue = _parseDefaulted(c)) {
                eatWhitespace(c);
                if (not c.ended())
                    return Error::invalidData("unexpected tokens");

                auto prop = makeRc<DefaultedProperty>(longhandId, defaultValue.expect());
                prop->important = important;
                stylesheet.pushBack(prop);
                return Ok();
            }

            if (auto property = procs->parse(c, mode)) {
                eatWhitespace(c);
                if (not c.ended())
                    return Error::invalidData("unexpected tokens");

                (*property)->important = important;
                stylesheet.pushBack(*property);
                return Ok();
            }

            if (_containsVariable(c)) {
                auto prop = makeRc<DeferredProperty>(longhandId, NONE, c, mode);
                prop->important = important;
                stylesheet.pushBack(prop);
                return Ok();
            }

            return Error::invalidData("invalid property");
        },
        [&](ShorthandId shorthandId) -> Res<> {
            Opt<ShorthandProcs> procs = _propertyTables.shorthandProcs[toUnderlyingType(shorthandId)];

            if (not procs)
                return Error::notImplemented("unimplemented property");

            if (auto defaultValue = _parseDefaulted(c)) {
                eatWhitespace(c);
                if (not c.ended())
                    return Error::invalidData("unexpected tokens");

                for (auto const& longhandId : expandShorthand(shorthandId)) {
                    auto prop = makeRc<DefaultedProperty>(longhandId, defaultValue.expect());
                    prop->important = important;
                    stylesheet.pushBack(prop);
                }
                return Ok();
            }

            auto oldLen = stylesheet.len();
            if (auto property = procs->parse(c, stylesheet, mode)) {
                eatWhitespace(c);
                if (not c.ended()) {
                    stylesheet.trunc(oldLen);
                    return Error::invalidData("unexpected tokens");
                }

                for (auto i = oldLen; i < stylesheet.len(); i++)
                    stylesheet[i]->important = important;

                return Ok();
            }

            if (_containsVariable(c)) {
                for (auto const& longhandId : expandShorthand(shorthandId)) {
                    auto prop = makeRc<DeferredProperty>(longhandId, Some(shorthandId), c, mode);
                    prop->important = important;
                    stylesheet.pushBack(prop);
                }
            }

            return Error::invalidData("invalid property");
        }
    );
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
