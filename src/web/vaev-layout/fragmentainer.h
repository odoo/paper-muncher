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
    bool _isDiscoveryMode = false;

    void setDiscovery() { _isDiscoveryMode = true; }

    void unsetDiscovery() { _isDiscoveryMode = false; }

    bool isDiscoveryMode() {
        return allowBreak() and _isDiscoveryMode;
    }

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

    bool acceptsFit(Px verticalPosition, Px verticalSize, Px pendingVerticalSizes) {
        return verticalPosition + verticalSize + pendingVerticalSizes <= defaultSize.y;
    }
};

} // namespace Vaev::Layout
