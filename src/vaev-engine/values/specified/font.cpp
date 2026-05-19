module;

#include <karm/macros>

export module Vaev.Engine:values.specified.font;

import Karm.Core;
import Karm.Ref;
import Karm.Gfx;

import :css;
import :values.specified.angle;
import :values.specified.base;
import :values.specified.percentageMix;
import :values.specified.keywords;
import :values.specified.primitives;
import :values.computed;
import :values.common;

namespace Vaev::Experimental {

export enum struct FontDisplay {
    AUTO,
    BLOCK,
    SWAP,
    FALLBACK,
    OPTIONAL,

    _LEN,
};

// MARK: FontWidth -------------------------------------------------------------
// https://www.w3.org/TR/css-fonts-4/#propdef-font-width

using FontWidth = Union<
    Keywords::Normal,
    Percentage,
    Keywords::UltraCondensed,
    Keywords::ExtraCondensed,
    Keywords::Condensed,
    Keywords::SemiCondensed,
    Keywords::SemiExpanded,
    Keywords::Expanded,
    Keywords::ExtraExpanded,
    Keywords::UltraExpanded>;

export template <>
struct ValueTraits<FontWidth> {
    using ComputedType = Percentage;

    static ComputedType compute(FontWidth const& width, ComputationContext const&, Style::ComputedValues const&) {
        return width.visit(
            [](Percentage p) {
                return p;
            },
            [](Keywords::UltraCondensed) {
                return Percentage{50.0};
            },
            [](Keywords::ExtraCondensed) {
                return Percentage{62.5};
            },
            [](Keywords::Condensed) {
                return Percentage{75.0};
            },
            [](Keywords::SemiCondensed) {
                return Percentage{87.5};
            },
            [](Keywords::Normal) {
                return Percentage{100.0};
            },
            [](Keywords::SemiExpanded) {
                return Percentage{112.5};
            },
            [](Keywords::Expanded) {
                return Percentage{125.0};
            },
            [](Keywords::ExtraExpanded) {
                return Percentage{150.0};
            },
            [](Keywords::UltraExpanded) {
                return Percentage{200.0};
            }
        );
    }

    static FontWidth fromComputed(ComputedType const& computed) {
        return computed;
    }

    static Res<FontWidth> parse(Cursor<Css::Sst>& c) {
        return parseOneOf<FontWidth>(c);
    }
};

// MARK: FontStyle -------------------------------------------------------------
// https://drafts.csswg.org/css-fonts-4/#propdef-font-style

using FontStyle = _FontStyle<Angle>;

export template <>
struct ValueTraits<Pair<Keywords::Oblique, Angle>> {
    static Res<Pair<Keywords::Oblique, Angle>> parse(Cursor<Css::Sst>& c) {
        auto oblique = try$(parseValue<Keywords::Oblique>(c));
        auto angle = try$(parseValue<Angle>(c));

        return Ok(Pair{oblique, angle});
    }
};

export template <>
struct ValueTraits<FontStyle> {
    using ComputedType = _FontStyle<Degree>;

    static ComputedType compute(FontStyle const& style, ComputationContext const& ctx, Style::ComputedValues const& computedValues) {
        return style.visit(
            [&](Pair<Keywords::Oblique, Angle> const& pair) -> ComputedType {
                return Pair{Keywords::OBLIQUE, computeValue(pair.v1, ctx, computedValues)};
            },
            [](auto&& other) -> ComputedType {
                return other;
            }
        );
    }

    static FontStyle fromComputed(ComputedType const& computed) {
        return computed.visit(
            [&](Pair<Keywords::Oblique, Degree> const& pair) -> FontStyle {
                return Pair{Keywords::OBLIQUE, valueFromComputed<Angle>(pair.v1)};
            },
            [](auto&& other) -> FontStyle {
                return other;
            }
        );
    }

    static Res<FontStyle> parse(Cursor<Css::Sst>& c) {
        return parseOneOf<FontStyle>(c);
    }
};

// MARK: FontWeight
// https://www.w3.org/TR/css-fonts-4/#font-weight-absolute-values

export using FontWeight = Union<Number, Keywords::Normal, Keywords::Bold, Keywords::Bolder, Keywords::Lighter>;

export template <>
struct ValueTraits<FontWeight> {
    using ComputedType = Number;

    static ComputedType compute(FontWeight const& weight, ComputationContext const&, Style::ComputedValues const& computedValues) {
        return weight.visit(
            [](Number number) {
                return number;
            },
            [](Keywords::Normal) {
                return 400.0;
            },
            [](Keywords::Bold) {
                return 700.0;
            },

            // https://drafts.csswg.org/css-fonts/#relative-weights
            [&](Keywords::Bolder) {
                Number w = computedValues.font->weight;

                if (w < 350.0)
                    return 400.0;

                if (w < 550.0)
                    return 700.0;

                if (w < 900.0)
                    return 900.0;

                return w;
            },

            // https://drafts.csswg.org/css-fonts/#relative-weights
            [&](Keywords::Lighter) {
                Number w = computedValues.font->weight;

                if (w < 100.0)
                    return w;

                if (w < 550.0)
                    return 100.0;

                if (w < 750.0)
                    return 400.0;

                return 700.0;
            }
        );
    }

    static FontWeight fromComputed(ComputedType const& computed) {
        return computed;
    }

    static Res<FontWeight> parse(Cursor<Css::Sst>& c) {
        return parseOneOf<FontWeight>(c);
    }
};

// MARK: FontSize --------------------------------------------------------------
// https://www.w3.org/TR/css-fonts-4/#font-size-prop

// TODO: Keywords::Math
export using FontSize = FlatUnion<
    Keywords::XxSmall,
    Keywords::XSmall,
    Keywords::Small,
    Keywords::Medium,
    Keywords::Large,
    Keywords::XLarge,
    Keywords::XxLarge,
    Keywords::XxxLarge,
    Keywords::Larger,
    Keywords::Smaller,
    LengthPercentage>;

export template <>
struct ValueTraits<FontSize> {
    using ComputedType = Px;

    static ComputedType compute(FontSize const& size, ComputationContext const& ctx, Style::ComputedValues const& computedValues) {
        return size.visit(
            [&](Keywords::XxSmall) {
                return ctx.userFontSize * 3.0 / 5.0;
            },
            [&](Keywords::XSmall) {
                return ctx.userFontSize * 3.0 / 4.0;
            },
            [&](Keywords::Small) {
                return ctx.userFontSize * 8.0 / 9.0;
            },
            [&](Keywords::Medium) {
                return ctx.userFontSize;
            },
            [&](Keywords::Large) {
                return ctx.userFontSize * 6.0 / 5.0;
            },
            [&](Keywords::XLarge) {
                return ctx.userFontSize * 3.0 / 2.0;
            },
            [&](Keywords::XxLarge) {
                return ctx.userFontSize * 2.0;
            },
            [&](Keywords::XxxLarge) {
                return ctx.userFontSize * 3.0;
            },
            [&](Keywords::Larger) {
                return computedValues.font->size * 1.25;
            },
            [&](Keywords::Smaller) {
                return computedValues.font->size * 0.875;
            },
            [&](Length const& length) {
                return computeValue(length, ctx, computedValues);
            },
            [&](Percentage const& percentage) {
                return computedValues.font->size * percentage.value() / 100.0;
            }
        );
    }

    static FontSize fromComputed(ComputedType const& computed) {
        return Length(computed.value(), Length::PX);
    }

    static Res<FontSize> parse(Cursor<Css::Sst>& c) {
        return parseOneOf<FontSize>(c);
    }
};

// MARK: FontFamily ------------------------------------------------------------

using FontFamilies = Vec<Symbol>;

export template <>
struct ValueTraits<FontFamilies> {
    using ComputedType = Vec<Symbol>;

    static ComputedType compute(FontFamilies const& families, ComputationContext const&, Style::ComputedValues const&) {
        return families;
    }

    static FontFamilies fromComputed(ComputedType const& computed) {
        return computed;
    }

    static Res<FontFamilies> parse(Cursor<Css::Sst>& c) {
        FontFamilies value{};
        eatWhitespace(c);
        while (not c.ended()) {
            value.pushBack(try$(_parseFamily(c)));

            eatWhitespace(c);
            c.skip(Css::Token::comma());
            eatWhitespace(c);
        }
        return Ok(value);
    }

    static Res<Symbol> _parseFamily(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c.peek() == Css::Token::STRING)
            return Ok(Symbol::from(try$(parseValue<String>(c))));

        StringBuilder familyStrBuilder;
        usize amountOfIdents = 0;

        if (c.peek() == Css::Token::IDENT) {
            while (not c.ended() and c.peek() == Css::Token::IDENT) {
                if (++amountOfIdents > 1)
                    familyStrBuilder.append(' ');
                familyStrBuilder.append(c.next().token.data);
                eatWhitespace(c);
            }
        } else {
            return Error::invalidData("expected font family name");
        }

        return Ok(Symbol::from(familyStrBuilder.take()));
    }
};

} // namespace Vaev::Experimental
