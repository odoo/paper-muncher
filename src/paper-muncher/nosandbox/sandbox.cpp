module PaperMuncher;

import Karm.Core;
import Karm.Logger;

using namespace Karm;

namespace PaperMuncher {

Res<> hardenSandbox() {
    logWarn("sandbox hardening is not supported in this environment.");
    return Ok();
}

} // namespace PaperMuncher
