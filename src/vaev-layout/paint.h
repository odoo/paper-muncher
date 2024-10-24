#pragma once

#include <karm-scene/stack.h>

#include "frag.h"

namespace Vaev::Layout {

void wireframe(Box &frag, Gfx::Canvas &g);

void paint(Box &frag, Scene::Stack &stack);

} // namespace Vaev::Layout
