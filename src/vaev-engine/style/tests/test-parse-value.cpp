#include <karm/test>

import Vaev.Engine;
import Karm.Math;
import Karm.Diag;

using namespace Karm;
using namespace Karm::Math::Literals;

namespace Vaev::Style::Tests {

test$("vaev-css-build-display") {
    auto testCase = [&](Str input, Css::Display expected) -> Res<> {
        auto lex = Css::Lexer{input};

        auto diags = Diag::Collector::ignore();
        auto [val, _] = consumeDeclarationValue(lex, diags);
        Cursor<Css::Sst> sst{val};
        auto res = parseValue<Css::Display>(sst);
        assert$(res);
        assertEq$(res.expect(), expected);
        return Ok();
    };

    // Inside
    try$(testCase("flow", Css::Display{Css::Display::FLOW, Css::Display::BLOCK}));
    try$(testCase("flow-root", Css::Display{Css::Display::FLOW_ROOT, Css::Display::BLOCK}));
    try$(testCase("flex", Css::Display{Css::Display::FLEX, Css::Display::BLOCK}));
    try$(testCase("grid", Css::Display{Css::Display::GRID, Css::Display::BLOCK}));
    try$(testCase("ruby", Css::Display{Css::Display::RUBY, Css::Display::BLOCK}));
    try$(testCase("table", Css::Display{Css::Display::TABLE, Css::Display::BLOCK}));
    try$(testCase("math", Css::Display{Css::Display::MATH, Css::Display::BLOCK}));

    // Outside
    try$(testCase("block", Css::Display{Css::Display::FLOW, Css::Display::BLOCK}));
    try$(testCase("inline", Css::Display{Css::Display::FLOW, Css::Display::INLINE}));
    try$(testCase("run-in", Css::Display{Css::Display::FLOW, Css::Display::RUN_IN}));

    // Default
    try$(testCase("block flow", Css::Display{Css::Display::FLOW, Css::Display::BLOCK}));
    try$(testCase("inline flow", Css::Display{Css::Display::FLOW, Css::Display::INLINE}));
    try$(testCase("run-in flow", Css::Display{Css::Display::FLOW, Css::Display::RUN_IN}));

    // list-item
    try$(testCase("run-in flow list-item", Css::Display{Css::Display::FLOW, Css::Display::RUN_IN}));

    // Internal
    try$(testCase("table-row-group", Css::Display::TABLE_ROW_GROUP));
    try$(testCase("table-header-group", Css::Display::TABLE_HEADER_GROUP));
    try$(testCase("table-footer-group", Css::Display::TABLE_FOOTER_GROUP));
    try$(testCase("table-row", Css::Display::TABLE_ROW));
    try$(testCase("table-cell", Css::Display::TABLE_CELL));
    try$(testCase("table-column-group", Css::Display::TABLE_COLUMN_GROUP));
    try$(testCase("table-column", Css::Display::TABLE_COLUMN));
    try$(testCase("table-caption", Css::Display::TABLE_CAPTION));
    try$(testCase("ruby-base", Css::Display::RUBY_BASE));
    try$(testCase("ruby-text", Css::Display::RUBY_TEXT));
    try$(testCase("ruby-base-container", Css::Display::RUBY_BASE_CONTAINER));
    try$(testCase("ruby-text-container", Css::Display::RUBY_TEXT_CONTAINER));

    // Box
    try$(testCase("contents", Css::Display::CONTENTS));
    try$(testCase("none", Css::Display::NONE));

    // Legacy
    try$(testCase("inline-block", Css::Display{Css::Display::FLOW_ROOT, Css::Display::INLINE}));
    try$(testCase("inline-table", Css::Display{Css::Display::TABLE, Css::Display::INLINE}));
    try$(testCase("inline-flex", Css::Display{Css::Display::FLEX, Css::Display::INLINE}));
    try$(testCase("inline-grid", Css::Display{Css::Display::GRID, Css::Display::INLINE}));

    return Ok();
}

test$("vaev-css-build-margin") {
    auto testCase = [&](Str input, MarginProperty::Value expected) -> Res<> {
        auto registry = defaultRegistry();
        auto res = registry.parseValue(Properties::MARGIN, input, {});
        assert$(res);

        auto prop = try$(res.take().cast<MarginProperty>());

        assertEq$(Io::format("{}", prop->_value), Io::format("{}", expected));

        return Ok();
    };

    try$(testCase(
        "30px;",
        MarginProperty::Value{Css::LengthPercentage::fromCanonical(30)}
    ));

    try$(testCase(
        "1px 2px;",
        MarginProperty::Value{
            Css::LengthPercentage::fromCanonical(1),
            Css::LengthPercentage::fromCanonical(2),
        }
    ));

    try$(testCase(
        "1px /* x */ 2px;",
        MarginProperty::Value{
            Css::LengthPercentage::fromCanonical(1),
            Css::LengthPercentage::fromCanonical(2),
        }
    ));

    try$(testCase(
        "1px 2px 3px;",
        MarginProperty::Value{
            Css::LengthPercentage::fromCanonical(1),
            Css::LengthPercentage::fromCanonical(2),
            Css::LengthPercentage::fromCanonical(3),
        }
    ));

    try$(testCase(
        "1px 2px 3px 4px;",
        MarginProperty::Value{
            Css::LengthPercentage::fromCanonical(1),
            Css::LengthPercentage::fromCanonical(2),
            Css::LengthPercentage::fromCanonical(3),
            Css::LengthPercentage::fromCanonical(4),
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
        return Css::LengthPercentage::fromCanonical(v);
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
