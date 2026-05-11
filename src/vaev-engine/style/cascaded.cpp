export module Vaev.Engine:style.cascaded;

import Karm.Core;
import Karm.Gfx;
import Karm.Math;

import :css;
import :values;
import :props;
import :style.stylesheet;

using namespace Karm;

namespace Vaev::Style {

// https://www.w3.org/TR/css-cascade-4/#cascaded
export struct CascadedValues {
    struct Entry {
        Rc<Property> property;
        Css::Important important;
        Origin origin;
        Specificity specificity;
        usize declarationOrder;

        auto priority() const {
            return property->registration->resolutionOrder;
        }

        auto operator<=>(Entry const& other) const {
            if (priority() != other.priority())
                return priority() <=> other.priority();
            if (important != other.important)
                return important <=> other.important;
            if (origin != other.origin)
                return origin <=> other.origin;
            if (specificity != other.specificity)
                return specificity <=> other.specificity;
            return declarationOrder <=> other.declarationOrder;
        }
    };

    Map<Symbol, Entry> _entries;
    usize _declarationOrder = 0;

    void put(Rc<Property> property, Origin origin, Specificity specificity) {
        auto name = property->registration->name();
        Entry entry{property, property->important, origin, specificity, _declarationOrder++};
        auto& existing = _entries.lookupOrPutDefault(name, entry);
        if (existing.property != property and entry >= existing)
            existing = entry;
    }

    void clear() {
        _declarationOrder = 0;
        _entries.clear();
    }

    Vec<Entry> _resolveDependencies() const {
        auto entries =
            _entries.iterValue() |
            Collect<Vec<Entry>>();
        stableSort(entries);
        return entries;
    }

    Rc<ComputedValues> apply(ComputedValues const& parentComputedValues, RegisteredPropertySet& registeredPropertySet) {
        auto computedValues = registeredPropertySet.inheritsComputedValues(parentComputedValues);
        for (auto& entry : _resolveDependencies()) {
            auto& property = entry.property;

            if (property->isBogusProperty())
                continue;

            if (property->isShorthandProperty()) {
                for (auto& longhandProperty : property->expandShorthand(registeredPropertySet, parentComputedValues, *computedValues)) {
                    longhandProperty->apply(parentComputedValues, *computedValues);
                }
                continue;
            }

            property->apply(parentComputedValues, *computedValues);
        }

        return computedValues;
    }
};

} // namespace Vaev::Style
