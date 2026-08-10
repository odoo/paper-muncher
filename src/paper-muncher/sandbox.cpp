export module PaperMuncher:sandbox;

import Karm.Core;

using namespace Karm;

namespace PaperMuncher {

struct Sandbox {
    DataSize memory;
};

export Res<> hardenSandbox(Sandbox sandbox);

} // namespace PaperMuncher
