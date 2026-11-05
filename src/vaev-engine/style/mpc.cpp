export module Vaev.Engine:style.mpc;

import Karm.Core;
import Karm.Gc;

import :dom.element;
import :style.computed;
import :style.rules;

using namespace Karm;

namespace Vaev::Style {

// FIXME: Investigate if an LRU would be better.
struct MatchedPropertyCache {
    struct Key {
        struct NonOwned {
            ComputedValues const& parent;
            Slice<MatchingRule> matchingRules;

            void hash(Meta::Derive<Hasher> auto& h) const {
                Karm::hash(h, reinterpret_cast<usize>(&parent));

                for (auto const& m : matchingRules) {
                    Karm::hash(h, reinterpret_cast<usize>(&m.rule));
                    Karm::hash(h, m.specificity.a);
                    Karm::hash(h, m.specificity.b);
                    Karm::hash(h, m.specificity.c);
                }
            }
        };

        ComputedValues const* parent;
        Vec<Pair<StyleRule const*, Specificity>> matchingRules = {};

        static Key fromNonOwned(NonOwned key) {
            Vec<Pair<StyleRule const*, Specificity>> tempMatching(key.matchingRules.len());
            for (auto const& m : key.matchingRules)
                tempMatching.pushBack({&m.rule, m.specificity});

            return Key{
                .parent = &key.parent,
                .matchingRules = std::move(tempMatching),
            };
        }

        bool operator==(Key const& other) const = default;

        bool operator==(NonOwned other) const {
            if (parent != &other.parent or matchingRules.len() != other.matchingRules.len())
                return false;

            for (usize i = 0; i < matchingRules.len(); i++) {
                auto const [rule, specificity] = matchingRules[i];
                auto const& otherMatch = other.matchingRules[i];
                if (rule != &otherMatch.rule or specificity != otherMatch.specificity)
                    return false;
            }

            return true;
        }

        void hash(Meta::Derive<Hasher> auto& h) const {
            Karm::hash(h, reinterpret_cast<usize>(parent));

            for (auto const& [rule, spec] : matchingRules) {
                Karm::hash(h, reinterpret_cast<usize>(rule));
                Karm::hash(h, spec.a);
                Karm::hash(h, spec.b);
                Karm::hash(h, spec.c);
            }
        }
    };

    Map<Key, Cow<ComputedValues>> _entries = {};
    usize _lookups = 0;
    usize _hits = 0;
    usize _ineligible = 0;

    bool eligible(Gc::Ref<Dom::Element> el, Opt<Symbol> pseudoElement) {
        bool eligible = not pseudoElement and
                        el->namespaceUri() == Html::NAMESPACE and
                        not el->containsStylingAttribute();
        if (not eligible)
            _ineligible++;
        return eligible;
    }

    Opt<Cow<ComputedValues>> lookup(ComputedValues const& parent, Slice<MatchingRule> matchingRules) {
        _lookups++;
        auto cached = _entries.lookup(Key::NonOwned{parent, matchingRules});
        if (cached)
            _hits++;
        return cached;
    }

    void put(ComputedValues const& parent, Slice<MatchingRule> matchingRules, Cow<ComputedValues> values) {
        _entries.put(
            Key::fromNonOwned({
                .parent = parent,
                .matchingRules = matchingRules,
            }),
            std::move(values)
        );
    }

    void repr(Io::Emit& e) const {
        e("(mpc hits:{}/{} entries:{} ineligible:{})", _hits, _lookups, _entries.len(), _ineligible);
    }
};

} // namespace Vaev::Style
