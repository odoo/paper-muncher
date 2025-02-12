#pragma once

#include <karm-gc/heap.h>
#include <karm-mime/url.h>
#include <karm-net/http/http.h>
#include <karm-sys/chan.h>
#include <karm-sys/file.h>
#include <vaev-dom/document.h>
#include <vaev-style/stylesheet.h>

namespace Vaev::Driver {

struct ChunkedTransfer : public Io::Writer {
    bool isDone = false;

    Res<> _raiseIfDone() {
        if (isDone)
            return Error::brokenPipe("Cannot write to transfer after it is done.");
        return Ok();
    }

    virtual Res<usize> done() = 0;
};

struct FileChunkTransfer : public ChunkedTransfer {

    Rc<Sys::FileWriter> out;

    FileChunkTransfer(Rc<Sys::FileWriter> out) : out(out) {}

    Res<usize> write(Bytes bytes) override {
        try$(_raiseIfDone());
        return out->write(bytes);
    }

    Res<usize> done() override {
        isDone = true;
        return Ok(0);
    }
};

struct StdoutChunkTransfer : public ChunkedTransfer {

    Res<usize> write(Bytes bytes) override {
        try$(_raiseIfDone());
        return Sys::out().write(bytes);
    }

    Res<usize> done() override {
        isDone = true;
        return Ok(0);
    }
};

struct Fetcher {

    Opt<Rc<Fetcher>> fallback;

    Fetcher(Opt<Rc<Fetcher>> fallback = NONE) : fallback(fallback) {}

    virtual Res<String> fetch(Mime::Url const& url) = 0;

    virtual Res<Rc<ChunkedTransfer>> transfer(Mime::Url const& url) = 0;
};

struct FileFetcher : public Fetcher {

    virtual ~FileFetcher() = default;

    Mime::Url const STDIN_URL = "about:stdin"_url;
    Mime::Url const STDOUT_URL = "about:stdout"_url;

    Res<String> fetch(Mime::Url const& url) override {
        if (url == STDIN_URL)
            return Io::readAllUtf8(Sys::in());

        auto file = try$(Sys::File::open(url));
        return Io::readAllUtf8(file);
    }

    virtual Res<Rc<ChunkedTransfer>> transfer(Mime::Url const& url) override {
        if (url == STDOUT_URL)
            return Ok(makeRc<StdoutChunkTransfer>());

        return Ok(makeRc<FileChunkTransfer>(makeRc<Sys::FileWriter>(try$(Sys::File::create(url)))));
    };
};

struct HttPipeTransfer : public ChunkedTransfer {

    inline static Buf<Byte> const lineSeparator = bytes("\r\n"s);
    static usize const MAX_BUF_SIZE = 512;

    Io::Writer& _out;
    Io::Count cntedOut;
    Io::TextEncoder<Utf8> encoder;
    Mime::Url const& destUrl;

    HttPipeTransfer(Mime::Url const& url, Io::Writer& out = Sys::out())
        : _out(out), cntedOut(out), encoder(cntedOut), destUrl{url} {}

    bool wroteHeader = false;

    Res<> _writeHeader() {
        Net::Http::Request req;
        req.method = Karm::Net::Http::Method::POST;
        req.path = destUrl.path;
        req.version = Net::Http::Version{.major = 1, .minor = 1};
        req.add("Transfer-Encoding", "chunked");

        try$(req.unparse(encoder));

        wroteHeader = true;
        return Ok();
    }

    Io::BufferWriter bw;

    Res<> flushBuffer() {
        if (bw.bytes().len() == 0)
            return Ok();

        try$(Io::format(encoder, "{}", bw.bytes().len()));
        try$(encoder.write(lineSeparator));
        try$(cntedOut.write(bw.bytes()));
        try$(encoder.write(lineSeparator));
        bw.clear();

        return Ok();
    }

    Res<usize> write(Bytes b) override {
        try$(_raiseIfDone());

        auto startWriterPos = cntedOut._pos;
        if (not wroteHeader)
            try$(_writeHeader());

        try$(bw.write(b));
        if (bw.bytes().len() >= MAX_BUF_SIZE)
            try$(flushBuffer());

        return Ok(cntedOut._pos - startWriterPos);
    }

    Res<usize> done() override {
        auto startWriterPos = cntedOut._pos;

        try$(flushBuffer());
        try$(encoder.write(bytes("0"s)));
        try$(encoder.write(lineSeparator));
        try$(encoder.write(lineSeparator));

        isDone = true;
        return Ok(cntedOut._pos - startWriterPos);
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
        auto response = try$(Karm::Net::Http::Response::read(input));
        auto body = try$(response.readBody(input));

        if (response.code != Karm::Net::Http::Code::OK)
            return Error::invalidData("Unexpected HTTP code");

        if (not body)
            return Error::invalidData("Body is expected");

        Io::BufReader br{body.unwrap()};
        auto res = try$(Io::readAllUtf8(br));

        return Ok(res);
    }

    Res<> _sendFetchRequest(Mime::Url const& url) {
        Karm::Net::Http::Request req;
        req.method = Karm::Net::Http::Method::GET;
        req.path = url.path;
        req.version = Net::Http::Version{.major = 1, .minor = 1};
        try$(req.unparse(output));

        return Ok();
    }

    Res<String> fetch(Mime::Url const& url) override {
        if (url.scheme != "file"s) {
            if (not fallback)
                return Error::invalidInput("Unsupported URL scheme");

            return fallback.unwrap()->fetch(url);
        }

        try$(_sendFetchRequest(url));
        auto buf = try$(_readFileFromResponse());
        return Ok(buf);
    }

    virtual Res<Rc<ChunkedTransfer>> transfer(Mime::Url const& url) override {
        return Ok(makeRc<HttPipeTransfer>(url, output));
    };
};

Rc<Fetcher> makeFetcher(bool isHTTPipe);

Res<Style::StyleSheet> fetchStylesheet(Fetcher& fetcher, Mime::Url url, Style::Origin origin = Style::Origin::AUTHOR);

void fetchStylesheets(Fetcher& fetcher, Gc::Ref<Dom::Node> node, Style::StyleBook& sb);

Res<Gc::Ref<Dom::Document>> fetchDocument(Fetcher& fetcher, Gc::Heap& heap, Mime::Url const& url);

Res<Gc::Ref<Dom::Document>> loadDocument(Fetcher& fetcher, Gc::Heap& heap, Mime::Url const& url, Mime::Mime const& mime);

Res<Gc::Ref<Dom::Document>> viewSource(Gc::Heap& heap, Mime::Url const& url);

} // namespace Vaev::Driver
