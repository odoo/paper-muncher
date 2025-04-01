#pragma once

#include <karm-sys/mmap.h>

#include "picture.h"

namespace marK::Image {

Res<Picture> load(Sys::Mmap&& map);

Res<Picture> load(Mime::Url url);

Res<Picture> loadOrFallback(Mime::Url url);

} // namespace Karm::Image
