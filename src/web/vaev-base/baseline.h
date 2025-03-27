#pragma once

#include <karm-base/base.h>
#include <karm-io/emit.h>

#include "keywords.h"

namespace Vaev::Style {

using BaselineSource = Union<Keywords::Auto, Keywords::FirstLine, Keywords::LastLine>;

using AlignmentBaseline = Union<
    Keywords::Baseline,
    Keywords::TextBottom,
    Keywords::Alphabetic,
    Keywords::Ideographic,
    Keywords::Middle,
    Keywords::Central,
    Keywords::Mathematical,
    Keywords::TextTop>;

using DominantBaseline = Union<
    Keywords::Auto,
    Keywords::Baseline,
    Keywords::TextBottom,
    Keywords::Alphabetic,
    Keywords::Ideographic,
    Keywords::Middle,
    Keywords::Central,
    Keywords::Mathematical,
    Keywords::TextTop>;

struct Baseline {
    BaselineSource source = Keywords::AUTO;
    AlignmentBaseline alignment = Keywords::BASELINE;
    DominantBaseline dominant = Keywords::AUTO;

    void repr(Io::Emit& e) const {
        e("(baselines");
        e(" source={}", source);
        e(" alignment={}", alignment);
        e(" dominant={}", dominant);
        e(")");
    }
};

} // namespace Vaev::Style
