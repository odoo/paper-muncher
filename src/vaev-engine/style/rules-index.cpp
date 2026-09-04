export module Vaev.Engine:style.ruleIndex;

import Karm.Core;

import :style.rules;

using namespace Karm;

namespace Vaev::Style {

// Identifiers live in one hash space, so a class and a tag of the same name would
// otherwise collide. A collision only costs a false positive, but they are cheap to
// avoid.
enum struct IdentKind : u8 {
    ID,
    CLASS,
    TYPE,
};

static u64 _hashIdent(Str name, IdentKind kind) {
    // FNV-1a, with the kind mixed in first. Zero is reserved as the terminator of a
    // rule's hash list, so it is never returned.
    u64 hash = 0xcbf29ce484222325uLL;
    hash = (hash ^ static_cast<u64>(kind)) * 0x100000001b3uLL;
    for (usize i = 0; i < name.len(); ++i)
        hash = (hash ^ static_cast<u64>(static_cast<u8>(name[i]))) * 0x100000001b3uLL;
    return hash ? hash : 1;
}

// Tracks the identifiers — ids, tag names, classes — of the ancestors of the element
// currently being styled.
//
// A selector like `.table-striped > tbody > tr > td` can only match inside a
// `.table-striped`. Matching runs right-to-left, so without this the whole ancestor
// walk runs before that is discovered. Asking the filter first rejects the rule on
// two array reads.
//
// The filter answers "definitely not an ancestor" exactly and "possibly an ancestor"
// approximately, which is the safe direction: a false positive costs a full match
// that would have happened anyway, and false negatives cannot occur.
struct SelectorFilter {
    CountingBloom<4096> _bloom;
    Vec<Vec<u64>> _stack;

    void push(Gc::Ref<Dom::Element> element) {
        Vec<u64> hashes;

        if (auto id = element->id())
            hashes.pushBack(_hashIdent(*id, IdentKind::ID));

        hashes.pushBack(_hashIdent(element->qualifiedName.name.str(), IdentKind::TYPE));

        for (auto const& class_ : element->classList._tokens)
            hashes.pushBack(_hashIdent(class_.str(), IdentKind::CLASS));

        for (auto hash : hashes)
            _bloom.add(hash);

        _stack.pushBack(std::move(hashes));
    }

    void pop() {
        for (auto hash : last(_stack))
            _bloom.remove(hash);
        _stack.popBack();
    }

    bool maybeHasAncestor(u64 hash) const {
        return _bloom.maybeContains(hash);
    }
};

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

    void _add(Cursor<StyleRule> rule, usize ruleId, Selector const& selector) {
        selector.visit(
            [&](TypeSelector const& s) {
                auto const& qualifiedNameSelector = s.qualifiedName;

                if (not isLookupable(TypeSelector{qualifiedNameSelector})) {
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
                if (isLookupable(*s.rhs) or s.rhs->is<Nfix>()) {
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
                        if (isLookupable(inner)) {
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
                        if (isLookupable(inner)) {
                            _add(rule, ruleId, inner);
                        } else if (auto key = _lookupKeyFor(inner)) {
                            // A compound branch such as `a.text-danger:hover`. Index it
                            // under one necessary key rather than sending the whole rule
                            // to the every-element bucket because one branch happens not
                            // to be a bare class or type. Comma-separated groups like
                            // this are most of a framework stylesheet.
                            _add(rule, ruleId, *key);
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

    // Identifiers a rule requires of the subject's ancestors, indexed by rule id and
    // terminated by a zero. Empty for rules that require nothing, which are then
    // never filtered.
    static constexpr usize MAX_ANCESTOR_HASHES = 4;
    Vec<Array<u64, MAX_ANCESTOR_HASHES>> _ruleAncestorHashes;

    static void _harvestSimple(Selector const& selector, Array<u64, MAX_ANCESTOR_HASHES>& out, usize& n) {
        if (n == MAX_ANCESTOR_HASHES)
            return;

        if (auto s = selector.is<IdSelector>())
            out[n++] = _hashIdent(s->id.str(), IdentKind::ID);
        else if (auto s = selector.is<ClassSelector>())
            out[n++] = _hashIdent(s->class_, IdentKind::CLASS);
        else if (auto s = selector.is<TypeSelector>()) {
            if (auto name = s->qualifiedName.exactName())
                out[n++] = _hashIdent(name->str(), IdentKind::TYPE);
        }
    }

    // A compound selector: everything in it is required of the same element.
    static void _harvestCompound(Selector const& selector, Array<u64, MAX_ANCESTOR_HASHES>& out, usize& n) {
        if (auto nfix = selector.is<Nfix>()) {
            // Only AND requires all of its parts; nothing inside :not()/:is()/:where()
            // has to be present.
            if (nfix->type != Nfix::AND)
                return;

            for (auto const& inner : nfix->inners)
                _harvestSimple(inner, out, n);
            return;
        }

        _harvestSimple(selector, out, n);
    }

    // Climb the chain of elements above the subject, harvesting what each must be.
    //
    // Only descendant and child combinators put an element on the ancestor chain. At a
    // sibling combinator we stop: its left side is a sibling of an ancestor, not an
    // ancestor, and anything further up would be reached through it.
    static void _harvestAncestors(Selector const& selector, Array<u64, MAX_ANCESTOR_HASHES>& out, usize& n) {
        if (auto infix = selector.is<Infix>()) {
            if (infix->type != Infix::DESCENDANT and infix->type != Infix::CHILD)
                return;

            _harvestCompound(*infix->rhs, out, n);
            _harvestAncestors(*infix->lhs, out, n);
            return;
        }

        _harvestCompound(selector, out, n);
    }

    static Array<u64, MAX_ANCESTOR_HASHES> _ancestorHashesFor(Selector const& selector) {
        Array<u64, MAX_ANCESTOR_HASHES> out = {};
        usize n = 0;

        // A comma group matches if any branch does, so an identifier is only required
        // when every branch requires it. Not worth the bookkeeping — take no hashes.
        if (auto infix = selector.is<Infix>())
            if (infix->type == Infix::DESCENDANT or infix->type == Infix::CHILD)
                _harvestAncestors(*infix->lhs, out, n);

        return out;
    }

    // Facts about a rule's selector that matching needs but that do not depend on the
    // element being matched. Computing them per element meant walking the selector
    // tree again for every candidate rule on every element.
    struct RuleFacts {
        Specificity spec = {0, 0, 0};

        // isLookupEquivalentToMatch of the whole selector.
        bool lookupEquivalent = false;

        // Whether every inner of the selector — when it is an Nfix — is itself
        // lookup-equivalent. Both the AND and the OR shortcut need this before they
        // may accept a rule on lookup hits alone.
        bool allInnersLookupEquivalent = false;
    };

    Vec<RuleFacts> _ruleFacts;

    static RuleFacts _factsFor(Selector const& selector) {
        RuleFacts facts;

        facts.spec = spec(selector);
        facts.lookupEquivalent = isLookupEquivalentToMatch(selector);

        if (auto nfix = selector.is<Nfix>()) {
            facts.allInnersLookupEquivalent = true;
            for (auto const& inner : nfix->inners) {
                if (not isLookupEquivalentToMatch(inner)) {
                    facts.allInnersLookupEquivalent = false;
                    break;
                }
            }
        }

        return facts;
    }

    void add(StyleRule const& rule) {
        _ruleCount++;
        // Rule ids start at 1; slot 0 is unused.
        _ruleAncestorHashes.resize(_ruleCount + 1);
        _ruleAncestorHashes[_ruleCount] = _ancestorHashesFor(rule.selector);
        _ruleFacts.resize(_ruleCount + 1);
        _ruleFacts[_ruleCount] = _factsFor(rule.selector);
        _add(&rule, _ruleCount, rule.selector);
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

    // Whether a selector can be *found* through a lookup table, which is a weaker
    // property than the one above: a table hit narrows the candidates down, it does
    // not necessarily prove the selector matches.
    //
    // Type selectors are keyed by name alone, so a namespaced one — every type
    // selector in a sheet that declares `@namespace`, which is all of the user agent
    // sheets — is perfectly indexable but still has to be verified afterwards.
    // Treating those two properties as one left `_typeNameRules` completely empty
    // and pushed every type rule into `_nonLookupRules`, where it was tested against
    // every element in the document.
    static bool isLookupable(TypeSelector const& selector) {
        return selector.qualifiedName.exactName() != NONE;
    }

    static bool isLookupable(Selector const& selector) {
        if (auto s = selector.is<TypeSelector>())
            return isLookupable(*s);

        return isLookupEquivalentToMatch(selector);
    }

    // How selective a simple selector is as a lookup key; higher is better.
    static usize _lookupKeyRank(Selector const& selector) {
        if (selector.is<IdSelector>())
            return 3;
        if (selector.is<ClassSelector>())
            return 2;
        return 1;
    }

    // Find one lookup key for a selector that is not itself a simple lookupable
    // selector — a compound like `a.text-danger:hover`, or a complex one like
    // `.input-group .form-control`.
    //
    // The key only has to be a *necessary* condition: the selector requires all of
    // its parts, so anything matching it necessarily carries the key. Indexing on
    // the key can therefore never lose a match, and the full evaluation still
    // decides. That is what makes it safe to index a branch we cannot prove from
    // the table alone.
    static Selector const* _lookupKeyFor(Selector const& selector) {
        // In a complex selector the subject is the right-hand side.
        if (auto infix = selector.is<Infix>())
            return _lookupKeyFor(*infix->rhs);

        if (auto nfix = selector.is<Nfix>()) {
            // Only AND requires all of its parts. A nested OR/:not()/:where()
            // gives us no single necessary key.
            if (nfix->type != Nfix::AND)
                return nullptr;

            Selector const* best = nullptr;
            for (auto const& inner : nfix->inners) {
                if (not isLookupable(inner))
                    continue;
                if (not best or _lookupKeyRank(inner) > _lookupKeyRank(*best))
                    best = &inner;
            }
            return best;
        }

        return isLookupable(selector) ? &selector : nullptr;
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

        for (auto const& [name, value] : element->attributes.iterItems()) {
            auto const& attrName = name.name;
            auto key = Tuple{attrName, value->value.str()};

            considerCursorIfPresent(_attrPresentRules, attrName);
            considerCursorIfPresent(_attrExactValueRules, key);
        }

        if (_nonLookupRules.len())
            _cursors.pushBack({_nonLookupRules.buf(), _nonLookupRules.len()});
    }

    MatchingRules _matchingRules;

    // Reject a rule whose ancestor requirements the current element cannot meet,
    // before paying for the match itself.
    //
    // Only rules that get this far need checking: the shortcuts that accept a rule
    // straight from a lookup hit apply to selectors made purely of simple selectors,
    // which have no combinators and so require nothing of any ancestor.
    bool _filterRejects(usize ruleId) const {
        if (not _filter)
            return false;

        for (auto hash : _ruleAncestorHashes[ruleId]) {
            if (hash == 0)
                break;
            if (not _filter->maybeHasAncestor(hash))
                return true;
        }

        return false;
    }

    void _evalStyleRule(StyleRule const& rule, usize ruleId, Gc::Ref<Dom::Element> el, Opt<Symbol> pseudoElement) {
        if (_filterRejects(ruleId))
            return;

        if (auto specificity = rule.match(el, pseudoElement))
            _matchingRules.pushBack({&rule, specificity.unwrap()});
    }

    bool _maybeDeferRuleEvaluation(Entry const& entry, usize countMatchesWithCurrentRule) {
        auto const [ruleId, styleRule] = entry;
        auto const& facts = _ruleFacts[ruleId];

        if (facts.lookupEquivalent) {
            _matchingRules.pushBack({styleRule, facts.spec});
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

        // Every inner was reached through a lookup table, but a table hit only
        // proves a match for keys that carry the entire selector. A namespaced type
        // selector is keyed by name alone, so the namespace is still unverified.
        if (not facts.allInnersLookupEquivalent)
            return false;

        _matchingRules.pushBack({styleRule, facts.spec});
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
                    _evalStyleRule(*lastStyleRule, lastRuleId, el, pseudoElement);
                } else {
                    // NOTE: If an element has 2 or more occourence of this rule in its list, we can assume
                    // the rule as matched, since at least one of the occourences is due to a lookupable selector,
                    // which is guaranteed to match.
                    //
                    // That only holds while every branch that could have produced those
                    // occurrences proves a match on its own. Two namespaced type selectors
                    // sharing a name land in the same bucket and can both be hit by an
                    // element in neither namespace, so those go back to a full evaluation.
                    auto const& facts = _ruleFacts[lastRuleId];

                    if (not facts.allInnersLookupEquivalent) {
                        _evalStyleRule(*lastStyleRule, lastRuleId, el, pseudoElement);
                        return;
                    }

                    _matchingRules.pushBack({lastStyleRule, facts.spec});
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
                _evalStyleRule(*_cursors[bestCursorIdx]->rule, _cursors[bestCursorIdx]->order, el, pseudoElement);

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

    SelectorFilter const* _filter = nullptr;

    MatchingRules match(Gc::Ref<Dom::Element> el, Opt<Symbol> pseudoElement, SelectorFilter const* filter = nullptr) {
        _filter = filter;
        _cursors.clear();
        _matchingRules.clear();

        _collectMatchedRulesCursors(el, pseudoElement);
        _mergeMatchedRules(el, pseudoElement);

        return _matchingRules;
    }
};

} // namespace Vaev::Style
