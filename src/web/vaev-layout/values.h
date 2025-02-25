#pragma once

#include <karm-text/font.h>
#include <vaev-base/color.h>
#include <vaev-base/font.h>
#include <vaev-base/width.h>

#include "base.h"

namespace Vaev::Layout {

template <typename T>
concept Resolvable = requires {
    typename T::Resolved;
};

template <typename T>
using Resolved = Meta::Cond<Resolvable<T>, typename T::Resolved, T>;
static_assert(Resolvable<PercentOr<Length>>);

struct Resolver {
    f64 userFontSize = 16;   /// Font size of the user agent
    f64 parentFontSize = 16; /// Font size of the parent box

    Opt<Text::Font> rootFont = NONE;                 /// Font of the root element
    Opt<Text::Font> boxFont = NONE;                  /// Font of the current box
    Viewport viewport = {.small = {800_au, 600_au}}; /// Viewport of the current box
    Axis boxAxis = Axis::HORIZONTAL;                 /// Inline axis of the current box

    static Resolver from(Tree const& tree, Box const& box);

    Resolver inherit(Resolver const& resolver);

    Au _resolveFontRelative(Length const& value);

    Au resolve(Length const& value);

    Au resolve(PercentOr<Length> const& value, Au relative);

    Au resolve(Width const& value, Au relative);

    Au resolve(FontSize const& value);

    // MARK: Eval --------------------------------------------------------------

    template <typename T>
    Resolved<T> _resolveUnary(CalcOp, Resolved<T>) {
        notImplemented();
    }

    template <typename T>
    Resolved<T> _resolveInfix(CalcOp op, Resolved<T> lhs, Resolved<T> rhs) {
        switch (op) {
        case CalcOp::ADD:
            return lhs + rhs;
        case CalcOp::SUBSTRACT:
            return lhs - rhs;
        case CalcOp::MULTIPLY:
            return lhs * rhs;
        case CalcOp::DIVIDE:
            return lhs / rhs;
        default:
            panic("unexpected operator");
        }
    }

    template <typename T, typename... Args>
    auto resolve(CalcValue<T> const& calc, Args... args) -> Resolved<T> {
        auto resolveUnion = Visitor{
            [&](T const& v) {
                return resolve(v, args...);
            },
            [&](CalcValue<T>::Leaf const& v) {
                return resolve<T>(*v, args...);
            },
            [&](Number const& v) {
                return Math::i24f8{v};
            }
        };

        return calc.visit(Visitor{
            [&](CalcValue<T>::Value const& v) {
                return v.visit(resolveUnion);
            },
            [&](CalcValue<T>::Unary const& u) {
                return _resolveUnary<T>(
                    u.op,
                    u.val.visit(resolveUnion)
                );
            },
            [&](CalcValue<T>::Binary const& b) {
                return _resolveInfix<T>(
                    b.op,
                    b.lhs.visit(resolveUnion),
                    b.rhs.visit(resolveUnion)
                );
            }
        });
    }
};

// MARK: Resolve during layout -------------------------------------------------

Au resolve(Tree const& tree, Box const& box, Length const& value);

Au resolve(Tree const& tree, Box const& box, PercentOr<Length> const& value, Au relative);

Au resolve(Tree const& tree, Box const& box, Width const& value, Au relative);

Au resolve(Tree const& tree, Box const& box, FontSize const& value);

template <typename T, typename... Args>
static inline auto resolve(Tree const& tree, Box const& box, CalcValue<T> const& value, Args... args) -> Resolved<T> {
    return Resolver::from(tree, box).resolve(value, args...);
}

} // namespace Vaev::Layout
