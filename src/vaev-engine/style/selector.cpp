module;

#include <karm-base/box.h>
#include <karm-base/vec.h>
#include <karm-logger/logger.h>

export module Vaev.Engine:style.selector;

import :css;
import :dom;
import :values;
import :style.namespace_;

namespace Vaev::Style {

static constexpr bool DEBUG_SELECTORS = false;

// enum order is the operator priority (the lesser the most important)
enum struct OpCode {
    NOP,
    OR,         // ,
    DESCENDANT, // ' '
    CHILD,      // >
    ADJACENT,   // +
    SUBSEQUENT, // ~
    NOT,        // :not()
    WHERE,      // :where()
    AND,        // a.b
    COLUMN,     // ||
};

export struct Selector;

export void unparse(Selector const& sel, Io::Emit& e);

export struct SelectorUnparser {
    Selector& s;

    void repr(Io::Emit& e) const {
        unparse(s, e);
    }
};

export struct QualifiedNameSelector {
    Opt<Symbol> ns;   // If namespace is NONE, it means any namespace is accepted.
    Opt<Symbol> name; // If name is NONE, it means any name is accepted.

    QualifiedNameSelector()
        : ns(NONE), name(NONE) {}

    QualifiedNameSelector(Opt<Symbol> ns, Opt<Symbol> name)
        : ns(ns), name(name) {}

    QualifiedNameSelector(Dom::QualifiedName const& qualifiedName)
        : ns(qualifiedName.ns), name(qualifiedName.name) {}

    static QualifiedNameSelector wildcard() {
        return {NONE, NONE};
    }

    bool match(Dom::QualifiedName const& q) const {
        if (ns and q.ns != *ns)
            return false;
        if (name and q.name != *name)
            return false;
        return true;
    }

    void repr(Io::Emit& e) const {
        if (ns and name)
            e("{}|{}", *ns, *name);
        else if (name)
            e("{}", *name);
        else if (ns)
            e("{}|*", *ns);
        else
            e("*");
    }

    bool isWildcard() const {
        return not ns or not name;
    }

    Dom::QualifiedName fullyQualified() const {
        if (isWildcard())
            panic("cannot fully qualify a wildcard selector");
        return {ns.unwrap(), name.unwrap()};
    }

    bool operator==(QualifiedNameSelector const&) const = default;
};

struct EmptySelector {
    void repr(Io::Emit& e) const {
        e("EMPTY");
    }

    bool operator==(EmptySelector const&) const = default;
};

export constexpr EmptySelector EMPTY = {};

export struct Infix {
    enum struct Type {
        DESCENDANT, // ' '
        CHILD,      // >
        ADJACENT,   // +
        SUBSEQUENT, // ~
        COLUMN,     // ||
        _LEN,
    };

    using enum Type;
    Type type;
    Box<Selector> lhs;
    Box<Selector> rhs;

    void repr(Io::Emit& e) const {
        e("({} {} {})", *lhs, type, *rhs);
    }

    bool operator==(Infix const&) const;
};

export struct Nfix {
    // NOTE: is(), not() and where() are coded as Nfixes instead of Pseudo
    enum struct Type {
        AND,   // ''
        OR,    // :is(), ', '
        NOT,   // :not()
        WHERE, // :where()
        _LEN,
    };

    using enum Type;

    Type type;
    Vec<Selector> inners;

    void repr(Io::Emit& e) const {
        e("({} {})", type, inners);
    }

    bool operator==(Nfix const&) const;
};

// https://www.w3.org/TR/selectors-3/#typenmsp
export struct TypeSelector {
    QualifiedNameSelector qualifiedName;

    TypeSelector(Opt<Symbol> ns, Opt<Symbol> name)
        : qualifiedName(ns, name) {}

    TypeSelector(QualifiedNameSelector qualifiedName)
        : qualifiedName(qualifiedName) {}

    TypeSelector(Dom::QualifiedName const& qualifiedName)
        : qualifiedName(qualifiedName.ns, qualifiedName.name) {}

    static TypeSelector universal() {
        return {QualifiedNameSelector::wildcard()};
    }

    void repr(Io::Emit& e) const {
        e("{}", qualifiedName);
    }

    bool operator==(TypeSelector const&) const = default;
};

export struct IdSelector {
    Symbol id;

    void repr(Io::Emit& e) const {
        e("#{}", id);
    }

    bool operator==(IdSelector const&) const = default;
};

export struct ClassSelector {
    String class_;

    void repr(Io::Emit& e) const {
        e(".{}", class_);
    }

    bool operator==(ClassSelector const&) const = default;
};

export struct AnB {
    isize a, b;

    void repr(Io::Emit& e) const {
        e("{}n{}{}", a, b < 0 ? "-"s : "+"s, b);
    }

    bool operator==(AnB const&) const = default;
};

export enum struct Dir {
    LTR,
    RTL,
};

export struct Pseudo {
    enum struct Type {
#define PSEUDO(ID, ...) ID,
#include "defs/pseudo.inc"
#undef PSEUDO

        _LEN,
    };

    static Opt<Type> _Type(Str name) {
#define PSEUDO(IDENT, NAME) \
    if (name == NAME)       \
        return Type::IDENT;
#include "defs/pseudo.inc"
#undef PSEUDO

        return NONE;
    }

    static Pseudo make(Str name) {
        auto id = _Type(name);
        if (id) {
            auto result = Type{*id};
            return result;
        }
        return Type{0};
    }

    using enum Type;
    using Extra = Union<None, String, AnB, Dir>;

    Type type;
    Extra extra = NONE;

    Pseudo() = default;

    Pseudo(Type type, Extra extra = NONE)
        : type(type), extra(extra) {}

    bool operator==(Pseudo const&) const = default;

    void repr(Io::Emit& e) const {
        e("{}", type);
    }
};

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
    Case case_;
    Match match;
    String value;

    void repr(Io::Emit& e) const {
        e("[{} {} {} {}]", qualifiedName, case_, match, value);
    }

    bool operator==(AttributeSelector const&) const = default;
};

using _Selector = Union<
    Nfix,
    Infix,
    TypeSelector,
    EmptySelector,
    IdSelector,
    ClassSelector,
    Pseudo,
    AttributeSelector>;

export void unparse(Selector const& sel, Io::Emit& e);

export struct Selector : _Selector {
    using _Selector::_Selector;

    static Selector empty() {
        return EMPTY;
    }

    static Selector and_(Vec<Selector> selectors) {
        return Nfix{
            Nfix::AND,
            std::move(selectors),
        };
    }

    static Selector or_(Vec<Selector> selectors) {
        return Nfix{
            Nfix::OR,
            std::move(selectors),
        };
    }

    static Selector not_(Selector selector) {
        return Nfix{
            Nfix::NOT,
            {std::move(selector)},
        };
    }

    static Selector where(Selector selector) {
        return Nfix{
            Nfix::WHERE,
            {std::move(selector)},
        };
    }

    static Selector descendant(Selector lhs, Selector rhs) {
        return Infix{
            Infix::DESCENDANT,
            makeBox<Selector>(std::move(lhs)),
            makeBox<Selector>(std::move(rhs)),
        };
    }

    static Selector child(Selector lhs, Selector rhs) {
        return Infix{
            Infix::CHILD,
            makeBox<Selector>(std::move(lhs)),
            makeBox<Selector>(std::move(rhs)),
        };
    }

    static Selector adjacent(Selector lhs, Selector rhs) {
        return Infix{
            Infix::ADJACENT,
            makeBox<Selector>(std::move(lhs)),
            makeBox<Selector>(std::move(rhs)),
        };
    }

    static Selector subsequent(Selector lhs, Selector rhs) {
        return Infix{
            Infix::SUBSEQUENT,
            makeBox<Selector>(std::move(lhs)),
            makeBox<Selector>(std::move(rhs)),
        };
    }

    static Selector column(Selector lhs, Selector rhs) {
        return Infix{
            Infix::COLUMN,
            makeBox<Selector>(std::move(lhs)),
            makeBox<Selector>(std::move(rhs)),
        };
    }

    void repr(Io::Emit& e) const {
        visit([&](auto const& v) {
            e("{}", v);
        });
    }

    bool operator==(Selector const&) const = default;

    static Res<QualifiedNameSelector> _parseQualifiedNameSelector(Cursor<Css::Sst>& cur, Namespace const& ns, Opt<Symbol> default_) {
        // ns|name
        // ns|*
        // *|name
        // *|*
        // name
        if (cur.ended())
            return Error::invalidData("expected qualified name selector, got empty input");

        Opt<Symbol> firstName = NONE;
        if (cur->token == Css::Token::IDENT) {
            firstName = Symbol::from(cur->token.data);
            cur.next();
        } else if (cur.skip(Css::Token::delim("*"))) {
            firstName = NONE;
        }

        if (not cur.skip(Css::Token::delim("|"))) {
            return Ok(QualifiedNameSelector{
                default_,
                firstName,
            });
        }

        Opt<Symbol> secondName = NONE;
        ;
        if (cur->token == Css::Token::IDENT) {
            secondName = Symbol::from(cur->token.data);
            cur.next();
        } else if (cur.skip(Css::Token::delim("*"))) {
            secondName = NONE;
        }

        if (firstName != NONE)
            firstName = try$(ns.lookup(*firstName));

        return Ok(QualifiedNameSelector{
            firstName,
            secondName,
        });
    }

    static Res<Selector> _parseAttributeSelector(Slice<Css::Sst> content, Namespace const&) {
        auto case_ = AttributeSelector::INSENSITIVE;
        QualifiedNameSelector qualifiedName;
        String value = ""s;
        auto match = AttributeSelector::PRESENT;

        usize step = 0;
        Cursor cur = content;

        while (not cur.ended() and cur->token.data != "]"s) {
            if (cur.skip(Css::Token::WHITESPACE))
                continue;

            switch (step) {
            case 0:
                qualifiedName = {NONE, Symbol::from(cur->token.data)};
                step++;
                break;
            case 1:
                if (cur->token.data != "="s) {
                    auto attrSelectorCase = cur.next().token.data;

                    if (cur.ended())
                        break;

                    if (not(cur.peek() == (Css::Token::delim("="))))
                        break;

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
                        break;
                    }
                } else {
                    match = AttributeSelector::EXACT;
                }
                step++;
                break;
            case 2:
                value = parseValue<String>(cur).unwrapOr(""s);
                step++;
                break;
            case 3:
                if (cur->token.data == "s") {
                    case_ = AttributeSelector::SENSITIVE;
                }
                break;
            }

            if (not cur.ended())
                cur.next();
        }

        return Ok(AttributeSelector{
            qualifiedName,
            case_,
            match,
            value,
        });
    }

    // consume an Op Code
    static OpCode _peekOpCode(Cursor<Css::Sst>& cur) {
        if (cur.ended()) {
            return OpCode::NOP;
        }
        if (*cur != Css::Sst::TOKEN)
            return OpCode::AND;

        switch (cur->token.type) {
        case Css::Token::COMMA:
            cur.next();
            return OpCode::OR;

        case Css::Token::WHITESPACE:
            cur.next();
            // a white space could be an operator or be ignored if followed by another op
            if (cur.ended())
                return OpCode::NOP;

            if (
                cur.peek() == Css::Token::IDENT or
                cur.peek() == Css::Token::HASH or
                cur.peek().token.data == "." or
                cur.peek().token.data == "*" or
                cur.peek().token.data == ":"
            ) {
                return OpCode::DESCENDANT;
            } else {
                auto op = _peekOpCode(cur);

                if (cur.rem() > 1 and cur.peek(1).token == Css::Token::WHITESPACE) {
                    if (cur.ended())
                        return OpCode::NOP;
                    cur.next();
                }

                return op;
            }

        case Css::Token::DELIM:
            if (cur.rem() <= 1) {
                return OpCode::NOP;
            }

            if (cur->token.data == ">") {
                if (cur.ended()) {
                    return OpCode::NOP;
                }
                cur.next();
                return OpCode::CHILD;
            } else if (cur->token.data == "~") {
                if (cur.ended()) {
                    return OpCode::NOP;
                }
                cur.next();
                return OpCode::SUBSEQUENT;
            } else if (cur->token.data == "+") {
                if (cur.ended()) {
                    return OpCode::NOP;
                }
                cur.next();
                return OpCode::ADJACENT;
            } else if (cur->token.data == "." or cur->token.data == "*") {
                return OpCode::AND;
            } else {
                return OpCode::NOP;
            }

        case Css::Token::COLON:
        default:
            return OpCode::AND;
        }
    }

    // consume a selector element (everything  that has a lesser priority than the current OP)
    static Res<Selector> _parseSelectorElement(Cursor<Css::Sst>& cur, OpCode currentOp, Namespace const& ns) {
        if (cur.ended()) {
            logErrorIf(DEBUG_SELECTORS, "unterminated selector");
            return Error::invalidData("unterminated selector");
        }

        Selector val = Selector::empty();
        if (*cur == Css::Sst::TOKEN) {
            switch (cur->token.type) {
            case Css::Token::WHITESPACE:
                cur.next();
                return _parseSelectorElement(cur, currentOp, ns);

            case Css::Token::HASH:
                val = IdSelector{Symbol::from(next(cur->token.data, 1))};
                break;
            case Css::Token::IDENT:
                val = TypeSelector{ns.default_, Symbol::from(cur->token.data)};
                break;
            case Css::Token::DELIM:
                if (cur->token.data == ".") {
                    cur.next();
                    val = ClassSelector{cur->token.data};
                } else if (cur->token.data == "*") {
                    val = TypeSelector{ns.default_, NONE};
                }
                break;
            case Css::Token::COLON:
                cur.next();
                if (cur->token.type == Css::Token::COLON) {
                    cur.next();
                    if (cur.ended()) {
                        logErrorIf(DEBUG_SELECTORS, "unterminated selector");
                        return Error::invalidData("unterminated selector");
                    }
                }

                if (cur->prefix == Css::Token::function("not(")) {
                    Cursor<Css::Sst> c = cur->content;
                    // consume a whole selector not a single one
                    val = Selector::not_(try$(Selector::parse(c, ns)));
                } else {
                    val = Pseudo::make(cur->token.data);
                }
                break;
            default:
                return Error::invalidData("unexpected token in selector");
                break;
            }
        } else if (cur->type == Css::Sst::BLOCK) {
            val = try$(_parseAttributeSelector(cur->content, ns));
        } else {
            return Error::invalidData("unexpected selector element");
        }

        cur.next();
        if (not cur.ended()) {
            Cursor rb = cur;
            OpCode nextOpCode = _peekOpCode(cur);
            if (nextOpCode > currentOp) {
                val = try$(_parseInfixExpr(val, cur, nextOpCode, ns));
            } else {
                cur = rb;
            }
        }
        return Ok(val);
    }

    static Res<Selector> _parseNfixExpr(Selector lhs, OpCode op, Cursor<Css::Sst>& cur, Namespace const& ns) {
        Vec<Selector> selectors = {
            lhs,
            try$(_parseSelectorElement(cur, op, ns)),
        };
        // all the selectors between the op eg : a,b.B,c -> [a,b.B,c]

        while (not cur.ended()) {
            Cursor<Css::Sst> rollBack = cur;

            OpCode nextOpCode = _peekOpCode(cur);

            if (nextOpCode == OpCode::NOP) {
                break;
            } else if (nextOpCode == op) {
                // adding the selector to the nfix
                selectors.pushBack(try$(_parseSelectorElement(cur, op, ns)));
            } else if (nextOpCode == OpCode::COLUMN or nextOpCode == OpCode::OR or nextOpCode == OpCode::AND) {
                // parse new nfix
                if (nextOpCode < op) {
                    cur = rollBack;
                    break;
                }
                last(selectors) = try$(_parseNfixExpr(last(selectors), nextOpCode, cur, ns));
            } else {
                // parse new infix if the next op is more important

                if (nextOpCode < op) {
                    cur = rollBack;
                    break;
                }

                last(selectors) = try$(_parseInfixExpr(last(selectors), cur, nextOpCode, ns));
                if (cur.rem() == 2 and cur.peek(1) == Css::Token::WHITESPACE) [[likely]] {
                    cur.next();
                }
            }
        }

        switch (op) {
        case OpCode::AND:
            return Ok(Selector::and_(selectors));

        case OpCode::OR:
            return Ok(Selector::or_(selectors));

        default:
            return Ok(Selector::and_(selectors));
        }
    }

    static Res<Selector> _parseInfixExpr(Selector lhs, Cursor<Css::Sst>& cur, OpCode opCode, Namespace const& ns) {
        if (opCode == OpCode::NOP)
            opCode = _peekOpCode(cur);

        switch (opCode) {
        case OpCode::NOP:
            return Ok(lhs);

        case OpCode::DESCENDANT:
            return Ok(Selector::descendant(lhs, try$(_parseSelectorElement(cur, opCode, ns))));

        case OpCode::CHILD:
            return Ok(Selector::child(lhs, try$(_parseSelectorElement(cur, opCode, ns))));

        case OpCode::ADJACENT:
            return Ok(Selector::adjacent(lhs, try$(_parseSelectorElement(cur, opCode, ns))));

        case OpCode::SUBSEQUENT:
            return Ok(Selector::subsequent(lhs, try$(_parseSelectorElement(cur, opCode, ns))));

        case OpCode::NOT:
            return Ok(Selector::not_(try$(_parseSelectorElement(cur, opCode, ns))));

        case OpCode::WHERE:
            return Ok(Selector::where(try$(_parseSelectorElement(cur, opCode, ns))));

        case OpCode::COLUMN:
        case OpCode::OR:
        case OpCode::AND:
            return _parseNfixExpr(lhs, opCode, cur, ns);
        }
    }

    static Res<Selector> parse(Cursor<Css::Sst>& c, Namespace const& ns = {}) {
        if (not c)
            return Error::invalidData("expected selector");

        logDebugIf(DEBUG_SELECTORS, "PARSING SELECTOR : {}", c);
        Selector currentSelector = try$(_parseSelectorElement(c, OpCode::NOP, ns));

        while (not c.ended()) {
            currentSelector = try$(_parseInfixExpr(currentSelector, c, OpCode::NOP, ns));
        }
        return Ok(currentSelector);
    }

    static Res<Selector> parse(Io::SScan& s, Namespace const& ns = {}) {
        Css::Lexer lex = s;
        auto val = consumeSelector(lex);
        Cursor<Css::Sst> c{val};
        return parse(c, ns);
    };

    static Res<Selector> parse(Str input, Namespace const& ns = {}) {
        Io::SScan s{input};
        return parse(s, ns);
    };

    auto unparsed() lifetimebound {
        return SelectorUnparser{*this};
    }
};

bool Infix::operator==(Infix const&) const = default;

bool Nfix::operator==(Nfix const&) const = default;

// MARK: Unparsing -------------------------------------------------------------

export void unparse(Selector const& sel, Io::Emit& e) {
    sel.visit(Visitor{
        [&](Nfix const& s) {
            if (s.type == Nfix::OR) {
                for (usize i = 0; i < s.inners.len(); i++) {
                    if (i != s.inners.len() - 1) {
                        e("{},", s.inners[i]);
                    } else {
                        e("{}", s.inners[i]);
                    }
                }
            } else if (s.type == Nfix::AND) {
                for (usize i = 0; i < s.inners.len(); i++) {
                    e("{}", s.inners[i]);
                }
            } else {
                e("{}", s);
            }
        },
        [&](Meta::Contains<ClassSelector, IdSelector, TypeSelector> auto const& s) -> void {
            e("{}", s);
        },
        [&](Infix const& s) -> void {
            if (s.type == Infix::DESCENDANT) {
                e("{} {}", s.lhs, s.rhs);
            } else if (s.type == Infix::CHILD) {
                e("{}>{}", s.lhs, s.rhs);
            } else if (s.type == Infix::ADJACENT) {
                e("{}+{}", s.lhs, s.rhs);
            } else if (s.type == Infix::SUBSEQUENT) {
                e("{}~{}", s.lhs, s.rhs);
                ;
            } else {
                e("{}", s);
            }
        },
        [&](Pseudo const& s) -> void {
            e("{}", s);
        },
        [&](AttributeSelector const& s) {
            e("{}", s);
        },
        [&](auto const& s) -> void {
            e("{}", s);
        },
    });
}

// MARK: Selector Specificity --------------------------------------------------
// https://www.w3.org/TR/selectors-3/#specificity

export struct Spec {
    // a: The number of ID selectors in the selector.
    // b: The number of class selectors, attributes selectors, and pseudo-classes in the selector.
    // c: The number of type selectors and pseudo-elements in the selector.
    isize a, b, c;

    static Spec const NOMATCH;
    static Spec const ZERO, A, B, C;

    Spec(isize a, isize b, isize c)
        : a(a), b(b), c(c) {}

    Spec operator+(Spec const& other) const {
        return {
            a + other.a,
            b + other.b,
            c + other.c,
        };
    }

    Spec operator and(Spec const& other) const {
        return {
            a + other.a,
            b + other.b,
            c + other.c,
        };
    }

    Spec operator or(Spec const& other) const {
        if (*this > other)
            return *this;
        return other;
    }

    Spec operator not() const {
        return {
            a,
            b,
            c
        };
    }

    bool operator==(Spec const& other) const = default;
    auto operator<=>(Spec const& other) const = default;

    void repr(Io::Emit& e) const {
        e("{}-{}-{}", a, b, c);
    }
};

inline Spec const Spec::ZERO = {0, 0, 0};
inline Spec const Spec::A = {1, 0, 0};
inline Spec const Spec::B = {0, 1, 0};
inline Spec const Spec::C = {0, 0, 1};

Spec const INLINE_SPEC = Spec::ZERO;
Spec const PRESENTATION_ATTR_SPEC = Spec::ZERO;

// https://www.w3.org/TR/selectors-4/#specificity-rules
export Spec spec(Selector const& s) {
    return s.visit(Visitor{
        [](Nfix const& n) {
            // FIXME: missing other pseudo class selectors implemented as nfix
            if (n.type == Nfix::WHERE)
                return Spec::ZERO;

            Spec sum = Spec::ZERO;
            for (auto& inner : n.inners)
                sum = sum + spec(inner);
            return sum;
        },
        [](Infix const& i) {
            return spec(*i.lhs) + spec(*i.rhs);
        },
        [](EmptySelector const&) {
            return Spec::ZERO;
        },
        [](IdSelector const&) {
            return Spec::A;
        },
        [](TypeSelector const& s) {
            if (s.qualifiedName.name)
                return Spec::C;
            else
                return Spec::ZERO;
        },
        [](ClassSelector const&) {
            return Spec::B;
        },
        [](Pseudo const&) {
            return Spec::B;
        },
        [](AttributeSelector const&) {
            return Spec::B;
        },
        [](auto const& s) {
            logWarnIf(DEBUG_SELECTORS, "unimplemented selector: {}", s);
            return Spec::ZERO;
        },
    });
}

} // namespace Vaev::Style
