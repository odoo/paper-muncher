#include <karm-io/pack.h>
#include <karm-test/macros.h>

namespace Karm::Io::Tests {

test$("test-emit-total") {
    struct DummyWriter : public Io::Writer {
        Res<usize> write(Bytes bytes) override {
            return Ok(bytes.len());
        }
    };

    DummyWriter writer;
    TextEncoder textEncoder{writer};
    Emit e{textEncoder};

    (void)e.writeRune(' ');

    expectEq$(e.flush().unwrap(), 1u);

    (void)e.writeRune('\n');

    expectEq$(e.flush().unwrap(), 2u);

    (void)e.writeRune('\n');
    (void)e.writeRune(' ');

    expectEq$(e.flush().unwrap(), 4u);

    (void)e.writeRune('\n');
    (void)e.writeRune('\n');

    // TODO: confirm this is expected behaviour; if not, a solution would be to implement _newline as a usize counter
    expectEq$(e.flush().unwrap(), 5u);

    return Ok();
}

} // namespace Karm::Io::Tests
