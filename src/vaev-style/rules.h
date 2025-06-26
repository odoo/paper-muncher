#pragma once

#include "fonts.h"
#include "media.h"
#include "origin.h"
#include "page.h"
#include "props.h"
#include "selector.h"

namespace Vaev::Style {

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

struct Rule : _Rule {
    using _Rule::_Rule;

    void repr(Io::Emit& e) const;

    static Rule parse(Css::Sst const& sst, Origin origin = Origin::AUTHOR);
};

using RuleWithId = Tuple<usize, Cursor<Rule>>;

struct StyleSheetList;

struct RuleLookup {
    Map<String, Vec<RuleWithId>> typeRules;
    Map<String, Vec<RuleWithId>> iDRules;
    Map<String, Vec<RuleWithId>> classRules;

    Map<String, Vec<RuleWithId>> attrPresentRules;
    Map<Pair<String>, Vec<RuleWithId>> attrExactValueRules;

    Vec<RuleWithId> nonLookupRules;
    usize _ruleCount = 0;

    template <typename T>
    void _initDictIfEmpty(T key, Map<T, Vec<RuleWithId>>& dict) {
        if (not dict.has(key))
            dict.put(key, Vec<RuleWithId>{});
    }

    void _buildRuleFromSelector(Cursor<Rule> rule, usize _ruleCount, Selector const& selector) {
        selector.visit(Visitor{
            [&](TypeSelector const& s) {
                _initDictIfEmpty(s.elementName, typeRules);
                typeRules.get(s.elementName).pushBack({_ruleCount, rule});
            },
            [&](IdSelector const& s) {
                _initDictIfEmpty(s.id, iDRules);
                iDRules.get(s.id).pushBack({_ruleCount, rule});
            },
            [&](ClassSelector const& s) {
                _initDictIfEmpty(s.class_, classRules);
                classRules.get(s.class_).pushBack({_ruleCount, rule});
            },
            [&](AttributeSelector const& s) {
                if (s.match == AttributeSelector::Match::PRESENT) {
                    _initDictIfEmpty(s.name, attrPresentRules);
                    attrPresentRules.get(s.name).pushBack({_ruleCount, rule});
                } else if (s.match == AttributeSelector::Match::EXACT) {
                    _initDictIfEmpty({s.name, s.value}, attrExactValueRules);
                    attrExactValueRules.get({s.name, s.value}).pushBack({_ruleCount, rule});
                } else {
                    nonLookupRules.pushBack({_ruleCount, rule});
                }
            },
            [&](Infix const& s) {
                _buildRuleFromSelector(rule, _ruleCount, s.rhs);
            },
            [&](auto const&) {
                nonLookupRules.pushBack({_ruleCount, rule});
            },
        });
    }

    void _buildRule(Cursor<Rule> rule, Media media) {
        rule->visit(Visitor{
            [&](StyleRule const& r) {
                _ruleCount++;
                _buildRuleFromSelector(rule, _ruleCount, r.selector);
            },
            [&](MediaRule const& r) {
                if (r.match(media))
                    for (auto const& subRule : r.rules)
                        _buildRule(&subRule, media);
            },
            [&](auto const&) {
                // Ignore other rule types
            },
        });
    }

    void build(Media media, StyleSheetList const& styleBook);

    static bool lookupIsMatch(AttributeSelector const& selector) {
        return selector.match == AttributeSelector::Match::PRESENT ||
               selector.match == AttributeSelector::Match::EXACT;
    }

    static bool lookupIsMatch(Selector const& selector) {
        if (selector.is<TypeSelector>() || selector.is<IdSelector>() ||
            selector.is<ClassSelector>())
            return true;

        if (not selector.is<AttributeSelector>())
            return false;

        return lookupIsMatch(selector.unwrap<AttributeSelector>());
    }
};

} // namespace Vaev::Style
