module;

#include <stdlib.h>

export module Vaev.Engine:style.ruleIndex;

import Karm.Core;
import Karm.Sys;

import :style.ancestorFilter;
import :style.rules;

using namespace Karm;

namespace Vaev::Style {

namespace _FilterStats {
struct State {
    usize evalCalls = 0;
    usize rejected = 0;
    usize nonEmptyRanges = 0;
    bool dumpRegistered = false;
};
inline State& _state() {
    static State s;
    return s;
}
inline void _dump() {
    auto& s = _state();
    Sys::errln("=== AncestorFilter stats ===");
    Sys::errln("_evalStyleRule calls:  {}", s.evalCalls);
    Sys::errln("rejected by filter:    {}", s.rejected);
    Sys::errln("non-empty ranges seen: {}", s.nonEmptyRanges);
    Sys::errln("============================");
}
inline void _record(bool hasRange, bool wasRejected) {
    auto& s = _state();
    if (not s.dumpRegistered) {
        s.dumpRegistered = true;
        atexit(_dump);
    }
    s.evalCalls++;
    if (hasRange)
        s.nonEmptyRanges++;
    if (wasRejected)
        s.rejected++;
}
} // namespace _FilterStats

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

    Map<Symbol, Vec<Entry>> _idRules;
    Map<String, Vec<Entry>> _classRules;
    Map<Symbol, Vec<Entry>> _typeNameRules;
    Map<Symbol, Vec<Entry>> _pseudoRules;
    Map<Symbol, Vec<Entry>> _attrPresentRules;
    Map<Tuple<Symbol, String>, Vec<Entry>> _attrExactValueRules;

    Vec<Entry> _nonLookupRules;

    Map<usize, usize> _ruleIdToNeededCount;

    Vec<u16> _allAncestorHashes;
    Vec<urange> _ruleAncestorHashes;

    void _add(Cursor<StyleRule> rule, usize ruleId, Selector const& selector) {
        selector.visit(
            [&](TypeSelector const& s) {
                auto const& qualifiedNameSelector = s.qualifiedName;

                if (not isLookupEquivalentToMatch(qualifiedNameSelector)) {
                    _nonLookupRules.pushBack({ruleId, rule});
                    return;
                }

                _typeNameRules.lookupOrPutDefault(qualifiedNameSelector.exactName().unwrap()).pushBack({ruleId, rule});
            },
            [&](PseudoElementSelector const& s) {
                _pseudoRules.lookupOrPutDefault(s.type).pushBack({ruleId, rule});
            },
            [&](IdSelector const& s) {
                _idRules.lookupOrPutDefault(s.id).pushBack({ruleId, rule});
            },
            [&](ClassSelector const& s) {
                _classRules.lookupOrPutDefault(s.class_).pushBack({ruleId, rule});
            },
            [&](AttributeSelector const& s) {
                if (not isLookupEquivalentToMatch(s)) {
                    _nonLookupRules.pushBack({ruleId, rule});
                    return;
                }

                auto name = s.qualifiedName.exactName().unwrap();

                if (s.match == AttributeSelector::Match::PRESENT) {
                    _attrPresentRules.lookupOrPutDefault(name).pushBack({ruleId, rule});
                } else if (s.match == AttributeSelector::Match::EXACT) {
                    _attrExactValueRules.lookupOrPutDefault(Tuple{name, s.value}).pushBack({ruleId, rule});
                }
            },
            [&](Infix const& s) {
                if (isLookupEquivalentToMatch(*s.rhs) or s.rhs->is<Nfix>()) {
                    _add(rule, ruleId, *s.rhs);
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
            }
        );
    }

    void _collectSimpleHashes(Selector const& selector) {
        selector.visit(
            [&](IdSelector const& s) {
                _allAncestorHashes.pushBack(AncestorFilter::hashEntry(AncestorFilter::ID, s.id.str()));
            },
            [&](ClassSelector const& s) {
                _allAncestorHashes.pushBack(AncestorFilter::hashEntry(AncestorFilter::CLASS, s.class_));
            },
            [&](TypeSelector const& s) {
                if (auto [name] = s.qualifiedName.exactName())
                    _allAncestorHashes.pushBack(AncestorFilter::hashEntry(AncestorFilter::TYPE, name.str()));
            },
            [&](AttributeSelector const& s) {
                if (auto [name] = s.qualifiedName.exactName())
                    _allAncestorHashes.pushBack(AncestorFilter::hashEntry(AncestorFilter::ATTR, name.str()));
            },
            [&](auto const&) {}
        );
    }

    void _collectCompoundHashes(Selector const& selector) {
        if (auto nfix = selector.is<Nfix>()) {
            if (nfix->type != Nfix::AND)
                return;

            for (auto const& inner : nfix->inners)
                _collectSimpleHashes(inner);

            return;
        }
        _collectSimpleHashes(selector);
    }

    void _collectAncestorHashes(Selector const& selector) {
        if (auto infix = selector.is<Infix>()) {
            if (infix->type != Infix::DESCENDANT and infix->type != Infix::CHILD)
                return;

            _collectCompoundHashes(*infix->rhs);
            _collectAncestorHashes(*infix->lhs);

            return;
        }
        _collectCompoundHashes(selector);
    }

    void _indexAncestorHashes(Selector const& selector) {
        usize before = _allAncestorHashes.len();
        if (auto infix = selector.is<Infix>())
            if (infix->type == Infix::DESCENDANT or infix->type == Infix::CHILD)
                _collectAncestorHashes(*infix->lhs);

        _ruleAncestorHashes.pushBack({before, _allAncestorHashes.len() - before});
    }

    void add(StyleRule const& rule) {
        _indexAncestorHashes(rule.selector);
        _add(&rule, _ruleCount, rule.selector);
        _ruleCount++;
    }

    static bool isLookupEquivalentToMatch(AttributeSelector const& selector) {
        if (selector.match != AttributeSelector::Match::PRESENT and
            selector.match != AttributeSelector::Match::EXACT)
            return false;

        return selector.qualifiedName.ns.is<Universal>() and
               selector.qualifiedName.exactName() != NONE;
    }

    static bool isLookupEquivalentToMatch(TypeSelector const& selector) {
        return selector.qualifiedName.ns.is<Universal>() and
               selector.qualifiedName.exactName() != NONE;
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
            auto rules = lookup.lookup(key);
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

        for (auto const& attr : element->attributes) {
            auto const& attrName = attr.qualifiedName.name;
            auto key = Tuple{attrName, attr.value.str()};

            considerCursorIfPresent(_attrPresentRules, attrName);
            considerCursorIfPresent(_attrExactValueRules, key);
        }

        if (_nonLookupRules.len())
            _cursors.pushBack({_nonLookupRules.buf(), _nonLookupRules.len()});
    }

    MatchingRules _matchingRules;

    void _evalStyleRule(StyleRule const& rule, usize ruleId, Gc::Ref<Dom::Element> el, Opt<Symbol> pseudoElement, AncestorFilter const& ancestorFilter) {
        auto ancestorHashes = sub(_allAncestorHashes, _ruleAncestorHashes[ruleId]);
        bool rejected = ancestorFilter.rejects(ancestorHashes);
        _FilterStats::_record(ancestorHashes.len() > 0, rejected);
        if (rejected)
            return;

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

        auto neededCount = _ruleIdToNeededCount.lookup(ruleId);

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

    void _mergeMatchedRules(Gc::Ref<Dom::Element> el, Opt<Symbol> pseudoElement, AncestorFilter const& ancestorFilter) {
        usize countMatchesWithCurrentRule = 0;
        Opt<usize> lastRuleId = NONE;
        Cursor<StyleRule> lastStyleRule = nullptr;

        auto maybeFinalizeNfixOrRule = [&]() {
            if (not lastStyleRule)
                return;

            if (auto nfix = lastStyleRule->selector.is<Nfix>()) {
                if (nfix->type != Nfix::OR)
                    return;

                if (countMatchesWithCurrentRule == 1) {
                    _evalStyleRule(*lastStyleRule, *lastRuleId, el, pseudoElement, ancestorFilter);
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
            usize order = _cursors[bestCursorIdx]->order;
            if (not lastRuleId or *lastRuleId != order) {
                maybeFinalizeNfixOrRule();
                countMatchesWithCurrentRule = 1;
            } else {
                countMatchesWithCurrentRule++;
            }

            if (not _maybeDeferRuleEvaluation(*_cursors[bestCursorIdx], countMatchesWithCurrentRule))
                _evalStyleRule(*_cursors[bestCursorIdx]->rule, order, el, pseudoElement, ancestorFilter);

            lastStyleRule = _cursors[bestCursorIdx]->rule;
            lastRuleId = Some(order);

            _cursors[bestCursorIdx].next();
            if (_cursors[bestCursorIdx].ended()) {
                std::swap(_cursors[bestCursorIdx], last(_cursors));
                _cursors.popBack();
            }
        }

        maybeFinalizeNfixOrRule();
    }

    MatchingRules match(Gc::Ref<Dom::Element> el, Opt<Symbol> pseudoElement, AncestorFilter const& ancestorFilter) {
        _cursors.clear();
        _matchingRules.clear();

        _collectMatchedRulesCursors(el, pseudoElement);
        _mergeMatchedRules(el, pseudoElement, ancestorFilter);

        return _matchingRules;
    }
};

} // namespace Vaev::Style
