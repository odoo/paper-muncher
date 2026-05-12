export module Hideo.Browser:app;

import Mdi;
import Karm.App;
import Karm.Gc;
import Karm.Http;
import Karm.Kira;
import Karm.Ref;
import Karm.Sys;
import Karm.Ui;
import Karm.Gfx;
import Vaev.Engine;
import Vaev.View;
import :dialogs;
import :model;

using namespace Karm::Literals;
using namespace Karm::Ref::Literals;

namespace Vaev::Browser {

#ifdef __ck_host__

Ui::Child openInDefaultBrowser(TabState const& s) {
    return Kr::contextMenuItem(
        [&](auto& n) {
            auto url = s.history.current().url;
            if (url.scheme != "http" and url.scheme != "https" and url.scheme != "file") {
                Ui::showDialog(
                    n,
                    Kr::alertDialog(
                        "Could not open in default browser"s,
                        Io::format("Only http, https, and file urls can be opened in the default browser.")
                    )
                );
                return;
            }

            auto res = Sys::launch({
                .action = Ref::Uti::PUBLIC_OPEN,
                .objects = {s.history.current().url},
            });

            if (not res)
                Ui::showDialog(
                    n,
                    Kr::alertDialog(
                        "Could not open in default browser"s,
                        Io::format("Failed to open in default browser\n\n{}", res)
                    )
                );
        },
        Mdi::WEB, "Open in default browser..."
    );
}

#endif

Ui::Child mainMenu([[maybe_unused]] TabState const& s, Ui::Send<TabAction> send) {
    Opt<Ui::Send<>> printAction = NONE;
    if (s.loadingResult)
        printAction = [window = s.window](auto& n) {
            Ui::showDialog(n, View::printDialog(window));
        };

    return Kr::contextMenuContent({
        Kr::contextMenuItem(
            Ui::SINK<>,
            Mdi::BOOKMARK_OUTLINE, "Add bookmark..."
        ),
        Kr::separator(),
        Kr::contextMenuItem(
            printAction,
            Mdi::PRINTER, "Print..."
        ),
#ifdef __ck_host__
        openInDefaultBrowser(s),
#endif
        Kr::separator(),
        Kr::contextMenuItem(rbind(send, ToggleDeveloperMode{}), Mdi::CODE_TAGS, "Developer Tools"),
        Kr::contextMenuCheck(rbind(send, ToggleWireframe{}), s.inspect.wireframe, "Show wireframe"),
        Kr::separator(),
        Kr::contextMenuItem(Ui::SINK<>, Mdi::COG, "Settings"),
    });
}

Ui::Child addressMenu() {
    return Kr::contextMenuContent({
        Kr::contextMenuDock({
            Ui::labelMedium("Your are viewing a secure page") | Ui::insets(8),
            Kr::contextMenuIcon(Ui::SINK<>, Mdi::CLOSE),
        }),
    });
}

Ui::Child reloadButton(TabState const& s, Ui::Send<TabAction> send) {
    return (
               s.status == Status::LOADING
                   ? Kr::progress()
                   : Ui::icon(Mdi::REFRESH)
           ) |
           Ui::insets(6) |
           Ui::center() |
           Ui::minSize({32, 32}) |
           Ui::button(rbind(send, Reload{}), Ui::ButtonStyle::subtle());
}

Ui::Child addressBar(TabState const& s, Ui::Send<TabAction> send) {
    Ui::Send<> showPopoverAction = [&](auto& n) {
        Ui::showPopover(n, n.bound().bottomStart(), addressMenu());
    };

    Ui::Send<String> updateLocationAction = [send](auto& n, String text) {
        send(n, UpdateLocation{text});
    };

    return Ui::hflow(
               Ui::button(
                   showPopoverAction,
                   Ui::ButtonStyle::subtle(), Mdi::TUNE_VARIANT
               ),
               Ui::input(
                   Ui::TextStyles::labelMedium(),
                   s.locationInput,
                   updateLocationAction
               ) | Ui::vcenter() |
                   Ui::hscroll() | Ui::grow()
           ) |
           Ui::box({
               .padding = {0, 0, 0, 0},
               .borderRadii = 4,
               .backgroundFill = Ui::GRAY800,
           }) |
           Ui::keyboardShortcut(
               App::Key::ENTER,
               rbind(send, NavigateLocation{})
           ) |
           Ui::focusable() |
           Ui::keyboardShortcut(App::Key::L, App::KeyMod::CTRL);
}

Ui::Child contextMenu(TabState const& s, Ui::Send<TabAction> send) {
    Opt<Ui::Send<>> goBackAction = NONE;
    if (s.history.canGoBack())
        goBackAction = rbind(GoBack{});

    Opt<Ui::Send<>> goForwardAction = NONE;
    if (s.history.canGoForward())
        goForwardAction = rbind(GoForward{});

    Ui::Send<> viewSourceAction = rbind(
        send,
        Navigate{s.history.current().url, Ref::Uti::PUBLIC_MODIFY}
    );

    return Kr::contextMenuContent({
        Kr::contextMenuDock({
            Kr::contextMenuIcon(goBackAction, Mdi::ARROW_LEFT),
            Kr::contextMenuIcon(goForwardAction, Mdi::ARROW_RIGHT),
            Kr::contextMenuIcon(rbind(send, Reload{}), Mdi::REFRESH),
        }),
        Kr::separator(),
        Kr::contextMenuItem(
            viewSourceAction,
            Mdi::CODE_TAGS,
            "View Source..."
        ),
        Kr::contextMenuItem(
            rbind(send, ToggleDeveloperMode{}),
            Mdi::BUTTON_CURSOR,
            "Inspect"
        ),
    });
}

Ui::Child inspectorContent(TabState const& s, Ui::Send<TabAction> send) {
    if (not s.loadingResult) {
        return Ui::labelMedium(Ui::GRAY500, "No document") |
               Ui::center();
    }

    return Vaev::Browser::inspect(
        s.window,
        s.inspect,
        [send](auto& n, auto a) {
            send(n, a);
        }
    );
}

Ui::Child bookmarkSidePanel(State const& s) {
    if (not s.bookmarks) {
        return Ui::labelMedium(Ui::GRAY500, "No bookmarks") |
               Ui::center();
    }

    Ui::Children children;
    for (auto& bm : s.bookmarks) {
        children.pushBack(
            Ui::button(
                Model::bind<Navigate>(bm.url),
                Mdi::BOOKMARK,
                bm.name
            )
        );
    }

    return Ui::vflow(
               4,
               std::move(children)
           ) |
           Ui::vscroll();
}

Ui::Child inspectorSidePanel(TabState const& s, Ui::Send<TabAction> send) {
    return Kr::sidePanelContent({
        Kr::sidePanelTitle(Model::bind<ToggleDeveloperMode>(), "Developer Tools"),
        Kr::separator(),
        inspectorContent(s, send) | Ui::grow(),
    });
}

Ui::Child alert(TabState const& s, String title, String body, Ui::Send<TabAction> send) {
    return Kr::errorPageContent({
        Kr::errorPageTitle(Mdi::GOOGLE_DOWNASAUR, title),
        Kr::errorPageBody(body),
        Kr::errorPageFooter({
            Ui::button(Model::bindIf<GoBack>(s.history.canGoBack()), "Go Back"),
            Ui::button(Model::bind<Reload>(), Ui::ButtonStyle::primary(), "Reload"),
        }),
    });
}

Ui::Child webview(TabState const& s, Ui::Send<TabAction> send) {
    if (not s.loadingResult)
        return alert(s, "The page could not be loaded"s, Io::toStr(s.loadingResult), send);

    Opt<Dom::OriginatingElement> selected = NONE;
    if (s.inspect.selectedNode) {
        if (auto it = s.inspect.selectedNode->is<Dom::Element>())
            selected = Dom::OriginatingElement{Gc::Ref(*const_cast<Dom::Element*>(it.upgrade()._ptr))};
    }

    return View::viewport(
               s.window,
               {
                   .wireframe = s.inspect.wireframe,
                   .selected = selected,
               }
           ) |
           Ui::box({
               .backgroundFill = Gfx::WHITE,
           }) |
           Kr::contextMenu([&, send] {
               return contextMenu(s, send);
           });
}

Ui::Child appContent(TabState const& s, Ui::Send<TabAction> send) {
    auto wv = webview(s, send) | Kr::scaffoldContent();

    if (not s.developerMode)
        return wv;

    return Ui::hflow(
        wv | Ui::grow(),
        inspectorSidePanel(s, send) |
            Kr::resizable(Kr::ResizeHandle::START, {320}, NONE) |
            Kr::scaffoldContent()
    );
}

export Ui::Child app(Rc<Dom::Window> window) {
    State state = window;
    state.bookmarks.pushBack({.name = "smnx.sh"s, .url = "http://smnx.sh"_url});
    state.bookmarks.pushBack({.name = "The Project (snapshot)"s, .url = "bundle://vaev-browser.main/www-the-project.html"_url});
    state.bookmarks.pushBack({.name = "Google (snapshot)"s, .url = "bundle://vaev-browser.main/google.html"_url});
    state.bookmarks.pushBack({.name = "Hackernews (snapshot)"s, .url = "bundle://vaev-browser.main/hackernews.html"_url});

    return Ui::reducer<Model>(
        std::move(state),
        [](State const& s) {
            auto scaffold = Kr::scaffold({
                .icon = Mdi::SURFING,
                .title = "Browser"s,
                .startTools = [&] -> Ui::Children {
                    return {
                        Ui::button(
                            Model::bindIf<GoBack>(s.tab.history.canGoBack()),
                            Ui::ButtonStyle::subtle(),
                            Mdi::ARROW_LEFT
                        ) | Ui::keyboardShortcut(App::Key::LEFT, App::KeyMod::ALT, Model::bind<GoBack>()) |
                            Ui::keyboardShortcut(App::Key::LEFT, App::KeyMod::ALT),
                        Ui::button(
                            Model::bindIf<GoForward>(
                                s.tab.history.canGoForward()
                            ),
                            Ui::ButtonStyle::subtle(), Mdi::ARROW_RIGHT
                        ) |
                            Ui::keyboardShortcut(App::Key::RIGHT, App::KeyMod::ALT),
                        reloadButton(s.tab, Model::map<TabAction>()),
                    };
                },
                .middleTools = [&] -> Ui::Children {
                    return {addressBar(s.tab, Model::map<TabAction>()) | Ui::grow()};
                },
                .endTools = [&] -> Ui::Children {
                    return {
                        Ui::button(
                            [&](Ui::Node& n) {
                                Ui::showPopover(n, n.bound().bottomEnd(), mainMenu(s.tab, Model::map<TabAction>()));
                            },
                            Ui::ButtonStyle::subtle(),
                            Mdi::DOTS_HORIZONTAL
                        ),
                    };
                },
                .sidebar = [&] {
                    return Kr::sidenavContent({bookmarkSidePanel(s)});
                },
                .body = [&] {
                    return appContent(s.tab, Model::map<TabAction>());
                },
            });
            return scaffold |
                   Ui::keyboardShortcut(App::Key::R, App::KeyMod::CTRL, Model::bind<Reload>()) |
                   Ui::keyboardShortcut(App::Key::F5, Model::bind<Reload>()) |
                   Ui::keyboardShortcut(App::Key::F12, Model::bind<ToggleDeveloperMode>()) |
                   Ui::keyboardShortcut(App::Key::P, App::KeyMod::CTRL, [&](auto& n) {
                       if (not s.tab.loadingResult)
                           return;
                       Ui::showDialog(
                           n,
                           View::printDialog(s.tab.window)
                       );
                   });
        }
    );
}

} // namespace Vaev::Browser
