#include <karm-base/opt.h>
#include <karm-math/float.h>
#include <karm-test/macros.h>

namespace Karm::Math::Tests {

test$("f64-niche") {
    Opt<f64> test;

    expectEq$(sizeof(test), sizeof(f64));
    expectEq$(test.has(), false);
    expectEq$(test, NONE);
    test = 5;
    expectEq$(test.unwrap(), 5);
    expectEq$(test.take(), 5);
    expectEq$(test, NONE);
    test = Math::NAN;
    expectEq$(test.has(), true);
    test = -Math::NAN;
    expectEq$(test.has(), true);

    return Ok();
}

test$("f32-niche") {
    Opt<f32> test;

    expectEq$(sizeof(test), sizeof(f32));
    expectEq$(test.has(), false);
    expectEq$(test, NONE);
    test = 5;
    expectEq$(test.unwrap(), 5);
    expectEq$(test.take(), 5);
    expectEq$(test, NONE);

    f32 const NAN = 0.0f / 0.0f;
    f32 const INF = 1.0f / 0.0f;
    f32 const NEG_INF = -1.0f / 0.0f;
    Array<f32, 10> values = {
        NAN,
        NEG_INF,
        NAN,
        -NAN,
        INF * 0.0f,
        NEG_INF * 0.0f,
        0.0f / 0.0f,
        0.0f / (-0.0f),
        INF / INF,
        INF / NEG_INF,
    };
    for (auto val : values) {
        test = val;
        expectEq$(test.has(), true);
    }

    return Ok();
}

test$("f16-niche") {
    Opt<f16> test;

    expectEq$(sizeof(test), sizeof(f16));
    expectEq$(test.has(), false);
    expectEq$(test, NONE);
    test = 5;
    expectEq$(test.unwrap(), 5);
    expectEq$(test.take(), 5);
    expectEq$(test, NONE);

    Array<f64, 10> values = {
        Math::INF,
        Math::NEG_INF,
        Math::NAN,
        -Math::NAN,
        Math::INF * 0.0,
        Math::NEG_INF * 0.0,
        0.0 / 0.0,
        0.0 / (-0.0),
        Math::INF / Math::INF,
        Math::INF / Math::NEG_INF,
    };
    for (auto val : values) {
        test = val;
        expectEq$(test.has(), true);
    }

    return Ok();
}

} // namespace Karm::Math::Tests
