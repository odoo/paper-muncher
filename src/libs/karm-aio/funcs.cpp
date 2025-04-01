module;

#include <karm-async/task.h>
#include <karm-base/array.h>
#include <karm-io/text.h>

export module Karm.Aio:funcs;

import :traits;

namespace marK::Aio {

export Async::Task<usize> copyAsync(AsyncReadable auto& reader, AsyncWritable auto& writer) {
    Array<Byte, 4096> buffer = {};
    usize result = 0;
    while (true) {
        auto read = co_trya$(reader.readAsync(mutBytes(buffer)));
        if (read == 0)
            co_return Ok(result);

        result += read;

        auto written = co_trya$(writer.writeAsync(sub(buffer, 0, read)));
        if (written != read)
            co_return Ok(result);
    }
}

export Async::Task<String> readAllUtf8Async(AsyncReadable auto& reader) {
    Io::StringWriter w;
    Array<Utf8::Unit, 512> buf = {};
    while (true) {
        usize read = co_trya$(reader.readAsync(buf.mutBytes()));
        if (read == 0)
            break;
        co_try$(w.writeUnit(sub(buf, 0, read)));
    }
    co_return Ok(w.take());
}

} // namespace marK::Aio
