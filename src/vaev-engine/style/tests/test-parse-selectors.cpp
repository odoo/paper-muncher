#include <karm-test/macros.h>

import Vaev.Engine;

namespace Vaev::Style::Tests {

test$("vaev-style-parse-simple-selectors") {
    expect$(
        not Selector::parse("").has()
    );

    expectEq$(
        try$(Selector::parse("html")),
        TypeSelector{Html::HTML_TAG}
    );

    expectEq$(
        try$(Selector::parse("html ")),
        TypeSelector{Html::HTML_TAG}
    );

    expectEq$(
        try$(Selector::parse(" html")),
        TypeSelector{Html::HTML_TAG}
    );

    expectEq$(
        try$(Selector::parse(".className")),
        ClassSelector{"className"s}
    );

    expectEq$(
        try$(Selector::parse("#idName")),
        IdSelector{"idName"_sym}
    );

    expectEq$(
        try$(Selector::parse("*")),
        (TypeSelector{Html::NAMESPACE, NONE})
    );

    return Ok();
}

test$("vaev-style-parse-nfix-selectors") {
    expectEq$(
        try$(Selector::parse("html,.className")),
        Selector::or_({
            TypeSelector{Html::HTML_TAG},
            ClassSelector{"className"s},
        })
    );

    expectEq$(
        try$(Selector::parse("html,.className , \n #idName")),
        Selector::or_({
            TypeSelector{Html::HTML_TAG},
            ClassSelector{"className"s},
            IdSelector{"idName"_sym},
        })
    );

    expectEq$(
        try$(Selector::parse("html,.className , \n #idName,*")),
        Selector::or_({
            TypeSelector{Html::HTML_TAG},
            ClassSelector{"className"s},
            IdSelector{"idName"_sym},
            TypeSelector{Html::NAMESPACE, NONE},
        })
    );

    expectEq$(
        try$(Selector::parse("html.className")),
        Selector::and_({
            TypeSelector{Html::HTML_TAG},
            ClassSelector{"className"s},
        })
    );

    return Ok();
}

test$("vaev-style-parse-infix-selectors") {
    expectEq$(
        try$(Selector::parse("html .className")),
        Selector::descendant(
            TypeSelector{Html::HTML_TAG},
            ClassSelector{"className"s}
        )
    );

    expectEq$(
        try$(Selector::parse("html>.className")),
        Selector::child(
            TypeSelector{Html::HTML_TAG},
            ClassSelector{"className"s}
        )
    );

    expectEq$(
        try$(Selector::parse("html > .className")),
        Selector::child(
            TypeSelector{Html::HTML_TAG},
            ClassSelector{"className"s}
        )
    );

    expectEq$(
        try$(Selector::parse("html > .className #idName")),
        Selector::descendant(
            Selector::child(
                TypeSelector{Html::HTML_TAG},
                ClassSelector{"className"s}
            ),
            IdSelector{"idName"_sym}
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
            TypeSelector{Html::HTML_TAG},
            ClassSelector{"className"s}
        )
    );

    return Ok();
}

test$("vaev-style-parse-subsequent-selectors") {
    expectEq$(
        try$(Selector::parse("html~.className")),
        Selector::subsequent(
            TypeSelector{Html::HTML_TAG},
            ClassSelector{"className"s}
        )
    );

    return Ok();
}

test$("vaev-style-parse-mixed-selectors") {
    expectEq$(
        try$(Selector::parse("html > .className#idName")),
        Selector::child(
            TypeSelector{Html::HTML_TAG},
            Selector::and_({
                ClassSelector{"className"s},
                IdSelector{"idName"_sym},
            })
        )
    );

    expectEq$(
        try$(Selector::parse("html#idName .className")),
        Selector::descendant(
            Selector::and_({
                TypeSelector{Html::HTML_TAG},
                IdSelector{"idName"_sym},
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
            Selector::and_({TypeSelector{Html::TR_TAG}, Selector::not_(Pseudo{Pseudo::LAST_CHILD})}),
            Selector::and_({TypeSelector{Html::TH_TAG}, {Selector::not_(Pseudo{Pseudo::FIRST_CHILD})}})
        )
    );

    expectEq$(
        try$(Selector::parse("td, .o_content .o .o_table th ")),
        Selector::or_(
            {TypeSelector{Html::TD_TAG},
             Selector::descendant(
                 Selector::descendant(
                     Selector::descendant(
                         ClassSelector{"o_content"s},
                         ClassSelector{"o"s}
                     ),
                     ClassSelector{"o_table"s}
                 ),
                 TypeSelector{Html::TH_TAG}
             )}
        )
    );

    expectEq$(
        try$(Selector::parse("td, .o_content .o_table th ")),
        Selector::or_(
            {
                TypeSelector{Html::TD_TAG},
                Selector::descendant(
                    Selector::descendant(
                        ClassSelector{"o_content"s},
                        ClassSelector{"o_table"s}
                    ),
                    TypeSelector{Html::TH_TAG}
                ),
            }
        )
    );

    expectEq$(
        try$(Selector::parse(".o_content .o_table > thead > tr:not(:last-child) th:not(:first-child)")),
        Selector::descendant(
            Selector::child(
                Selector::descendant(
                    ClassSelector{"o_content"s},
                    Selector::child(ClassSelector{"o_table"s}, TypeSelector{Html::THEAD_TAG})
                ),
                Selector::and_({
                    TypeSelector{Html::TR_TAG},
                    Selector::not_(Pseudo{Pseudo::LAST_CHILD}),
                })
            ),
            Selector::and_({
                TypeSelector{Html::TH_TAG},
                {Selector::not_(Pseudo{Pseudo::FIRST_CHILD})},
            })
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
            TypeSelector{Html::HTML_TAG},
            Pseudo{Pseudo::HOVER},
        })
    );

    // this should pass for legacy resons
    // https://www.w3.org/TR/selectors-3/#pseudo-elements
    expectEq$(
        try$(Selector::parse("html:after")),
        Selector::and_({
            TypeSelector{Html::HTML_TAG},
            Pseudo{Pseudo::AFTER},
        })
    );

    expectEq$(
        try$(Selector::parse("html::after")),
        Selector::and_({
            TypeSelector{Html::HTML_TAG},
            Pseudo{Pseudo::AFTER},
        })
    );

    expectEq$(
        try$(Selector::parse("div::not(.foo)")),
        Selector::and_({
            TypeSelector{Html::DIV_TAG},
            Selector::not_(ClassSelector{"foo"s}),
        })
    );

    expectEq$(
        try$(Selector::parse("div::not(.foo, .bar)")),
        Selector::and_({
            TypeSelector{Html::DIV_TAG},
            Selector::not_(Selector::or_({
                ClassSelector{"foo"s},
                ClassSelector{"bar"s},
            })),
        })
    );

    return Ok();
}

test$("vaev-style-parse-attribute-selectors") {
    expectEq$(
        try$(Selector::parse(".className[type]")),
        Selector::and_({
            ClassSelector{"className"s},
            AttributeSelector{
                QualifiedNameSelector{NONE, "type"_sym},
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
                QualifiedNameSelector{NONE, "type"_sym},
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
                QualifiedNameSelector{NONE, "type"_sym},
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
                QualifiedNameSelector{NONE, "type"_sym},
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
                QualifiedNameSelector{NONE, "type"_sym},
                AttributeSelector::SENSITIVE,
                AttributeSelector::EXACT,
                "text"s,
            },
        })
    );

    return Ok();
}

} // namespace Vaev::Style::Tests
