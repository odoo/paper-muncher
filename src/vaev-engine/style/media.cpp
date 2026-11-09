export module Vaev.Engine:style.media;

import Karm.Core;
import Karm.Print;
import Karm.Math;
import Karm.Logger;
import Karm.Debug;

import :css.values;
import :style.values;
import :style.viewport;
import :props.base;
import :props.registry;
import :css;

using namespace Karm;
using namespace Vaev::Literals;

namespace Vaev::Style {

// MARK: Media Values ----------------------------------------------------------

// https://drafts.csswg.org/mediaqueries/#media-types
export using MediaType = Union<
    Keywords::All,
    Keywords::Print,
    Keywords::Screen,
    Keywords::Tty,
    Keywords::Tv,
    Keywords::Projection,
    Keywords::Handheld,
    Keywords::Braille,
    Keywords::Embossed,
    Keywords::Speech,
    Keywords::Aural>;

// https://drafts.csswg.org/mediaqueries/#orientation
export using Orientation = Union<
    Keywords::Portrait,
    Keywords::Landscape>;

// https://drafts.csswg.org/mediaqueries/#scan
export using Scan = Union<
    Keywords::Interlace,
    Keywords::Progressive>;

// https://drafts.csswg.org/mediaqueries/#update
export using Update = Union<
    Keywords::None,
    Keywords::Slow,
    Keywords::Fast>;

// https://drafts.csswg.org/mediaqueries/#mf-overflow-block
export using OverflowBlock = Union<
    Keywords::None,
    Keywords::Scroll,
    Keywords::Paged>;

// https://drafts.csswg.org/mediaqueries/#mf-overflow-inline
export using OverflowInline = Union<
    Keywords::None,
    Keywords::Scroll>;

// https://drafts.csswg.org/mediaqueries/#color-gamut
export using ColorGamut = Union<
    Keywords::Srgb,
    Keywords::DisplayP3,
    Keywords::Rec2020>;

// https://drafts.csswg.org/mediaqueries/#pointer
export using Pointer = Union<
    Keywords::None,
    Keywords::Coarse,
    Keywords::Fine>;

// https://drafts.csswg.org/mediaqueries/#hover
export using Hover = Union<
    Keywords::None,
    Keywords::Hover>;

// https://drafts.csswg.org/mediaqueries/#reduced-motion
export using ReducedMotion = Union<
    Keywords::NoPreference,
    Keywords::Reduce>;

// https://drafts.csswg.org/mediaqueries/#reduced-transparency
export using ReducedTransparency = Union<
    Keywords::NoPreference,
    Keywords::Reduce>;

// https://drafts.csswg.org/mediaqueries/#contrast
export using Contrast = Union<
    Keywords::NoPreference,
    Keywords::More,
    Keywords::Less>;

// https://drafts.csswg.org/mediaqueries/#forced-colors
export using Colors = Union<
    Keywords::None,
    Keywords::Active>;

// https://drafts.csswg.org/mediaqueries/#color-scheme
export using ColorScheme = Union<
    Keywords::Light,
    Keywords::Dark>;

// https://drafts.csswg.org/mediaqueries/#reduced-data
export using ReducedData = Union<
    Keywords::NoPreference,
    Keywords::Reduce>;

// MARK: wkhtmltopdf Quirks ----------------------------------------------------

static auto quirksMediaZeroViewport = Debug::Flag::feature(
    "wkhtmltopdf-quirks-media-zero-viewport",
    "Evaluate media queries against a 0x0 viewport, like wkhtmltopdf"
);

static auto quirksMediaScreen = Debug::Flag::feature(
    "wkhtmltopdf-quirks-media-screen",
    "Evaluate media queries with the screen media type when printing, like wkhtmltopdf"
);

// MARK: Display Area ----------------------------------------------------------

export struct DisplayArea {
    MediaType type = Keywords::SCREEN;

    Math::Vec2<Pixel> viewport = {800_px, 600_px};

    Dppx resolution = 1_dppx;

    Scan scan = Keywords::PROGRESSIVE;
    bool grid = false;
    Update update = Keywords::FAST;
    OverflowBlock overflowBlock = Keywords::SCROLL;
    OverflowInline overflowInline = Keywords::SCROLL;

    Integer color = 8;
    Integer colorIndex = 0;
    Integer monochrome = 0;
    ColorGamut colorGamut = Keywords::SRGB;

    Pointer pointer = Keywords::FINE;
    Hover hover = Keywords::HOVER;
    Pointer anyPointer = Keywords::FINE;
    Hover anyHover = Keywords::HOVER;

    static Math::Vec2<Pixel> _toViewport(Math::Vec2f deviceSize, Dppx resolution) {
        return {
            Pixel{deviceSize.width / resolution.value()},
            Pixel{deviceSize.height / resolution.value()},
        };
    }

    static DisplayArea forView(Math::Vec2f deviceSize, Dppx resolution = 1_dppx) {
        return {
            .type = Keywords::SCREEN,
            .viewport = _toViewport(deviceSize, resolution),
            .resolution = resolution,
        };
    }

    static DisplayArea forRender(Math::Vec2f deviceSize, Dppx resolution) {
        return {
            .type = Keywords::SCREEN,
            .viewport = _toViewport(deviceSize, resolution),
            .resolution = resolution,
            .update = Keywords::NONE,
            .overflowBlock = Keywords::NONE,
            .overflowInline = Keywords::NONE,
            .pointer = Keywords::NONE,
            .hover = Keywords::NONE,
            .anyPointer = Keywords::NONE,
            .anyHover = Keywords::NONE,
        };
    }

    static DisplayArea forPrint(Print::Settings const& settings) {
        auto pageSize = settings.pageSize();
        return {
            .type = Keywords::PRINT,
            .viewport = {Pixel{static_cast<f64>(pageSize.width) / settings.scale}, Pixel{static_cast<f64>(pageSize.height) / settings.scale}},
            .resolution = Dppx{settings.scale},
        };
    }

    f64 scale() const {
        return resolution.value();
    }

    Math::Vec2f deviceSize() const {
        return {viewport.width.value() * scale(), viewport.height.value() * scale()};
    }

    DisplayArea withViewport(Math::Vec2<Pixel> viewport) const {
        DisplayArea area = *this;
        area.viewport = viewport;
        return area;
    }

    bool operator==(DisplayArea const&) const = default;
};

// MARK: Viewer Preferences ----------------------------------------------------

export struct ViewerPrefs {
    ReducedMotion reducedMotion = Keywords::NO_PREFERENCE;
    ReducedTransparency reducedTransparency = Keywords::NO_PREFERENCE;
    Contrast contrast = Keywords::NO_PREFERENCE;
    Colors forcedColors = Keywords::NONE;
    ColorScheme colorScheme = Keywords::LIGHT;
    ReducedData reducedData = Keywords::NO_PREFERENCE;

    static ViewerPrefs forRender() {
        return {
            .reducedMotion = Keywords::REDUCE,
            .reducedTransparency = Keywords::REDUCE,
            .contrast = Keywords::MORE,
        };
    }

    bool operator==(ViewerPrefs const&) const = default;
};

// MARK: Media -----------------------------------------------------------------

export struct Media {
    /// 2.3. Media Types
    /// https://drafts.csswg.org/mediaqueries/#media-types
    MediaType type;

    // 4. MARK: Viewport/Page Dimensions Media Features

    /// 4.1. Width: the width feature
    /// https://drafts.csswg.org/mediaqueries/#width
    Pixel width;

    /// 4.2. Height: the height feature
    /// https://drafts.csswg.org/mediaqueries/#height
    Pixel height;

    /// 4.3. Aspect-Ratio: the aspect-ratio feature
    /// https://drafts.csswg.org/mediaqueries/#aspect-ratio
    f64 aspectRatio;

    /// 4.4. Orientation: the orientation feature
    /// https://drafts.csswg.org/mediaqueries/#orientation
    Orientation orientation;

    // 5. MARK: Display Quality Media Features

    /// 5.1. Resolution: the resolution feature
    /// https://drafts.csswg.org/mediaqueries/#resolution
    Dppx resolution;

    /// 5.2. Scan: the scan feature
    /// https://drafts.csswg.org/mediaqueries/#scan
    Scan scan = Keywords::PROGRESSIVE;

    /// 5.3. Grid: the grid feature
    /// https://drafts.csswg.org/mediaqueries/#grid
    bool grid = false;

    /// 5.4. Update: the update feature
    /// https://drafts.csswg.org/mediaqueries/#update
    Update update;

    /// 5.5. Overflow Block: the overflow-block feature
    /// https://drafts.csswg.org/mediaqueries/#mf-overflow-block
    OverflowBlock overflowBlock;

    /// 5.6. Overflow Inline: the overflow-inline feature
    /// https://drafts.csswg.org/mediaqueries/#mf-overflow-inline
    OverflowInline overflowInline;

    //  6. MARK: Color Media Features

    // 6.1. Color: the color feature
    /// https://drafts.csswg.org/mediaqueries/#color
    Integer color;

    /// 6.2. Color Index: the color-index feature
    /// https://drafts.csswg.org/mediaqueries/#color-index
    Integer colorIndex;

    /// 6.3. Monochrome: the monochrome feature
    /// https://drafts.csswg.org/mediaqueries/#monochrome
    Integer monochrome;

    /// 6.4. Color Gamut: the color-gamut feature
    /// https://drafts.csswg.org/mediaqueries/#color-gamut
    ColorGamut colorGamut;

    // 7. MARK: Interaction Media Features

    /// 7.1. Pointer: the pointer feature
    /// https://drafts.csswg.org/mediaqueries/#pointer
    Pointer pointer;

    /// 7.2. Hover: the hover feature
    /// https://drafts.csswg.org/mediaqueries/#hover
    Hover hover;

    /// 7.3. Any Pointer: the any-pointer feature
    /// https://drafts.csswg.org/mediaqueries/#any-input
    Pointer anyPointer;

    /// 7.4. Any Hover: the any-hover feature
    /// https://drafts.csswg.org/mediaqueries/#any-input
    Hover anyHover;

    // 11. MARK: User Preference Media Features

    // 11.1. Detecting the desire for less motion on the page:
    //       the prefers-reduced-motion feature
    //
    // https://drafts.csswg.org/mediaqueries-5/#prefers-reduced-motion
    ReducedMotion prefersReducedMotion;

    // 11.2. Detecting the desire for reduced transparency on the page:
    //       the prefers-reduced-transparency feature
    // https://drafts.csswg.org/mediaqueries-5/#prefers-reduced-transparency
    ReducedTransparency prefersReducedTransparency;

    // 11.3. Detecting the desire for increased or decreased color contrast
    //       from elements on the page: the prefers-contrast feature
    //
    // https://drafts.csswg.org/mediaqueries-5/#prefers-contrast
    Contrast prefersContrast;

    // 11.4. Detecting Forced Colors Mode: the forced-colors feature
    // https://drafts.csswg.org/mediaqueries-5/#forced-colors
    Colors forcedColors;

    // 11.5. Detecting the desire for light or dark color schemes:
    //       the prefers-color-scheme feature
    // https://drafts.csswg.org/mediaqueries-5/#prefers-color-scheme
    ColorScheme prefersColorScheme;

    // 11.6. Detecting the desire for reduced data usage when loading a page:
    //       the prefers-reduced-data feature
    // https://drafts.csswg.org/mediaqueries-5/#prefers-reduced-data
    ReducedData prefersReducedData;

    // Appendix A: Deprecated Media Features
    Pixel deviceWidth;
    Pixel deviceHeight;
    f64 deviceAspectRatio;

    static Media from(DisplayArea const& area, ViewerPrefs const& prefs) {
        auto ratio = [](f64 width, f64 height) -> f64 {
            if (height == 0)
                return 0;
            return width / height;
        };

        auto viewport = area.viewport;
        auto deviceSize = area.deviceSize();
        auto type = area.type;

        if (quirksMediaZeroViewport)
            viewport = {};

        if (quirksMediaScreen and type == Keywords::PRINT)
            type = Keywords::SCREEN;

        return {
            .type = type,
            .width = viewport.width,
            .height = viewport.height,
            .aspectRatio = ratio(viewport.width.value(), viewport.height.value()),
            .orientation = viewport.height >= viewport.width
                               ? Orientation{Keywords::PORTRAIT}
                               : Orientation{Keywords::LANDSCAPE},

            .resolution = area.resolution,
            .scan = area.scan,
            .grid = area.grid,
            .update = area.update,

            .overflowBlock = area.overflowBlock,
            .overflowInline = area.overflowInline,

            .color = area.color,
            .colorIndex = area.colorIndex,
            .monochrome = area.monochrome,
            .colorGamut = area.colorGamut,
            .pointer = area.pointer,
            .hover = area.hover,
            .anyPointer = area.anyPointer,
            .anyHover = area.anyHover,

            .prefersReducedMotion = prefs.reducedMotion,
            .prefersReducedTransparency = prefs.reducedTransparency,
            .prefersContrast = prefs.contrast,
            .forcedColors = prefs.forcedColors,
            .prefersColorScheme = prefs.colorScheme,
            .prefersReducedData = prefs.reducedData,

            .deviceWidth = Pixel{deviceSize.width},
            .deviceHeight = Pixel{deviceSize.height},
            .deviceAspectRatio = ratio(deviceSize.width, deviceSize.height),
        };
    }

    bool operator==(Media const&) const = default;
};

// https://drafts.csswg.org/mediaqueries-4/#units
// Relative length units in media queries are based on the initial value,
// which means that units are never based on results of declarations.
static ComputationContext const& _initialComputationContext(Media const& media) {
    static auto memo = [&] {
        auto initialStyle = defaultRegistry().initialComputedValues();
        auto cx = ComputationContext{};
        cx.populateUsingRootComputedValues(*initialStyle);
        cx.populateUsingParentComputedValues(*initialStyle);
        cx.populateUsingOwnComputedValues(*initialStyle);
        cx.populateUsingViewport(Viewport{.small = {media.width, media.height}});
        return cx;
    }();

    return memo;
}

// MARK: Media Features --------------------------------------------------------

export enum struct RangePrefix {
    MIN,
    MAX,
    EXACT,
    BOOL,

    _LEN,
};

export template <typename T>
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

export template <StrLit NAME, typename T, auto Media::* F>
struct RangeFeature {
    using Bound = RangeBound<T>;
    using Inner = T;

    Bound lower{};
    Bound upper{};

    static constexpr Str name() {
        return NAME;
    }

    using Actual = Meta::RemoveConstVolatileRef<decltype(Meta::declval<Media>().*F)>;

    static Actual _computed(T const& value, Media const& media) {
        if constexpr (Meta::Same<T, Actual>)
            return value;
        else if constexpr (requires { toComputed(value, _initialComputationContext(media)); })
            return toComputed(value, _initialComputationContext(media));
        else
            return toComputed(value);
    }

    bool match(Media const& media) const {
        Actual actual = media.*F;
        bool result = true;

        if (lower.type == Bound::INCLUSIVE) {
            result &= actual >= _computed(lower.value, media);
        } else if (lower.type == Bound::EXCLUSIVE) {
            result &= actual > _computed(lower.value, media);
        }

        if (upper.type == Bound::INCLUSIVE) {
            result &= actual <= _computed(upper.value, media);
        } else if (upper.type == Bound::EXCLUSIVE) {
            result &= actual < _computed(upper.value, media);
        }

        if (not lower.type and not upper.type) {
            result = actual != Actual{};
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

export template <StrLit NAME, typename T, auto Media::* F>
struct DiscreteFeature {
    using Inner = T;

    Opt<T> value = NONE;

    DiscreteFeature() = default;

    DiscreteFeature(T value)
        : value(Some(std::move(value))) {}

    static constexpr Str name() {
        return NAME;
    }

    static DiscreteFeature make(RangePrefix) {
        return {};
    }

    static DiscreteFeature make(RangePrefix prefix, T value) {
        if (prefix == RangePrefix::BOOL)
            return {};
        return {std::move(value)};
    }

    // https://drafts.csswg.org/mediaqueries-5/#mq-boolean-context
    static bool _trueInBooleanContext(T const& actual) {
        if constexpr (Meta::Same<T, bool>) {
            return actual;
        } else {
            if constexpr (requires { actual == Keywords::NONE; })
                if (actual == Keywords::NONE)
                    return false;
            if constexpr (requires { actual == Keywords::NO_PREFERENCE; })
                if (actual == Keywords::NO_PREFERENCE)
                    return false;
            return true;
        }
    }

    bool match(T const& actual) const {
        if (not value)
            return _trueInBooleanContext(actual);
        if constexpr (Meta::Same<T, MediaType>)
            if (*value == Keywords::ALL)
                return true;
        return actual == *value;
    }

    bool match(Media const& media) const {
        return match(media.*F);
    }

    void repr(Io::Emit& e) const {
        e("({}: {})", NAME, value);
    }
};

/// 2.3. Media Types
/// https://drafts.csswg.org/mediaqueries/#media-types
export using TypeFeature = DiscreteFeature<"type", MediaType, &Media::type>;

// 4. MARK: Viewport/Page Dimensions Media Features ----------------------------

/// 4.1. Width: the width feature
/// https://drafts.csswg.org/mediaqueries/#width
export using WidthFeature = RangeFeature<"width", Css::PureLength, &Media::width>;

/// 4.2. Height: the height feature
/// https://drafts.csswg.org/mediaqueries/#height
export using HeightFeature = RangeFeature<"height", Css::PureLength, &Media::height>;

/// 4.3. Aspect-Ratio: the aspect-ratio feature
/// https://drafts.csswg.org/mediaqueries/#aspect-ratio
export using AspectRatioFeature = RangeFeature<"aspect-ratio", f64, &Media::aspectRatio>;

/// 4.4. Orientation: the orientation feature
/// https://drafts.csswg.org/mediaqueries/#orientation
export using OrientationFeature = DiscreteFeature<"orientation", Orientation, &Media::orientation>;

// 5. MARK: Display Quality Media Features

/// 5.1. Resolution: the resolution feature
/// https://drafts.csswg.org/mediaqueries/#resolution
export using ResolutionFeature = RangeFeature<"resolution", Css::PureResolution, &Media::resolution>;

/// 5.2. Scan: the scan feature
/// https://drafts.csswg.org/mediaqueries/#orientation
export using Orientation = Union<
    Keywords::Portrait,
    Keywords::Landscape>;

// https://drafts.csswg.org/mediaqueries/#scan
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
export using DeviceWidthFeature = RangeFeature<"device-width", Css::PureLength, &Media::deviceWidth>;
export using DeviceHeightFeature = RangeFeature<"device-height", Css::PureLength, &Media::deviceHeight>;
export using DeviceAspectRatioFeature = RangeFeature<"device-aspect-ratio", f64, &Media::deviceAspectRatio>;

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
        return _store.visit(
            [&](auto const& value) {
                return value.match(media);
            },
            [](None) {
                return true;
            }
        );
    }

    void repr(Io::Emit& e) const {
        _store.visit(
            [&](auto const& value) {
                e("{}", value);
            },
            [&](None) {
                e("all");
            }
        );
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

Opt<Feature> _parseMediaFeature(Cursor<Css::Sst>& c) {
    if (c.ended()) {
        logWarn("unexpected end of input");
        return NONE;
    }

    if (*c != Css::Token::IDENT) {
        logWarn("expected ident");
        return NONE;
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
            logWarn("failed to parse value for feature {:#}: {}", F::name(), maybeValue.none());
            return true;
        }

        prop.emplace(F::make(prefix, maybeValue.take()));
        return true;
    });

    if (not prop) {
        logWarn("cannot parse feature: {}", unexplodedName);
        return NONE;
    }

    return prop;
}

MediaQuery _parseMediaQueryInfix(Cursor<Css::Sst>& c);

// https://drafts.csswg.org/mediaqueries-4/#error-handling
// A media query that does not match the grammar in the previous section
// must be replaced by not all during parsing.
MediaQuery _matchNothing() {
    return MediaQuery::negate(TypeFeature{Keywords::ALL});
}

MediaQuery _parseMediaQueryLeaf(Cursor<Css::Sst>& c) {
    if (c.ended()) {
        logWarn("unexpected end of input");
        return _matchNothing();
    }

    if (c.skip(Css::Token::ident("not"))) {
        return MediaQuery::negate(_parseMediaQueryInfix(c));
    } else if (c.skip(Css::Token::ident("only"))) {
        return _parseMediaQueryInfix(c);
    } else if (c.peek() == Css::Sst::BLOCK) {
        Cursor<Css::Sst> content = c.next().content;
        return _parseMediaQueryInfix(content);
    } else if (auto type = parseValue<MediaType>(c)) {
        return TypeFeature{type.take()};
    } else if (auto feature = _parseMediaFeature(c)) {
        return feature.take();
    } else
        return _matchNothing();
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
