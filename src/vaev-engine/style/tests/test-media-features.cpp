#include <karm/test>

import Karm.Print;
import Vaev.Engine;
import Karm.Math;

using namespace Karm;
using namespace Karm::Math::Literals;
using namespace Vaev::Literals;

namespace Vaev::Style::Tests {

static Media const TEST_MEDIA = {
    .type = Style::MediaType::SCREEN,
    .width = 1920_px,
    .height = 1080_px,
    .aspectRatio = 16.0 / 9.0,
    .orientation = Print::Orientation::LANDSCAPE,

    .resolution = 1_dppx,
    .scan = Style::Scan::PROGRESSIVE,
    .grid = false,
    .update = Style::Update::NONE,
    .overflowBlock = Style::OverflowBlock::NONE,
    .overflowInline = Style::OverflowInline::NONE,

    .color = 8,
    .colorIndex = 256,
    .monochrome = 0,
    .colorGamut = Style::ColorGamut::SRGB,
    .pointer = Style::Pointer::NONE,
    .hover = Style::Hover::NONE,
    .anyPointer = Style::Pointer::FINE,
    .anyHover = Style::Hover::HOVER,

    .prefersReducedMotion = Style::ReducedMotion::REDUCE,
    .prefersReducedTransparency = Style::ReducedTransparency::NO_PREFERENCE,
    .prefersContrast = Style::Contrast::LESS,
    .forcedColors = Style::Colors::NONE,
    .prefersColorScheme = Style::ColorScheme::LIGHT,
    .prefersReducedData = Style::ReducedData::REDUCE,

    .deviceWidth = 1920_px,
    .deviceHeight = 1080_px,
    .deviceAspectRatio = 16.0 / 9.0,
};

test$("feature-type") {
    assert$(TypeFeature{Style::MediaType::SCREEN}.match(TEST_MEDIA));

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
    assert$(ScanFeature{Style::Scan::PROGRESSIVE}.match(TEST_MEDIA));

    return Ok();
}

test$("feature-grid") {
    assert$(GridFeature{false}.match(TEST_MEDIA));

    return Ok();
}

test$("feature-update") {
    assert$(UpdateFeature{Style::Update::NONE}.match(TEST_MEDIA));

    return Ok();
}

test$("feature-overflow-block") {
    assert$(OverflowBlockFeature{Style::OverflowBlock::NONE}.match(TEST_MEDIA));

    return Ok();
}

test$("feature-overflow-inline") {
    assert$(OverflowInlineFeature{Style::OverflowInline::NONE}.match(TEST_MEDIA));

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
    assert$(ColorGamutFeature{Style::ColorGamut::SRGB}.match(TEST_MEDIA));

    return Ok();
}

test$("feature-pointer") {
    assert$(PointerFeature{Style::Pointer::NONE}.match(TEST_MEDIA));

    return Ok();
}

test$("feature-hover") {
    assert$(HoverFeature{Style::Hover::NONE}.match(TEST_MEDIA));

    return Ok();
}

test$("feature-any-pointer") {
    assert$(AnyPointerFeature{Style::Pointer::FINE}.match(TEST_MEDIA));

    return Ok();
}

test$("feature-any-hover") {
    assert$(AnyHoverFeature{Style::Hover::HOVER}.match(TEST_MEDIA));

    return Ok();
}

test$("feature-prefers-reduced-motion") {
    assert$(PrefersReducedMotionFeature{Style::ReducedMotion::REDUCE}.match(TEST_MEDIA));

    return Ok();
}

test$("feature-prefers-reduced-transparency") {
    assert$(PrefersReducedTransparencyFeature{Style::ReducedTransparency::NO_PREFERENCE}.match(TEST_MEDIA));

    return Ok();
}

test$("feature-prefers-contrast") {
    assert$(PrefersContrastFeature{Style::Contrast::LESS}.match(TEST_MEDIA));

    return Ok();
}

test$("feature-forced-colors") {
    assert$(ForcedColorsFeature{Style::Colors::NONE}.match(TEST_MEDIA));

    return Ok();
}

test$("feature-prefers-color-scheme") {
    assert$(PrefersColorSchemeFeature{Style::ColorScheme::LIGHT}.match(TEST_MEDIA));

    return Ok();
}

test$("feature-prefers-reduced-data") {
    assert$(PrefersReducedDataFeature{Style::ReducedData::REDUCE}.match(TEST_MEDIA));

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
