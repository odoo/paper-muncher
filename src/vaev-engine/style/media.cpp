export module Vaev.Engine:style.media;

import Karm.Core;
import Karm.Print;
import Karm.Math;
import Karm.Logger;

import :values;
import :css;

using namespace Karm;

namespace Vaev::Style {

// MARK: Media -----------------------------------------------------------------

export struct Media {
    /// 2.3. Media Types
    /// https://drafts.csswg.org/mediaqueries/#media-types
    Computed<MediaType> type;

    // 4. MARK: Viewport/Page Dimensions Media Features

    /// 4.1. Width: the width feature
    /// https://drafts.csswg.org/mediaqueries/#width
    Computed<Length> width;

    /// 4.2. Height: the height feature
    /// https://drafts.csswg.org/mediaqueries/#height
    Computed<Length> height;

    /// 4.3. Device Width: the device-width feature
    /// https://drafts.csswg.org/mediaqueries/#aspect-ratio
    Computed<Ratio> aspectRatio;

    /// 4.4. Orientation: the orientation feature
    /// https://drafts.csswg.org/mediaqueries/#orientation
    Computed<Print::Orientation> orientation;

    // 5. MARK: Display Quality Media Features

    /// 5.1. Resolution: the resolution feature
    /// https://drafts.csswg.org/mediaqueries/#resolution
    Computed<Resolution> resolution;

    /// 5.2. Scan: the scan feature
    /// https://drafts.csswg.org/mediaqueries/#scan
    Computed<Scan> scan = Scan::PROGRESSIVE;

    /// 5.3. Grid: the grid feature
    /// https://drafts.csswg.org/mediaqueries/#grid
    Computed<bool> grid = false;

    /// 5.4. Update: the update feature
    /// https://drafts.csswg.org/mediaqueries/#update
    Computed<Update> update;

    /// 5.5. Overflow Block: the overflow-block feature
    /// https://drafts.csswg.org/mediaqueries/#mf-overflow-block
    Computed<OverflowBlock> overflowBlock;

    /// 5.6. Overflow Inline: the overflow-inline feature
    /// https://drafts.csswg.org/mediaqueries/#mf-overflow-inline
    Computed<OverflowInline> overflowInline;

    //  6. MARK: Color Media Features

    // 6.1. Color: the color feature
    /// https://drafts.csswg.org/mediaqueries/#color
    Computed<Integer> color;

    /// 6.2. Color Index: the color-index feature
    /// https://drafts.csswg.org/mediaqueries/#color-index
    Computed<Integer> colorIndex;

    /// 6.3. Monochrome: the monochrome feature
    /// https://drafts.csswg.org/mediaqueries/#monochrome
    Computed<Integer> monochrome;

    /// 6.4. Color Gamut: the color-gamut feature
    /// https://drafts.csswg.org/mediaqueries/#color-gamut
    Computed<ColorGamut> colorGamut;

    // 7. MARK: Interaction Media Features

    /// 7.1. Pointer: the pointer feature
    /// https://drafts.csswg.org/mediaqueries/#pointer
    Computed<Pointer> pointer;

    /// 7.2. Hover: the hover feature
    /// https://drafts.csswg.org/mediaqueries/#hover
    Computed<Hover> hover;

    /// 7.3. Any Pointer: the any-pointer feature
    /// https://drafts.csswg.org/mediaqueries/#any-input
    Computed<Pointer> anyPointer;

    /// 7.4. Any Hover: the any-hover feature
    /// https://drafts.csswg.org/mediaqueries/#any-input
    Computed<Hover> anyHover;

    // 11. MARK: User Preference Media Features

    // 11.1. Detecting the desire for less motion on the page:
    //       the prefers-reduced-motion feature
    //
    // https://drafts.csswg.org/mediaqueries-5/#prefers-reduced-motion
    Computed<ReducedMotion> prefersReducedMotion;

    // 11.2. Detecting the desire for reduced transparency on the page:
    //       the prefers-reduced-transparency feature
    // https://drafts.csswg.org/mediaqueries-5/#prefers-reduced-transparency
    Computed<ReducedTransparency> prefersReducedTransparency;

    // 11.3. Detecting the desire for increased or decreased color contrast
    //       from elements on the page: the prefers-contrast feature
    //
    // https://drafts.csswg.org/mediaqueries-5/#prefers-contrast
    Computed<Contrast> prefersContrast;

    // 11.4. Detecting Forced Colors Mode: the forced-colors feature
    // https://drafts.csswg.org/mediaqueries-5/#forced-colors
    Computed<Colors> forcedColors;

    // 11.5. Detecting the desire for light or dark color schemes:
    //       the prefers-color-scheme feature
    // https://drafts.csswg.org/mediaqueries-5/#prefers-color-scheme
    Computed<ColorScheme> prefersColorScheme;

    // 11.6. Detecting the desire for reduced data usage when loading a page:
    //       the prefers-reduced-data feature
    // https://drafts.csswg.org/mediaqueries-5/#prefers-reduced-data
    Computed<ReducedData> prefersReducedData;

    // Appendix A: Deprecated Media Features
    Computed<Length> deviceWidth;
    Computed<Length> deviceHeight;
    Computed<Ratio> deviceAspectRatio;

    // Viewport for continuous media and page box for paged media.
    bool changeDisplayArea(Vec2Au displayArea) {
        if (width == displayArea.width and height == displayArea.height)
            return false;

        width = displayArea.width;
        height = displayArea.height;
        aspectRatio = displayArea.width / displayArea.height;

        orientation = Print::orientationFromSize(displayArea);

        deviceWidth = displayArea.width;
        deviceHeight = displayArea.height;
        deviceAspectRatio = displayArea.width / displayArea.height;

        return true;
    }

    ComputationContext computationContext() const {
        // HACK: The page box is used as the viewport instead of the page area.
        //       This best-effort approximation is acceptable since using viewport
        //       units inside @media is a pathological case.
        return ComputationContext{
            .rootFont = Gfx::Font{Gfx::Fontface::fallback(), 16},
            .font = Gfx::Font{Gfx::Fontface::fallback(), 16},
            .writingMode = _WritingMode::HORIZONTAL_TB,
            .viewport = {.small = displayArea()},
            .displayArea = displayArea(),
        };
    }

    static Media forView(Vec2Au viewport, ColorScheme colorScheme) {
        return {
            .type = MediaType::SCREEN,
            .width = viewport.width,
            .height = viewport.height,
            .aspectRatio = viewport.width / viewport.height,
            .orientation = Print::orientationFromSize(viewport),

            .resolution = Resolution::fromDpi(96),
            .scan = Scan::PROGRESSIVE,
            .grid = false,
            .update = Update::FAST,

            .overflowBlock = OverflowBlock::SCROLL,
            .overflowInline = OverflowInline::SCROLL,

            .color = 8,
            .colorIndex = 0,
            .monochrome = 0,
            .colorGamut = ColorGamut::SRGB,
            .pointer = Pointer::FINE,
            .hover = Hover::HOVER,
            .anyPointer = Pointer::FINE,
            .anyHover = Hover::HOVER,

            .prefersReducedMotion = ReducedMotion::NO_PREFERENCE,
            .prefersReducedTransparency = ReducedTransparency::NO_PREFERENCE,
            .prefersContrast = Contrast::NO_PREFERENCE,
            .forcedColors = Colors::NONE,
            .prefersColorScheme = colorScheme,
            .prefersReducedData = ReducedData::NO_PREFERENCE,

            // NOTE: Deprecated Media Features
            .deviceWidth = viewport.width,
            .deviceHeight = viewport.height,
            .deviceAspectRatio = viewport.width / viewport.height,
        };
    }

    static Media defaultMedia() {
        return forView({800_au, 600_au}, ColorScheme::LIGHT);
    }

    static Media forRender(Vec2Au viewport, Resolution scale) {
        return {
            .type = MediaType::SCREEN,
            .width = viewport.width,
            .height = viewport.height,
            .aspectRatio = viewport.width / viewport.height,
            .orientation = Print::orientationFromSize(viewport),

            .resolution = scale,
            .scan = Scan::PROGRESSIVE,
            .grid = false,
            .update = Update::NONE,

            .overflowBlock = OverflowBlock::NONE,
            .overflowInline = OverflowInline::NONE,

            .color = 8,
            .colorIndex = 0,
            .monochrome = 0,
            .colorGamut = ColorGamut::SRGB,
            .pointer = Pointer::NONE,
            .hover = Hover::NONE,
            .anyPointer = Pointer::NONE,
            .anyHover = Hover::NONE,

            .prefersReducedMotion = ReducedMotion::REDUCE,
            .prefersReducedTransparency = ReducedTransparency::REDUCE,
            .prefersContrast = Contrast::MORE,
            .forcedColors = Colors::NONE,
            .prefersColorScheme = ColorScheme::LIGHT,
            .prefersReducedData = ReducedData::NO_PREFERENCE,

            // NOTE: Deprecated Media Features
            .deviceWidth = viewport.width,
            .deviceHeight = viewport.height,
            .deviceAspectRatio = viewport.width / viewport.height,
        };
    }

    static Media forPrint(Print::Settings const& settings) {
        return {
            .type = MediaType::PRINT,
            .width = settings.size.width,
            .height = settings.size.height,
            .aspectRatio = settings.size.width / settings.size.height,
            .orientation = Print::orientationFromSize(settings.size),
            .resolution = Resolution{settings.scale, Resolution::X},
            .scan = Scan::PROGRESSIVE,
            .grid = false,
            .update = Update::FAST,

            .overflowBlock = OverflowBlock::SCROLL,
            .overflowInline = OverflowInline::SCROLL,

            .color = 8,
            .colorIndex = 0,
            .monochrome = 0,
            .colorGamut = ColorGamut::SRGB,
            .pointer = Pointer::FINE,
            .hover = Hover::HOVER,
            .anyPointer = Pointer::FINE,
            .anyHover = Hover::HOVER,

            .prefersReducedMotion = ReducedMotion::NO_PREFERENCE,
            .prefersReducedTransparency = ReducedTransparency::NO_PREFERENCE,
            .prefersContrast = Contrast::NO_PREFERENCE,
            .forcedColors = Colors::NONE,
            .prefersColorScheme = ColorScheme::LIGHT,
            .prefersReducedData = ReducedData::NO_PREFERENCE,

            // NOTE: Deprecated Media Features
            // NOTE: This is only correct for paged media
            .deviceWidth = settings.size.width,
            .deviceHeight = settings.size.height,
            .deviceAspectRatio = settings.size.width / settings.size.height,
        };
    }

    // Viewport for continuous media and page box for paged media.
    Vec2Au displayArea() const {
        return {width, height};
    }
};

// MARK: Media Features --------------------------------------------------------

export enum struct RangePrefix {
    MIN,   // min-<feature> : value
    MAX,   // max-<feature> : value
    EXACT, // <feature> : value
    BOOL,  // <feature>

    _LEN,
};

export template <Value T>
struct RangeBound {
    enum Type : u8 {
        NONE,
        INCLUSIVE,
        EXCLUSIVE,

        _LEN,
    };

    T value = {};
    Type type = NONE;

    void repr(Io::Emit& e) const {
        e(
            "{}{}",
            value,
            type == INCLUSIVE   ? "i"
            : type == EXCLUSIVE ? "e"
                                : ""
        );
    }
};

export template <StrLit NAME, Value T, auto Media::* F>
struct RangeFeature {
    using Bound = RangeBound<T>;
    using Inner = T;

    Bound lower{};
    Bound upper{};

    static constexpr Str name() {
        return NAME;
    }

    bool match(Media const& media) const {
        auto actual = media.*F;

        auto ctx = media.computationContext();

        bool result = true;

        if (lower.type == Bound::INCLUSIVE) {
            result &= actual >= computeValue(lower.value, ctx);
        } else if (lower.type == Bound::EXCLUSIVE) {
            result &= actual > computeValue(lower.value, ctx);
        }

        if (upper.type == Bound::INCLUSIVE) {
            result &= actual <= computeValue(upper.value, ctx);
        } else if (upper.type == Bound::EXCLUSIVE) {
            result &= actual < computeValue(upper.value, ctx);
        }

        // both types are NONE, evaluate in boolean context
        if (not lower.type and not upper.type) {
            result = actual != Computed<T>{};
        }

        return result;
    }

    static RangeFeature min(T value) {
        return {
            .lower = {value, Bound::INCLUSIVE},
        };
    }

    static RangeFeature max(T value) {
        return {
            .upper = {value, Bound::INCLUSIVE},
        };
    }

    static RangeFeature exact(T value) {
        return {
            .lower = {value, Bound::INCLUSIVE},
            .upper = {value, Bound::INCLUSIVE},
        };
    }

    static RangeFeature boolean() {
        return {};
    }

    static RangeFeature make(RangePrefix prefix, T value = {}) {
        switch (prefix) {
        case RangePrefix::MIN:
            return min(value);
        case RangePrefix::MAX:
            return max(value);
        case RangePrefix::EXACT:
            return exact(value);
        case RangePrefix::BOOL:
            return boolean();

        default:
            unreachable();
        }
    }

    void repr(Io::Emit& e) const {
        e("{}: {} - {}", NAME, lower, upper);
    }
};

export template <StrLit NAME, Value T, auto Media::* F>
struct DiscreteFeature {
    using Inner = T;

    enum Type : u8 {
        NONE,
        EQUAL,

        _LEN,
    };

    T value{};
    Type type = EQUAL;

    static constexpr Str name() {
        return NAME;
    }

    static DiscreteFeature make(RangePrefix prefix, T value = {}) {
        if (prefix == RangePrefix::BOOL)
            return {.type = NONE};
        return {value};
    }

    bool match(Media const& media) const {
        auto actual = media.*F;

        auto ctx = media.computationContext();

        if (type == Type::NONE)
            return actual != T{};
        return actual == computeValue(value, ctx);
    }

    void repr(Io::Emit& e) const {
        e("({}: {} {})", NAME, type, value);
    }
};

/// 2.3. Media Types
/// https://drafts.csswg.org/mediaqueries/#media-types
export using TypeFeature = DiscreteFeature<"type", MediaType, &Media::type>;

// 4. MARK: Viewport/Page Dimensions Media Features ----------------------------

/// 4.1. Width: the width feature
/// https://drafts.csswg.org/mediaqueries/#width
export using WidthFeature = RangeFeature<"width", Length, &Media::width>;

/// 4.2. Height: the height feature
/// https://drafts.csswg.org/mediaqueries/#height
export using HeightFeature = RangeFeature<"height", Length, &Media::height>;

/// 4.3. Aspect-Ratio: the aspect-ratio feature
/// https://drafts.csswg.org/mediaqueries/#aspect-ratio
export using AspectRatioFeature = RangeFeature<"aspect-ratio", Ratio, &Media::aspectRatio>;

/// 4.4. Orientation: the orientation feature
/// https://drafts.csswg.org/mediaqueries/#orientation
export using OrientationFeature = DiscreteFeature<"orientation", Print::Orientation, &Media::orientation>;

// 5. MARK: Display Quality Media Features

/// 5.1. Resolution: the resolution feature
/// https://drafts.csswg.org/mediaqueries/#resolution
export using ResolutionFeature = RangeFeature<"resolution", Resolution, &Media::resolution>;

/// 5.2. Scan: the scan feature
/// https://drafts.csswg.org/mediaqueries/#scan
export using ScanFeature = DiscreteFeature<"scan", Scan, &Media::scan>;

/// 5.3. Grid: the grid feature
/// https://drafts.csswg.org/mediaqueries/#grid
export using GridFeature = DiscreteFeature<"grid", bool, &Media::grid>;

/// 5.4. Update: the update feature
/// https://drafts.csswg.org/mediaqueries/#update
export using UpdateFeature = DiscreteFeature<"update", Update, &Media::update>;

/// 5.5. Overflow Block: the overflow-block feature
/// https://drafts.csswg.org/mediaqueries/#mf-overflow-block
export using OverflowBlockFeature = DiscreteFeature<"overflow-block", OverflowBlock, &Media::overflowBlock>;

/// 5.6. Overflow Inline: the overflow-inline feature
/// https://drafts.csswg.org/mediaqueries/#mf-overflow-inline
export using OverflowInlineFeature = DiscreteFeature<"overflow-inline", OverflowInline, &Media::overflowInline>;

//  6. MARK: Color Media Features ----------------------------------------------

// 6.1. Color: the color feature
/// https://drafts.csswg.org/mediaqueries/#color
export using ColorFeature = RangeFeature<"color", Integer, &Media::color>;

/// 6.2. Color Index: the color-index feature
/// https://drafts.csswg.org/mediaqueries/#color-index
export using ColorIndexFeature = RangeFeature<"color-index", Integer, &Media::colorIndex>;

/// 6.3. Monochrome: the monochrome feature
/// https://drafts.csswg.org/mediaqueries/#monochrome
export using MonochromeFeature = RangeFeature<"monochrome", Integer, &Media::monochrome>;

/// 6.4. Color Gamut: the color-gamut feature
/// https://drafts.csswg.org/mediaqueries/#color-gamut
export using ColorGamutFeature = DiscreteFeature<"color-gamut", ColorGamut, &Media::colorGamut>;

// 7. MARK: Interaction Media Features

/// 7.1. Pointer: the pointer feature
/// https://drafts.csswg.org/mediaqueries/#pointer
export using PointerFeature = DiscreteFeature<"pointer", Pointer, &Media::pointer>;

/// 7.2. Hover: the hover feature
/// https://drafts.csswg.org/mediaqueries/#hover
export using HoverFeature = DiscreteFeature<"hover", Hover, &Media::hover>;

/// 7.3. Any Pointer: the any-pointer feature
/// https://drafts.csswg.org/mediaqueries/#any-pointer
export using AnyPointerFeature = DiscreteFeature<"pointer", Pointer, &Media::anyPointer>;

/// 7.4. Any Hover: the any-hover feature
/// https://drafts.csswg.org/mediaqueries/#any-hover
export using AnyHoverFeature = DiscreteFeature<"hover", Hover, &Media::anyHover>;

// 11. MARK: User Preference Media Features ------------------------------------

// 11.1. Detecting the desire for less motion on the page:
//       the prefers-reduced-motion feature
//
// https://drafts.csswg.org/mediaqueries-5/#prefers-reduced-motion
export using PrefersReducedMotionFeature = DiscreteFeature<"prefers-reduced-motion", ReducedMotion, &Media::prefersReducedMotion>;

// 11.2. Detecting the desire for reduced transparency on the page:
//       the prefers-reduced-transparency feature
// https://drafts.csswg.org/mediaqueries-5/#prefers-reduced-transparency
export using PrefersReducedTransparencyFeature = DiscreteFeature<"prefers-reduced-transparency", ReducedTransparency, &Media::prefersReducedTransparency>;

// 11.3. Detecting the desire for increased or decreased color contrast
//       from elements on the page: the prefers-contrast feature
//
// https://drafts.csswg.org/mediaqueries-5/#prefers-contrast
export using PrefersContrastFeature = DiscreteFeature<"prefers-contrast", Contrast, &Media::prefersContrast>;

// 11.4. Detecting Forced Colors Mode: the forced-colors feature
// https://drafts.csswg.org/mediaqueries-5/#forced-colors
export using ForcedColorsFeature = DiscreteFeature<"forced-colors", Colors, &Media::forcedColors>;

// 11.5. Detecting the desire for light or dark color schemes:
//       the prefers-color-scheme feature
// https://drafts.csswg.org/mediaqueries-5/#prefers-color-scheme
export using PrefersColorSchemeFeature = DiscreteFeature<"prefers-color-scheme", ColorScheme, &Media::prefersColorScheme>;

// 11.6. Detecting the desire for reduced data usage when loading a page:
//       the prefers-reduced-data feature
// https://drafts.csswg.org/mediaqueries-5/#prefers-reduced-data
export using PrefersReducedDataFeature = DiscreteFeature<"prefers-reduced-data", ReducedData, &Media::prefersReducedData>;

// Appendix A: Deprecated Media Features
export using DeviceWidthFeature = RangeFeature<"device-width", Length, &Media::deviceWidth>;
export using DeviceHeightFeature = RangeFeature<"device-height", Length, &Media::deviceHeight>;
export using DeviceAspectRatioFeature = RangeFeature<"device-aspect-ratio", Ratio, &Media::deviceAspectRatio>;

// MARK: Media Feature ---------------------------------------------------------

using _Feature = Union<
    TypeFeature,
    WidthFeature,
    HeightFeature,
    AspectRatioFeature,
    OrientationFeature,
    ResolutionFeature,
    ScanFeature,
    GridFeature,
    UpdateFeature,
    OverflowBlockFeature,
    OverflowInlineFeature,
    ColorFeature,
    ColorIndexFeature,
    MonochromeFeature,
    ColorGamutFeature,
    PointerFeature,
    HoverFeature,
    AnyPointerFeature,
    AnyHoverFeature,
    PrefersReducedMotionFeature,
    PrefersReducedTransparencyFeature,
    PrefersContrastFeature,
    ForcedColorsFeature,
    PrefersColorSchemeFeature,
    PrefersReducedDataFeature,
    DeviceWidthFeature,
    DeviceHeightFeature,
    DeviceAspectRatioFeature>;

export struct Feature : _Feature {
    using _Feature::_Feature;

    void repr(Io::Emit& e) const {
        visit([&](auto const& feature) {
            e("{}", feature);
        });
    }

    bool match(Media const& media) const {
        return visit([&](auto const& feature) {
            return feature.match(media);
        });
    }

    static Feature type(MediaType value) {
        return TypeFeature{value};
    }

    static Feature width(WidthFeature value) {
        return value;
    }
};

// MARK: Media Queries ---------------------------------------------------------

export struct MediaQuery {
    struct _Infix {
        enum struct Type {
            AND,
            OR,

            _LEN0,
        };

        Type type;
        Box<MediaQuery> lhs;
        Box<MediaQuery> rhs;

        bool match(Media const& media) const {
            switch (type) {
            case AND:
                return lhs->match(media) and rhs->match(media);
            case OR:
                return lhs->match(media) or rhs->match(media);
            default:
                return false;
            }
        }

        void repr(Io::Emit& e) const {
            e("({} {} {})", *lhs, type, *rhs);
        }
    };

    struct _Prefix {
        enum struct Type {
            NOT,
            ONLY,

            _LEN1,
        };

        Type type;
        Box<MediaQuery> query;

        bool match(Media const& media) const {
            switch (type) {
            case NOT:
                return not query->match(media);
            case ONLY:
                return query->match(media);
            default:
                return false;
            }
        }

        void repr(Io::Emit& e) const {
            e("({} {})", type, *query);
        }
    };

    using _Store = Union<
        None,
        _Infix,
        _Prefix,
        Feature>;

    using enum _Infix::Type;
    using enum _Prefix::Type;

    _Store _store = NONE;

    MediaQuery() = default;

    MediaQuery(None) : _store(None{}) {}

    MediaQuery(Feature feature) : _store(feature) {}

    MediaQuery(Meta::Convertible<Feature> auto&& feature)
        : _store(Feature{
              std::forward<decltype(feature)>(feature),
          }) {}

    MediaQuery(_Prefix::Type type, MediaQuery query)
        : _store(_Prefix{
              type,
              makeBox<MediaQuery>(query),
          }) {}

    MediaQuery(_Infix::Type type, MediaQuery lhs, MediaQuery rhs)
        : _store(_Infix{
              type,
              makeBox<MediaQuery>(lhs),
              makeBox<MediaQuery>(rhs),
          }) {}

    static MediaQuery negate(MediaQuery query) {
        return MediaQuery{NOT, query};
    }

    static MediaQuery only(MediaQuery query) {
        return MediaQuery{ONLY, query};
    }

    static MediaQuery combineAnd(MediaQuery lhs, MediaQuery rhs) {
        return MediaQuery{AND, lhs, rhs};
    }

    static MediaQuery combineOr(MediaQuery lhs, MediaQuery rhs) {
        return MediaQuery{OR, lhs, rhs};
    }

    bool match(Media const& media) const {
        return _store.visit(Visitor{
            [&](auto const& value) {
                return value.match(media);
            },
            [](None) {
                return true;
            },
        });
    }

    void repr(Io::Emit& e) const {
        _store.visit(Visitor{
            [&](auto const& value) {
                e("{}", value);
            },
            [&](None) {
                e("all");
            },
        });
    }
};

// MARK: Parser ----------------------------------------------------------------

Pair<RangePrefix, Str> _explodeFeatureName(Io::SScan s) {
    if (s.skip("min-"))
        return {RangePrefix::MIN, s.remStr()};
    else if (s.skip("max-"))
        return {RangePrefix::MAX, s.remStr()};
    else
        return {RangePrefix::EXACT, s.remStr()};
}

Feature _parseMediaFeature(Cursor<Css::Sst>& c) {
    if (c.ended()) {
        logWarn("unexpected end of input");
        return TypeFeature{MediaType::OTHER};
    }

    if (*c != Css::Token::IDENT) {
        logWarn("expected ident");
        return TypeFeature{MediaType::OTHER};
    }

    auto unexplodedName = c.next().token.data;
    auto [prefix, name] = _explodeFeatureName(unexplodedName.str());

    Opt<Feature> prop;

    eatWhitespace(c);
    Feature::any([&]<typename F>() -> bool {
        if (name != F::name())
            return false;

        if (not c.skip(Css::Token::COLON)) {
            prop.emplace(F::make(RangePrefix::BOOL));
            return true;
        }

        eatWhitespace(c);

        auto maybeValue = parseValue<typename F::Inner>(c);
        if (not maybeValue) {
            logWarn("failed to parse value for feature {#}: {}", F::name(), maybeValue.none());
            return true;
        }

        prop.emplace(F::make(prefix, maybeValue.take()));
        return true;
    });

    if (not prop) {
        logWarn("cannot parse feature: {}", unexplodedName);
        return TypeFeature{MediaType::OTHER};
    }

    return prop.take();
}

MediaQuery _parseMediaQueryInfix(Cursor<Css::Sst>& c);

MediaQuery _parseMediaQueryLeaf(Cursor<Css::Sst>& c) {
    if (c.skip(Css::Token::ident("not"))) {
        return MediaQuery::negate(_parseMediaQueryInfix(c));
    } else if (c.skip(Css::Token::ident("only"))) {
        return _parseMediaQueryInfix(c);
    } else if (c.peek() == Css::Sst::BLOCK) {
        Cursor<Css::Sst> content = c.next().content;
        return _parseMediaQueryInfix(content);
    } else if (auto type = parseValue<MediaType>(c)) {
        return TypeFeature{type.take()};
    } else
        return _parseMediaFeature(c);
}

MediaQuery _parseMediaQueryInfix(Cursor<Css::Sst>& c) {
    auto lhs = _parseMediaQueryLeaf(c);

    eatWhitespace(c);
    while (not c.ended()) {
        if (c.skip(Css::Token::ident("and"))) {
            eatWhitespace(c);
            lhs = MediaQuery::combineAnd(lhs, _parseMediaQueryLeaf(c));
        } else if (c.skip(Css::Token::ident("or"))) {
            eatWhitespace(c);
            lhs = MediaQuery::combineOr(lhs, _parseMediaQueryLeaf(c));
        } else {
            break;
        }
    }
    return lhs;
}

// https://drafts.csswg.org/mediaqueries/#mq-syntax
// https://drafts.csswg.org/mediaqueries/#typedef-media-query-list
export MediaQuery parseMediaQuery(Cursor<Css::Sst>& c) {
    eatWhitespace(c);

    // This definition of <media-query-list> parsing intentionally accepts an empty list.
    if (c.ended())
        return {};

    MediaQuery lhs = _parseMediaQueryInfix(c);
    eatWhitespace(c);
    while (not c.ended() and c.skip(Css::Token::COMMA)) {
        eatWhitespace(c);
        auto rhs = _parseMediaQueryInfix(c);
        lhs = MediaQuery::combineOr(lhs, rhs);
        eatWhitespace(c);
    }

    return lhs;
}

} // namespace Vaev::Style
