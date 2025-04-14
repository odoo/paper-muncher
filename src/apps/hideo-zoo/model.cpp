module;

#include <karm-gfx/icon.h>
#include <karm-ui/reducer.h>

export module Hideo.Zoo:model;

namespace Hideo::Zoo {

struct Page {
    Gfx::Icon icon;
    Str name;
    Str description;
    Func<Ui::Child()> build;
};

struct State {
    usize page;
};

struct Switch {
    usize page;
};

export using Action = Union<Switch>;

Ui::Task<Action> reduce(State& s, Action a) {
    a.visit(
        [&](Switch action) {
            s.page = action.page;
        }
    );

    return NONE;
}

export using Model = Ui::Model<State, Action, reduce>;

} // namespace Hideo::Zoo
