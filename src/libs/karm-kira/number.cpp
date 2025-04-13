module;

#include <karm-ui/focus.h>
#include <karm-ui/layout.h>
#include <mdi/minus.h>
#include <mdi/plus.h>

export module Karm.Kira:number;

namespace Karm::Kira {

export Ui::Child number(f64 value, Ui::OnChange<f64> onChange, f64 step) {
    return Ui::hflow(
               Ui::button(
                   [onChange, value, step](auto& n) {
                       onChange(n, value - step);
                   },
                   Ui::ButtonStyle::subtle(),
                   Mdi::MINUS
               ),
               Ui::labelMedium("{:.02}", value) | Ui::insets({0, 4}) | Ui::center(),
               Ui::button(
                   [onChange, value, step](auto& n) {
                       onChange(n, value + step);
                   },
                   Ui::ButtonStyle::subtle(),
                   Mdi::PLUS
               )
           ) |
           Ui::box({
               .borderRadii = 4,
               .borderWidth = 1,
               .borderFill = Ui::GRAY800,
           }) |
           Ui::focusable();
}

} // namespace Karm::Kira
