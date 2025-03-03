#pragma once

#include <karm-aio/traits.h>
#include <karm-async/task.h>
#include <karm-io/traits.h>
#include <karm-sys/file.h>

namespace Karm::Net::Http {

struct Body : public Aio::Reader {
    static Rc<Body> from(Sys::File);

    static Rc<Body> from(Buf<Byte>);

    virtual ~Body() = default;
};

} // namespace Karm::Net::Http
