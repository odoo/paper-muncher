export module Vaev.Browser:app;

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
import :inspect;
import :dialogs;
import :model;

using namespace Karm::Literals;
using namespace Karm::Ref::Literals;

namespace Vaev::Browser {

#ifdef __ck_host__

Ui::Child openInDefaultBrowser(State const& s) {
    return Kr::contextMenuItem(
        [&](auto& n) {
            auto url = s.currentUrl().url;
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
                .objects = {s.currentUrl().url},
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

Ui::Child mainMenu([[maybe_unused]] State const& s) {
    return Kr::contextMenuContent({
        Kr::contextMenuItem(
            Ui::SINK<>,
            Mdi::BOOKMARK_OUTLINE, "Add bookmark..."
        ),
        Kr::separator(),
        Kr::contextMenuItem(
            not s.loadingResult
                ? Opt<Ui::Send<>>{NONE}
                : [window = s.window](auto& n) {
                      Ui::showDialog(
                          n,
                          View::printDialog(window)
                      );
                  },
            Mdi::PRINTER, "Print..."
        ),
#ifdef __ck_host__
        openInDefaultBrowser(s),
#endif
        Kr::separator(),
        Kr::contextMenuItem(Model::bind<ToggleDeveloperMode>(), Mdi::CODE_TAGS, "Developer Tools"),
        Kr::contextMenuCheck(Model::bind<ToggleWireframe>(), s.wireframe, "Show wireframe"),
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

Ui::Child reloadButton(State const& s) {
    return (
               s.status == Status::LOADING
                   ? Kr::progress()
                   : Ui::icon(Mdi::REFRESH)
           ) |
           Ui::insets(6) |
           Ui::center() |
           Ui::minSize({32, 32}) |
           Ui::button(Model::bind<Reload>(), Ui::ButtonStyle::subtle());
}

Ui::Child addressBar(State const& s) {
    return Ui::hflow(
               Ui::button(
                   [&](auto& n) {
                       Ui::showPopover(n, n.bound().bottomStart(), addressMenu());
                   },
                   Ui::ButtonStyle::subtle(), Mdi::TUNE_VARIANT
               ),
               Ui::input(Ui::TextStyles::labelMedium(), s.locationInput, Model::map<UpdateLocation>()) |
                   Ui::vcenter() |
                   Ui::hscroll() |
                   Ui::grow()
           ) |
           Ui::box({
               .padding = {0, 0, 0, 0},
               .borderRadii = 4,
               .backgroundFill = Ui::GRAY800,
           }) |
           Ui::keyboardShortcut(App::Key::ENTER, Model::bind<NavigateLocation>()) |
           Ui::focusable() |
           Ui::keyboardShortcut(App::Key::L, App::KeyMod::CTRL);
}

Ui::Child contextMenu(State const& s) {
    return Kr::contextMenuContent({
        Kr::contextMenuDock({
            Kr::contextMenuIcon(Model::bindIf<GoBack>(s.canGoBack()), Mdi::ARROW_LEFT),
            Kr::contextMenuIcon(Model::bindIf<GoForward>(s.canGoForward()), Mdi::ARROW_RIGHT),
            Kr::contextMenuIcon(Model::bind<Reload>(), Mdi::REFRESH),
        }),
        Kr::separator(),
        Kr::contextMenuItem(
            Model::bind<Navigate>(
                s.currentUrl().url,
                Ref::Uti::PUBLIC_MODIFY
            ),
            Mdi::CODE_TAGS,
            "View Source..."
        ),
        Kr::contextMenuItem(
            Model::bind<ToggleDeveloperMode>(),
            Mdi::BUTTON_CURSOR,
            "Inspect"
        ),
    });
}

Ui::Child inspectorContent(State const& s) {
    if (not s.loadingResult) {
        return Ui::labelMedium(Ui::GRAY500, "No document") |
               Ui::center() | Kr::scaffoldContent();
    }

    return inspect(
        s.window,
        s.inspect,
        [&](auto& n, auto a) {
            Model::bubble(n, a);
        }
    );
}

Ui::Child alert(State const& s, String title, String body) {
    return Kr::errorPageContent({
        Kr::errorPageTitle(Mdi::GOOGLE_DOWNASAUR, title),
        Kr::errorPageBody(body),
        Kr::errorPageFooter({
            Ui::button(Model::bindIf<GoBack>(s.canGoBack()), "Go Back"),
            Ui::button(Model::bind<Reload>(), Ui::ButtonStyle::primary(), "Reload"),
        }),
    });
}

Ui::Child webview(State const& s) {
    if (not s.loadingResult)
        return alert(s, "The page could not be loaded"s, Io::toStr(s.loadingResult));

    Opt<Dom::OriginatingElement> selected = NONE;
    if (s.inspect.selectedNode) {
        if (auto it = s.inspect.selectedNode->is<Dom::Element>())
            selected = Dom::OriginatingElement{Gc::Ref(*const_cast<Dom::Element*>(it.upgrade()._ptr))};
    }

    return View::viewport(
               s.window,
               {
                   .wireframe = s.wireframe,
                   .selected = selected,
               }
           ) |
           Ui::box({
               .backgroundFill = Gfx::WHITE,
           }) |
           Kr::contextMenu([&] {
               return contextMenu(s);
           });
}

Ui::Child appContent(State const& s) {
    auto wv = webview(s) | Kr::scaffoldContent();
    if (not s.developerMode)
        return wv;

    return Ui::hflow(
        wv |
            Ui::grow(),
        inspectorContent(s) | Kr::resizable(Kr::ResizeHandlePosition::START, {320}, NONE)
    );
}

export Ui::Child app(State state) {
    return Ui::reducer<Model>(
        std::move(state),
        [](State const& s) {
            auto scaffold = Kr::scaffold({
                .icon = Mdi::SURFING,
                .title = "Browser"s,
                .startTools = [&] -> Ui::Children {
                    return {
                        Ui::button(Model::bindIf<GoBack>(s.canGoBack()), Ui::ButtonStyle::subtle(), Mdi::ARROW_LEFT) | Ui::keyboardShortcut(App::Key::LEFT, App::KeyMod::ALT, Model::bind<GoBack>()) |
                            Ui::keyboardShortcut(App::Key::LEFT, App::KeyMod::ALT),
                        Ui::button(Model::bindIf<GoForward>(s.canGoForward()), Ui::ButtonStyle::subtle(), Mdi::ARROW_RIGHT) |
                            Ui::keyboardShortcut(App::Key::RIGHT, App::KeyMod::ALT),
                        reloadButton(s),
                    };
                },
                .middleTools = [&] -> Ui::Children {
                    return {
                        Ui::empty(36),
                        addressBar(s) | Ui::grow(),
                        Ui::empty(36),
                    };
                },
                .endTools = [&] -> Ui::Children {
                    return {
                        Ui::button(
                            [&](Ui::Node& n) {
                                Ui::showPopover(n, n.bound().bottomEnd(), mainMenu(s));
                            },
                            Ui::ButtonStyle::subtle(),
                            Mdi::DOTS_HORIZONTAL
                        ),
                    };
                },
                .body = [&] {
                    return appContent(s);
                },
                .size = {1024, 768},
            });
            return scaffold |
                   Ui::keyboardShortcut(App::Key::R, App::KeyMod::CTRL, Model::bind<Reload>()) |
                   Ui::keyboardShortcut(App::Key::F5, Model::bind<Reload>()) |
                   Ui::keyboardShortcut(App::Key::F12, Model::bind<ToggleDeveloperMode>()) |
                   Ui::keyboardShortcut(App::Key::P, App::KeyMod::CTRL, [&](auto& n) {
                       if (not s.loadingResult)
                           return;

                       Ui::showDialog(
                           n,
                           View::printDialog(s.window)
                       );
                   });
        }
    );
}

} // namespace Vaev::Browser
