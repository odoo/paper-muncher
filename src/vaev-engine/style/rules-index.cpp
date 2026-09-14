export module Vaev.Engine:style.ruleIndex;

import Karm.Core;

import :style.rules;

using namespace Karm;

namespace Vaev::Style {

struct RuleIndex {
    struct Entry {
        StyleRule const& originatingRule;
        Selector const& selector;
    };

    Map<String, Vec<Entry>> _idRules;
    Map<String, Vec<Entry>> _classRules;
    Map<Symbol, Vec<Entry>> _typeRules;
    Map<Symbol, Vec<Entry>> _attrRules;

    Vec<Entry> _complexRules;

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

    void _insert(Opt<Candidate> candidate, StyleRule const& rule, Selector const& selector) {

        if (not candidate) {
            _complexRules.emplaceBack(rule, selector);
        } else if (candidate->destination == DestinationBucket::ID) {
            _idRules.lookupOrPutDefault(candidate->key).emplaceBack(rule, selector);
        } else if (candidate->destination == DestinationBucket::CLASS) {
            _classRules.lookupOrPutDefault(candidate->key).emplaceBack(rule, selector);
        } else if (candidate->destination == DestinationBucket::TYPE) {
            _typeRules.lookupOrPutDefault(Symbol::from(candidate->key)).emplaceBack(rule, selector);
        } else if (candidate->destination == DestinationBucket::ATTR) {
            _attrRules.lookupOrPutDefault(Symbol::from(candidate->key)).emplaceBack(rule, selector);
        } else {
            unreachable();
        }
    }

    void add(StyleRule const& rule) {
        if (auto s = rule.selector.is<Nfix>(); s and s->type == Nfix::OR) {
            for (auto const& inner : s->inners) {
                _insert(_addInner(inner), rule, inner);
            }
        } else {
            _insert(_addInner(rule.selector), rule, rule.selector);
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

    MatchingRules match(Gc::Ref<Dom::Element> el, Opt<Symbol> pseudoElement) {
        MatchingRules matching;

        auto _insertIfBucketHit = [&](auto const& bucket, auto const& key) {
            if (auto [entries] = bucket.lookup(key)) {
                for (auto const& entry : entries) {
                    if (auto [specificity] = matchSelector(entry.selector, el, pseudoElement)) {
                        matching.pushBack({entry.originatingRule, specificity});
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
            if (auto [specificity] = matchSelector(entry.selector, el, pseudoElement)) {
                matching.pushBack({entry.originatingRule, specificity});
            }
        }

        return matching;
    }
};

} // namespace Vaev::Style
