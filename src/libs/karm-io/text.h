#pragma once

#include "traits.h"

namespace Karm::Io {

// MARK: Writer & Encoder ------------------------------------------------------

struct TextWriter :
    public Writer,
    public Flusher {
    using Writer::write;

    template <StaticEncoding E>
    Res<> format(_Str<E> str) {
        for (auto rune : iterRunes(str))
            try$(writeRune(rune));
        return Ok();
    }

    template <StaticEncoding E, typename... Args>
    Res<> format(_Str<E> fmt, Args&&... args); // NOTE: Include karm-fmt/fmt.h to use

    virtual Res<usize> writeRunes(Slice<Rune> runes) = 0;

    Res<usize> flush() override {
        return Ok(0uz);
    }
};

template <StaticEncoding E = typename Sys::Encoding>
struct TextEncoderBase :
    public TextWriter {

    using Writer::write;

    Res<usize> writeRunes(Slice<Rune> runes) override {
        usize written = 0;
        for (auto rune : runes) {
            typename E::One one;
            if (not E::encodeUnit(rune, one)) {
                return Ok(written);
            }
            written += try$(write(bytes(one)));
        }
        return Ok(written);
    }
};

template <StaticEncoding E = typename Sys::Encoding>
struct TextEncoder :
    public TextEncoderBase<E> {
    Io::Writer& _writer;

    TextEncoder(Io::Writer& writer)
        : _writer(writer) {}

    Res<usize> write(Bytes bytes) override {
        return _writer.write(bytes);
    }
};

template <StaticEncoding E>
struct _StringWriter :
    public TextWriter,
    public _StringBuilder<E> {

    _StringWriter(usize cap = 16) : _StringBuilder<E>(cap) {}

    Res<usize> write(Bytes) override {
        panic("can't write raw bytes to a string");
    }

    Res<usize> writeRune(Rune rune) override {
        _StringBuilder<E>::append(rune);
        return Ok(E::runeLen(rune));
    }

    Res<usize> writeUnit(Slice<typename E::Unit> unit) {
        _StringBuilder<E>::append(unit);
        return Ok(unit.len());
    }
};

using StringWriter = _StringWriter<Utf8>;

// MARK: Formating -------------------------------------------------------------

template <StaticEncoding E, typename... Args>
Res<> TextWriter::format(_Str<E> fmt, Args&&... args) {
}

// MARK: Formatters ------------------------------------------------------------

} // namespace Karm::Io
