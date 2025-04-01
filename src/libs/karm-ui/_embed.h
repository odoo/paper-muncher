#pragma once

#include <karm-sys/context.h>
#include <karm-ui/host.h>

namespace marK::Ui::_Embed {

Res<Rc<Host>> makeHost(Child root);

} // namespace Karm::Ui::_Embed
