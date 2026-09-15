export module Vaev.Engine:style.ruleIndex;

import Karm.Core;

import :style.ancestorFilter;
import :style.matcher;
import :style.rules;

using namespace Karm;

namespace Vaev::Style {

struct RuleIndex {
    struct Entry {
        StyleRule const& originatingRule;
        Selector const& selector;
        urange ancestorHashes;
        usize order;
        Opt<Symbol> pseudoElement;
    };

    Map<String, Vec<Entry>> _idRules;
    Map<String, Vec<Entry>> _classRules;
    Map<Symbol, Vec<Entry>> _typeRules;
    Map<Symbol, Vec<Entry>> _attrRules;

    Vec<Entry> _complexRules;

    Vec<u16> _allAncestorHashes;

    usize _ruleCounter = 0;

    enum class DestinationBucket {
        ATTR,
        TYPE,
        CLASS,
        ID,
    };

    struct Candidate {
        DestinationBucket destination;
        Str key;
    };

    void _insert(Opt<Candidate> candidate, StyleRule const& rule, Selector const& selector, usize order) {
        auto ancestorHashes = _indexAncestorHashes(selector);
        auto pseudoElement = selectorPseudoElement(selector);

        if (not candidate) {
            _complexRules.emplaceBack(rule, selector, ancestorHashes, order, pseudoElement);
        } else if (candidate->destination == DestinationBucket::ID) {
            _idRules.lookupOrPutDefault(candidate->key).emplaceBack(rule, selector, ancestorHashes, order, pseudoElement);
        } else if (candidate->destination == DestinationBucket::CLASS) {
            _classRules.lookupOrPutDefault(candidate->key).emplaceBack(rule, selector, ancestorHashes, order, pseudoElement);
        } else if (candidate->destination == DestinationBucket::TYPE) {
            _typeRules.lookupOrPutDefault(Symbol::from(candidate->key)).emplaceBack(rule, selector, ancestorHashes, order, pseudoElement);
        } else if (candidate->destination == DestinationBucket::ATTR) {
            _attrRules.lookupOrPutDefault(Symbol::from(candidate->key)).emplaceBack(rule, selector, ancestorHashes, order, pseudoElement);
        } else {
            unreachable();
        }
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
                if (auto [name] = s.qualifiedName.exactName()) {
                    if (not oneOf(name, Html::ID_ATTR.name, Html::CLASS_ATTR.name, Html::STYLE_ATTR.name))
                        _allAncestorHashes.pushBack(AncestorFilter::hashEntry(AncestorFilter::ATTR, name.str()));
                }
            },
            [&](auto const&) {
            }
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

    urange _indexAncestorHashes(Selector const& selector) {
        usize before = _allAncestorHashes.len();
        if (auto infix = selector.is<Infix>())
            if (infix->type == Infix::DESCENDANT or infix->type == Infix::CHILD)
                _collectAncestorHashes(*infix->lhs);

        return {before, _allAncestorHashes.len() - before};
    }

    void add(StyleRule const& rule) {
        auto order = _ruleCounter++;

        if (auto s = rule.selector.is<Nfix>(); s and s->type == Nfix::OR) {
            for (auto const& inner : s->inners) {
                _insert(_addInner(inner), rule, inner, order);
            }
        } else {
            _insert(_addInner(rule.selector), rule, rule.selector, order);
        }
    }

    Opt<Candidate> _addInner(Selector const& selector) {
        return selector.visit(
            [&](TypeSelector const& s) -> Opt<Candidate> {
                auto const& qualifiedNameSelector = s.qualifiedName;

                if (auto [name] = qualifiedNameSelector.exactName()) {
                    return Some(Candidate{DestinationBucket::TYPE, name.str()});
                }
                return NONE;
            },
            [&](IdSelector const& s) -> Opt<Candidate> {
                return Some(Candidate{DestinationBucket::ID, s.id.str()});
            },
            [&](ClassSelector const& s) -> Opt<Candidate> {
                return Some(Candidate{DestinationBucket::CLASS, s.class_.str()});
            },
            [&](AttributeSelector const& s) -> Opt<Candidate> {
                if (auto [name] = s.qualifiedName.exactName()) {
                    return Some(Candidate{DestinationBucket::ATTR, name.str()});
                }
                return NONE;
            },
            [&](Infix const& s) -> Opt<Candidate> {
                return _addInner(*s.rhs);
            },
            [&](Nfix const& s) -> Opt<Candidate> {
                if (s.type == Nfix::AND) {
                    Opt<Candidate> max = NONE;

                    for (auto const& inner : s.inners) {
                        auto current = _addInner(inner);

                        if (max) {
                            if (current and current->destination >= max->destination) {
                                max = current;
                            }
                        } else {
                            max = current;
                        }
                    }

                    return max;
                }

                return NONE;
            },
            [&](auto const&) -> Opt<Candidate> {
                return NONE;
            }
        );
    }

    Vec<MatchingRule> match(Gc::Ref<Dom::Element> el, Opt<Symbol> pseudoElement, AncestorFilter const& ancestorFilter) {
        Vec<MatchingRule> matching;

        auto _insertIfBucketHit = [&](auto const& bucket, auto const& key) {
            if (auto [entries] = bucket.lookup(key)) {
                for (auto const& entry : entries) {
                    if (entry.pseudoElement != pseudoElement)
                        continue;

                    if (ancestorFilter.rejects(sub(_allAncestorHashes, entry.ancestorHashes)))
                        continue;

                    if (auto [specificity] = matchPreprocessedSelector(entry.selector, el, pseudoElement)) {
                        matching.pushBack({entry.originatingRule, specificity, entry.order});
                    }
                }
            }
        };

        for (auto const& class_ : el->classList._tokens) {
            _insertIfBucketHit(_classRules, class_);
        }

        if (auto id = el->id()) {
            _insertIfBucketHit(_idRules, *id);
        }

        _insertIfBucketHit(_typeRules, el->qualifiedName.name);

        for (auto const& attr : el->attributes) {
            _insertIfBucketHit(_attrRules, attr.qualifiedName.name);
        }

        for (auto const& entry : _complexRules) {
            if (entry.pseudoElement != pseudoElement)
                continue;

            if (ancestorFilter.rejects(sub(_allAncestorHashes, entry.ancestorHashes)))
                continue;

            if (auto [specificity] = matchPreprocessedSelector(entry.selector, el, pseudoElement)) {
                matching.pushBack({entry.originatingRule, specificity, entry.order});
            }
        }

        return matching;
    }
};

} // namespace Vaev::Style
