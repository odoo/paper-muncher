export module Vaev.Script:lang.ast;

import Karm.Core;
import :value;

using namespace Karm;

namespace Vaev::Script::Ast {

// MARK: Identifier ------------------------------------------------------------

export struct Identifier {};

export struct PrivateIdentifier {};

// MARK: Literal ---------------------------------------------------------------

export struct NumericLiteral {};

export struct StringLiteral {};

// https://tc39.es/ecma262/#prod-Literal
export using Literal = Union<
    Null,
    Boolean,
    NumericLiteral,
    StringLiteral>;

// MARK: Expression ------------------------------------------------------------

// https://tc39.es/ecma262/#sec-ecmascript-language-expressions
export struct Expression {
};

// https://tc39.es/ecma262/#prod-PrimaryExpression
// NOSPEC: This is supposed to be part of PrimaryExpression
//         but for simplicity we chop it down
export struct ThisExpression : Expression {
};

// https://tc39.es/ecma262/#prod-PrimaryExpression
// NOSPEC: This is supposed to be part of PrimaryExpression
//         but for simplicity we chop it down
export struct LiteralExpression : Expression {
    Literal literal;
};

// https://tc39.es/ecma262/#prod-IdentifierReference
export struct IdentifierExpression : Expression {
    Identifier identifier;
};

// https://tc39.es/ecma262/#prod-MemberExpression
export struct MemberExpression {
    Box<Expression> expression;
    Union<Box<Expression>, Identifier, PrivateIdentifier> identifier;
};

// MARK: Statement -------------------------------------------------------------
// https://tc39.es/ecma262/#sec-ecmascript-language-statements-and-declarations

// https://tc39.es/ecma262/#prod-Statement
export struct Statement {
};

// https://tc39.es/ecma262/#prod-ExpressionStatement
export struct ExpressionStatement : Statement {
    Box<Expression> expression;
};

// MARK: Script ---------------------------------------------------------------

// https://tc39.es/ecma262/#prod-Script
export struct Script {
    Vec<Box<Statement>> body;
};

} // namespace Vaev::Script::Ast
