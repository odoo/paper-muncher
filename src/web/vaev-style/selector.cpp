module;

#include <karm-base/box.h>
#include <karm-base/vec.h>
#include <karm-io/fmt.h>
#include <karm-logger/logger.h>
#include <vaev-dom/element.h>

export module Vaev.Style:selector;

import Vaev.Style.Css;
import :values;

namespace Vaev::Style {

static constexpr bool DEBUG_SELECTORS = false;

// 17. Calculating a selector’s specificity
// https://www.w3.org/TR/selectors-4/#specificity-rules
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

export Spec const INLINE_SPEC = Spec::ZERO;

export struct Selector;

export struct UniversalSelector {
    void repr(Io::Emit& e) const {
        e("*");
    }

    bool operator==(UniversalSelector const&) const = default;
};

constexpr UniversalSelector UNIVERSAL = {};

struct EmptySelector {
    void repr(Io::Emit& e) const {
        e("EMPTY");
    }

    bool operator==(EmptySelector const&) const = default;
};

constexpr EmptySelector EMPTY = {};

struct Infix {
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

struct Nfix {
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

export struct TypeSelector {
    TagName type;

    void repr(Io::Emit& e) const {
        e("{}", type);
    }

    bool operator==(TypeSelector const&) const = default;
};

export struct IdSelector {
    String id;

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

struct AnB {
    isize a, b;

    void repr(Io::Emit& e) const {
        e("{}n{}{}", a, b < 0 ? "-"s : "+"s, b);
    }

    bool operator==(AnB const&) const = default;
};

enum struct Dir {
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
        // logDebug("make type {} {}", name, id);
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

    String name;
    Case case_;
    Match match;
    String value;

    void repr(Io::Emit& e) const {
        e("[{} {} {} {}]", name, case_, match, value);
    }

    bool operator==(AttributeSelector const&) const = default;
};

using _Selector = Union<
    Nfix,
    Infix,
    TypeSelector,
    UniversalSelector,
    EmptySelector,
    IdSelector,
    ClassSelector,
    Pseudo,
    AttributeSelector>;

// MARK: Parser ----------------------------------------------------------------

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

static Res<Selector> _parseSelectorElement(Cursor<Css::Sst>& cur, OpCode currentOp);
static Res<Selector> _parseInfixExpr(Selector lhs, Cursor<Css::Sst>& cur, OpCode opCode = OpCode::NOP);
inline void unparse(Selector const& sel, Io::Emit& e);

struct Selector : public _Selector {
    using _Selector::_Selector;

    Selector() : _Selector{UniversalSelector{}} {}

    static Selector universal() {
        return UNIVERSAL;
    }

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

    static Res<Selector> parse(Cursor<Css::Sst>& c) {
        if (not c)
            return Error::invalidData("expected selector");

        logDebugIf(DEBUG_SELECTORS, "PARSING SELECTOR : {}", c);
        Selector currentSelector = try$(_parseSelectorElement(c, OpCode::NOP));

        while (not c.ended()) {
            currentSelector = try$(_parseInfixExpr(currentSelector, c));
        }
        return Ok(currentSelector);
    }

    static Res<Selector> parse(Io::SScan& s) {
        Css::Lexer lex = s;
        auto val = consumeSelector(lex);
        Cursor<Css::Sst> c{val};
        return parse(c);
    }

    static Res<Selector> parse(Str input) {
        Io::SScan s{input};
        return parse(s);
    }

    auto unparsed() lifetimebound {
        struct Unparser {
            Selector& s;

            void repr(Io::Emit& e) const {
                unparse(s, e);
            }
        };

        return Unparser{*this};
    }
};

static Selector _parseAttributeSelector(Slice<Css::Sst> content) {
    auto caze = AttributeSelector::INSENSITIVE;
    Str name = "";
    String value = ""s;
    auto match = AttributeSelector::PRESENT;

    usize step = 0;
    Cursor<Css::Sst> cur = content;

    while (not cur.ended() and cur->token.data != "]"s) {
        if (cur.skip(Css::Token::WHITESPACE))
            continue;

        switch (step) {
        case 0:
            name = cur->token.data;
            step++;
            break;
        case 1:
            if (cur->token.data != "="s) {
                if (cur.ended() or cur.peek(1).token.data != "="s) {
                    break;
                }

                if (cur->token.data == "~") {
                    match = AttributeSelector::CONTAINS;
                } else if (cur->token.data == "|") {
                    match = AttributeSelector::HYPHENATED;
                } else if (cur->token.data == "^") {
                    match = AttributeSelector::STR_START_WITH;
                } else if (cur->token.data == "$") {
                    match = AttributeSelector::STR_END_WITH;
                } else if (cur->token.data == "*") {
                    match = AttributeSelector::STR_CONTAIN;
                } else {
                    break;
                }
                cur.next();
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
                caze = AttributeSelector::SENSITIVE;
            }
            break;
        }

        if (not cur.ended())
            cur.next();
    }

    return AttributeSelector{
        name,
        caze,
        match,
        value,
    };
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

static Res<Selector> _parseNfixExpr(Selector lhs, OpCode op, Cursor<Css::Sst>& cur);

Res<Selector> _parseInfixExpr(Selector lhs, Cursor<Css::Sst>& cur, OpCode opCode) {
    if (opCode == OpCode::NOP)
        opCode = _peekOpCode(cur);

    switch (opCode) {
    case OpCode::NOP:
        return Ok(lhs);

    case OpCode::DESCENDANT:
        return Ok(Selector::descendant(lhs, try$(_parseSelectorElement(cur, opCode))));

    case OpCode::CHILD:
        return Ok(Selector::child(lhs, try$(_parseSelectorElement(cur, opCode))));

    case OpCode::ADJACENT:
        return Ok(Selector::adjacent(lhs, try$(_parseSelectorElement(cur, opCode))));

    case OpCode::SUBSEQUENT:
        return Ok(Selector::subsequent(lhs, try$(_parseSelectorElement(cur, opCode))));

    case OpCode::NOT:
        return Ok(Selector::not_(try$(_parseSelectorElement(cur, opCode))));

    case OpCode::WHERE:
        return Ok(Selector::where(try$(_parseSelectorElement(cur, opCode))));

    case OpCode::COLUMN:
    case OpCode::OR:
    case OpCode::AND:
        return _parseNfixExpr(lhs, opCode, cur);
    }
}

// consume a selector element (everything  that has a lesser priority than the current OP)
Res<Selector> _parseSelectorElement(Cursor<Css::Sst>& cur, OpCode currentOp) {
    if (cur.ended()) {
        logErrorIf(DEBUG_SELECTORS, "ERROR : unterminated selector");
        return Error::invalidData("unterminated selector");
    }
    Selector val;

    if (*cur == Css::Sst::TOKEN) {
        switch (cur->token.type) {
        case Css::Token::WHITESPACE:
            cur.next();
            return _parseSelectorElement(cur, currentOp);
        case Css::Token::HASH:
            val = IdSelector{next(cur->token.data, 1)};
            break;
        case Css::Token::IDENT:
            val = TypeSelector{TagName::make(cur->token.data, Vaev::HTML)};
            break;
        case Css::Token::DELIM:
            if (cur->token.data == ".") {
                cur.next();
                val = ClassSelector{cur->token.data};
            } else if (cur->token.data == "*") {
                val = UniversalSelector{};
            }
            break;
        case Css::Token::COLON:
            cur.next();
            if (cur->token.type == Css::Token::COLON) {
                cur.next();
                if (cur.ended()) {
                    logErrorIf(DEBUG_SELECTORS, "ERROR : unterminated selector");
                    return Error::invalidData("unterminated selector");
                }
            }

            if (cur->prefix == Css::Token::function("not(")) {
                Cursor<Css::Sst> c = cur->content;
                // consume a whole selector not a single one
                val = Selector::not_(try$(Selector::parse(c)));
            } else {
                val = Pseudo::make(cur->token.data);
            }
            break;
        default:
            val = ClassSelector{cur->token.data};
            break;
        }
    } else if (cur->type == Css::Sst::BLOCK) {
        val = _parseAttributeSelector(cur->content);
    } else {
        return Error::invalidData("unexped sst node");
    }

    cur.next();
    if (not cur.ended()) {
        Cursor rb = cur;
        OpCode nextOpCode = _peekOpCode(cur);
        if (nextOpCode > currentOp) {
            val = try$(_parseInfixExpr(val, cur, nextOpCode));
        } else {
            cur = rb;
        }
    }
    return Ok(val);
}

Res<Selector> _parseNfixExpr(Selector lhs, OpCode op, Cursor<Css::Sst>& cur) {
    Vec<Selector> selectors = {
        lhs,
        try$(_parseSelectorElement(cur, op)),
    };
    // all the selectors between the op eg : a,b.B,c -> [a,b.B,c]

    while (not cur.ended()) {
        Cursor<Css::Sst> rollBack = cur;

        OpCode nextOpCode = _peekOpCode(cur);

        if (nextOpCode == OpCode::NOP) {
            break;
        } else if (nextOpCode == op) {
            // adding the selector to the nfix
            selectors.pushBack(try$(_parseSelectorElement(cur, op)));
        } else if (nextOpCode == OpCode::COLUMN or nextOpCode == OpCode::OR or nextOpCode == OpCode::AND) {
            // parse new nfix
            if (nextOpCode < op) {
                cur = rollBack;
                break;
            }
            last(selectors) = try$(_parseNfixExpr(last(selectors), nextOpCode, cur));
        } else {
            // parse new infix if the next op is more important

            if (nextOpCode < op) {
                cur = rollBack;
                break;
            }

            if (not(cur.rem() == 2 and cur.peek(1) == Css::Token::WHITESPACE)) [[likely]] {
                last(selectors) = try$(_parseInfixExpr(last(selectors), cur, nextOpCode));

                // auto const lhs = _parseSelectorElement(cur, op);
                // selectors.pushBack(_parseInfixExpr(lhs, cur, nextOpCode));
            } else {
                last(selectors) = try$(_parseInfixExpr(last(selectors), cur, nextOpCode));
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

inline void unparse(Selector const& sel, Io::Emit& e) {
    sel.visit(Visitor{[&](Nfix const& s) {
                          if (s.type == Nfix::OR) {
                              for (usize i = 0; i < s.inners.len(); i++) {
                                  if (i != s.inners.len() - 1) {
                                      e("{},", s.inners[i]);
                                  } else {
                                      e("{}", s.inners[i]);
                                      ;
                                  }
                              }
                          } else if (s.type == Nfix::AND) {
                              for (usize i = 0; i < s.inners.len(); i++) {
                                  e("{}", s.inners[i]);
                                  ;
                              }
                          } else {
                              e("{}", s);
                          }
                      },
                      [&](Meta::Contains<UniversalSelector, ClassSelector, IdSelector, TypeSelector> auto const& s) -> void {
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
                      }});
}

inline bool Infix::operator==(Infix const&) const = default;

inline bool Nfix::operator==(Nfix const&) const = default;

// MARK: Selector Specificity ---------------------------------------------------

// https://www.w3.org/TR/selectors-3/#specificity
export Spec spec(Selector const& sel) {
    return sel.visit(Visitor{[](Nfix const& n) {
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
                             [](UniversalSelector const&) {
                                 return Spec::ZERO;
                             },
                             [](EmptySelector const&) {
                                 return Spec::ZERO;
                             },
                             [](IdSelector const&) {
                                 return Spec::A;
                             },
                             [](TypeSelector const&) {
                                 return Spec::C;
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
                             }});
}

} // namespace Vaev::Style
