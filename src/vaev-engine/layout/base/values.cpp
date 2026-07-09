export module Vaev.Engine:layout.values;

import Karm.Gfx;
import Karm.Math;
import Karm.Logger;

import :values;
import :layout.formating;

namespace Vaev::Layout {

export struct Resolver :
    RelativeLengthContextData,
    FontSizeContextData {

    static Resolver from(Tree const& tree, Box const& box) {
        Gfx::Font font = Gfx::Font{box.style->fontFace, box.style->font->size.cast<f64>()};
        Gfx::Font rootFont = Gfx::Font{tree.root.style->fontFace, 16};

        Resolver ctx;

        ctx.fontSize = font.fontSize();
        ctx.xHeight = font.xHeight();
        ctx.capHeight = font.capHeight();
        ctx.zeroAdvance = font.zeroAdvance();
        ctx.lineHeight = font.lineHeight();

        ctx.rootFontSize = rootFont.fontSize();
        ctx.rootXHeight = rootFont.xHeight();
        ctx.rootCapHeight = rootFont.capHeight();
        ctx.rootZeroAdvance = rootFont.zeroAdvance();
        ctx.rootLineHeight = rootFont.lineHeight();

        ctx.viewportSmallWidth = tree.viewport.small.width.cast<f64>();
        ctx.viewportSmallHeight = tree.viewport.small.height.cast<f64>();

        ctx.viewportLargeWidth = tree.viewport.large.width.cast<f64>();
        ctx.viewportLargeHeight = tree.viewport.large.height.cast<f64>();

        ctx.viewportDynamicWidth = tree.viewport.dynamic.width.cast<f64>();
        ctx.viewportDynamicHeight = tree.viewport.dynamic.height.cast<f64>();

        return ctx;
    }

    Au resolve(Length const& value) {
        return Vaev::resolve(value, *this);
    }

    Au resolve(LineWidth const& value) {
        return Vaev::resolve(value, *this);
    }

    Au resolve(PercentOr<Length> const& value, Au relative) {
        return Vaev::resolve(value, *this, relative);
    }

    Au resolve(Width const& value, Au relative) {
        if (value.is<Keywords::Auto>())
            return 0_au;
        return Vaev::resolve(value.unwrap<CalcValue<PercentOr<Length>>>(), *this, relative);
    }

    Rad resolve(Angle const& value) {
        return Rad{value.toRadian()};
    }

    Number resolve(Number const& value) {
        return value;
    }

    Au resolve(FontSize const& value) {
        return Vaev::resolve(value, *this);
    }

    template <typename T, typename... Args>
    Resolved<T> resolve(CalcValue<T> const& calc, Args... args) {
        return Vaev::resolve(calc, *this, args...);
    }
};

// MARK: Resolve during layout -------------------------------------------------

// HACK: Temporary workaround while we don't properly evaluate computed values
export bool isPurePercentage(CalcValue<PercentOr<Length>> calcValue) {
    if (not calcValue._inner.is<CalcValue<PercentOr<Length>>::Value>())
        return false;

    auto const& value = calcValue._inner.unwrap<CalcValue<PercentOr<Length>>::Value>();
    if (not value.is<PercentOr<Length>>())
        return false;

    return value.unwrap<PercentOr<Length>>().is<Percent>();
}

export Au resolve(Tree const& tree, Box const& box, Length const& value) {
    return Resolver::from(tree, box).resolve(value);
}

export Au resolve(Tree const& tree, Box const& box, PercentOr<Length> const& value, Au relative) {
    return Resolver::from(tree, box).resolve(value, relative);
}

export Au resolve(Tree const& tree, Box const& box, Width const& value, Au relative) {
    return Resolver::from(tree, box).resolve(value, relative);
}

export Au resolve(Tree const& tree, Box const& box, LineWidth const& value) {
    return Resolver::from(tree, box).resolve(value);
}

export Au resolve(Tree const& tree, Box const& box, FontSize const& value) {
    return Resolver::from(tree, box).resolve(value);
}

export template <typename T, typename... Args>
auto resolve(Tree const& tree, Box const& box, CalcValue<T> const& value, Args... args) -> Resolved<T> {
    return Resolver::from(tree, box).resolve(value, args...);
}

} // namespace Vaev::Layout
