#include <karm/test>

import Vaev.Engine;

using namespace Karm;
using namespace Karm::Literals;

namespace Vaev::Style::Tests {

static Res<String> _roundtrip(Str input, NamespaceScope const& ns = {}) {
    Io::StringWriter w;
    try$(serialize(w, try$(parseSelector(input, ns))));
    return Ok(w.take());
}

static ComplexSelectorUnit _compound(Opt<TypeSelector> type, Vec<SubclassSelector> subclasses = {}) {
    return {
        .selector = Some(CompoundSelector{
            .type = type,
            .subclasses = subclasses,
        }),
    };
}

static ComplexSelectorUnit _compound(TypeSelector type, Vec<SubclassSelector> subclasses = {}) {
    return _compound(Some(type), subclasses);
}

static TypeSelector _type(Str name) {
    return {.qualifiedName = {UNIVERSAL, Symbol::from(name)}};
}

test$("vaev-style-parse-simple-selectors") {
    assertNot$(parseSelector(""));
    assertNot$(parseSelector("   "));

    assertEq$(
        try$(parseSelector("html")),
        (SelectorList{ComplexSelector{.rhs = _compound(_type("html"))}})
    );

    assertEq$(try$(_roundtrip("html")), "html"s);
    assertEq$(try$(_roundtrip("html ")), "html"s);
    assertEq$(try$(_roundtrip(" html")), "html"s);
    assertEq$(try$(_roundtrip(".className")), ".className"s);
    assertEq$(try$(_roundtrip("#idName")), "#idName"s);
    assertEq$(try$(_roundtrip("*")), "*"s);

    return Ok();
}

test$("vaev-style-parse-selector-lists") {
    assertEq$(
        try$(parseSelector("html,.className")),
        (SelectorList{
            ComplexSelector{.rhs = _compound(_type("html"))},
            ComplexSelector{.rhs = _compound(NONE, {ClassSelector{"className"s}})},
        })
    );

    assertEq$(try$(_roundtrip("html,.className , \n #idName")), "html, .className, #idName"s);
    assertEq$(try$(_roundtrip("html,.className , \n #idName,*")), "html, .className, #idName, *"s);

    assertNot$(parseSelector("html,"));
    assertNot$(parseSelector(",html"));
    assertNot$(parseSelector("html,,.a"));

    return Ok();
}

test$("vaev-style-parse-compound-selectors") {
    assertEq$(
        try$(parseSelector("html.className")),
        (SelectorList{ComplexSelector{.rhs = _compound(_type("html"), {ClassSelector{"className"s}})}})
    );

    assertEq$(try$(_roundtrip("html/* x */.className")), "html.className"s);
    assertEq$(try$(_roundtrip(".className#idName")), ".className#idName"s);

    assertNot$(parseSelector(".className*"));
    assertNot$(parseSelector("."));
    assertNot$(parseSelector("#"));

    return Ok();
}

test$("vaev-style-parse-complex-selectors") {
    assertEq$(
        try$(parseSelector("html > .className #idName")),
        (SelectorList{ComplexSelector{
            .lhs = {
                {_compound(_type("html")), SelectorCombinator::CHILD},
                {_compound(NONE, {ClassSelector{"className"s}}), SelectorCombinator::DESCENDANT},
            },
            .rhs = _compound(NONE, {IdSelector{"idName"_sym}}),
        }})
    );

    assertEq$(try$(_roundtrip("html .className")), "html .className"s);
    assertEq$(try$(_roundtrip("html /* x */ .className")), "html .className"s);
    assertEq$(try$(_roundtrip("html>.className")), "html > .className"s);
    assertEq$(try$(_roundtrip("html > .className")), "html > .className"s);
    assertEq$(try$(_roundtrip("html +.className")), "html + .className"s);
    assertEq$(try$(_roundtrip("html~.className")), "html ~ .className"s);
    assertEq$(try$(_roundtrip("html > .className#idName")), "html > .className#idName"s);
    assertEq$(try$(_roundtrip("html#idName .className")), "html#idName .className"s);
    assertEq$(try$(_roundtrip("tr:not(:last-child) th:not(:first-child)")), "tr:not(:last-child) th:not(:first-child)"s);
    assertEq$(try$(_roundtrip("td, .o_content .o .o_table th ")), "td, .o_content .o .o_table th"s);
    assertEq$(
        try$(_roundtrip(".o_content .o_table > thead > tr:not(:last-child) th:not(:first-child)")),
        ".o_content .o_table > thead > tr:not(:last-child) th:not(:first-child)"s
    );

    return Ok();
}

test$("vaev-style-reject-invalid-combinators") {
    assertNot$(parseSelector("html~"));
    assertNot$(parseSelector("html+"));
    assertNot$(parseSelector("html>"));
    assertNot$(parseSelector(">html"));
    assertNot$(parseSelector("html > > body"));
    assertNot$(parseSelector("html || body"));

    return Ok();
}

test$("vaev-style-parse-anb") {
    assertEq$(
        try$(AnB::parse("odd")),
        AnB(2, 1)
    );

    assertEq$(
        try$(AnB::parse("even")),
        AnB(2, 0)
    );

    assertEq$(
        try$(AnB::parse("4")),
        AnB(0, 4)
    );

    assertEq$(
        try$(AnB::parse("-n")),
        AnB(-1, 0)
    );

    assertEq$(
        try$(AnB::parse("n")),
        AnB(1, 0)
    );

    assertEq$(
        try$(AnB::parse("3n")),
        AnB(3, 0)
    );

    assertEq$(
        try$(AnB::parse("2n+5")),
        AnB(2, 5)
    );

    assertEq$(
        try$(AnB::parse("2n + 5")),
        AnB(2, 5)
    );

    assertEq$(
        try$(AnB::parse("2n +5")),
        AnB(2, 5)
    );

    assertEq$(
        try$(AnB::parse("2n -5")),
        AnB(2, -5)
    );

    assertEq$(
        try$(AnB::parse("2n-5")),
        AnB(2, -5)
    );

    assertEq$(
        try$(AnB::parse("n-5")),
        AnB(1, -5)
    );

    assertEq$(
        try$(AnB::parse("n- 5")),
        AnB(1, -5)
    );

    assertEq$(
        try$(AnB::parse("n+ 5")),
        AnB(1, 5)
    );

    assertNot$(AnB::parse("n+"));
    assertNot$(AnB::parse("n+-5"));
    assertNot$(AnB::parse("n- +5"));
    assertNot$(AnB::parse("n-+ 5"));
    assertNot$(AnB::parse("n -+5"));
    assertNot$(AnB::parse("+ n"));
    assertNot$(AnB::parse("+ n -3"));

    return Ok();
}

test$("vaev-style-anb-index-match") {
    {
        AnB anb{2, 0};
        assertNot$(anb.match(1));
        assert$(anb.match(2));
        assertNot$(anb.match(3));
        assert$(anb.match(4));
    }
    {
        AnB anb{2, 1};
        assert$(anb.match(1));
        assertNot$(anb.match(2));
        assert$(anb.match(3));
        assertNot$(anb.match(4));
    }
    {
        AnB anb{3, -1};
        for (usize i = 1; i <= 10; i++)
            assertEq$(anb.match(i), i == 2 or i == 5 or i == 8);
    }
    {
        AnB anb{-3, 10};
        for (usize i = 1; i <= 20; i++)
            assertEq$(anb.match(i), i == 1 or i == 4 or i == 7 or i == 10);
    }
    {
        AnB anb{1, 1};
        for (usize i = 1; i <= 20; i++)
            assert$(anb.match(i));
    }
    {
        AnB anb{1, 2};
        for (usize i = 1; i <= 20; i++)
            assertEq$(anb.match(i), i >= 2);
    }
    {
        AnB anb{0, 0};
        for (usize i = 1; i <= 20; i++)
            assertNot$(anb.match(i));
    }
    {
        AnB anb{0, 21};
        for (usize i = 1; i <= 21; i++)
            assertEq$(anb.match(i), i == 21);
    }

    return Ok();
}

test$("vaev-style-parse-pseudo-classes") {
    assertEq$(
        try$(parseSelector(":root")),
        (SelectorList{ComplexSelector{.rhs = _compound(NONE, {PseudoClassSelector{PseudoClass::ROOT}})}})
    );

    assertEq$(
        try$(parseSelector(":first-child")),
        (SelectorList{ComplexSelector{.rhs = _compound(NONE, {ChildIndexPseudoClass{.type = ChildIndexPseudoClass::FIRST_CHILD}})}})
    );

    assertEq$(
        try$(parseSelector(":nth-child(odd of .class)")),
        (SelectorList{ComplexSelector{.rhs = _compound(NONE, {ChildIndexPseudoClass{
                                                                 .type = ChildIndexPseudoClass::NTH_CHILD,
                                                                 .anb = AnB(2, 1),
                                                                 .s = {ComplexSelector{.rhs = _compound(NONE, {ClassSelector{"class"s}})}},
                                                             }})}})
    );

    assertEq$(try$(_roundtrip(":nth-child(odd)")), ":nth-child(2n+1)"s);
    assertEq$(try$(_roundtrip(":nth-child(n of .class)")), ":nth-child(1n+0 of .class)"s);
    assertEq$(try$(_roundtrip(":nth-last-child(even)")), ":nth-last-child(2n+0)"s);
    assertEq$(try$(_roundtrip(":nth-last-child(even of #id)")), ":nth-last-child(2n+0 of #id)"s);
    assertEq$(try$(_roundtrip(":nth-of-type(3n+1)")), ":nth-of-type(3n+1)"s);
    assertEq$(try$(_roundtrip(":nth-last-of-type(3n+1)")), ":nth-last-of-type(3n+1)"s);
    assertEq$(try$(_roundtrip(":last-child")), ":last-child"s);
    assertEq$(try$(_roundtrip(".class :last-child")), ".class :last-child"s);
    assertEq$(try$(_roundtrip("html:hover")), "html:hover"s);
    assertEq$(try$(_roundtrip(":ROOT")), ":root"s);

    assertNot$(parseSelector(":nth-of-type(3n+1 of .class)"));
    assertNot$(parseSelector(":nth-last-of-type(3n+1 of .class)"));
    assertNot$(parseSelector(":nth-child(3n+1 of)"));
    assertNot$(parseSelector(":unknown-pseudo-class"));
    assertNot$(parseSelector(":unknown-pseudo-class(.a)"));
    assertNot$(parseSelector(": hover"));

    return Ok();
}

test$("vaev-style-parse-logical-pseudo-classes") {
    assertEq$(
        try$(parseSelector(":not(.className)")),
        (SelectorList{ComplexSelector{.rhs = _compound(NONE, {NegationPseudoClass{{ComplexSelector{.rhs = _compound(NONE, {ClassSelector{"className"s}})}}}})}})
    );

    assertEq$(try$(_roundtrip("div:not(.foo, .bar)")), "div:not(.foo, .bar)"s);
    assertEq$(try$(_roundtrip(":not(:first-child)")), ":not(:first-child)"s);
    assertEq$(try$(_roundtrip(":is(.a .b, #c)")), ":is(.a .b, #c)"s);
    assertEq$(try$(_roundtrip(":where(.a > .b)")), ":where(.a > .b)"s);
    assertEq$(try$(_roundtrip(":has(.a)")), ":has(.a)"s);
    assertEq$(try$(_roundtrip(":has(> .a .b)")), ":has(> .a .b)"s);
    assertEq$(try$(_roundtrip(":has(+ .a, ~ .b)")), ":has(+ .a, ~ .b)"s);

    // https://www.w3.org/TR/selectors-4/#forgiving-selector
    // The <forgiving-selector-list> production instead parses each selector in the list individually, simply ignoring
    // ones that fail to parse, so the remaining selectors can still be used.
    assertEq$(try$(_roundtrip(":is(.a, :unknown, .b)")), ":is(.a, .b)"s);
    assertEq$(try$(_roundtrip(":where(:unknown)")), ":where()"s);

    assertNot$(parseSelector(":not(.a, :unknown)"));
    assertNot$(parseSelector(":not(::before)"));
    assertEq$(try$(_roundtrip(":is(::before)")), ":is()"s);
    assertNot$(parseSelector(":has(::before)"));
    assertNot$(parseSelector("div::not(.foo)"));

    return Ok();
}

test$("vaev-style-parse-pseudo-elements") {
    assertEq$(
        try$(parseSelector("html::after")),
        (SelectorList{ComplexSelector{
            .rhs = {
                .selector = Some(CompoundSelector{.type = Some(_type("html"))}),
                .pseudo = {{PseudoElementSelector{Dom::PseudoElement::AFTER}}},
            },
        }})
    );

    // this should pass for legacy reasons
    // https://www.w3.org/TR/selectors-3/#pseudo-elements
    assertEq$(try$(_roundtrip("html:after")), "html::after"s);
    assertEq$(try$(_roundtrip(":before")), "::before"s);
    assertEq$(try$(_roundtrip("li::marker")), "li::marker"s);
    assertEq$(try$(_roundtrip(".a::before:hover")), ".a::before:hover"s);
    assertEq$(try$(_roundtrip("::selection")), "::selection"s);

    assertNot$(parseSelector("html::after .a"));
    assertNot$(parseSelector("::before.a"));
    assertNot$(parseSelector("::before:not(.a)"));

    return Ok();
}

test$("vaev-style-parse-attribute-selectors") {
    assertEq$(
        try$(parseSelector(".className[type]")),
        (SelectorList{ComplexSelector{.rhs = _compound(NONE, {
                                                                 ClassSelector{"className"s},
                                                                 AttributeSelector{
                                                                     QualifiedNameSelector{NULL_NAMESPACE, "type"_sym},
                                                                     AttributeSelector::SENSITIVE,
                                                                     AttributeSelector::PRESENT,
                                                                     ""s,
                                                                 },
                                                             })}})
    );

    assertEq$(
        try$(parseSelector("[type*='text' i]")),
        (SelectorList{ComplexSelector{.rhs = _compound(NONE, {
                                                                 AttributeSelector{
                                                                     QualifiedNameSelector{NULL_NAMESPACE, "type"_sym},
                                                                     AttributeSelector::INSENSITIVE,
                                                                     AttributeSelector::STR_CONTAIN,
                                                                     "text"s,
                                                                 },
                                                             })}})
    );

    assertEq$(try$(_roundtrip(".className[type='text']")), ".className[type=\"text\"]"s);
    assertEq$(try$(_roundtrip(".className[ type = 'text' ]")), ".className[type=\"text\"]"s);
    assertEq$(try$(_roundtrip(".className[type=text s]")), ".className[type=\"text\"]"s);
    assertEq$(try$(_roundtrip("[lang|=en]")), "[lang|=\"en\"]"s);
    assertEq$(try$(_roundtrip("[a~=b][c^=d][e$=f]")), "[a~=\"b\"][c^=\"d\"][e$=\"f\"]"s);
    assertEq$(try$(_roundtrip("*|p")), "p"s);
    assertEq$(try$(_roundtrip("*|*[*|title^='si on']")), "*[*|title^=\"si on\"]"s);
    assertEq$(try$(_roundtrip("*|*[title^='si on']")), "*[title^=\"si on\"]"s);
    assertEq$(try$(_roundtrip("*|*[|title^='si on']")), "*[title^=\"si on\"]"s);

    assertNot$(parseSelector("[]"));
    assertNot$(parseSelector("[a=]"));
    assertNot$(parseSelector("[a=b c]"));
    assertNot$(parseSelector("[a%=b]"));
    assertNot$(parseSelector("[undeclared|a]"));

    return Ok();
}

test$("vaev-style-parse-namespaces") {
    NamespaceScope ns;
    ns.default_ = Some("http://www.w3.org/1999/xhtml"_sym);
    ns.prefixes.put("svg"_sym, "http://www.w3.org/2000/svg"_sym);

    assertEq$(
        try$(parseSelector(".a", ns)),
        (SelectorList{ComplexSelector{.rhs = _compound(TypeSelector{{QualifiedNameSelector::NamespacePattern{"http://www.w3.org/1999/xhtml"_sym}, UNIVERSAL}}, {ClassSelector{"a"s}})}})
    );

    assertEq$(try$(_roundtrip("svg|rect", ns)), "http://www.w3.org/2000/svg|rect"s);
    assertEq$(try$(_roundtrip("*|rect", ns)), "rect"s);
    assertNot$(parseSelector("math|mi", ns));

    return Ok();
}

} // namespace Vaev::Style::Tests
