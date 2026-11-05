module;

#include <karm/macros>

export module Vaev.Engine:values.percent;

import Karm.Core;

import :css;
import :values.base;
import :values.unitValue;
import :values.primitives;
import :values.resolved;

using namespace Karm;

namespace Vaev {

// MARK: Percentage ------------------------------------------------------------
// https://drafts.csswg.org/css-values/#percentages

export using Percent = Distinct<f64, struct _PercentTag>;

} // namespace Vaev

export template <>
struct Karm::Io::Formatter<Vaev::Percent> {
    Res<> format(Io::TextWriter& writer, Vaev::Percent value) {
        return Io::format(writer, "{}%", value.value());
    }
};
