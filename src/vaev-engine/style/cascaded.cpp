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

    Vec<Opt<Entry>> _normalProperties = Vec<Opt<Entry>>::filled(enumLen<LonghandId>(), NONE);
    Map<Symbol, Entry> _customProperties;
    usize _nextAttributeOrder = 0;

    void _putLonghand(Rc<Property> property, Origin origin, usize layerOrder, Specificity specificity, bool important, usize ruleOrder, usize declarationOrder) {
        Entry entry{specificity, property, ruleOrder, declarationOrder, important, origin, layerOrder};

        if (property->id == LonghandId::CUSTOM) {
            auto name = property.is<CustomProperty>()->_value.name;
            auto existing = _customProperties.lookupOrPutDefault(name, entry);
            if (entry >= existing)
                existing = entry;
        } else {
            auto& existing = _normalProperties[toUnderlyingType(property->id)];
            if (not existing or entry >= *existing)
                existing = Some(entry);
        }
    }

    void putStyleRule(StyleRule const& rule, usize layerOrder, Specificity specificity, usize ruleOrder) {
        usize declarationOrder = 0;
        for (auto& prop : rule.props) {
            if (prop->isBogusProperty())
                continue;

            _putLonghand(prop, rule.origin, layerOrder, specificity, prop->important, ruleOrder, declarationOrder++);
        }
    }

    void putStyleAttribute(Rc<Property> property, Origin origin, Specificity specificity) {
        if (property->isBogusProperty())
            return;

        _putLonghand(property, origin, 0, specificity, property->important, 0, _nextAttributeOrder++);
    }

    void clear() {
        _nextAttributeOrder = 0;
        _customProperties.clear();
        for (auto& entry : _normalProperties)
            entry = NONE;
    }

    void applyCustomProperties(ComputedValues const& parent, Cow<ComputedValues>& child, ComputationContext const& cx) const {
        for (auto& entry : _customProperties.iterValue()) {
            auto& prop = entry.property;
            prop->apply(parent, child.cow(), cx);
        }
    }

    void apply(ComputationPhase phase, ComputedValues const& parent, Cow<ComputedValues>& child, ComputationContext const& cx) const {
        for (auto& entry : _normalProperties) {
            if (not entry)
                continue;

            auto& prop = entry->property;

            if (computationPhase(prop->id) != phase)
                continue;

            prop->apply(parent, child.cow(), cx);
        }
    }
};

} // namespace Vaev::Style
