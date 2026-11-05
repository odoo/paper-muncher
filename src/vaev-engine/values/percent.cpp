module;

#include <karm/macros>

export module Vaev.Engine:values.percent;

import Karm.Core;

import :css;
import :values.base;
import :values.unitValue;
import :values.primitives;

using namespace Karm;

namespace Vaev::Style {

// MARK: Percentage ------------------------------------------------------------
// https://drafts.csswg.org/css-values/#percentages

export using Percent = Distinct<f64, struct _PercentTag>;

} // namespace Vaev::Style

export template <>
struct Karm::Io::Formatter<Vaev::Style::Percent> {
    Res<> format(Io::TextWriter& writer, Vaev::Style::Percent value) {
        return Io::format(writer, "{}%", value.value());
    }
};
