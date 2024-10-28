#include "values.h"

#include "box.h"
#include "writing.h"

namespace Vaev::Layout {

Px _resolveFontRelative(Tree &tree, Box &box, Length value) {
    Karm::Text::Font rootFont = {
        tree.root.fontFace,
        tree.root.layout.fontSize.cast<f64>(),
    };

    Karm::Text::Font fragFont = {
        box.fontFace,
        box.layout.fontSize.cast<f64>(),
    };

    switch (value.unit()) {

    case Length::Unit::EM:
        return Px::fromFloatNearest(value.val() * fragFont.fontSize());

    case Length::Unit::REM:
        return Px::fromFloatNearest(value.val() * rootFont.fontSize());

    case Length::Unit::EX:
        return Px::fromFloatNearest(value.val() * fragFont.xHeight());

    case Length::Unit::REX:
        return Px::fromFloatNearest(value.val() * rootFont.xHeight());

    case Length::Unit::CAP:
        return Px::fromFloatNearest(value.val() * fragFont.capHeight());

    case Length::Unit::RCAP:
        return Px::fromFloatNearest(value.val() * rootFont.capHeight());

    case Length::Unit::CH:
        return Px::fromFloatNearest(value.val() * fragFont.zeroAdvance());

    case Length::Unit::RCH:
        return Px::fromFloatNearest(value.val() * rootFont.zeroAdvance());

    case Length::Unit::IC:
        return Px::fromFloatNearest(value.val() * fragFont.zeroAdvance());

    case Length::Unit::RIC:
        return Px::fromFloatNearest(value.val() * rootFont.zeroAdvance());

    case Length::Unit::LH:
        return Px::fromFloatNearest(value.val() * fragFont.lineHeight());

    case Length::Unit::RLH:
        return Px::fromFloatNearest(value.val() * rootFont.lineHeight());
    default:
        panic("expected font-relative unit");
    }
}

Px resolve(Tree &tree, Box &box, Length value) {
    if (value.isFontRelative())
        return _resolveFontRelative(tree, box, value);
    switch (value.unit()) {

    // Viewport-relative

    // https://drafts.csswg.org/css-values/#vw

    // Equal to 1% of the width of current viewport.
    case Length::Unit::VW:
    case Length::Unit::LVW:
        return Px::fromFloatNearest(value.val() * tree.viewport.large.width.cast<f64>() / 100);

    case Length::Unit::SVW:
        return Px::fromFloatNearest(value.val() * tree.viewport.small.width.cast<f64>() / 100);

    case Length::Unit::DVW:
        return Px::fromFloatNearest(value.val() * tree.viewport.dynamic.width.cast<f64>() / 100);

    // https://drafts.csswg.org/css-values/#vh
    // Equal to 1% of the height of current viewport.
    case Length::Unit::VH:
    case Length::Unit::LVH:
        return Px::fromFloatNearest(value.val() * tree.viewport.large.height.cast<f64>() / 100);

    case Length::Unit::SVH:
        return Px::fromFloatNearest(value.val() * tree.viewport.small.height.cast<f64>() / 100);

    case Length::Unit::DVH:
        return Px::fromFloatNearest(value.val() * tree.viewport.dynamic.height.cast<f64>() / 100);

    // https://drafts.csswg.org/css-values/#vi
    // Equal to 1% of the size of the viewport in the box’s inline axis.
    case Length::Unit::VI:
    case Length::Unit::LVI:
        if (mainAxis(box) == Axis::HORIZONTAL) {
            return Px::fromFloatNearest(value.val() * tree.viewport.large.width.cast<f64>() / 100);
        } else {
            return Px::fromFloatNearest(value.val() * tree.viewport.large.height.cast<f64>() / 100);
        }

    case Length::Unit::SVI:
        if (mainAxis(box) == Axis::HORIZONTAL) {
            return Px::fromFloatNearest(value.val() * tree.viewport.small.width.cast<f64>() / 100);
        } else {
            return Px::fromFloatNearest(value.val() * tree.viewport.small.height.cast<f64>() / 100);
        }

    case Length::Unit::DVI:
        if (mainAxis(box) == Axis::HORIZONTAL) {
            return Px::fromFloatNearest(value.val() * tree.viewport.dynamic.width.cast<f64>() / 100);
        } else {
            return Px::fromFloatNearest(value.val() * tree.viewport.dynamic.height.cast<f64>() / 100);
        }

    // https://drafts.csswg.org/css-values/#vb
    // Equal to 1% of the size of the viewport in the box’s block axis.
    case Length::Unit::VB:
    case Length::Unit::LVB:
        if (crossAxis(box) == Axis::HORIZONTAL) {
            return Px::fromFloatNearest(value.val() * tree.viewport.large.width.cast<f64>() / 100);
        } else {
            return Px::fromFloatNearest(value.val() * tree.viewport.large.height.cast<f64>() / 100);
        }

    case Length::Unit::SVB:
        if (crossAxis(box) == Axis::HORIZONTAL) {
            return Px::fromFloatNearest(value.val() * tree.viewport.small.width.cast<f64>() / 100);
        } else {
            return Px::fromFloatNearest(value.val() * tree.viewport.small.height.cast<f64>() / 100);
        }

    case Length::Unit::DVB:
        if (crossAxis(box) == Axis::HORIZONTAL) {
            return Px::fromFloatNearest(value.val() * tree.viewport.dynamic.width.cast<f64>() / 100);
        } else {
            return Px::fromFloatNearest(value.val() * tree.viewport.dynamic.height.cast<f64>() / 100);
        }

    // https://drafts.csswg.org/css-values/#vmin
    // Equal to the smaller of vw and vh.
    case Length::Unit::VMIN:
    case Length::Unit::LVMIN:
        return min(
            resolve(tree, box, Length(value.val(), Length::Unit::VW)),
            resolve(tree, box, Length(value.val(), Length::Unit::VH))
        );

    case Length::Unit::SVMIN:
        return min(
            resolve(tree, box, Length(value.val(), Length::Unit::SVW)),
            resolve(tree, box, Length(value.val(), Length::Unit::SVH))
        );

    case Length::Unit::DVMIN:
        return min(
            resolve(tree, box, Length(value.val(), Length::Unit::DVW)),
            resolve(tree, box, Length(value.val(), Length::Unit::DVH))
        );

    // https://drafts.csswg.org/css-values/#vmax
    // Equal to the larger of vw and vh.
    case Length::Unit::VMAX:
    case Length::Unit::LVMAX:
        return max(
            resolve(tree, box, Length(value.val(), Length::Unit::VW)),
            resolve(tree, box, Length(value.val(), Length::Unit::VH))
        );

    case Length::Unit::DVMAX:
        return max(
            resolve(tree, box, Length(value.val(), Length::Unit::DVW)),
            resolve(tree, box, Length(value.val(), Length::Unit::DVH))
        );

    case Length::Unit::SVMAX:
        return max(
            resolve(tree, box, Length(value.val(), Length::Unit::SVW)),
            resolve(tree, box, Length(value.val(), Length::Unit::SVH))
        );

    // Absolute
    // https://drafts.csswg.org/css-values/#absolute-lengths
    case Length::Unit::CM:
        return Px::fromFloatNearest(value.val() * tree.viewport.dpi.cast<f64>() / 2.54);

    case Length::Unit::MM:
        return Px::fromFloatNearest(value.val() * tree.viewport.dpi.cast<f64>() / 25.4);

    case Length::Unit::Q:
        return Px::fromFloatNearest(value.val() * tree.viewport.dpi.cast<f64>() / 101.6);

    case Length::Unit::IN:
        return Px::fromFloatNearest(value.val() * tree.viewport.dpi.cast<f64>());

    case Length::Unit::PT:
        return Px::fromFloatNearest(value.val() * tree.viewport.dpi.cast<f64>() / 72.0);

    case Length::Unit::PC:
        return Px::fromFloatNearest(value.val() * tree.viewport.dpi.cast<f64>() / 6.0);

    case Length::Unit::PX:
        return Px::fromFloatNearest(value.val());

    default:
        panic("invalid length unit");
    }
}

template <typename T>
concept Resolvable = requires {
    typename T::Resolved;
};

template <typename T>
using Resolved = Meta::Cond<Resolvable<T>, typename T::Resolved, T>;
static_assert(Resolvable<PercentOr<Length>>);

Px resolve(Tree &tree, Box &box, PercentOr<Length> value, Px relative) {
    if (value.resolved())
        return resolve(tree, box, value.value());
    return Px{relative.cast<f64>() * (value.percent().value() / 100.)};
}

template <typename T>
Resolved<T> resolveInfix(typename CalcValue<T>::OpCode op, Resolved<T> lhs, Resolved<T> rhs) {
    switch (op) {
    case CalcValue<T>::OpCode::ADD:
        return lhs + rhs;
    case CalcValue<T>::OpCode::SUBSTRACT:
        return lhs - rhs;
    case CalcValue<T>::OpCode::MULTIPLY:
        return lhs * rhs;
    case CalcValue<T>::OpCode::DIVIDE:
        return lhs / rhs;
    default:
        return lhs;
    }
}

template <typename T>
auto resolve(Tree &tree, Box &box, CalcValue<T> const &value, Px relative) {
    if (value.type == CalcValue<T>::OpType::FIXED) {
        return resolve(tree, box, value.lhs.template unwrap<T>(), relative);
    } else if (value.type == CalcValue<T>::OpType::SINGLE) {
        // TODO: compute result of funtion here with the resolved value
        return resolve(tree, box, value.lhs.template unwrap<T>(), relative);
    } else if (value.type == CalcValue<T>::OpType::CALC) {
        auto resolveUnion = Visitor{
            [&](T const &v) {
                return resolve<T>(tree, box, v, relative);
            },
            [&](CalcValue<T>::Leaf const &v) {
                return resolve<T>(tree, box, *v, relative);
            },
            [&](Number const &v) {
                return Math::i24f8{v};
            },
            [&](None const &) -> Resolved<T> {
                panic("invalid value in calc expression");
            }
        };

        return resolveInfix<T>(
            value.op,
            value.lhs.visit(resolveUnion),
            value.rhs.visit(resolveUnion)
        );
    }

    unreachable();
}

Px resolve(Tree &tree, Box &box, Width value, Px relative) {
    if (value == Width::Type::AUTO)
        return Px{0};
    return resolve(tree, box, value.value, relative);
}

Px resolve(Tree &tree, Box &box, FontSize value) {
    // FIXME: get from user settings
    f64 userFontSizes = 16;

    // FIXME: get from parent
    f64 parentFontSize = userFontSizes;

    switch (value.named()) {
    case FontSize::XX_SMALL:
        return Px::fromFloatNearest(userFontSizes * 0.5);
    case FontSize::X_SMALL:
        return Px::fromFloatNearest(userFontSizes * 0.75);
    case FontSize::SMALL:
        return Px::fromFloatNearest(userFontSizes * 0.875);
    case FontSize::MEDIUM:
        return Px::fromFloatNearest(userFontSizes);
    case FontSize::LARGE:
        return Px::fromFloatNearest(userFontSizes * 1.125);
    case FontSize::X_LARGE:
        return Px::fromFloatNearest(userFontSizes * 1.25);
    case FontSize::XX_LARGE:
        return Px::fromFloatNearest(userFontSizes * 1.5);

    case FontSize::LARGER:
        return Px::fromFloatNearest(parentFontSize * 1.25);
    case FontSize::SMALLER:
        return Px::fromFloatNearest(parentFontSize * 0.875);

    case FontSize::LENGTH:
        return resolve(tree, box, value.value(), Px{parentFontSize});

    default:
        panic("unimplemented font size");
        break;
    }
}

} // namespace Vaev::Layout
