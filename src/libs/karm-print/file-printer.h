#pragma once

#include <karm-print/printer.h>
#include <vaev-base/resolution.h>

namespace Karm::Print {

struct FilePrinter : public Printer {
    Vaev::Resolution _scale;

    FilePrinter(Vaev::Resolution scale) : _scale(scale) {}

    virtual Res<> write(Io::Writer &w) = 0;
};

} // namespace Karm::Print
