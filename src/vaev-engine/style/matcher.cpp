export module Vaev.Engine:style.matcher;

import Karm.Core;
import Karm.Gc;
import Karm.Debug;
import Karm.Logger;

import :dom.element;
import :style.selector;

using namespace Karm;

namespace Vaev::Style {

static auto debugMatching = Debug::Flag::debug("web-style-matching", "Log failures to match selectors");
static auto featureNthChild = Debug::Flag::feature("web-style-nth_child", "Enable :nth-child() and related selectors");

static bool _matchSelector(Selector const& selector, Gc::Ref<Dom::Element> element, Opt<Symbol> pseudoElement);
export Opt<Spec> matchSelector(Selector const& selector, Gc::Ref<Dom::Element> element, Opt<Symbol> pseudoElement);

// https://www.w3.org/TR/selectors-4/#descendant-combinators
static bool _matchDescendant(Selector const& selector, Gc::Ref<Dom::Element> element) {
    Gc::Ptr<Dom::Node> curr = element;
    while (curr->hasParentNode()) {
        auto parent = curr->parentNode();
        if (auto el = parent->is<Dom::Element>())
            if (_matchSelector(selector, *el, NONE))
                return true;
        curr = parent;
    }
    return false;
}

// https://www.w3.org/TR/selectors-4/#child-combinators
static bool _matchChild(Selector const& selector, Gc::Ref<Dom::Element> element) {
    if (not element->hasParentNode())
        return false;

    auto parent = element->parentNode();
    if (auto el = parent->is<Dom::Element>())
        return _matchSelector(selector, *el, NONE);
    return false;
}

// https://www.w3.org/TR/selectors-4/#adjacent-sibling-combinators
static bool _matchAdjacent(Selector const& selector, Gc::Ref<Dom::Element> element) {
    if (not element->hasPreviousSibling())
        return false;

    auto prev = element->previousSibling();
    if (auto el = prev->is<Dom::Element>())
        return _matchSelector(selector, *el, NONE);
    return false;
}

// https://www.w3.org/TR/selectors-4/#general-sibling-combinators
static bool _matchSubsequent(Selector const& selector, Gc::Ref<Dom::Element> element) {
    Gc::Ptr<Dom::Node> current = element;
    while (current->hasPreviousSibling()) {
        auto prev = current->previousSibling();
        if (auto el = prev->is<Dom::Element>())
            if (_matchSelector(selector, *el, NONE))
                return true;
        current = prev;
    }
    return false;
}

static bool _match(Infix const& selector, Gc::Ref<Dom::Element> element, Opt<Symbol> pseudoElement) {
    if (not _matchSelector(selector.rhs.unwrap(), element, pseudoElement))
        return false;

    switch (selector.type) {
    case Infix::DESCENDANT: // ' '
        return _matchDescendant(*selector.lhs, element);

    case Infix::CHILD: // >
        return _matchChild(*selector.lhs, element);

    case Infix::ADJACENT: // +
        return _matchAdjacent(*selector.lhs, element);

    case Infix::SUBSEQUENT: // ~
        return _matchSubsequent(*selector.lhs, element);

    default:
        logWarnIf(debugMatching, "unimplemented selector: {}", selector);
        return false;
    }
}

static bool _match(Nfix const& selector, Gc::Ref<Dom::Element> element, Opt<Symbol> pseudoElement) {
    switch (selector.type) {
    case Nfix::AND:
        for (auto& inner : selector.inners)
            if (not _matchSelector(inner, element, pseudoElement))
                return false;
        return true;

    // 4.1. Selector Lists
    // https://www.w3.org/TR/selectors-4/#grouping
    // and
    // 4.2. The Matches-Any Pseudo-class: :is()
    // https://www.w3.org/TR/selectors-4/#matchess
    case Nfix::OR:
        for (auto& inner : selector.inners)
            if (_matchSelector(inner, element, pseudoElement))
                return true;
        return false;

    case Nfix::NOT:
        return not _matchSelector(selector.inners[0], element, pseudoElement);

    case Nfix::WHERE:
        return not _matchSelector(selector.inners[0], element, pseudoElement);

    default:
        logWarnIf(debugMatching, "unimplemented selector: {}", selector);
        return false;
    }
}

// 5.1. Type (tag name) selector
// https://www.w3.org/TR/selectors-4/#type
static bool _match(TypeSelector const& selector, Gc::Ref<Dom::Element> element) {
    return selector.qualifiedName.match(element->qualifiedName);
}

static bool _match(IdSelector const& selector, Gc::Ref<Dom::Element> element) {
    return element->id() == selector.id;
}

static bool _match(ClassSelector const& selector, Gc::Ref<Dom::Element> element) {
    return element->classList.contains(selector.class_);
}

// 6. Attribute Selector
// https://www.w3.org/TR/selectors-4/#attribute-selectors
static bool _match(AttributeSelector const& selector, Gc::Ref<Dom::Element> element) {
    // TODO: What should we do if there are multiple attributes
    //       with the same name but different namespaces?
    auto maybeAttrValue = selector.qualifiedName.isWildcard()
                              ? element->getAttributeUnqualified(selector.qualifiedName.name.unwrap("unexpected wildcard attribute selector"))
                              : element->getAttribute(selector.qualifiedName.fullyQualified());

    if (selector.match == AttributeSelector::PRESENT) {
        // Represents an element with the att attribute, whatever the value of the attribute.
        return maybeAttrValue != NONE;
    }

    if (not maybeAttrValue)
        return false;

    auto attrValue = maybeAttrValue.unwrap();

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
        for (auto piece : split(attrValue, ' ')) {
            if (piece.len() == 0)
                continue;

            if (startWith(piece, selector.value, cmp) == Match::YES)
                return true;
        }

        return false;
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

// 8.2. The Link History Pseudo-classes: :link and :visited
// https://www.w3.org/TR/selectors-4/#link

static bool _matchLink(Gc::Ref<Dom::Element> element) {
    return element->qualifiedName == Html::A_TAG and element->hasAttribute(Html::HREF_ATTR);
}

// 14.3.1. :nth-child() pseudo-class
// 14.3.2. :nth-last-child() pseudo-class
// 14.3.3. :first-child pseudo-class
// 14.3.4. :last-child pseudo-class
// https://www.w3.org/TR/selectors-4/#the-nth-child-pseudo
// https://www.w3.org/TR/selectors-4/#the-nth-last-child-pseudo
// https://www.w3.org/TR/selectors-4/#the-first-child-pseudo
// https://www.w3.org/TR/selectors-4/#the-last-child-pseudo
static bool _matchNthChild(PseudoClassSelector::AnBofS const& anbOfS, Gc::Ref<Dom::Element> element, bool reverseLookup) {
    if (not featureNthChild.enabled)
        return false;

    auto [anb, selector] = anbOfS;
    if (selector) {
        if (not matchSelector(*(selector.unwrap()), *element, NONE))
            return false;

        auto filterFunc = [&](Gc::Ptr<Dom::Node> node) {
            auto el = node->is<Dom::Element>();
            return el ? matchSelector(*(selector.unwrap()), *el, NONE) != NONE : false;
        };
        auto index = reverseLookup ? element->reverseIndex(filterFunc) : element->index(filterFunc);
        return anb.match(index + 1);
    }

    auto filterFunc = [&](Gc::Ptr<Dom::Node> node) {
        return node->is<Dom::Element>() != NONE;
    };
    auto index = reverseLookup ? element->reverseIndex(filterFunc) : element->index(filterFunc);
    return anb.match(index + 1);
}

// 14.4.1. :nth-of-type pseudo-class
// 14.4.2. :nth-last-of-type pseudo-class
// 14.4.3. :first-of-type pseudo-class
// 14.4.4. :last-of-type pseudo-class
// https://www.w3.org/TR/selectors-4/#the-nth-of-type-pseudo
// https://www.w3.org/TR/selectors-4/#the-nth-last-of-type-pseudo
// https://www.w3.org/TR/selectors-4/#the-first-of-type-pseudo
// https://www.w3.org/TR/selectors-4/#the-last-of-type-pseudo
static bool _matchNthOfType(AnB const& anb, Gc::Ref<Dom::Element> element, bool reverseLookup) {
    if (not featureNthChild.enabled)
        return false;

    auto name = element->qualifiedName;

    auto filterFunc = [&](Gc::Ptr<Dom::Node> node) {
        auto el = node->is<Dom::Element>();
        return el ? el->qualifiedName == name : false;
    };

    auto index = reverseLookup
                     ? element->reverseIndex(filterFunc)
                     : element->index(filterFunc);
    return anb.match(index + 1);
}

static bool _match(PseudoElementSelector const& selector, Gc::Ref<Dom::Element>, Opt<Symbol> pseudoElement) {
    if (not pseudoElement)
        return false;

    return selector.type == pseudoElement;
}

static bool _match(PseudoClassSelector const& selector, Gc::Ref<Dom::Element> element) {
    switch (selector.type) {
    case PseudoClassSelector::LINK:
        return _matchLink(element);

    case PseudoClassSelector::ROOT:
        return element->qualifiedName == Html::HTML_TAG;

    case PseudoClassSelector::FIRST_OF_TYPE:
        return _matchNthOfType(AnB{0, 1}, element, false);

    case PseudoClassSelector::LAST_OF_TYPE:
        return _matchNthOfType(AnB{0, 1}, element, true);

    case PseudoClassSelector::FIRST_CHILD:
        return _matchNthChild(PseudoClassSelector::AnBofS{AnB{0, 1}, NONE}, element, false);

    case PseudoClassSelector::LAST_CHILD:
        return _matchNthChild(PseudoClassSelector::AnBofS{AnB{0, 1}, NONE}, element, true);

    case PseudoClassSelector::NTH_CHILD:
        return _matchNthChild(selector.extra.unwrap<PseudoClassSelector::AnBofS>("unexpected missing AnB"), element, false);

    case PseudoClassSelector::NTH_LAST_CHILD:
        return _matchNthChild(selector.extra.unwrap<PseudoClassSelector::AnBofS>("unexpected missing AnB"), element, true);

    case PseudoClassSelector::NTH_OF_TYPE:
        return _matchNthOfType(selector.extra.unwrap<PseudoClassSelector::AnBofS>("unexpected missing AnB").v0, element, false);

    case PseudoClassSelector::NTH_LAST_OF_TYPE:
        return _matchNthOfType(selector.extra.unwrap<PseudoClassSelector::AnBofS>("unexpected missing AnB").v0, element, true);

    default:
        logDebugIf(debugMatching, "unimplemented pseudo class: {}", selector);
        return false;
    }
}

// MARK: Selector --------------------------------------------------------------

static bool _matchSelector(Selector const& selector, Gc::Ref<Dom::Element> element, Opt<Symbol> pseudoElement) {
    // Route the selector to the appropriate matching function.
    return selector.visit(Visitor{[&](auto const& s) {
        if constexpr (requires { _match(s, element, pseudoElement); })
            return _match(s, element, pseudoElement);
        if constexpr (requires { _match(s, element); })
            return _match(s, element);

        logWarnIf(debugMatching, "unimplemented selector: {}", s);
        return false;
    }});
}

export Opt<Spec> matchSelector(Selector const& selector, Gc::Ref<Dom::Element> element, Opt<Symbol> pseudoElement = NONE) {
    if (auto n = selector.is<Nfix>(); n and n->type == Nfix::OR) {
        Opt<Spec> specificity;
        for (auto& inner : n->inners) {
            if (_matchSelector(inner, element, pseudoElement))
                specificity = max(specificity, spec(inner));
        }
        return specificity;
    }

    if (_matchSelector(selector, element, pseudoElement))
        return spec(selector);

    return NONE;
}

} // namespace Vaev::Style
