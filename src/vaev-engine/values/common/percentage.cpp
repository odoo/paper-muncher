export module Vaev.Engine:values.common.percentage;

import Karm.Core;

using namespace Karm;

namespace Vaev::Experimental {

// https://drafts.csswg.org/css-values/#percentages
export using Percentage = Distinct<f64, struct _PercentageTag>;

} // namespace Vaev::Experimental
