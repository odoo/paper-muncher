export module Vaev.Engine:style.select.selectors;

import Karm.Core;

import :dom.element;
import :style.select.anb;
import :style.select.pseudoClass;
import :style.select.qualifiedName;

using namespace Karm;
using namespace Karm::Literals;

namespace Vaev::Style {

export struct ComplexSelector;

// https://www.w3.org/TR/selectors-4/#selector-list
export using SelectorList = Vec<ComplexSelector>;

export struct RelativeComplexSelector;

// https://www.w3.org/TR/selectors-4/#typedef-relative-selector-list
export using RelativeSelectorList = Vec<RelativeComplexSelector>;

// https://www.w3.org/TR/selectors-4/#type-selector
// https://www.w3.org/TR/selectors-4/#universal-selector
export struct TypeSelector {
    QualifiedNameSelector qualifiedName;

    static TypeSelector universal() {
        return {
            .qualifiedName = {UNIVERSAL, UNIVERSAL},
        };
    }

    bool operator==(TypeSelector const&) const = default;

    void repr(Io::Emit& e) const {
        e("(type-selector {})", qualifiedName);
    }
};

// https://www.w3.org/TR/selectors-4/#attribute-selector
export struct AttributeSelector {
    enum Case {
        SENSITIVE,
        INSENSITIVE,

        _LEN0,
    };

    enum Match {
        PRESENT, //< [attr]

        EXACT,      //< [attr="value"]
        CONTAINS,   //< [attr~="value"]
        HYPHENATED, //< [attr|="value"]

        STR_START_WITH, //< [attr^="value"]
        STR_END_WITH,   //< [attr$="value"]
        STR_CONTAIN,    //< [attr*="value"]

        _LEN1,
    };

    QualifiedNameSelector qualifiedName;
    Case case_ = SENSITIVE;
    Match match = PRESENT;
    String value = ""s;

    bool operator==(AttributeSelector const&) const = default;

    void repr(Io::Emit& e) const {
        e("(attribute-selector {} {} {} {#})", qualifiedName, case_, match, value);
    }
};

// https://www.w3.org/TR/selectors-4/#class-selector
export struct ClassSelector {
    String class_;

    bool operator==(ClassSelector const&) const = default;

    void repr(Io::Emit& e) const {
        e("(class-selector {})", class_);
    }
};

// https://www.w3.org/TR/selectors-4/#id-selector
export struct IdSelector {
    Symbol id;

    bool operator==(IdSelector const&) const = default;

    void repr(Io::Emit& e) const {
        e("(id-selector {})", id);
    }
};

// https://www.w3.org/TR/selectors-4/#pseudo-elements
export struct PseudoElementSelector {
    Symbol type;

    bool operator==(PseudoElementSelector const&) const = default;

    void repr(Io::Emit& e) const {
        e("(pseudo-element-selector {})", type);
    }
};

// https://www.w3.org/TR/selectors-4/#pseudo-class
export struct PseudoClassSelector {
    PseudoClass pseudoClass;

    bool operator==(PseudoClassSelector const&) const = default;

    void repr(Io::Emit& e) const {
        e("(pseudo-class-selector {})", serializePseudoClass(pseudoClass));
    }
};

// https://www.w3.org/TR/selectors-4/#matches
export struct MatchAnyPseudoClass {
    SelectorList selectors;

    bool operator==(MatchAnyPseudoClass const&) const;

    void repr(Io::Emit& e) const {
        e("(match-any-pseudo-class {})", selectors);
    }
};

// https://www.w3.org/TR/selectors-4/#negation
export struct NegationPseudoClass {
    SelectorList selectors;

    bool operator==(NegationPseudoClass const&) const;

    void repr(Io::Emit& e) const {
        e("(negation-pseudo-class {})", selectors);
    }
};

// https://www.w3.org/TR/selectors-4/#zero-matches
export struct SpecificityAdjustmentPseudoClass {
    SelectorList selectors;

    bool operator==(SpecificityAdjustmentPseudoClass const&) const;

    void repr(Io::Emit& e) const {
        e("(specificity-adjustment-pseudo-class {})", selectors);
    }
};

// https://www.w3.org/TR/selectors-4/#relational
export struct RelationalPseudoClass {
    RelativeSelectorList selectors;

    bool operator==(RelationalPseudoClass const&) const;

    void repr(Io::Emit& e) const {
        e("(relational-pseudo-class {})", selectors);
    }
};

export enum struct Dir {
    LTR,
    RTL,

    _LEN,
};

// https://www.w3.org/TR/selectors-4/#the-dir-pseudo
export struct DirPseudoClass {
    Dir dir;

    bool operator==(DirPseudoClass const&) const = default;

    void repr(Io::Emit& e) const {
        e("(dir-pseudo-class {})", dir);
    }
};

// https://www.w3.org/TR/selectors-4/#the-lang-pseudo
export struct LangPseudoClass {
    Vec<String> langs;

    bool operator==(LangPseudoClass const&) const = default;

    void repr(Io::Emit& e) const {
        e("(lang-pseudo-class {})", langs);
    }
};

// https://www.w3.org/TR/selectors-4/#child-index
export struct ChildIndexPseudoClass {
    enum Type {
        NTH_CHILD,
        NTH_LAST_CHILD,
        FIRST_CHILD,
        LAST_CHILD,
        ONLY_CHILD,

        _LEN,
    };

    Type type;
    AnB anb = {0, 1};
    SelectorList s = {};

    bool operator==(ChildIndexPseudoClass const&) const;

    void repr(Io::Emit& e) const {
        e("(child-index-pseudo-class {} {} {})", type, anb, s);
    }
};

// https://www.w3.org/TR/selectors-4/#typed-child-index
export struct TypedChildIndexPseudoClass {
    enum Type {
        NTH_OF_TYPE,
        NTH_LAST_OF_TYPE,
        FIRST_OF_TYPE,
        LAST_OF_TYPE,
        ONLY_OF_TYPE,

        _LEN,
    };

    Type type;
    AnB anb = {0, 1};

    bool operator==(TypedChildIndexPseudoClass const&) const = default;

    void repr(Io::Emit& e) const {
        e("(typed-child-index-pseudo-class {} {})", type, anb);
    }
};

// https://www.w3.org/TR/css-nesting-1/#nest-selector
export struct NestingSelector {
    bool operator==(NestingSelector const&) const = default;

    void repr(Io::Emit& e) const {
        e("(nesting-selector)");
    }
};

// https://www.w3.org/TR/selectors-4/#typedef-subclass-selector
export using SubclassSelector = Union<
    AttributeSelector,
    ClassSelector,
    IdSelector,
    PseudoClassSelector,
    MatchAnyPseudoClass,
    NegationPseudoClass,
    SpecificityAdjustmentPseudoClass,
    RelationalPseudoClass,
    DirPseudoClass,
    LangPseudoClass,
    ChildIndexPseudoClass,
    TypedChildIndexPseudoClass,
    NestingSelector>;

// https://www.w3.org/TR/selectors-4/#typedef-pseudo-compound-selector
export struct PseudoCompoundSelector {
    PseudoElementSelector pseudoElement;
    Vec<PseudoClassSelector> pseudoClasses = {};

    bool operator==(PseudoCompoundSelector const&) const = default;

    void repr(Io::Emit& e) const {
        e("(pseudo-compound-selector {} {})", pseudoElement, pseudoClasses);
    }
};

// https://www.w3.org/TR/selectors-4/#compound
export struct CompoundSelector {
    Opt<TypeSelector> type = NONE;
    Vec<SubclassSelector> subclasses = {};

    bool operator==(CompoundSelector const&) const = default;

    void repr(Io::Emit& e) const {
        e("(compound-selector {} {})", type, subclasses);
    }
};

// https://www.w3.org/TR/selectors-4/#combinators
export enum struct SelectorCombinator {
    DESCENDANT,   //< ' '
    CHILD,        //< >
    NEXT_SIBLING, //< +
    SUBSEQUENT,   //< ~

    _LEN,
};

export Str serializeCombinator(SelectorCombinator combinator) {
    switch (combinator) {
    case SelectorCombinator::DESCENDANT:
        return " ";
    case SelectorCombinator::CHILD:
        return " > ";
    case SelectorCombinator::NEXT_SIBLING:
        return " + ";
    case SelectorCombinator::SUBSEQUENT:
        return " ~ ";
    default:
        unreachable();
    }
}

// https://www.w3.org/TR/selectors-4/#typedef-complex-selector-unit
export struct ComplexSelectorUnit {
    Opt<CompoundSelector> selector = NONE;
    Vec<PseudoCompoundSelector> pseudo = {};

    bool operator==(ComplexSelectorUnit const&) const = default;

    void repr(Io::Emit& e) const {
        e("(complex-selector-unit {} {})", selector, pseudo);
    }
};

export struct CombinatedComplexSelectorUnit : ComplexSelectorUnit {
    SelectorCombinator combinator;

    bool operator==(CombinatedComplexSelectorUnit const&) const = default;

    void repr(Io::Emit& e) const {
        e("(combinated-complex-selector-unit {} {} {})", selector, pseudo, combinator);
    }
};

// https://www.w3.org/TR/selectors-4/#complex
// NOTE: Each unit of `lhs` is linked to the unit that follows it (or to `rhs`)
//       by its combinator, `rhs` being the subject of the selector.
export struct ComplexSelector {
    Vec<CombinatedComplexSelectorUnit> lhs = {};
    ComplexSelectorUnit rhs = {};

    bool operator==(ComplexSelector const&) const = default;

    void repr(Io::Emit& e) const {
        e("(complex-selector {} {})", lhs, rhs);
    }
};

// https://www.w3.org/TR/selectors-4/#typedef-relative-selector
// NOTE: Each unit is linked to the unit that precedes it (or to the anchor
//       element for the first one) by its combinator.
export struct RelativeComplexSelector {
    Vec<CombinatedComplexSelectorUnit> selectors = {};

    bool operator==(RelativeComplexSelector const&) const = default;

    void repr(Io::Emit& e) const {
        e("(relative-complex-selector {})", selectors);
    }
};

bool MatchAnyPseudoClass::operator==(MatchAnyPseudoClass const&) const = default;

bool NegationPseudoClass::operator==(NegationPseudoClass const&) const = default;

bool SpecificityAdjustmentPseudoClass::operator==(SpecificityAdjustmentPseudoClass const&) const = default;

bool RelationalPseudoClass::operator==(RelationalPseudoClass const&) const = default;

bool ChildIndexPseudoClass::operator==(ChildIndexPseudoClass const&) const = default;

} // namespace Vaev::Style
