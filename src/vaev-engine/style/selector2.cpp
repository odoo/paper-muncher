module;

#include <karm/macros>

export module Vaev.Engine:style.selector2;

import Karm.Core;
import Karm.Glob;

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

Opt<SelectorCombinator> parseCombinator(Rune rune) {
    switch (rune) {
    case ' ':
        return Some(SelectorCombinator::DESCENDANT);
    case '>':
        return Some(SelectorCombinator::CHILD);
    case '+':
        return Some(SelectorCombinator::NEXT_SIBLING);
    case '~':
        return Some(SelectorCombinator::NEXT_SIBLING);
    default:
        return NONE;
    }
}

Str serializeCombinator(SelectorCombinator combinator) {
    switch (combinator) {
    case SelectorCombinator::DESCENDANT:
        return " ";
    case SelectorCombinator::CHILD:
        return ">";
    case SelectorCombinator::NEXT_SIBLING:
        return "+";
    case SelectorCombinator::SUBSEQUENT:
        return "~";
    default:
        unreachable();
    }
}

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
            e("*|{}", exactName);
        else if (auto exactNs = ns.is<Symbol>(); exactNs and exactName)
            e("{}|{}", exactNs, exactName);
        else if (exactName)
            e("{}", exactName);
        else if (ns.is<Universal>())
            e("*");
        else if (auto exactNs = ns.is<Symbol>())
            e("{}|*", exactNs);
        else
            e("|*");
    }
};

// MARK: Selectors -------------------------------------------------------------

struct ComplexSelector;
struct RelativeComplexSelector;

// https://www.w3.org/TR/selectors-4/#selector-list
using SelectorList = Vec<ComplexSelector>;

// https://www.w3.org/TR/selectors-4/#typedef-relative-selector-list
using RelativeSelectorList = Vec<RelativeComplexSelector>;

// https://www.w3.org/TR/selectors-4/#type-selector
// https://www.w3.org/TR/selectors-4/#universal-selector
struct TypeSelector {
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

    void repr(Io::Emit& e) const {
        e("(attribute-selector qualifiedName:{} case:{} match:{} value:{})", qualifiedName, case_, match, value);
    }
};

// https://www.w3.org/TR/selectors-4/#class-selector
struct ClassSelector {
    String class_;

    bool operator==(ClassSelector const&) const = default;

    void repr(Io::Emit& e) const {
        e("(class-selector {})", class_);
    }
};

// https://www.w3.org/TR/selectors-4/#id-selector
struct IdSelector {
    Symbol id;

    bool operator==(IdSelector const&) const = default;

    void repr(Io::Emit& e) const {
        e("(id-selector {})", id);
    }
};

// https://www.w3.org/TR/selectors-4/#pseudo-elements
struct PseudoElementSelector {
    Symbol type;

    bool operator==(PseudoElementSelector const&) const = default;

    void repr(Io::Emit& e) const {
        e("(pseudo-element-selector {})", type);
    }
};

// https://www.w3.org/TR/selectors-4/#pseudo-class
struct PseudoClassSelector {
    Dom::PseudoClass pseudoClass;

    bool operator==(PseudoClassSelector const&) const = default;

    void repr(Io::Emit& e) const {
        e("(pseudo-class-selector {})", pseudoClass);
    }
};

// https://www.w3.org/TR/selectors-4/#matches
struct MatchAnyPseudoClass {
    SelectorList selectors;

    bool operator==(MatchAnyPseudoClass const&) const = default;

    void repr(Io::Emit& e) const {
        e("(match-any-pseudo-class {})", selectors);
    }
};

// https://www.w3.org/TR/selectors-4/#negation
struct NegationPseudoClass {
    SelectorList selectors;

    bool operator==(NegationPseudoClass const&) const = default;

    void repr(Io::Emit& e) const {
        e("(negation-pseudo-class)");
    }
};

// https://www.w3.org/TR/selectors-4/#zero-matches
struct SpecificityAdjustmentPseudoClass {
    SelectorList selectors;

    bool operator==(SpecificityAdjustmentPseudoClass const&) const = default;

    void repr(Io::Emit& e) const {
        e("(specificity-adjustment-pseudo-class)");
    }
};

// https://www.w3.org/TR/selectors-4/#relational
struct RelationalPseudoClass {
    RelativeSelectorList selectors;

    bool operator==(RelationalPseudoClass const&) const = default;

    void repr(Io::Emit& e) const {
        e("(relational-pseudo-class)");
    }
};

export enum struct Dir {
    LTR,
    RTL,
    _LEN,
};

// https://www.w3.org/TR/selectors-4/#the-dir-pseudo
struct DirPseudoClass {
    Dir dir;

    bool operator==(DirPseudoClass const&) const = default;

    void repr(Io::Emit& e) const {
        e("(dir-pseudo-class {})", dir);
    }
};

// https://www.w3.org/TR/selectors-4/#the-dir-pseudo
struct LangPseudoClass {
    String lang;

    bool operator==(LangPseudoClass const&) const = default;

    void repr(Io::Emit& e) const {
        e("(lang-pseudo-class)");
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
    enum Type {
        NTH_CHILD,
        NTH_LAST_CHILD,
        FIRST_CHILD,
        LAST_CHILD,
        ONLY_CHILD,
        _LEN,
    };

    Type type;
    AnB anb;
    SelectorList s;

    bool operator==(ChildIndexPseudoClass const&) const = default;

    void repr(Io::Emit& e) const {
        e("(child-index-pseudo-class {} AnB:{} s:{})", type, anb, s);
    }
};

// https://www.w3.org/TR/selectors-4/#typed-child-index
struct TypedChildIndexPseudoClass {
    enum Type {
        NTH_OF_TYPE,
        NTH_LAST_OF_TYPE,
        FIRST_OF_TYPE,
        LAST_OF_TYPE,
        ONLY_OF_TYPE,
        _LEN,
    };

    Type type;
    AnB anb;

    bool operator==(TypedChildIndexPseudoClass const&) const = default;

    void repr(Io::Emit& e) const {
        e("(typed-child-index-pseudo-class {} AnB:{})", type, anb);
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

// https://www.w3.org/TR/selectors-4/#typedef-subclass-selector
using SubclassSelector = Union<
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
    ChildIndexPseudoClass,
    TypedChildIndexPseudoClass,
    NestingSelector>;

// https://www.w3.org/TR/selectors-4/#compound
struct CompoundSelector {
    Opt<TypeSelector> type;
    Vec<SubclassSelector> subclasses;

    bool operator==(CompoundSelector const&) const = default;

    void repr(Io::Emit& e) const {
        e("(compound-selector {})", subclasses);
    }
};

struct CombinatedSelector {
    CompoundSelector selector;
    SelectorCombinator combinator;

    void repr(Io::Emit& e) const {
        e("{} {} ", selector, combinator);
    }
};

// https://www.w3.org/TR/selectors-4/#complex
struct ComplexSelector {
    Vec<CombinatedSelector> lhs;
    CompoundSelector rhs;

    bool operator==(ComplexSelector const&) const = default;

    void repr(Io::Emit& e) const {
        e("(complex-selector {}{})", lhs, rhs);
    }
};

struct Selector {
    SelectorList selectors;

    bool operator==(Selector const&) const = default;

    void repr(Io::Emit& e) const {
        e("(selector {})", selectors);
    }
};

// https://www.w3.org/TR/selectors-4/#typedef-relative-selector
struct RelativeComplexSelector {
    Vec<CombinatedSelector> selectors;
};

// https://www.w3.org/TR/selectors-4/#relative
struct RelativeSelector {
    RelativeSelectorList selectors;

    bool operator==(RelativeSelector const&) const = default;

    void repr(Io::Emit& e) const {
        e("(selector {})", selectors);
    }
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

Res<TypeSelector> _parseTypeSelector(Cursor<Css::Sst>& s, NamespaceScope const& ns) {
    auto rollback = s.rollbackPoint();
    auto qualifiedName = try$(_parseQualifiedNameSelector(s, ns, ns.defaultNamespacePattern()));
    rollback.disarm();
    return Ok<TypeSelector>(std::move(qualifiedName));
}

Res<AttributeSelector> _parseAttributeSelector(Cursor<Css::Sst>& s, NamespaceScope const& ns) {
    if (s->type != Css::Sst::BLOCK)
        return Error::invalidData("expected attribute selector");

    auto case_ = AttributeSelector::SENSITIVE;
    auto match = AttributeSelector::PRESENT;
    String value = ""s;
    Cursor cur = sub(s->content);

    eatWhitespace(cur);
    if (cur.ended())
        return Error::invalidData("expected attribute name");

    auto qualifiedName = try$(_parseQualifiedNameSelector(cur, ns, NULL_NAMESPACE));

    eatWhitespace(cur);
    if (cur.ended()) {
        s.next();
        return Ok(AttributeSelector{
            qualifiedName,
            case_,
            match,
            value,
        });
    }

    if (cur.skip(Css::Token::delim("="))) {
        match = AttributeSelector::EXACT;
    } else {
        if (*cur != Css::Sst::TOKEN or cur->token.type != Css::Token::DELIM)
            return Error::invalidData("unexpected attribute selector operator");

        auto attrSelectorCase = cur->token.data;
        cur.next();

        if (cur.ended() or not cur.skip(Css::Token::delim("=")))
            return Error::invalidData("expected '=' after attribute selector operator");

        if (attrSelectorCase == "~") {
            match = AttributeSelector::CONTAINS;
        } else if (attrSelectorCase == "|") {
            match = AttributeSelector::HYPHENATED;
        } else if (attrSelectorCase == "^") {
            match = AttributeSelector::STR_START_WITH;
        } else if (attrSelectorCase == "$") {
            match = AttributeSelector::STR_END_WITH;
        } else if (attrSelectorCase == "*") {
            match = AttributeSelector::STR_CONTAIN;
        } else {
            return Error::invalidData("unexpected attribute selector operator");
        }
    }

    eatWhitespace(cur);

    if (cur.ended())
        return Error::invalidData("expected attribute selector value");

    if (auto maybeValue = parseValue<String>(cur)) {
        value = maybeValue.take();
    } else if (*cur == Css::Token::IDENT) {
        value = cur->token.data;
        cur.next();
    } else {
        return Error::invalidData("expected attribute selector value");
    }

    eatWhitespace(cur);
    if (not cur.ended()) {
        if (*cur != Css::Sst::TOKEN or cur->token != Css::Token::IDENT)
            return Error::invalidData("unexpected attribute selector modifier");

        if (cur->token.data == "s") {
            case_ = AttributeSelector::SENSITIVE;
        } else if (cur->token.data == "i") {
            case_ = AttributeSelector::INSENSITIVE;
        } else {
            return Error::invalidData("unexpected attribute selector modifier");
        }

        cur.next();
        eatWhitespace(cur);
    }

    if (not cur.ended())
        return Error::invalidData("unexpected attribute selector content");

    s.next();
    return Ok(AttributeSelector{
        qualifiedName,
        case_,
        match,
        value,
    });
}

Res<ClassSelector> _parseClassSelector(Cursor<Css::Sst>& s) {
    auto rollback = s.rollbackPoint();
    if (not s.skipIf(Css::tokenDataIs(".")))
        return Error::invalidData("expected class selector");

    if (s->token != Css::Token::IDENT)
        return Error::invalidData("expected class selector");

    rollback.disarm();
    return Ok(ClassSelector{s->token.data});
}

Res<IdSelector> _parseIdSelector(Cursor<Css::Sst>& s) {
    auto rollback = s.rollbackPoint();
    if (not s.skipIf(Css::tokenDataIs("#")))
        return Error::invalidData("expected id selector");

    if (s->token != Css::Token::IDENT)
        return Error::invalidData("expected id selector");

    return Ok(ClassSelector{s->token.data});
}

Res<PseudoElementSelector> _parsePseudoElementSelector(Cursor<Css::Sst>& s) {}

Res<PseudoClassSelector> _parsePseudoClassSelector(Cursor<Css::Sst>& s) {}

Res<MatchAnyPseudoClass> _parseMatchAnyPseudoClass(Cursor<Css::Sst>& s) {}

Res<NegationPseudoClass> _parseNegationPseudoClass(Cursor<Css::Sst>& s) {}

Res<SpecificityAdjustmentPseudoClass> _parseSpecificityAdjustmentPseudoClass(Cursor<Css::Sst>& s) {}

Res<RelationalPseudoClass> _parseRelationalPseudoClass(Cursor<Css::Sst>& s) {}

Res<Dir> _parseDirPseudoClass(Cursor<Css::Sst>& s) {}

Res<LangPseudoClass> _parseLangPseudoClass(Cursor<Css::Sst>& s) {}

// https://www.w3.org/TR/selectors-4/#nth-of-type-pseudo
Res<ChildIndexPseudoClass> _parseChildIndexPseudoClass(Cursor<Css::Sst>& s) {
    auto rollback = s.rollbackPoint();

    if (s.ended() or not s.skip(Css::Token::COLON))
        return Error::invalidData("expected nth-child selector"s);

    if (s->prefix == Css::Token::function("nth-child("s)) {
    } else if (s->prefix == Css::Token::function("nth-last-child("s)) {
    } else if (*s == Css::Token::ident("first-child")) {
    } else if (*s == Css::Token::ident("last-child")) {
    } else if (*s == Css::Token::ident("only-child")) {
    } else {
        return Error::invalidData("expected nth-child selector");
    }
}

Res<TypedChildIndexPseudoClass> _parseTypedChildIndexPseudoClass(Cursor<Css::Sst>& s) {
    auto rollback = s.rollbackPoint();

    if (s.ended() or not s.skip(Css::Token::COLON))
        return Error::invalidData("expected nth-child selector"s);

    if (s->prefix == Css::Token::function("nth-of-type("s)) {
    } else if (s->prefix == Css::Token::function("nth-of-type("s)) {
    } else if (*s == Css::Token::ident("first-of-type")) {
    } else if (*s == Css::Token::ident("last-of-type")) {
    } else if (*s == Css::Token::ident("only-of-type")) {
    } else {
        return Error::invalidData("expected <typed-child-index-pseudo-class>");
    }
}

// https://www.w3.org/TR/css-nesting-1/#nest-selector
Res<NestingSelector> _parseNestingSelector(Cursor<Css::Sst>& s) {
    if (s.peek().token == "&"s)
        return Error::invalidData("expected nth-child selector"s);
    s.next();
    return Ok<NestingSelector>();
}

// https://www.w3.org/TR/selectors-4/#typedef-subclass-selector
Res<SubclassSelector> _parseSubclassSelector(Cursor<Css::Sst>& s) {
    // <subclass-selector> = <id-selector> | <class-selector> |
    //                       <attribute-selector> | <pseudo-class-selector>
}

// MARK: Compound Selector

// https://www.w3.org/TR/selectors-4/#typedef-compound-selector
void _parseCompoundSelector(Cursor<Css::Sst>& s) {
    // <compound-selector> = [ <type-selector>? <subclass-selector>* ]!
}

// https://www.w3.org/TR/selectors-4/#typedef-compound-selector-list
CompoundSelector _parseCompoundSelectorList(Cursor<Css::Sst>& s) {
    // <compound-selector-list> = <compound-selector>#
}

// MARK: Complex Selector

// https://www.w3.org/TR/selectors-4/#typedef-complex-selector-unit
void _parseComplexSelectorUnit(Cursor<Css::Sst>& s) {
    // <complex-selector-unit> = [ <compound-selector>? <pseudo-compound-selector>* ]!
}

// https://www.w3.org/TR/selectors-4/#typedef-complex-selector
Res<ComplexSelector> _parseComplexSelector(Cursor<Css::Sst>& s) {
    // <complex-selector> = <complex-selector-unit> [ <combinator>? <complex-selector-unit> ]*
}

// https://www.w3.org/TR/selectors-4/#typedef-complex-selector-list
Res<Vec<ComplexSelector>> _parseComplexSelectorList(Cursor<Css::Sst>& s) {
    // <complex-selector-list> = <complex-selector>#
    Vec<ComplexSelector> complexSelectors;
    do {
        eatWhitespace(s);
        complexSelectors.pushBack(try$(_parseComplexSelector(s)));
        eatWhitespace(s);
    } while (s.skip(Css::Token::COMMA));
    return Ok(std::move(complexSelectors));
}

// MARK: Selector

// https://www.w3.org/TR/selectors-4/#typedef-selector-list
Res<SelectorList> _parseSelectorList(Cursor<Css::Sst>& s) {
    // <selector-list> = <complex-selector-list>
    return _parseComplexSelectorList(s);
}

// https://www.w3.org/TR/selectors-4/#parse-selector
export Res<Selector> parseSelector(Cursor<Css::Sst>& s) {
    // 1. Let selector be the result of parsing source as a <selector-list>. If this returns failure, it’s an invalid selector; return failure.
    auto selector = try$(_parseSelectorList(s));
    // 2. If selector is an invalid selector for any other reason (such as, for example, containing an undeclared namespace prefix), return failure.
    // 3. Otherwise, return selector.
    return Ok(selector);
}

// MARK: Relative Selector

// https://www.w3.org/TR/selectors-4/#typedef-relative-selector
Res<RelativeComplexSelector> _parseRelativeSelector(Cursor<Css::Sst>& s) {
    // <relative-selector> = <combinator>? <complex-selector>
}

// https://www.w3.org/TR/selectors-4/#typedef-relative-selector-list
Res<RelativeSelectorList> _parseRelativeSelectorList(Cursor<Css::Sst>& s) {
    // <relative-selector-list> = <relative-selector>#
    Vec<RelativeComplexSelector> complexSelectors;
    do {
        eatWhitespace(s);
        complexSelectors.pushBack(try$(_parseRelativeSelector(s)));
        eatWhitespace(s);
    } while (s.skip(Css::Token::COMMA));
    return Ok(std::move(complexSelectors));
}

// https://www.w3.org/TR/selectors-4/#parse-relative-selector
export Res<RelativeSelector> parseRelativeSelector(Cursor<Css::Sst>& s) {
    // 1. Let selector be the result of parsing source as a <relative-selector-list>. If this returns failure, it’s an invalid selector; return failure.
    auto selector = try$(_parseRelativeSelectorList(s));
    // 2. If selector is an invalid selector for any other reason (such as, for example, containing an undeclared namespace prefix), return failure.
    // 3. Otherwise, return selector.
    return Ok(std::move(selector));
}

// MARK: Serialize -------------------------------------------------------------

void serialize(SelectorList const& selector, Io::Emit& e);
void serialize(RelativeSelectorList const& selector, Io::Emit& e);

void serialize(TypeSelector const& selector, Io::Emit& e) {
    e(".{}", selector.qualifiedName);
}

void serialize(AttributeSelector const& selector, Io::Emit& e) {
    e("[");
    e("{}", selector.qualifiedName);
    if (selector.match != AttributeSelector::PRESENT) {
        switch (selector.match) {
        case AttributeSelector::PRESENT:
            break;
        case AttributeSelector::EXACT:
            e("=");
            break;
        case AttributeSelector::CONTAINS:
            e("~=");
            break;
        case AttributeSelector::HYPHENATED:
            e("|=");
            break;
        case AttributeSelector::STR_START_WITH:
            e("^=");
            break;
        case AttributeSelector::STR_END_WITH:
            e("$=");
            break;
        case AttributeSelector::STR_CONTAIN:
            e("*=");
            break;
        default:
            unreachable();
        }
        serializeValue(e, selector.value).expect();
    }
    e("]");
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

void serialize(PseudoClassSelector const& selector, Io::Emit& e) {
    e(":{}", serializePseudoClass(selector.pseudoClass));
}

void serialize(MatchAnyPseudoClass const& selector, Io::Emit& e) {
    e(":is(");
    serialize(selector.selectors, e);
    e(")");
}

void serialize(NegationPseudoClass const& selector, Io::Emit& e) {
    e(":not(");
    serialize(selector.selectors, e);
    e(")");
}

void serialize(SpecificityAdjustmentPseudoClass const& selector, Io::Emit& e) {
    e(":where(");
    serialize(selector.selectors, e);
    e(")");
}

void serialize(RelationalPseudoClass const& selector, Io::Emit& e) {
    e(":has(");
    serialize(selector.selectors, e);
    e(")");
}

void serialize(DirPseudoClass const& selector, Io::Emit& e) {
    e(":dir({})", selector.dir == Dir::LTR ? "ltr" : "rtl");
}

void serialize(LangPseudoClass const& selector, Io::Emit& e) {
    e(":lang({})", selector.lang);
}

void serialize(ChildIndexPseudoClass const& selector, Io::Emit& e) {
    switch (selector.type) {
    case ChildIndexPseudoClass::NTH_CHILD:
        e(":nth-child");
        break;
    case ChildIndexPseudoClass::NTH_LAST_CHILD:
        e(":nth-last-child");
        break;
    case ChildIndexPseudoClass::FIRST_CHILD:
        e(":first-child");
        return;
    case ChildIndexPseudoClass::LAST_CHILD:
        e(":last-child");
        return;
    case ChildIndexPseudoClass::ONLY_CHILD:
        e(":only-child");
        return;
    default:
        unreachable();
    }
}

void serialize(TypedChildIndexPseudoClass const& selector, Io::Emit& e) {
    switch (selector.type) {
    case TypedChildIndexPseudoClass::NTH_OF_TYPE:
        e(":nth-of-type");
        break;
    case TypedChildIndexPseudoClass::NTH_LAST_OF_TYPE:
        e(":nth-last-of-type");
        break;
    case TypedChildIndexPseudoClass::FIRST_OF_TYPE:
        e(":first-of-type");
        break;
    case TypedChildIndexPseudoClass::LAST_OF_TYPE:
        e(":last-of-type");
        break;
    case TypedChildIndexPseudoClass::ONLY_OF_TYPE:
        e(":only-of-type");
        break;
    default:
        unreachable();
    }
}

void serialize(NestingSelector const&, Io::Emit& e) {
    e("&");
}

void serialize(SubclassSelector const& selector, Io::Emit& e) {
    selector.visit([&](auto& s) {
        serialize(s, e);
    });
}

void serialize(CompoundSelector const& selector, Io::Emit& e) {
    for (auto& s : selector)
        serialize(s, e);
}

void serialize(ComplexSelector const& selector, Io::Emit& e) {
    for (auto& [s, c] : selector.lhs) {
        serialize(s, e);
        e("{}", serializeCombinator(c));
    }
    serialize(selector.rhs, e);
}

void serialize(SelectorList const& selector, Io::Emit& e) {
    for (auto& [s, i] : iter(selector) | Index()) {
        if (i > 0)
            e(", ");
        serialize(s, e);
    }
}

void serialize(Selector const& selector, Io::Emit& e) {
    serialize(selector.selectors, e);
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

Specificity specificity(ComplexSelector const&);

static Specificity maxSpecificity(auto const& selectors) {
    Specificity res = Specificity::ZERO;
    for (auto const& s : selectors)
        res = max(res, specificity(s));
    return res;
}

Specificity specificity(MatchAnyPseudoClass const& s) {
    return maxSpecificity(s.selectors);
}

Specificity specificity(NegationPseudoClass const& s) {
    return maxSpecificity(s.selectors);
}

// :where()
Specificity specificity(SpecificityAdjustmentPseudoClass const&) {
    return Specificity::ZERO;
}

Specificity specificity(RelationalPseudoClass const& s) {
    return maxSpecificity(s.selectors);
}

Specificity specificity(DirPseudoClass const&) {
    return Specificity::B;
}

Specificity specificity(LangPseudoClass const&) {
    return Specificity::B;
}

Specificity specificity(ChildIndexPseudoClass const& s) {
    return Specificity::B + maxSpecificity(s.s);
}

Specificity specificity(TypedChildIndexPseudoClass const&) {
    return Specificity::B;
}

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
    for (auto const& subclass : s.subclasses)
        res = res + specificity(subclass);
    return res;
}

Specificity specificity(ComplexSelector const& s) {
    Specificity res = Specificity::ZERO;
    for (auto const& compound : s.lhs)
        res = res + specificity(compound.selector);
    res = res + specificity(s.rhs);
    return res;
}

Specificity specificity(RelativeComplexSelector const& s) {
}

// MARK: Matching --------------------------------------------------------------

struct MatchResult {
    bool match = false;
    Specificity specificity;
};

struct MatchSubject {
    Gc::Ref<Dom::Element> element;
    Opt<Symbol> pseudoElement = NONE;
    Dir direction = Dir::LTR;
    Str lang = "";
};

// https://www.w3.org/TR/selectors-4/#type-selectors
bool match(TypeSelector const& selector, MatchSubject const& subject) {
    return selector.qualifiedName.match(subject.element->qualifiedName);
}

static Opt<Str> _getAttributeValue(AttributeSelector const& selector, Gc::Ref<Dom::Element> element) {
    auto name = selector.qualifiedName.exactName();
    if (not name)
        return NONE;

    if (selector.qualifiedName.ns.is<Universal>())
        return element->getAttributeUnqualified(name.expect().str());

    return element->getAttribute(selector.qualifiedName.fullyQualified());
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
bool match(AttributeSelector const& selector, MatchSubject const& subject) {
    auto maybeAttrValue = _getAttributeValue(selector, subject.element);

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

bool match(ClassSelector const& selector, MatchSubject const& subject) {
    return subject.element->classList.contains(selector.class_);
}

bool match(IdSelector const& selector, MatchSubject const& subject) {
    return subject.element->id() == selector.id;
}

bool match(PseudoElementSelector const& selector, MatchSubject const& subject) {
    return subject.pseudoElement == selector.type;
}

bool match(PseudoClassSelector const& selector, MatchSubject const& subject) {
    return contains(subject.element->pseudoClasses(), selector.pseudoClass);
}

bool match(MatchAnyPseudoClass const& selector, MatchSubject const& subject) {
}

bool match(NegationPseudoClass const& selector, MatchSubject const& subject) {
}

bool match(SpecificityAdjustmentPseudoClass const& selector, MatchSubject const& subject) {
}

bool match(RelationalPseudoClass const& selector, MatchSubject const& subject) {
}

bool match(DirPseudoClass const& selector, MatchSubject const& subject) {
    return subject.direction == selector.dir;
}

bool match(LangPseudoClass const& selector, MatchSubject const& subject) {
    return Glob::matchGlob(selector.lang, subject.lang);
}

bool match(ChildIndexPseudoClass const& selector, MatchSubject const& subject) {
}

bool match(TypedChildIndexPseudoClass const& selector, MatchSubject const& subject) {
}

bool match(NestingSelector const& selector, MatchSubject const& subject) {
}

bool match(SubclassSelector const& selector, MatchSubject const& subject) {
}

bool match(CompoundSelector const& selector, MatchSubject const& subject) {
}

// MARK: Selector

bool match(ComplexSelector const& selector, usize i, MatchSubject const& subject) {
}

bool match(ComplexSelector const& selector, MatchSubject const& subject) {
    if (not match(selector.rhs, subject))
        return false;
    return match(selector, 0, subject);
}

MatchResult match(SelectorList const& selector, MatchSubject const& subject) {
    for (auto& s : selector)
        if (match(s, subject))
            return {true, specificity(s)};
    return {false};
}

MatchResult match(Selector const& selector, MatchSubject const& subject) {
    return match(selector.selectors, subject);
}

// MARK: Relative Selector

bool match(RelativeComplexSelector const& selector, usize i, MatchSubject const& subject) {
}

bool match(RelativeComplexSelector const& selector, MatchSubject const& subject) {
    return match(selector, 0, subject);
}

MatchResult match(RelativeSelectorList const& selector, MatchSubject const& subject) {
    for (auto& s : selector)
        if (match(s, subject))
            return {true, specificity(s)};
    return {false};
}

MatchResult match(RelativeSelector const& selector, MatchSubject const& subject) {
    return match(selector.selectors, subject);
}

} // namespace Vaev::Style
