export module Vaev.Browser:model;

import Karm.Ref;
import Vaev.Engine;
import :inspect;

using namespace Karm::Literals;
using namespace Karm::Ref::Literals;

namespace Vaev::Browser {

struct Navigate {
    Ref::Url url;
    Ref::Uti action = Ref::Uti::PUBLIC_OPEN;
};

enum struct Status {
    LOADING,
    LOADED,
};

struct State {
    Rc<Dom::Window> window;
    bool developerMode = false;
    Status status = Status::LOADED;
    Res<> loadingResult = Ok();
    usize currentIndex = 0;
    Vec<Navigate> history = {};
    InspectState inspect = {};
    bool wireframe = false;
    String locationInput;

    State(Rc<Dom::Window> window, bool developerMode)
        : window{window},
          developerMode{developerMode},
          history{Navigate{window->location()}},
          locationInput(window->location().str()) {}

    bool canGoBack() const {
        return currentIndex > 0;
    }

    bool canGoForward() const {
        return currentIndex < history.len() - 1;
    }

    Navigate const& currentUrl() const {
        return history[currentIndex];
    }
};

struct Reload {};

struct Loaded {
    Res<> result;
};

struct GoBack {};

struct GoForward {};

struct ToggleWireframe {};

struct ToggleDeveloperMode {};

struct UpdateLocation {
    String location;
};

struct NavigateLocation {};

using Action = Union<
    Reload,
    Loaded,
    GoBack,
    GoForward,
    ToggleWireframe,
    ToggleDeveloperMode,
    InspectorAction,
    Navigate,
    UpdateLocation,
    NavigateLocation>;

Async::_Task<Opt<Action>> navigateAsync(Rc<Dom::Window> window, Navigate nav, Async::CancellationToken ct) {
    co_return Loaded{(co_await window->loadLocationAsync(nav.url, nav.action, ct))};
}

Ui::Task<Action> reduce(State& s, Action a) {
    return a.visit(
        [&](Reload) -> Ui::Task<Action> {
            if (s.status == Status::LOADING)
                return NONE;
            s.status = Status::LOADING;
            return navigateAsync(s.window, s.currentUrl(), Async::CancellationToken::uninterruptible());
        },
        [&](Loaded l) -> Ui::Task<Action> {
            s.status = Status::LOADED;
            s.loadingResult = l.result;
            s.locationInput = s.currentUrl().url.str();
            return NONE;
        },
        [&](GoBack) -> Ui::Task<Action> {
            s.currentIndex--;
            return reduce(s, Reload{});
        },
        [&](GoForward) -> Ui::Task<Action> {
            s.currentIndex++;
            return reduce(s, Reload{});
        },
        [&](ToggleWireframe) -> Ui::Task<Action> {
            s.wireframe = not s.wireframe;
            return NONE;
        },
        [&](ToggleDeveloperMode) -> Ui::Task<Action> {
            s.developerMode = not s.developerMode;
            return NONE;
        },
        [&](InspectorAction a) -> Ui::Task<Action> {
            s.inspect.apply(a);
            return NONE;
        },
        [&](Navigate n) -> Ui::Task<Action> {
            s.history.trunc(s.currentIndex + 1);
            s.history.pushBack(n);
            s.currentIndex++;
            return reduce(s, Reload{});
        },
        [&](UpdateLocation u) -> Ui::Task<Action> {
            s.locationInput = u.location;
            return NONE;
        },
        [&](NavigateLocation) -> Ui::Task<Action> {
            return reduce(s, Navigate{Ref::Url::parse(s.locationInput)});
        }
    );

    return NONE;
}

using Model = Ui::Model<State, Action, reduce>;

} // namespace Vaev::Browser
