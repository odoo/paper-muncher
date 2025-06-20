export module Vaev.Style:rules;

import :fonts;
import :media;
import :origin;
import :page;
import :props;
import :selector;
import :namespace_;

namespace Vaev::Style {

static bool DEBUG_RULE = false;

struct Rule;

// https://www.w3.org/TR/cssom-1/#the-cssstylerule-interface
export struct StyleRule {
    Selector selector = UNIVERSAL;
    Vec<StyleProp> props;
    Origin origin = Origin::AUTHOR;

    static StyleRule parse(Css::Sst const& sst, Origin origin) {
        if (sst != Css::Sst::RULE)
            panic("expected rule");

        if (sst.prefix != Css::Sst::LIST)
            panic("expected list");

        StyleRule res;

        // Parse the selector.
        auto& prefix = sst.prefix.unwrap();
        Cursor<Css::Sst> prefixContent = prefix->content;
        auto maybeSelector = Selector::parse(prefixContent);
        if (maybeSelector) {
            res.selector = maybeSelector.take();
        } else {
            logWarn("failed to parse selector: {}: {}", prefix->content, maybeSelector);
            res.selector = EmptySelector{};
        }

        // Parse the properties.
        for (auto const& item : sst.content) {
            if (item == Css::Sst::DECL) {
                auto prop = parseDeclaration<StyleProp>(item);
                if (prop)
                    res.props.pushBack(prop.take());
            } else {
                logWarnIf(DEBUG_RULE, "unexpected item in style rule: {}", item);
            }
        }

        res.origin = origin;
        return res;
    }

    Opt<Spec> match(Gc::Ref<Dom::Element> el) const {
        return matchSelector(selector, el);
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
    Mime::Url url;

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

    static MediaRule parse(Css::Sst const& sst, Origin origin, Namespace& ns) {
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
                logWarn("unexpected item in media rule: {}", item.type);
            }
        }

        return res;
    }

    bool match(Media const& m) const {
        return media.match(m);
    }

    void repr(Io::Emit& e) const {
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
    Symbol uri;

    static NamespaceRule parse(Css::Sst const& sst, Namespace& ns) {
        if (sst != Css::Sst::RULE)
            panic("expected rule");

        if (sst.prefix != Css::Sst::LIST)
            panic("expected list");

        auto& prefix = sst.prefix.unwrap();
        Cursor<Css::Sst> prefixContent = prefix->content;

        // Parse the prefix and URI.
        auto maybePrefix = Css::parseIdentifier(prefixContent);

        auto maybeUri = Css::parseString(prefixContent);
        if (!maybeUri) {
            logWarn("failed to parse namespace URI");
            return {};
        }

        // Store the namespace.
        if (maybePrefix)
            ns.prefixes[maybePrefix.unwrap()] = maybeUri.unwrap();
        else
            ns.default_ = maybeUri.unwrap();

        return {maybePrefix, maybeUri.take()};
    }

    void repr(Io::Emit& e) const {
        e("(namespace-rule {} {})", prefix, uri);
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
            return MediaRule::parse(sst, ns);
        else if (tok.data == "@import")
            return ImportRule::parse(sst);
        else if (tok.data == "@font-face")
            return FontFaceRule::parse(sst);
        else if (tok.data == "@page")
            return PageRule::parse(sst);
        else if (tok.data == "@supports") {
            logWarn("cannot parse '@supports' at-rule");
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

} // namespace Vaev::Style
