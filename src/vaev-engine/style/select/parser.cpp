module;

#include <karm/macros>

export module Vaev.Engine:style.select.parser;

import Karm.Core;
import Karm.Diag;

import :css;
import :css.values;
import :style.values;
import :dom.element;
import :style.select.anb;
import :style.select.qualifiedName;
import :style.select.selectors;

using namespace Karm;
using namespace Karm::Literals;

namespace Vaev::Style {

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

static Res<SelectorList> _parseSelectorList(Cursor<Css::Sst>& s, NamespaceScope const& ns);

static Res<RelativeSelectorList> _parseRelativeSelectorList(Cursor<Css::Sst>& s, NamespaceScope const& ns);

static Str _functionName(Css::Sst const& sst) {
    Str name = (*sst.prefix)->token.data;
    if (name.len() and last(name) == '(')
        name = Str{name.begin(), name.len() - 1};
    return name;
}

static bool _isBlock(Css::Sst const& sst, Css::Token::Type type) {
    return sst == Css::Sst::BLOCK and sst.token.type == type;
}

static bool _hasPseudoElement(SelectorList const& selectors) {
    return iter(selectors) | Any([](auto const& s) {
               return s.rhs.pseudo.len() > 0;
           });
}

static Res<SelectorList> _parseFunctionArgsAsSelectorList(Css::Sst const& fn, NamespaceScope const& ns) {
    Cursor<Css::Sst> c = fn.content;
    auto selectors = try$(_parseSelectorList(c, ns));
    if (_hasPseudoElement(selectors))
        return Error::invalidData("pseudo-elements are not allowed here");
    return Ok(std::move(selectors));
}

// MARK: Combinator ------------------------------------------------------------

// https://www.w3.org/TR/selectors-4/#typedef-combinator
static Opt<SelectorCombinator> _parseCombinator(Cursor<Css::Sst>& s) {
    // <combinator> = '>' | '+' | '~' | [ '|' '|' ]
    if (s.skip(Css::Token::delim(">")))
        return Some(SelectorCombinator::CHILD);
    if (s.skip(Css::Token::delim("+")))
        return Some(SelectorCombinator::NEXT_SIBLING);
    if (s.skip(Css::Token::delim("~")))
        return Some(SelectorCombinator::SUBSEQUENT);
    return NONE;
}

// MARK: Qualified Name --------------------------------------------------------

static bool _isNamePart(Css::Sst const& sst) {
    return sst == Css::Token::IDENT or sst == Css::Token::delim("*");
}

static bool _startsQualifiedName(Cursor<Css::Sst> const& s) {
    if (s.ended())
        return false;
    if (_isNamePart(*s))
        return true;
    return s.rem() >= 2 and s[0] == Css::Token::delim("|") and _isNamePart(s[1]);
}

// https://www.w3.org/TR/selectors-4/#typedef-wq-name
static Res<QualifiedNameSelector> _parseQualifiedName(Cursor<Css::Sst>& s, NamespaceScope const& ns, QualifiedNameSelector::NamespacePattern defaultNamespace) {
    // <ns-prefix> = [ <ident-token> | '*' ]? '|'
    Opt<QualifiedNameSelector::NamespacePattern> namespacePattern = NONE;
    if (s.rem() >= 2 and s[0] == Css::Token::delim("|") and _isNamePart(s[1])) {
        namespacePattern = Some(QualifiedNameSelector::NamespacePattern{NULL_NAMESPACE});
        s.next();
    } else if (s.rem() >= 3 and _isNamePart(s[0]) and s[1] == Css::Token::delim("|") and _isNamePart(s[2])) {
        if (s[0] == Css::Token::delim("*"))
            namespacePattern = Some(QualifiedNameSelector::NamespacePattern{UNIVERSAL});
        else
            namespacePattern = Some(QualifiedNameSelector::NamespacePattern{try$(ns.lookup(Symbol::from(s[0].token.data)))});
        s.next(2);
    }

    if (s.ended() or not _isNamePart(*s))
        return Error::invalidData("expected qualified name");

    auto name = *s == Css::Token::delim("*")
                    ? QualifiedNameSelector::NamePattern{UNIVERSAL}
                    : QualifiedNameSelector::NamePattern{Symbol::from(s->token.data)};
    s.next();

    return Ok(QualifiedNameSelector{
        namespacePattern.unwrapOr(defaultNamespace),
        name,
    });
}

// MARK: Simple Selectors ------------------------------------------------------

// https://www.w3.org/TR/selectors-4/#type-selectors
static Res<TypeSelector> _parseTypeSelector(Cursor<Css::Sst>& s, NamespaceScope const& ns) {
    return Ok(TypeSelector{try$(_parseQualifiedName(s, ns, ns.defaultNamespacePattern()))});
}

// https://www.w3.org/TR/selectors-4/#attribute-representation
// https://www.w3.org/TR/selectors-4/#attribute-substrings
// https://www.w3.org/TR/selectors-4/#attribute-case
static Res<AttributeSelector> _parseAttributeSelector(Cursor<Css::Sst>& s, NamespaceScope const& ns) {
    if (s.ended() or not _isBlock(*s, Css::Token::LEFT_SQUARE_BRACKET))
        return Error::invalidData("expected attribute selector");

    AttributeSelector res;
    Cursor<Css::Sst> cur = s->content;

    Css::eatWhitespace(cur);
    res.qualifiedName = try$(_parseQualifiedName(cur, ns, NULL_NAMESPACE));
    Css::eatWhitespace(cur);

    if (cur.ended()) {
        s.next();
        return Ok(std::move(res));
    }

    if (cur.skip(Css::Token::delim("="))) {
        res.match = AttributeSelector::EXACT;
    } else {
        if (*cur != Css::Token::DELIM)
            return Error::invalidData("unexpected attribute selector operator");

        auto op = cur.next().token.data;
        if (not cur.skip(Css::Token::delim("=")))
            return Error::invalidData("expected '=' after attribute selector operator");

        if (op == "~")
            res.match = AttributeSelector::CONTAINS;
        else if (op == "|")
            res.match = AttributeSelector::HYPHENATED;
        else if (op == "^")
            res.match = AttributeSelector::STR_START_WITH;
        else if (op == "$")
            res.match = AttributeSelector::STR_END_WITH;
        else if (op == "*")
            res.match = AttributeSelector::STR_CONTAIN;
        else
            return Error::invalidData("unexpected attribute selector operator");
    }

    Css::eatWhitespace(cur);

    if (cur.ended())
        return Error::invalidData("expected attribute selector value");

    if (*cur == Css::Token::STRING or *cur == Css::Token::IDENT)
        res.value = cur.next().token.data;
    else
        return Error::invalidData("expected attribute selector value");

    Css::eatWhitespace(cur);

    if (not cur.ended()) {
        if (cur.skip(Css::Token::ident("s")))
            res.case_ = AttributeSelector::SENSITIVE;
        else if (cur.skip(Css::Token::ident("i")))
            res.case_ = AttributeSelector::INSENSITIVE;
        else
            return Error::invalidData("unexpected attribute selector modifier");

        Css::eatWhitespace(cur);
    }

    if (not cur.ended())
        return Error::invalidData("unexpected attribute selector content");

    s.next();
    return Ok(std::move(res));
}

// https://www.w3.org/TR/selectors-4/#class-html
static Res<ClassSelector> _parseClassSelector(Cursor<Css::Sst>& s) {
    if (s.rem() < 2 or s[0] != Css::Token::delim(".") or s[1] != Css::Token::IDENT)
        return Error::invalidData("expected class selector");
    s.next();
    return Ok(ClassSelector{s.next().token.data});
}

// https://www.w3.org/TR/selectors-4/#id-selectors
static Res<IdSelector> _parseIdSelector(Cursor<Css::Sst>& s) {
    if (s.ended() or *s != Css::Token::HASH)
        return Error::invalidData("expected id selector");
    return Ok(IdSelector{Symbol::from(next(s.next().token.data, 1))});
}

// https://www.w3.org/TR/css-nesting-1/#nest-selector
static Res<NestingSelector> _parseNestingSelector(Cursor<Css::Sst>& s) {
    if (not s.skip(Css::Token::delim("&")))
        return Error::invalidData("expected nesting selector");
    return Ok(NestingSelector{});
}

// MARK: Pseudo-Elements -------------------------------------------------------

// https://www.w3.org/TR/selectors-4/#pseudo-element-syntax
static bool _isLegacyPseudoElement(Str name) {
    return eqCi(name, "before"s) or
           eqCi(name, "after"s) or
           eqCi(name, "first-line"s) or
           eqCi(name, "first-letter"s);
}

static bool _startsPseudoElement(Cursor<Css::Sst> const& s) {
    if (s.rem() < 2 or s[0] != Css::Token::COLON)
        return false;

    if (s[1] == Css::Token::COLON)
        return true;

    return s[1] == Css::Token::IDENT and _isLegacyPseudoElement(s[1].token.data);
}

static Res<PseudoElementSelector> _parsePseudoElementSelector(Cursor<Css::Sst>& s) {
    // ::before ::after :before :after
    if (not s.skip(Css::Token::COLON))
        return Error::invalidData("expected pseudo-element selector");
    s.skip(Css::Token::COLON);

    if (s.ended() or *s != Css::Token::IDENT)
        return Error::invalidData("expected pseudo-element name");

    return Ok(PseudoElementSelector{Symbol::from(Io::format("::{}", s.next().token.data))});
}

// MARK: Pseudo-Classes --------------------------------------------------------

// https://www.w3.org/TR/selectors-4/#the-dir-pseudo
static Res<DirPseudoClass> _parseDirPseudoClass(Css::Sst const& fn) {
    // :dir(ltr)
    Cursor<Css::Sst> c = fn.content;
    Css::eatWhitespace(c);

    DirPseudoClass res;
    if (c.skip(Css::Token::ident("ltr")))
        res.dir = Dir::LTR;
    else if (c.skip(Css::Token::ident("rtl")))
        res.dir = Dir::RTL;
    else
        return Error::invalidData("expected 'ltr' or 'rtl'");

    Css::eatWhitespace(c);
    if (not c.ended())
        return Error::invalidData("unexpected content after direction");

    return Ok(res);
}

// https://www.w3.org/TR/selectors-4/#the-lang-pseudo
static Res<LangPseudoClass> _parseLangPseudoClass(Css::Sst const& fn) {
    // :lang(zh, "*-hant")
    Cursor<Css::Sst> c = fn.content;
    LangPseudoClass res;

    do {
        Css::eatWhitespace(c);
        if (c.ended() or (*c != Css::Token::IDENT and *c != Css::Token::STRING))
            return Error::invalidData("expected language range");
        res.langs.pushBack(c.next().token.data);
        Css::eatWhitespace(c);
    } while (c.skip(Css::Token::COMMA));

    if (not c.ended())
        return Error::invalidData("unexpected content after language range");

    return Ok(std::move(res));
}

// https://www.w3.org/TR/selectors-4/#child-index
static Res<ChildIndexPseudoClass> _parseNthChildPseudoClass(ChildIndexPseudoClass::Type type, Css::Sst const& fn, NamespaceScope const& ns) {
    // :nth-child(An+B [of S]?)
    Cursor<Css::Sst> c = fn.content;
    ChildIndexPseudoClass res{.type = type};

    res.anb = try$(AnB::parse(c));
    Css::eatWhitespace(c);

    if (c.ended())
        return Ok(std::move(res));

    if (not c.skip(Css::Token::ident("of")))
        return Error::invalidData("expected 'of' in nth-child selector");

    res.s = try$(_parseSelectorList(c, ns));
    if (_hasPseudoElement(res.s))
        return Error::invalidData("pseudo-elements are not allowed here");

    return Ok(std::move(res));
}

// https://www.w3.org/TR/selectors-4/#typed-child-index
static Res<TypedChildIndexPseudoClass> _parseNthOfTypePseudoClass(TypedChildIndexPseudoClass::Type type, Css::Sst const& fn) {
    // :nth-of-type(An+B)
    Cursor<Css::Sst> c = fn.content;
    TypedChildIndexPseudoClass res{.type = type};

    res.anb = try$(AnB::parse(c));
    Css::eatWhitespace(c);

    if (not c.ended())
        return Error::invalidData("unexpected content after An+B");

    return Ok(res);
}

// https://www.w3.org/TR/selectors-4/#matches
static SelectorList _parseForgivingSelectorList(Css::Sst const& fn, NamespaceScope const& ns) {
    SelectorList res;
    Slice<Css::Sst> content = fn.content;

    usize start = 0;
    for (usize i = 0; i <= content.len(); i++) {
        if (i < content.len() and content[i] != Css::Token::COMMA)
            continue;

        Cursor<Css::Sst> c{content.buf() + start, content.buf() + i};
        start = i + 1;

        Css::eatWhitespace(c);
        auto selector = _parseSelectorList(c, ns);
        if (not selector or _hasPseudoElement(selector.expect()))
            continue;

        for (auto& s : selector.take())
            res.pushBack(std::move(s));
    }

    return res;
}

static Res<SubclassSelector> _parsePseudoClassFunction(Css::Sst const& fn, NamespaceScope const& ns) {
    auto name = _functionName(fn);

    if (eqCi(name, "not"s))
        return Ok(NegationPseudoClass{try$(_parseFunctionArgsAsSelectorList(fn, ns))});

    if (eqCi(name, "is"s))
        return Ok(MatchAnyPseudoClass{_parseForgivingSelectorList(fn, ns)});

    if (eqCi(name, "where"s))
        return Ok(SpecificityAdjustmentPseudoClass{_parseForgivingSelectorList(fn, ns)});

    if (eqCi(name, "has"s)) {
        Cursor<Css::Sst> c = fn.content;
        return Ok(RelationalPseudoClass{try$(_parseRelativeSelectorList(c, ns))});
    }

    if (eqCi(name, "dir"s))
        return Ok(try$(_parseDirPseudoClass(fn)));

    if (eqCi(name, "lang"s))
        return Ok(try$(_parseLangPseudoClass(fn)));

    if (eqCi(name, "nth-child"s))
        return Ok(try$(_parseNthChildPseudoClass(ChildIndexPseudoClass::NTH_CHILD, fn, ns)));

    if (eqCi(name, "nth-last-child"s))
        return Ok(try$(_parseNthChildPseudoClass(ChildIndexPseudoClass::NTH_LAST_CHILD, fn, ns)));

    if (eqCi(name, "nth-of-type"s))
        return Ok(try$(_parseNthOfTypePseudoClass(TypedChildIndexPseudoClass::NTH_OF_TYPE, fn)));

    if (eqCi(name, "nth-last-of-type"s))
        return Ok(try$(_parseNthOfTypePseudoClass(TypedChildIndexPseudoClass::NTH_LAST_OF_TYPE, fn)));

    return Error::invalidData("unknown functional pseudo-class");
}

static Res<SubclassSelector> _parsePseudoClassIdent(Str name) {
    if (eqCi(name, "first-child"s))
        return Ok(ChildIndexPseudoClass{.type = ChildIndexPseudoClass::FIRST_CHILD});

    if (eqCi(name, "last-child"s))
        return Ok(ChildIndexPseudoClass{.type = ChildIndexPseudoClass::LAST_CHILD});

    if (eqCi(name, "only-child"s))
        return Ok(ChildIndexPseudoClass{.type = ChildIndexPseudoClass::ONLY_CHILD});

    if (eqCi(name, "first-of-type"s))
        return Ok(TypedChildIndexPseudoClass{.type = TypedChildIndexPseudoClass::FIRST_OF_TYPE});

    if (eqCi(name, "last-of-type"s))
        return Ok(TypedChildIndexPseudoClass{.type = TypedChildIndexPseudoClass::LAST_OF_TYPE});

    if (eqCi(name, "only-of-type"s))
        return Ok(TypedChildIndexPseudoClass{.type = TypedChildIndexPseudoClass::ONLY_OF_TYPE});

    if (auto pseudoClass = parsePseudoClass(name))
        return Ok(PseudoClassSelector{*pseudoClass});

    return Error::invalidData("unknown pseudo-class");
}

// https://www.w3.org/TR/selectors-4/#typedef-pseudo-class-selector
static Res<SubclassSelector> _parsePseudoClassSelector(Cursor<Css::Sst>& s, NamespaceScope const& ns) {
    if (not s.skip(Css::Token::COLON) or s.ended())
        return Error::invalidData("expected pseudo-class selector");

    if (*s == Css::Token::IDENT)
        return _parsePseudoClassIdent(s.next().token.data);

    if (*s == Css::Sst::FUNC)
        return _parsePseudoClassFunction(s.next(), ns);

    return Error::invalidData("expected pseudo-class name");
}

// MARK: Compound Selector -----------------------------------------------------

// https://www.w3.org/TR/selectors-4/#typedef-subclass-selector
static Opt<Res<SubclassSelector>> _parseSubclassSelector(Cursor<Css::Sst>& s, NamespaceScope const& ns) {
    // <subclass-selector> = <id-selector> | <class-selector> |
    //                       <attribute-selector> | <pseudo-class-selector>
    if (s.ended())
        return NONE;

    if (*s == Css::Token::HASH)
        return Some(Res<SubclassSelector>{_parseIdSelector(s)});

    if (*s == Css::Token::delim("."))
        return Some(Res<SubclassSelector>{_parseClassSelector(s)});

    if (_isBlock(*s, Css::Token::LEFT_SQUARE_BRACKET))
        return Some(Res<SubclassSelector>{_parseAttributeSelector(s, ns)});

    if (*s == Css::Token::delim("&"))
        return Some(Res<SubclassSelector>{_parseNestingSelector(s)});

    if (*s == Css::Token::COLON and not _startsPseudoElement(s))
        return Some(_parsePseudoClassSelector(s, ns));

    return NONE;
}

// https://www.w3.org/TR/selectors-4/#typedef-compound-selector
static Res<Opt<CompoundSelector>> _parseCompoundSelector(Cursor<Css::Sst>& s, NamespaceScope const& ns) {
    // <compound-selector> = [ <type-selector>? <subclass-selector>* ]!
    CompoundSelector res;
    bool empty = true;

    if (_startsQualifiedName(s)) {
        res.type = Some(try$(_parseTypeSelector(s, ns)));
        empty = false;
    }

    while (auto subclass = _parseSubclassSelector(s, ns)) {
        res.subclasses.pushBack(try$(subclass.take()));
        empty = false;
    }

    if (empty)
        return Ok(NONE);

    return Ok(Some(std::move(res)));
}

// https://www.w3.org/TR/selectors-4/#typedef-pseudo-compound-selector
static Res<PseudoCompoundSelector> _parsePseudoCompoundSelector(Cursor<Css::Sst>& s) {
    // <pseudo-compound-selector> = <pseudo-element-selector> <pseudo-class-selector>*
    PseudoCompoundSelector res{try$(_parsePseudoElementSelector(s))};

    while (not s.ended() and *s == Css::Token::COLON and not _startsPseudoElement(s)) {
        s.next();
        if (s.ended() or *s != Css::Token::IDENT)
            return Error::invalidData("expected user action pseudo-class after pseudo-element");

        auto pseudoClass = parsePseudoClass(s.next().token.data);
        if (not pseudoClass)
            return Error::invalidData("unknown pseudo-class");
        res.pseudoClasses.pushBack(PseudoClassSelector{*pseudoClass});
    }

    return Ok(std::move(res));
}

// MARK: Complex Selector ------------------------------------------------------

// https://www.w3.org/TR/selectors-4/#typedef-complex-selector-unit
static Res<ComplexSelectorUnit> _parseComplexSelectorUnit(Cursor<Css::Sst>& s, NamespaceScope const& ns) {
    // <complex-selector-unit> = [ <compound-selector>? <pseudo-compound-selector>* ]!
    ComplexSelectorUnit res;
    res.selector = try$(_parseCompoundSelector(s, ns));

    while (_startsPseudoElement(s))
        res.pseudo.pushBack(try$(_parsePseudoCompoundSelector(s)));

    if (not res.selector and not res.pseudo)
        return Error::invalidData("expected selector");

    // https://www.w3.org/TR/selectors-4/#type-nmsp
    // If a default namespace is declared, compound selectors without type selectors in them still only match elements
    // in that default namespace.
    if (ns.default_) {
        if (not res.selector)
            res.selector = Some(CompoundSelector{});
        if (not res.selector->type)
            res.selector->type = Some(TypeSelector{{ns.defaultNamespacePattern(), UNIVERSAL}});
    }

    return Ok(std::move(res));
}

// https://www.w3.org/TR/selectors-4/#typedef-complex-selector
static Res<ComplexSelector> _parseComplexSelector(Cursor<Css::Sst>& s, NamespaceScope const& ns) {
    // <complex-selector> = <complex-selector-unit> [ <combinator>? <complex-selector-unit> ]*
    ComplexSelector res;
    auto unit = try$(_parseComplexSelectorUnit(s, ns));

    while (true) {
        auto rollback = s.rollbackPoint();
        auto beforeWhitespace = s;
        Css::eatWhitespace(s);
        bool sawWhitespace = s != beforeWhitespace;

        if (s.ended() or *s == Css::Token::COMMA)
            break;

        auto combinator = _parseCombinator(s);
        if (combinator)
            Css::eatWhitespace(s);
        else if (sawWhitespace)
            combinator = Some(SelectorCombinator::DESCENDANT);
        else
            return Error::invalidData("unexpected token in selector");

        rollback.disarm();

        // https://www.w3.org/TR/selectors-4/#pseudo-element-structure
        // Selectors containing combinators after the pseudo-element are otherwise invalid.
        if (unit.pseudo)
            return Error::invalidData("unexpected combinator after pseudo-element");

        res.lhs.pushBack({std::move(unit), *combinator});
        unit = try$(_parseComplexSelectorUnit(s, ns));
    }

    res.rhs = std::move(unit);
    return Ok(std::move(res));
}

// https://www.w3.org/TR/selectors-4/#typedef-selector-list
static Res<SelectorList> _parseSelectorList(Cursor<Css::Sst>& s, NamespaceScope const& ns) {
    // <selector-list> = <complex-selector-list>
    // <complex-selector-list> = <complex-selector>#
    SelectorList res;
    while (true) {
        Css::eatWhitespace(s);
        res.pushBack(try$(_parseComplexSelector(s, ns)));
        Css::eatWhitespace(s);

        if (s.ended())
            break;

        if (not s.skip(Css::Token::COMMA))
            return Error::invalidData("unexpected token in selector list");
    }
    return Ok(std::move(res));
}

// https://www.w3.org/TR/selectors-4/#parse-selector
export Res<SelectorList> parseSelector(Cursor<Css::Sst>& s, NamespaceScope const& ns = {}) {
    // 1. Let selector be the result of parsing source as a <selector-list>. If this returns failure, it’s an invalid selector; return failure.
    // 2. If selector is an invalid selector for any other reason (such as, for example, containing an undeclared namespace prefix), return failure.
    // 3. Otherwise, return selector.
    return _parseSelectorList(s, ns);
}

export Res<SelectorList> parseSelector(Str input, NamespaceScope const& ns = {}) {
    Css::Lexer lex{input};
    auto diags = Diag::Collector::ignore();
    auto content = Css::consumeSelector(lex, diags);
    Cursor<Css::Sst> c{content};
    return parseSelector(c, ns);
}

// MARK: Relative Selector -----------------------------------------------------

// https://www.w3.org/TR/selectors-4/#typedef-relative-selector
static Res<RelativeComplexSelector> _parseRelativeSelector(Cursor<Css::Sst>& s, NamespaceScope const& ns) {
    // <relative-selector> = <combinator>? <complex-selector>
    auto anchorCombinator = _parseCombinator(s).unwrapOr(SelectorCombinator::DESCENDANT);
    Css::eatWhitespace(s);

    auto complex = try$(_parseComplexSelector(s, ns));
    if (_hasPseudoElement({complex}))
        return Error::invalidData("pseudo-elements are not allowed here");

    RelativeComplexSelector res;
    auto combinator = anchorCombinator;
    for (auto& unit : complex.lhs) {
        auto next = unit.combinator;
        res.selectors.pushBack({std::move(unit), combinator});
        combinator = next;
    }
    res.selectors.pushBack({std::move(complex.rhs), combinator});
    return Ok(std::move(res));
}

// https://www.w3.org/TR/selectors-4/#typedef-relative-selector-list
static Res<RelativeSelectorList> _parseRelativeSelectorList(Cursor<Css::Sst>& s, NamespaceScope const& ns) {
    // <relative-selector-list> = <relative-selector>#
    RelativeSelectorList res;
    while (true) {
        Css::eatWhitespace(s);
        res.pushBack(try$(_parseRelativeSelector(s, ns)));
        Css::eatWhitespace(s);

        if (s.ended())
            break;

        if (not s.skip(Css::Token::COMMA))
            return Error::invalidData("unexpected token in relative selector list");
    }
    return Ok(std::move(res));
}

// https://www.w3.org/TR/selectors-4/#parse-relative-selector
export Res<RelativeSelectorList> parseRelativeSelector(Cursor<Css::Sst>& s, NamespaceScope const& ns = {}) {
    // 1. Let selector be the result of parsing source as a <relative-selector-list>. If this returns failure, it’s an invalid selector; return failure.
    // 2. If selector is an invalid selector for any other reason (such as, for example, containing an undeclared namespace prefix), return failure.
    // 3. Otherwise, return selector.
    return _parseRelativeSelectorList(s, ns);
}

} // namespace Vaev::Style
