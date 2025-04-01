#pragma once

#include <karm-base/func.h>
#include <karm-base/opt.h>
#include <karm-base/rc.h>
#include <karm-base/string.h>
#include <karm-base/vec.h>
#include <karm-logger/logger.h>
#include <karm-sys/chan.h>
#include <karm-sys/context.h>

namespace marK::Cli {

// MARK: Tokenizer -------------------------------------------------------------

struct Token {
    enum struct Kind {
        OPERAND,
        OPTION,
        FLAG,
        EXTRA,

        _LEN,
    };

    using enum Kind;

    Kind kind;
    Rune flag = 0;
    Str value = "";

    Token(Str value)
        : Token(Kind::OPERAND, value) {}

    Token(Kind kind, Str value)
        : kind(kind), value(value) {}

    Token(Kind kind)
        : kind(kind) {}

    Token(Rune flag)
        : kind(Kind::FLAG), flag(flag) {}

    bool operator==(Token const& other) const = default;

    void repr(Io::Emit& e) const {
        if (kind == Kind::FLAG)
            e("(token {} {#c})", kind, flag);
        else
            e("(token {} {#})", kind, value);
    }
};

void tokenize(Str arg, Vec<Token>& out);

void tokenize(Slice<Str> args, Vec<Token>& out);

void tokenize(int argc, char** argv, Vec<Token>& out);

// MARK: Values ----------------------------------------------------------------

template <typename T>
struct ValueParser;

template <>
struct ValueParser<bool> {
    static Res<> usage(Io::TextWriter& w);

    static Res<bool> parse(Cursor<Token>&);
};

template <>
struct ValueParser<isize> {
    static Res<> usage(Io::TextWriter& w);

    static Res<isize> parse(Cursor<Token>& c);
};

template <>
struct ValueParser<Str> {
    static Res<> usage(Io::TextWriter& w);

    static Res<Str> parse(Cursor<Token>& c);
};

template <typename T>
struct ValueParser<Vec<T>> {
    static Res<> usage(Io::TextWriter& w) {
        ValueParser<T>::usage(w);
        return w.writeStr("..."s);
    }

    static Res<Vec<T>> parse(Cursor<Token>& c) {
        Vec<T> values;

        while (not c.ended() and c->kind == Token::OPERAND) {
            values.pushBack(try$(ValueParser<T>::parse(c)));
        }

        return Ok(values);
    }
};

template <typename T>
struct ValueParser<Opt<T>> {
    static Res<> usage(Io::TextWriter& w) {
        return ValueParser<T>::usage(w);
    }

    static Res<Opt<T>> parse(Cursor<Token>& c) {
        if (c.ended() or c->kind != Token::OPERAND)
            return Ok(NONE);

        return ValueParser<T>::parse(c);
    }
};

// MARK: Options ---------------------------------------------------------------

enum struct OptionKind {
    OPTION,
    OPERAND,
    EXTRA,
};

struct _OptionImpl {
    OptionKind kind;
    Opt<Rune> shortName;
    String longName;
    String description;

    _OptionImpl(
        OptionKind kind,
        Opt<Rune> shortName,
        String longName,
        String description
    ) : kind(kind),
        shortName(shortName),
        longName(std::move(longName)),
        description(std::move(description)) {}

    virtual ~_OptionImpl() = default;

    virtual Res<> eval(Cursor<Token>& c) = 0;

    virtual Res<> usage(Io::TextWriter& w) {
        if (kind == OptionKind::OPTION) {
            try$(w.writeRune('['));
            if (shortName)
                try$(format(w, "-{c},", shortName.unwrap()));
            try$(format(w, "--{}", longName));
            try$(w.writeRune(']'));
        } else if (kind == OptionKind::OPERAND) {
            try$(w.writeStr(longName.str()));
        } else if (kind == OptionKind::EXTRA) {
            try$(format(w, "[-- {}...]", longName));
        }

        return Ok();
    }
};

template <typename T>
struct OptionImpl : public _OptionImpl {
    Opt<T> value;

    OptionImpl(
        OptionKind kind,
        Opt<Rune> shortName,
        String longName,
        String description,
        Opt<T> defaultValue
    ) : _OptionImpl(kind, shortName, std::move(longName), std::move(description)),
        value(defaultValue) {}

    Res<> eval(Cursor<Token>& c) override {
        value = try$(ValueParser<T>::parse(c));
        return Ok();
    }
};

template <typename T>
struct Option {
    Rc<OptionImpl<T>> _impl;

    Option(Rc<OptionImpl<T>> impl)
        : _impl(std::move(impl)) {}

    T const& unwrap() const {
        return _impl->value.unwrap();
    }

    operator T() const {
        return _impl->value.unwrapOr(T{});
    }

    operator Rc<_OptionImpl>() {
        return _impl;
    }
};

using Flag = Option<bool>;

static inline Flag flag(Opt<Rune> shortName, String longName, String description) {
    return makeRc<OptionImpl<bool>>(OptionKind::OPTION, shortName, longName, description, false);
}

template <typename T>
static inline Option<T> option(Opt<Rune> shortName, String longName, String description, Opt<T> defaultValue = NONE) {
    return makeRc<OptionImpl<T>>(OptionKind::OPTION, shortName, longName, description, defaultValue);
}

template <typename T>
static inline Option<T> operand(String longName, String description, T defaultValue = {}) {
    return makeRc<OptionImpl<T>>(OptionKind::OPERAND, NONE, longName, description, defaultValue);
}

static inline Option<Vec<Str>> extra(String description) {
    return makeRc<OptionImpl<Vec<Str>>>(OptionKind::EXTRA, NONE, ""s, description, Vec<Str>{});
}

// MARK: Command ---------------------------------------------------------------

struct Command : Meta::Pinned {
    using Callback = Func<Async::Task<>(Sys::Context&)>;

    struct Props {
    };

    String longName;
    Opt<Rune> shortName;
    String description = ""s;
    Vec<Rc<_OptionImpl>> options;
    Opt<Callback> callbackAsync;

    Vec<Rc<Command>> _commands;
    Command* _parent = nullptr;

    Option<bool> _help = flag('h', "help"s, "Show this help message and exit."s);
    Option<bool> _usage = flag('u', "usage"s, "Show usage message and exit."s);
    Option<bool> _version = flag('v', "version"s, "Show version information and exit."s);

    bool _invoked = false;

    Command(
        String longName,
        Opt<Rune> shortName = NONE,
        String description = ""s,
        Vec<Rc<_OptionImpl>> options = {},
        Opt<Callback> callbackAsync = NONE
    )
        : longName(std::move(longName)),
          shortName(shortName),
          description(std::move(description)),
          options(std::move(options)),
          callbackAsync(std::move(callbackAsync)) {
        options.pushBack(_help);
        options.pushBack(_usage);
        options.pushBack(_version);
    }

    Command& subCommand(
        String longName,
        Opt<Rune> shortName = NONE,
        String description = ""s,
        Vec<Rc<_OptionImpl>> options = {},
        Opt<Callback> callbackAsync = NONE
    ) {
        auto cmd = makeRc<Command>(
            longName,
            shortName,
            description,
            options,
            std::move(callbackAsync)
        );
        cmd->_parent = this;
        _commands.pushBack(cmd);
        return *last(_commands);
    }

    template <typename T>
    void option(Option<T> field) {
        options.pushBack(field._impl);
    }

    template <typename T>
    Option<T> option(
        Opt<Rune> shortName,
        String longName,
        String description,
        Opt<T> defaultValue = NONE
    ) {
        auto store = makeRc<OptionImpl<T>>(
            OptionKind::OPTION,
            shortName,
            longName,
            description,
            defaultValue
        );
        options.pushBack(store);
        return {store};
    }

    Flag flag(Opt<Rune> shortName, String longName, String description) {
        auto store = makeRc<OptionImpl<bool>>(
            OptionKind::OPTION,
            shortName,
            longName,
            description,
            false
        );
        options.pushBack(store);
        return {store};
    }

    Res<> usage(Io::TextWriter& w) {
        auto printLongName = [](this auto& self, Command& cmd, Io::TextWriter& w) -> Res<> {
            if (cmd._parent)
                try$(self(*cmd._parent, w));
            try$(format(w, "{} ", cmd.longName));
            return Ok();
        };

        try$(printLongName(*this, w));

        for (auto& opt : options) {
            if (opt->kind != OptionKind::OPTION)
                continue;
            try$(opt->usage(w));
            try$(w.writeRune(' '));
        }

        for (auto& opt : options) {
            if (opt->kind != OptionKind::OPERAND)
                continue;
            try$(opt->usage(w));
            try$(w.writeRune(' '));
        }

        for (auto& opt : options) {
            if (opt->kind != OptionKind::EXTRA)
                continue;
            try$(opt->usage(w));
        }

        if (marK::any(_commands)) {
            try$(format(w, "<command> [args...]"));
        }

        return Ok();
    }

    Res<> _showUsage(Io::TextWriter& w) {
        try$(format(w, "Usage: "));
        try$(usage(w));
        try$(w.writeRune('\n'));

        return Ok();
    }

    Res<> _showHelp(Io::TextWriter& w) {
        try$(format(w, "Usage:\n  "));
        try$(usage(w));
        try$(w.writeStr("\n\n"s));

        try$(format(w, "Description:\n  {}\n\n", description));

        if (marK::any(options)) {
            try$(w.writeStr("Options:\n"s));
            for (auto& opt : options) {
                if (opt->kind != OptionKind::OPTION)
                    continue;

                try$(w.writeStr("  "s));
                if (opt->shortName)
                    try$(format(w, "-{c}, ", opt->shortName.unwrap()));

                try$(format(w, "--{} - {}\n", opt->longName, opt->description));
            }
            try$(w.writeRune('\n'));
        }

        if (marK::any(_commands)) {
            try$(w.writeStr("Subcommands:\n"s));
            for (auto& cmd : _commands) {
                try$(format(w, "  {c} {} - {}\n", cmd->shortName.unwrapOr(' '), cmd->longName, cmd->description));
            }
            try$(w.writeRune('\n'));
        }

        return Ok();
    }

    Res<bool> _evalOption(Cursor<Token>& c) {
        bool found = false;

        for (auto& opt : options) {
            if (c.ended())
                break;

            if (opt->kind != OptionKind::OPTION)
                continue;

            bool shortNameMatch = opt->shortName and c->flag == opt->shortName.unwrap();
            bool longNameMatch = c->value == opt->longName;

            if (not(shortNameMatch or longNameMatch))
                continue;

            c.next();
            try$(opt->eval(c));
            found = true;
        }

        return Ok(found);
    }

    Res<> _evalParams(Cursor<Token>& c) {
        for (auto& opt : options) {
            while (try$(_evalOption(c)))
                ;

            if (c.ended())
                break;

            if (opt->kind != OptionKind::OPERAND)
                continue;

            try$(opt->eval(c));
        }

        return Ok();
    }

    Async::Task<> execAsync(Sys::Context& ctx) {
        auto args = Sys::useArgs(ctx);
        Vec<Token> tokens;
        for (usize i = 0; i < args.len(); ++i)
            tokenize(args[i], tokens);
        co_return co_await execAsync(ctx, tokens);
    }

    Async::Task<> execAsync(Sys::Context& ctx, Slice<Str> args) {
        Vec<Token> tokens;
        tokenize(args, tokens);
        co_return co_await execAsync(ctx, tokens);
    }

    Async::Task<> execAsync(Sys::Context& ctx, Cursor<Token> c) {
        co_try$(_evalParams(c));

        if (_help) {
            co_try$(_showHelp(Sys::out()));
            co_return Ok();
        }

        if (_usage) {
            co_try$(_showUsage(Sys::out()));
            co_return Ok();
        }

        if (_version) {
            co_try$(format(Sys::out(), "{} {}\n", longName, stringify$(__ck_version_value) ""s));
            co_return Ok();
        }

        _invoked = true;
        if (callbackAsync)
            co_trya$(callbackAsync.unwrap()(ctx));

        if (marK::any(_commands) and c.ended()) {
            co_try$(_showUsage(Sys::out()));
            co_return Error::invalidInput("expected subcommand");
        }

        if (not c.ended()) {
            if (c->kind != Token::OPERAND)
                co_return Error::invalidInput("expected subcommand");

            if (not marK::any(_commands))
                co_return Error::invalidInput("unexpected subcommand");

            auto value = c->value;
            c.next();

            for (auto& cmd : _commands) {

                bool shortNameMatch = value.len() == 1 and iterRunes(value).first() == cmd->shortName;
                bool longNameMatch = value == cmd->longName;

                if (not(shortNameMatch or longNameMatch))
                    continue;

                co_return co_await cmd->execAsync(ctx, c);
            }

            co_return Error::invalidInput("unknown subcommand");
        }
        co_return Ok();
    }

    operator bool() const {
        return _invoked;
    }
};

} // namespace Karm::Cli
