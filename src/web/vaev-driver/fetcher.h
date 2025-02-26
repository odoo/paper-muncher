#pragma once

#include <karm-base/size.h>
#include <karm-gc/heap.h>
#include <karm-mime/url.h>
#include <karm-net/http/http.h>
#include <karm-sys/chan.h>
#include <karm-sys/file.h>
#include <vaev-dom/document.h>
#include <vaev-style/stylesheet.h>

namespace Vaev::Driver {

struct Transfer : public Io::Writer {
    bool _closed = false;

    Res<> throwIfClosed() {
        if (_closed)
            return Error::brokenPipe("transfer is closed");
        return Ok();
    }

    virtual Res<> close() {
        _closed = true;
        return Ok();
    }
};

struct FileTransfer : public Transfer {
    Sys::FileWriter _out;

    FileTransfer(Sys::FileWriter&& out) : _out(std::move(out)) {}

    Res<usize> write(Bytes bytes) override {
        try$(throwIfClosed());
        return _out.write(bytes);
    }

    Res<> close() override {
        try$(throwIfClosed());
        try$(_out.flush());
        return Transfer::close();
    }
};

struct Fetcher {
    Opt<Rc<Fetcher>> _next;

    Fetcher(Opt<Rc<Fetcher>> fallback = NONE)
        : _next(fallback) {}

    virtual Res<String> fetch(Mime::Url const& url) = 0;

    virtual Res<Rc<Transfer>> transfer(Mime::Url const& url) = 0;
};

struct FileFetcher : public Fetcher {
    virtual ~FileFetcher() = default;

    Res<String> fetch(Mime::Url const& url) override {
        auto file = try$(Sys::File::open(url));
        return Io::readAllUtf8(file);
    }

    Res<Rc<Transfer>> transfer(Mime::Url const& url) override {
        auto fileWriter = try$(Sys::File::create(url));
        return Ok(makeRc<FileTransfer>(std::move(fileWriter)));
    };
};

struct HttpPipeTransfer : public Transfer {
    static inline Bytes LINE_SEP = "\r\n"_bytes;
    static inline usize CHUNK_SIZE = kib(64);

    Io::Writer& _out;
    Mime::Url _url;
    Io::BufferWriter _bw;
    bool _wroteHeader = false;

    HttpPipeTransfer(Io::Writer& out, Mime::Url url)
        : _out(out), _url(url), _bw(CHUNK_SIZE) {
    }

    Res<> _writeHeader() {
        Net::Http::Request req;
        req.method = Net::Http::POST;
        req.path = _url.path;
        req.version = Net::Http::Version{.major = 1, .minor = 1};
        req.header.add("Transfer-Encoding", "chunked");

        Io::TextEncoder<Latin1> enc{_out};
        try$(req.unparse(enc));

        _wroteHeader = true;
        return Ok();
    }

    Res<> _flushBuffer() {
        if (_bw.bytes().len() == 0)
            return Ok();

        Io::TextEncoder<Latin1> enc{_out};
        try$(Io::format(enc, "{}", _bw.bytes().len()));
        try$(enc.write(LINE_SEP));
        try$(enc.write(_bw.bytes()));
        try$(enc.write(LINE_SEP));
        _bw.clear();

        return Ok();
    }

    Res<usize> _fillBuffer(Bytes buf) {
        usize chunk = min(buf.len(), CHUNK_SIZE - _bw.bytes().len());
        if (chunk == 0)
            return Ok(0);
        return _bw.write(sub(buf, 0, chunk));
    }

    Res<usize> write(Bytes b) override {
        try$(throwIfClosed());

        usize written = 0;
        while (true) {
            auto chunk = try$(_fillBuffer(b));
            if (chunk == 0)
                break;

            b = next(b, chunk);
            written += chunk;

            if (not _wroteHeader)
                try$(_writeHeader());

            if (_bw.bytes().len() == CHUNK_SIZE)
                try$(_flushBuffer());
        }

        return Ok(written);
    }

    Res<> close() override {
        try$(throwIfClosed());

        try$(_flushBuffer());
        try$(_out.write(bytes("0"s)));
        try$(_out.write(LINE_SEP));
        try$(_out.write(LINE_SEP));

        return Transfer::close();
    }
};

struct HttpPipe : public Fetcher {
    Io::Reader& input = Sys::in();
    Io::TextWriter& output = Sys::out();

    HttpPipe(Opt<Rc<Fetcher>> fallback = NONE)
        : Fetcher(fallback) {}

    HttpPipe(Io::Reader& input, Io::TextWriter& output, Opt<Rc<Fetcher>> fallback = NONE)
        : Fetcher(fallback), input(input), output(output) {}

    virtual ~HttpPipe() = default;

    Res<String> _readFileFromResponse() {
        auto response = try$(Net::Http::Response::read(input));
        auto body = try$(response.readBody(input));

        if (response.code != Net::Http::OK)
            return Error::invalidData("Unexpected HTTP code");

        if (not body)
            return Error::invalidData("Body is expected");

        Io::BufReader br{body.unwrap()};
        auto res = try$(Io::readAllUtf8(br));

        return Ok(res);
    }

    Res<> _sendFetchRequest(Mime::Url const& url) {
        Net::Http::Request req;
        req.method = Net::Http::GET;
        req.path = url.path;
        req.version = Net::Http::Version{.major = 1, .minor = 1};
        try$(req.unparse(output));

        return Ok();
    }

    Res<String> fetch(Mime::Url const& url) override {
        if (url.scheme != "file"s) {
            if (not _next)
                return Error::invalidInput("Unsupported URL scheme");

            return _next.unwrap()->fetch(url);
        }

        try$(_sendFetchRequest(url));
        auto buf = try$(_readFileFromResponse());
        return Ok(buf);
    }

    virtual Res<Rc<Transfer>> transfer(Mime::Url const& url) override {
        return Ok(makeRc<HttpPipeTransfer>(output, url));
    };
};

Rc<Fetcher> makeFetcher(bool isHTTPipe);

Res<Style::StyleSheet> fetchStylesheet(Fetcher& fetcher, Mime::Url url, Style::Origin origin = Style::Origin::AUTHOR);

void fetchStylesheets(Fetcher& fetcher, Gc::Ref<Dom::Node> node, Style::StyleBook& sb);

Res<Gc::Ref<Dom::Document>> fetchDocument(Fetcher& fetcher, Gc::Heap& heap, Mime::Url const& url);

Res<Gc::Ref<Dom::Document>> loadDocument(Fetcher& fetcher, Gc::Heap& heap, Mime::Url const& url, Mime::Mime const& mime);

Res<Gc::Ref<Dom::Document>> viewSource(Gc::Heap& heap, Mime::Url const& url);

} // namespace Vaev::Driver
