export module Vaev.Engine:values.baseline;

import Karm.Core;
import :values.calc;
import :values.keywords;
import :values.percent;

using namespace Karm;

namespace Vaev::Style {

// https://www.w3.org/TR/css-inline-3/#baseline-source
export using BaselineSource = Union<Keywords::Auto, Keywords::First, Keywords::Last>;

// https://www.w3.org/TR/css-inline-3/#alignment-baseline-property
export using AlignmentBaseline = Union<
    Keywords::Baseline,
    Keywords::TextBottom,
    Keywords::Alphabetic,
    Keywords::Ideographic,
    Keywords::Middle,
    Keywords::Central,
    Keywords::Mathematical,
    Keywords::TextTop>;

// https://www.w3.org/TR/css-inline-3/#dominant-baseline-property
export using DominantBaseline = Union<
    Keywords::Auto,
    Keywords::Baseline,
    Keywords::TextBottom,
    Keywords::Alphabetic,
    Keywords::Ideographic,
    Keywords::Middle,
    Keywords::Central,
    Keywords::Mathematical,
    Keywords::TextTop>;

// https://drafts.csswg.org/css-inline/#propdef-baseline-shift
export using BaselineShift = Union<
    CalcValue<PercentOr<Length>>,
    Keywords::Sub,
    Keywords::Super,
    Keywords::Top,
    Keywords::Center,
    Keywords::Bottom>;

export struct Baseline {
    BaselineSource source = Keywords::AUTO;
    AlignmentBaseline alignment = Keywords::BASELINE;
    DominantBaseline dominant = Keywords::AUTO;
    BaselineShift shift = CalcValue<PercentOr<Length>>(Length{});

    void repr(Io::Emit& e) const {
        e("(baselines");
        e(" source={}", source);
        e(" alignment={}", alignment);
        e(" dominant={}", dominant);
        e(")");
    }
};

} // namespace Vaev::Style
