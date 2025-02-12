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
    bool validState = false;
    Io::Writer& _out;

    ChunkedTransfer(Io::Writer& out) : _out(out){};
    virtual Res<Net::Http::Response> done() = 0;
};

struct FileChunkTransfer : public ChunkedTransfer {

    FileChunkTransfer(Io::Writer& out) : ChunkedTransfer(out) {}

    Res<usize> write(Bytes bytes) override {
        return _out.write(bytes);
    }

    Res<Net::Http::Response> done() override {
        return Ok(Net::Http::Response{});
    }
};

struct Fetcher {

    Vec<Rc<Fetcher>> fallbackFetcher;

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

    Opt<Sys::FileWriter> fileWriter;

    virtual Res<Rc<ChunkedTransfer>> transfer(Mime::Url const& url) override {
        if (url == STDOUT_URL)
            return Ok(makeRc<FileChunkTransfer>(Sys::out()));

        fileWriter = try$(Sys::File::create(url));
        return Ok(makeRc<FileChunkTransfer>(fileWriter.unwrap()));
    };
};

struct HttPipeTransfer : public ChunkedTransfer {

    Io::TextEncoder<Utf8> encoder;

    HttPipeTransfer(Io::Writer& out) : ChunkedTransfer(out), encoder(out) {}

    Bytes const lineSeparator = bytes("\r\n"s);

    bool wroteHeader = false;

    Res<usize> _writeHeader() {
        Net::Http::Request req;
        req.method = Karm::Net::Http::Method::POST;
        req.path = ""_url.path;
        req.version = Net::Http::Version{.major = 1, .minor = 1};
        req.add("Transfer-Encoding", "chunked");

        auto written = try$(req.unparse(encoder));

        wroteHeader = true;

        return Ok(written);
    }

    Res<> flushBuffer() {
        if (bw.len() == 0)
            return Ok();

        try$(Io::format(encoder, "{}", bw.len()));
        try$(encoder.write(lineSeparator));
        try$(_out.write(bytes(bw)));
        try$(encoder.write(lineSeparator));
        bw.clear();

        return Ok();
    }

    Vec<Byte> bw;

    Res<usize> write(Bytes b) override {
        usize written = 0;
        if (not wroteHeader) {
            written += try$(_writeHeader());
        }

        bw.pushBack(b);
        if (bw.len() > 1000) {
            try$(flushBuffer());
        }

        return Ok(written);
    }

    Res<Net::Http::Response> done() override {
        try$(flushBuffer());
        try$(encoder.write(bytes("0"s)));
        try$(encoder.write(lineSeparator));
        try$(encoder.write(lineSeparator));

        auto response = try$(Karm::Net::Http::Response::read(Sys::in()));
        auto body = try$(response.readBody(Sys::in()));

        if (response.code != Net::Http::Code::OK)
            return Error::invalidData("Unexpected HTTP response code");

        if (body)
            return Error::invalidInput("Unexpected body in response");

        return Ok(response);
    }
};

struct HttpPipe : public Fetcher {

    virtual ~HttpPipe() = default;

    Io::Reader& input = Sys::in();

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

    int cnt = 0;

    Res<String> fetch(Mime::Url const& url) override {
        // FIXME: should be a != supported_scheme
        if (url.scheme == "bundle"s) {
            if (fallbackFetcher)
                return fallbackFetcher[0]->fetch(url);
            else
                return Error::invalidInput("Unsupported URL scheme");
        }

        Karm::Net::Http::Request req;
        req.method = Karm::Net::Http::Method::GET;
        req.path = url.path;
        req.version = Net::Http::Version{.major = 1, .minor = 1};
        try$(req.unparse(Sys::out()));

        auto buf = try$(_readFileFromResponse());

        return Ok(buf);
    }

    virtual Res<Rc<ChunkedTransfer>> transfer(Mime::Url const&) override {
        return Ok(makeRc<HttPipeTransfer>(Sys::out()));
    };
};

Rc<Fetcher> makeFetcher(bool isHTTPipe);

Res<Style::StyleSheet> fetchStylesheet(Fetcher& fetcher, Mime::Url url, Style::Origin origin = Style::Origin::AUTHOR);

void fetchStylesheets(Fetcher& fetcher, Gc::Ref<Dom::Node> node, Style::StyleBook& sb);

Res<Gc::Ref<Dom::Document>> fetchDocument(Fetcher& fetcher, Gc::Heap& heap, Mime::Url const& url);

Res<Gc::Ref<Dom::Document>> loadDocument(Fetcher& fetcher, Gc::Heap& heap, Mime::Url const& url, Mime::Mime const& mime);

Res<Gc::Ref<Dom::Document>> viewSource(Gc::Heap& heap, Mime::Url const& url);

} // namespace Vaev::Driver
