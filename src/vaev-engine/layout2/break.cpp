export module Vaev.Engine:layout2.break_;

import Karm.Core;
import Karm.Math;
import Karm.Logger;

using namespace Karm;
using namespace Karm::Math;
using namespace Karm::Math::Literals;

namespace Vaev::Layout2 {

// https://www.w3.org/TR/css-break-3/#fragmentation-container
export struct Fragmentainer {
    enum struct Type {
        PAGE,
        COLUMN,
        REGION,
    };
    Type type;

    // https://www.w3.org/TR/css-break-3/#fragmentation-direction
    enum struct Direction {
        HORIZONTAL,
        VERTICAL,
    };
    Direction direction;

    Au blockSize;
    Au blockOffset;

    // https://www.w3.org/TR/css-break-3/#remaining-fragmentainer-extent
    Au remainingExtent() const {
        return blockSize - blockOffset;
    }

    Fragmentainer at(Au offset) const {
        logInfo("Created sub-fragmentainer: {}", blockSize - (blockOffset + offset));

        return {type, direction, blockSize, blockOffset + offset};
    }
};

// https://www.w3.org/TR/css-break-3/#unforced-breaks
enum struct BreakAppeal {
    /* more to get added */
    DROPS_AVOID,
    DROPS_ORPHANS_WINDOWS,
    PERFECT,
    _LEN,
};

export struct BreakOpportunity : Meta::NoCopy {
    Opt<Box<BreakOpportunity>> inner;
    usize index;
    Au consumedBlockSize;
    BreakAppeal appeal;
};

export struct BreakNode : Meta::NoCopy {
    bool preservedMargin = false;

    struct BlockResumeData {
    };

    using Inner = Union<BlockResumeData>;

    Inner _inner;
    Vec<BreakNode> _children;

    void repr(Io::Emit& e) const {
        e("(break-node [");

        if (not isEmpty(_children)) {
            e("\n");
            e.indent();
            for (auto const& child : _children)
                child.repr(e);
            e.deindent();
            e("\n");
        }

        e("])");
    }
};

} // namespace Vaev::Layout2
