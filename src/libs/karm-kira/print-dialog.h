#pragma once

#include <karm-print/page.h>
#include <karm-ui/input.h>

#include "_prelude.h"

namespace marK::Kira {

using PrintPreview = SharedFunc<Vec<Print::Page>(Print::Settings const&)>;

Ui::Child printDialog(PrintPreview preview);

} // namespace Karm::Kira
