module PaperMuncher;

import Karm.Core;
import Karm.Logger;

using namespace Karm;

namespace PaperMuncher {

Res<> hardenSandbox(Sandbox) {
    logWarn("sandbox hardening is not supported in this environment.");
    return Ok();
}

} // namespace PaperMuncher
