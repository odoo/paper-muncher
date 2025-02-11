#pragma once

#include <karm-base/backtrace.h>
#include <karm-base/box.h>
#include <karm-base/cow.h>
#include <karm-base/endian.h>
#include <karm-base/enum.h>
#include <karm-base/map.h>
#include <karm-base/rc.h>
#include <karm-base/time.h>
#include <karm-base/tuple.h>
#include <karm-base/vec.h>
#include <karm-io/aton.h>
#include <karm-io/impls.h>
#include <karm-io/sscan.h>
#include <karm-io/traits.h>
#include <karm-meta/signess.h>

namespace Karm::Io {

template <typename T>
struct Formatter;

struct _Args {
    virtual ~_Args() = default;
    virtual usize len() = 0;
    virtual Res<> format(Io::SScan& scan, Io::TextWriter& w, usize index) = 0;
};

template <typename... Ts>
struct Args : public _Args {
    Tuple<Ts...> _tuple{};

    Args(Ts&&... ts) : _tuple(std::forward<Ts>(ts)...) {}

    usize len() override {
        return _tuple.len();
    }

    Res<> format(Io::SScan& scan, Io::TextWriter& w, usize index) override {
        Res<> result = Error::invalidData("format index out of range");
        usize i = 0;
        _tuple.visit([&](auto const& t) {
            if (index == i) {
                using U = Meta::RemoveConstVolatileRef<decltype(t)>;
                Formatter<U> formatter;
                if constexpr (requires() {
                                  formatter.parse(scan);
                              }) {
                    formatter.parse(scan);
                }
                result = formatter.format(w, t);
            }
            i++;

            return true;
        });
        return result;
    }
};

Res<> _format(Io::TextWriter& w, Str format, _Args& args);

inline Res<> format(Io::TextWriter& w, Str format) {
    try$(w.format(format));
    return Ok();
}

template <typename... Ts>
inline Res<> format(Io::TextWriter& w, Str format, Ts&&... ts) {
    Args<Ts...> args{std::forward<Ts>(ts)...};
    return _format(w, format, args);
}

inline Res<String> format(Str format) {
    return Ok(format);
}

template <typename... Ts>
inline Res<String> format(Str format, Ts&&... ts) {
    Io::StringWriter w{};
    Args<Ts...> args{std::forward<Ts>(ts)...};
    try$(_format(w, format, args));
    return Ok(w.take());
}

template <typename T>
inline Res<String> toStr(T const& t, Str format = "") {
    Io::StringWriter w{};
    Formatter<T> formatter;
    if constexpr (requires(Io::SScan& scan) {
                      formatter.parse(scan);
                  }) {
        Io::SScan scan{format};
        formatter.parse(scan);
    }
    try$(formatter.format(w, t));
    return Ok(w.take());
}

// MARK: Align Formatting ------------------------------------------------------

enum struct Align {
    LEFT,
    RIGHT,
    CENTER,
};

template <typename T>
struct Aligned {
    T _inner;
    Align _align;
    usize _width;
};

inline auto aligned(auto inner, Align align, usize width) {
    return Aligned<decltype(inner)>{inner, align, width};
}

template <typename T>
struct Formatter<Aligned<T>> {
    Formatter<T> _innerFmt{};

    void parse(Io::SScan& scan) {
        if constexpr (requires() {
                          _innerFmt.parse(scan);
                      }) {
            _innerFmt.parse(scan);
        }
    }

    Res<> format(Io::TextWriter& w, Aligned<T> val) {
        Io::StringWriter buf;
        try$(_innerFmt.format(buf, val._inner));
        usize width = buf.len();

        if (width < val._width) {
            usize pad = val._width - width;
            switch (val._align) {
            case Align::LEFT:
                try$(w.format(buf.str()));
                for (usize i = 0; i < pad; i++)
                    try$(w.writeRune(' '));
                break;
            case Align::RIGHT:
                for (usize i = 0; i < pad; i++)
                    try$(w.writeRune(' '));
                try$(w.format(buf.str()));
                break;
            case Align::CENTER:
                for (usize i = 0; i < pad / 2; i++)
                    try$(w.writeRune(' '));
                try$(w.format(buf.str()));
                for (usize i = 0; i < pad / 2; i++)
                    try$(w.writeRune(' '));
                break;
            }
        } else {
            try$(w.format(buf.str()));
        }

        return Ok();
    }
};

// MARK: Case Formatting -------------------------------------------------------

enum struct Case {
    DEFAULT,
    CAMEL,
    CAPITAL,
    CONSTANT,
    DOT,
    HEADER,
    NO,
    PARAM,
    PASCAL,
    PATH,
    SENTENCE,
    SNAKE,
    TITLE,
    SWAP,
    LOWER,
    LOWER_FIRST,
    UPPER,
    UPPER_FIRST,
    SPONGE,
};

Res<String> toDefaultCase(Str str);

Res<String> toCamelCase(Str str);

Res<String> toCapitalCase(Str str);

Res<String> toConstantCase(Str str);

Res<String> toDotCase(Str str);

Res<String> toHeaderCase(Str str);

Res<String> toNoCase(Str str);

Res<String> toParamCase(Str str);

Res<String> toPascalCase(Str str);

Res<String> toPathCase(Str str);

Res<String> toSentenceCase(Str str);

Res<String> toSnakeCase(Str str);

Res<String> toTitleCase(Str str);

Res<String> toSwapCase(Str str);

Res<String> toLowerCase(Str str);

Res<String> toLowerFirstCase(Str str);

Res<String> toUpperCase(Str str);

Res<String> toUpperFirstCase(Str str);

Res<String> toSpongeCase(Str str);

Res<String> changeCase(Str str, Case toCase);

template <typename T>
struct Cased {
    T _inner;
    Case _case;
};

inline auto cased(auto inner, Case cased) {
    return Cased<decltype(inner)>{inner, cased};
}

template <typename T>
struct Formatter<Cased<T>> {
    Formatter<T> _inner{};

    void parse(Io::SScan& scan) {
        if constexpr (requires() {
                          _inner.parse(scan);
                      }) {
            _inner.parse(scan);
        }
    }

    Res<> format(Io::TextWriter& w, Cased<T> val) {
        Io::StringWriter sw;
        try$(_inner.format(sw, val._inner));
        String result = try$(changeCase(sw.str(), val._case));
        try$(w.format(result.str()));
        return Ok();
    }
};

// MARK: Number Formatting -----------------------------------------------------

struct NumberFormatter {
    bool prefix = false;
    bool isChar = false;
    usize base = 10;
    usize width = 0;
    char fillChar = ' ';
    bool trailingZeros = false;
    usize precision = 6;

    Str formatPrefix();

    void parse(Str str);

    void parse(Io::SScan& scan);

    Res<> formatUnsigned(Io::TextWriter& w, usize val);

    Res<> formatSigned(Io::TextWriter& w, isize val);

    Res<> formatFloat(Io::TextWriter& w, f64 val);

    Res<> formatRune(Io::TextWriter& w, Rune val);
};

template <Meta::UnsignedIntegral T>
struct Formatter<T> : public NumberFormatter {
    Res<> format(Io::TextWriter& w, T const& val) {
        if (isChar)
            try$(formatRune(w, val));
        else
            try$(formatUnsigned(w, val));
        return Ok();
    }
};

template <Meta::SignedIntegral T>
struct Formatter<T> : public NumberFormatter {
    Res<> format(Io::TextWriter& w, T const& val) {
        if (isChar)
            try$(w.writeRune(val));
        else
            try$(formatSigned(w, val));
        return Ok();
    }
};

template <Meta::Float T>
struct Formatter<T> : public NumberFormatter {
    Res<> format(Io::TextWriter& w, f64 const& val) {
        return formatFloat(w, val);
    }
};

template <typename T>
struct Formatter<Be<T>> : public Formatter<T> {
    Res<> format(Io::TextWriter& w, Be<T> const& val) {
        return Formatter<T>::format(w, val.value());
    }
};

template <typename T>
struct Formatter<Le<T>> : public Formatter<T> {
    Res<> format(Io::TextWriter& w, Le<T> const& val) {
        return Formatter<T>::format(w, val.value());
    }
};

// MARK: Format Enums ----------------------------------------------------------

template <Meta::Enum T>
struct Formatter<T> {
    Res<> format(Io::TextWriter& w, T val) {
        if constexpr (BoundedEnum<T>) {
            try$(w.format(nameOf<T>(val)));
            return Ok();
        } else {
            return Io::format(w, "({} {})", nameOf<T>(), toUnderlyingType(val));
        }
    }
};

// MARK: Format Pointers -------------------------------------------------------

template <typename T>
struct Formatter<T*> {
    bool _prefix = false;

    void parse(Io::SScan& scan) {
        _prefix = scan.skip('#');
    }

    Res<> format(Io::TextWriter& w, T* val) {
        if (_prefix) {
            try$(w.writeRune('('));
            try$(w.format(nameOf<T>()));
            try$(w.format(" *)"s));
        }

        if (val) {
            NumberFormatter fmt;
            fmt.base = 16;
            fmt.fillChar = '0';
            fmt.width = sizeof(T*) * 2;
            fmt.prefix = true;
            try$(fmt.formatUnsigned(w, (usize)val));
        } else {
            try$(w.format("nullptr"s));
        }

        return Ok();
    }
};

template <>
struct Formatter<std::nullptr_t> {
    Res<> format(Io::TextWriter& w, std::nullptr_t) {
        try$(w.format("nullptr"s));
        return Ok();
    }
};

// MARK: Format Optionals ------------------------------------------------------

template <>
struct Formatter<None> {
    Res<> format(Io::TextWriter& w, None const&) {
        try$(w.format("None"s));
        return Ok();
    }
};

template <>
struct Formatter<bool> {
    Res<> format(Io::TextWriter& w, bool val) {
        try$(w.format(val ? "True"s : "False"s));
        return Ok();
    }
};

template <typename T>
struct Formatter<Opt<T>> {
    Formatter<T> inner;

    void parse(Io::SScan& scan) {
        if constexpr (requires() {
                          inner.parse(scan);
                      }) {
            inner.parse(scan);
        }
    }

    Res<> format(Io::TextWriter& w, Opt<T> const& val) {
        if (val)
            try$(inner.format(w, *val));
        else
            try$(w.format("None"s));
        return Ok();
    }
};

// MARK: Format Results --------------------------------------------------------

template <typename T>
struct Formatter<Ok<T>> {
    Formatter<T> inner;

    void parse(Io::SScan& scan) {
        if constexpr (requires() {
                          inner.parse(scan);
                      }) {
            inner.parse(scan);
        }
    }

    Res<> format(Io::TextWriter& w, Ok<T> const& val) {
        if constexpr (Meta::Same<T, None>)
            try$(w.format("Ok"s));
        else
            try$(inner.format(w, val.inner));
        return Ok();
    }
};

template <>
struct Formatter<Error> {
    Res<> format(Io::TextWriter& w, Error const& val) {
        Str msg = Str::fromNullterminated(val.msg());
        try$(w.format(msg));
        return Ok();
    }
};

template <typename T, typename E>
struct Formatter<Res<T, E>> {
    Formatter<T> _fmtOk;
    Formatter<E> _fmtErr;

    void parse(Io::SScan& scan) {
        if constexpr (requires() {
                          _fmtOk.parse(scan);
                      }) {
            _fmtOk.parse(scan);
        }

        if constexpr (requires() {
                          _fmtErr.parse(scan);
                      }) {
            _fmtErr.parse(scan);
        }
    }

    Res<> format(Io::TextWriter& w, Res<T, E> const& val) {
        if (val)
            return _fmtOk.format(w, val.unwrap());
        return _fmtErr.format(w, val.none());
    }
};

// MARK: Format Unions ---------------------------------------------------------

template <typename... Ts>
struct Formatter<Union<Ts...>> {

    Res<> format(Io::TextWriter& w, Union<Ts...> const& val) {
        return val.visit([&](auto const& v) -> Res<> {
            return Io::format(w, "{}", v);
        });
    }
};

// MARK: Format Ordering -------------------------------------------------------

template <>
struct Formatter<std::strong_ordering> {
    Res<> format(Io::TextWriter& w, std::strong_ordering val) {
        if (val == std::strong_ordering::less)
            try$(w.format("Less"s));
        else if (val == std::strong_ordering::greater)
            try$(w.format("Greater"s));
        else
            try$(w.format("Equal"s));
        return Ok();
    }
};

template <>
struct Formatter<std::weak_ordering> {
    Res<> format(Io::TextWriter& w, std::weak_ordering val) {
        if (val == std::weak_ordering::less)
            try$(w.format("Less"s));
        else if (val == std::weak_ordering::greater)
            try$(w.format("Greater"s));
        else
            try$(w.format("Equivalent"s));
        return Ok();
    }
};

template <>
struct Formatter<std::partial_ordering> {
    Res<> format(Io::TextWriter& w, std::partial_ordering val) {
        if (val == std::partial_ordering::equivalent)
            try$(w.format("Equivalent"s));
        else if (val == std::partial_ordering::less)
            try$(w.format("Less"s));
        else if (val == std::partial_ordering::greater)
            try$(w.format("Greater"s));
        else
            try$(w.format("Unordered"s));

        return Ok();
    }
};

// MARK: Format References -----------------------------------------------------

template <typename T>
struct Formatter<Rc<T>> {
    Formatter<T> formatter;

    void parse(Io::SScan& scan) {
        if constexpr (requires() {
                          formatter.parse(scan);
                      }) {
            formatter.parse(scan);
        }
    }

    Res<> format(Io::TextWriter& w, Rc<T> const& val) {
        return formatter.format(w, val.unwrap());
    }
};

template <typename T>
struct Formatter<Weak<T>> {
    Formatter<T> formatter;

    void parse(Io::SScan& scan) {
        if constexpr (requires() {
                          formatter.parse(scan);
                      }) {
            formatter.parse(scan);
        }
    }

    Res<> format(Io::TextWriter& w, Weak<T> const& val) {
        auto inner = val.upgrade();
        if (not inner)
            try$(w.format("None"s));
        else
            try$(formatter.format(w, inner.unwrap()));
        return Ok();
    }
};

template <typename T>
struct Formatter<Box<T>> {
    Formatter<T> formatter;

    void parse(Io::SScan& scan) {
        if constexpr (requires() {
                          formatter.parse(scan);
                      }) {
            formatter.parse(scan);
        }
    }

    Res<> format(Io::TextWriter& w, Box<T> const& val) {
        return formatter.format(w, *val);
    }
};

template <typename T>
struct Formatter<Cow<T>> {
    Formatter<T> formatter;

    void parse(Io::SScan& scan) {
        if constexpr (requires() {
                          formatter.parse(scan);
                      }) {
            formatter.parse(scan);
        }
    }

    Res<usize> format(Io::TextWriter& w, Cow<T> const& val) {
        return formatter.format(w, *val);
    }
};

// MARK: Format Reflectable ----------------------------------------------------

struct Emit;

template <typename T>
struct Repr;

template <typename T>
concept ReprMethod = requires(T t, Emit& emit) {
    t.repr(emit);
};

template <typename T>
concept Reprable =
    ReprMethod<T> or
    requires(T t, Emit& emit) {
        Repr<T>::repr(emit, t);
    };

// MARK: Format Sliceable ------------------------------------------------------

template <Sliceable T>
struct Formatter<T> {
    Formatter<typename T::Inner> inner;

    void parse(Io::SScan& scan) {
        if constexpr (requires() {
                          inner.parse(scan);
                      }) {
            inner.parse(scan);
        }
    }

    Res<> format(Io::TextWriter& w, T const& val) {
        try$(w.format("["s));
        for (usize i = 0; i < val.len(); i++) {
            if (i != 0)
                try$(w.format(", "s));
            try$(inner.format(w, val[i]));
        }
        try$(w.format("]"s));
        return Ok();
    }
};

// MARK: Format Map ------------------------------------------------------------

template <typename K, typename V>
struct Formatter<Map<K, V>> {
    Formatter<K> keyFormatter;
    Formatter<V> valFormatter;

    void parse(Io::SScan& scan) {
        if constexpr (requires() {
                          keyFormatter.parse(scan);
                          valFormatter.parse(scan);
                      }) {
            keyFormatter.parse(scan);
            valFormatter.parse(scan);
        }
    }

    Res<> format(Io::TextWriter& w, Map<K, V> const& val) {
        try$(w.format("{"s));
        bool first = true;
        for (auto const& [key, value] : val.iter()) {
            if (not first)
                try$(w.format(", "s));
            first = false;
            try$(keyFormatter.format(w, key));
            try$(w.format(": "s));
            try$(valFormatter.format(w, value));
        }
        try$(w.format("}"s));
        return Ok();
    }
};

// MARK: Format Range ----------------------------------------------------------

template <typename T, typename Tag>
struct Formatter<Range<T, Tag>> {

    Formatter<T> inner;

    void parse(Io::SScan& scan) {
        if constexpr (requires() {
                          inner.parse(scan);
                      }) {
            inner.parse(scan);
        }
    }

    Res<> format(Io::TextWriter& w, Range<T, Tag> const& val) {
        try$(w.format("["s));
        try$(inner.format(w, val.start));
        try$(w.format("-"s));
        try$(inner.format(w, val.end()));
        try$(w.format("]"s));
        return Ok();
    }
};

// MARK: Format String ---------------------------------------------------------

template <StaticEncoding E>
struct StringFormatter {
    bool prefix = false;

    void parse(Io::SScan& scan) {
        if (scan.skip('#'))
            prefix = true;
    }

    Res<> format(Io::TextWriter& w, _Str<E> text) {
        if (not prefix) {
            try$(w.format(text));
            return Ok();
        }

        try$(w.writeRune('"'));
        for (Rune c : iterRunes(text)) {
            if (c == '"')
                try$(w.format("\\\""s));
            else if (c == '\\')
                try$(w.format("\\\\"s));
            else if (c == '\a')
                try$(w.format("\\a"s));
            else if (c == '\b')
                try$(w.format("\\b"s));
            else if (c == '\f')
                try$(w.format("\\f"s));
            else if (c == '\n')
                try$(w.format("\\n"s));
            else if (c == '\r')
                try$(w.format("\\r"s));
            else if (c == '\t')
                try$(w.format("\\t"s));
            else if (c == '\v')
                try$(w.format("\\v"s));
            else if (not isAsciiPrint(c))
                try$(Io::format(w, "\\u{x}", c));
            else
                try$(w.writeRune(c));
        }
        try$(w.writeRune('"'));
        return Ok();
    }
};

template <StaticEncoding E>
struct Formatter<_Str<E>> : public StringFormatter<E> {};

template <StaticEncoding E>
struct Formatter<_String<E>> : public StringFormatter<E> {
    Res<> format(Io::TextWriter& w, _String<E> const& text) {
        return StringFormatter<E>::format(w, text.str());
    }
};

template <usize N>
struct Formatter<StrLit<N>> : public StringFormatter<Utf8> {};

template <>
struct Formatter<char const*> : public StringFormatter<Utf8> {
    Res<> format(Io::TextWriter& w, char const* text) {
        _Str<Utf8> str = Str::fromNullterminated(text);
        return StringFormatter::format(w, str);
    }
};

// MARK: Format Time -----------------------------------------------------------

template <>
struct Formatter<Duration> {
    Res<> format(Io::TextWriter& w, Duration const& val) {
        return Io::format(w, "{}.{03}s", val.toSecs(), val.toMSecs() % 1000);
    }
};

template <>
struct Formatter<Instant> {
    Res<> format(Io::TextWriter& w, Instant const& val) {
        return Io::format(w, "monotonic:{}", val._value);
    }
};

template <>
struct Formatter<SystemTime> {
    Res<> format(Io::TextWriter& w, SystemTime const& val) {
        return Io::format(w, "{}", DateTime::fromInstant(val));
    }
};

template <>
struct Formatter<Time> {
    Res<> format(Io::TextWriter& w, Time const& val) {
        return Io::format(w, "{02}:{02}:{02}", val.hour, val.minute, val.second);
    }
};

template <>
struct Formatter<Date> {
    Res<> format(Io::TextWriter& w, Date const& val) {
        return Io::format(w, "{04}-{02}-{02}", (isize)val.year, (usize)val.month + 1, (usize)val.day + 1);
    }
};

template <>
struct Formatter<DateTime> {
    Res<> format(Io::TextWriter& w, DateTime const& val) {
        return Io::format(w, "{} {}", val.date, val.time);
    }
};

// MARK: Format Tuple ----------------------------------------------------------

template <typename Car, typename Cdr>
struct Formatter<Pair<Car, Cdr>> {
    Res<> format(Io::TextWriter& w, Pair<Car, Cdr> const& val) {
        try$(w.writeRune('{'));

        Formatter<Car> carFormatter;
        try$(carFormatter.format(w, val.v0));
        try$(w.format(", "s));

        Formatter<Cdr> cdrFormatter;
        try$(cdrFormatter.format(w, val.v1));
        try$(w.writeRune('}'));
        return Ok();
    }
};

template <typename... Ts>
struct Formatter<Tuple<Ts...>> {
    Res<> format(Io::TextWriter& w, Tuple<Ts...> const& val) {
        bool first = true;
        try$(w.writeRune('{'));
        try$(val.visit([&]<typename T>(T const& f) -> Res<> {
            if (not first)
                try$(w.format(", "s));

            Formatter<T> formatter;
            try$(formatter.format(w, f));

            first = false;
            return Ok();
        }));
        try$(w.writeRune('}'));
        return Ok();
    }
};

// MARK: Format backtrace ------------------------------------------------------

template <>
struct Formatter<Backtrace> {
    Res<> format(Io::TextWriter& w, Backtrace const& val) {
        if (val.status() == Backtrace::DISABLED) {
            try$(w.format("(backtrace disabled)"s));
        } else if (val.status() == Backtrace::UNSUPPORTED) {
            try$(w.format("(backtrace unsupported)"s));
        } else {
            usize index = 1;
            for (auto const& frame : val.frames()) {
                try$(Io::format(w, "#{}: {} at {}:{}\n", index, frame.desc, frame.file, frame.line));
                index++;
            }
        }

        return Ok();
    }
};

} // namespace Karm::Io
