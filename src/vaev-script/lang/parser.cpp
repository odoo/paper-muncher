module;

#include <karm/macros>

export module Vaev.Script:lang.parser;

import Karm.Core;
import :lang.ast;
import :lang.lexer;

using namespace Karm;

namespace Vaev::Script {

// https://tc39.es/ecma262/#prod-Expression
static Res<Box<Ast::Expression>> parseExpression(Cursor<Token>& c) {
    if (c.skip(Token::THIS))
        return Ok(Ast::ThisExpression{});

    return Error::invalidInput("expected statement");
}

// https://tc39.es/ecma262/#prod-Statement
static Res<Box<Ast::Statement>> parseStatement(Cursor<Token>& c) {
    return Ok(Ast::ExpressionStatement{
        .expression = try$(parseExpression(c)),
    });
}

// https://tc39.es/ecma262/#prod-StatementList
static Res<Vec<Box<Ast::Statement>>> parseStatementList(Cursor<Token>& c) {
    Vec<Box<Ast::Statement>> statements;
    while (not c.ended())
        statements.pushBack(try$(parseStatement(c)));
    return Ok(std::move(statements));
}

// https://tc39.es/ecma262/#prod-Script
export Res<Ast::Script> parseScript(Cursor<Token>& c) {
    return Ok(Ast::Script{
        .body = try$(parseStatementList(c)),
    });
}

} // namespace Vaev::Script
