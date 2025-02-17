#include "color.h"

#include "karm-logger/logger.h"

namespace Vaev {

Opt<Color> parseNamedColor(Str name) {

#define COLOR(ID, NAME, ...)   \
    if (eqCi(name, #NAME ""s)) \
        return ID;
#include "defs/colors.inc"
#undef COLOR

    return NONE;
}

Opt<SystemColor> parseSystemColor(Str name) {
#define COLOR(ID, NAME, ...) \
    if (name == #NAME)       \
        return SystemColor::ID;
#include "defs/system-colors.inc"
#undef COLOR

    return NONE;
}

static Array<Gfx::Color, static_cast<usize>(SystemColor::_LEN)> SYSTEM_COLOR = {
#define COLOR(NAME, _, VALUE) Gfx::Color::fromHex(VALUE),
#include "defs/system-colors.inc"
#undef COLOR
};

Gfx::Color resolve(Color c, Gfx::Color currentColor) {
    switch (c.type) {
    case Color::Type::SRGB:
        return c._color.unwrap<Gfx::Color>();

    case Color::Type::SYSTEM:
        return SYSTEM_COLOR[static_cast<usize>(c._color.unwrap<SystemColor>())];

    case Color::Type::CURRENT:
        return currentColor;

    case Color::Type::MIX: {
        // https://developer.mozilla.org/en-US/docs/Web/CSS/color_value/color-mix#percentage
        Color::ColorMix& cm = c._color.unwrap<Color::ColorMix>();

        auto resolvedColorA = resolve(*cm.colorA, currentColor);
        auto resolvedColorB = resolve(*cm.colorB, currentColor);

        i8 percA, percB;

        if (cm.percA == NONE and cm.percB == NONE) {
            percA = percB = 50;
        } else if (cm.percA == NONE and cm.percB) {
            percB = cm.percB.unwrap();
            percA = 100 - percB;
        } else if (cm.percA and cm.percB == NONE) {
            percA = cm.percA.unwrap();
            percB = 100 - percA;
        } else {
            percA = cm.percA.unwrap();
            percB = cm.percB.unwrap();
        }

        if (percA == percB and percA == 0) {
            logWarn("cannot mix colors when both have zero percentages");
            return Gfx::WHITE;
        }

        f64 normPercB = percB;

        if (percA + percB != 100)
            normPercB = normPercB / (percA + percB);
        else
            normPercB /= 100;

        auto finalColor = cm.colorSpace.interpolate(resolvedColorA, resolvedColorB, normPercB);
        if (percA + percB < 100)
            finalColor.alpha = percA + percB;
        return finalColor;
    }

    default:
        panic("Invalid color type");
    }
}

} // namespace Vaev
