module;

#include <karm/macros>

export module Vaev.Engine:values.length;

import Karm.Core;
import Karm.Math;

import :css;
import :values.base;
import :values.resolved;

using namespace Karm;

namespace Vaev {

// 6. MARK: Distance Units: the <length> type
// https://drafts.csswg.org/css-values/#lengths

export struct [[gnu::packed]] Length {
    enum struct Unit : u8 {
#define LENGTH(NAME, ...) NAME,
#include "defs/lengths.inc"

#undef LENGTH

        _LEN,
    };

    using enum Unit;

    f64 _val = 0;
    Unit _unit = Unit::PX;

    constexpr f64 val() const {
        return _val;
    }

    constexpr Unit unit() const {
        return _unit;
    }

    constexpr Length() = default;

    constexpr Length(f64 val, Unit unit)
        : _val(val), _unit(unit) {}

    constexpr Length(Au val)
        : _val(val) {}

    constexpr bool isAbsolute() const {
        switch (_unit) {
        case Unit::CM:
        case Unit::MM:
        case Unit::Q:
        case Unit::IN:
        case Unit::PT:
        case Unit::PC:
        case Unit::PX:
            return true;
        default:
            return false;
        }
    }

    constexpr bool isFontRelative() const {
        switch (_unit) {
        case Unit::EM:
        case Unit::REM:
        case Unit::EX:
        case Unit::REX:
        case Unit::CAP:
        case Unit::RCAP:
        case Unit::CH:
        case Unit::RCH:
        case Unit::IC:
        case Unit::RIC:
        case Unit::LH:
        case Unit::RLH:
            return true;

        default:
            return false;
        }
    }

    constexpr bool isViewportRelative() const {
        switch (_unit) {
        case Unit::VW:
        case Unit::SVW:
        case Unit::LVW:
        case Unit::DVW:
        case Unit::VH:
        case Unit::SVH:
        case Unit::LVH:
        case Unit::DVH:
        case Unit::VI:
        case Unit::SVI:
        case Unit::LVI:
        case Unit::DVI:
        case Unit::VB:
        case Unit::SVB:
        case Unit::LVB:
        case Unit::DVB:
        case Unit::VMIN:
        case Unit::SVMIN:
        case Unit::LVMIN:
        case Unit::DVMIN:
        case Unit::VMAX:
        case Unit::SVMAX:
        case Unit::LVMAX:
        case Unit::DVMAX:
            return true;
        default:
            return false;
        }
    }

    constexpr bool isRelative() const {
        return not isAbsolute();
    }

    constexpr bool operator==(Length const& other) const {
        return _val == other._val and _unit == other._unit;
    }

    void repr(Io::Emit& e) const {
        e("{}{}", _val, _unit);
    }
};

export template <>
struct _Resolved<Length> {
    using Type = Au;
};

export template <>
struct ValueTraits<Length> : DefaultValueTraits<Length> {
    using Computed = Au;

    static Computed compute(Length const val, ComputationContext& ctx) {
        switch (val.unit()) {
            // https://drafts.csswg.org/css-values/#font-relative-lengths
        case Length::EM:
            return Au(val.val() * ctx.font.unwrap().fontSize());

        case Length::REM:
            return Au(val.val() * ctx.rootFont.unwrap().fontSize());

        case Length::EX:
            return Au(val.val() * ctx.font.unwrap().xHeight());

        case Length::REX:
            return Au(val.val() * ctx.rootFont.unwrap().xHeight());

        case Length::CAP:
            return Au(val.val() * ctx.font.unwrap().capHeight());

        case Length::RCAP:
            return Au(val.val() * ctx.rootFont.unwrap().capHeight());

        case Length::CH:
            return Au(val.val() * ctx.font.unwrap().zeroAdvance());

        case Length::RCH:
            return Au(val.val() * ctx.rootFont.unwrap().zeroAdvance());

        case Length::IC:
            return Au(val.val() * ctx.font.unwrap().zeroAdvance());

        case Length::RIC:
            return Au(val.val() * ctx.rootFont.unwrap().zeroAdvance());

        case Length::LH:
            return Au(val.val() * ctx.font.unwrap().lineHeight());

        case Length::RLH:
            return Au(val.val() * ctx.rootFont.unwrap().lineHeight());

            // https://drafts.csswg.org/css-values/#viewport-relative-lengths

            // https://drafts.csswg.org/css-values/#vw
            // Equal to 1% of the width of current viewport.
        case Length::VW:
        case Length::LVW:
            return val.val() * ctx.viewport.large.width / 100;

        case Length::SVW:
            return val.val() * ctx.viewport.small.width / 100;

        case Length::DVW:
            return val.val() * ctx.viewport.dynamic.width / 100;

            // https://drafts.csswg.org/css-values/#vh
            // Equal to 1% of the height of current viewport.
        case Length::VH:
        case Length::LVH:
            return val.val() * ctx.viewport.large.height / 100;

        case Length::SVH:
            return val.val() * ctx.viewport.small.height / 100;

        case Length::DVH:
            return val.val() * ctx.viewport.dynamic.height / 100;

            // https://drafts.csswg.org/css-values/#vi
            // Equal to 1% of the size of the viewport in the box’s inline axis.
        case Length::VI:
        case Length::LVI:
            if (ctx.writingMode == _WritingMode::HORIZONTAL_TB) {
                return val.val() * ctx.viewport.large.width / 100;
            } else {
                return val.val() * ctx.viewport.large.height / 100;
            }

        case Length::SVI:
            if (ctx.writingMode == _WritingMode::HORIZONTAL_TB) {
                return val.val() * ctx.viewport.small.width / 100;
            } else {
                return val.val() * ctx.viewport.small.height / 100;
            }

        case Length::DVI:
            if (ctx.writingMode == _WritingMode::HORIZONTAL_TB) {
                return val.val() * ctx.viewport.dynamic.width / 100;
            } else {
                return val.val() * ctx.viewport.dynamic.height / 100;
            }

            // https://drafts.csswg.org/css-values/#vb
            // Equal to 1% of the size of the viewport in the box’s block axis.
        case Length::VB:
        case Length::LVB:
            if (ctx.writingMode == _WritingMode::HORIZONTAL_TB) {
                return val.val() * ctx.viewport.large.width / 100;
            } else {
                return val.val() * ctx.viewport.large.height / 100;
            }

        case Length::SVB:
            if (ctx.writingMode == _WritingMode::HORIZONTAL_TB) {
                return val.val() * ctx.viewport.small.width / 100;
            } else {
                return val.val() * ctx.viewport.small.height / 100;
            }

        case Length::DVB:
            if (ctx.writingMode == _WritingMode::HORIZONTAL_TB) {
                return val.val() * ctx.viewport.dynamic.width / 100;
            } else {
                return val.val() * ctx.viewport.dynamic.height / 100;
            }

            // https://drafts.csswg.org/css-values/#vmin
            // Equal to the smaller of vw and vh.
        case Length::VMIN:
        case Length::LVMIN:
            return min(
                computeValue(Length(val.val(), Length::VW), ctx),
                computeValue(Length(val.val(), Length::VH), ctx)
            );

        case Length::SVMIN:
            return min(
                computeValue(Length(val.val(), Length::SVW), ctx),
                computeValue(Length(val.val(), Length::SVH), ctx)
            );

        case Length::DVMIN:
            return min(
                computeValue(Length(val.val(), Length::DVW), ctx),
                computeValue(Length(val.val(), Length::DVH), ctx)
            );

        // https://drafts.csswg.org/css-values/#vmax
        // Equal to the larger of vw and vh.
        case Length::VMAX:
        case Length::LVMAX:
            return max(
                computeValue(Length(val.val(), Length::VW), ctx),
                computeValue(Length(val.val(), Length::VH), ctx)
            );

        case Length::DVMAX:
            return max(
                computeValue(Length(val.val(), Length::DVW), ctx),
                computeValue(Length(val.val(), Length::DVH), ctx)
            );

        case Length::SVMAX:
            return max(
                computeValue(Length(val.val(), Length::SVW), ctx),
                computeValue(Length(val.val(), Length::SVH), ctx)
            );

            // https://drafts.csswg.org/css-values/#absolute-lengths
        case Length::CM:
            return Au(val.val() * 96 / 2.54);

        case Length::MM:
            return Au(val.val() * 96 / 25.4);

        case Length::Q:
            return Au(val.val() * 96 / 101.6);

        case Length::IN:
            return Au(val.val() * 96);

        case Length::PT:
            return Au(val.val() * 96 / 72.0);

        case Length::PC:
            return Au(val.val() * 96 / 6.0);

        case Length::PX:
            return Au(val.val());

        default:
            panic("invalid unit");
        }
    }

    static Res<Length::Unit> _parseLengthUnit(Str unit) {
#define LENGTH(NAME, ...)      \
    if (eqCi(unit, #NAME ""s)) \
        return Ok(Length::Unit::NAME);
#include "defs/lengths.inc"

#undef LENGTH

        return Error::invalidData("unknown length unit");
    }

    static Res<Length> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c.peek() == Css::Token::DIMENSION) {
            Io::SScan scan = c->token.data.str();
            auto value = Io::atof(scan, {.allowExp = false}).unwrapOr(0.0);
            auto unit = try$(_parseLengthUnit(scan.remStr()));
            c.next();

            return Ok(Length{value, unit});
        }

        if (c.skip(Css::Token::number("0"))) {
            return Ok(Length{0.0, Length::Unit::PX});
        }

        return Error::invalidData("expected length");
    }
};

} // namespace Vaev
