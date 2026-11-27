#include <karm/test>

import Vaev.Engine;
import Karm.Font;
import Karm.Logger;

using namespace Karm;
using namespace Karm::Literals;

namespace Vaev::Style::Tests {

test$("test-specificity-selector-list") {
    auto selector = try$(parseSelector(".a, .b#x, .c.d.e.f"));

    StyleRule rule{
        .selectors = selector,
        .declarations = {}
    };

    {
        Dom::Element elZeroMatches{Html::DIV_TAG};

        assert$(rule.match(MatchSubject{elZeroMatches}) == NONE);
    }
    {
        Dom::Element elOneMatch{Html::DIV_TAG};
        elOneMatch.classList.add("a");

        assert$(rule.match(MatchSubject{elOneMatch}) == Specificity(0, 1, 0));
    }
    {
        Dom::Element elOneMatch{Html::DIV_TAG};
        elOneMatch.classList.add("c");
        elOneMatch.classList.add("d");
        elOneMatch.classList.add("e");
        elOneMatch.classList.add("f");

        assert$(rule.match(MatchSubject{elOneMatch}) == Specificity(0, 4, 0));
    }
    {
        Dom::Element elAnotherMatch{Html::DIV_TAG};
        elAnotherMatch.classList.add("b");
        elAnotherMatch.setAttribute(Html::ID_ATTR, "x"s);

        assert$(rule.match(MatchSubject{elAnotherMatch}) == Specificity(1, 1, 0));
    }
    {
        Dom::Element twoMatches{Html::DIV_TAG};

        twoMatches.classList.add("b");
        twoMatches.setAttribute(Html::ID_ATTR, "x"s);
        twoMatches.classList.add("a");

        assert$(rule.match(MatchSubject{twoMatches}) == Specificity(1, 1, 0));
    }

    return Ok();
}

test$("test-specificity-is-where") {
    Dom::Element el{Html::DIV_TAG};
    el.classList.add("a");
    el.setAttribute(Html::ID_ATTR, "x"s);

    // https://www.w3.org/TR/selectors-4/#zero-matches
    StyleRule where{.selectors = try$(parseSelector("div:where(#x, .a)")), .declarations = {}};
    assert$(where.match(MatchSubject{el}) == Specificity(0, 0, 1));

    // https://www.w3.org/TR/selectors-4/#matches
    StyleRule is{.selectors = try$(parseSelector("div:is(.a, #y)")), .declarations = {}};
    assert$(is.match(MatchSubject{el}) == Specificity(1, 0, 1));

    StyleRule whereNoMatch{.selectors = try$(parseSelector("div:where(.b)")), .declarations = {}};
    assert$(whereNoMatch.match(MatchSubject{el}) == NONE);

    return Ok();
}

test$("test-specificity-simple-selector-class") {
    auto selector = try$(parseSelector(".a"));
    Specificity specificity{Style::specificity(first(selector))};

    assert$(specificity == Specificity(0, 1, 0));

    return Ok();
}

test$("test-specificity-simple-selector-id") {
    auto selector = try$(parseSelector("#id"));
    Specificity specificity{Style::specificity(first(selector))};

    assert$(specificity == Specificity(1, 0, 0));

    return Ok();
}

test$("test-specificity-simple-selector-type") {
    auto selector = try$(parseSelector("div"));
    Specificity specificity{Style::specificity(first(selector))};

    assert$(specificity == Specificity(0, 0, 1));

    return Ok();
}

test$("test-specificity-simple-selector-universal") {
    auto selector = try$(parseSelector("*"));
    Specificity specificity{Style::specificity(first(selector))};

    assert$(specificity == Specificity(0, 0, 0));

    return Ok();
}

test$("test-specificity-simple-selector-attr") {
    auto selector = try$(parseSelector("[a=b]"));
    Specificity specificity{Style::specificity(first(selector))};

    assert$(specificity == Specificity(0, 1, 0));

    return Ok();
}

test$("test-specificity-simple-selector-pseudo") {
    auto selector = try$(parseSelector(":hover"));
    Specificity specificity{Style::specificity(first(selector))};

    assert$(specificity == Specificity(0, 1, 0));

    return Ok();
}

test$("test-specificity-compound-selector-1") {
    auto selector = try$(parseSelector(".a.b.c.d"));
    Specificity specificity{Style::specificity(first(selector))};

    assert$(specificity == Specificity(0, 4, 0));

    return Ok();
}

test$("test-specificity-compound-selector-2") {
    auto selector = try$(parseSelector("#id.a"));
    Specificity specificity{Style::specificity(first(selector))};

    assert$(specificity == Specificity(1, 1, 0));

    return Ok();
}

test$("test-specificity-compound-selector-repeated-simple-selector") {
    auto selector = try$(parseSelector(".a.a.a"));
    Specificity specificity{Style::specificity(first(selector))};

    assert$(specificity == Specificity(0, 3, 0));

    return Ok();
}

test$("test-specificity-compound-selector-one-of-each-simple-1") {
    auto selector = try$(parseSelector("div#id.a[x=y]:hover"));
    Specificity specificity{Style::specificity(first(selector))};

    assert$(specificity == Specificity(1, 3, 1));

    return Ok();
}

test$("test-specificity-compound-selector-one-of-each-simple-2") {
    auto selector = try$(parseSelector("*#id.a[x=y]:hover"));
    Specificity specificity{Style::specificity(first(selector))};

    assert$(specificity == Specificity(1, 3, 0));

    return Ok();
}

test$("test-specificity-pseudo-not") {
    auto selector = try$(parseSelector("button:not([DISABLED])"));
    Specificity specificity{Style::specificity(first(selector))};

    assert$(specificity == Specificity(0, 1, 1));

    return Ok();
}

test$("test-specificity-descendant-combinator") {
    auto selector = try$(parseSelector("div button .a #x"));
    Specificity specificity{Style::specificity(first(selector))};

    assert$(specificity == Specificity(1, 1, 2));

    return Ok();
}

test$("test-specificity-child-combinator") {
    auto selector = try$(parseSelector("div > button > .a > #x"));
    Specificity specificity{Style::specificity(first(selector))};

    assert$(specificity == Specificity(1, 1, 2));

    return Ok();
}

test$("test-specificity-next-sibling-combinator") {
    auto selector = try$(parseSelector("div + button + .a + #x"));
    Specificity specificity{Style::specificity(first(selector))};

    assert$(specificity == Specificity(1, 1, 2));

    return Ok();
}

test$("test-specificity-subsequent-sibling-combinator") {
    auto selector = try$(parseSelector("div ~ button ~ .a ~ #x"));
    Specificity specificity{Style::specificity(first(selector))};

    assert$(specificity == Specificity(1, 1, 2));

    return Ok();
}

test$("test-specificity-mixed-combinators") {
    auto selector = try$(parseSelector("div button ~ .a > #x + #y"));
    Specificity specificity{Style::specificity(first(selector))};

    assert$(specificity == Specificity(2, 1, 2));

    return Ok();
}

test$("test-specificity-not-with-child-list") {
    auto selector = try$(parseSelector(":not(.a, #b)"));
    Specificity specificity{Style::specificity(first(selector))};

    assert$(specificity == Specificity(1, 0, 0));

    return Ok();
}

test$("test-specificity-nth-child-with-sub-selector") {
    auto selector = try$(parseSelector(":nth-child(n of .a, #b)"));
    Specificity specificity{Style::specificity(first(selector))};

    assert$(specificity == Specificity(1, 1, 0));

    return Ok();
}

test$("test-specificity-nth-last-child-with-sub-selector") {
    auto selector = try$(parseSelector(":nth-last-child(n of .a, #b)"));
    Specificity specificity{Style::specificity(first(selector))};

    assert$(specificity == Specificity(1, 1, 0));

    return Ok();
}

test$("test-specificity-pseudo-elements") {
    assertEq$(Style::specificity(first(try$(parseSelector("::before")))), Specificity(0, 0, 1));
    assertEq$(Style::specificity(first(try$(parseSelector("div::before:hover")))), Specificity(0, 1, 2));
    return Ok();
}

test$("test-specificity-logical-combinations") {
    assertEq$(Style::specificity(first(try$(parseSelector(":where(#a) .b")))), Specificity(0, 1, 0));
    assertEq$(Style::specificity(first(try$(parseSelector(":is(#a, .b) .c")))), Specificity(1, 1, 0));
    assertEq$(Style::specificity(first(try$(parseSelector(":has(> #a, .b)")))), Specificity(1, 0, 0));
    return Ok();
}

test$("test-specificity-tree-structural") {
    assertEq$(Style::specificity(first(try$(parseSelector(":first-child")))), Specificity(0, 1, 0));
    assertEq$(Style::specificity(first(try$(parseSelector("li:nth-of-type(2)")))), Specificity(0, 1, 1));
    return Ok();
}

} // namespace Vaev::Style::Tests
