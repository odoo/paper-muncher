module;

#include <karm-print/page.h>
#include <karm-print/printer.h>
#include <karm-ui/dialog.h>
#include <karm-ui/focus.h>
#include <karm-ui/input.h>
#include <karm-ui/layout.h>
#include <karm-ui/reducer.h>
#include <karm-ui/scroll.h>
#include <mdi/account-circle.h>
#include <mdi/account.h>
#include <mdi/alarm.h>
#include <mdi/alert.h>
#include <mdi/card.h>
#include <mdi/cat.h>
#include <mdi/checkbox-marked.h>
#include <mdi/clock-outline.h>
#include <mdi/clock.h>
#include <mdi/content-copy.h>
#include <mdi/content-cut.h>
#include <mdi/content-paste.h>
#include <mdi/counter.h>
#include <mdi/cursor-text.h>
#include <mdi/delete.h>
#include <mdi/dock-bottom.h>
#include <mdi/dock-left.h>
#include <mdi/dock-right.h>
#include <mdi/dock-top.h>
#include <mdi/duck.h>
#include <mdi/email.h>
#include <mdi/file-search.h>
#include <mdi/form-select.h>
#include <mdi/form-textbox.h>
#include <mdi/format-list-bulleted-type.h>
#include <mdi/gradient-horizontal.h>
#include <mdi/information-outline.h>
#include <mdi/information.h>
#include <mdi/loading.h>
#include <mdi/lock.h>
#include <mdi/menu.h>
#include <mdi/open-in-app.h>
#include <mdi/palette.h>
#include <mdi/pencil.h>
#include <mdi/printer.h>
#include <mdi/radiobox-marked.h>
#include <mdi/rectangle.h>
#include <mdi/share.h>
#include <mdi/target.h>
#include <mdi/text-box.h>
#include <mdi/timer-outline.h>
#include <mdi/timer-sand.h>
#include <mdi/toggle-switch-outline.h>
#include <mdi/toggle-switch.h>
#include <mdi/tree.h>
#include <mdi/tune-variant.h>
#include <mdi/view-compact.h>

export module Hideo.Zoo:pages;

import Karm.Kira;
import Hideo.Files;
import Hideo.Printers;
import :model;

namespace Hideo::Zoo {

export Page PAGE_ALERT{
    Mdi::ALERT,
    "Alert",
    "A modal dialog that interrupts the user with important content and expects a response.",
    [] {
        return Ui::button(
                   [](auto& n) {
                       Ui::showDialog(
                           n,
                           Kr::dialogContent({
                               Kr::dialogHeader({
                                   Kr::dialogTitle("Are you absolutely sure?"s),
                                   Kr::dialogDescription("This action cannot be undone. This will permanently delete your account and remove your data from our servers."s),
                               }),
                               Kr::dialogFooter({
                                   Kr::dialogCancel(),
                                   Kr::dialogAction(Ui::NOP, "Continue"s),
                               }),
                           })
                       );
                   },
                   "Show alert"
               ) |
               Ui::center();
    },
};

export Page PAGE_AVATAR{
    Mdi::ACCOUNT_CIRCLE,
    "Avatar",
    "An image element with a fallback for representing the user.",
    [] {
        return Ui::hflow(
                   16,
                   Kr::avatar(),
                   Kr::avatar("CV"s),
                   Kr::avatar(Mdi::CAT)
               ) |
               Ui::center();
    },
};

Page PAGE_BADGE{
    Mdi::CARD,
    "Badge",
    "Displays a badge or a component that looks like a badge.",
    [] {
        return Ui::vflow(
                   16,
                   Math::Align::CENTER,
                   Kr::badge(Kr::BadgeStyle::INFO, "Info"s),
                   Kr::badge(Kr::BadgeStyle::SUCCESS, "Success"s),
                   Kr::badge(Kr::BadgeStyle::WARNING, "Warning"s),
                   Kr::badge(Kr::BadgeStyle::ERROR, "Error"s),
                   Kr::badge(Gfx::GREEN, "New"s)
               ) |
               Ui::center();
    },
};

Page PAGE_CARD{
    Mdi::RECTANGLE,
    "Card",
    "A container that groups and styles its children.",
    [] {
        return Ui::labelMedium(
                   Ui::GRAY700,
                   "This is a card"
               ) |
               Ui::center() |
               Ui::pinSize({400, 300}) |
               Kr::card() | Ui::center();
    },
};

Page PAGE_CHECKBOX{
    Mdi::CHECKBOX_MARKED,
    "Checkbox",
    "A control that allows the user to toggle between checked and not checked.",
    [] {
        return Kr::checkbox(
                   true,
                   NONE
               ) |
               Ui::center();
    },
};

Page PAGE_CLOCK{
    Mdi::CLOCK,
    "Clock",
    "An analogue clock that display the time",
    [] {
        return Kr::clock({}) |
               Ui::pinSize({400, 300}) |
               Ui::center();
    },
};

Page PAGE_COLOR_INPUT{
    Mdi::PALETTE,
    "Color Input",
    "A control that allows the user to pick a color.",
    [] {
        return Kr::colorInput(
                   Gfx::RED,
                   NONE
               ) |
               Ui::center();
    },
};

Page PAGE_CONTEXT_MENU{
    Mdi::MENU,
    "Context Menu",
    "Displays a menu to the user — such as a set of actions or functions — triggered by a button.",
    [] {
        return Ui::labelLarge(Ui::GRAY500, "Right click here") |
               Ui::center() |
               Ui::bound() |
               Kr::contextMenu([] {
                   return Kr::contextMenuContent({
                       Kr::contextMenuItem(Ui::NOP, Mdi::OPEN_IN_APP, "Open"),
                       Kr::contextMenuItem(Ui::NOP, Mdi::PENCIL, "Edit"),
                       Ui::separator(),
                       Kr::contextMenuItem(Ui::NOP, Mdi::CONTENT_COPY, "Copy"),
                       Kr::contextMenuItem(Ui::NOP, Mdi::CONTENT_CUT, "Cut"),
                       Kr::contextMenuItem(Ui::NOP, Mdi::CONTENT_PASTE, "Paste"),
                       Ui::separator(),
                       Kr::contextMenuItem(Ui::NOP, Mdi::SHARE, "Interact…"),
                       Kr::contextMenuItem(Ui::NOP, Mdi::CURSOR_TEXT, "Rename…"),
                       Ui::separator(),
                       Kr::contextMenuItem(Ui::NOP, Mdi::DELETE, "Delete"),
                       Ui::separator(),
                       Kr::contextMenuItem(Ui::NOP, Mdi::INFORMATION_OUTLINE, "Properties…"),
                   });
               });
    },
};

Page PAGE_DIALOG{
    Mdi::INFORMATION,
    "Dialog",
    "A window overlaid on either the primary window or another dialog window, rendering the content underneath inert.",
    [] {
        return Ui::button(
                   [](auto& n) {
                       Ui::showDialog(
                           n,
                           Kr::dialogContent({
                               Kr::dialogTitleBar("Dialog"s),
                               Ui::labelLarge("Hello, world") | Ui::center() | Ui::pinSize({200, 160}),
                           })
                       );
                   },
                   "Show dialog"
               ) |
               Ui::center();
    },
};

Page PAGE_FILE_DIALOG{
    Mdi::FILE_SEARCH,
    "File Dialog",
    "A window overlaid on either the primary window or another dialog window, rendering the content underneath inert.",
    [] {
        return Ui::vflow(
                   6,
                   Ui::button(
                       [](auto& n) {
                           Ui::showDialog(
                               n,
                               Hideo::Files::openDialog([](auto&, auto url) {
                                   logInfo("selected file: {}", url);
                               })
                           );
                       },
                       "Open File..."
                   ),
                   Ui::button(
                       [](auto& n) {
                           Ui::showDialog(
                               n,
                               Hideo::Files::saveDialog([](auto&, auto url) {
                                   logInfo("selected file: {}", url);
                               })
                           );
                       },
                       "Save As..."
                   ),
                   Ui::button(
                       [](auto& n) {
                           Ui::showDialog(
                               n,
                               Hideo::Files::directoryDialog([](auto&, auto url) {
                                   logInfo("selected directory: {}", url);
                               })
                           );
                       },
                       "Open Directory..."
                   )
               ) |
               Ui::center();
    },
};

Page PAGE_FOCUS{
    Mdi::TARGET,
    "Focusable",
    "A control that can be focused by the user.",
    [] {
        return Ui::vflow(
                   8,
                   Ui::labelLarge("Apple") | Ui::focusable(),
                   Ui::labelLarge("Banana") | Ui::focusable(),
                   Ui::labelLarge("Cherry") | Ui::focusable()
               ) |
               Ui::center();
    },
};

Page PAGE_HSV_SQUARE{
    Mdi::GRADIENT_HORIZONTAL,
    "HSV Square",
    "A control that allows the user to pick a color using a square.",
    [] {
        return Kr::hsvSquare({}, Ui::IGNORE<Gfx::Hsv>) |
               Ui::box({
                   .borderWidth = 1,
                   .borderFill = Ui::GRAY800,
               }) |
               Ui::center();
    },
};

Page PAGE_INPUT{
    Mdi::FORM_TEXTBOX,
    "Input",
    "Displays a form input field or a component that looks like an input field.",
    [] {
        struct State {
            String username;
            String email;
            String password;
            String text;
        };

        struct UpdateUsername {
            String username;
        };

        struct UpdateEmail {
            String email;
        };

        struct UpdatePassword {
            String password;
        };

        struct UpdateText {
            String text;
        };

        using Action = Union<UpdateUsername, UpdateEmail, UpdatePassword, UpdateText>;

        auto reduce = [](State& s, Action a) -> Ui::Task<Action> {
            a.visit(
                Visitor{
                    [&](UpdateUsername& u) {
                        s.username = u.username;
                    },
                    [&](UpdateEmail& u) {
                        s.email = u.email;
                    },
                    [&](UpdatePassword& u) {
                        s.password = u.password;
                    },
                    [&](UpdateText& u) {
                        s.text = u.text;
                    }
                }
            );

            return NONE;
        };

        using Model = Ui::Model<State, Action, reduce>;

        return Ui::reducer<Model>(
            {},
            [](State const& s) {
                return Ui::vflow(
                           16,
                           Math::Align::CENTER,
                           Kr::input(Mdi::ACCOUNT, "Username"s, s.username, Model::map<UpdateUsername>()) | Ui::pinSize({240, Ui::UNCONSTRAINED}),
                           Kr::input(Mdi::EMAIL, "Email"s, s.email, Model::map<UpdateEmail>()) | Ui::pinSize({240, Ui::UNCONSTRAINED}),
                           Kr::input(Mdi::LOCK, "Password"s, s.password, Model::map<UpdatePassword>()) | Ui::pinSize({240, Ui::UNCONSTRAINED}),
                           Kr::input("Text"s, s.text, Model::map<UpdateText>()) | Ui::pinSize({240, Ui::UNCONSTRAINED}),
                           Ui::labelMedium(
                               "Your username is {} and your email is {}."s,
                               s.username ? s.username.str() : "unknown"s,
                               s.email ? s.email.str() : "unknown"s
                           )
                       ) |
                       Ui::center();
            }
        );
    },
};

Page PAGE_NAVBAR{
    Mdi::DOCK_BOTTOM,
    "Navigation Bar"s,
    "A horizontal navigation bar that displays a list of links.",
    [] {
        return Ui::state(0, [](auto state, auto bind) {
            return Ui::vflow(
                Ui::grow(NONE),
                Kr::navbarContent({
                    Kr::navbarItem(
                        bind(0),
                        Mdi::ALARM,
                        "Alarm",
                        state == 0
                    ),
                    Kr::navbarItem(
                        bind(1),
                        Mdi::CLOCK_OUTLINE,
                        "Clock",
                        state == 1
                    ),
                    Kr::navbarItem(
                        bind(2),
                        Mdi::TIMER_SAND,
                        "Timer",
                        state == 2
                    ),
                    Kr::navbarItem(
                        bind(3),
                        Mdi::TIMER_OUTLINE,
                        "Stopwatch",
                        state == 3
                    ),
                })
            );
        });
    },
};

Page PAGE_NUMBER{
    Mdi::COUNTER,
    "Number",
    "An input where the user selects a value by incrementing or decrementing.",
    [] {
        return Kr::number(
                   100, [](auto&, ...) {
                   },
                   10
               ) |
               Ui::center();
    },
};

Page PAGE_PRINT_DIALOG{
    Mdi::PRINTER,
    "Print dialog",
    "Prompts the user to print a document.",
    [] {
        return Ui::button(
                   [](auto& n) {
                       Ui::showDialog(
                           n,
                           Hideo::Printers::printDialog([](Print::Settings const& s) -> Vec<Print::Page> {
                               return {
                                   {s.paper},
                                   {s.paper},
                                   {s.paper},
                               };
                           })
                       );
                   },
                   "Show dialog"
               ) |
               Ui::center();
    },
};

Page PAGE_PROGRESS{
    Mdi::LOADING,
    "Progress",
    "A loading indicator that spins to indicate that the application is busy.",
    [] {
        return Ui::hflow(8, Kr::progress(12), Kr::progress(18), Kr::progress(24), Kr::progress(48)) |
               Ui::center();
    },
};

Page PAGE_RADIO{
    Mdi::RADIOBOX_MARKED,
    "Radio",
    "A set of checkable buttons—known as radio buttons—where no more than one of the buttons can be checked at a time.",
    [] {
        return Kr::radio(
                   true,
                   NONE
               ) |
               Ui::center();
    },
};

Page PAGE_RESIZABLE{
    Mdi::VIEW_COMPACT,
    "Resizable",
    "A control that allows the user to resize an element.",
    [] {
        return Ui::hflow(
            Ui::labelMedium("One") | Ui::center() | Kr::resizable(Kr::ResizeHandle::END, 200, NONE),
            Ui::vflow(
                Ui::labelMedium("Two") | Ui::center() | Kr::resizable(Kr::ResizeHandle::BOTTOM, 200, NONE),
                Ui::labelMedium("Three") | Ui::center() | Ui::grow()
            ) | Ui::grow()
        );
    },
};

Page PAGE_RICHTEXT{
    Mdi::TEXT_BOX,
    "Rich Text"s,
    "An area that displays text with various styles and formatting options.",
    [] {
        auto prose = makeRc<Text::Prose>(Ui::TextStyles::bodyMedium());

        prose->append("This is a simple text with no formatting.\n"s);

        prose->append("We can change color: "s);

        prose->pushSpan();
        prose->spanColor(Gfx::RED);
        prose->append("red"s);
        prose->popSpan();
        prose->append(", "s);

        prose->pushSpan();
        prose->spanColor(Gfx::GREEN);
        prose->append(" green"s);
        prose->popSpan();
        prose->append(", "s);

        prose->pushSpan();
        prose->spanColor(Gfx::BLUE);
        prose->append("blue"s);
        prose->popSpan();

        prose->append(".\n"s);

        return Ui::text(prose) | Ui::center();
    },
};


Page PAGE_ROWS{
    Mdi::FORMAT_LIST_BULLETED_TYPE,
    "Settings Rows",
    "A collection of rows that can be used to display settings.",
    [] {
        auto button = Kr::buttonRow(
            [](auto&n){
                Ui::showDialog(n, Kr::alertDialog("Message"s, "This is a message"s));
            },
            "Cool duck app"s,
            "Install"s
        );

        auto title = Kr::titleRow("Some Settings"s);

        auto list = Kr::card(
            button,
            Ui::separator(),
            Kr::treeRow(
                [&] -> Ui::Child {
                    return Ui::icon(Mdi::TOGGLE_SWITCH);
                },
                "Switches"s,
                NONE,
                Ui::Slots{[&] -> Ui::Children {
                    return {
                        Kr::toggleRow(true, NONE, "Some property"s),
                        Kr::toggleRow(true, NONE, "Some property"s),
                        Kr::toggleRow(true, NONE, "Some property"s),
                    };
                }}
            ),

            Ui::separator(),
            Kr::treeRow(
                [&] -> Ui::Child {
                    return Ui::icon(Mdi::CHECKBOX_MARKED);
                },
                "Checkboxs"s,
                NONE,
                Ui::Slots{[&] -> Ui::Children {
                    return {
                        Kr::checkboxRow(true, NONE, "Some property"s),
                        Kr::checkboxRow(false, NONE, "Some property"s),
                        Kr::checkboxRow(false, NONE, "Some property"s),
                    };
                }}
            ),

            Ui::separator(),
            Kr::treeRow(
                [&] -> Ui::Child {
                    return Ui::icon(Mdi::RADIOBOX_MARKED);
                },
                "Radios"s,
                NONE,
                Ui::Slots{[&] -> Ui::Children {
                    return {
                        Kr::radioRow(true, NONE, "Some property"s),
                        Kr::radioRow(false, NONE, "Some property"s),
                        Kr::radioRow(false, NONE, "Some property"s)
                    };
                }}
            ),
            Ui::separator(),
            Kr::sliderRow(
                0.5,
                NONE,
                "Some property"s
            )
        );

        return Ui::vflow(8, title, list) |
               Ui::maxSize({420, Ui::UNCONSTRAINED}) |
               Ui::grow() |
               Ui::hcenter() |
               Ui::vscroll();
    },
};

Page PAGE_SELECT{
    Mdi::FORM_SELECT,
    "Select",
    "A control that allows the user to toggle between checked and not checked.",
    [] {
        return Kr::select(
                   Kr::selectValue("Pick something to eat"s),
                   [] -> Ui::Children {
                       return {
                           Kr::selectGroup({
                               Kr::selectLabel("Fruits"s),
                               Kr::selectItem(Ui::NOP, "Apple"s),
                               Kr::selectItem(Ui::NOP, "Banana"s),
                               Kr::selectItem(Ui::NOP, "Cherry"s),
                           }),
                           Ui::separator(),
                           Kr::selectGroup({
                               Kr::selectLabel("Vegetables"s),
                               Kr::selectItem(Ui::NOP, "Carrot"s),
                               Kr::selectItem(Ui::NOP, "Cucumber"s),
                               Kr::selectItem(Ui::NOP, "Tomato"s),
                           }),
                           Ui::separator(),
                           Kr::selectGroup({
                               Kr::selectLabel("Meat"s),
                               Kr::selectItem(Ui::NOP, "Beef"s),
                               Kr::selectItem(Ui::NOP, "Chicken"s),
                               Kr::selectItem(Ui::NOP, "Pork"s),
                           }),
                       };
                   }
               ) |
               Ui::center();
    },
};

Page PAGE_SIDENAV{
    Mdi::DOCK_LEFT,
    "Side Navigation"s,
    "A vertical list of links that can be toggled open and closed.",
    [] {
        return Ui::hflow(
            Kr::sidenav({
                Kr::sidenavTitle("Navigation"s),
                Kr::sidenavItem(true, Ui::NOP, Mdi::DUCK, "Item 1"s),
                Kr::sidenavTree(Mdi::TREE, "Item 2"s, [] {
                    return Ui::vflow(
                        8,
                        Kr::sidenavItem(false, Ui::NOP, Mdi::DUCK, "Subitem 1"s),
                        Kr::sidenavItem(false, Ui::NOP, Mdi::DUCK, "Subitem 2"s),
                        Kr::sidenavItem(false, Ui::NOP, Mdi::DUCK, "Subitem 3"s)
                    );
                }),
                Kr::sidenavItem(false, Ui::NOP, Mdi::DUCK, "Item 3"s),
            }),
            Ui::separator()
        );
    },
};

Page PAGE_SIDE_PANEL{
    Mdi::DOCK_RIGHT,
    "Side Panel",
    "A panel that slides in from the side of the screen to display aditional information or properties",
    [] {
        return Ui::state(false, [](auto state, auto bind) {
            auto content = Ui::button(bind(!state), "Toggle Side Panel") |
                           Ui::center() |
                           Ui::grow();

            if (not state) {
                return content;
            }

            return Ui::hflow(
                content,
                Ui::separator(),
                Kr::sidePanelContent({
                    Kr::sidePanelTitle(bind(false), "Side Panel"),
                    Ui::separator(),
                    Ui::labelMedium(Ui::GRAY500, "This is a side panel.") |
                        Ui::center() | Ui::grow(),
                })
            );
        });
    },
};

Page PAGE_SLIDER{
    Mdi::TUNE_VARIANT,
    "Slider",
    "An input where the user selects a value from within a given range.",
    [] {
        return Kr::slider(
                   0.5,
                   NONE,
                   Mdi::CAT,
                   "Cuteness"
               ) |
               Ui::minSize({320, Ui::UNCONSTRAINED}) |
               Ui::center();
    },
};

Page PAGE_TITLEBAR{
    Mdi::DOCK_TOP,
    "Titlebar"s,
    "An area at the top of a window that displays the title of the window and may include other elements such as a menu button, close button, and minimize button.",
    [] {
        return Kr::titlebar(Mdi::DUCK, "Cool App"s) | Ui::center();
    },
};

Page PAGE_TOGGLE{
    Mdi::TOGGLE_SWITCH_OUTLINE,
    "Toggle",
    "A control that allows the user to toggle between checked and not checked.",
    [] {
        return Kr::toggle(
                   true,
                   NONE
               ) |
               Ui::center();
    },
};

export Array PAGES = {
    &PAGE_ALERT,
    &PAGE_AVATAR,
    &PAGE_BADGE,
    &PAGE_CARD,
    &PAGE_CHECKBOX,
    &PAGE_CLOCK,
    &PAGE_COLOR_INPUT,
    &PAGE_CONTEXT_MENU,
    &PAGE_DIALOG,
    &PAGE_FILE_DIALOG,
    &PAGE_FOCUS,
    &PAGE_HSV_SQUARE,
    &PAGE_INPUT,
    &PAGE_NAVBAR,
    &PAGE_NUMBER,
    &PAGE_PRINT_DIALOG,
    &PAGE_PROGRESS,
    &PAGE_RADIO,
    &PAGE_RESIZABLE,
    &PAGE_RICHTEXT,
    &PAGE_ROWS,
    &PAGE_SELECT,
    &PAGE_SIDE_PANEL,
    &PAGE_SIDENAV,
    &PAGE_SLIDER,
    &PAGE_TITLEBAR,
    &PAGE_TOGGLE,
};

} // namespace Hideo::Zoo
