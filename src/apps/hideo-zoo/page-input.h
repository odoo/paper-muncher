#pragma once

#include <karm-kira/input.h>
#include <karm-ui/layout.h>
#include <mdi/account.h>
#include <mdi/email.h>
#include <mdi/form-textbox.h>
#include <mdi/lock.h>

#include "model.h"

namespace Hideo::Zoo {

static inline Page PAGE_INPUT{
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

} // namespace Hideo::Zoo
