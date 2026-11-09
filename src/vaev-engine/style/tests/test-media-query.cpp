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
    .type = Keywords::SCREEN,
    .width = 1920_px,
    .height = 1080_px,
    .aspectRatio = 16.0 / 9.0,
    .orientation = Style::Keywords::LANDSCAPE,

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

test$("logical-and") {
    auto query = MediaQuery::combineAnd(
        TypeFeature{Keywords::SCREEN},
        WidthFeature::min(Css::PureLength{Css::UnitValue{1920, Css::UnitValue::Unit::PX}})
    );

    assert$(query.match(TEST_MEDIA));

    return Ok();
}

test$("logical-or") {
    auto query = MediaQuery::combineOr(
        TypeFeature{Keywords::SCREEN},
        WidthFeature::min(Css::PureLength{Css::UnitValue{1920, Css::UnitValue::Unit::PX}})
    );

    assert$(query.match(TEST_MEDIA));

    return Ok();
}

test$("logical-not") {
    auto query = MediaQuery::negate(TypeFeature{Keywords::PRINT});

    assert$(query.match(TEST_MEDIA));

    return Ok();
}

test$("logical-only") {
    auto query = MediaQuery::only(TypeFeature{Keywords::SCREEN});

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
