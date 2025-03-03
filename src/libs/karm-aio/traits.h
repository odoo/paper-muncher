#pragma once

#include <karm-async/task.h>

namespace Karm::Aio {

struct Writer {
    virtual ~Writer() = default;
    virtual Async::Task<usize> writeAsync(Bytes buf) = 0;
};

struct Reader {
    virtual ~Reader() = default;
    virtual Async::Task<usize> readAsync(MutBytes buf) = 0;
};

} // namespace Karm::Aio
