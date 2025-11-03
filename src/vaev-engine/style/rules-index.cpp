export module Vaev.Engine:style.ruleIndex;

import Karm.Core;

import :style.rules;

using namespace Karm;

namespace Vaev::Style {

// Used to speed up the lookup of style rules by using lookup tables.
// This is useful for rules described by:
// - Simple selectors other than class selectors
// - OR or AND infixes that contain lookupable selectors
// - Complex selectors where the right-hand side is a lookupable selector
// Currently, only a subset of lookupable simple selectors are implemented:
//  - Attr selectors with non-qualified namespaces could also be lookupable selectors.
//  - All type selectors could be lookupable selectors.
//  - :is(), :where() could be lookuable selectors.
struct RuleIndex {
    struct Entry {
        usize order;
        Cursor<StyleRule> rule;
    };

    usize _ruleCount = 0;

    Map<Str, Vec<Entry>> _idRules;
    Map<Str, Vec<Entry>> _classRules;
    Map<Symbol, Vec<Entry>> _typeNameRules;
    Map<Symbol, Vec<Entry>> _pseudoRules;
    Map<Symbol, Vec<Entry>> _attrPresentRules;
    Map<Tuple<Symbol, Str>, Vec<Entry>> _attrExactValueRules;

    Vec<Entry> _nonLookupRules;

    Map<usize, usize> _ruleIdToNeededCount;

    void _add(Cursor<StyleRule> rule, usize ruleId, Selector const& selector) {
        selector.visit(Visitor{
            [&](TypeSelector const& s) {
                auto const& qualifiedNameSelector = s.qualifiedName;

                if (not isLookupEquivalentToMatch(qualifiedNameSelector)) {
                    _nonLookupRules.pushBack({ruleId, rule});
                    return;
                }

                _typeNameRules.getOrDefault(qualifiedNameSelector.name.unwrap()).pushBack({ruleId, rule});
            },
            [&](PseudoElementSelector const& s) {
                _pseudoRules.getOrDefault(s.type).pushBack({ruleId, rule});
            },
            [&](IdSelector const& s) {
                _idRules.getOrDefault(s.id.str()).pushBack({ruleId, rule});
            },
            [&](ClassSelector const& s) {
                _classRules.getOrDefault(s.class_).pushBack({ruleId, rule});
            },
            [&](AttributeSelector const& s) {
                if (not isLookupEquivalentToMatch(s)) {
                    _nonLookupRules.pushBack({ruleId, rule});
                    return;
                }

                auto const& name = *s.qualifiedName.name;

                if (s.match == AttributeSelector::Match::PRESENT) {
                    _attrPresentRules.getOrDefault(name).pushBack({ruleId, rule});
                } else if (s.match == AttributeSelector::Match::EXACT) {
                    _attrExactValueRules.getOrDefault({name, s.value}).pushBack({ruleId, rule});
                }
            },
            [&](Infix const& s) {
                if (isLookupEquivalentToMatch(s.rhs) or s.rhs->is<Nfix>()) {
                    _add(rule, ruleId, s.rhs);
                } else {
                    _nonLookupRules.pushBack({ruleId, rule});
                }
            },
            [&](Nfix const& s) {
                if (s.type == Nfix::AND) {
                    // NOTE: We could remove the lookupable selectors from the nfix since they are
                    // already handled by the lookup phase. However, computing specificy should be done
                    // before removing said selectors.
                    usize conditionsCount = 0;
                    for (auto const& inner : s.inners) {
                        if (isLookupEquivalentToMatch(inner)) {
                            conditionsCount++;
                            _add(rule, ruleId, inner);
                        }
                    }

                    if (conditionsCount == 0) {
                        _nonLookupRules.pushBack({ruleId, rule});
                    } else {
                        _ruleIdToNeededCount.put(ruleId, conditionsCount);
                    }
                } else if (s.type == Nfix::OR) {
                    bool hasNonLookupable = false;
                    for (auto const& inner : s.inners) {
                        if (isLookupEquivalentToMatch(inner)) {
                            _add(rule, ruleId, inner);
                        } else {
                            hasNonLookupable = true;
                        }
                    }
                    if (hasNonLookupable)
                        _nonLookupRules.pushBack({ruleId, rule});
                } else {
                    _nonLookupRules.pushBack({ruleId, rule});
                }
            },
            [&](auto const&) {
                _nonLookupRules.pushBack({ruleId, rule});
            },
        });
    }

    void add(StyleRule const& rule) {
        _ruleCount++;
        _add(&rule, _ruleCount, rule.selector);
    }

    static bool isLookupEquivalentToMatch(AttributeSelector const& selector) {
        if (selector.match != AttributeSelector::Match::PRESENT and
            selector.match != AttributeSelector::Match::EXACT)
            return false;

        return selector.qualifiedName.ns != NONE;
    }

    static bool isLookupEquivalentToMatch(TypeSelector const& selector) {
        return selector.qualifiedName.ns == NONE &&
               selector.qualifiedName.name != NONE;
    }

    static bool isLookupEquivalentToMatch(Selector const& selector) {
        if (auto s = selector.is<AttributeSelector>())
            return isLookupEquivalentToMatch(*s);

        if (auto s = selector.is<TypeSelector>())
            return isLookupEquivalentToMatch(*s);

        return selector.is<PseudoElementSelector>() or
               selector.is<IdSelector>() or
               selector.is<ClassSelector>();
    }

    Vec<Cursor<Entry>> _cursors;

    void _collectMatchedRulesCursors(Gc::Ref<Dom::Element> element, Opt<Symbol> pseudoElement) {
        auto considerCursorIfPresent = [&](auto& lookup, auto const& key) {
            auto rules = lookup.access(key);
            if (rules)
                _cursors.pushBack({rules->buf(), rules->len()});
        };

        for (auto const& class_ : element->classList._tokens) {
            considerCursorIfPresent(_classRules, class_.str());
        }

        if (auto id = element->id()) {
            considerCursorIfPresent(_idRules, *id);
        }

        if (pseudoElement)
            considerCursorIfPresent(_pseudoRules, pseudoElement.unwrap());

        considerCursorIfPresent(_typeNameRules, element->qualifiedName.name);

        for (auto const& [name, value] : element->attributes.iterUnordered()) {
            auto const& attrName = name.name;
            auto key = Tuple<Symbol, Str>{attrName, value->value.str()};

            considerCursorIfPresent(_attrPresentRules, attrName);
            considerCursorIfPresent(_attrExactValueRules, key);
        }

        if (_nonLookupRules.len())
            _cursors.pushBack({_nonLookupRules.buf(), _nonLookupRules.len()});
    }

    MatchingRules _matchingRules;

    void _evalStyleRule(StyleRule const& rule, Gc::Ref<Dom::Element> el, Opt<Symbol> pseudoElement) {
        if (auto specificity = rule.match(el, pseudoElement))
            _matchingRules.pushBack({&rule, specificity.unwrap()});
    }

    bool _maybeDeferRuleEvaluation(Entry const& entry, usize countMatchesWithCurrentRule) {
        auto const [ruleId, styleRule] = entry;

        if (isLookupEquivalentToMatch(styleRule->selector)) {
            _matchingRules.pushBack({styleRule, spec(styleRule->selector)});
            return true;
        }

        auto nfix = styleRule->selector.is<Nfix>();

        if (not nfix or (nfix->type != Nfix::AND and nfix->type != Nfix::OR))
            return false;

        if (nfix->type == Nfix::OR) {
            // Deferring the evaluation to after we know how many times this rule was matched.
            return true;
        }

        auto neededCount = _ruleIdToNeededCount.tryGet(ruleId);

        if (not neededCount) {
            // This selector doesn't have a needed count, meaning that is has no lookupable selectors.
            return false;
        }

        if (countMatchesWithCurrentRule != *neededCount) {
            // We still expect more internal lookupable selectors to be matched for this AND Nfix
            return true;
        }

        if (nfix->inners.len() != countMatchesWithCurrentRule) {
            // We matched all lookupable selectors as a "pre-condition" to evaluate the rule,
            // but we need now to evaluate the whole rule since it has non-lookupable selectors.
            return false;
        }

        _matchingRules.pushBack({styleRule, spec(styleRule->selector)});
        return true;
    }

    void _mergeMatchedRules(Gc::Ref<Dom::Element> el, Opt<Symbol> pseudoElement) {
        usize countMatchesWithCurrentRule = 0;
        usize lastRuleId = 0;
        Cursor<StyleRule> lastStyleRule = nullptr;

        auto maybeFinalizeNfixOrRule = [&]() {
            if (not lastStyleRule)
                return;

            if (auto nfix = lastStyleRule->selector.is<Nfix>()) {
                if (nfix->type != Nfix::OR)
                    return;

                if (countMatchesWithCurrentRule == 1) {
                    _evalStyleRule(*lastStyleRule, el, pseudoElement);
                } else {
                    // NOTE: If an element has 2 or more occourence of this rule in its list, we can assume
                    // the rule as matched, since at least one of the occourences is due to a lookupable selector,
                    // which is guaranteed to match.
                    _matchingRules.pushBack({lastStyleRule, spec(lastStyleRule->selector)});
                }
            }
        };

        while (_cursors.len() > 0) {
            usize bestCursorIdx = 0;
            for (usize i = 1; i < _cursors.len(); i++) {
                if (_cursors[i]->order < _cursors[bestCursorIdx]->order) {
                    bestCursorIdx = i;
                }
            }

            // NOTE: This is quite hot code and doing this check every time is not ideal,
            // but it was the only way found to allow defering the evaluation of OR infixes until
            // we know how many times this rule was matched.
            if (lastRuleId != _cursors[bestCursorIdx]->order) {
                maybeFinalizeNfixOrRule();
                countMatchesWithCurrentRule = 1;
            } else {
                countMatchesWithCurrentRule++;
            }

            if (not _maybeDeferRuleEvaluation(*_cursors[bestCursorIdx], countMatchesWithCurrentRule))
                _evalStyleRule(*_cursors[bestCursorIdx]->rule, el, pseudoElement);

            lastStyleRule = _cursors[bestCursorIdx]->rule;
            lastRuleId = _cursors[bestCursorIdx]->order;

            _cursors[bestCursorIdx].next();
            if (_cursors[bestCursorIdx].ended()) {
                std::swap(_cursors[bestCursorIdx], last(_cursors));
                _cursors.popBack();
            }
        }

        maybeFinalizeNfixOrRule();
    }

    MatchingRules match(Gc::Ref<Dom::Element> el, Opt<Symbol> pseudoElement) {
        _cursors.clear();
        _matchingRules.clear();

        _collectMatchedRulesCursors(el, pseudoElement);
        _mergeMatchedRules(el, pseudoElement);

        return _matchingRules;
    }
};

} // namespace Vaev::Style
