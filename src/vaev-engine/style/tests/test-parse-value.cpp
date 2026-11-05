#include <karm/test>

import Vaev.Engine;
import Karm.Math;
import Karm.Diag;

using namespace Karm;
using namespace Karm::Math::Literals;

namespace Vaev::Style::Tests {

test$("vaev-css-build-display") {
    auto testCase = [&](Str input, Display expected) -> Res<> {
        auto lex = Css::Lexer{input};

        auto diags = Diag::Collector::ignore();
        auto [val, _] = consumeDeclarationValue(lex, diags);
        Cursor<Css::Sst> sst{val};
        auto res = parseValue<Display>(sst);
        assert$(res);
        assertEq$(res.expect(), expected);
        return Ok();
    };

    // Inside
    try$(testCase("flow", Display{Display::FLOW, Display::BLOCK}));
    try$(testCase("flow-root", Display{Display::FLOW_ROOT, Display::BLOCK}));
    try$(testCase("flex", Display{Display::FLEX, Display::BLOCK}));
    try$(testCase("grid", Display{Display::GRID, Display::BLOCK}));
    try$(testCase("ruby", Display{Display::RUBY, Display::BLOCK}));
    try$(testCase("table", Display{Display::TABLE, Display::BLOCK}));
    try$(testCase("math", Display{Display::MATH, Display::BLOCK}));

    // Outside
    try$(testCase("block", Display{Display::FLOW, Display::BLOCK}));
    try$(testCase("inline", Display{Display::FLOW, Display::INLINE}));
    try$(testCase("run-in", Display{Display::FLOW, Display::RUN_IN}));

    // Default
    try$(testCase("block flow", Display{Display::FLOW, Display::BLOCK}));
    try$(testCase("inline flow", Display{Display::FLOW, Display::INLINE}));
    try$(testCase("run-in flow", Display{Display::FLOW, Display::RUN_IN}));

    // list-item
    try$(testCase("run-in flow list-item", Display{Display::FLOW, Display::RUN_IN}));

    // Internal
    try$(testCase("table-row-group", Display::TABLE_ROW_GROUP));
    try$(testCase("table-header-group", Display::TABLE_HEADER_GROUP));
    try$(testCase("table-footer-group", Display::TABLE_FOOTER_GROUP));
    try$(testCase("table-row", Display::TABLE_ROW));
    try$(testCase("table-cell", Display::TABLE_CELL));
    try$(testCase("table-column-group", Display::TABLE_COLUMN_GROUP));
    try$(testCase("table-column", Display::TABLE_COLUMN));
    try$(testCase("table-caption", Display::TABLE_CAPTION));
    try$(testCase("ruby-base", Display::RUBY_BASE));
    try$(testCase("ruby-text", Display::RUBY_TEXT));
    try$(testCase("ruby-base-container", Display::RUBY_BASE_CONTAINER));
    try$(testCase("ruby-text-container", Display::RUBY_TEXT_CONTAINER));

    // Box
    try$(testCase("contents", Display::CONTENTS));
    try$(testCase("none", Display::NONE));

    // Legacy
    try$(testCase("inline-block", Display{Display::FLOW_ROOT, Display::INLINE}));
    try$(testCase("inline-table", Display{Display::TABLE, Display::INLINE}));
    try$(testCase("inline-flex", Display{Display::FLEX, Display::INLINE}));
    try$(testCase("inline-grid", Display{Display::GRID, Display::INLINE}));

    return Ok();
}

test$("vaev-css-build-margin") {
    auto testCase = [&](Str input, Margins expected) -> Res<> {
        auto registry = defaultRegistry();
        auto res = registry.parseValue(Properties::MARGIN, input, {});
        assert$(res);

        auto prop = try$(res.take().cast<MarginProperty>());

        assertEq$(Io::format("{}", prop->_value), Io::format("{}", expected));

        return Ok();
    };

    try$(testCase(
        "30px;",
        Margins{LengthPercentage::fromCanonical(30)}
    ));

    try$(testCase(
        "1px 2px;",
        Margins{
            LengthPercentage::fromCanonical(1),
            LengthPercentage::fromCanonical(2),
        }
    ));

    try$(testCase(
        "1px /* x */ 2px;",
        Margins{
            LengthPercentage::fromCanonical(1),
            LengthPercentage::fromCanonical(2),
        }
    ));

    try$(testCase(
        "1px 2px 3px;",
        Margins{
            LengthPercentage::fromCanonical(1),
            LengthPercentage::fromCanonical(2),
            LengthPercentage::fromCanonical(3),
        }
    ));

    try$(testCase(
        "1px 2px 3px 4px;",
        Margins{
            LengthPercentage::fromCanonical(1),
            LengthPercentage::fromCanonical(2),
            LengthPercentage::fromCanonical(3),
            LengthPercentage::fromCanonical(4),
        }
    ));

    return Ok();
}

test$("vaev-css-build-logical-margin") {
    auto testCase = [&]<typename P>(Symbol const& name, Str input, LogicalMargins expected) -> Res<> {
        auto registry = defaultRegistry();
        auto res = registry.parseValue(name, input, {});
        assert$(res);

        auto prop = try$(res.take().cast<P>());

        assertEq$(Io::format("{}", prop->_value), Io::format("{}", expected));

        return Ok();
    };

    auto px = [](f64 v) {
        return LengthPercentage::fromCanonical(v);
    };

    try$(testCase.operator()<MarginInlineProperty>(Properties::MARGIN_INLINE, "1px;", {px(1), px(1)}));
    try$(testCase.operator()<MarginInlineProperty>(Properties::MARGIN_INLINE, "1px 2px;", {px(1), px(2)}));
    try$(testCase.operator()<MarginInlineProperty>(Properties::MARGIN_INLINE, "auto 2px;", {Keywords::AUTO, px(2)}));
    try$(testCase.operator()<MarginBlockProperty>(Properties::MARGIN_BLOCK, "1px;", {px(1), px(1)}));
    try$(testCase.operator()<MarginBlockProperty>(Properties::MARGIN_BLOCK, "1px 2px;", {px(1), px(2)}));

    return Ok();
}

test$("vaev-css-build-line-width-keywords") {
    auto registry = defaultRegistry();

    assert$(registry.parseValue(Properties::BORDER_BOTTOM_WIDTH, "thin", {}));
    assert$(registry.parseValue(Properties::BORDER_TOP, "thick solid red", {}));
    assert$(registry.parseValue(Properties::OUTLINE, "medium dashed blue", {}));

    // Each component of the shorthands may only appear once.
    assert$(not registry.parseValue(Properties::BORDER_TOP, "thin solid thick", {}));
    assert$(not registry.parseValue(Properties::OUTLINE, "thin outset thick", {}));

    return Ok();
}

test$("vaev-css-build-transform-zero-angle") {
    auto registry = defaultRegistry();

    // https://drafts.csswg.org/css-transforms/#funcdef-transform-rotate
    // rotate(), skew(), skewX() and skewY() accept <angle> | <zero>.
    assert$(registry.parseValue(Properties::TRANSFORM, "rotate(0)", {}));
    assert$(registry.parseValue(Properties::TRANSFORM, "skew(0, 0)", {}));
    assert$(registry.parseValue(Properties::TRANSFORM, "skewX(0) skewY(0)", {}));

    // Only zero may be unitless.
    assert$(not registry.parseValue(Properties::TRANSFORM, "rotate(1)", {}));

    return Ok();
}

} // namespace Vaev::Style::Tests
