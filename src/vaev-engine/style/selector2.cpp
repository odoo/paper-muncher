module;

#include <karm/macros>

export module Vaev.Engine:style.selector2;

import Karm.Core;
import :css.parser;
import :dom.element;

using namespace Karm;
using namespace Karm::Literals;

// https://www.w3.org/TR/selectors-4

namespace Vaev::Style {

// https://www.w3.org/TR/selectors-4/#combinators
enum struct SelectorCombinator {
    DESCENDANT,   // ' '
    CHILD,        // >
    NEXT_SIBLING, // +
    SUBSEQUENT,   // ~

    _LEN,
};

export struct NullNamespace {
    bool operator==(NullNamespace const&) const = default;
};

export constexpr NullNamespace NULL_NAMESPACE = {};

export struct Universal {
    bool operator==(Universal const&) const = default;
};

export constexpr Universal UNIVERSAL = {};

export struct QualifiedNameSelector {
    using NamespacePattern = Union<
        NullNamespace, // represents the name B that belongs to no namespace.
        Universal,     // represents the name C in any namespace, including no namespace.
        Symbol>;

    using NamePattern = Union<Universal, Symbol>;

    NamespacePattern ns; // Null namespace, universal namespace, or an exact namespace.
    NamePattern name;    // Universal name or an exact name.

    QualifiedNameSelector()
        : ns(UNIVERSAL), name(UNIVERSAL) {}

    QualifiedNameSelector(NamespacePattern ns, NamePattern name)
        : ns(ns), name(name) {}

    QualifiedNameSelector(Symbol ns, Symbol name)
        : QualifiedNameSelector(NamespacePattern{ns}, NamePattern{name}) {}

    static QualifiedNameSelector wildcard() {
        return {UNIVERSAL, UNIVERSAL};
    }

    Opt<Symbol> exactName() const {
        if (auto exact = name.is<Symbol>())
            return Some(*exact);
        return NONE;
    }

    bool match(Dom::QualifiedName const& q) const {
        if (
            not ns.visit(
                [&](NullNamespace const&) {
                    return q.ns == NONE;
                },
                [&](Universal const&) {
                    return true;
                },
                [&](Symbol const& exact) {
                    return q.ns == exact;
                }
            )
        )
            return false;

        if (
            not name.visit(
                [&](Universal const&) {
                    return true;
                },
                [&](Symbol const& exact) {
                    return q.name == exact;
                }
            )
        )
            return false;

        return true;
    }

    Dom::QualifiedName fullyQualified() const {
        auto exactName = this->exactName();

        if (ns.is<Universal>() or not exactName)
            panic("cannot fully qualify a wildcard selector");

        if (ns.is<NullNamespace>())
            return {NONE, exactName.expect()};

        return {Some(*ns.is<Symbol>()), exactName.expect()};
    }

    bool operator==(QualifiedNameSelector const&) const = default;

    void repr(Io::Emit& e) const {
        auto exactName = this->exactName();

        if (ns.is<Universal>() and exactName)
            e("*|{}", exactName.expect());
        else if (auto exactNs = ns.is<Symbol>(); exactNs and exactName)
            e("{}|{}", *exactNs, exactName.expect());
        else if (exactName)
            e("{}", exactName.expect());
        else if (ns.is<Universal>())
            e("*");
        else if (auto exactNs = ns.is<Symbol>())
            e("{}|*", *exactNs);
        else
            e("|*");
    }
};

// MARK: Selectors -------------------------------------------------------------

struct ComplexSelector;

// https://www.w3.org/TR/selectors-4/#selector-list
using SelectorList = Vec<ComplexSelector>;

// https://www.w3.org/TR/selectors-4/#type-selector
// https://www.w3.org/TR/selectors-4/#universal-selector
struct TypeSelector {
    QualifiedNameSelector qualifiedName;

    bool operator==(TypeSelector const&) const = default;
};

// https://www.w3.org/TR/selectors-4/#attribute-selector
struct AttributeSelector {
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
    Case case_;
    Match match;
    String value;

    bool operator==(AttributeSelector const&) const = default;
};

// https://www.w3.org/TR/selectors-4/#class-selector
struct ClassSelector {
    String class_;

    bool operator==(ClassSelector const&) const = default;
};

// https://www.w3.org/TR/selectors-4/#id-selector
struct IdSelector {
    Symbol id;

    bool operator==(IdSelector const&) const = default;

    void repr(Io::Emit& e) const {
        e("(id-selector)");
    }
};

// https://www.w3.org/TR/selectors-4/#pseudo-elements
struct PseudoElementSelector {
    Symbol type;

    bool operator==(PseudoElementSelector const&) const = default;

    void repr(Io::Emit& e) const {
        e("{}", type);
    }
};

enum struct PseudoClass {
#define PSEUDO(ID, ...) ID,
#include "defs/pseudo-class.inc"

#undef PSEUDO
    _LEN,
};

// https://www.w3.org/TR/selectors-4/#pseudo-class
struct PseudoClassSelector {
    PseudoClass pseudoClass;

    bool operator==(PseudoClassSelector const&) const = default;

    void repr(Io::Emit& e) const {
        e("(pseudo-class {})", pseudoClass);
    }
};

// https://www.w3.org/TR/selectors-4/#matches
struct MatchAnyPseudoClass {
    bool operator==(MatchAnyPseudoClass const&) const = default;

    void repr(Io::Emit& e) const {
        e("(match-any-pseudo-class)");
    }
};

// https://www.w3.org/TR/selectors-4/#negation
struct NegationPseudoClass {
    bool operator==(NegationPseudoClass const&) const = default;

    void repr(Io::Emit& e) const {
        e("(negation-pseudo-class)");
    }
};

// https://www.w3.org/TR/selectors-4/#zero-matches
struct SpecificityAdjustmentPseudoClass {
    bool operator==(SpecificityAdjustmentPseudoClass const&) const = default;

    void repr(Io::Emit& e) const {
        e("(specificity-adjustment-pseudo-class)");
    }
};

// https://www.w3.org/TR/selectors-4/#relational
struct RelationalPseudoClass {
    bool operator==(RelationalPseudoClass const&) const = default;

    void repr(Io::Emit& e) const {
        e("(relational-pseudo-class)");
    }
};

export enum struct Dir {
    LTR,
    RTL,
};

// https://www.w3.org/TR/selectors-4/#the-dir-pseudo
struct DirPseudoClass {
    Dir dir;

    bool operator==(DirPseudoClass const&) const = default;

    void repr(Io::Emit& e) const {
        e("(dir-pseudo-class)");
    }
};

// https://www.w3.org/TR/selectors-4/#the-dir-pseudo
struct LangPseudoClass {
    bool operator==(LangPseudoClass const&) const = default;

    void repr(Io::Emit& e) const {
        e("(lang-pseudo-class)");
    }
};

// https://www.w3.org/TR/selectors-4/#the-root-pseudo
struct RootPseudoClass {
    bool operator==(RootPseudoClass const&) const = default;

    void repr(Io::Emit& e) const {
        e("(root-pseudo-class)");
    }
};

// https://www.w3.org/TR/selectors-4/#the-empty-pseudo
struct EmptyPseudoClass {
    bool operator==(EmptyPseudoClass const&) const = default;

    void repr(Io::Emit& e) const {
        e("(empty-pseudo-class)");
    }
};

// https://www.w3.org/TR/css-syntax-3/#the-anb-type
export struct AnB {
    Integer a, b;

    void repr(Io::Emit& e) const {
        e("{}n{}{}", a, b < 0 ? ""s : "+"s, b);
    }

    bool operator==(AnB const&) const = default;

    bool match(usize index) const {
        // is index = An + B for a non-negative integer n >= 0
        Integer intIndex = (Integer)index - b;

        if (a == 0)
            return intIndex == 0;

        if (intIndex % a != 0)
            return false;

        return intIndex / a >= 0;
    }

    static Res<Integer> _parseAfterN(Cursor<Css::Sst>& cur) {
        eatWhitespace(cur);
        if (cur.ended())
            return Ok(0);

        if (cur.peek() == Css::Token::NUMBER) {
            if (cur->token.data[0] != '+' and cur->token.data[0] != '-')
                return Error::invalidData("expected '+' or '-' in AnB expression after 'n'");

            return Ok(try$(parseValue<Integer>(cur)));
        } else if (cur.peek() == Css::Token::DELIM) {
            if (cur->token.data != "+"s and cur->token.data != "-"s)
                return Error::invalidData("expected '+' or '-' in AnB expression after 'n'");

            bool isNegative = false;
            if (cur->token.data == "-"s)
                isNegative = true;
            cur.next();
            eatWhitespace(cur);

            if (not cur.ended() and (cur->token.data[0] == '+' or cur->token.data[0] == '-'))
                return Error::invalidData("unexpected '+' or '-' in AnB expression after 'n'");
            Integer unsigedB = try$(parseValue<Integer>(cur));
            return Ok(isNegative ? -unsigedB : unsigedB);
        }

        return Ok(0);
    }

    static Res<Integer> _parseAfterNDash(Cursor<Css::Sst>& cur) {
        eatWhitespace(cur);
        if (cur.ended() or cur.peek() != Css::Token::NUMBER)
            return Error::invalidData("expected number");

        if (not isAsciiDigit(cur->token.data[0]))
            return Error::invalidData("expected unsigned number");

        return Ok(try$(parseValue<Integer>(cur)));
    }

    static Res<AnB> _parseIdent(Cursor<Css::Sst>& cur, bool precededByPlus) {
        auto const& ident = cur->token.data;

        usize digitCount = 0;
        while (digitCount < ident.len() and isAsciiDigit(ident[ident.len() - 1 - digitCount])) {
            digitCount++;
        }

        cur.next();
        eatWhitespace(cur);

        if (ident[0] == '-' and precededByPlus)
            return Error::invalidData("unexpected '+' before negative A in AnB expression");

        if (digitCount == 0) {
            if (ident == "n"s) {
                return Ok(AnB{1, try$(_parseAfterN(cur))});
            } else if (ident == "-n"s) {
                return Ok(AnB{-1, try$(_parseAfterN(cur))});
            } else if (ident == "n-") {
                return Ok(AnB{1, -try$(_parseAfterNDash(cur))});
            } else if (ident == "-n-") {
                return Ok(AnB{-1, -try$(_parseAfterNDash(cur))});
            }
        } else {
            if (ident[ident.len() - 1 - digitCount] == '-')
                digitCount++;

            auto B = Io::atoi(Str{ident.str().end() - digitCount, digitCount});
            if (not B)
                return Error::invalidData("invalid B in AnB expression");

            Str nPart = {ident.str().begin(), ident.len() - digitCount};
            if (nPart == "n"s) {
                return Ok(AnB{1, *B});
            } else if (nPart == "-n"s) {
                return Ok(AnB{-1, *B});
            }
        }

        return Error::invalidData("invalid AnB expression");
    }

    static Res<AnB> _parseDimension(Cursor<Css::Sst>& cur, bool precededByPlus) {
        Io::SScan scan = cur->token.data.str();
        Opt<Integer> A = Io::atoi(scan);

        if (not A)
            return Error::invalidData("expected integer for A in AnB expression");

        if (*A < 0 and precededByPlus)
            return Error::invalidData("unexpected '+' before negative A in AnB expression");

        auto unit = scan.remStr();
        usize digitCount = 0;
        while (digitCount < unit.len() and isAsciiDigit(unit[unit.len() - 1 - digitCount])) {
            digitCount++;
        }

        auto nUnit = Str{unit.begin(), unit.len() - digitCount};
        if (nUnit != "n"s and nUnit != "n-"s) {
            return Error::invalidData("ill-formed 'n' in AnB expression");
        }

        cur.next();
        eatWhitespace(cur);

        if (digitCount == 0) {
            if (nUnit == "n"s) {
                return Ok(AnB{*A, try$(_parseAfterN(cur))});
            } else if (nUnit == "n-"s) {
                return Ok(AnB{*A, -try$(_parseAfterNDash(cur))});
            }
        } else {
            if (nUnit == "n-"s)
                digitCount++;

            Opt<Integer> B = Io::atoi(Str{unit.end() - digitCount, digitCount});
            if (not B)
                return Error::invalidData("invalid B in AnB expression");
            return Ok(AnB{*A, *B});
        }

        return Error::invalidData("expected dimension in AnB expression");
    }

    // https://www.w3.org/TR/css-syntax-3/#the-anb-type
    static Res<AnB> parse(Cursor<Css::Sst>& cur) {
        eatWhitespace(cur);

        if (cur.ended())
            return Error::invalidData("expected AnB expression");

        if (cur.peek() == Css::Token::IDENT) {
            auto const& ident = cur->token.data;
            if (ident == "odd" or ident == "even") {
                cur.next();
                return Ok(AnB{2, ident == "odd"});
            }
        }

        if (cur.peek() == Css::Token::NUMBER) {
            return Ok(AnB{0, try$(parseValue<Integer>(cur))});
        }

        bool precededByPlus = false;
        if (cur.peek() == Css::Token::DELIM and cur->token.data == "+"s) {
            cur.next();
            if (cur.ended() or cur.peek() == Css::Token::WHITESPACE)
                return Error::invalidData("expected 'n' after '+' in AnB expression");
            precededByPlus = true;
        }

        if (cur.peek() == Css::Token::IDENT) {
            return Ok(try$(_parseIdent(cur, precededByPlus)));
        } else if (cur.peek() == Css::Token::DIMENSION) {
            return Ok(try$(_parseDimension(cur, precededByPlus)));
        }

        return Error::invalidData("expected AnB expression");
    }

    static Res<AnB> parse(Io::SScan& s) {
        Css::Lexer lex = s;
        Diag::Collector diags = Diag::Collector::ignore();
        auto val = consumeSelector(lex, diags);
        Cursor<Css::Sst> c{val};
        return parse(c);
    };

    static Res<AnB> parse(Str input) {
        Io::SScan s{input};
        return parse(s);
    };
};

// https://www.w3.org/TR/selectors-4/#child-index
struct ChildIndexPseudoClass {
    AnB anb;
    SelectorList s;

    bool operator==(ChildIndexPseudoClass const&) const = default;

    void repr(Io::Emit& e) const {
        e("(child-index-pseudo-class AnB:{} s:{})", anb, s);
    }
};

// https://www.w3.org/TR/selectors-4/#typed-child-index
struct TypedChildIndexPseudoClass {
    AnB anb;

    bool operator==(TypedChildIndexPseudoClass const&) const = default;

    void repr(Io::Emit& e) const {
        e("(typed-child-index-pseudo-class AnB:{})", anb);
    }
};

// https://www.w3.org/TR/css-nesting-1/#nest-selector
struct NestingSelector {
    SelectorList selector;

    bool operator==(NestingSelector const&) const = default;

    void repr(Io::Emit& e) const {
        e("(nesting-selector)");
    }
};

// https://www.w3.org/TR/selectors-4/#simple
using SimpleSelector = Union<
    TypeSelector,
    AttributeSelector,
    ClassSelector,
    IdSelector,
    PseudoElementSelector,
    PseudoClassSelector,
    MatchAnyPseudoClass,
    NegationPseudoClass,
    SpecificityAdjustmentPseudoClass,
    RelationalPseudoClass,
    DirPseudoClass,
    LangPseudoClass,
    RootPseudoClass,
    EmptyPseudoClass,
    ChildIndexPseudoClass,
    TypedChildIndexPseudoClass,
    NestingSelector>;

// https://www.w3.org/TR/selectors-4/#compound
struct CompoundSelector {
    Vec<SimpleSelector> selectors;

    bool operator==(CompoundSelector const&) const = default;

    void repr(Io::Emit& e) const {
        e("(compound-selector {})", selectors);
    }
};

// https://www.w3.org/TR/selectors-4/#complex
struct ComplexSelector {
    struct Lhs {
        CompoundSelector selector;
        SelectorCombinator combinator;

        void repr(Io::Emit& e) const {
            e("{} {} ", selector, combinator);
        }
    };

    Vec<Lhs> lhs;
    CompoundSelector rhs;

    bool operator==(ComplexSelector const&) const = default;

    void repr(Io::Emit& e) const {
        e("(complex-selector {}{})", lhs, rhs);
    }
};

struct Selector {
    SelectorList selectors;

    bool operator==(Selector const&) const = default;
};

// MARK: Parser ================================================================

// https://www.w3.org/TR/selectors-4/#typedef-combinator
Res<SelectorCombinator> _parseCombinator(Cursor<Css::Sst>& s) {
    // <combinator> = '>' | '+' | '~' | [ '|' '|' ]
    if (s.skipIf(Css::tokenDataIs(">")))
        return Ok(SelectorCombinator::CHILD);
    else if (s.skipIf(Css::tokenDataIs("+")))
        return Ok(SelectorCombinator::NEXT_SIBLING);
    else if (s.skipIf(Css::tokenDataIs("~")))
        return Ok(SelectorCombinator::SUBSEQUENT);
    else
        return Error::invalidData("expected <combinator>");
}

// https://drafts.csswg.org/css-namespaces/#scope
export struct NamespaceScope {
    Opt<Symbol> default_ = NONE;
    Map<Symbol, Symbol> prefixes = {};

    QualifiedNameSelector::NamespacePattern defaultNamespacePattern() const {
        return default_
                   ? QualifiedNameSelector::NamespacePattern{default_.expect()}
                   : QualifiedNameSelector::NamespacePattern{UNIVERSAL};
    }

    Res<Symbol> lookup(Symbol sym) const {
        auto maybeRes = prefixes.lookup(sym);
        if (maybeRes == NONE)
            return Error::invalidInput("unknown namespace prefix");
        return Ok(*maybeRes);
    }
};

static Res<QualifiedNameSelector> _parseQualifiedNameSelector(Cursor<Css::Sst>& s, NamespaceScope const& ns, QualifiedNameSelector::NamespacePattern defaultNamespace) {
    // ns|name
    // ns|*
    // *|name
    // *|*
    // name
    if (s.ended())
        return Error::invalidData("expected qualified name selector, got empty input");

    enum struct NamespacePrefix {
        OMITTED,
        IDENT,
        STAR,
    };

    auto namespacePrefix = NamespacePrefix::OMITTED;
    Opt<Symbol> firstName = NONE;
    if (s->token == Css::Token::IDENT) {
        namespacePrefix = NamespacePrefix::IDENT;
        firstName = Some(Symbol::from(s->token.data));
        s.next();
    } else if (s.skip(Css::Token::delim("*"))) {
        namespacePrefix = NamespacePrefix::STAR;
        firstName = NONE;
    }

    if (not s.skip(Css::Token::delim("|"))) {
        return Ok(QualifiedNameSelector{
            defaultNamespace,
            firstName ? QualifiedNameSelector::NamePattern{firstName.expect()} : QualifiedNameSelector::NamePattern{UNIVERSAL},
        });
    }

    Opt<Symbol> secondName = NONE;

    if (s->token == Css::Token::IDENT) {
        secondName = Some(Symbol::from(s->token.data));
        s.next();
    } else if (s.skip(Css::Token::delim("*"))) {
        secondName = NONE;
    }

    if (namespacePrefix == NamespacePrefix::IDENT)
        firstName = Some(try$(ns.lookup(*firstName)));

    if (namespacePrefix == NamespacePrefix::STAR)
        return Ok(QualifiedNameSelector{
            QualifiedNameSelector::NamespacePattern{UNIVERSAL},
            secondName ? QualifiedNameSelector::NamePattern{secondName.expect()} : QualifiedNameSelector::NamePattern{UNIVERSAL},
        });

    return Ok(QualifiedNameSelector{
        (namespacePrefix == NamespacePrefix::IDENT ? QualifiedNameSelector::NamespacePattern{firstName.expect()} : QualifiedNameSelector::NamespacePattern{NULL_NAMESPACE}),
        secondName ? QualifiedNameSelector::NamePattern{secondName.expect()} : QualifiedNameSelector::NamePattern{UNIVERSAL},
    });
}

// MARK: Simple Selector

Res<TypeSelector> _parseTypeSelector(Cursor<Css::Sst>& s) {}

Res<AttributeSelector> _parseAttributeSelector(Cursor<Css::Sst>& s) {}

Res<ClassSelector> _parseClassSelector(Cursor<Css::Sst>& s) {}

Res<IdSelector> _parseIdSelector(Cursor<Css::Sst>& s) {}

Res<PseudoElementSelector> _parsePseudoElementSelector(Cursor<Css::Sst>& s) {}

Res<PseudoClassSelector> _parsePseudoClassSelector(Cursor<Css::Sst>& s) {}

Res<MatchAnyPseudoClass> _parseMatchAnyPseudoClass(Cursor<Css::Sst>& s) {}

Res<NegationPseudoClass> _parseNegationPseudoClass(Cursor<Css::Sst>& s) {}

void _parseSpecificityAdjustmentPseudoClass(Cursor<Css::Sst>& s) {}

void _parseRelationalPseudoClass(Cursor<Css::Sst>& s) {}

void _parseDirPseudoClass(Cursor<Css::Sst>& s) {}

void _parseLangPseudoClass(Cursor<Css::Sst>& s) {}

void _parseRootPseudoClass(Cursor<Css::Sst>& s) {}

void _parseEmptyPseudoClass(Cursor<Css::Sst>& s) {}

void _parseChildIndexPseudoClass(Cursor<Css::Sst>& s) {}

void _parseTypedChildIndexPseudoClass(Cursor<Css::Sst>& s) {}

void _parseNestingSelector(Cursor<Css::Sst>& s) {}

void _parseSimpleSelector(Cursor<Css::Sst>& s) {
}

// MARK: Compound Selector

// https://www.w3.org/TR/selectors-4/#typedef-compound-selector
void _parseCompoundSelector(Cursor<Css::Sst>& s) {
    // <compound-selector> = [ <type-selector>? <subclass-selector>* ]!
}

// https://www.w3.org/TR/selectors-4/#typedef-compound-selector-list
void _parseCompoundSelectorList(Cursor<Css::Sst>& s) {
    // <compound-selector-list> = <compound-selector>#
}

// MARK: Complex Selector

// https://www.w3.org/TR/selectors-4/#typedef-complex-selector-unit
void _parseComplexSelectorUnit(Cursor<Css::Sst>& s) {
    // <complex-selector-unit> = [ <compound-selector>? <pseudo-compound-selector>* ]!
}

// https://www.w3.org/TR/selectors-4/#typedef-complex-selector
void _parseComplexSelector(Cursor<Css::Sst>& s) {
    // <complex-selector> = <complex-selector-unit> [ <combinator>? <complex-selector-unit> ]*
}

// https://www.w3.org/TR/selectors-4/#typedef-complex-selector-list
Res<Vec<ComplexSelector>> _parseComplexSelectorList(Cursor<Css::Sst>& s) {
    // <complex-selector-list> = <complex-selector>#
}

// MARK: Selector

// https://www.w3.org/TR/selectors-4/#typedef-selector-list
Res<SelectorList> _parseSelectorList(Io::SScan& s) {
    // <selector-list> = <complex-selector-list>
    return _parseComplexSelectorList(s);
}

// https://www.w3.org/TR/selectors-4/#parse-selector
export void parseSelector() {
    // 1. Let selector be the result of parsing source as a <selector-list>. If this returns failure, it’s an invalid selector; return failure.
    auto selector = try$(_parseSelectorList(s));
    // 2. If selector is an invalid selector for any other reason (such as, for example, containing an undeclared namespace prefix), return failure.
    // 3. Otherwise, return selector.
    return Ok(selector);
}

// MARK: Relative Selector

// https://www.w3.org/TR/selectors-4/#typedef-relative-selector
Res<ComplexSelector> _parseRelativeSelector() {
    // <relative-selector> = <combinator>? <complex-selector>
}

// https://www.w3.org/TR/selectors-4/#typedef-relative-selector-list
Res<SelectorList> _parseRelativeSelectorList(Io::SScan& s) {
    // <relative-selector-list> = <relative-selector>#
    while (not s.ended()) {
    }
}

// https://www.w3.org/TR/selectors-4/#parse-relative-selector
export Res<Selector> parseRelativeSelector(Io::SScan& s) {
    // 1. Let selector be the result of parsing source as a <relative-selector-list>. If this returns failure, it’s an invalid selector; return failure.
    auto selector = try$(_parseRelativeSelectorList(s));
    // 2. If selector is an invalid selector for any other reason (such as, for example, containing an undeclared namespace prefix), return failure.
    // 3. Otherwise, return selector.
    return Ok(std::move(selector));
}

// MARK: Serialize -------------------------------------------------------------

void serialize(TypeSelector const&, Io::Emit& e) {
}

void serialize(AttributeSelector const&, Io::Emit& e) {
}

void serialize(ClassSelector const& selector, Io::Emit& e) {
    e(".{}", selector.class_);
}

void serialize(IdSelector const& selector, Io::Emit& e) {
    e("#{}", selector.id);
}

void serialize(PseudoElementSelector const& selector, Io::Emit& e) {
    e("::{}", selector);
}

void serialize(PseudoClassSelector const&, Io::Emit& e) {
}

void serialize(MatchAnyPseudoClass const&, Io::Emit& e) {
}

void serialize(NegationPseudoClass const&, Io::Emit& e) {
}

void serialize(SpecificityAdjustmentPseudoClass const&, Io::Emit& e) {
}

void serialize(RelationalPseudoClass const&, Io::Emit& e) {
}

void serialize(DirPseudoClass const&, Io::Emit& e) {
}

void serialize(LangPseudoClass const&, Io::Emit& e) {
}

void serialize(RootPseudoClass const&, Io::Emit& e) {
}

void serialize(EmptyPseudoClass const&, Io::Emit& e) {
}

void serialize(ChildIndexPseudoClass const&, Io::Emit& e) {
}

void serialize(TypedChildIndexPseudoClass const&, Io::Emit& e) {
}

void serialize(NestingSelector const&, Io::Emit& e) {
}

void serialize(SimpleSelector const&, Io::Emit& e) {
}

void serialize(CompoundSelector const&, Io::Emit& e) {
}

void serialize(ComplexSelector const&, Io::Emit& e) {
}

void serialize(SelectorList const&, Io::Emit& e) {
}

void serialize(Selector const&, Io::Emit& e) {
}

// MARK: Specificity -----------------------------------------------------------

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

Specificity const INLINE_SPEC = Specificity::ZERO;
Specificity const PRESENTATION_ATTR_SPEC = Specificity::ZERO;
Specificity const PRESENTATION_HINT_SPEC = Specificity::ZERO;

Specificity specificity(TypeSelector const& selector) {
    if (selector.qualifiedName.exactName())
        return Specificity::C;
    else
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

Specificity specificity(MatchAnyPseudoClass const&) {
}

Specificity specificity(NegationPseudoClass const&) {
}

Specificity specificity(SpecificityAdjustmentPseudoClass const&) {
}

Specificity specificity(RelationalPseudoClass const&) {
}

Specificity specificity(DirPseudoClass const&) {
}

Specificity specificity(LangPseudoClass const&) {
}

Specificity specificity(RootPseudoClass const&) {
}

Specificity specificity(EmptyPseudoClass const&) {
}

Specificity specificity(ChildIndexPseudoClass const&) {
}

Specificity specificity(TypedChildIndexPseudoClass const&) {
}

Specificity specificity(NestingSelector const&) {
}

Specificity specificity(SimpleSelector const&) {
}

Specificity specificity(CompoundSelector const&) {
}

Specificity specificity(ComplexSelector const&) {
}

// MARK: Matching --------------------------------------------------------------

// https://www.w3.org/TR/selectors-4/#type-selectors
bool match(TypeSelector const& selector, Gc::Ref<Dom::Element> element) {
    return selector.qualifiedName.match(element->qualifiedName);
}

bool match(AttributeSelector selector, Gc::Ref<Dom::Element> element) {
}

bool match(ClassSelector selector, Gc::Ref<Dom::Element> element) {
}

bool match(IdSelector selector, Gc::Ref<Dom::Element> element) {
}

bool match(PseudoElementSelector selector, Gc::Ref<Dom::Element> element) {
}

bool match(PseudoClassSelector selector, Gc::Ref<Dom::Element> element) {
}

bool match(MatchAnyPseudoClass selector, Gc::Ref<Dom::Element> element) {
}

bool match(NegationPseudoClass selector, Gc::Ref<Dom::Element> element) {
}

bool match(SpecificityAdjustmentPseudoClass selector, Gc::Ref<Dom::Element> element) {
}

bool match(RelationalPseudoClass selector, Gc::Ref<Dom::Element> element) {
}

bool match(DirPseudoClass selector, Gc::Ref<Dom::Element> element) {
}

bool match(LangPseudoClass selector, Gc::Ref<Dom::Element> element) {
}

bool match(RootPseudoClass selector, Gc::Ref<Dom::Element> element) {
}

bool match(EmptyPseudoClass selector, Gc::Ref<Dom::Element> element) {
}

bool match(ChildIndexPseudoClass selector, Gc::Ref<Dom::Element> element) {
}

bool match(TypedChildIndexPseudoClass selector, Gc::Ref<Dom::Element> element) {
}

bool match(NestingSelector selector, Gc::Ref<Dom::Element> element) {
}

bool match(SimpleSelector selector, Gc::Ref<Dom::Element> element) {
}

bool match(CompoundSelector selector, Gc::Ref<Dom::Element> element) {
}

bool match(ComplexSelector selector, Gc::Ref<Dom::Element> element) {
}

bool match(SelectorList selector, Gc::Ref<Dom::Element> element) {
}

bool match(Selector selector, Gc::Ref<Dom::Element> element, Opt<Symbol> const& pseudoElement = NONE) {
}

} // namespace Vaev::Style
