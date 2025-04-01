#pragma once

#include <karm-ui/input.h>

#include "_prelude.h"

namespace marK::Kira {

Ui::Child input(Mdi::Icon icon, String placeholder, String text, Ui::OnChange<String> onChange = NONE);

Ui::Child input(String placeholder, String text, Ui::OnChange<String> onChange = NONE);

} // namespace Karm::Kira
