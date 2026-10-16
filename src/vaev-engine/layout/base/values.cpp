export module Vaev.Engine:layout.values;

import Karm.Gfx;
import Karm.Math;
import Karm.Logger;

import :values;
import :layout.formating;

namespace Vaev::Layout {

template <typename T>
auto _auToPx(T value) {
    if constexpr (Meta::Same<T, Au>)
        return Pixel{value.template cast<f64>()};
    else
        return value;
}

template <typename T>
auto _pxToAu(T value) {
    if constexpr (Meta::Same<T, Pixel>)
        return Au{value};
    else
        return value;
}

export template <typename T>
using LayoutResolved = decltype(_pxToAu(Meta::declval<Resolved<T>>()));

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
        return _pxToAu(Vaev::resolve(value, *this));
    }

    Au resolve(LineWidth const& value) {
        return _pxToAu(Vaev::resolve(value, *this));
    }

    Au resolve(PercentOr<Length> const& value, Au relative) {
        return _pxToAu(Vaev::resolve(value, *this, _auToPx(relative)));
    }

    Au resolve(Union<Keywords::Auto, Calc<PercentOr<Length>>> const& value, Au relative) {
        if (value.is<Keywords::Auto>())
            return 0_au;
        return _pxToAu(Vaev::resolve(value.expect<Calc<PercentOr<Length>>>(), *this, _auToPx(relative)));
    }

    Rad resolve(Angle const& value) {
        return Rad{value.toRadian()};
    }

    Number resolve(Number const& value) {
        return value;
    }

    Au resolve(FontSize const& value) {
        return _pxToAu(Vaev::resolve(value, *this));
    }

    template <typename T, typename... Args>
    LayoutResolved<T> resolve(Calc<T> const& calc, Args... args) {
        return _pxToAu(Vaev::resolve(calc, *this, _auToPx(args)...));
    }
};

// MARK: Resolve during layout -------------------------------------------------

// HACK: Temporary workaround while we don't properly evaluate computed values
export bool isPurePercentage(Calc<PercentOr<Length>> calcValue) {
    if (not calcValue._inner.is<Calc<PercentOr<Length>>::Value>())
        return false;

    auto const& value = calcValue._inner.expect<Calc<PercentOr<Length>>::Value>();
    if (not value.is<PercentOr<Length>>())
        return false;

    return value.expect<PercentOr<Length>>().is<Percent>().has();
}

export Au resolve(Tree const& tree, Box const& box, Length const& value) {
    return Resolver::from(tree, box).resolve(value);
}

export Au resolve(Tree const& tree, Box const& box, PercentOr<Length> const& value, Au relative) {
    return Resolver::from(tree, box).resolve(value, relative);
}

export Au resolve(Tree const& tree, Box const& box, Union<Keywords::Auto, Calc<PercentOr<Length>>> const& value, Au relative) {
    return Resolver::from(tree, box).resolve(value, relative);
}

export Au resolve(Tree const& tree, Box const& box, LineWidth const& value) {
    return Resolver::from(tree, box).resolve(value);
}

export Au resolve(Tree const& tree, Box const& box, FontSize const& value) {
    return Resolver::from(tree, box).resolve(value);
}

export template <typename T, typename... Args>
auto resolve(Tree const& tree, Box const& box, Calc<T> const& value, Args... args) -> LayoutResolved<T> {
    return Resolver::from(tree, box).resolve(value, args...);
}

} // namespace Vaev::Layout
