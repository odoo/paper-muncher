#include <karm-sys/file.h>

#include "bmp/decoder.h"
#include "gif/decoder.h"
#include "jpeg/decoder.h"
#include "png/decoder.h"
#include "qoi/decoder.h"
#include "tga/decoder.h"

//
#include "loader.h"

namespace marK::Image {

static Res<Picture> loadBmp(Bytes bytes) {
    auto bmp = try$(Bmp::Decoder::init(bytes));
    auto img = Gfx::Surface::alloc({bmp.width(), bmp.height()});
    try$(bmp.decode(*img));
    return Ok(img);
}

static Res<Picture> loadQoi(Bytes bytes) {
    auto qoi = try$(Qoi::Decoder::init(bytes));
    auto img = Gfx::Surface::alloc({qoi.width(), qoi.height()});
    try$(qoi.decode(*img));
    return Ok(img);
}

static Res<Picture> loadPng(Bytes bytes) {
    auto png = try$(Png::Decoder::init(bytes));
    auto img = Gfx::Surface::alloc({png.width(), png.height()});
    try$(png.decode(*img));
    return Ok(img);
}

static Res<Picture> loadJpeg(Bytes bytes) {
    auto jpeg = try$(Jpeg::Decoder::init(bytes));
    auto img = Gfx::Surface::alloc({jpeg.width(), jpeg.height()});
    try$(jpeg.decode(*img));
    return Ok(img);
}

static Res<Picture> loadTga(Bytes bytes) {
    auto tga = try$(Tga::Decoder::init(bytes));
    auto img = Gfx::Surface::alloc({tga.width(), tga.height()});
    try$(tga.decode(*img));
    return Ok(img);
}

static Res<Picture> loadGif(Bytes bytes) {
    auto gif = try$(Gif::Decoder::init(bytes));
    auto img = Gfx::Surface::alloc({gif.width(), gif.height()});
    try$(gif.decode(*img));
    return Ok(img);
}

Res<Picture> load(Sys::Mmap&& map) {
    if (Bmp::Decoder::sniff(map.bytes())) {
        return loadBmp(map.bytes());
    } else if (Qoi::Decoder::sniff(map.bytes())) {
        return loadQoi(map.bytes());
    } else if (Png::Decoder::sniff(map.bytes())) {
        return loadPng(map.bytes());
    } else if (Jpeg::Decoder::sniff(map.bytes())) {
        return loadJpeg(map.bytes());
    } else if (Tga::Decoder::sniff(map.bytes())) {
        return loadTga(map.bytes());
    } else if (Gif::Decoder::sniff(map.bytes())) {
        return loadGif(map.bytes());
    } else {
        return Error::invalidData("unknown image format");
    }
}

Res<Picture> load(Mime::Url url) {
    auto file = try$(Sys::File::open(url));
    auto map = try$(Sys::mmap().map(file));
    return load(std::move(map));
}

Res<Picture> loadOrFallback(Mime::Url url) {
    if (auto result = load(url); result)
        return result;
    return Ok(Gfx::Surface::fallback());
}

} // namespace marK::Image
