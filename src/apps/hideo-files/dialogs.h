#pragma once

#include <karm-ui/node.h>

namespace Hideo::Files {

using OnFile = SharedFunc<void(Ui::Node&, Mime::Url)>;

Ui::Child openDialog(OnFile onFile);

Ui::Child saveDialog(OnFile onFile);

Ui::Child directoryDialog(OnFile onFile);

} // namespace Hideo::Files
