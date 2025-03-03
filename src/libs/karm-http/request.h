#pragma once

#include <karm-mime/url.h>

#include "body.h"
#include "header.h"
#include "method.h"

namespace Karm::Net::Http {

struct Request {
    Method method;
    Mime::Url url;
    Version version;
    Header header;
    Opt<Rc<Body>> body;

    static Res<Request> parse(Io::SScan& s);

    Res<> unparse(Io::TextWriter& w);
};

} // namespace Karm::Http
