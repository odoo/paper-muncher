export module Vaev.Engine:style.rules;

import Karm.Core;
import Karm.Gc;
import Karm.Ref;
import Karm.Logger;

import :css;
import :style.fonts;
import :style.media;
import :style.origin;
import :style.page;
import :style.props;
import :style.selector;
import :style.namespace_;
import :style.matcher;

using namespace Karm;

namespace Vaev::Style {

static auto debugRule = Debug::Flag::debug("web-style-rule", "Print error message related to css rule parsing");

export struct Rule;

// https://www.w3.org/TR/cssom-1/#the-cssstylerule-interface
export struct StyleRule {
    Selector selector = TypeSelector::universal();
    Vec<StyleProp> props;
    Origin origin = Origin::AUTHOR;

    static StyleRule parse(Css::Sst const& sst, Origin origin, Namespace& ns) {
        if (sst != Css::Sst::RULE)
            panic("expected rule");

        if (sst.prefix != Css::Sst::LIST)
            panic("expected list");

        StyleRule res;

        // Parse the selector.
        auto& prefix = sst.prefix.unwrap();
        Cursor<Css::Sst> prefixContent = prefix->content;
        auto maybeSelector = Selector::parse(prefixContent, ns);
        if (maybeSelector) {
            res.selector = maybeSelector.take();
        } else {
            logWarnIf(debugRule, "failed to parse selector: {}: {}", prefix->content, maybeSelector);
            res.selector = EmptySelector{};
        }

        // Parse the properties.
        for (auto const& item : sst.content) {
            if (item == Css::Sst::DECL) {
                auto prop = parseDeclaration<StyleProp>(item);
                if (prop)
                    res.props.pushBack(prop.take());
            } else {
                logWarnIf(debugRule, "unexpected item in style rule: {}", item);
            }
        }

        res.origin = origin;
        return res;
    }

    Opt<Spec> match(Gc::Ref<Dom::Element> element, Opt<Symbol> pseudoElement) const {
        return matchSelector(selector, element, pseudoElement);
    }

    void repr(Io::Emit& e) const {
        e("(style-rule");
        e.indent();
        e("\nselector: {}", selector);
        if (props) {
            e.newline();
            e("props: [");
            e.indentNewline();
            for (auto const& prop : props) {
                e("{}\n", prop);
            }
            e.deindent();
            e("]\n");
        }
        e.deindent();
        e(")");
    }
};

// https://www.w3.org/TR/cssom-1/#the-cssimportrule-interface
export struct ImportRule {
    Ref::Url url;

    static ImportRule parse(Css::Sst const&) {
        return {};
    }

    void repr(Io::Emit& e) const {
        e("(import-rule {})", url);
    }
};

// https://www.w3.org/TR/css-conditional-3/#the-cssmediarule-interface
export struct MediaRule {
    MediaQuery media;
    Vec<Rule> rules;

    static MediaRule parse(Css::Sst const& sst, Origin origin, Namespace& ns);

    bool match(Media const& m) const {
        return media.match(m);
    }

    void repr(Io::Emit& e) const;
};

// https://www.w3.org/TR/css-fonts-4/#cssfontfacerule
export struct FontFaceRule {
    Vec<FontDesc> descs;

    static FontFaceRule parse(Css::Sst const& sst) {
        return {parseDeclarations<FontDesc>(sst, false)};
    }

    void repr(Io::Emit& e) const {
        e("(font-face-rule {})", descs);
    }
};

// https://drafts.csswg.org/css-namespaces/#syntax
export struct NamespaceRule {
    Opt<Symbol> prefix;
    Symbol url;

    static NamespaceRule parse(Css::Sst const& sst, Namespace& ns) {
        if (sst != Css::Sst::RULE)
            panic("expected rule");

        if (sst.prefix != Css::Sst::LIST)
            panic("expected list");

        auto& prefix = sst.prefix.unwrap();
        Cursor<Css::Sst> prefixContent = prefix->content;

        eatWhitespace(prefixContent);
        Opt<Symbol> maybePrefix;
        if (*prefixContent == Css::Token::IDENT) {
            maybePrefix = Symbol::from(prefixContent->token.data);
            prefixContent.next();
        }

        eatWhitespace(prefixContent);
        auto maybeUrl = parseValue<String>(prefixContent);
        if (not maybeUrl) {
            maybeUrl = parseUrlIntoString(prefixContent);
        }
        if (not maybeUrl) {
            logWarn("expected namespace URI, got: {}", prefixContent);
            return {NONE, ""_sym};
        }

        // Store the namespace.
        if (maybePrefix)
            ns.prefixes.put(maybePrefix.unwrap(), Symbol::from(maybeUrl.unwrap()));
        else
            ns.default_ = Symbol::from(maybeUrl.unwrap());

        return {maybePrefix, Symbol::from(maybeUrl.take())};
    }

    void repr(Io::Emit& e) const {
        e("(namespace-rule {} {})", prefix, url);
    }
};

export struct PageRule {
    Vec<PageSelector> selectors;
    Vec<StyleProp> props;
    Vec<PageAreaRule> areas;

    static PageRule parse(Css::Sst const& sst) {
        if (sst != Css::Sst::RULE)
            panic("expected rule");

        if (sst.prefix != Css::Sst::LIST)
            panic("expected list");

        PageRule res;

        // Parse the selector
        auto& prefix = sst.prefix.unwrap();
        Cursor<Css::Sst> prefixContent = prefix->content;
        res.selectors = PageSelector::parseList(prefixContent);

        // Parse the properties.
        for (auto const& item : sst.content) {
            if (item == Css::Sst::DECL) {
                auto prop = parseDeclaration<StyleProp>(item);
                if (prop)
                    res.props.pushBack(prop.take());
            } else if (item == Css::Sst::RULE and
                       item.token == Css::Token::AT_KEYWORD) {
                auto rule = PageAreaRule::parse(item);
                if (rule)
                    res.areas.pushBack(*rule);
            } else {
                logWarnIf(debugRule, "unexpected item in style rule: {}", item);
            }
        }

        return res;
    }

    bool match(Page const& page) const {
        if (selectors.len() == 0)
            return true;

        for (auto& s : selectors) {
            if (s.match(page))
                return true;
        }

        return false;
    }

    void apply(PageSpecifiedValues& c) const {
        for (auto const& prop : props) {
            prop.apply(*c.style, *c.style);
        }

        for (auto const& area : areas) {
            auto& areaPseudoElement = c.area(area.area);
            area.apply(*areaPseudoElement.specifiedValues());
        }
    }

    void repr(Io::Emit& e) const {
        e("(page-rule\nselectors: {}\nprops: {}\nmargins: {})", selectors, props, areas);
    }
};

// https://www.w3.org/TR/cssom-1/#the-cssrule-interface
using _Rule = Union<
    StyleRule,
    FontFaceRule,
    MediaRule,
    ImportRule,
    NamespaceRule,
    PageRule>;

export struct Rule : _Rule {
    using _Rule::_Rule;

    static Rule parse(Css::Sst const& sst, Origin origin, Namespace& ns) {
        if (sst != Css::Sst::RULE)
            panic("expected rule");

        auto tok = sst.token;
        if (tok.data == "@media")
            return MediaRule::parse(sst, origin, ns);
        else if (tok.data == "@import")
            return ImportRule::parse(sst);
        else if (tok.data == "@font-face")
            return FontFaceRule::parse(sst);
        else if (tok.data == "@page")
            return PageRule::parse(sst);
        else if (tok.data == "@supports") {
            logWarnIf(debugRule, "cannot parse '@supports' at-rule");
            return StyleRule{};
        } else if (tok.data == "@namespace") {
            return NamespaceRule::parse(sst, ns);
        } else
            return StyleRule::parse(sst, origin, ns);
    }

    void repr(Io::Emit& e) const {
        visit([&](auto const& r) {
            e("{}", r);
        });
    }
};

MediaRule MediaRule::parse(Css::Sst const& sst, Origin origin, Namespace& ns) {
    if (sst != Css::Sst::RULE)
        panic("expected rule");

    if (sst.prefix != Css::Sst::LIST)
        panic("expected list");

    MediaRule res;

    // Parse the media query.
    auto& prefix = sst.prefix.unwrap();
    Cursor<Css::Sst> prefixContent = prefix->content;
    res.media = parseMediaQuery(prefixContent);

    // Parse the rules.
    for (auto const& item : sst.content) {
        if (item == Css::Sst::RULE) {
            res.rules.pushBack(Rule::parse(item, origin, ns));
        } else {
            logWarnIf(debugRule, "unexpected item in media rule: {}", item.type);
        }
    }

    return res;
}

void MediaRule::repr(Io::Emit& e) const {
    e("(media-rule");
    e.indent();
    e("\nmedia: {}", media);
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
}

using MatchingRules = Vec<Tuple<Cursor<StyleRule>, Spec>>;

} // namespace Vaev::Style
