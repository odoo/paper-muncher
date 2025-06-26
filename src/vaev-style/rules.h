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

    // this rule ID can only be used if we find it at least minCount times
    Map<usize, usize> ruleIdToNeededCount;

    Vec<RuleWithId> nonLookupRules;
    usize _ruleCount = 0;

    template <typename T>
    void _initDictIfEmpty(T key, Map<T, Vec<RuleWithId>>& dict) {
        if (not dict.has(key))
            dict.put(key, Vec<RuleWithId>{});
    }

    void _buildRuleFromSelector(Cursor<Rule> rule, usize _ruleCount, Selector const& selector) {
        // Recursive level here should not be deeper than 2
        // If it is 1, it should be to a lookupable selector
        // If it is 2, it should be infix/nfix and then lookupable selector
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
                if (lookupIsMatch(s.rhs) or s.rhs->is<Nfix>()) {
                    _buildRuleFromSelector(rule, _ruleCount, s.rhs);
                } else {
                    nonLookupRules.pushBack({_ruleCount, rule});
                }
            },
            [&](Nfix const& s) {
                if (s.type == Nfix::AND) {
                    // the lookups are a condition to match the rule
                    // if there are no lookups, we add the rule to the fallback list
                    // if there are X lookups and we get all the X instances for an element, we are "allowed" to eval the rule
                    // (maybe we can strip the lookupble selectors from this infix so they are not evaluated again. if
                    // thats not possible, we should build infixes putting lookupable selectors first since these are cheaper to evaluate.
                    // actually, putting them first is 100% agnostic of this lookup, its just benefitting from short circuit evaluation)
                    usize conditionsCount = 0;
                    for (auto const& inner : s.inners) {
                        if (lookupIsMatch(inner)) {
                            conditionsCount++;
                            _buildRuleFromSelector(rule, _ruleCount, inner);
                        }
                    }

                    if (conditionsCount == 0) {
                        nonLookupRules.pushBack({_ruleCount, rule});
                    } else {
                        ruleIdToNeededCount.put(_ruleCount, conditionsCount);
                    }
                } else if (s.type == Nfix::OR) {
                    // if an element has 2 or more occourence of this rule in its list, we can assume
                    // the rule as matched, since at least one of the occourences is due to a lookupable selector,
                    // which is guaranteed to match
                    // it also makes sense to order the inners of the infix by putting lookupable selectors first, since
                    // these should be cheaper to evaluate
                    bool hasNonLookupable = false;
                    for (auto const& inner : s.inners) {
                        if (lookupIsMatch(inner)) {
                            _buildRuleFromSelector(rule, _ruleCount, inner);
                        } else {
                            hasNonLookupable = true;
                        }

                        if (hasNonLookupable)
                            nonLookupRules.pushBack({_ruleCount, rule});
                    }
                } else {
                    nonLookupRules.pushBack({_ruleCount, rule});
                }
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

    Vec<Cursor<RuleWithId>> collectCursors(Gc::Ref<Dom::Element> el) {
        Vec<Cursor<RuleWithId>> cursors;

        for (auto const& class_ : el->classList._tokens) {
            if (classRules.has(class_)) {
                auto const& rules = classRules.get(class_);
                cursors.pushBack({rules.buf(), rules.len()});
            }
        }

        if (auto id = el->id()) {
            if (iDRules.has(*id)) {
                auto const& rules = iDRules.get(*id);
                cursors.pushBack({rules.buf(), rules.len()});
            }
        }

        if (auto tag = el->tagName.name()) {
            if (typeRules.has(tag)) {
                auto const& rules = typeRules.get(tag);
                cursors.pushBack({rules.buf(), rules.len()});
            }
        }

        for (auto const& [name, value] : el->attributes.iter()) {
            if (auto attrName = name.name()) {
                if (attrPresentRules.has(attrName)) {
                    auto const& rules = attrPresentRules.get(attrName);
                    cursors.pushBack({rules.buf(), rules.len()});
                }
                if (attrExactValueRules.has({attrName, value->value})) {
                    auto const& rules = attrExactValueRules.get({attrName, value->value});
                    cursors.pushBack({rules.buf(), rules.len()});
                }
            }
        }

        cursors.pushBack({nonLookupRules.buf(), nonLookupRules.len()});

        return cursors;
    }
};

} // namespace Vaev::Style
