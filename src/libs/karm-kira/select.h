#pragma once

#include <karm-ui/input.h>

#include "_prelude.h"

namespace marK::Kira {

Ui::Child select(Ui::Child value, Ui::Slots options);

Ui::Child selectValue(String text);

Ui::Child selectLabel(String text);

Ui::Child selectItem(Ui::OnPress onPress, String t);

Ui::Child selectGroup(Ui::Children children);

} // namespace Karm::Kira
