export module Vaev.Engine:style.ruleIndex;

import Karm.Core;

import :style.ancestorFilter;
import :style.layers;
import :style.rules;
import :style.select;

using namespace Karm;

namespace Vaev::Style {

struct RuleIndex {
    struct Entry {
        StyleRule const& originatingRule;
        ComplexSelector const& selector;
        Specificity specificity;
        urange ancestorHashes;
        usize order;
        Layer layer;
        Opt<Symbol> pseudoElement;
    };

    Map<String, Vec<Entry>> _idRules;
    Map<String, Vec<Entry>> _classRules;
    Map<Symbol, Vec<Entry>> _typeRules;
    Map<Symbol, Vec<Entry>> _attrRules;

    Vec<Entry> _complexRules;
    Vec<u16> _allAncestorHashes;
    usize _ruleCount = 0;

    enum class Bucket {
        ATTR,
        TYPE,
        CLASS,
        ID,
    };

    // NOTE: Type and attribute names are matched against their ASCII lowercase form on HTML elements in HTML
    //       documents, so rules are indexed under both forms.
    struct BucketKey {
        Bucket bucket;
        Str value;
        Str asciiLowercaseValue = value;
    };

    static Opt<BucketKey> _bestBucket(Opt<BucketKey> lhs, Opt<BucketKey> rhs) {
        if (not lhs)
            return rhs;
        if (not rhs)
            return lhs;
        return rhs->bucket > lhs->bucket ? rhs : lhs;
    }

    static Opt<BucketKey> _bucketFor(SubclassSelector const& selector) {
        return selector.visit(
            [&](IdSelector const& s) -> Opt<BucketKey> {
                return Some(BucketKey{Bucket::ID, s.id.str()});
            },
            [&](ClassSelector const& s) -> Opt<BucketKey> {
                return Some(BucketKey{Bucket::CLASS, s.class_.str()});
            },
            [&](AttributeSelector const& s) -> Opt<BucketKey> {
                if (auto const& [name] = s.qualifiedName.exactName()) {
                    return Some(BucketKey{Bucket::ATTR, name.str(), s.qualifiedName.exactName(true)->str()});
                }
                return NONE;
            },
            [&](auto const&) -> Opt<BucketKey> {
                return NONE;
            }
        );
    }

    static Opt<BucketKey> _bucketFor(TypeSelector const& selector) {
        if (auto const& [name] = selector.qualifiedName.exactName()) {
            return Some(BucketKey{Bucket::TYPE, name.str(), selector.qualifiedName.exactName(true)->str()});
        }
        return NONE;
    }

    static Opt<BucketKey> _bucketFor(CompoundSelector const& selector) {
        Opt<BucketKey> best = NONE;

        for (auto& subclass : selector.subclasses)
            best = _bestBucket(best, _bucketFor(subclass));

        if (auto const& [typeSelector] = selector.type)
            best = _bestBucket(best, _bucketFor(typeSelector));

        return best;
    }

    static Opt<BucketKey> _bucketFor(ComplexSelectorUnit const& selectorUnit) {
        if (auto const& [compoundSelector] = selectorUnit.selector)
            return _bucketFor(compoundSelector);
        return NONE;
    }

    static Opt<BucketKey> _bucketFor(ComplexSelector const& selector) {
        return _bucketFor(selector.rhs);
    }

    void _collectSelectorHashes(SubclassSelector const& selector) {
        selector.visit(
            [&](IdSelector const& s) {
                _allAncestorHashes.pushBack(AncestorFilter::hashEntry(AncestorFilter::ID, s.id.str()));
            },
            [&](ClassSelector const& s) {
                _allAncestorHashes.pushBack(AncestorFilter::hashEntry(AncestorFilter::CLASS, s.class_));
            },
            [&](AttributeSelector const& s) {
                if (auto const& [name] = s.qualifiedName.exactName()) {
                    if (not oneOf(name, Html::ID_ATTR.name, Html::CLASS_ATTR.name, Html::STYLE_ATTR.name))
                        _allAncestorHashes.pushBack(AncestorFilter::hashEntry(AncestorFilter::ATTR, name.str()));
                }
            },
            [&](auto const&) {
            }
        );
    }

    void _collectSelectorHashes(TypeSelector const& selector) {
        if (auto const& [name] = selector.qualifiedName.exactName())
            _allAncestorHashes.pushBack(AncestorFilter::hashEntry(AncestorFilter::TYPE, name.str()));
    }

    void _collectSelectorHashes(CompoundSelector const& selector) {
        for (auto& s : selector.subclasses)
            _collectSelectorHashes(s);
        if (auto& [s] = selector.type)
            _collectSelectorHashes(s);
    }

    void _collectSelectorHashes(ComplexSelectorUnit const& selectorUnit) {
        if (auto& [s] = selectorUnit.selector)
            _collectSelectorHashes(s);
    }

    // NOTE: A unit of the left hand side is an ancestor of the subject if it is
    //       linked to its right neighbor by a descendant or child combinator.
    void _collectSelectorHashes(ComplexSelector const& selector) {
        for (auto& s : selector.lhs)
            if (s.combinator == SelectorCombinator::DESCENDANT or s.combinator == SelectorCombinator::CHILD)
                _collectSelectorHashes(static_cast<ComplexSelectorUnit const&>(s));
    }

    urange _indexAncestorHashes(ComplexSelector const& selector) {
        usize before = _allAncestorHashes.len();
        _collectSelectorHashes(selector);
        return urange::fromStartEnd(before, _allAncestorHashes.len());
    }

    void add(StyleRule const& rule, Layer layer) {
        auto order = _ruleCount++;
        for (auto& selector : rule.selectors) {
            auto key = _bucketFor(selector);
            Entry entry{
                rule,
                selector,
                specificity(selector),
                _indexAncestorHashes(selector),
                order,
                layer,
                selectorPseudoElement(selector),
            };

            if (not key) {
                _complexRules.pushBack(entry);
            } else if (key->bucket == Bucket::ID) {
                _idRules.lookupOrPutDefault(key->value).pushBack(entry);
            } else if (key->bucket == Bucket::CLASS) {
                _classRules.lookupOrPutDefault(key->value).pushBack(entry);
            } else if (key->bucket == Bucket::TYPE) {
                _typeRules.lookupOrPutDefault(Symbol::from(key->value)).pushBack(entry);
                if (key->asciiLowercaseValue != key->value)
                    _typeRules.lookupOrPutDefault(Symbol::from(key->asciiLowercaseValue)).pushBack(entry);
            } else if (key->bucket == Bucket::ATTR) {
                _attrRules.lookupOrPutDefault(Symbol::from(key->value)).pushBack(entry);
                if (key->asciiLowercaseValue != key->value)
                    _attrRules.lookupOrPutDefault(Symbol::from(key->asciiLowercaseValue)).pushBack(entry);
            } else {
                unreachable();
            }
        }
    }

    Vec<MatchingRule> match(MatchSubject const& subject, AncestorFilter const& ancestorFilter) {
        Vec<MatchingRule> matching;

        auto _insertIfMatch = [&](Entry const& entry) {
            if (entry.pseudoElement != subject.pseudoElement)
                return;

            if (ancestorFilter.rejects(sub(_allAncestorHashes, entry.ancestorHashes)))
                return;

            if (Style::match(entry.selector, subject))
                matching.pushBack({entry.originatingRule, entry.specificity, entry.order, entry.layer});
        };

        auto _insertIfBucketHit = [&](auto const& bucket, auto const& key) {
            if (auto [entries] = bucket.lookup(key))
                for (auto const& entry : entries)
                    _insertIfMatch(entry);
        };

        for (auto const& class_ : subject.element->classList._tokens)
            _insertIfBucketHit(_classRules, class_);

        if (auto id = subject.element->id())
            _insertIfBucketHit(_idRules, *id);

        _insertIfBucketHit(_typeRules, subject.element->qualifiedName.name);

        for (auto const& attr : subject.element->attributes)
            _insertIfBucketHit(_attrRules, attr.qualifiedName.name);

        for (auto const& entry : _complexRules)
            _insertIfMatch(entry);

        return matching;
    }
};

} // namespace Vaev::Style
