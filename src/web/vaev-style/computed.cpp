#include "computed.h"

#include "props.h"

namespace Vaev::Style {

Computed const& Computed::initial() {
    static Computed computed = [] {
        Computed res{};
        StyleProp::any([&]<typename T>(Meta::Type<T>) {
            if constexpr (requires { T::initial(); })
                T{}.apply(res);
        });
        return res;
    }();
    return computed;
}

void Computed::inherit(Computed const& parent) {
    color = parent.color;
    font = parent.font;
    text = parent.text;
    variables = parent.variables;
    visibility = parent.visibility;
}

void Computed::repr(Io::Emit& e) const {
    e("(computed");
    e(" color: {}", color);
    e(" opacity: {}", opacity);
    e(" aligns: {}", aligns);
    e(" gaps: {}", gaps);
    e(" backgrounds: {}", backgrounds);
    e(" borders: {}", borders);
    e(" margin: {}", margin);
    e(" padding: {}", padding);
    e(" boxSizing: {}", boxSizing);
    e(" sizing: {}", sizing);
    e(" overflows: {}", overflows);
    e(" position: {}", position);
    e(" offsets: {}", offsets);
    e(" writingMode: {}", writingMode);
    e(" direction: {}", direction);
    e(" display: {}", display);
    e(" order: {}", order);
    e(" visibility: {}", visibility);
    e(" table: {}", table);
    e(" font: {}", font);
    e(" text: {}", text);
    e(" flex: {}", flex);
    e(" break: {}", break_);
    e(" float: {}", float_);
    e(" clear: {}", clear);
    e(" zIndex: {}", zIndex);
    e(" variables: {}", variables);
    e(")");
}

void Computed::setCustomProp(Str varName, Css::Content value) {
    variables.cow().put(varName, value);
}

Css::Content Computed::getCustomProp(Str varName) const {
    auto value = variables->access(varName);
    if (value)
        return *value;
    return {};
}
} // namespace Vaev::Style
