#pragma once

#include "body.h"
#include "code.h"
#include "header.h"

namespace Karm::Net::Http {

struct Response {
    Version version;
    Code code = Code::OK;
    Header header;
    Opt<Rc<Body>> body;

    struct Writer : public Io::Writer {
        virtual Header& header() = 0;
        virtual Res<> writeHeader(Code code) = 0;
    };

    static Res<Response> parse(Io::SScan& s);

    static Res<Response> read(Io::Reader& r);

    Res<Opt<Buf<Byte>>> readBody(Io::Reader& r);
};

} // namespace Karm::Http
