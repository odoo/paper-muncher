export module Vaev.Engine:values.computed;

import Karm.Core;
import Karm.Math;
import Karm.Gfx;

import :values.writing;

using namespace Karm;

namespace Vaev {

export template <typename T>
struct _Computed {
    using Type = T;
};

export template <typename T>
using Computed = typename _Computed<T>::Type;

export struct Viewport {
    // https://drafts.csswg.org/css-values/#small-viewport-size
    RectAu small;
    // https://drafts.csswg.org/css-values/#large-viewport-size
    RectAu large = small;
    // https://drafts.csswg.org/css-values/#dynamic-viewport-size
    RectAu dynamic = small;
};

struct ComputationContext {
    // FIXME: Fonts are not optional but are wrapped in Opt<> due to the lack of a default constructor.
    //        They also require mutability for caching stuff which forces ComputationContext to be passed
    //        by mut reference. Ideally they should be replaced by a richer version of
    Opt<Gfx::Font> rootFont = NONE;
    Opt<Gfx::Font> font = NONE;
    WritingMode writingMode = WritingMode::HORIZONTAL_TB;
    Viewport viewport = {.small = {800_au, 600_au}}; /// Viewport of the current box
    Vec2Au displayArea = {800_au, 600_au};
};

export template <typename T>
concept Computible = requires(T const& a, ComputationContext& ctx) {
    { toComputed(a, ctx) } -> Meta::Convertible<Computed<T>>;
};

export template <typename T>
struct IdentityComputed {
    using Type = T;
};

export template <typename T>
    requires Meta::Derive<_Computed<T>, IdentityComputed<T>>
Computed<T> toComputed(T const& val, ComputationContext&) {
    return val;
}

} // namespace Vaev
