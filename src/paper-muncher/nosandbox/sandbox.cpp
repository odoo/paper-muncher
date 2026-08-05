module PaperMuncher;

import Karm.Core;
import Karm.Logger;

using namespace Karm;

namespace PaperMuncher {

Res<> hardenSandbox() {
    logWarn("Sandbox hardening is not supported in this environment.");
    return Ok();
}

} // namespace PaperMuncher
