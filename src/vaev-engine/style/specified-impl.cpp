module Vaev.Engine;

import :props.registry;

namespace Vaev::Style {

SpecifiedValues const& SpecifiedValues::initial() {
    static SpecifiedValues computed = [] {
        SpecifiedValues values{};
        auto registry = defaultRegistry();
        for (auto& [_, registration] : registry.registrations().iterUnordered())
            if (not registration->flags().has(Property::SHORTHAND_PROPERTY))
                registration->initial()->apply(values, values);
        return values;
    }();
    return computed;
}

} // namespace Vaev::Style
