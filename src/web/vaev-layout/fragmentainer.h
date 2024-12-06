#pragma once

#include <karm-image/picture.h>
#include <karm-text/run.h>
#include <vaev-markup/dom.h>
#include <vaev-style/computer.h>

#include "frag.h"

namespace Vaev::Layout {

// https://www.w3.org/TR/css-break-3/#fragmentainer
// https://www.w3.org/TR/css-break-3/#fragmentation-context
struct FragmentationContext {

    // discovery mode:
    // first pass that identifies the breakpoints (forced or not); that is, breakpoints are only created and manipulated
    // in discovery mode
    bool isDiscoveryMode = true;

    Vec2Px defaultSize = {Limits<Px>::MAX, Limits<Px>::MAX};

    FragmentationContext() {}

    FragmentationContext(Vec2Px defaultSize) : defaultSize(defaultSize) {}

    // NOTE: a bit rudimentar, but necessary while some displays do not have fragmentation implemented
    usize monolithicCount = 0;

    void enterMonolithicBox() {
        monolithicCount++;
    }

    void leaveMonolithicBox() {
        monolithicCount--;
    }

    bool hasInfiniteDimensions() {
        return defaultSize == Vec2Px{Limits<Px>::MAX, Limits<Px>::MAX};
    }

    bool allowBreak() {
        return not hasInfiniteDimensions() and monolithicCount == 0;
    }

    bool acceptsFit(Vec2Px position, Vec2Px sizeBox, Vec2Px ancestralsBorderPadding) {
        Vec2Px total = position + sizeBox + ancestralsBorderPadding;
        // NOTE: not considering X coordinate for overflow
        return total.y <= defaultSize.y;
    }

    Px leftVerticalSpace(Vec2Px position, Vec2Px ancestralsBorderPadding) {
        return defaultSize.y - position.y - ancestralsBorderPadding.y;
    }
};

} // namespace Vaev::Layout
