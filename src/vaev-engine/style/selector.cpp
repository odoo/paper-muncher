module;

#include <karm/macros>

export module Vaev.Engine:style.selector;

import Karm.Core;
import Karm.Logger;

import :css;
import :css.values;
import :style.values;
import :dom.element;

using namespace Karm;

namespace Vaev::Style {

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

    bool operator==(Selector const&) const;

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

    static Res<PseudoClassSelector> _parsePseudoClassFunction(Cursor<Css::Sst>& cur, NamespaceScope const& ns) {
        auto funcName = cur->prefix.expect()->token.data.str();
        funcName = Str{funcName.begin(), funcName.len() - 1};

        Cursor<Css::Sst> c = cur->content;
        auto anb = try$(AnB::parse(c));
        eatWhitespace(c);

        if (c.ended()) {
            return Ok(PseudoClassSelector::make(funcName, PseudoClassSelector::AnBofS{anb, NONE}));
        }

        if (funcName == "nth-of-type" or funcName == "nth-last-of-type")
            return Error::invalidData("unexpected content after AnB nth-*of-type");

        if (not c.skip(Css::Token::ident("of")))
            return Error::invalidData("expected 'of' in pseudo-class function");

        return Ok(PseudoClassSelector::make(funcName, PseudoClassSelector::AnBofS{anb, Some(makeBox<Selector>(try$(Selector::parse(c, ns))))}));
    }

    // consume a selector element (everything  that has a lesser priority than the current OP)
    static Res<Selector> _parseSelectorElement(Cursor<Css::Sst>& cur, OpCode currentOp, NamespaceScope const& ns) {
        if (cur.ended()) {
            logErrorIf(DEBUG_SELECTORS, "unterminated selector");
            return Error::invalidData("unterminated selector");
        }

        Selector val = Selector::empty();
        bool consumed = false;
        if (*cur == Css::Sst::TOKEN) {
            switch (cur->token.type) {
            case Css::Token::WHITESPACE:
                cur.next();
                return _parseSelectorElement(cur, currentOp, ns);

            case Css::Token::HASH:
                val = IdSelector{Symbol::from(next(cur->token.data, 1))};
                break;
            case Css::Token::IDENT: {
                Cursor qualifiedNameCur = cur;
                val = TypeSelector{try$(_parseQualifiedNameSelector(qualifiedNameCur, ns, ns.defaultNamespacePattern()))};
                cur = qualifiedNameCur;
                consumed = true;
            } break;
            case Css::Token::DELIM:
                if (cur->token.data == ".") {
                    cur.next();
                    val = ClassSelector{cur->token.data};
                } else if (cur->token.data == "*") {
                    Cursor qualifiedNameCur = cur;
                    val = TypeSelector{try$(_parseQualifiedNameSelector(qualifiedNameCur, ns, ns.defaultNamespacePattern()))};
                    cur = qualifiedNameCur;
                    consumed = true;
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

                if (cur->type == Css::Sst::Type::TOKEN) {
                    Str name = cur->token.data;
                    if (name == "before")
                        val = PseudoElementSelector{Dom::PseudoElement::BEFORE};
                    else if (name == "after")
                        val = PseudoElementSelector{Dom::PseudoElement::AFTER};
                    else if (name == "marker")
                        val = PseudoElementSelector{Dom::PseudoElement::MARKER};
                    else
                        val = PseudoClassSelector::make(cur->token.data);
                } else if (cur->type == Css::Sst::Type::FUNC) {
                    if (cur->prefix == Css::Token::function("not(")) {
                        Cursor<Css::Sst> c = cur->content;
                        // consume a whole selector not a single one
                        val = not_(try$(Selector::parse(c, ns)));
                    } else if (cur->prefix == Css::Token::function("is(")) {
                        // https://www.w3.org/TR/selectors-4/#matches
                        Cursor<Css::Sst> c = cur->content;
                        val = try$(Selector::parse(c, ns));
                    } else if (cur->prefix == Css::Token::function("where(")) {
                        // https://www.w3.org/TR/selectors-4/#zero-matches
                        Cursor<Css::Sst> c = cur->content;
                        val = where(try$(Selector::parse(c, ns)));
                    } else {
                        val = try$(_parsePseudoClassFunction(cur, ns));
                    }
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

        if (not consumed)
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

    static Selector _impliedDefaultNamespace(NamespaceScope const& ns) {
        return Selector{TypeSelector{
            ns.defaultNamespacePattern(),
            QualifiedNameSelector::NamePattern{UNIVERSAL},
        }};
    }

    static Res<Selector> _parseNfixExpr(Selector lhs, OpCode op, Cursor<Css::Sst>& cur, NamespaceScope const& ns) {
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
            if (ns.default_) {
                auto hasTypeSelector =
                    iter(selectors) | Any([](Selector const& s) {
                        return s.is<TypeSelector>();
                    });

                if (not hasTypeSelector)
                    selectors.pushFront(_impliedDefaultNamespace(ns));
            }

            return Ok(Selector::and_(selectors));

        case OpCode::OR:
            return Ok(Selector::or_(selectors));

        default:
            return Ok(Selector::and_(selectors));
        }
    }

    static Res<Selector> _parseInfixExpr(Selector lhs, Cursor<Css::Sst>& cur, OpCode opCode, NamespaceScope const& ns) {
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

    static Res<Selector> parse(Cursor<Css::Sst>& c, NamespaceScope const& ns = {}) {
        if (not c)
            return Error::invalidData("expected selector");

        logDebugIf(DEBUG_SELECTORS, "PARSING SELECTOR : {}", c);
        Selector selector = try$(_parseSelectorElement(c, OpCode::NOP, ns));

        while (not c.ended()) {
            auto oldC = c;
            selector = try$(_parseInfixExpr(selector, c, OpCode::NOP, ns));
            if (not c.ended() and oldC == c)
                return Error::invalidData("unexpected token after selector");
        }

        // a lone class, id, attribute or pseudo selector. Needs to be namespace qualified
        if (ns.default_ and
            not selector.is<Infix>() and
            not selector.is<Nfix>() and
            not selector.is<TypeSelector>() and
            not selector.is<EmptySelector>()) {
            return Ok(and_({_impliedDefaultNamespace(ns), selector}));
        }

        return Ok(selector);
    }

    static Res<Selector> parse(Io::SScan& s, NamespaceScope const& ns = {}) {
        Css::Lexer lex = s;
        Diag::Collector diags;
        auto val = consumeSelector(lex, diags);
        Cursor<Css::Sst> c{val};
        return parse(c, ns);
    };

    static Res<Selector> parse(Str input, NamespaceScope const& ns = {}) {
        Io::SScan s{input};
        return parse(s, ns);
    };

    auto unparsed() lifetimebound {
        struct SelectorUnparser {
            Selector& s;

            void repr(Io::Emit& e) const {
                unparse(s, e);
            }
        };

        return SelectorUnparser{*this};
    }
};

} // namespace Vaev::Style
