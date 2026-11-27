module;

#include <karm/macros>

export module Vaev.Engine:style.select.anb;

import Karm.Core;
import Karm.Diag;

import :css;
import :css.values;
import :style.values;

using namespace Karm;
using namespace Karm::Literals;

namespace Vaev::Style {

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

} // namespace Vaev::Style
