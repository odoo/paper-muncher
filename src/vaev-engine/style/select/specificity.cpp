export module Vaev.Engine:style.select.specificity;

import Karm.Core;

import :style.select.selectors;

using namespace Karm;
using namespace Karm::Literals;

namespace Vaev::Style {

// https://www.w3.org/TR/selectors-4/#specificity
export struct Specificity {
    // a: The number of ID selectors in the selector.
    // b: The number of class selectors, attributes selectors, and pseudo-classes in the selector.
    // c: The number of type selectors and pseudo-elements in the selector.
    u16 a, b, c;

    static Specificity const ZERO, A, B, C;

    Specificity(u16 a, u16 b, u16 c)
        : a(a), b(b), c(c) {}

    // Due to storage limitations, implementations may have limitations on the size of A, B, or C. If so,
    // values higher than the limit must be clamped to that limit, and not overflow.
    Specificity operator+(Specificity const& other) const {
        return {
            saturatingAdd(a, other.a),
            saturatingAdd(b, other.b),
            saturatingAdd(c, other.c),
        };
    }

    bool operator==(Specificity const& other) const = default;
    auto operator<=>(Specificity const& other) const = default;

    void repr(Io::Emit& e) const {
        e("{}-{}-{}", a, b, c);
    }
};

inline Specificity const Specificity::ZERO = {0, 0, 0};
inline Specificity const Specificity::A = {1, 0, 0};
inline Specificity const Specificity::B = {0, 1, 0};
inline Specificity const Specificity::C = {0, 0, 1};

export Specificity const INLINE_SPEC = Specificity::ZERO;
export Specificity const PRESENTATION_ATTR_SPEC = Specificity::ZERO;
export Specificity const PRESENTATION_HINT_SPEC = Specificity::ZERO;

export Specificity specificity(ComplexSelector const&);

Specificity specificity(RelativeComplexSelector const&);

static Specificity _maxSpecificity(auto const& selectors) {
    Specificity res = Specificity::ZERO;
    for (auto const& s : selectors)
        res = max(res, specificity(s));
    return res;
}

// https://www.w3.org/TR/selectors-4/#specificity-rules

Specificity specificity(TypeSelector const& s) {
    if (s.qualifiedName.exactName())
        return Specificity::C;
    return Specificity::ZERO;
}

Specificity specificity(AttributeSelector const&) {
    return Specificity::B;
}

Specificity specificity(ClassSelector const&) {
    return Specificity::B;
}

Specificity specificity(IdSelector const&) {
    return Specificity::A;
}

Specificity specificity(PseudoElementSelector const&) {
    return Specificity::C;
}

Specificity specificity(PseudoClassSelector const&) {
    return Specificity::B;
}

// The specificity of an :is(), :not(), or :has() pseudo-class is replaced by the
// specificity of the most specific complex selector in its selector list argument.
Specificity specificity(MatchAnyPseudoClass const& s) {
    return _maxSpecificity(s.selectors);
}

Specificity specificity(NegationPseudoClass const& s) {
    return _maxSpecificity(s.selectors);
}

Specificity specificity(RelationalPseudoClass const& s) {
    return _maxSpecificity(s.selectors);
}

// The specificity of a :where() pseudo-class is replaced by zero.
Specificity specificity(SpecificityAdjustmentPseudoClass const&) {
    return Specificity::ZERO;
}

Specificity specificity(DirPseudoClass const&) {
    return Specificity::B;
}

Specificity specificity(LangPseudoClass const&) {
    return Specificity::B;
}

// The specificity of an :nth-child() or :nth-last-child() selector is the specificity of the pseudo-class itself
// (counting as one pseudo-class selector) plus the specificity of the most specific complex selector in its selector
// list argument (if any).
Specificity specificity(ChildIndexPseudoClass const& s) {
    return Specificity::B + _maxSpecificity(s.s);
}

Specificity specificity(TypedChildIndexPseudoClass const&) {
    return Specificity::B;
}

// https://www.w3.org/TR/css-nesting-1/#nest-selector
// The specificity of the nesting selector is equal to the largest specificity among the complex selectors in the
// parent style rule’s selector list (identical to the behavior of :is()), or zero if no such selector list exists.
Specificity specificity(NestingSelector const&) {
    return Specificity::ZERO;
}

Specificity specificity(SubclassSelector const& s) {
    return s.visit([](auto const& s) {
        return specificity(s);
    });
}

Specificity specificity(CompoundSelector const& s) {
    Specificity res = Specificity::ZERO;
    if (s.type)
        res = res + specificity(*s.type);
    for (auto const& subclass : s.subclasses)
        res = res + specificity(subclass);
    return res;
}

Specificity specificity(PseudoCompoundSelector const& s) {
    Specificity res = specificity(s.pseudoElement);
    for (auto const& pseudoClass : s.pseudoClasses)
        res = res + specificity(pseudoClass);
    return res;
}

Specificity specificity(ComplexSelectorUnit const& s) {
    Specificity res = Specificity::ZERO;
    if (s.selector)
        res = res + specificity(*s.selector);
    for (auto const& pseudo : s.pseudo)
        res = res + specificity(pseudo);
    return res;
}

export Specificity specificity(ComplexSelector const& s) {
    Specificity res = specificity(s.rhs);
    for (auto const& unit : s.lhs)
        res = res + specificity(static_cast<ComplexSelectorUnit const&>(unit));
    return res;
}

Specificity specificity(RelativeComplexSelector const& s) {
    Specificity res = Specificity::ZERO;
    for (auto const& unit : s.selectors)
        res = res + specificity(static_cast<ComplexSelectorUnit const&>(unit));
    return res;
}

} // namespace Vaev::Style
