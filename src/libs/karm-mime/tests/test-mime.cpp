#include <karm-mime/mime.h>
#include <karm-test/macros.h>

namespace marK::Mime::Tests {

test$("karm-mime-mime-parse") {
    auto mime = "text/plain"_mime;
    expectEq$(mime.type(), "text"s);
    expectEq$(mime.subtype(), "plain"s);
    expectEq$(mime.str(), "text/plain"s);
    return Ok();
}

} // namespace marK::Mime::Tests
