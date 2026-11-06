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

test$("logical-and") {
    auto query = MediaQuery::combineAnd(
        TypeFeature{Css::MediaType::SCREEN},
        WidthFeature::min(Css::PureLength::fromPx(1920))
    );

    assert$(query.match(TEST_MEDIA));

    return Ok();
}

test$("logical-or") {
    auto query = MediaQuery::combineOr(
        TypeFeature{Css::MediaType::SCREEN},
        WidthFeature::min(Css::PureLength::fromPx(1920))
    );

    assert$(query.match(TEST_MEDIA));

    return Ok();
}

test$("logical-not") {
    auto query = MediaQuery::negate(TypeFeature{Css::MediaType::PRINT});

    assert$(query.match(TEST_MEDIA));

    return Ok();
}

test$("logical-only") {
    auto query = MediaQuery::only(TypeFeature{Css::MediaType::SCREEN});

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
