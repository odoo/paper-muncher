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
    Io::Writer& _out;

    virtual Res<usize> _write(Bytes bytes) = 0;

    Res<usize> write(Bytes bytes) override {
        if (isDone)
            return Error::brokenPipe("Cannot write to transfer after it is done.");
        return _write(bytes);
    }

    virtual Res<usize> _done() = 0;

    Res<usize> done() {
        auto written = try$(_done());
        isDone = true;
        return Ok(written);
    }

    ChunkedTransfer(Io::Writer& out) : _out(out){};
};

struct FileChunkTransfer : public ChunkedTransfer {

    FileChunkTransfer(Io::Writer& out) : ChunkedTransfer(out) {}

    Res<usize> _write(Bytes bytes) override {
        return _out.write(bytes);
    }

    Res<usize> _done() override {
        return Ok(0);
    }
};

struct Fetcher {

    virtual Res<String> fetch(Mime::Url const& url) = 0;

    virtual Res<Rc<ChunkedTransfer>> transfer(Mime::Url const& url) = 0;
};

struct FileFetcher : public Fetcher {

    Mime::Url const STDIN_URL = "about:stdin"_url;
    Mime::Url const STDOUT_URL = "about:stdout"_url;

    Opt<Sys::FileWriter> fileWriter;

    Res<String> fetch(Mime::Url const& url) override {
        if (url == STDIN_URL)
            return Io::readAllUtf8(Sys::in());

        auto file = try$(Sys::File::open(url));
        return Io::readAllUtf8(file);
    }

    virtual Res<Rc<ChunkedTransfer>> transfer(Mime::Url const& url) override {
        if (url == STDOUT_URL)
            return Ok(makeRc<FileChunkTransfer>(Sys::out()));

        Sys::FileWriter fileWriter{try$(Sys::File::create(url))};
        return Ok(makeRc<FileChunkTransfer>(fileWriter));
    };
};

Res<Style::StyleSheet> fetchStylesheet(Fetcher& fetcher, Mime::Url url, Style::Origin origin = Style::Origin::AUTHOR);

void fetchStylesheets(Fetcher& fetcher, Gc::Ref<Dom::Node> node, Style::StyleBook& sb);

Res<Gc::Ref<Dom::Document>> fetchDocument(Fetcher& fetcher, Gc::Heap& heap, Mime::Url const& url);

Res<Gc::Ref<Dom::Document>> loadDocument(Fetcher& fetcher, Gc::Heap& heap, Mime::Url const& url, Mime::Mime const& mime);

Res<Gc::Ref<Dom::Document>> viewSource(Gc::Heap& heap, Mime::Url const& url);

} // namespace Vaev::Driver
