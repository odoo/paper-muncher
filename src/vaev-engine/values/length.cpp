module;

#include <karm/macros>

export module Vaev.Engine:values.length;

import Karm.Core;
import Karm.Math;

import :css;
import :values.base;
import :values.resolved;
import :values.computed;

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
        : _val(val.cast<f64>()) {}

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

    constexpr std::partial_ordering operator<=>(Length const& other) const {
        if (_unit != other._unit)
            return std::partial_ordering::unordered;
        return _val <=> other._val;
    }

    Au toComputed(ComputationContext& ctx) const {
        switch (_unit) {

        // https://drafts.csswg.org/css-values/#font-relative-lengths
        case Unit::EM:
            return Au::fromFloatNearest(_val * ctx.font.unwrap().fontSize());

        case Unit::REM:
            return Au::fromFloatNearest(_val * ctx.rootFont.unwrap().fontSize());

        case Unit::EX:
            return Au::fromFloatNearest(_val * ctx.font.unwrap().xHeight());

        case Unit::REX:
            return Au::fromFloatNearest(_val * ctx.rootFont.unwrap().xHeight());

        case Unit::CAP:
            return Au::fromFloatNearest(_val * ctx.font.unwrap().capHeight());

        case Unit::RCAP:
            return Au::fromFloatNearest(_val * ctx.rootFont.unwrap().capHeight());

        case Unit::CH:
            return Au::fromFloatNearest(_val * ctx.font.unwrap().zeroAdvance());

        case Unit::RCH:
            return Au::fromFloatNearest(_val * ctx.rootFont.unwrap().zeroAdvance());

        case Unit::IC:
            return Au::fromFloatNearest(_val * ctx.font.unwrap().zeroAdvance());

        case Unit::RIC:
            return Au::fromFloatNearest(_val * ctx.rootFont.unwrap().zeroAdvance());

        case Unit::LH:
            return Au::fromFloatNearest(_val * ctx.font.unwrap().lineHeight());

        case Unit::RLH:
            return Au::fromFloatNearest(_val * ctx.rootFont.unwrap().lineHeight());

        // https://drafts.csswg.org/css-values/#viewport-relative-lengths

        // https://drafts.csswg.org/css-values/#vw
        // Equal to 1% of the width of current viewport.
        case Unit::VW:
        case Unit::LVW:
            return Au::fromFloatNearest(_val * ctx.viewport.large.width.cast<f64>() / 100);

        case Unit::SVW:
            return Au::fromFloatNearest(_val * ctx.viewport.small.width.cast<f64>() / 100);

        case Unit::DVW:
            return Au::fromFloatNearest(_val * ctx.viewport.dynamic.width.cast<f64>() / 100);

        // https://drafts.csswg.org/css-values/#vh
        // Equal to 1% of the height of current viewport.
        case Unit::VH:
        case Unit::LVH:
            return Au::fromFloatNearest(_val * ctx.viewport.large.height.cast<f64>() / 100);

        case Unit::SVH:
            return Au::fromFloatNearest(_val * ctx.viewport.small.height.cast<f64>() / 100);

        case Unit::DVH:
            return Au::fromFloatNearest(_val * ctx.viewport.dynamic.height.cast<f64>() / 100);

        // https://drafts.csswg.org/css-values/#vi
        // Equal to 1% of the size of the viewport in the box’s inline axis.
        case Unit::VI:
        case Unit::LVI:
            if (ctx.writingMode == WritingMode::HORIZONTAL_TB) {
                return Au::fromFloatNearest(_val * ctx.viewport.large.width.cast<f64>() / 100);
            } else {
                return Au::fromFloatNearest(_val * ctx.viewport.large.height.cast<f64>() / 100);
            }

        case Unit::SVI:
            if (ctx.writingMode == WritingMode::HORIZONTAL_TB) {
                return Au::fromFloatNearest(_val * ctx.viewport.small.width.cast<f64>() / 100);
            } else {
                return Au::fromFloatNearest(_val * ctx.viewport.small.height.cast<f64>() / 100);
            }

        case Unit::DVI:
            if (ctx.writingMode == WritingMode::HORIZONTAL_TB) {
                return Au::fromFloatNearest(_val * ctx.viewport.dynamic.width.cast<f64>() / 100);
            } else {
                return Au::fromFloatNearest(_val * ctx.viewport.dynamic.height.cast<f64>() / 100);
            }

        // https://drafts.csswg.org/css-values/#vb
        // Equal to 1% of the size of the viewport in the box’s block axis.
        case Unit::VB:
        case Unit::LVB:
            if (ctx.writingMode == WritingMode::HORIZONTAL_TB) {
                return Au::fromFloatNearest(_val * ctx.viewport.large.width.cast<f64>() / 100);
            } else {
                return Au::fromFloatNearest(_val * ctx.viewport.large.height.cast<f64>() / 100);
            }

        case Unit::SVB:
            if (ctx.writingMode == WritingMode::HORIZONTAL_TB) {
                return Au::fromFloatNearest(_val * ctx.viewport.small.width.cast<f64>() / 100);
            } else {
                return Au::fromFloatNearest(_val * ctx.viewport.small.height.cast<f64>() / 100);
            }

        case Unit::DVB:
            if (ctx.writingMode == WritingMode::HORIZONTAL_TB) {
                return Au::fromFloatNearest(_val * ctx.viewport.dynamic.width.cast<f64>() / 100);
            } else {
                return Au::fromFloatNearest(_val * ctx.viewport.dynamic.height.cast<f64>() / 100);
            }

        // https://drafts.csswg.org/css-values/#vmin
        // Equal to the smaller of vw and vh.
        case Unit::VMIN:
        case Unit::LVMIN:
            return min(
                Length(_val, Unit::VW).toComputed(ctx),
                Length(_val, Unit::VH).toComputed(ctx)
            );

        case Unit::SVMIN:
            return min(
                Length(_val, Unit::SVW).toComputed(ctx),
                Length(_val, Unit::SVH).toComputed(ctx)
            );

        case Unit::DVMIN:
            return min(
                Length(_val, Unit::DVW).toComputed(ctx),
                Length(_val, Unit::DVH).toComputed(ctx)
            );

        // https://drafts.csswg.org/css-values/#vmax
        // Equal to the larger of vw and vh.
        case Unit::VMAX:
        case Unit::LVMAX:
            return max(
                Length(_val, Unit::VW).toComputed(ctx),
                Length(_val, Unit::VH).toComputed(ctx)
            );

        case Unit::DVMAX:
            return max(
                Length(_val, Unit::DVW).toComputed(ctx),
                Length(_val, Unit::DVH).toComputed(ctx)
            );

        case Unit::SVMAX:
            return max(
                Length(_val, Unit::SVW).toComputed(ctx),
                Length(_val, Unit::SVH).toComputed(ctx)
            );

        // https://drafts.csswg.org/css-values/#absolute-lengths
        case Unit::CM:
            return Au::fromFloatNearest(_val * 96 / 2.54);

        case Unit::MM:
            return Au::fromFloatNearest(_val * 96 / 25.4);

        case Unit::Q:
            return Au::fromFloatNearest(_val * 96 / 101.6);

        case Unit::IN:
            return Au::fromFloatNearest(_val * 96);

        case Unit::PT:
            return Au::fromFloatNearest(_val * 96 / 72.0);

        case Unit::PC:
            return Au::fromFloatNearest(_val * 96 / 6.0);

        case Unit::PX:
            return Au::fromFloatNearest(_val);

        default:
            panic("invalid unit");
        }
    }

    void repr(Io::Emit& e) const {
        e("{}{}", _val, _unit);
    }
};

export template <>
struct _Computed<Length> {
    using Type = Au;
};

export template <>
struct _Resolved<Length> {
    using Type = Au;
};

export template <>
struct ValueParser<Length> {
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
