#include "rules.h"

#include "decls.h"
#include "matcher.h"

namespace Vaev::Style {

static bool DEBUG_RULE = false;

// MARK: StyleRule -------------------------------------------------------------

Opt<Spec> StyleRule::match(Gc::Ref<Dom::Element> el) const {
    return matchSelector(selector, el);
}

void StyleRule::repr(Io::Emit& e) const {
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

StyleRule StyleRule::parse(Css::Sst const& sst, Origin origin) {
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

// MARK: ImportRule ------------------------------------------------------------

void ImportRule::repr(Io::Emit& e) const {
    e("(import-rule {})", url);
}

ImportRule ImportRule::parse(Css::Sst const&) {
    return {};
}

// MARK: MediaRule -------------------------------------------------------------

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

bool MediaRule::match(Media const& m) const {
    return media.match(m);
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

// MARK: FontFaceRule ----------------------------------------------------------

void FontFaceRule::repr(Io::Emit& e) const {
    e("(font-face-rule {})", descs);
}

FontFaceRule FontFaceRule::parse(Css::Sst const& sst) {
    return {parseDeclarations<FontDesc>(sst, false)};
}

// MARK: Rule ------------------------------------------------------------------

void Rule::repr(Io::Emit& e) const {
    visit([&](auto const& r) {
        e("{}", r);
    });
}

Rule Rule::parse(Css::Sst const& sst, Origin origin) {
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

void RuleLookup::_buildRuleFromSelector(Cursor<Rule> rule, usize _ruleCount, Selector const& selector) {
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
                // NOTE: The lookups are a condition to try to match the rule
                // If there are no lookups, we add the rule to the fallback list
                // If there are X lookups and we get all the X instances for an element, we are "allowed" to eval the rule

                // TODO: we can remove the looupable selectors from the nfix since they are already handled by the
                // lookup phase. however, computing specificy should be done before removing said selectors
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
                // NOTE: If an element has 2 or more occourence of this rule in its list, we can assume
                // the rule as matched, since at least one of the occourences is due to a lookupable selector,
                // which is guaranteed to match
                bool hasNonLookupable = false;
                for (auto const& inner : s.inners) {
                    if (lookupIsMatch(inner)) {
                        _buildRuleFromSelector(rule, _ruleCount, inner);
                    } else {
                        hasNonLookupable = true;
                    }
                }
                if (hasNonLookupable)
                    nonLookupRules.pushBack({_ruleCount, rule});
            } else {
                nonLookupRules.pushBack({_ruleCount, rule});
            }
        },
        [&](auto const&) {
            nonLookupRules.pushBack({_ruleCount, rule});
        },
    });
}

void RuleLookup::_buildRule(Cursor<Rule> rule, Media media) {
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

Vec<Cursor<RuleLookup::RuleWithId>> RuleLookup::collectMatchedRulesCursors(Gc::Ref<Dom::Element> el) {
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

} // namespace Vaev::Style
