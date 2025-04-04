#pragma once

#include <karm-gc/ptr.h>
#include <karm-print/page.h>
#include <karm-ui/input.h>
#include <vaev-dom/document.h>

#include "_prelude.h"

namespace Karm::Kira {

using PrintPreview = SharedFunc<Vec<Print::Page>(Print::Settings const&)>;

Ui::Child printDialog(PrintPreview preview);

} // namespace Karm::Kira
