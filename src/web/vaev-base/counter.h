#pragma once

#include <karm-base/map.h>
#include <karm-base/string.h>

namespace Vaev::Style {

enum struct SymbolsType {
    CYCLIC,
    NUMERIC,
    ALPHABETIC,
    SYMBOLIC,
    FIXED,
};

struct SymbolsFunc {
    SymbolsType type;
};

struct Counter {
};

struct CounterSet {
    Map<String, Counter> counters;
};

struct CounterStyle {};

struct CounterFunc {
};

struct CountersFunc {
    String counter;
    String sep;
};

} // namespace Vaev::Style
