export module Vaev.Script:agent;

import Karm.Core;
import Karm.Gc;
import Vaev.Idl;

using namespace Karm;

namespace Vaev::Script {

// https://tc39.es/ecma262/#sec-agent-clusters
export struct AgentCluster {
    Gc::Heap heap;
};

// https://tc39.es/ecma262/#agent
export struct Agent : Idl::PlatformObject {
    AgentCluster& cluster;

    Agent(AgentCluster& cluster)
        : cluster(cluster) {}

    Gc::Heap& heap() {
        return cluster.heap;
    }

    bool is(Meta::Id id) const override {
        return id == Meta::idOf<Agent>() or PlatformObject::is(id);
    }
};

} // namespace Vaev::Script
