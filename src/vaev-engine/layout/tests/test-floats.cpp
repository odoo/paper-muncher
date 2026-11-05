#include <karm/test>

import Vaev.Engine;
import Karm.Core;
import Karm.Math;

using namespace Karm;
using namespace Karm::Math::Literals;

namespace Vaev::Layout::Tests {

static Range<Au> const CONTAINER = {0_au, 200_au};

test$("float-side-rtl") {
    assertEq$(resolveFloatSide(Float::LEFT, Keywords::RTL), FloatSide::LEFT);
    assertEq$(resolveFloatSide(Float::RIGHT, Keywords::RTL), FloatSide::RIGHT);
    assertEq$(resolveFloatSide(Float::INLINE_START, Keywords::LTR), FloatSide::LEFT);
    assertEq$(resolveFloatSide(Float::INLINE_END, Keywords::LTR), FloatSide::RIGHT);
    assertEq$(resolveFloatSide(Float::INLINE_START, Keywords::RTL), FloatSide::RIGHT);
    assertEq$(resolveFloatSide(Float::INLINE_END, Keywords::RTL), FloatSide::LEFT);
    return Ok();
}

test$("float-place-left-stacks-horizontally") {
    Floats floats;
    auto first = floats.place(FloatSide::LEFT, {25_au, 100_au}, 0_au, CONTAINER);
    floats.add({FloatSide::LEFT, first});
    auto second = floats.place(FloatSide::LEFT, {25_au, 100_au}, 0_au, CONTAINER);

    assertEq$(first, (RectAu{0_au, 0_au, 25_au, 100_au}));
    assertEq$(second, (RectAu{25_au, 0_au, 25_au, 100_au}));
    return Ok();
}

test$("float-place-right-stacks-from-right") {
    Floats floats;
    auto first = floats.place(FloatSide::RIGHT, {25_au, 100_au}, 0_au, CONTAINER);
    floats.add({FloatSide::RIGHT, first});
    auto second = floats.place(FloatSide::RIGHT, {25_au, 100_au}, 0_au, CONTAINER);

    assertEq$(first.x, 175_au);
    assertEq$(second.x, 150_au);
    return Ok();
}

test$("float-place-moves-down-when-full") {
    Floats floats;
    floats.add({FloatSide::LEFT, {0_au, 0_au, 150_au, 40_au}});
    floats.add({FloatSide::RIGHT, {150_au, 0_au, 50_au, 60_au}});

    auto placed = floats.place(FloatSide::LEFT, {100_au, 10_au}, 0_au, CONTAINER);

    // Beside the right float once the left one ends.
    assertEq$(placed, (RectAu{0_au, 40_au, 100_au, 10_au}));
    return Ok();
}

test$("float-place-two-halves-one-line") {
    Range<Au> container = {0_au, 199_au};
    Au half = container.size / 2;

    Floats floats;
    auto first = floats.place(FloatSide::LEFT, {half, 10_au}, 0_au, container);
    floats.add({FloatSide::LEFT, first});
    auto second = floats.place(FloatSide::LEFT, {half, 10_au}, 0_au, container);

    assertEq$(second.y, 0_au);
    return Ok();
}

test$("float-place-rounded-halves-one-line") {
    // NOTE: Percentages resolve with rounding, so two 50% floats can exceed their container by one unit each.
    Range<Au> container = {0_au, Au::fromRaw(39'999)};
    Au half = Au::fromRaw(20'000);

    Floats floats;
    auto first = floats.place(FloatSide::LEFT, {half, 10_au}, 0_au, container);
    floats.add({FloatSide::LEFT, first});
    auto second = floats.place(FloatSide::LEFT, {half, 10_au}, 0_au, container);

    assertEq$(second.y, 0_au);
    return Ok();
}

test$("float-place-not-above-previous-float") {
    Floats floats;
    floats.add({FloatSide::RIGHT, {150_au, 30_au, 50_au, 10_au}});

    auto placed = floats.place(FloatSide::LEFT, {10_au, 10_au}, 0_au, CONTAINER);

    assertEq$(placed.y, 30_au);
    return Ok();
}

test$("float-clearance") {
    Floats floats;
    assertEq$(floats.clearance(Clear::BOTH, Keywords::LTR), NONE);

    floats.add({FloatSide::RIGHT, {150_au, 0_au, 50_au, 60_au}});
    assertEq$(floats.clearance(Clear::LEFT, Keywords::LTR), NONE);
    assertEq$(floats.clearance(Clear::NONE, Keywords::LTR), NONE);
    assertEq$(floats.clearance(Clear::RIGHT, Keywords::LTR), Opt<Au>{Some(60_au)});
    assertEq$(floats.clearance(Clear::BOTH, Keywords::LTR), Opt<Au>{Some(60_au)});
    assertEq$(floats.clearance(Clear::INLINE_END, Keywords::LTR), Opt<Au>{Some(60_au)});
    assertEq$(floats.clearance(Clear::INLINE_END, Keywords::RTL), NONE);
    return Ok();
}

test$("float-free-band") {
    Floats floats;
    floats.add({FloatSide::RIGHT, {150_au, 0_au, 50_au, 60_au}});

    auto beside = floats.freeBand(10_au, 20_au, CONTAINER);
    assertEq$(beside.start, 0_au);
    assertEq$(beside.end(), 150_au);

    auto below = floats.freeBand(60_au, 20_au, CONTAINER);
    assertEq$(below.start, 0_au);
    assertEq$(below.end(), 200_au);

    assertEq$(floats.nextBottom(10_au, 20_au), Opt<Au>{Some(60_au)});
    assertEq$(floats.nextBottom(60_au, 20_au), NONE);
    return Ok();
}

test$("float-place-outside-containing-block-beside-right-float") {
    Floats floats;
    floats.add({FloatSide::RIGHT, {450_au, 0_au, 50_au, 300_au}});

    auto placed = floats.place(FloatSide::LEFT, {425_au, 10_au}, 0_au, {0_au, 400_au});
    assertEq$(placed.xy, (Vec2Au{0_au, 0_au}));

    auto below = floats.place(FloatSide::LEFT, {475_au, 10_au}, 0_au, {0_au, 400_au});
    assertEq$(below.y, 300_au);

    return Ok();
}

} // namespace Vaev::Layout::Tests
