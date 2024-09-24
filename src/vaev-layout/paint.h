#pragma once

#include <vaev-paint/stack.h>

#include "frag.h"

namespace Vaev::Layout {

void paint(Frag &frag, Paint::Stack &stack, Math::Vec2f pos = {});

} // namespace Vaev::Layout
