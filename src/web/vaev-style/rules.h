#pragma once

#include "fonts.h"
#include "media.h"
#include "origin.h"
#include "page.h"
#include "props.h"
#include "selector.h"

namespace Vive::Style {

struct Rule;

// https://www.w3.org/TR/cssom-1/#the-cssstylerule-interface
struct StyleRule {
    Selector selector = UNIVERSAL;
    Vec<StyleProp> props;
    Origin origin = Origin::AUTHOR;

    void repr(Io::Emit& e) const;

    Opt<Spec> match(Gc::Ref<Dom::Element> el) const;

    static StyleRule parse(Css::Sst const& sst, Origin origin = Origin::AUTHOR);
};

// https://www.w3.org/TR/cssom-1/#the-cssimportrule-interface
struct ImportRule {
    Mime::Url url;

    void repr(Io::Emit& e) const;

    static ImportRule parse(Css::Sst const&);
};

// https://www.w3.org/TR/css-conditional-3/#the-cssmediarule-interface
struct MediaRule {
    MediaQuery media;
    Vec<Rule> rules;

    void repr(Io::Emit& e) const;

    bool match(Media const& m) const;

    static MediaRule parse(Css::Sst const& sst);
};

// https://www.w3.org/TR/css-fonts-4/#cssfontfacerule
struct FontFaceRule {
    Vec<FontDesc> descs;

    void repr(Io::Emit& e) const;

    static FontFaceRule parse(Css::Sst const& sst);
};

// https://www.w3.org/TR/cssom-1/#the-cssrule-interface
using _Rule = Union<
    StyleRule,
    FontFaceRule,
    MediaRule,
    ImportRule,
    PageRule>;

struct Rule : public _Rule {
    using _Rule::_Rule;

    void repr(Io::Emit& e) const;

    static Rule parse(Css::Sst const& sst, Origin origin = Origin::AUTHOR);
};

} // namespace Vaev::Style
