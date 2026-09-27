#include <karm/test>

import Karm.Print;
import Vaev.Engine;
import Karm.Math;

using namespace Karm;
using namespace Karm::Math::Literals;

namespace Vaev::Style::Tests {

static Media const TEST_MEDIA = {
    .type = MediaType::SCREEN,
    .width = 1920_au,
    .height = 1080_au,
    .aspectRatio = 16.0 / 9.0,
    .orientation = Print::Orientation::LANDSCAPE,

    .resolution = Resolution::fromDpi(96),
    .scan = Scan::PROGRESSIVE,
    .grid = false,
    .update = Update::NONE,
    .overflowBlock = OverflowBlock::NONE,
    .overflowInline = OverflowInline::NONE,

    .color = 8,
    .colorIndex = 256,
    .monochrome = 0,
    .colorGamut = ColorGamut::SRGB,
    .pointer = Pointer::NONE,
    .hover = Hover::NONE,
    .anyPointer = Pointer::FINE,
    .anyHover = Hover::HOVER,

    .prefersReducedMotion = ReducedMotion::REDUCE,
    .prefersReducedTransparency = ReducedTransparency::NO_PREFERENCE,
    .prefersContrast = Contrast::LESS,
    .forcedColors = Colors::NONE,
    .prefersColorScheme = ColorScheme::LIGHT,
    .prefersReducedData = ReducedData::REDUCE,

    .deviceWidth = 1920_au,
    .deviceHeight = 1080_au,
    .deviceAspectRatio = 16.0 / 9.0,
};

test$("feature-type") {
    assert$(TypeFeature{MediaType::SCREEN}.match(TEST_MEDIA));

    return Ok();
}

test$("feature-width") {
    assert$(WidthFeature::min(1000_au).match(TEST_MEDIA));

    assert$(WidthFeature::max(2000_au).match(TEST_MEDIA));

    assert$(WidthFeature::exact(1920_au).match(TEST_MEDIA));

    return Ok();
}

test$("feature-height") {
    assert$(HeightFeature::min(1000_au).match(TEST_MEDIA));

    assert$(HeightFeature::max(2000_au).match(TEST_MEDIA));

    assert$(HeightFeature::exact(1080_au).match(TEST_MEDIA));

    return Ok();
}

test$("feature-aspect-ratio") {
    assert$(AspectRatioFeature::min(16.0 / 9.0).match(TEST_MEDIA));

    assert$(AspectRatioFeature::max(16.0 / 9.0).match(TEST_MEDIA));

    assert$(AspectRatioFeature::exact(16.0 / 9.0).match(TEST_MEDIA));

    return Ok();
}

test$("feature-orientation") {
    assert$(OrientationFeature{Print::Orientation::LANDSCAPE}.match(TEST_MEDIA));

    return Ok();
}

test$("feature-resolution") {
    assert$(ResolutionFeature::min(Resolution::fromDpi(96)).match(TEST_MEDIA));

    assert$(ResolutionFeature::max(Resolution::fromDpi(96)).match(TEST_MEDIA));

    assert$(ResolutionFeature::exact(Resolution::fromDpi(96)).match(TEST_MEDIA));

    return Ok();
}

test$("feature-scan") {
    assert$(ScanFeature{Scan::PROGRESSIVE}.match(TEST_MEDIA));

    return Ok();
}

test$("feature-grid") {
    assert$(GridFeature{false}.match(TEST_MEDIA));

    return Ok();
}

test$("feature-update") {
    assert$(UpdateFeature{Update::NONE}.match(TEST_MEDIA));

    return Ok();
}

test$("feature-overflow-block") {
    assert$(OverflowBlockFeature{OverflowBlock::NONE}.match(TEST_MEDIA));

    return Ok();
}

test$("feature-overflow-inline") {
    assert$(OverflowInlineFeature{OverflowInline::NONE}.match(TEST_MEDIA));

    return Ok();
}

test$("feature-color") {
    assert$(ColorFeature::min(8).match(TEST_MEDIA));

    assert$(ColorFeature::max(8).match(TEST_MEDIA));

    assert$(ColorFeature::exact(8).match(TEST_MEDIA));

    return Ok();
}

test$("feature-color-index") {
    assert$(ColorIndexFeature::min(256).match(TEST_MEDIA));

    assert$(ColorIndexFeature::max(256).match(TEST_MEDIA));

    assert$(ColorIndexFeature::exact(256).match(TEST_MEDIA));

    return Ok();
}

test$("feature-monochrome") {
    assert$(MonochromeFeature::min(0).match(TEST_MEDIA));

    assert$(MonochromeFeature::max(0).match(TEST_MEDIA));

    assert$(MonochromeFeature::exact(0).match(TEST_MEDIA));

    return Ok();
}

test$("feature-color-gamut") {
    assert$(ColorGamutFeature{ColorGamut::SRGB}.match(TEST_MEDIA));

    return Ok();
}

test$("feature-pointer") {
    assert$(PointerFeature{Pointer::NONE}.match(TEST_MEDIA));

    return Ok();
}

test$("feature-hover") {
    assert$(HoverFeature{Hover::NONE}.match(TEST_MEDIA));

    return Ok();
}

test$("feature-any-pointer") {
    assert$(AnyPointerFeature{Pointer::FINE}.match(TEST_MEDIA));

    return Ok();
}

test$("feature-any-hover") {
    assert$(AnyHoverFeature{Hover::HOVER}.match(TEST_MEDIA));

    return Ok();
}

test$("feature-prefers-reduced-motion") {
    assert$(PrefersReducedMotionFeature{ReducedMotion::REDUCE}.match(TEST_MEDIA));

    return Ok();
}

test$("feature-prefers-reduced-transparency") {
    assert$(PrefersReducedTransparencyFeature{ReducedTransparency::NO_PREFERENCE}.match(TEST_MEDIA));

    return Ok();
}

test$("feature-prefers-contrast") {
    assert$(PrefersContrastFeature{Contrast::LESS}.match(TEST_MEDIA));

    return Ok();
}

test$("feature-forced-colors") {
    assert$(ForcedColorsFeature{Colors::NONE}.match(TEST_MEDIA));

    return Ok();
}

test$("feature-prefers-color-scheme") {
    assert$(PrefersColorSchemeFeature{ColorScheme::LIGHT}.match(TEST_MEDIA));

    return Ok();
}

test$("feature-prefers-reduced-data") {
    assert$(PrefersReducedDataFeature{ReducedData::REDUCE}.match(TEST_MEDIA));

    return Ok();
}

test$("feature-device-width") {
    assert$(DeviceWidthFeature::min(1920_au).match(TEST_MEDIA));

    assert$(DeviceWidthFeature::max(1920_au).match(TEST_MEDIA));

    assert$(DeviceWidthFeature::exact(1920_au).match(TEST_MEDIA));

    return Ok();
}

test$("feature-device-height") {
    assert$(DeviceHeightFeature::min(1080_au).match(TEST_MEDIA));

    assert$(DeviceHeightFeature::max(1080_au).match(TEST_MEDIA));

    assert$(DeviceHeightFeature::exact(1080_au).match(TEST_MEDIA));

    return Ok();
}

test$("feature-device-aspect-ratio") {
    assert$(DeviceAspectRatioFeature::min(16.0 / 9.0).match(TEST_MEDIA));

    assert$(DeviceAspectRatioFeature::max(16.0 / 9.0).match(TEST_MEDIA));

    assert$(DeviceAspectRatioFeature::exact(16.0 / 9.0).match(TEST_MEDIA));

    return Ok();
}

} // namespace Vaev::Style::Tests
