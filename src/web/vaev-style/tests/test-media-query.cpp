#include <karm-test/macros.h>
#include <vaev-style/media.h>

namespace Vaev::Style::Tests {

static Media const TEST_MEDIA = {
    .type = MediaType::SCREEN,
    .width = Px(1920),
    .height = Px(1080),
    .aspectRatio = 16.0 / 9.0,
    .orientation = Orientation::LANDSCAPE,

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
};

test$("logical-and") {
    auto query = MediaQuery::combineAnd(
        TypeFeature{MediaType::SCREEN},
        WidthFeature::min(1920_px)
    );

    expect$(query.match(TEST_MEDIA));

    return Ok();
}

test$("logical-or") {
    auto query = MediaQuery::combineOr(
        TypeFeature{MediaType::SCREEN},
        WidthFeature::min(1920_px)
    );

    expect$(query.match(TEST_MEDIA));

    return Ok();
}

test$("logical-not") {
    auto query = MediaQuery::negate(TypeFeature{MediaType::PRINT});

    expect$(query.match(TEST_MEDIA));

    return Ok();
}

test$("logical-only") {
    auto query = MediaQuery::only(TypeFeature{MediaType::SCREEN});

    expect$(query.match(TEST_MEDIA));

    return Ok();
}

} // namespace Vaev::Style::Tests