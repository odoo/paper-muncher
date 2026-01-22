module;

#include <karm-core/macros.h>

export module Vaev.Engine:style.decls;

import Karm.Math;
import Karm.Logger;

import :css;
import :props;

namespace Vaev::Style {

template <typename T>
Res<T> _parseDeclarationValue(Cursor<Css::Sst>& c) {
    if constexpr (requires { T{}.parse(c); }) {
        T t;
        try$(t.parse(c));

        return Ok(std::move(t));
    } else {
        return Error::notImplemented("missing parser for declaration");
    }
}

template <typename P, typename T>
Res<P> _parseDeclaration(Css::Sst const& sst) {
    Cursor<Css::Sst> content = sst.content;

    eatWhitespace(content);
    P prop = try$(_parseDeclarationValue<T>(content));

    if constexpr (requires { P::important; })
        prop.important = sst.important;

    if (not content.ended()) {
        return Error::invalidData("unknown tokens in content");
    }

    return Ok(std::move(prop));
}

export template <typename P>
Res<P> parseDeclaration(Css::Sst const& sst) {
    if (sst != Css::Sst::DECL)
        panic("expected declaration");

    if (sst.token != Css::Token::IDENT)
        panic("expected ident");

    Res<P> resDecl = Error::invalidData("unknown declaration");

    P::any(
        Visitor{
            [&]<typename T>() -> bool {
                if (sst.token != Css::Token::ident(T::name()))
                    return false;
                resDecl = _parseDeclaration<P, T>(sst);
                return true;
            }
        }
    );

    return resDecl;
}

export template <typename P>
Vec<P> parseDeclarations(Css::Content const& sst) {
    Vec<P> res;

    for (auto const& item : sst) {
        if (item != Css::Sst::DECL) {
            continue;
        }

        auto prop = parseDeclaration<P>(item);

        if (not prop) {
            continue;
        }
        res.pushBack(prop.take());
    }

    return res;
}

export template <typename P>
Vec<P> parseDeclarations(Css::Sst const& sst) {
    return parseDeclarations<P>(sst.content);
}

export template <typename P>
Vec<P> parseDeclarations(Str style) {
    Css::Lexer lex{style};
    auto sst = Css::consumeDeclarationList(lex, true);
    return parseDeclarations<P>(sst);
}

} // namespace Vaev::Style
