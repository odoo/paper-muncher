#include <karm-test/macros.h>
#include <vaev-style/computer.h>
#include <vaev-style/select.h>

#include "vaev-style/rules.h"

namespace Vaev::Style::Tests {

test$("test-specificity-selector-list") {
    Selector selector{try$(Selector::parse(".a, .b#x, .c.d.e.f"))};

    auto makeRule = [&]() -> Rule {
        StyleRule rule{
            .selector = selector,
            .props = {}
        };
        return rule;
    };

    Rule rule = makeRule();

    Style::StyleBook stylebook;
    Style::Media media;
    Style::Computer computer{media, stylebook};

    {
        Markup::Element elZeroMatches{Markup::Element(Html::DIV)};

        Vec<Tuple<Cursor<StyleRule>, Spec>> matchingRules;
        computer._evalRule(rule, elZeroMatches, matchingRules);

        expect$(matchingRules.len() == 0);
    }
    {
        Markup::Element elOneMatch{Markup::Element(Html::DIV)};
        elOneMatch.classList.add("a");

        Vec<Tuple<Cursor<StyleRule>, Spec>> matchingRules;
        computer._evalRule(rule, elOneMatch, matchingRules);

        expect$(matchingRules[0].v1 == Spec(0, 1, 0));
    }
    {
        Markup::Element elOneMatch{Markup::Element(Html::DIV)};
        elOneMatch.classList.add("c");
        elOneMatch.classList.add("d");
        elOneMatch.classList.add("e");
        elOneMatch.classList.add("f");

        Vec<Tuple<Cursor<StyleRule>, Spec>> matchingRules;
        computer._evalRule(rule, elOneMatch, matchingRules);

        expect$(matchingRules[0].v1 == Spec(0, 4, 0));
    }
    {
        Markup::Element elAnotherMatch{Markup::Element(Html::DIV)};
        elAnotherMatch.classList.add("b");
        elAnotherMatch.setAttribute(AttrName::make("id"s, Vaev::HTML), "x"s);

        Vec<Tuple<Cursor<StyleRule>, Spec>> matchingRules;
        computer._evalRule(rule, elAnotherMatch, matchingRules);

        expect$(matchingRules[0].v1 == Spec(1, 1, 0));
    }
    {
        Markup::Element twoMatches{Markup::Element(Html::DIV)};

        twoMatches.classList.add("b");
        twoMatches.setAttribute(AttrName::make("id"s, Vaev::HTML), "x"s);
        twoMatches.classList.add("a");

        Vec<Tuple<Cursor<StyleRule>, Spec>> matchingRules;
        computer._evalRule(rule, twoMatches, matchingRules);

        expect$(matchingRules[0].v1 == Spec(1, 1, 0));
    }

    return Ok();
}


} // namespace Vaev::Style::Tests
