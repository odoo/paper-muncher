#pragma once

#include <karm-io/fmt.h>
#include <karm-io/traits.h>
#include <karm-sys/_embed.h>

#include "fd.h"

namespace Karm::Sys {

struct In : public Io::Reader {
    Rc<Fd> _fd;

    In(Rc<Fd> fd)
        : _fd(fd) {}

    Res<usize> read(MutBytes bytes) override {
        return _fd->read(bytes);
    }

    Rc<Fd> fd() {
        return _fd;
    }
};

struct Out : public Io::TextEncoderBase<> {
    Rc<Fd> _fd;

    Out(Rc<Fd> fd)
        : _fd(fd) {}

    Res<usize> write(Bytes bytes) override {
        return _fd->write(bytes);
    }

    Rc<Fd> fd() {
        return _fd;
    }

    Res<usize> flush() override {
        return _fd->flush();
    }
};

struct Err : public Io::TextEncoderBase<> {
    Rc<Fd> _fd;

    Err(Rc<Fd> fd)
        : _fd(fd) {}

    Res<usize> write(Bytes bytes) override {
        return _fd->write(bytes);
    }

    Rc<Fd> fd() {
        return _fd;
    }

    Res<usize> flush() override {
        return _fd->flush();
    }
};

In& in();

Out& out();

Err& err();

inline void print(Str str = "", auto&&... args) {
    (void)Io::format(out(), str, std::forward<decltype(args)>(args)...);
}

inline void err(Str str = "", auto&&... args) {
    (void)Io::format(err(), str, std::forward<decltype(args)>(args)...);
}

inline void println(Str str = "", auto&&... args) {
    // NOTE: Using a lambda here for proper error handling even if we ignore the result.
    (void)([&] -> Res<> {
        try$(Io::format(out(), str, std::forward<decltype(args)>(args)...));
        try$(out().format(Str{Sys::LINE_ENDING}));
        try$(out().flush());
        return Ok();
    }());
}

inline void errln(Str str = "", auto&&... args) {
    // NOTE: Using a lambda here for proper error handling even if we ignore the result.
    (void)([&] -> Res<> {
        try$(Io::format(err(), str, std::forward<decltype(args)>(args)...));
        try$(err().format(Str{Sys::LINE_ENDING}));
        try$(err().flush());
    }());
}

} // namespace Karm::Sys
