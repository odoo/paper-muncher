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
        Specificity specificity;
        Rc<Property> property;
        usize ruleOrder;
        usize declarationOrder;
        bool important;
        Origin origin;
        usize layerOrder;

        auto operator<=>(Entry const& other) const {
            if (important != other.important)
                return important <=> other.important;
            if (origin != other.origin)
                return origin <=> other.origin;
            if (layerOrder != other.layerOrder)
                return important ? other.layerOrder <=> layerOrder : layerOrder <=> other.layerOrder;
            if (specificity != other.specificity)
                return specificity <=> other.specificity;
            if (ruleOrder != other.ruleOrder)
                return ruleOrder <=> other.ruleOrder;
            return declarationOrder <=> other.declarationOrder;
        }
    };

    Map<Symbol, Entry> _entries;
    usize _nextAttributeOrder = 0;

    void _putLonghand(Rc<Property> property, Origin origin, usize layerOrder, Specificity specificity, bool important, usize ruleOrder, usize declarationOrder) {
        auto name = property->registration->name;
        Entry entry{specificity, property, ruleOrder, declarationOrder, important, origin, layerOrder};
        auto& existing = _entries.lookupOrPutDefault(name, entry);
        if (entry >= existing)
            existing = entry;
    }

    void putStyleRule(StyleRule const& rule, usize layerOrder, Specificity specificity, usize ruleOrder) {
        usize declarationOrder = 0;
        for (auto& [prop, important] : rule.declarations) {
            if (prop->isBogusProperty())
                continue;

            _putLonghand(prop, rule.origin, layerOrder, specificity, important, ruleOrder, declarationOrder++);
        }
    }

    void putStyleAttribute(Declaration const& declaration, Origin origin, Specificity specificity) {
        auto const& [prop, important] = declaration;

        if (prop->isBogusProperty())
            return;

        _putLonghand(prop, origin, 0, specificity, important, 0, _nextAttributeOrder++);
    }

    void clear() {
        _nextAttributeOrder = 0;
        _entries.clear();
    }

    void expands(ComputedValues const& parent, ComputedValues& child, RegisteredPropertySet& registeredPropertySet) {
        Vec<Entry> shorthandEntries;
        for (auto const& entry : _entries.iterValue()) {
            if (entry.property->isShorthandProperty())
                shorthandEntries.pushBack(entry);
        }

        for (auto& entry : shorthandEntries) {
            auto& prop = entry.property;
            _entries.remove(prop->registration->name).expect();
            for (auto& longhandProperty : prop->expand(registeredPropertySet, parent, child)) {
                _putLonghand(longhandProperty, entry.origin, entry.layerOrder, entry.specificity, entry.important, entry.ruleOrder, entry.declarationOrder);
            }
        }
    }

    void apply(Property::ComputationPhase computationPhase, ComputedValues const& parent, Cow<ComputedValues>& child, ComputationContext const& cx) {
        for (auto& entry : _entries.iterValue()) {
            auto& prop = entry.property;

            if (prop->registration->computationPhase() != computationPhase)
                continue;

            prop->apply(parent, child.cow(), cx);
        }
    }
};

} // namespace Vaev::Style
