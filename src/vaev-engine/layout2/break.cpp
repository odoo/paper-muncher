export module Vaev.Engine:layout2.break_;

import Karm.Core;
import Karm.Math;
import Karm.Logger;

import :style;

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

    bool wouldOverflow(Au offset) const {
        return blockSize < blockOffset + offset;
    }

    bool hasContext(Type type) const {
        // FIXME: Check the full context chain once we support columns.
        return this->type == type;
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
    BreakBetween originatingBreak;
    BreakAppeal appeal;

    void repr(Io::Emit& e) const {
        e("(break appeal={} index={} inner={})", appeal, index, inner);
    }
};

export struct BreakNode : Meta::NoCopy {
    bool preservedMargin = false;

    struct BlockResumeData {
        usize startChildIndex;
    };

    using Inner = Union<BlockResumeData>;

    Inner _inner;
    Vec<BreakNode> _children;

    void repr(Io::Emit& e) const {
        e("(break-node preservedMargins={} [", preservedMargin);

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

export usize breakPrecedence(BreakBetween b) {
    switch (b) {

    case BreakBetween::AUTO:
        return 0;
    case BreakBetween::AVOID_REGION:
    case BreakBetween::AVOID_COLUMN:
    case BreakBetween::AVOID_PAGE:
        return 1;
    case BreakBetween::AVOID:
        return 2;
    case BreakBetween::REGION:
    case BreakBetween::COLUMN:
        return 3;
    case BreakBetween::PAGE:
        return 4;
    case BreakBetween::LEFT:
    case BreakBetween::RIGHT:
    case BreakBetween::RECTO:
    case BreakBetween::VERSO:
        return 5;
    case BreakBetween::_LEN:
        unreachable();
    }
}

} // namespace Vaev::Layout2
