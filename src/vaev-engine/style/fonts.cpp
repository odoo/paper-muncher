module;

#include <karm/macros>

export module Vaev.Engine:style.fonts;

import Karm.Core;
import Karm.Ref;
import Karm.Gfx;

import :values;
import :css.values;
import :style.values;
import :css;

namespace Vaev::Style {

// MARK: FontFace --------------------------------------------------------------

export struct FontFace {
    Symbol family = ""_sym;

    Vec<Css::FontSource> sources;

    Union<None, Css::FontStyle, Range<Deg>> style = Css::FontStyle{Css::FontStyle::NORMAL};

    Opt<Range<Gfx::FontWeight>> weight = Some(Range<Gfx::FontWeight>::emptyAt(Gfx::FontWeight::REGULAR));
    Opt<Range<Percent>> width = Some(Range<Percent>::emptyAt(Percent{Css::FontWidth{Css::FontWidth::NORMAL}.val().value()}));

    Vec<Range<Rune>> unicodeRange;

    Vec<Css::FontFeature> features;
    Vec<Css::FontVariation> variations;

    Opt<Percent> ascentOverride = NONE;
    Opt<Percent> descentOverride = NONE;
    Opt<Percent> lineGapOverride = NONE;
    Opt<Percent> sizeAdjust = NONE;

    Css::FontDisplay display = Css::FontDisplay::AUTO;

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
        if (style.is<Css::FontStyle>())
            resolvedStyle = style.expect<Css::FontStyle>().val;

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

// https://www.w3.org/TR/css-fonts-4

// MARK: font-family
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

// MARK: src
// https://www.w3.org/TR/css-fonts-4/#src-desc
export struct SrcFontDescriptor {
    Vec<Css::FontSource> value;

    static Str name() { return "src"; }

    static auto initial() {
        return Vec<Css::FontSource>{};
    }

    void apply(FontFace& f) {
        f.sources = value;
    }

    Res<> parse(Cursor<Css::Sst>& c) {
        Vec<Css::FontSource> fontSrcs;
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

// MARK: font-style
// https://www.w3.org/TR/css-fonts-4/#font-style-desc
export struct FontStyleFontDescriptor {
    Union<None, Css::FontStyle, Range<Deg>> value;

    static Str name() { return "font-style"; }

    static auto initial() { return Css::FontStyle::NORMAL; }

    void apply(FontFace& f) const {
        f.style = value;
    }

    Res<> parse(Cursor<Css::Sst>& c) {
        if (c.skip(Css::Token::ident("auto"))) {
            value = NONE;
            return Ok();
        }

        auto style = try$(parseValue<Css::FontStyle>(c));
        if (style != Css::FontStyle::OBLIQUE) {
            value = style;
            return Ok();
        }

        auto val = parseValue<Css::PureAngle>(c);
        if (not val) {
            value = style;
            return Ok();
        }

        value = Range<Deg>::fromStartEnd(toComputed(style.obliqueAngle), toComputed(val.expect()));
        return Ok();
    }
};

// MARK: font-weight
// https://www.w3.org/TR/css-fonts-4/#font-weight-desc
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

        auto weight = try$(parseValue<Gfx::FontWeight>(c));

        auto val = parseValue<Gfx::FontWeight>(c);
        if (not val) {
            value = Some(Range<Gfx::FontWeight>::emptyAt(weight));
            return Ok();
        }

        value = Some(Range<Gfx::FontWeight>::fromStartEnd(weight, val.expect()));

        return Ok();
    }
};

// MARK: font-width
// https://www.w3.org/TR/css-fonts-4/#font-width-prop
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
            value = Some(Range<Percent>::emptyAt(Percent{width.val().value()}));
            return Ok();
        }

        value = Some(Range<Percent>::fromStartEnd(Percent{width.val().value()}, Percent{val.expect().val().value()}));

        return Ok();
    }
};

// MARK: unicode-range
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

// MARK: font-feature-settings
// https://www.w3.org/TR/css-fonts-4/#font-rend-desc
export struct FontFeatureSettingsFontDescriptor {
    Vec<Css::FontFeature> value;

    static Str name() { return "font-feature-settings"; }

    static Vec<Css::FontFeature> initial() { return {}; }

    void apply(FontFace& f) const {
        f.features = value;
    }
};

// MARK: font-variation-settingsl
// https://www.w3.org/TR/css-fonts-4/#font-rend-desc
export struct FontVariationSettingsFontDescriptor {
    Vec<Css::FontVariation> value;

    static Str name() { return "font-variation-settings"; }

    static Vec<Css::FontVariation> initial() { return {}; }

    void apply(FontFace& f) const {
        f.variations = value;
    }
};

// MARK: font-named-instance
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

// MARK: font-display
// https://www.w3.org/TR/css-fonts-4/#font-display-desc
export struct FontDisplayFontDescriptor {
    Css::FontDisplay value = initial();

    static Str name() { return "font-display"; }

    static Css::FontDisplay initial() { return Css::FontDisplay::AUTO; }

    void apply(FontFace& f) const {
        f.display = value;
    }

    Res<> parse(Cursor<Css::Sst>& c) {
        if (c.skip(Css::Token::ident("auto"))) {
            value = Css::FontDisplay::AUTO;
        } else if (c.skip(Css::Token::ident("block"))) {
            value = Css::FontDisplay::BLOCK;
        } else if (c.skip(Css::Token::ident("swap"))) {
            value = Css::FontDisplay::SWAP;
        } else if (c.skip(Css::Token::ident("fallback"))) {
            value = Css::FontDisplay::FALLBACK;
        } else if (c.skip(Css::Token::ident("optional"))) {
            value = Css::FontDisplay::OPTIONAL;
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

// MARK: descent-override
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

// MARK: line-gap-override
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

// MARK: size-adjust
// https://www.w3.org/TR/css-fonts-5/#size-adjust-desc
export struct SizeAdjustFontDescriptor {
    Css::PurePercentage value = initial();

    static Str name() { return "size-adjust"; }

    static Css::PurePercentage initial() { return Css::PurePercentage{100}; }

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
