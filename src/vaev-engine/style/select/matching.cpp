export module Vaev.Engine:style.select.matching;

import Karm.Core;
import Karm.Debug;
import Karm.Gc;

import :dom.element;
import :style.select.pseudoClass;
import :style.select.selectors;
import :style.select.specificity;

using namespace Karm;
using namespace Karm::Literals;

namespace Vaev::Style {

static auto featureNthChild = Debug::Flag::feature("web-style-nth_child", "Enable :nth-child() and related selectors", true);

export struct MatchSubject {
    Gc::Ref<Dom::Element> element;
    Opt<Symbol> pseudoElement = NONE;
    bool inHtmlDocument = false;

    bool isDocumentElement() const {
        return element->isDocumentElement() and not pseudoElement;
    }

    bool isHtmlElementInHtmlDocument() const {
        return inHtmlDocument and element->namespaceUri() == Html::NAMESPACE;
    }

    MatchSubject withPseudoElement(Symbol pseudoElement) const {
        MatchSubject copy = *this;
        copy.pseudoElement = Some(pseudoElement);
        return copy;
    }

    MatchSubject withElement(Gc::Ref<Dom::Element> element) const {
        MatchSubject copy = *this;
        copy.element = element;
        copy.pseudoElement = NONE;
        return copy;
    }

    MatchSubject originatingElement() const {
        return withElement(element);
    }
};

export bool match(ComplexSelector const& selector, MatchSubject const& subject);

static bool _matchAny(SelectorList const& selectors, MatchSubject const& subject) {
    return iter(selectors) | Any([&](auto const& s) {
               return match(s, subject);
           });
}

// MARK: Simple Selectors ------------------------------------------------------

// https://www.w3.org/TR/selectors-4/#type-selectors
static bool _match(TypeSelector const& selector, MatchSubject const& subject) {
    // https://html.spec.whatwg.org/multipage/semantics-other.html#case-sensitivity-of-selectors
    // When comparing a CSS element type selector to the names of HTML elements in HTML documents, the CSS element type
    // selector must first be converted to ASCII lowercase. The same selector when compared to other elements must be
    // compared according to its original case. In both cases, to match, the values must be identical to each other
    // (and therefore the comparison is case sensitive).
    return selector.qualifiedName.match(subject.element->qualifiedName, subject.isHtmlElementInHtmlDocument());
}

static Opt<Str> _getAttributeValue(AttributeSelector const& selector, MatchSubject const& subject) {
    if (auto ns = selector.qualifiedName.ns.is<Symbol>()) {
        auto name = selector.qualifiedName.exactName();
        if (not name)
            return NONE;
        return subject.element->getAttribute({Some(*ns), *name});
    }

    // https://html.spec.whatwg.org/multipage/semantics-other.html#case-sensitivity-of-selectors
    // When comparing the name part of a CSS attribute selector to the names of attributes on HTML elements in HTML
    // documents, the name part of the CSS attribute selector must first be converted to ASCII lowercase. The same
    // selector when compared to other attributes must be compared according to its original case. In both cases, the
    // comparison is case-sensitive.
    auto name = selector.qualifiedName.exactName(subject.isHtmlElementInHtmlDocument());
    if (not name)
        return NONE;

    if (selector.qualifiedName.ns.is<Universal>())
        return subject.element->getAttributeUnqualified(name->str());

    return subject.element->getAttribute({NONE, *name});
}

static bool _matchContainsWord(Str attrValue, Str word, auto cmp) {
    usize tokenStart = 0;
    while (tokenStart < attrValue.len()) {
        while (tokenStart < attrValue.len() and isAsciiSpace(attrValue[tokenStart]))
            tokenStart++;

        usize tokenEnd = tokenStart;
        while (tokenEnd < attrValue.len() and not isAsciiSpace(attrValue[tokenEnd]))
            tokenEnd++;

        if (tokenStart != tokenEnd and startWith(sub(attrValue, tokenStart, tokenEnd), word, cmp) == Match::YES)
            return true;

        tokenStart = tokenEnd;
    }

    return false;
}

// https://www.w3.org/TR/selectors-4/#attribute-selectors
static bool _match(AttributeSelector const& selector, MatchSubject const& subject) {
    auto maybeAttrValue = _getAttributeValue(selector, subject);

    if (selector.match == AttributeSelector::PRESENT) {
        // Represents an element with the att attribute, whatever the value of the attribute.
        return maybeAttrValue != NONE;
    }

    if (not maybeAttrValue)
        return false;

    auto attrValue = maybeAttrValue.expect();

    auto cmp = [&selector](Rune const& a, Rune const& b) {
        if (selector.case_ == AttributeSelector::INSENSITIVE)
            return toAsciiLower(a) == toAsciiLower(b);
        else
            return a == b;
    };

    if (selector.match == AttributeSelector::EXACT) {
        // Represents an element with the att attribute whose value is exactly "val".
        return startWith(attrValue, selector.value.str(), cmp) == Match::YES;
    } else if (selector.match == AttributeSelector::CONTAINS) {
        // If "val" contains whitespace, it will never represent anything (since the words are separated by spaces).
        if (contains(selector.value, ' '))
            return false;

        // Also if "val" is the empty string, it will never represent anything.
        if (selector.value.len() == 0)
            return false;

        // Represents an element with the att attribute whose value is a whitespace-separated list of words,
        // one of which is exactly "val".
        return _matchContainsWord(attrValue, selector.value, cmp);
    } else if (selector.match == AttributeSelector::HYPHENATED) {
        // Represents an element with the att attribute, its value either being exactly "val"
        auto prefixMatch = startWith(attrValue, selector.value.str(), cmp);
        if (prefixMatch == Match::YES)
            return true;
        if (prefixMatch == Match::NO)
            return false;

        // or beginning with "val" immediately followed by "-" (U+002D).
        return attrValue[selector.value.len()] == '-';
    } else {
        // 6.2. Substring matching attribute selectors
        // https://www.w3.org/TR/selectors-4/#attribute-substrings

        // If "val" is the empty string then the selector does not represent anything.
        if (selector.value.len() == 0)
            return false;

        if (selector.match == AttributeSelector::STR_START_WITH) {
            // Represents an element with the att attribute whose value begins with the prefix "val".
            return startWith(attrValue, selector.value.str(), cmp) != Match::NO;
        } else if (selector.match == AttributeSelector::STR_END_WITH) {
            // Represents an element with the att attribute whose value ends with the suffix "val".
            return endWith(attrValue, selector.value.str(), cmp) != Match::NO;
        } else {
            // Represents an element with the att attribute whose value contains at least one instance of the
            // substring "val".
            return contains(attrValue, selector.value, cmp);
        }
    }
}

// https://www.w3.org/TR/selectors-4/#class-html
static bool _match(ClassSelector const& selector, MatchSubject const& subject) {
    return subject.element->classList.contains(selector.class_);
}

// https://www.w3.org/TR/selectors-4/#id-selectors
static bool _match(IdSelector const& selector, MatchSubject const& subject) {
    return subject.element->id() == selector.id;
}

static bool _match(PseudoElementSelector const& selector, MatchSubject const& subject) {
    return subject.pseudoElement == selector.type;
}

static bool _match(PseudoClassSelector const& selector, MatchSubject const& subject) {
    return matchPseudoClass(*subject.element, selector.pseudoClass);
}

// MARK: Logical Combinations --------------------------------------------------

// https://www.w3.org/TR/selectors-4/#matches
static bool _match(MatchAnyPseudoClass const& selector, MatchSubject const& subject) {
    return _matchAny(selector.selectors, subject);
}

// https://www.w3.org/TR/selectors-4/#negation
static bool _match(NegationPseudoClass const& selector, MatchSubject const& subject) {
    return not _matchAny(selector.selectors, subject);
}

// https://www.w3.org/TR/selectors-4/#zero-matches
static bool _match(SpecificityAdjustmentPseudoClass const& selector, MatchSubject const& subject) {
    return _matchAny(selector.selectors, subject);
}

static bool _match(ComplexSelectorUnit const& unit, MatchSubject const& subject);

static bool _matchRelative(RelativeComplexSelector const& selector, usize index, MatchSubject const& subject) {
    auto const& unit = selector.selectors[index];
    auto anchor = subject.element;
    auto matchHere = [&](Gc::Ref<Dom::Element> el) {
        auto candidate = subject.withElement(el);
        if (not _match(unit, candidate))
            return false;
        if (index + 1 == selector.selectors.len())
            return true;
        return _matchRelative(selector, index + 1, candidate);
    };

    switch (unit.combinator) {
    case SelectorCombinator::DESCENDANT:
        for (auto child : anchor->iterChildrenOfType<Dom::Element>())
            for (auto node : child->iterDepthFirst())
                if (auto el = node->as<Dom::Element>(); el and matchHere(*el))
                    return true;
        return false;

    case SelectorCombinator::CHILD:
        return anchor->iterChildrenOfType<Dom::Element>() | Any(matchHere);

    case SelectorCombinator::NEXT_SIBLING:
        if (auto el = anchor->nextElementSibling())
            return matchHere(*el);
        return false;

    case SelectorCombinator::SUBSEQUENT:
        for (auto el = anchor->nextElementSibling(); el; el = el->nextElementSibling())
            if (matchHere(*el))
                return true;
        return false;

    default:
        unreachable();
    }
}

// https://www.w3.org/TR/selectors-4/#relational
static bool _match(RelationalPseudoClass const& selector, MatchSubject const& subject) {
    return iter(selector.selectors) | Any([&](auto const& s) {
               return _matchRelative(s, 0, subject.originatingElement());
           });
}

// MARK: Linguistic Pseudo-classes ---------------------------------------------

static Opt<Str> _inheritedAttribute(Gc::Ref<Dom::Element> element, Dom::QualifiedName name) {
    for (Gc::Ptr<Dom::Element> curr = element; curr; curr = curr->parentElement())
        if (auto value = curr->getAttribute(name))
            return value;
    return NONE;
}

// https://www.w3.org/TR/selectors-4/#the-dir-pseudo
static bool _match(DirPseudoClass const& selector, MatchSubject const& subject) {
    auto dir = _inheritedAttribute(subject.element, Html::DIR_ATTR);
    if (dir and eqCi(*dir, "rtl"s))
        return selector.dir == Dir::RTL;
    return selector.dir == Dir::LTR;
}

// https://www.rfc-editor.org/rfc/rfc4647#section-3.3.2
static bool _matchLangRange(Str lang, Str range) {
    auto tag = iterSplit(lang, '-') | Collect<Vec<Str>>();
    auto ranges = iterSplit(range, '-') | Collect<Vec<Str>>();

    if (not tag or not ranges)
        return false;

    if (ranges[0] != "*"s and not eqCi(ranges[0], tag[0]))
        return false;

    usize i = 1, j = 1;
    while (i < ranges.len()) {
        if (ranges[i] == "*"s) {
            i++;
        } else if (j >= tag.len()) {
            return false;
        } else if (eqCi(ranges[i], tag[j])) {
            i++;
            j++;
        } else if (tag[j].len() == 1) {
            return false;
        } else {
            j++;
        }
    }

    return true;
}

// https://www.w3.org/TR/selectors-4/#the-lang-pseudo
static bool _match(LangPseudoClass const& selector, MatchSubject const& subject) {
    auto lang = _inheritedAttribute(subject.element, Html::LANG_ATTR);
    if (not lang or lang->len() == 0)
        return false;

    return iter(selector.langs) | Any([&](auto const& range) {
               return _matchLangRange(*lang, range);
           });
}

// MARK: Tree-Structural Pseudo-classes ----------------------------------------

// https://www.w3.org/TR/selectors-4/#child-index
static bool _match(ChildIndexPseudoClass const& selector, MatchSubject const& subject) {
    if (not featureNthChild.enabled)
        return false;

    auto element = subject.element;

    switch (selector.type) {
    case ChildIndexPseudoClass::FIRST_CHILD:
        return element->nthChildNumber() == 1;

    case ChildIndexPseudoClass::LAST_CHILD:
        return element->nthLastChildNumber() == 1;

    case ChildIndexPseudoClass::ONLY_CHILD:
        return element->nthChildNumber() == 1 and element->nthLastChildNumber() == 1;

    case ChildIndexPseudoClass::NTH_CHILD:
    case ChildIndexPseudoClass::NTH_LAST_CHILD: {
        bool reverse = selector.type == ChildIndexPseudoClass::NTH_LAST_CHILD;

        if (not selector.s)
            return selector.anb.match(reverse ? element->nthLastChildNumber() : element->nthChildNumber());

        if (not _matchAny(selector.s, subject.originatingElement()))
            return false;

        auto filter = [&](Gc::Ptr<Dom::Node> node) {
            auto el = node->as<Dom::Element>();
            return el and _matchAny(selector.s, subject.withElement(*el));
        };
        auto index = reverse ? element->reverseIndex(filter) : element->index(filter);
        return selector.anb.match(index + 1);
    }

    default:
        unreachable();
    }
}

// https://www.w3.org/TR/selectors-4/#typed-child-index
static bool _match(TypedChildIndexPseudoClass const& selector, MatchSubject const& subject) {
    if (not featureNthChild.enabled)
        return false;

    auto element = subject.element;

    switch (selector.type) {
    case TypedChildIndexPseudoClass::FIRST_OF_TYPE:
        return element->nthOfTypeNumber() == 1;

    case TypedChildIndexPseudoClass::LAST_OF_TYPE:
        return element->nthLastOfTypeNumber() == 1;

    case TypedChildIndexPseudoClass::ONLY_OF_TYPE:
        return element->nthOfTypeNumber() == 1 and element->nthLastOfTypeNumber() == 1;

    case TypedChildIndexPseudoClass::NTH_OF_TYPE:
        return selector.anb.match(element->nthOfTypeNumber());

    case TypedChildIndexPseudoClass::NTH_LAST_OF_TYPE:
        return selector.anb.match(element->nthLastOfTypeNumber());

    default:
        unreachable();
    }
}

// https://www.w3.org/TR/css-nesting-1/#nest-selector
static bool _match(NestingSelector const&, MatchSubject const& subject) {
    // When used in the selector of a nested style rule, the nesting selector represents the elements matched by the
    // parent rule. When used in any other context, it represents the same elements as :scope in that context (unless
    // otherwise defined).
    return subject.element->isDocumentElement();
}

// MARK: Compound Selectors ----------------------------------------------------

static bool _match(SubclassSelector const& selector, MatchSubject const& subject) {
    return selector.visit([&](auto const& s) {
        return _match(s, subject);
    });
}

// https://www.w3.org/TR/selectors-4/#compound
static bool _match(CompoundSelector const& selector, MatchSubject const& subject) {
    if (selector.type and not _match(*selector.type, subject))
        return false;

    for (auto const& subclass : selector.subclasses)
        if (not _match(subclass, subject))
            return false;

    return true;
}

// https://www.w3.org/TR/selectors-4/#pseudo-compound
static bool _match(PseudoCompoundSelector const& selector, MatchSubject const& subject) {
    if (not _match(selector.pseudoElement, subject))
        return false;

    for (auto const& pseudoClass : selector.pseudoClasses)
        if (not _match(pseudoClass, subject))
            return false;

    return true;
}

static bool _match(ComplexSelectorUnit const& unit, MatchSubject const& subject) {
    if (unit.selector and not _match(*unit.selector, subject.originatingElement()))
        return false;

    // FIXME: Add support for sub-pseudo-elements (eg. ::after::marker)
    if (unit.pseudo.len() > 1)
        return false;

    if (unit.pseudo.len() == 1)
        return _match(unit.pseudo[0], subject);

    return not subject.pseudoElement;
}

// MARK: Complex Selectors -----------------------------------------------------

static bool _matchLhs(ComplexSelector const& selector, usize rem, MatchSubject const& subject) {
    if (rem == 0)
        return true;

    auto const& unit = selector.lhs[rem - 1];
    auto element = subject.element;
    auto matchHere = [&](Gc::Ref<Dom::Element> el) {
        auto candidate = subject.withElement(el);
        return _match(unit, candidate) and _matchLhs(selector, rem - 1, candidate);
    };

    switch (unit.combinator) {
    // https://www.w3.org/TR/selectors-4/#descendant-combinators
    case SelectorCombinator::DESCENDANT:
        for (auto el = element->parentElement(); el; el = el->parentElement())
            if (matchHere(*el))
                return true;
        return false;

    // https://www.w3.org/TR/selectors-4/#child-combinators
    case SelectorCombinator::CHILD:
        if (auto el = element->parentElement())
            return matchHere(*el);
        return false;

    // https://www.w3.org/TR/selectors-4/#adjacent-sibling-combinators
    case SelectorCombinator::NEXT_SIBLING:
        if (auto el = element->previousElementSibling())
            return matchHere(*el);
        return false;

    // https://www.w3.org/TR/selectors-4/#general-sibling-combinators
    case SelectorCombinator::SUBSEQUENT:
        for (auto el = element->previousElementSibling(); el; el = el->previousElementSibling())
            if (matchHere(*el))
                return true;
        return false;

    default:
        unreachable();
    }
}

export bool match(ComplexSelector const& selector, MatchSubject const& subject) {
    if (not _match(selector.rhs, subject))
        return false;
    return _matchLhs(selector, selector.lhs.len(), subject.originatingElement());
}

// https://www.w3.org/TR/selectors-4/#match-a-selector-against-an-element
export Opt<Specificity> match(SelectorList const& selectors, MatchSubject const& subject) {
    Opt<Specificity> res = NONE;
    for (auto const& s : selectors)
        if (match(s, subject))
            res = Some(max(res.unwrapOr(Specificity::ZERO), specificity(s)));
    return res;
}

// https://www.w3.org/TR/selectors-4/#pseudo-element
export Opt<Symbol> selectorPseudoElement(ComplexSelector const& selector) {
    if (selector.rhs.pseudo)
        return Some(selector.rhs.pseudo[0].pseudoElement.type);
    return NONE;
}

} // namespace Vaev::Style
