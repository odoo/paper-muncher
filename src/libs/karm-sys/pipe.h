#pragma once

#include "file.h"

namespace marK::Sys {

struct Pipe {
    FileWriter in;
    FileReader out;

    static Res<Pipe> create();
};

} // namespace Karm::Sys
