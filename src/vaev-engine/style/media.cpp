export module Vaev.Engine:style.media;

import Karm.Core;
import Karm.Print;
import Karm.Math;
import Karm.Logger;
import Karm.Debug;

import :values;
import :css.values;
import :style.values;
import :css;

using namespace Karm;
using namespace Vaev::Literals;

namespace Vaev::Style {

// MARK: Media feature values ------------------------------------------------

// MARK: Media Types -----------------------------------------------------------
// https://drafts.csswg.org/mediaqueries/#media-types
export enum struct MediaType {
    ALL,
    PRINT,
    SCREEN,
    OTHER, // tty, tv, projection, handheld, braille, aural, embossed

    _LEN,
};

// MARK: Scan ------------------------------------------------------------------
// https://drafts.csswg.org/mediaqueries/#scan
export enum struct Scan {
    INTERLACE,
    PROGRESSIVE,

    _LEN,
};

// MARK: Update ----------------------------------------------------------------
// https://drafts.csswg.org/mediaqueries/#update
export enum struct Update {
    NONE,
    SLOW,
    FAST,

    _LEN,
};

// MARK: OverflowBlock ---------------------------------------------------------
// https://drafts.csswg.org/mediaqueries/#mf-overflow-block
export enum struct OverflowBlock {
    NONE,
    SCROLL,
    PAGED,

    _LEN,
};

// MARK: OverflowInline --------------------------------------------------------
// https://drafts.csswg.org/mediaqueries/#mf-overflow-inline
export enum struct OverflowInline {
    NONE,
    SCROLL,

    _LEN,
};

// MARK: Color Gamut -----------------------------------------------------------
// https://drafts.csswg.org/mediaqueries/#color-gamut
export enum struct ColorGamut {
    SRGB,
    DISPLAY_P3,
    REC2020,

    _LEN,
};

// MARK: Pointer ---------------------------------------------------------------
// https://drafts.csswg.org/mediaqueries/#pointer
export enum struct Pointer {
    NONE,
    COARSE,
    FINE,

    _LEN,
};

// MARK: Hover -----------------------------------------------------------------
// https://drafts.csswg.org/mediaqueries/#hover
export enum struct Hover {
    NONE,
    HOVER,

    _LEN,
};

// MARK: Reduce Motion ---------------------------------------------------------
// https://drafts.csswg.org/mediaqueries/#reduced-motion
export enum struct ReducedMotion {
    NO_PREFERENCE,
    REDUCE,

    _LEN,
};

// MARK: ReducedTransparency ---------------------------------------------------
// https://drafts.csswg.org/mediaqueries/#reduced-transparency
export enum struct ReducedTransparency {
    NO_PREFERENCE,
    REDUCE,

    _LEN,
};

// MARK: Contrast --------------------------------------------------------------
// https://drafts.csswg.org/mediaqueries/#contrast
export enum struct Contrast {
    NO_PREFERENCE,
    MORE,
    LESS,

    _LEN,
};

// MARK: Colors ----------------------------------------------------------------
// https://drafts.csswg.org/mediaqueries/#forced-colors
export enum struct Colors {
    NONE,
    ACTIVE,

    _LEN,
};

// MARK: Color Scheme ----------------------------------------------------------
// https://drafts.csswg.org/mediaqueries/#color-scheme
export enum struct ColorScheme {
    LIGHT,
    DARK,

    _LEN,
};

// MARK: ReducedData
// https://drafts.csswg.org/mediaqueries/#reduced-data
export enum struct ReducedData {
    NO_PREFERENCE,
    REDUCE,

    _LEN,
};

} // namespace Vaev::Style

namespace Vaev::Css {

export template <>
struct ValueParser<Style::MediaType> {
    static Res<Style::MediaType> parse(Cursor<Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c.skip(Token::ident("all")))
            return Ok(Style::MediaType::ALL);

        if (c.skip(Token::ident("print")))
            return Ok(Style::MediaType::PRINT);

        if (c.skip(Token::ident("screen")))
            return Ok(Style::MediaType::SCREEN);

        // NOTE: The spec says that we should reconize the following media types
        //       but they should not match anything
        static constexpr Array OTHER = {
            "tty",
            "tv",
            "projection",
            "handheld",
            "braille",
            "embossed",
            "speech",
            "aural",
        };

        for (auto const& item : OTHER) {
            if (c.skip(Token::ident(item)))
                return Ok(Style::MediaType::OTHER);
        }
        return Error::invalidData("expected media type");
    }
};

export template <>
struct ValueParser<Style::Scan> {
    static Res<Style::Scan> parse(Cursor<Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c.skip(Token::ident("interlace")))
            return Ok(Style::Scan::INTERLACE);
        else if (c.skip(Token::ident("progressive")))
            return Ok(Style::Scan::PROGRESSIVE);
        else
            return Error::invalidData("expected scan value");
    }
};

export template <>
struct ValueParser<Style::Update> {
    static Res<Style::Update> parse(Cursor<Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c.skip(Token::ident("none")))
            return Ok(Style::Update::NONE);
        else if (c.skip(Token::ident("slow")))
            return Ok(Style::Update::SLOW);
        else if (c.skip(Token::ident("fast")))
            return Ok(Style::Update::FAST);
        else
            return Error::invalidData("expected update value");
    }
};

export template <>
struct ValueParser<Style::OverflowBlock> {
    static Res<Style::OverflowBlock> parse(Cursor<Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c.skip(Token::ident("none")))
            return Ok(Style::OverflowBlock::NONE);
        else if (c.skip(Token::ident("scroll")))
            return Ok(Style::OverflowBlock::SCROLL);
        else if (c.skip(Token::ident("paged")))
            return Ok(Style::OverflowBlock::PAGED);
        else
            return Error::invalidData("expected overflow block value");
    }
};

export template <>
struct ValueParser<Style::OverflowInline> {
    static Res<Style::OverflowInline> parse(Cursor<Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c.skip(Token::ident("none")))
            return Ok(Style::OverflowInline::NONE);
        else if (c.skip(Token::ident("scroll")))
            return Ok(Style::OverflowInline::SCROLL);
        else
            return Error::invalidData("expected overflow inline value");
    }
};

export template <>
struct ValueParser<Style::ColorGamut> {
    static Res<Style::ColorGamut> parse(Cursor<Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c.skip(Token::ident("srgb")))
            return Ok(Style::ColorGamut::SRGB);
        else if (c.skip(Token::ident("p3")))
            return Ok(Style::ColorGamut::DISPLAY_P3);
        else if (c.skip(Token::ident("rec2020")))
            return Ok(Style::ColorGamut::REC2020);
        else
            return Error::invalidData("expected color gamut");
    }
};

export template <>
struct ValueParser<Style::Pointer> {
    static Res<Style::Pointer> parse(Cursor<Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c.skip(Token::ident("none")))
            return Ok(Style::Pointer::NONE);
        else if (c.skip(Token::ident("coarse")))
            return Ok(Style::Pointer::COARSE);
        else if (c.skip(Token::ident("fine")))
            return Ok(Style::Pointer::FINE);
        else
            return Error::invalidData("expected pointer value");
    }
};

export template <>
struct ValueParser<Style::Hover> {
    static Res<Style::Hover> parse(Cursor<Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c.skip(Token::ident("none")))
            return Ok(Style::Hover::NONE);
        else if (c.skip(Token::ident("hover")))
            return Ok(Style::Hover::HOVER);
        else
            return Error::invalidData("expected hover value");
    }
};

export template <>
struct ValueParser<Style::ReducedMotion> {
    static Res<Style::ReducedMotion> parse(Cursor<Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c.skip(Token::ident("no-preference")))
            return Ok(Style::ReducedMotion::NO_PREFERENCE);
        else if (c.skip(Token::ident("reduce")))
            return Ok(Style::ReducedMotion::REDUCE);
        else
            return Error::invalidData("expected reduced motion value");
    }
};

export template <>
struct ValueParser<Style::ReducedTransparency> {
    static Res<Style::ReducedTransparency> parse(Cursor<Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c.skip(Token::ident("no-preference")))
            return Ok(Style::ReducedTransparency::NO_PREFERENCE);
        else if (c.skip(Token::ident("reduce")))
            return Ok(Style::ReducedTransparency::REDUCE);
        else
            return Error::invalidData("expected reduced transparency value");
    }
};

export template <>
struct ValueParser<Style::Contrast> {
    static Res<Style::Contrast> parse(Cursor<Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c.skip(Token::ident("less")))
            return Ok(Style::Contrast::LESS);
        else if (c.skip(Token::ident("more")))
            return Ok(Style::Contrast::MORE);
        else
            return Error::invalidData("expected contrast value");
    }
};

export template <>
struct ValueParser<Style::Colors> {
    static Res<Style::Colors> parse(Cursor<Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c.skip(Token::ident("none")))
            return Ok(Style::Colors::NONE);
        else if (c.skip(Token::ident("active")))
            return Ok(Style::Colors::ACTIVE);
        else
            return Error::invalidData("expected colors value");
    }
};

export template <>
struct ValueParser<Style::ColorScheme> {
    static Res<Style::ColorScheme> parse(Cursor<Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c.skip(Token::ident("light")))
            return Ok(Style::ColorScheme::LIGHT);
        else if (c.skip(Token::ident("dark")))
            return Ok(Style::ColorScheme::DARK);
        else
            return Error::invalidData("expected color scheme value");
    }
};

export template <>
struct ValueParser<Style::ReducedData> {
    static Res<Style::ReducedData> parse(Cursor<Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c.skip(Token::ident("no-preference")))
            return Ok(Style::ReducedData::NO_PREFERENCE);
        else if (c.skip(Token::ident("reduce")))
            return Ok(Style::ReducedData::REDUCE);
        else
            return Error::invalidData("expected reduced data value");
    }
};

} // namespace Vaev::Css

namespace Vaev::Style {

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
    MediaType type = MediaType::SCREEN;

    Math::Vec2<Pixel> viewport = {800_px, 600_px};

    Dppx resolution = 1_dppx;

    Scan scan = Scan::PROGRESSIVE;
    bool grid = false;
    Update update = Update::FAST;
    OverflowBlock overflowBlock = OverflowBlock::SCROLL;
    OverflowInline overflowInline = OverflowInline::SCROLL;

    Integer color = 8;
    Integer colorIndex = 0;
    Integer monochrome = 0;
    ColorGamut colorGamut = ColorGamut::SRGB;

    Pointer pointer = Pointer::FINE;
    Hover hover = Hover::HOVER;
    Pointer anyPointer = Pointer::FINE;
    Hover anyHover = Hover::HOVER;

    static Math::Vec2<Pixel> _toViewport(Math::Vec2f deviceSize, Dppx resolution) {
        return {
            Pixel{deviceSize.width / resolution.value()},
            Pixel{deviceSize.height / resolution.value()},
        };
    }

    static DisplayArea forView(Math::Vec2f deviceSize, Dppx resolution = 1_dppx) {
        return {
            .type = MediaType::SCREEN,
            .viewport = _toViewport(deviceSize, resolution),
            .resolution = resolution,
        };
    }

    static DisplayArea forRender(Math::Vec2f deviceSize, Dppx resolution) {
        return {
            .type = MediaType::SCREEN,
            .viewport = _toViewport(deviceSize, resolution),
            .resolution = resolution,
            .update = Update::NONE,
            .overflowBlock = OverflowBlock::NONE,
            .overflowInline = OverflowInline::NONE,
            .pointer = Pointer::NONE,
            .hover = Hover::NONE,
            .anyPointer = Pointer::NONE,
            .anyHover = Hover::NONE,
        };
    }

    static DisplayArea forPrint(Print::Settings const& settings) {
        auto pageSize = settings.pageSize();
        return {
            .type = MediaType::PRINT,
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
    ReducedMotion reducedMotion = ReducedMotion::NO_PREFERENCE;
    ReducedTransparency reducedTransparency = ReducedTransparency::NO_PREFERENCE;
    Contrast contrast = Contrast::NO_PREFERENCE;
    Colors forcedColors = Colors::NONE;
    ColorScheme colorScheme = ColorScheme::LIGHT;
    ReducedData reducedData = ReducedData::NO_PREFERENCE;

    static ViewerPrefs forRender() {
        return {
            .reducedMotion = ReducedMotion::REDUCE,
            .reducedTransparency = ReducedTransparency::REDUCE,
            .contrast = Contrast::MORE,
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
    Print::Orientation orientation;

    // 5. MARK: Display Quality Media Features

    /// 5.1. Resolution: the resolution feature
    /// https://drafts.csswg.org/mediaqueries/#resolution
    Dppx resolution;

    /// 5.2. Scan: the scan feature
    /// https://drafts.csswg.org/mediaqueries/#scan
    Scan scan = Scan::PROGRESSIVE;

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

        if (quirksMediaScreen and type == MediaType::PRINT)
            type = MediaType::SCREEN;

        return {
            .type = type,
            .width = viewport.width,
            .height = viewport.height,
            .aspectRatio = ratio(viewport.width.value(), viewport.height.value()),
            .orientation = viewport.height >= viewport.width
                               ? Print::Orientation::PORTRAIT
                               : Print::Orientation::LANDSCAPE,

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

// Media features are compared in their specified form.
static Css::PureLength _featureValue(Pixel value) {
    return Css::PureLength::fromPx(value.value());
}

static Css::PureResolution _featureValue(Dppx value) {
    return Css::PureResolution::fromDppx(value.value());
}

template <typename T>
static T const& _featureValue(T const& value) {
    return value;
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

    bool match(T actual) const {
        bool result = true;

        if (lower.type == Bound::INCLUSIVE) {
            result &= actual >= lower.value;
        } else if (lower.type == Bound::EXCLUSIVE) {
            result &= actual > lower.value;
        }

        if (upper.type == Bound::INCLUSIVE) {
            result &= actual <= upper.value;
        } else if (upper.type == Bound::EXCLUSIVE) {
            result &= actual < upper.value;
        }

        if (not lower.type and not upper.type) {
            result = actual != T{};
        }

        return result;
    }

    bool match(Media const& media) const {
        return match(_featureValue(media.*F));
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

    bool match(T actual) const {
        if (type == Type::NONE)
            return actual != T{};
        if constexpr (Meta::Same<T, MediaType>)
            if (value == MediaType::ALL)
                return true;
        return actual == value;
    }

    bool match(Media const& media) const {
        return match(media.*F);
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
export using WidthFeature = RangeFeature<"width", Css::PureLength, &Media::width>;

/// 4.2. Height: the height feature
/// https://drafts.csswg.org/mediaqueries/#height
export using HeightFeature = RangeFeature<"height", Css::PureLength, &Media::height>;

/// 4.3. Aspect-Ratio: the aspect-ratio feature
/// https://drafts.csswg.org/mediaqueries/#aspect-ratio
export using AspectRatioFeature = RangeFeature<"aspect-ratio", f64, &Media::aspectRatio>;

/// 4.4. Orientation: the orientation feature
/// https://drafts.csswg.org/mediaqueries/#orientation
export using OrientationFeature = DiscreteFeature<"orientation", Print::Orientation, &Media::orientation>;

// 5. MARK: Display Quality Media Features

/// 5.1. Resolution: the resolution feature
/// https://drafts.csswg.org/mediaqueries/#resolution
export using ResolutionFeature = RangeFeature<"resolution", Css::PureResolution, &Media::resolution>;

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
            logWarn("failed to parse value for feature {:#}: {}", F::name(), maybeValue.none());
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
    if (c.ended()) {
        logWarn("unexpected end of input");
        return TypeFeature{MediaType::OTHER};
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
