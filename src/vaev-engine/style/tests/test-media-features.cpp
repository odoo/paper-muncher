#include <karm/test>

import Karm.Print;
import Vaev.Engine;
import Karm.Math;

using namespace Karm;
using namespace Karm::Math::Literals;
using namespace Vaev::Literals;

namespace Vaev::Style::Tests {

static Media const TEST_MEDIA = {
    .type = Css::MediaType::SCREEN,
    .width = 1920_px,
    .height = 1080_px,
    .aspectRatio = 16.0 / 9.0,
    .orientation = Print::Orientation::LANDSCAPE,

    .resolution = 1_dppx,
    .scan = Css::Scan::PROGRESSIVE,
    .grid = false,
    .update = Css::Update::NONE,
    .overflowBlock = Css::OverflowBlock::NONE,
    .overflowInline = Css::OverflowInline::NONE,

    .color = 8,
    .colorIndex = 256,
    .monochrome = 0,
    .colorGamut = Css::ColorGamut::SRGB,
    .pointer = Css::Pointer::NONE,
    .hover = Css::Hover::NONE,
    .anyPointer = Css::Pointer::FINE,
    .anyHover = Css::Hover::HOVER,

    .prefersReducedMotion = Css::ReducedMotion::REDUCE,
    .prefersReducedTransparency = Css::ReducedTransparency::NO_PREFERENCE,
    .prefersContrast = Css::Contrast::LESS,
    .forcedColors = Css::Colors::NONE,
    .prefersColorScheme = Css::ColorScheme::LIGHT,
    .prefersReducedData = Css::ReducedData::REDUCE,

    .deviceWidth = 1920_px,
    .deviceHeight = 1080_px,
    .deviceAspectRatio = 16.0 / 9.0,
};

test$("feature-type") {
    assert$(TypeFeature{Css::MediaType::SCREEN}.match(TEST_MEDIA));

    return Ok();
}

test$("feature-width") {
    assert$(WidthFeature::min(Css::PureLength::fromPx(1000)).match(TEST_MEDIA));

    assert$(WidthFeature::max(Css::PureLength::fromPx(2000)).match(TEST_MEDIA));

    assert$(WidthFeature::exact(Css::PureLength::fromPx(1920)).match(TEST_MEDIA));

    return Ok();
}

test$("feature-height") {
    assert$(HeightFeature::min(Css::PureLength::fromPx(1000)).match(TEST_MEDIA));

    assert$(HeightFeature::max(Css::PureLength::fromPx(2000)).match(TEST_MEDIA));

    assert$(HeightFeature::exact(Css::PureLength::fromPx(1080)).match(TEST_MEDIA));

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
    assert$(ResolutionFeature::min(Css::PureResolution::fromDppx(1)).match(TEST_MEDIA));

    assert$(ResolutionFeature::max(Css::PureResolution::fromDppx(1)).match(TEST_MEDIA));

    assert$(ResolutionFeature::exact(Css::PureResolution::fromDppx(1)).match(TEST_MEDIA));

    return Ok();
}

test$("feature-scan") {
    assert$(ScanFeature{Css::Scan::PROGRESSIVE}.match(TEST_MEDIA));

    return Ok();
}

test$("feature-grid") {
    assert$(GridFeature{false}.match(TEST_MEDIA));

    return Ok();
}

test$("feature-update") {
    assert$(UpdateFeature{Css::Update::NONE}.match(TEST_MEDIA));

    return Ok();
}

test$("feature-overflow-block") {
    assert$(OverflowBlockFeature{Css::OverflowBlock::NONE}.match(TEST_MEDIA));

    return Ok();
}

test$("feature-overflow-inline") {
    assert$(OverflowInlineFeature{Css::OverflowInline::NONE}.match(TEST_MEDIA));

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
    assert$(ColorGamutFeature{Css::ColorGamut::SRGB}.match(TEST_MEDIA));

    return Ok();
}

test$("feature-pointer") {
    assert$(PointerFeature{Css::Pointer::NONE}.match(TEST_MEDIA));

    return Ok();
}

test$("feature-hover") {
    assert$(HoverFeature{Css::Hover::NONE}.match(TEST_MEDIA));

    return Ok();
}

test$("feature-any-pointer") {
    assert$(AnyPointerFeature{Css::Pointer::FINE}.match(TEST_MEDIA));

    return Ok();
}

test$("feature-any-hover") {
    assert$(AnyHoverFeature{Css::Hover::HOVER}.match(TEST_MEDIA));

    return Ok();
}

test$("feature-prefers-reduced-motion") {
    assert$(PrefersReducedMotionFeature{Css::ReducedMotion::REDUCE}.match(TEST_MEDIA));

    return Ok();
}

test$("feature-prefers-reduced-transparency") {
    assert$(PrefersReducedTransparencyFeature{Css::ReducedTransparency::NO_PREFERENCE}.match(TEST_MEDIA));

    return Ok();
}

test$("feature-prefers-contrast") {
    assert$(PrefersContrastFeature{Css::Contrast::LESS}.match(TEST_MEDIA));

    return Ok();
}

test$("feature-forced-colors") {
    assert$(ForcedColorsFeature{Css::Colors::NONE}.match(TEST_MEDIA));

    return Ok();
}

test$("feature-prefers-color-scheme") {
    assert$(PrefersColorSchemeFeature{Css::ColorScheme::LIGHT}.match(TEST_MEDIA));

    return Ok();
}

test$("feature-prefers-reduced-data") {
    assert$(PrefersReducedDataFeature{Css::ReducedData::REDUCE}.match(TEST_MEDIA));

    return Ok();
}

test$("feature-device-width") {
    assert$(DeviceWidthFeature::min(Css::PureLength::fromPx(1920)).match(TEST_MEDIA));

    assert$(DeviceWidthFeature::max(Css::PureLength::fromPx(1920)).match(TEST_MEDIA));

    assert$(DeviceWidthFeature::exact(Css::PureLength::fromPx(1920)).match(TEST_MEDIA));

    return Ok();
}

test$("feature-device-height") {
    assert$(DeviceHeightFeature::min(Css::PureLength::fromPx(1080)).match(TEST_MEDIA));

    assert$(DeviceHeightFeature::max(Css::PureLength::fromPx(1080)).match(TEST_MEDIA));

    assert$(DeviceHeightFeature::exact(Css::PureLength::fromPx(1080)).match(TEST_MEDIA));

    return Ok();
}

test$("feature-device-aspect-ratio") {
    assert$(DeviceAspectRatioFeature::min(16.0 / 9.0).match(TEST_MEDIA));

    assert$(DeviceAspectRatioFeature::max(16.0 / 9.0).match(TEST_MEDIA));

    assert$(DeviceAspectRatioFeature::exact(16.0 / 9.0).match(TEST_MEDIA));

    return Ok();
}

} // namespace Vaev::Style::Tests
