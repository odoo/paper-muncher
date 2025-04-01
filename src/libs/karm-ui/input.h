#pragma once

#include <karm-text/edit.h>

#include "box.h"
#include "funcs.h"
#include "view.h"

namespace marK::Ui {

template <typename T = None>
using OnChange = Meta::Cond<
    Meta::Same<T, None>,
    Opt<SharedFunc<void(Node&)>>,
    Opt<SharedFunc<void(Node&, T value)>>>;

template <typename T>
static inline auto IGNORE(Ui::Node&, T) {}

// MARK: Button ----------------------------------------------------------------

struct MouseListener {
    enum MouseState {
        IDLE,
        HOVER,
        PRESS,
    };

    MouseState _state = IDLE;
    Math::Vec2i _pos = {0, 0};

    auto state() const {
        return _state;
    }

    bool isIdle() const {
        return _state == IDLE;
    }

    bool isHover() const {
        return _state == HOVER;
    }

    bool isPress() const {
        return _state == PRESS;
    }

    auto pos() const {
        return _pos;
    }

    bool listen(Node& node, App::Event& event) {
        bool result = false;
        MouseState state = _state;

        if (auto e = event.is<App::MouseEvent>()) {
            if (not node.bound().contains(e->pos)) {
                state = IDLE;
            } else {
                if (state != PRESS) {
                    state = HOVER;
                }

                _pos = e->pos - node.bound().topStart();

                if (e->type == App::MouseEvent::PRESS and
                    e->button == App::MouseButton::LEFT) {
                    state = PRESS;
                    event.accept();

                } else if (e->type == App::MouseEvent::RELEASE and
                           e->button == App::MouseButton::LEFT) {
                    if (state == PRESS) {
                        state = HOVER;
                        result = true;
                        event.accept();
                    }
                }
            }
        } else if (auto e = event.is<App::MouseLeaveEvent>()) {
            state = IDLE;
        }

        if (state != _state) {
            _state = state;
            shouldRepaint(node);
        }

        return result;
    }
};

struct ButtonStyle {
    static constexpr isize RADIUS = 4;

    BoxStyle idleStyle{};
    BoxStyle hoverStyle{};
    BoxStyle pressStyle{};
    BoxStyle disabledStyle = {
        .foregroundFill = GRAY600,
    };

    static ButtonStyle none();

    static ButtonStyle regular(Gfx::ColorRamp ramp = GRAYS);

    static ButtonStyle secondary();

    static ButtonStyle primary();

    static ButtonStyle outline();

    static ButtonStyle subtle();

    static ButtonStyle text();

    static ButtonStyle destructive();

    ButtonStyle withRadii(Math::Radiif radii) const;

    ButtonStyle withForegroundFill(Gfx::Fill fill) const;

    ButtonStyle withPadding(Math::Insetsi insets) const;

    ButtonStyle withMargin(Math::Insetsi insets) const;
};

using OnPress = Opt<Func<void(Node&)>>;

static inline auto NOP(Ui::Node&) {}

Child button(OnPress onPress, ButtonStyle style, Child child);

inline auto button(OnPress onPress, ButtonStyle style) {
    return [onPress = std::move(onPress), style](Child child) mutable {
        return button(std::move(onPress), style, child);
    };
}

Child button(OnPress onPress, ButtonStyle style, Str t);

Child button(OnPress onPress, ButtonStyle style, Gfx::Icon i);

Child button(OnPress onPress, ButtonStyle style, Gfx::Icon i, Str t);

Child button(OnPress onPress, Child child);

inline auto button(OnPress onPress) {
    return [onPress = std::move(onPress)](Child child) mutable {
        return button(std::move(onPress), child);
    };
}

Child button(OnPress onPress, Str t);

Child button(OnPress onPress, Mdi::Icon i);

Child button(OnPress onPress, Mdi::Icon i, Str t);

// MARK: Input -----------------------------------------------------------------

Child input(Text::ProseStyle style, Rc<Text::Model> text, OnChange<Text::Action> onChange);

Child input(Text::ProseStyle, String text, OnChange<String> onChange);

Child input(Rc<Text::Model> text, OnChange<Text::Action> onChange);

// MARK: Slider ----------------------------------------------------------------

Child slider(f64 value, OnChange<f64> onChange, Child child);

static inline auto slider(f64 value, OnChange<f64> onChange) {
    return [value, onChange = std::move(onChange)](Child child) mutable {
        return slider(value, std::move(onChange), std::move(child));
    };
}

// MARK: Intent ----------------------------------------------------------------

using Filter = Func<void(Node&, App::Event& e)>;

Child intent(Filter map, Child child);

static inline auto intent(Filter filter) {
    return [filter = std::move(filter)](Child child) mutable {
        return intent(std::move(filter), std::move(child));
    };
}

} // namespace Karm::Ui
