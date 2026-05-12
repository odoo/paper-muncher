export module Hideo.Browser:model;

import Karm.Core;
import Karm.Ref;
import Vaev.Engine;
import :inspect;

using namespace Karm;
using namespace Karm::Literals;
using namespace Karm::Ref::Literals;

namespace Vaev::Browser {

// MARK: Tab State -------------------------------------------------------------

struct Navigate {
    Ref::Url url;
    Ref::Uti action = Ref::Uti::PUBLIC_OPEN;
};

struct History {
    Vec<Navigate> navigables;
    usize currentIndex = 0;

    History(Ref::Url url)
        : navigables{Navigate{url}} {}

    bool canGoBack() const {
        return currentIndex > 0;
    }

    bool canGoForward() const {
        return currentIndex < navigables.len() - 1;
    }

    Navigate const& current() const {
        return navigables[currentIndex];
    }
};

enum struct Status {
    LOADING,
    LOADED,
};

struct Bookmark {
    String name;
    Ref::Url url;
};

struct TabState {
    Status status = Status::LOADED;
    Res<> loadingResult = Ok();
    Rc<Dom::Window> window;
    History history;
    bool developerMode = false;
    InspectState inspect = {};
    String locationInput;

    TabState(Rc<Dom::Window> window)
        : window(window),
          history(window->location()),
          locationInput(window->location().str()) {}
};

struct Reload {};

struct Loaded {
    Res<> result;
};

struct GoBack {};

struct GoForward {};

struct UpdateLocation {
    String location;
};

struct ToggleDeveloperMode {};

struct NavigateLocation {};

using TabAction = Union<
    Reload,
    Loaded,
    GoBack,
    GoForward,
    Navigate,

    ToggleDeveloperMode,
    InspectorAction,

    UpdateLocation,
    NavigateLocation>;

Async::_Task<Opt<TabAction>> navigateAsync(Rc<Dom::Window> window, Navigate nav, Async::CancellationToken ct) {
    co_return Loaded{(co_await window->loadLocationAsync(nav.url, nav.action, ct))};
}

Ui::Task<TabAction> reduce(TabState& s, TabAction a) {
    return a.visit(
        [&](Reload) -> Ui::Task<TabAction> {
            if (s.status == Status::LOADING)
                return NONE;
            s.status = Status::LOADING;
            return navigateAsync(s.window, s.history.current(), Async::CancellationToken::uninterruptible());
        },
        [&](Loaded l) -> Ui::Task<TabAction> {
            s.status = Status::LOADED;
            s.loadingResult = l.result;
            s.locationInput = s.history.current().url.str();
            return NONE;
        },
        [&](GoBack) -> Ui::Task<TabAction> {
            s.history.currentIndex--;
            return reduce(s, Reload{});
        },
        [&](GoForward) -> Ui::Task<TabAction> {
            s.history.currentIndex++;
            return reduce(s, Reload{});
        },
        [&](Navigate n) -> Ui::Task<TabAction> {
            s.history.navigables.trunc(s.history.currentIndex + 1);
            s.history.navigables.pushBack(n);
            s.history.currentIndex++;
            return reduce(s, Reload{});
        },

        [&](ToggleDeveloperMode) -> Ui::Task<TabAction> {
            s.developerMode = not s.developerMode;
            return NONE;
        },
        [&](InspectorAction a) -> Ui::Task<TabAction> {
            s.inspect.apply(a);
            return NONE;
        },

        [&](UpdateLocation u) -> Ui::Task<TabAction> {
            s.locationInput = u.location;
            return NONE;
        },
        [&](NavigateLocation) -> Ui::Task<TabAction> {
            return reduce(s, Navigate{Ref::Url::parse(s.locationInput)});
        }
    );
}

// MARK: State -----------------------------------------------------------------

struct State {
    TabState tab;
    Vec<Bookmark> bookmarks;

    State(Rc<Dom::Window> window)
        : tab(window) {}
};

using Action = Union<
    TabAction>;

Ui::Task<Action> reduce(State& s, Action a) {
    return a.visit(
        [&](TabAction a) -> Ui::Task<Action> {
            auto task = reduce(s.tab, a);
            if (task) {
                return [t = task.take()]() mutable -> Async::_Task<Opt<Action>> {
                    co_return Opt<Action>{co_await std::move(t)};
                }();
            }
            return Ui::Task<Action>{NONE};
        }
    );
}

using Model = Ui::Model<State, Action, reduce>;

} // namespace Vaev::Browser