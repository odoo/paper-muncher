#include <karm/test>

import Karm.Print;
import Karm.Math;
import Karm.Diag;
import Vaev.Engine;

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

test$("logical-and") {
    auto query = MediaQuery::combineAnd(
        TypeFeature{Style::MediaType::SCREEN},
        WidthFeature::min(Css::PureLength::fromPx(1920))
    );

    assert$(query.match(TEST_MEDIA));

    return Ok();
}

test$("logical-or") {
    auto query = MediaQuery::combineOr(
        TypeFeature{Style::MediaType::SCREEN},
        WidthFeature::min(Css::PureLength::fromPx(1920))
    );

    assert$(query.match(TEST_MEDIA));

    return Ok();
}

test$("logical-not") {
    auto query = MediaQuery::negate(TypeFeature{Style::MediaType::PRINT});

    assert$(query.match(TEST_MEDIA));

    return Ok();
}

test$("logical-only") {
    auto query = MediaQuery::only(TypeFeature{Style::MediaType::SCREEN});

    assert$(query.match(TEST_MEDIA));

    return Ok();
}

test$("media-type-all") {
    auto lex = Css::Lexer{"all"};
    auto diags = Diag::Collector::ignore();
    auto [val, _] = consumeDeclarationValue(lex, diags);
    Cursor<Css::Sst> sst{val};
    auto query = parseMediaQuery(sst);

    assert$(query.match(TEST_MEDIA));

    return Ok();
}

} // namespace Vaev::Style::Tests
