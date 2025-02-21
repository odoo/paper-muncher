#pragma once

#include <karm-gc/heap.h>
#include <karm-mime/url.h>
#include <karm-net/http/http.h>
#include <karm-sys/chan.h>
#include <karm-sys/file.h>
#include <vaev-dom/document.h>
#include <vaev-style/stylesheet.h>

namespace Vaev::Driver {

struct Transfer : public Io::Writer {
    virtual Res<> close() = 0;
};

struct FileTransfer : public Transfer {
    bool _closed = false;
    Sys::FileWriter _out;

    FileTransfer(Sys::FileWriter&& out)
        : _out(std::move(out)) {}

    Res<usize> write(Bytes bytes) override {
        if (_closed)
            return Error::brokenPipe("transfer is closed");
        return _out.write(bytes);
    }

    Res<> close() override {
        return Ok();
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

    virtual Res<Rc<Transfer>> transfer(Mime::Url const& url) override {
        auto fileWriter = try$(Sys::File::create(url));
        return Ok(makeRc<FileTransfer>(fileWriter));
    };
};

struct HttPipeTransfer : public Transfer {
    // NOTE: Fine-tuned for linux pipe buffer size
    static constexpr usize const MAX_BUF_SIZE = Math::pow(2, 16);

    Mime::Url _url;
    Io::Writer& _chan;
    bool _start = true;
    Io::BufferWriter _buf;

    HttPipeTransfer(Mime::Url url, Io::Writer& chan)
        : _url(url), _chan(chan) {}

    Res<> _writeHeader() {
        Net::Http::Request req{
            .method = Net::Http::Method::POST,
            .path = _url.path,
            .version = {.major = 1, .minor = 1},
            .header = {
                {"Transfer-Encoding"s, "chunked"s},
            },
        };

        Io::TextEncoder<Utf8> enc{_chan};
        try$(req.unparse(enc));

        _start = true;
        return Ok();
    }

    Res<> _flushBuffer() {
        if (_buf.bytes().len() == 0)
            return Ok();

        Io::TextEncoder<Utf8> enc{_chan};
        try$(Io::format(enc, "{}", _buf.bytes().len()));
        try$(enc.write("\r\n"_bytes));
        try$(enc.write(_buf.bytes()));
        try$(enc.write("\r\n"_bytes));
        _buf.clear();

        return Ok();
    }

    Res<usize> write(Bytes buf) override {
        if (_start) {
            try$(_writeHeader());
            _start = false;
        }

        

        auto startWriterPos = cntedOut._pos;
        if (not _start)
            try$(_writeHeader());

        try$(bw.write(b));
        if (_buf.startWriterPos().len() >= MAX_BUF_SIZE)
            try$(_flushBuffer());

        return Ok(cntedOut._pos - startWriterPos);
    }

    Res<> close() override {
        try$(_flushBuffer());
        try$(encoder.write(bytes("0"s)));
        try$(encoder.write(LINE_SEP));
        try$(encoder.write(LINE_SEP));
        return Ok();
    }
};

struct HttpPipe : public Fetcher {

    Io::Reader& input = Sys::in();
    Io::TextWriter& output = Sys::out();

    HttpPipe(Opt<Rc<Fetcher>> fallback = NONE) : Fetcher(fallback) {}

    HttpPipe(Io::Reader& input, Io::TextWriter& output, Opt<Rc<Fetcher>> fallback = NONE)
        : Fetcher(fallback), input(input), output(output) {}

    virtual ~HttpPipe() = default;

    Res<String> _readFileFromResponse() {
        auto response = try$(Net::Http::Response::read(input));
        auto body = try$(response.readBody(input));

        if (response.code != Net::Http::OK)
            return Error::invalidData("unexpected HTTP code");

        if (not body)
            return Error::invalidData("body is expected");

        Io::BufReader br{body.unwrap()};
        auto res = try$(Io::readAllUtf8(br));

        return Ok(res);
    }

    Res<> _sendFetchRequest(Mime::Url const& url) {
        Net::Http::Request req;
        req.method = Net::Http::Method::GET;
        req.path = url.path;
        req.version = Net::Http::Version{.major = 1, .minor = 1};
        try$(req.unparse(output));

        return Ok();
    }

    Res<String> fetch(Mime::Url const& url) override {
        if (url.scheme != "file"s) {
            if (not _next)
                return Error::invalidInput("unsupported URL scheme");

            return _next.unwrap()->fetch(url);
        }

        try$(_sendFetchRequest(url));
        auto buf = try$(_readFileFromResponse());
        return Ok(buf);
    }

    virtual Res<Rc<Transfer>> transfer(Mime::Url const& url) override {
        return Ok(makeRc<HttPipeTransfer>(output, url));
    };
};

Rc<Fetcher> makeFetcher(bool isHTTPipe);

Res<Style::StyleSheet> fetchStylesheet(Fetcher& fetcher, Mime::Url url, Style::Origin origin = Style::Origin::AUTHOR);

void fetchStylesheets(Fetcher& fetcher, Gc::Ref<Dom::Node> node, Style::StyleBook& sb);

Res<Gc::Ref<Dom::Document>> fetchDocument(Fetcher& fetcher, Gc::Heap& heap, Mime::Url const& url);

Res<Gc::Ref<Dom::Document>> loadDocument(Fetcher& fetcher, Gc::Heap& heap, Mime::Url const& url, Mime::Mime const& mime);

Res<Gc::Ref<Dom::Document>> viewSource(Gc::Heap& heap, Mime::Url const& url);

} // namespace Vaev::Driver
