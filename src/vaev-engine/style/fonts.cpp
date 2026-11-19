module;

#include <karm/macros>

export module Vaev.Engine:style.fonts;

import Karm.Core;
import Karm.Ref;
import Karm.Gfx;

import :css.values;
import :style.values;
import :css;

namespace Vaev::Style {

// MARK: Font descriptor values ----------------------------------------------

export enum struct FontDisplay {
    AUTO,
    BLOCK,
    SWAP,
    FALLBACK,
    OPTIONAL,

    _LEN,
};

// MARK: FontFeature -----------------------------------------------------------

export using FontTag = Array<char, 4>;

export FontTag makeTag(Str tag) {
    if (tag.len() != 4)
        return {' ', ' ', ' ', ' '};
    return {tag[0], tag[1], tag[2], tag[3]};
}

export struct FontFeature {
    enum struct _Value {
        OFF,
        ON,
    };

    FontTag tag;
    isize val;

    FontFeature(Str tag, isize val)
        : tag(makeTag(tag)), val(val) {
    }

    FontFeature(Str tag, _Value val)
        : tag(makeTag(tag)), val(static_cast<isize>(val)) {
    }

    void repr(Io::Emit& e) const {
        e("(font-feature {} {})", tag, val);
    }

    bool operator==(FontFeature const& other) const {
        return tag == other.tag and val == other.val;
    }

    bool operator==(FontTag const& other) const {
        return tag == other;
    }

    bool operator==(_Value const& other) const {
        return val == static_cast<isize>(other);
    }
};

export struct FontVariation {
    FontTag tag;
    f64 val;

    FontVariation(Str tag, f64 val)
        : tag(makeTag(tag)), val(val) {
    }

    void repr(Io::Emit& e) const {
        e("(font-variation {} {})", tag, val);
    }

    bool operator==(FontTag const& other) const {
        return tag == other;
    }
};

export struct FontSource {
    Union<Ref::Url, FontFamily> identifier;
    Opt<String> format;

    FontSource(Ref::Url url) : identifier(url) {}

    FontSource(FontFamily family) : identifier(family) {}

    void repr(Io::Emit& e) const {
        e("(font-source {}", identifier);
        if (format)
            e(" format({})", *format);
        e(")");
    }
};


// MARK: FontFace --------------------------------------------------------------

export struct FontFace {
    Symbol family = ""_sym;

    Vec<FontSource> sources;

    Union<None, FontStyle, Range<Deg>> style = FontStyle{};

    Opt<Range<Gfx::FontWeight>> weight = Some(Range<Gfx::FontWeight>::emptyAt(Gfx::FontWeight::REGULAR));
    Opt<Range<Percent>> width = Some(Range<Percent>::emptyAt(computeValue(Css::FontWidth{Keywords::NORMAL})));

    Vec<Range<Rune>> unicodeRange;

    Vec<FontFeature> features;
    Vec<FontVariation> variations;

    Opt<Percent> ascentOverride = NONE;
    Opt<Percent> descentOverride = NONE;
    Opt<Percent> lineGapOverride = NONE;
    Opt<Percent> sizeAdjust = NONE;

    FontDisplay display = FontDisplay::AUTO;

    bool valid() const {
        return any(family.str()) and any(sources);
    }

    Gfx::FontAttrs attributes() const {
        Gfx::FontWeight resolvedWeight = Gfx::FontWeight::REGULAR;
        if (weight)
            resolvedWeight = weight.expect().start;

        Gfx::FontStretch resolvedStretch = Gfx::FontStretch::NORMAL;
        if (width)
            resolvedStretch = Gfx::FontStretch::fromPercent(width.expect().start.value());

        Gfx::FontStyle resolvedStyle = Gfx::FontStyle::NORMAL;
        if (auto const& [s] = style.is<FontStyle>())
            resolvedStyle = s.val;

        return {
            .family = family,
            .weight = resolvedWeight,
            .stretch = resolvedStretch,
            .style = resolvedStyle,
        };
    }

    Gfx::FontAdjust adjustments() const {
        return {
            .ascent = ascentOverride.unwrapOr(Percent{100}).value() / 100,
            .descent = descentOverride.unwrapOr(Percent{100}).value() / 100,
            .linegap = lineGapOverride.unwrapOr(Percent{100}).value() / 100,
            .sizeAdjust = sizeAdjust.unwrapOr(Percent{100}).value() / 100,
        };
    }
};

// MARK: Props -----------------------------------------------------------------

template <typename T>
struct _OneOrTwo {
    T start;
    Opt<T> end = NONE;

    void repr(Io::Emit& e) const {
        e("{}", start);
        if (end)
            e(" {}", *end);
    }
};

template <typename T>
Res<_OneOrTwo<T>> _parseOneOrTwo(Cursor<Css::Sst>& c) {
    auto start = try$(parseValue<T>(c));
    eatWhitespace(c);
    if (c.ended())
        return Ok(_OneOrTwo<T>{start, NONE});
    return Ok(_OneOrTwo<T>{start, Some(try$(parseValue<T>(c)))});
}

// MARK: font-family
// https://www.w3.org/TR/css-fonts-4/#font-family-desc
export struct FontFamilyFontDescriptor {
    using Value = Css::FontFamily;

    Value value = initial();

    static Str name() { return "font-family"; }

    static Value initial() { return Css::FontFamily{"serif"_sym}; }

    void apply(FontFace& f) const {
        f.family = value.name;
    }

    Res<> parse(Cursor<Css::Sst>& c) {
        value = try$(parseValue<Value>(c));
        return Ok();
    }
};

// MARK: src
// https://www.w3.org/TR/css-fonts-4/#src-desc
export struct SrcFontDescriptor {
    using Value = Vec<FontSource>;

    Value value;

    static Str name() { return "src"; }

    static auto initial() {
        return Value{};
    }

    void apply(FontFace& f) const {
        f.sources = value;
    }

    Res<> parse(Cursor<Css::Sst>& c) {
        Vec<FontSource> fontSrcs;
        while (true) {
            eatWhitespace(c);
            if (c.ended())
                return Error::invalidData("unexpected end of input");

            if (c.peek() == Css::Sst::FUNC and c.peek().prefix == Css::Token::function("local(")) {
                auto func = c.next();
                auto localFuncScan = Cursor<Css::Sst>{func.content};
                auto familyName = try$(parseValue<FontFamily>(localFuncScan));
                fontSrcs.pushBack(familyName);
                continue;
            }

            auto& fontSrc = fontSrcs.emplaceBack(try$(parseValue<Ref::Url>(c)));
            eatWhitespace(c);

            if (not c.ended() and c.peek() == Css::Sst::FUNC and c.peek().prefix == Css::Token::function("format(")) {
                auto formatScan = Cursor<Css::Sst>{c.next().content};

                eatWhitespace(formatScan);

                if (formatScan.ended())
                    return Error::invalidData("unexpected end of input");

                if (formatScan.peek() == Css::Token::STRING) {
                    fontSrc.format = Some(formatScan.next().token.data);
                } else if (
                    formatScan.peek() == Css::Token::ident("collection") or
                    formatScan.peek() == Css::Token::ident("opentype") or
                    formatScan.peek() == Css::Token::ident("svg") or
                    formatScan.peek() == Css::Token::ident("truetype") or
                    formatScan.peek() == Css::Token::ident("embedded-opentype") or
                    formatScan.peek() == Css::Token::ident("woff") or
                    formatScan.peek() == Css::Token::ident("woff2")
                ) {
                    fontSrc.format = Some(formatScan.next().token.data);
                }
            }

            eatWhitespace(c);

            if (not c.ended() and c.peek() == Css::Sst::FUNC and c.peek().prefix == Css::Token::function("tech(")) {
                // TODO: https://www.w3.org/TR/css-fonts-4/#font-tech-values
            }

            eatWhitespace(c);
            if (c.ended())
                break;

            if (not c.skip(Css::Token::Type::COMMA))
                return Error::invalidData("expected comma separating font srcs");
        }

        value = std::move(fontSrcs);

        return Ok();
    }
};

// MARK: font-style
// https://www.w3.org/TR/css-fonts-4/#font-style-desc
export struct FontStyleFontDescriptor {
    // oblique [ <angle [-90deg,90deg]>{1,2} ]?
    struct Oblique {
        Opt<_OneOrTwo<Css::Angle>> angles = NONE;

        void repr(Io::Emit& e) const {
            e("(oblique");
            if (angles)
                e(" {}", *angles);
            e(")");
        }
    };

    // auto | normal | italic | left | right | oblique [ <angle [-90deg,90deg]>{1,2} ]?
    using Value = Union<
        Keywords::Auto,
        Keywords::Normal,
        Keywords::Italic,
        Keywords::Left,
        Keywords::Right,
        Oblique>;

    Value value = initial();

    static Str name() { return "font-style"; }

    static Value initial() { return Keywords::AUTO; }

    // <angle [-90deg,90deg]>: math functions are clamped to the range.
    static Deg _clampAngle(Css::Angle const& angle) {
        return Deg{clamp(computeValue(angle).value(), -90.0, 90.0)};
    }

    void apply(FontFace& f) const {
        using Computed = Union<None, FontStyle, Range<Deg>>;
        f.style = value.visit(
            [](Keywords::Auto) -> Computed {
                return NONE;
            },
            [](Keywords::Italic) -> Computed {
                return FontStyle{.val = Gfx::FontStyle::ITALIC};
            },
            [](Oblique const& oblique) -> Computed {
                auto style = FontStyle{.val = Gfx::FontStyle::OBLIQUE};
                if (not oblique.angles)
                    return style;

                auto const& angles = *oblique.angles;
                if (angles.end)
                    return Range<Deg>::fromStartEnd(_clampAngle(angles.start), _clampAngle(*angles.end));

                style.obliqueAngle = _clampAngle(angles.start);
                return style;
            },
            // NOSPEC: left and right are treated as normal.
            [](auto) -> Computed {
                return FontStyle{};
            }
        );
    }

    Res<> parse(Cursor<Css::Sst>& c) {
        if (c.skip(Css::Token::ident("oblique"))) {
            eatWhitespace(c);
            if (c.ended()) {
                value = Oblique{};
                return Ok();
            }
            value = Oblique{Some(try$(_parseOneOrTwo<Css::Angle>(c)))};
            return Ok();
        }

        using Keyword = Union<
            Keywords::Auto,
            Keywords::Normal,
            Keywords::Italic,
            Keywords::Left,
            Keywords::Right>;

        value = try$(parseValue<Keyword>(c)).visit([](auto keyword) -> Value {
            return keyword;
        });
        return Ok();
    }
};

// MARK: font-weight
// https://www.w3.org/TR/css-fonts-4/#font-weight-desc
export struct FontWeightFontDescriptor {
    // auto | <font-weight-absolute>{1,2}
    using Value = Union<Keywords::Auto, _OneOrTwo<Css::FontWeightAbsolute>>;

    Value value = initial();

    static Str name() { return "font-weight"; }

    static Value initial() { return Keywords::AUTO; }

    static Gfx::FontWeight _weight(Css::FontWeightAbsolute const& weight) {
        return Gfx::FontWeight{static_cast<u16>(computeValue(weight))};
    }

    void apply(FontFace& f) const {
        f.weight = value.visit(
            [](Keywords::Auto) -> Opt<Range<Gfx::FontWeight>> {
                return NONE;
            },
            [](_OneOrTwo<Css::FontWeightAbsolute> const& weights) -> Opt<Range<Gfx::FontWeight>> {
                if (weights.end)
                    return Some(Range<Gfx::FontWeight>::fromStartEnd(_weight(weights.start), _weight(*weights.end)));
                return Some(Range<Gfx::FontWeight>::emptyAt(_weight(weights.start)));
            }
        );
    }

    Res<> parse(Cursor<Css::Sst>& c) {
        if (c.skip(Css::Token::ident("auto"))) {
            value = Keywords::AUTO;
            return Ok();
        }

        value = try$(_parseOneOrTwo<Css::FontWeightAbsolute>(c));
        return Ok();
    }
};

// MARK: font-width
// https://www.w3.org/TR/css-fonts-4/#font-width-prop
export struct FontWidthFontDescriptor {
    // auto | <'font-width'>{1,2}
    using Value = Union<Keywords::Auto, _OneOrTwo<Css::FontWidth>>;

    Value value = initial();

    static Str name() { return "font-width"; }

    static Value initial() { return Keywords::AUTO; }

    void apply(FontFace& f) const {
        f.width = value.visit(
            [](Keywords::Auto) -> Opt<Range<Percent>> {
                return NONE;
            },
            [](_OneOrTwo<Css::FontWidth> const& widths) -> Opt<Range<Percent>> {
                if (widths.end)
                    return Some(Range<Percent>::fromStartEnd(computeValue(widths.start), computeValue(*widths.end)));
                return Some(Range<Percent>::emptyAt(computeValue(widths.start)));
            }
        );
    }

    Res<> parse(Cursor<Css::Sst>& c) {
        if (c.skip(Css::Token::ident("auto"))) {
            value = Keywords::AUTO;
            return Ok();
        }

        value = try$(_parseOneOrTwo<Css::FontWidth>(c));
        return Ok();
    }
};

// MARK: unicode-range
// https://www.w3.org/TR/css-fonts-4/#unicode-range-desc
export struct UnicodeRangeFontDescriptor {
    using Value = Vec<Range<Rune>>;

    Value value;

    static Str name() { return "unicode-range"; }

    static auto initial() {
        return Value{};
    }

    void apply(FontFace& f) const {
        f.unicodeRange = value;
    }
};

// MARK: font-feature-settings
// https://www.w3.org/TR/css-fonts-4/#font-rend-desc
export struct FontFeatureSettingsFontDescriptor {
    using Value = Vec<FontFeature>;

    Value value;

    static Str name() { return "font-feature-settings"; }

    static Value initial() { return {}; }

    void apply(FontFace& f) const {
        f.features = value;
    }
};

// MARK: font-variation-settings
// https://www.w3.org/TR/css-fonts-4/#font-rend-desc
export struct FontVariationSettingsFontDescriptor {
    using Value = Vec<FontVariation>;

    Value value;

    static Str name() { return "font-variation-settings"; }

    static Value initial() { return {}; }

    void apply(FontFace& f) const {
        f.variations = value;
    }
};

// MARK: font-named-instance
// https://www.w3.org/TR/css-fonts-4/#font-named-instance
export struct FontNamedInstanceFontDescriptor {
    using Value = Union<Keywords::Auto, String>;

    Value value = initial();

    static Str name() { return "font-named-instance"; }

    static Value initial() { return Keywords::AUTO; }

    void apply(FontFace&) const {
        // TODO
    }

    Res<> parse(Cursor<Css::Sst>& c) {
        value = try$(parseValue<Value>(c));
        return Ok();
    }
};

// MARK: font-display
// https://www.w3.org/TR/css-fonts-4/#font-display-desc
export struct FontDisplayFontDescriptor {
    using Value = FontDisplay;

    Value value = initial();

    static Str name() { return "font-display"; }

    static Value initial() { return FontDisplay::AUTO; }

    void apply(FontFace& f) const {
        f.display = value;
    }

    Res<> parse(Cursor<Css::Sst>& c) {
        if (c.skip(Css::Token::ident("auto"))) {
            value = FontDisplay::AUTO;
        } else if (c.skip(Css::Token::ident("block"))) {
            value = FontDisplay::BLOCK;
        } else if (c.skip(Css::Token::ident("swap"))) {
            value = FontDisplay::SWAP;
        } else if (c.skip(Css::Token::ident("fallback"))) {
            value = FontDisplay::FALLBACK;
        } else if (c.skip(Css::Token::ident("optional"))) {
            value = FontDisplay::OPTIONAL;
        } else {
            return Error::invalidData("expected font-display value");
        }

        return Ok();
    }
};

// MARK: ascent-override
// https://www.w3.org/TR/css-fonts-4/#font-metrics-override-desc
export struct AscentOverrideFontDescriptor {
    // normal | <percentage [0,∞]>
    using Value = Union<Keywords::Normal, Css::Percentage>;

    Value value = initial();

    static Str name() { return "ascent-override"; }

    static Value initial() { return Keywords::NORMAL; }

    void apply(FontFace& f) const {
        // NOTE: normal is NONE
        f.ascentOverride = value.visit(
            [](Keywords::Normal) -> Opt<Percent> { return NONE; },
            [](Css::Percentage const& p) -> Opt<Percent> { return Some(computeValue(p)); }
        );
    }

    Res<> parse(Cursor<Css::Sst>& c) {
        value = try$(parseValue<Value>(c));
        return Ok();
    }
};

// MARK: descent-override
// https://www.w3.org/TR/css-fonts-4/#font-metrics-override-desc
export struct DescentOverrideStyleProp {
    // normal | <percentage [0,∞]>
    using Value = Union<Keywords::Normal, Css::Percentage>;

    Value value = initial();

    static Str name() { return "descent-override"; }

    static Value initial() { return Keywords::NORMAL; }

    void apply(FontFace& f) const {
        // NOTE: normal is NONE
        f.descentOverride = value.visit(
            [](Keywords::Normal) -> Opt<Percent> { return NONE; },
            [](Css::Percentage const& p) -> Opt<Percent> { return Some(computeValue(p)); }
        );
    }

    Res<> parse(Cursor<Css::Sst>& c) {
        value = try$(parseValue<Value>(c));
        return Ok();
    }
};

// MARK: line-gap-override
// https://www.w3.org/TR/css-fonts-4/#font-metrics-override-desc
export struct LineGapOverrideFontDescriptor {
    // normal | <percentage [0,∞]>
    using Value = Union<Keywords::Normal, Css::Percentage>;

    Value value = initial();

    static Str name() { return "line-gap-override"; }

    static Value initial() { return Keywords::NORMAL; }

    void apply(FontFace& f) const {
        // NOTE: normal is NONE
        f.lineGapOverride = value.visit(
            [](Keywords::Normal) -> Opt<Percent> { return NONE; },
            [](Css::Percentage const& p) -> Opt<Percent> { return Some(computeValue(p)); }
        );
    }

    Res<> parse(Cursor<Css::Sst>& c) {
        value = try$(parseValue<Value>(c));
        return Ok();
    }
};

// MARK: size-adjust
// https://www.w3.org/TR/css-fonts-5/#size-adjust-desc
export struct SizeAdjustFontDescriptor {
    // <percentage [0,∞]>
    using Value = Css::Percentage;

    Value value = initial();

    static Str name() { return "size-adjust"; }

    static Value initial() { return Css::percent(100); }

    void apply(FontFace& f) const {
        f.sizeAdjust = Some(computeValue(value));
    }

    Res<> parse(Cursor<Css::Sst>& c) {
        value = try$(parseValue<Value>(c));
        return Ok();
    }
};

using _FontDescriptor = Union<
    FontFamilyFontDescriptor,
    SrcFontDescriptor,
    FontStyleFontDescriptor,
    FontWeightFontDescriptor,
    FontWidthFontDescriptor,

    UnicodeRangeFontDescriptor,

    FontFeatureSettingsFontDescriptor,
    FontVariationSettingsFontDescriptor,
    FontNamedInstanceFontDescriptor,

    FontDisplayFontDescriptor,

    AscentOverrideFontDescriptor,
    DescentOverrideStyleProp,
    LineGapOverrideFontDescriptor,
    SizeAdjustFontDescriptor

    /**/
    >;

export struct FontDescriptor : _FontDescriptor {
    using _FontDescriptor::_FontDescriptor;

    static constexpr Array LEGACY_ALIAS = {
        Pair<Str>("font-stretch", "font-width"),
    };

    static Str mapLegacyAlias(Str name) {
        for (auto const& [from, to] : LEGACY_ALIAS) {
            if (from == name) [[unlikely]]
                return to;
        }

        return name;
    }

    Str name() const {
        return visit([](auto const& p) {
            return p.name();
        });
    }

    void apply(FontFace& f) const {
        visit([&](auto const& p) {
            p.apply(f);
        });
    }

    void repr(Io::Emit& e) const {
        e("({}", name());
        visit([&](auto const& p) {
            e(" {}", p.value);
        });
        e(")");
    }
};

} // namespace Vaev::Style
