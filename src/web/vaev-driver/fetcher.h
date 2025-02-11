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
            return Ok(makeRc<StdoutChunkTransfer>());

        return Ok(makeRc<FileChunkTransfer>(makeRc<Sys::FileWriter>(try$(Sys::File::create(url)))));
    };
};

Res<Style::StyleSheet> fetchStylesheet(Fetcher& fetcher, Mime::Url url, Style::Origin origin = Style::Origin::AUTHOR);

void fetchStylesheets(Fetcher& fetcher, Gc::Ref<Dom::Node> node, Style::StyleBook& sb);

Res<Gc::Ref<Dom::Document>> fetchDocument(Fetcher& fetcher, Gc::Heap& heap, Mime::Url const& url);

Res<Gc::Ref<Dom::Document>> loadDocument(Fetcher& fetcher, Gc::Heap& heap, Mime::Url const& url, Mime::Mime const& mime);

Res<Gc::Ref<Dom::Document>> viewSource(Gc::Heap& heap, Mime::Url const& url);

} // namespace Vaev::Driver
