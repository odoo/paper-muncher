#include <karm-test/macros.h>
#include <vaev-style/selector.h>

namespace Vaev::Style::Tests {

test$("vaev-style-parse-simple-selectors") {
    expect$(
        not Selector::parse("").has()
    );

    expectEq$(
        try$(Selector::parse("html")),
        TypeSelector{"html"s}
    );

    expectEq$(
        try$(Selector::parse("html ")),
        TypeSelector{"html"s}
    );

    expectEq$(
        try$(Selector::parse(" html")),
        TypeSelector{"html"s}
    );

    expectEq$(
        try$(Selector::parse(".className")),
        ClassSelector{"className"s}
    );

    expectEq$(
        try$(Selector::parse("#idName")),
        IdSelector{"idName"s}
    );

    expectEq$(
        try$(Selector::parse("*")),
        UniversalSelector{}
    );

    return Ok();
}

test$("vaev-style-parse-nfix-selectors") {
    expectEq$(
        try$(Selector::parse("html,.className")),
        Selector::or_({
            TypeSelector{"html"s},
            ClassSelector{"className"s},
        })
    );

    expectEq$(
        try$(Selector::parse("html,.className , \n #idName")),
        Selector::or_({
            TypeSelector{"html"s},
            ClassSelector{"className"s},
            IdSelector{"idName"s},
        })
    );

    expectEq$(
        try$(Selector::parse("html,.className , \n #idName,*")),
        Selector::or_({
            TypeSelector{"html"s},
            ClassSelector{"className"s},
            IdSelector{"idName"s},
            UniversalSelector{},
        })
    );

    expectEq$(
        try$(Selector::parse("html.className")),
        Selector::and_({
            TypeSelector{"html"s},
            ClassSelector{"className"s},
        })
    );

    return Ok();
}

test$("vaev-style-parse-infix-selectors") {
    expectEq$(
        try$(Selector::parse("html .className")),
        Selector::descendant(
            TypeSelector{"html"s},
            ClassSelector{"className"s}
        )
    );

    expectEq$(
        try$(Selector::parse("html>.className")),
        Selector::child(
            TypeSelector{"html"s},
            ClassSelector{"className"s}
        )
    );

    expectEq$(
        try$(Selector::parse("html > .className")),
        Selector::child(
            TypeSelector{"html"s},
            ClassSelector{"className"s}
        )
    );

    expectEq$(
        try$(Selector::parse("html > .className #idName")),
        Selector::descendant(
            Selector::child(
                TypeSelector{"html"s},
                ClassSelector{"className"s}
            ),
            IdSelector{"idName"s}
        )
    );

    expectEq$(
        try$(Selector::parse(":not(.className)")),
        Selector::not_(ClassSelector{"className"s})
    );

    return Ok();
}

test$("vaev-style-parse-adjacent-selectors") {
    expectEq$(
        try$(Selector::parse("html +.className")),
        Selector::adjacent(
            TypeSelector{"html"s},
            ClassSelector{"className"s}
        )
    );

    return Ok();
}

test$("vaev-style-parse-subsequent-selectors") {
    expectEq$(
        try$(Selector::parse("html~.className")),
        Selector::subsequent(
            TypeSelector{"html"s},
            ClassSelector{"className"s}
        )
    );

    return Ok();
}

test$("vaev-style-parse-mixed-selectors") {
    expectEq$(
        try$(Selector::parse("html > .className#idName")),
        Selector::child(
            TypeSelector{"html"s},
            Selector::and_({
                ClassSelector{"className"s},
                IdSelector{"idName"s},
            })
        )
    );

    expectEq$(
        try$(Selector::parse("html#idName .className")),
        Selector::descendant(
            Selector::and_({
                TypeSelector{"html"s},
                IdSelector{"idName"s},
            }),
            ClassSelector{"className"s}
        )
    );

    expectEq$(
        try$(Selector::parse(":not(:first-child)")),
        Selector::not_(Pseudo{Pseudo::FIRST_CHILD})
    );

    expectEq$(
        try$(Selector::parse("tr:not(:last-child) th:not(:first-child)")),
        Selector::descendant(
            Selector::and_({TypeSelector{"tr"s}, Selector::not_(Pseudo{Pseudo::LAST_CHILD})}),
            Selector::and_({TypeSelector{"th"s}, {Selector::not_(Pseudo{Pseudo::FIRST_CHILD})}})
        )
    );

    expectEq$(
        try$(Selector::parse("td, .o_content .o .o_table th ")),
        Selector::or_(
            {TypeSelector{"td"s},
             Selector::descendant(Selector::descendant(Selector::descendant(ClassSelector{"o_content"s}, ClassSelector{"o"s}), ClassSelector{"o_table"s}), TypeSelector{"th"s})}
        )
    );

    expectEq$(
        try$(Selector::parse("td, .o_content .o_table th ")),
        Selector::or_(
            {TypeSelector{"td"s},
             Selector::descendant(Selector::descendant(ClassSelector{"o_content"s}, ClassSelector{"o_table"s}), TypeSelector{"th"s})}
        )
    );

    expectEq$(
        try$(Selector::parse(".o_content .o_table > thead > tr:not(:last-child) th:not(:first-child)")),
        Selector::descendant(
            Selector::child(
                Selector::descendant(
                    ClassSelector{"o_content"s},
                    Selector::child(ClassSelector{"o_table"s}, TypeSelector{"thead"s})
                ),
                Selector::and_({TypeSelector{"tr"s}, Selector::not_(Pseudo{Pseudo::LAST_CHILD})})
            ),
            Selector::and_({TypeSelector{"th"s}, {Selector::not_(Pseudo{Pseudo::FIRST_CHILD})}})
        )
    );

    return Ok();
}

test$("vaev-style-parse-pseudo-selectors") {
    expectEq$(
        Pseudo{Pseudo::ROOT},
        Pseudo{Pseudo::ROOT}
    );

    expectEq$(
        try$(Selector::parse(":root")),
        Pseudo{Pseudo::ROOT}
    );

    expectEq$(
        try$(Selector::parse(":root")),
        Pseudo{Pseudo::make("root")}
    );

    expectEq$(
        try$(Selector::parse(":first-child")),
        Pseudo{Pseudo::FIRST_CHILD}
    );

    expectEq$(
        try$(Selector::parse(":last-child")),
        Pseudo{Pseudo::LAST_CHILD}
    );

    expectEq$(
        try$(Selector::parse(".class :last-child")),
        Selector::descendant(
            ClassSelector{"class"s},
            Pseudo{Pseudo::LAST_CHILD}
        )
    );

    expectEq$(
        try$(Selector::parse("html:hover")),
        Selector::and_({
            TypeSelector{"html"s},
            Pseudo{Pseudo::HOVER},
        })
    );

    // this should pass for legacy resons
    // https://www.w3.org/TR/selectors-3/#pseudo-elements
    expectEq$(
        try$(Selector::parse("html:after")),
        Selector::and_({
            TypeSelector{"html"s},
            Pseudo{Pseudo::AFTER},
        })
    );

    expectEq$(
        try$(Selector::parse("html::after")),
        Selector::and_({
            TypeSelector{"html"s},
            Pseudo{Pseudo::AFTER},
        })
    );

    expectEq$(
        try$(Selector::parse("div::not(.foo)")),
        Selector::and_({TypeSelector{"div"s}, Selector::not_(ClassSelector{"foo"s})})
    );

    expectEq$(
        try$(Selector::parse("div::not(.foo, .bar)")),
        Selector::and_({TypeSelector{"div"s}, Selector::not_(Selector::or_({ClassSelector{"foo"s}, ClassSelector{"bar"s}}))})
    );

    return Ok();
}

test$("vaev-style-parse-attribute-selectors") {
    expectEq$(
        try$(Selector::parse(".className[type]")),
        Selector::and_({
            ClassSelector{"className"s},
            AttributeSelector{
                "type"s,
                AttributeSelector::INSENSITIVE,
                AttributeSelector::PRESENT,
                ""s,
            },
        })
    );

    expectEq$(
        try$(Selector::parse(".className[type='text']")),
        Selector::and_({
            ClassSelector{"className"s},
            AttributeSelector{
                "type"s,
                AttributeSelector::INSENSITIVE,
                AttributeSelector::EXACT,
                "text"s,
            },
        })
    );

    expectEq$(
        try$(Selector::parse(".className[ type = 'text' ]")),
        Selector::and_({
            ClassSelector{"className"s},
            AttributeSelector{
                "type"s,
                AttributeSelector::INSENSITIVE,
                AttributeSelector::EXACT,
                "text"s,
            },
        })
    );

    expectEq$(
        try$(Selector::parse(".className[type*='text']")),
        Selector::and_({
            ClassSelector{"className"s},
            AttributeSelector{
                "type"s,
                AttributeSelector::INSENSITIVE,
                AttributeSelector::STR_CONTAIN,
                "text"s,
            },
        })
    );

    expectEq$(
        try$(Selector::parse(".className[type='text' s]")),
        Selector::and_({
            ClassSelector{"className"s},
            AttributeSelector{
                "type"s,
                AttributeSelector::SENSITIVE,
                AttributeSelector::EXACT,
                "text"s,
            },
        })
    );

    return Ok();
}

} // namespace Vaev::Style::Tests
