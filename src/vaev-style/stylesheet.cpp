module;

#include <karm-mime/mime.h>

export module Vaev.Style:stylesheet;

import :rules;

namespace Vaev::Style {

// https://www.w3.org/TR/cssom-1/#css-style-sheets
struct StyleSheet {
    Mime::Mime mime = "text/css"_mime;
    Mime::Url href = ""_url;
    Str title = "";
    Vec<Rule> rules;
    Origin origin = Origin::AUTHOR;

    static StyleSheet parse(Css::Sst const& sst, Mime::Url href, Origin origin) {
        Namespace ns;

        if (sst != Css::Sst::LIST)
            panic("expected list");

        StyleSheet res;
        for (auto const& item : sst.content) {
            if (item == Css::Sst::RULE) {
                res.rules.pushBack(Rule::parse(item, origin));
            } else {
                logWarn("unexpected item in stylesheet: {}", item.type);
            }
        }

        res.origin = origin;
        res.href = href;

        return res;
    }

    static StyleSheet parse(Io::SScan& s, Mime::Url href, Origin origin = Origin::AUTHOR) {
        Css::Lexer lex{s};
        Css::Sst sst = consumeRuleList(lex, true);
        return parse(sst, href, origin);
    }

    void add(Rule&& rule) {
        rules.pushBack(std::move(rule));
    }

    void repr(Io::Emit& e) const {
        e("(style-sheet {} {} {}", mime, href, title);

        e.indent();
        if (rules) {
            e.newline();
            e("rules: [");
            e.indentNewline();
            for (auto const& rule : rules) {
                e("{}\n", rule);
            }
            e.deindent();
            e("]\n");
        }
        e.deindent();
        e(")");
    }
};

struct StyleSheetList {
    Vec<StyleSheet> styleSheets;

    void add(StyleSheet&& sheet) {
        styleSheets.pushBack(std::move(sheet));
    }

    void repr(Io::Emit& e) const {
        e("(style-sheet-list {})", styleSheets);
    }
};

} // namespace Vaev::Style
