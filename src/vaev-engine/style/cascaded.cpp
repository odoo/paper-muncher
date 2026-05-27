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

        auto operator<=>(Entry const& other) const {
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

    void _putLonghand(Rc<Property> property, Origin origin, Specificity specificity, Css::Important important, usize declarationOrder) {
        auto name = property->registration->name();
        Entry entry{property, important, origin, specificity, declarationOrder};
        auto& existing = _entries.lookupOrPutDefault(name, entry);
        if (entry >= existing)
            existing = entry;
    }

    void put(Rc<Property> property, Origin origin, Specificity specificity) {
        if (property->isBogusProperty())
            return;

        _putLonghand(property, origin, specificity, property->important, _declarationOrder++);
    }

    void clear() {
        _declarationOrder = 0;
        _entries.clear();
    }

    void expandShorthands(ComputedValues const& parent, ComputedValues& child, RegisteredPropertySet& registeredPropertySet) {
        Vec<Entry> shorthandEntries;
        for (auto const& entry : _entries.iterValue()) {
            if (entry.property->isShorthandProperty())
                shorthandEntries.pushBack(entry);
        }

        for (auto& entry : shorthandEntries) {
            auto& prop = entry.property;
            _entries.remove(prop->registration->name()).unwrap();
            for (auto& longhandProperty : prop->expandShorthand(registeredPropertySet, parent, child)) {
                _putLonghand(longhandProperty, entry.origin, entry.specificity, prop->important, entry.declarationOrder);
            }
        }
    }

    void apply(Property::ComputationPhase computationPhase, ComputedValues const& parent, ComputedValues& child) {
        for (auto& entry : _entries.iterValue()) {
            auto& prop = entry.property;

            if (prop->registration->computationPhase() != computationPhase)
                continue;

            prop->apply(parent, child);
        }
    }
};

} // namespace Vaev::Style
