module;

#include <karm-base/vec.h>
#include <karm-gc/ptr.h>
#include <karm-io/emit.h>
#include <karm-logger/logger.h>
#include <karm-mime/url.h>
#include <vaev-dom/element.h>

export module Vaev.Style:rules;

import :fonts;
import :media;
import :origin;
import :props;
import :selector;
import :matcher;
import :decls;
import :page;

namespace Vaev::Style {

export struct Rule;

static constexpr bool DEBUG_RULE = false;

// MARK: StyleRule -------------------------------------------------------------

// https://www.w3.org/TR/cssom-1/#the-cssstylerule-interface
export struct StyleRule {
    Selector selector = UNIVERSAL;
    Vec<StyleProp> props;
    Origin origin = Origin::AUTHOR;

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

    Opt<Spec> match(Gc::Ref<Dom::Element> el) const {
        return matchSelector(selector, el);
    }

    static StyleRule parse(Css::Sst const& sst, Origin origin = Origin::AUTHOR) {
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
};

// MARK: ImportRule ------------------------------------------------------------

// https://www.w3.org/TR/cssom-1/#the-cssimportrule-interface
export struct ImportRule {
    Mime::Url url;

    void repr(Io::Emit& e) const {
        e("(import-rule {})", url);
    }

    static ImportRule parse(Css::Sst const&) {
        return {};
    }
};

// MARK: MediaRule -------------------------------------------------------------

// https://www.w3.org/TR/css-conditional-3/#the-cssmediarule-interface
export struct MediaRule {
    MediaQuery media;
    Vec<Rule> rules;

    void repr(Io::Emit& e) const;

    bool match(Media const& m) const {
        return media.match(m);
    }

    static MediaRule parse(Css::Sst const& sst);
};

// MARK: FontFaceRule ----------------------------------------------------------

// https://www.w3.org/TR/css-fonts-4/#cssfontfacerule
export struct FontFaceRule {
    Vec<FontDesc> descs;

    void repr(Io::Emit& e) const {
        e("(font-face-rule {})", descs);
    }

    static FontFaceRule parse(Css::Sst const& sst) {
        return {parseDeclarations<FontDesc>(sst, false)};
    }
};

// MARK: Rule ------------------------------------------------------------------

// https://www.w3.org/TR/cssom-1/#the-cssrule-interface
using _Rule = Union<
    StyleRule,
    FontFaceRule,
    MediaRule,
    ImportRule,
    PageRule>;

struct Rule : public _Rule {
    using _Rule::_Rule;

    void repr(Io::Emit& e) const {
        visit([&](auto const& r) {
            e("{}", r);
        });
    }

    static Rule parse(Css::Sst const& sst, Origin origin = Origin::AUTHOR) {
        if (sst != Css::Sst::RULE)
            panic("expected rule");

        auto tok = sst.token;
        if (tok.data == "@media")
            return MediaRule::parse(sst);
        else if (tok.data == "@import")
            return ImportRule::parse(sst);
        else if (tok.data == "@font-face")
            return FontFaceRule::parse(sst);
        else if (tok.data == "@page")
            return PageRule::parse(sst);
        else if (tok.data == "@supports") {
            logWarn("cannot parse '@supports' at-rule");
            return StyleRule{};
        } else
            return StyleRule::parse(sst, origin);
    }
};

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

MediaRule MediaRule::parse(Css::Sst const& sst) {
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
            res.rules.pushBack(Rule::parse(item));
        } else {
            logWarn("unexpected item in media rule: {}", item.type);
        }
    }

    return res;
}

} // namespace Vaev::Style
