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

    ParseFn* parse;
    InitialFn* initial;
    LoadFn* load;
    InheritFn* inherit = nullptr;
};

struct ShorthandProcs {
    using ParseFn = Res<>(Cursor<Css::Sst>&, Vec<Rc<Property>>& longhands, ParsingMode);

    ParseFn* parse;
};

struct PropertyTables {
    static constexpr usize LONGHAND_LEN = enumLen<LonghandId>();
    static constexpr usize SHORTHAND_LEN = enumLen<ShorthandId>();

    Array<Opt<LonghandProcs>, LONGHAND_LEN> longhandProcs = Array<Opt<LonghandProcs>, LONGHAND_LEN>::fill(Opt<LonghandProcs>{});
    Array<Flags<Property::Options>, LONGHAND_LEN> longhandFlags = Array<Flags<Property::Options>, LONGHAND_LEN>::fill(Flags<Property::Options>{});
    Array<ComputationPhase, LONGHAND_LEN> longhandPhase = Array<ComputationPhase, LONGHAND_LEN>::fill(ComputationPhase::NORMAL);
    Array<Slice<Str>, LONGHAND_LEN> longhandAliases = Array<Slice<Str>, LONGHAND_LEN>::fill(Slice<Str>{});

    Array<Opt<ShorthandProcs>, SHORTHAND_LEN> shorthandProcs = Array<Opt<ShorthandProcs>, SHORTHAND_LEN>::fill(Opt<ShorthandProcs>{});
    Array<Slice<LonghandId>, SHORTHAND_LEN> shorthandLonghands = Array<Slice<LonghandId>, SHORTHAND_LEN>::fill(Slice<LonghandId>{});
    Array<Slice<Str>, SHORTHAND_LEN> shorthandAliases = Array<Slice<Str>, SHORTHAND_LEN>::fill(Slice<Str>{});

    template <typename T>
    constexpr void registerLonghand() {
        auto id = toUnderlyingType(T::ID);

        if (longhandProcs[id])
            panic("longhand registered twice"); // Not a constant expression: fails the build.

        auto procs = LonghandProcs{
            .parse = &T::parse,
            .initial = &T::initial,
            .load = &T::load,
        };

        if constexpr (requires { &T::inherit; })
            procs.inherit = &T::inherit;

        longhandProcs[id] = Some(procs);
        longhandFlags[id] = T::FLAGS;
        longhandPhase[id] = T::PHASE;
        longhandAliases[id] = T::LEGACY_NAME_ALIASES;
    }

    template <typename T>
    constexpr void registerShorthand() {
        auto id = toUnderlyingType(T::ID);

        if (shorthandProcs[id])
            panic("shorthand registered twice"); // Not a constant expression: fails the build.

        auto procs = ShorthandProcs{
            .parse = &T::parse,
        };

        shorthandProcs[id] = Some(procs);
        shorthandLonghands[id] = T::LONGHANDS;

        if constexpr (requires { T::LEGACY_NAME_ALIASES; })
            shorthandAliases[id] = T::LEGACY_NAME_ALIASES;
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
    tables.registerShorthand<GapShorthand>();

    // Baseline
    tables.registerLonghand<DominantBaselineProperty>();
    tables.registerLonghand<BaselineSourceProperty>();
    tables.registerLonghand<AlignmentBaselineProperty>();
    tables.registerLonghand<BaselineShiftProperty>();
    tables.registerShorthand<VerticalAlignShorthand>();

    // Background
    tables.registerLonghand<BackgroundAttachmentProperty>();
    tables.registerLonghand<BackgroundColorProperty>();
    tables.registerLonghand<BackgroundImageProperty>();
    tables.registerLonghand<BackgroundPositionProperty>();
    tables.registerLonghand<BackgroundRepeatProperty>();
    tables.registerShorthand<BackgroundShorthand>();
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
    tables.registerShorthand<BorderColorShorthand>();

    // Borders – Widths
    tables.registerLonghand<BorderTopWidthProperty>();
    tables.registerLonghand<BorderRightWidthProperty>();
    tables.registerLonghand<BorderBottomWidthProperty>();
    tables.registerLonghand<BorderLeftWidthProperty>();

    // Borders – Styles
    tables.registerShorthand<BorderStyleShorthand>();
    tables.registerLonghand<BorderTopStyleProperty>();
    tables.registerLonghand<BorderRightStyleProperty>();
    tables.registerLonghand<BorderBottomStyleProperty>();
    tables.registerLonghand<BorderLeftStyleProperty>();

    // Borders – Radius
    tables.registerLonghand<BorderTopRightRadiusProperty>();
    tables.registerLonghand<BorderTopLeftRadiusProperty>();
    tables.registerLonghand<BorderBottomRightRadiusProperty>();
    tables.registerLonghand<BorderBottomLeftRadiusProperty>();
    tables.registerShorthand<BorderRadiusShorthand>();

    // Borders – Shorthand
    tables.registerShorthand<BorderTopShorthand>();
    tables.registerShorthand<BorderRightShorthand>();
    tables.registerShorthand<BorderBottomShorthand>();
    tables.registerShorthand<BorderLeftShorthand>();
    tables.registerShorthand<BorderShorthand>();
    tables.registerShorthand<BorderWidthShorthand>();

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
    tables.registerShorthand<PageBreakAfterShorthand>();
    tables.registerShorthand<PageBreakBeforeShorthand>();
    tables.registerShorthand<PageBreakInsideShorthand>();

    // Flex
    tables.registerLonghand<FlexBasisProperty>();
    tables.registerLonghand<FlexDirectionProperty>();
    tables.registerLonghand<FlexGrowProperty>();
    tables.registerLonghand<FlexShrinkProperty>();
    tables.registerLonghand<FlexWrapProperty>();
    tables.registerShorthand<FlexFlowShorthand>();
    tables.registerShorthand<FlexShorthand>();

    // Float & Clear
    tables.registerLonghand<FloatProperty>();
    tables.registerLonghand<ClearProperty>();

    // Font
    tables.registerLonghand<FontFamilyProperty>();
    tables.registerLonghand<FontWeightProperty>();
    tables.registerLonghand<FontWidthProperty>();

    tables.registerLonghand<FontStyleProperty>();
    tables.registerLonghand<FontSizeProperty>();
    tables.registerShorthand<FontShorthand>();

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
    tables.registerShorthand<MarginInlineShorthand>();

    tables.registerLonghand<MarginBlockStartProperty>();
    tables.registerLonghand<MarginBlockEndProperty>();
    tables.registerShorthand<MarginBlockShorthand>();

    // Outline
    tables.registerShorthand<OutlineShorthand>();
    tables.registerLonghand<OutlineColorProperty>();
    tables.registerLonghand<OutlineOffsetProperty>();
    tables.registerLonghand<OutlineStyleProperty>();
    tables.registerLonghand<OutlineWidthProperty>();

    // Overflow
    tables.registerLonghand<OverflowXProperty>();
    tables.registerLonghand<OverflowYProperty>();
    tables.registerLonghand<OverflowBlockProperty>();
    tables.registerLonghand<OverflowInlineProperty>();
    tables.registerShorthand<OverflowShorthand>();

    tables.registerLonghand<OpacityProperty>();

    // Padding
    tables.registerLonghand<PaddingTopProperty>();
    tables.registerLonghand<PaddingRightProperty>();
    tables.registerLonghand<PaddingBottomProperty>();
    tables.registerLonghand<PaddingLeftProperty>();
    tables.registerLonghand<PaddingInlineStartProperty>();
    tables.registerLonghand<PaddingInlineEndProperty>();
    tables.registerShorthand<PaddingShorthand>();

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
    tables.registerShorthand<ListStyleShorthand>();
    tables.registerLonghand<MarkerSideProperty>();

    // Writing
    tables.registerLonghand<WritingModeProperty>();
    tables.registerLonghand<DirectionProperty>();

    // Multi-column
    tables.registerLonghand<ColumnWidthProperty>();
    tables.registerLonghand<ColumnHeightProperty>();
    tables.registerLonghand<ColumnCountProperty>();
    tables.registerShorthand<ColumnsShorthand>();


    return tables;
}();

static Map<Symbol, PropertyId> const& _propertyIds() {
    static auto ids = [] {
        Map<Symbol, PropertyId> ids;
        for (usize i = 0; i < _longhandNames.len(); i++) {
            ids.put(Symbol::from(_longhandNames[i]), PropertyId{LonghandId(i)});
            for (auto alias : _propertyTables.longhandAliases[i])
                ids.put(Symbol::from(alias), PropertyId{LonghandId(i)});
        }
        for (usize i = 0; i < enumLen<ShorthandId>(); i++) {
            ids.put(Symbol::from(_shorthandNames[i]), PropertyId{ShorthandId(i)});
            for (auto alias : _propertyTables.shorthandAliases[i])
                ids.put(Symbol::from(alias), PropertyId{ShorthandId(i)});
        }
        return ids;
    }();
    return ids;
}

static LonghandProcs const& _expectProcs(LonghandId id) {
    return _propertyTables.longhandProcs[toUnderlyingType(id)].expect("unimplemented property");
}

[[maybe_unused]] static ShorthandProcs const& _expectProcs(ShorthandId id) {
    return _propertyTables.shorthandProcs[toUnderlyingType(id)].expect("unimplemented shorthand");
}

Slice<LonghandId> expandShorthand(ShorthandId id) {
    return _propertyTables.shorthandLonghands[toUnderlyingType(id)];
}

Opt<PropertyId> lookupProperty(Symbol const& name) {
    return _propertyIds().lookup(name);
}

bool isLonghandImplemented(LonghandId id) {
    return _propertyTables.longhandProcs[toUnderlyingType(id)].has();
}

ComputationPhase computationPhase(LonghandId id) {
    return _propertyTables.longhandPhase[toUnderlyingType(id)];
}

Rc<Property> initialProperty(LonghandId id) {
    return _expectProcs(id).initial();
}

Rc<Property> loadProperty(LonghandId id, ComputedValues const& c) {
    return _expectProcs(id).load(c);
}

static Res<Default> _parseDefaulted(Cursor<Css::Sst>& c) {
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

// https://drafts.csswg.org/css-variables/#using-variables
static bool _containsVariable(Cursor<Css::Sst> c) {
    while (not c.ended()) {
        if (c.peek() == Css::Sst::FUNC) {
            auto const& prefix = c.peek().prefix;
            auto prefixToken = prefix.expect()->token;
            if (prefixToken.data == "var(") {
                return true;
            }
        }

        // The var() can be nested inside a function or a block.
        if (_containsVariable(c.peek().content))
            return true;

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

Res<> parseProperty(Symbol name, Cursor<Css::Sst>& c, Vec<Rc<Property>>& stylesheet, ParsingMode mode, bool important) {
    // NOTE: PARTIAL excludes "--" itself, which is reserved.
    //       https://drafts.csswg.org/css-variables/#defining-variables
    if (startWith(name.str(), "--"s) == Match::PARTIAL) {
        Union<Css::Content, Default> value = Css::Content{c};

        auto keywordCursor = c;
        eatWhitespace(keywordCursor);
        if (auto keyword = _parseDefaulted(keywordCursor)) {
            eatWhitespace(keywordCursor);
            if (keywordCursor.ended())
                value = keyword.take();
        }

        auto prop = makeRc<CustomProperty>(name, value);
        prop->important = important;
        stylesheet.pushBack(prop);
        return Ok();
    }

    auto id = try$(lookupProperty(name).okOr(Error::invalidData("unknown property")));
    return id.visit(
        [&](LonghandId longhandId) -> Res<> {
            Opt<LonghandProcs> procs = _propertyTables.longhandProcs[toUnderlyingType(longhandId)];

            if (not procs)
                return Error::notImplemented("unimplemented property");

            eatWhitespace(c);

            if (auto defaultValue = _parseDefaulted(c)) {
                eatWhitespace(c);
                if (not c.ended())
                    return Error::invalidData("unexpected tokens");

                auto prop = makeRc<DefaultedProperty>(longhandId, defaultValue.expect());
                prop->important = important;
                stylesheet.pushBack(prop);
                return Ok();
            }

            auto start = c;
            if (auto property = procs->parse(c, mode)) {
                eatWhitespace(c);
                if (c.ended()) {
                    auto prop = property.take();
                    prop->important = important;
                    stylesheet.pushBack(prop);
                    return Ok();
                }
            }
            c = start;

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

            eatWhitespace(c);

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

            auto start = c;
            auto oldLen = stylesheet.len();
            if (procs->parse(c, stylesheet, mode)) {
                eatWhitespace(c);
                if (c.ended()) {
                    for (auto i = oldLen; i < stylesheet.len(); i++)
                        stylesheet[i]->important = important;
                    return Ok();
                }
            }
            stylesheet.trunc(oldLen);
            c = start;

            if (_containsVariable(c)) {
                for (auto const& longhandId : expandShorthand(shorthandId)) {
                    auto prop = makeRc<DeferredProperty>(longhandId, Some(shorthandId), c, mode);
                    prop->important = important;
                    stylesheet.pushBack(prop);
                }
                return Ok();
            }

            return Error::invalidData("invalid property");
        }
    );
}

void inheritIndividualProperties(ComputedValues const& parent, ComputedValues& child) {
    // NOTE: Properties flagged BULK_INHERITED are inherited by
    //       inheritsComputedValues() copying their whole block.
    for (usize i = 0; i < PropertyTables::LONGHAND_LEN; i++) {
        auto const& procs = _propertyTables.longhandProcs[i];
        if (not procs)
            continue;

        auto flags = _propertyTables.longhandFlags[i];
        if (not flags.has(Property::Options::INHERITED) or flags.has(Property::Options::BULK_INHERITED))
            continue;

        if (procs->inherit)
            procs->inherit(parent, child);
        else
            inheritProperty(static_cast<LonghandId>(i), parent, child);
    }
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
