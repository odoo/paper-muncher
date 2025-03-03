#pragma once

#include <karm-base/rc.h>

#include "traits.h"

namespace Karm::Aio {

struct Pipe {
    bool _closed;

    struct Reader : public Aio::Reader {
        Rc<Pipe> _pipe;
    };

    struct Writer : public Aio::Writer {
        Rc<Pipe> _pipe;
    };
};

Pair<Rc<Pipe::Writer>, Rc<Pipe::Reader>> pipe();

} // namespace Karm::Aio
