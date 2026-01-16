module Vaev.Engine;

import :props.registry;

namespace Vaev::Style {

SpecifiedValues const& SpecifiedValues::initial() {
    static SpecifiedValues computed = [] {
        SpecifiedValues res{};
        auto registry = defaultRegistry();
        for (auto& [_, v] : registry.registrations().iterUnordered())
            if (not v->flags().has(Property::SHORTHAND_PROPERTY))
                v->initial()->apply(res, res);
        return res;
    }();
    return computed;
}

} // namespace Vaev::Style
