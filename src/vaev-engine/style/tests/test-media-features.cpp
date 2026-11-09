#include <karm/test>

import Karm.Print;
import Vaev.Engine;
import Karm.Math;

using namespace Karm;
using namespace Karm::Math::Literals;
using namespace Vaev::Literals;

namespace Vaev::Style::Tests {

static Media const TEST_MEDIA = {
    .type = Keywords::SCREEN,
    .width = 1920_px,
    .height = 1080_px,
    .aspectRatio = 16.0 / 9.0,
    .orientation = Print::Orientation::LANDSCAPE,

    .resolution = 1_dppx,
    .scan = Keywords::PROGRESSIVE,
    .grid = false,
    .update = Keywords::NONE,
    .overflowBlock = Keywords::NONE,
    .overflowInline = Keywords::NONE,

    .color = 8,
    .colorIndex = 256,
    .monochrome = 0,
    .colorGamut = Keywords::SRGB,
    .pointer = Keywords::NONE,
    .hover = Keywords::NONE,
    .anyPointer = Keywords::FINE,
    .anyHover = Keywords::HOVER,

    .prefersReducedMotion = Keywords::REDUCE,
    .prefersReducedTransparency = Keywords::NO_PREFERENCE,
    .prefersContrast = Keywords::LESS,
    .forcedColors = Keywords::NONE,
    .prefersColorScheme = Keywords::LIGHT,
    .prefersReducedData = Keywords::REDUCE,

    .deviceWidth = 1920_px,
    .deviceHeight = 1080_px,
    .deviceAspectRatio = 16.0 / 9.0,
};

test$("feature-type") {
    assert$(TypeFeature{Keywords::SCREEN}.match(TEST_MEDIA));

    return Ok();
}

test$("feature-width") {
    assert$(WidthFeature::min(Css::PureLength{UnitValue{1000, UnitValue::Unit::PX}}).match(TEST_MEDIA));

    assert$(WidthFeature::max(Css::PureLength{UnitValue{2000, UnitValue::Unit::PX}}).match(TEST_MEDIA));

    assert$(WidthFeature::exact(Css::PureLength{UnitValue{1920, UnitValue::Unit::PX}}).match(TEST_MEDIA));

    return Ok();
}

test$("feature-height") {
    assert$(HeightFeature::min(Css::PureLength{UnitValue{1000, UnitValue::Unit::PX}}).match(TEST_MEDIA));

    assert$(HeightFeature::max(Css::PureLength{UnitValue{2000, UnitValue::Unit::PX}}).match(TEST_MEDIA));

    assert$(HeightFeature::exact(Css::PureLength{UnitValue{1080, UnitValue::Unit::PX}}).match(TEST_MEDIA));

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
    assert$(ResolutionFeature::min(Css::PureResolution{UnitValue{1, UnitValue::Unit::DPPX}}).match(TEST_MEDIA));

    assert$(ResolutionFeature::max(Css::PureResolution{UnitValue{1, UnitValue::Unit::DPPX}}).match(TEST_MEDIA));

    assert$(ResolutionFeature::exact(Css::PureResolution{UnitValue{1, UnitValue::Unit::DPPX}}).match(TEST_MEDIA));

    return Ok();
}

test$("feature-scan") {
    assert$(ScanFeature{Keywords::PROGRESSIVE}.match(TEST_MEDIA));

    return Ok();
}

test$("feature-grid") {
    assert$(GridFeature{false}.match(TEST_MEDIA));

    return Ok();
}

test$("feature-update") {
    assert$(UpdateFeature{Keywords::NONE}.match(TEST_MEDIA));

    return Ok();
}

test$("feature-overflow-block") {
    assert$(OverflowBlockFeature{Keywords::NONE}.match(TEST_MEDIA));

    return Ok();
}

test$("feature-overflow-inline") {
    assert$(OverflowInlineFeature{Keywords::NONE}.match(TEST_MEDIA));

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
    assert$(ColorGamutFeature{Keywords::SRGB}.match(TEST_MEDIA));

    return Ok();
}

test$("feature-pointer") {
    assert$(PointerFeature{Keywords::NONE}.match(TEST_MEDIA));

    return Ok();
}

test$("feature-hover") {
    assert$(HoverFeature{Keywords::NONE}.match(TEST_MEDIA));

    return Ok();
}

test$("feature-any-pointer") {
    assert$(AnyPointerFeature{Keywords::FINE}.match(TEST_MEDIA));

    return Ok();
}

test$("feature-any-hover") {
    assert$(AnyHoverFeature{Keywords::HOVER}.match(TEST_MEDIA));

    return Ok();
}

test$("feature-prefers-reduced-motion") {
    assert$(PrefersReducedMotionFeature{Keywords::REDUCE}.match(TEST_MEDIA));

    return Ok();
}

test$("feature-prefers-reduced-transparency") {
    assert$(PrefersReducedTransparencyFeature{Keywords::NO_PREFERENCE}.match(TEST_MEDIA));

    return Ok();
}

test$("feature-prefers-contrast") {
    assert$(PrefersContrastFeature{Keywords::LESS}.match(TEST_MEDIA));

    return Ok();
}

test$("feature-forced-colors") {
    assert$(ForcedColorsFeature{Keywords::NONE}.match(TEST_MEDIA));

    return Ok();
}

test$("feature-prefers-color-scheme") {
    assert$(PrefersColorSchemeFeature{Keywords::LIGHT}.match(TEST_MEDIA));

    return Ok();
}

test$("feature-prefers-reduced-data") {
    assert$(PrefersReducedDataFeature{Keywords::REDUCE}.match(TEST_MEDIA));

    return Ok();
}

test$("feature-device-width") {
    assert$(DeviceWidthFeature::min(Css::PureLength{UnitValue{1920, UnitValue::Unit::PX}}).match(TEST_MEDIA));

    assert$(DeviceWidthFeature::max(Css::PureLength{UnitValue{1920, UnitValue::Unit::PX}}).match(TEST_MEDIA));

    assert$(DeviceWidthFeature::exact(Css::PureLength{UnitValue{1920, UnitValue::Unit::PX}}).match(TEST_MEDIA));

    return Ok();
}

test$("feature-device-height") {
    assert$(DeviceHeightFeature::min(Css::PureLength{UnitValue{1080, UnitValue::Unit::PX}}).match(TEST_MEDIA));

    assert$(DeviceHeightFeature::max(Css::PureLength{UnitValue{1080, UnitValue::Unit::PX}}).match(TEST_MEDIA));

    assert$(DeviceHeightFeature::exact(Css::PureLength{UnitValue{1080, UnitValue::Unit::PX}}).match(TEST_MEDIA));

    return Ok();
}

test$("feature-device-aspect-ratio") {
    assert$(DeviceAspectRatioFeature::min(16.0 / 9.0).match(TEST_MEDIA));

    assert$(DeviceAspectRatioFeature::max(16.0 / 9.0).match(TEST_MEDIA));

    assert$(DeviceAspectRatioFeature::exact(16.0 / 9.0).match(TEST_MEDIA));

    return Ok();
}

} // namespace Vaev::Style::Tests
