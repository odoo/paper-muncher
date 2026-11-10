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
    Opt<Range<Percent>> width = Some(Range<Percent>::emptyAt(toComputed(Css::FontWidth{Css::Keywords::NORMAL})));

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

// https://www.w3.org/TR/css-fonts-4/#font-family-desc
export struct FontFamilyFontDescriptor {
    Symbol value = initial();

    static Str name() { return "font-family"; }

    static Symbol initial() { return "serif"_sym; }

    void apply(FontFace& f) const {
        f.family = value;
    }

    Res<> parse(Cursor<Css::Sst>& c) {
        if (c.peek() == Css::Token::STRING) {
            value = Symbol::from(try$(parseValue<String>(c)));
        } else if (c.peek() == Css::Token::IDENT) {
            value = Symbol::from(c.next().token.data);
        } else {
            return Error::invalidData("expected font-family value");
        }

        return Ok();
    }
};

// https://www.w3.org/TR/css-fonts-4/#src-desc
export struct SrcFontDescriptor {
    Vec<FontSource> value;

    static Str name() { return "src"; }

    static auto initial() {
        return Vec<FontSource>{};
    }

    void apply(FontFace& f) {
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

    void apply(FontFace& f) const {
        f.sources = value;
    }
};

// https://drafts.csswg.org/css-fonts-4/#descdef-font-face-font-style
export struct FontStyleFontDescriptor {
    // auto | normal | italic | left | right | oblique [ <angle [-90deg,90deg]>{1,2} ]?
    using Oblique = Css::All<Css::Keywords::Oblique, Opt<Css::Angle>, Opt<Css::Angle>>;
    using Value = Union<
        Css::Keywords::Auto,
        Css::Keywords::Normal,
        Css::Keywords::Italic,
        Css::Keywords::Left,
        Css::Keywords::Right,
        Oblique>;

    Union<None, FontStyle, Range<Deg>> value;

    static Str name() { return "font-style"; }

    static auto initial() { return FontStyle{}; }

    void apply(FontFace& f) const {
        f.style = value;
    }

    // <angle [-90deg,90deg]>: math functions are clamped to the range.
    static Deg _angle(Css::Angle const& angle) {
        return Deg{clamp(toComputed(angle).value(), -90.0, 90.0)};
    }

    Res<> parse(Cursor<Css::Sst>& c) {
        value = try$(parseValue<Value>(c)).visit(
            [](Css::Keywords::Auto) -> Union<None, FontStyle, Range<Deg>> {
                return NONE;
            },
            [](Css::Keywords::Italic) -> Union<None, FontStyle, Range<Deg>> {
                return FontStyle{.val = Gfx::FontStyle::ITALIC};
            },
            [](Oblique const& oblique) -> Union<None, FontStyle, Range<Deg>> {
                if (oblique.v1 and oblique.v2)
                    return Range<Deg>::fromStartEnd(_angle(*oblique.v1), _angle(*oblique.v2));
                auto style = FontStyle{.val = Gfx::FontStyle::OBLIQUE};
                if (oblique.v1)
                    style.obliqueAngle = _angle(*oblique.v1);
                return style;
            },
            // NOSPEC: left and right are treated as normal.
            [](auto) -> Union<None, FontStyle, Range<Deg>> {
                return FontStyle{};
            }
        );
        return Ok();
    }
};

// https://drafts.csswg.org/css-fonts-4/#descdef-font-face-font-weight
export struct FontWeightFontDescriptor {
    Opt<Range<Gfx::FontWeight>> value;

    static Str name() { return "font-weight"; }

    static auto initial() { return Gfx::FontWeight::REGULAR; }

    void apply(FontFace& f) const {
        f.weight = value;
    }

    Res<> parse(Cursor<Css::Sst>& c) {
        if (c.skip(Css::Token::ident("auto"))) {
            value = NONE;
            return Ok();
        }

        auto first = try$(parseValue<Css::FontWeightAbsolute>(c));
        auto start = Gfx::FontWeight{static_cast<u16>(toComputed(first))};

        auto second = parseValue<Css::FontWeightAbsolute>(c);
        if (not second) {
            value = Some(Range<Gfx::FontWeight>::emptyAt(start));
            return Ok();
        }

        auto end = Gfx::FontWeight{static_cast<u16>(toComputed(second.take()))};
        value = Some(Range<Gfx::FontWeight>::fromStartEnd(start, end));

        return Ok();
    }
};

// https://drafts.csswg.org/css-fonts-4/#descdef-font-face-font-width
export struct FontWidthFontDescriptor {
    Opt<Range<Percent>> value = initial();

    static Str name() { return "font-width"; }

    static Opt<Range<Percent>> initial() { return NONE; }

    void apply(FontFace& f) const {
        f.width = value;
    }

    Res<> parse(Cursor<Css::Sst>& c) {
        if (c.skip(Css::Token::ident("auto"))) {
            value = NONE;
            return Ok();
        }

        auto width = try$(parseValue<Css::FontWidth>(c));

        auto val = parseValue<Css::FontWidth>(c);
        if (not val) {
            value = Some(Range<Percent>::emptyAt(toComputed(width)));
            return Ok();
        }

        value = Some(Range<Percent>::fromStartEnd(toComputed(width), toComputed(val.expect())));

        return Ok();
    }
};

// https://www.w3.org/TR/css-fonts-4/#unicode-range-desc
export struct UnicodeRangeFontDescriptor {
    Vec<Range<Rune>> value;

    static Str name() { return "unicode-range"; }

    static auto initial() {
        return Vec<Range<Rune>>{};
    }

    void apply(FontFace& f) const {
        f.unicodeRange = value;
    }
};

// https://www.w3.org/TR/css-fonts-4/#font-rend-desc
export struct FontFeatureSettingsFontDescriptor {
    Vec<FontFeature> value;

    static Str name() { return "font-feature-settings"; }

    static Vec<FontFeature> initial() { return {}; }

    void apply(FontFace& f) const {
        f.features = value;
    }
};

// https://www.w3.org/TR/css-fonts-4/#font-rend-desc
export struct FontVariationSettingsFontDescriptor {
    Vec<FontVariation> value;

    static Str name() { return "font-variation-settings"; }

    static Vec<FontVariation> initial() { return {}; }

    void apply(FontFace& f) const {
        f.variations = value;
    }
};

// https://www.w3.org/TR/css-fonts-4/#font-named-instance
export struct FontNamedInstanceFontDescriptor {
    Opt<String> value = initial();

    static Str name() { return "font-named-instance"; }

    static Opt<String> initial() { return NONE; }

    void apply(FontFace&) const {
        // TODO
    }

    Res<> parse(Cursor<Css::Sst>& c) {
        if (c.skip(Css::Token::ident("auto"))) {
            value = NONE;
        } else {
            value = Some(try$(parseValue<String>(c)));
        }

        return Ok();
    }
};

// https://www.w3.org/TR/css-fonts-4/#font-display-desc
export struct FontDisplayFontDescriptor {
    FontDisplay value = initial();

    static Str name() { return "font-display"; }

    static FontDisplay initial() { return FontDisplay::AUTO; }

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
    Opt<Css::PurePercentage> value = initial();

    static Str name() { return "ascent-override"; }

    // NOTE: normal is NONE
    static Opt<Css::PurePercentage> initial() { return NONE; }

    void apply(FontFace& f) const {
        f.ascentOverride = value.map([](auto v) { return Percent{v.value()}; });
    }

    Res<> parse(Cursor<Css::Sst>& c) {
        if (c.skip(Css::Token::ident("normal"))) {
            value = NONE;
        } else {
            value = Some(try$(parseValue<Css::PurePercentage>(c)));
        }

        return Ok();
    }
};

// https://www.w3.org/TR/css-fonts-4/#font-metrics-override-desc
export struct DescentOverrideStyleProp {
    Opt<Css::PurePercentage> value = initial();

    static Str name() { return "descent-override"; }

    // NOTE: normal is NONE
    static Opt<Css::PurePercentage> initial() { return NONE; }

    void apply(FontFace& f) const {
        f.descentOverride = value.map([](auto v) { return Percent{v.value()}; });
    }

    Res<> parse(Cursor<Css::Sst>& c) {
        if (c.skip(Css::Token::ident("normal"))) {
            value = NONE;
        } else {
            value = Some(try$(parseValue<Css::PurePercentage>(c)));
        }

        return Ok();
    }
};

// https://www.w3.org/TR/css-fonts-4/#font-metrics-override-desc
export struct LineGapOverrideFontDescriptor {
    Opt<Css::PurePercentage> value = initial();

    static Str name() { return "line-gap-override"; }

    static Opt<Css::PurePercentage> initial() { return NONE; }

    void apply(FontFace& f) const {
        f.lineGapOverride = value.map([](auto v) { return Percent{v.value()}; });
    }

    Res<> parse(Cursor<Css::Sst>& c) {
        if (c.skip(Css::Token::ident("normal"))) {
            value = NONE;
        } else {
            value = Some(try$(parseValue<Css::PurePercentage>(c)));
        }

        return Ok();
    }
};

// https://www.w3.org/TR/css-fonts-5/#size-adjust-desc
export struct SizeAdjustFontDescriptor {
    Css::PurePercentage value = initial();

    static Str name() { return "size-adjust"; }

    static Css::PurePercentage initial() { return Css::percent(100); }

    void apply(FontFace& f) const {
        f.sizeAdjust = Some(Percent{value.value()});
    }

    Res<> parse(Cursor<Css::Sst>& c) {
        value = try$(parseValue<Css::PurePercentage>(c));
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
