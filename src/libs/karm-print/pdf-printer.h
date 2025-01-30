#pragma once

#include <karm-pdf/canvas.h>
#include <karm-pdf/values.h>
#include <karm-text/ttf.h>

#include "file-printer.h"

namespace Karm::Print {

struct PdfPage {
    PaperStock paper;
    Io::StringWriter data;
};

struct TrueTypeFontAdapter {

    Rc<Text::TtfFontface> _font;

    // TODO The PostScript language name for the value of BaseFont should be determined in one of two ways:
    Pdf::Name CIDFontName = "banana"s;
    Pdf::Name CMapName = "maca"s;

    Pdf::Ref CIDFontRef;
    Pdf::Ref CIDSystemInfoRef;
    Pdf::Ref CMapRef;
    Pdf::Ref fontFileRef;
    Pdf::Ref fontDescriptorRef;
    Pdf::Ref fontRef;

    TrueTypeFontAdapter(Rc<Text::TtfFontface> font, Pdf::Ref& alloc)
        : _font(font),
          CIDFontRef(alloc.alloc()),
          CIDSystemInfoRef(alloc.alloc()),
          CMapRef(alloc.alloc()),
          fontFileRef(alloc.alloc()),
          fontDescriptorRef(alloc.alloc()),
          fontRef(alloc.alloc()) {}

    usize fontDescriptorFlags() {
        // 9.8.2 Font descriptor flags
        // FIXME
        return 0;
    }

    Pdf::Stream fontFile() {
        // 9.9 Embedded font programs
        return Pdf::Stream{
            .dict = Pdf::Dict{
                {"Length"s, _font->_mmap.bytes().len()},
                {"Length1"s, _font->_mmap.bytes().len()},
            },
            .data = _font->_mmap.bytes(),
        };
    }

    Pdf::Dict CIDSystemInfo() {
        // 9.7.3 CIDSystemInfo dictionaries
        // FIXME
        return Pdf::Dict{
            {"Registry"s, String{"dunno2"}},
            {"Ordering"s, String{"dunno2"}},
            {"Supplement"s, usize{0}},
        };
    }

    Pdf::Stream CMap() {
        // 9.7.5 CMaps
        // 9.7.5.3 Embedded CMap files

        // Embedded CMap files shall conform to the format documented in Adobe Technical Note #5014, subject to these additional constraints:
        // The data shall follow the syntax defined in Adobe Technical Note #5014, Adobe CMap and CIDFont Files Specification.
        return Pdf::Stream{
            .dict = Pdf::Dict{
                {"Type"s, Pdf::Name{"CMap"s}},
                {"Name"s, CMapName},
                {"CIDSystemInfo"s, CIDSystemInfoRef},
                {"Length"s, _font->_parser._cmap.bytes().len()},
            },
            .data = _font->_parser._cmap.bytes()
        };
    }

    Pdf::Dict font() {
        // 9.7.6 Type 0 font dictionaries
        return Pdf::Dict{
            {"Type"s, Pdf::Name{"Font"s}},
            {"Subtype"s, Pdf::Name{"Type0"s}},
            {"BaseFont"s, CIDFontName},
            {"Encoding"s, CMapRef},
            {"DescendantFonts"s, Pdf::Array{CIDFontRef}},
        };
    }

    Pdf::Array fontBBox() {
        return Pdf::Array{isize{-177}, isize{-269}, isize{1123}, isize{866}};
    }

    Pdf::Dict fontDescriptors() {
        // 9.8 Font descriptors
        // 9.8.3 Font descriptors for CIDFonts
        return Pdf::Dict{
            {"Type"s, Pdf::Name{"FontDescriptor"s}},
            {"FontName"s, CIDFontName},
            {"Flags"s, fontDescriptorFlags()},
            {"FontBBox"s, fontBBox()},
            {"FontFile2"s, fontFileRef},
            {"ItalicAngle"s, usize{0}}, // FIXME
            {"Ascent"s, usize{720}},    // FIXME
            {"Descent"s, isize{-270}},  // FIXME
            {"CapHeight"s, usize{660}}, // FIXME
            {"StemV"s, usize{105}},     // FIXME
        };
    }

    Pdf::Dict CIDFont() {
        // 9.7.4 CIDFonts
        return Pdf::Dict{
            {"Type"s, Pdf::Name{"Font"s}},
            {"Subtype"s, Pdf::Name{"CIDFontType2"s}},
            {"BaseFont"s, CIDFontName},
            {"CIDSystemInfo"s, CIDSystemInfoRef},
            {"FontDescriptor"s, fontDescriptorRef},
            {"CIDToGIDMap"s, Pdf::Name{"Identity"}}, // /CIDToGIDMap /Identity
        };
    }

    Pdf::Ref addToFile(Pdf::File& file) {
        file.add(CMapRef, CMap());
        file.add(CIDSystemInfoRef, CIDSystemInfo());
        file.add(fontFileRef, fontFile());
        file.add(CIDFontRef, CIDFont());
        file.add(fontDescriptorRef, fontDescriptors());

        file.add(fontRef, font());
        return fontRef;
    }
};

struct PdfPrinter : public FilePrinter {
    Vec<PdfPage> _pages;
    Opt<Pdf::Canvas> _canvas;
    Pdf::FontManager fontManager;

    Gfx::Canvas& beginPage(PaperStock paper) override {
        auto& page = _pages.emplaceBack(paper);
        _canvas = Pdf::Canvas{page.data, paper.size(), &fontManager};

        // NOTE: PDF has the coordinate system origin at the bottom left corner.
        //       But we want to have it at the top left corner.
        _canvas->transform(
            {1, 0, 0, -1, 0, paper.height}
        );

        return *_canvas;
    }

    Pdf::File pdf() {
        Pdf::Ref alloc;

        Pdf::File file;
        file.header = "PDF-2.0"s;

        Pdf::Array pagesKids;
        Pdf::Ref pagesRef = alloc.alloc();

        // Fonts
        Map<usize, Pdf::Ref> fontManagerId2FontObjRef;
        for (auto& [_, value] : fontManager.mapping._els) {
            auto& [id, fontFace] = value;

            TrueTypeFontAdapter ttfAdapter{
                fontFace.cast<Text::TtfFontface>().unwrap(),
                alloc
            };

            auto fontRef = ttfAdapter.addToFile(file);
            fontManagerId2FontObjRef.put(id, fontRef);
        }

        // Page
        for (auto& p : _pages) {
            Pdf::Ref pageRef = alloc.alloc();
            Pdf::Ref contentsRef = alloc.alloc();

            // FIXME: adding all fonts for now on each page; later, we will need to filter by page
            Pdf::Dict pageFontsDict;
            for (auto& [managerId, objRef] : fontManagerId2FontObjRef._els) {
                auto formattedName = Io::format("F{}", managerId).unwrap();
                pageFontsDict.put(formattedName.str(), objRef);
            }

            file.add(
                pageRef,
                Pdf::Dict{
                    {"Type"s, Pdf::Name{"Page"s}},
                    {"Parent"s, pagesRef},
                    {"MediaBox"s,
                     Pdf::Array{
                         usize{0},
                         usize{0},
                         p.paper.width,
                         p.paper.height,
                     }},
                    {
                        "Contents"s,
                        contentsRef,
                    },
                    {
                        "Resources"s,
                        Pdf::Dict{
                            {"Font"s,
                             pageFontsDict
                            },
                        },
                    }
                }
            );

            file.add(
                contentsRef,
                Pdf::Stream{
                    .dict = Pdf::Dict{
                        {"Length"s, p.data.bytes().len()},
                    },
                    .data = p.data.bytes(),
                }
            );

            pagesKids.pushBack(pageRef);
        }

        // Pages
        file.add(
            pagesRef,
            Pdf::Dict{
                {"Type"s, Pdf::Name{"Pages"s}},
                {"Count"s, _pages.len()},
                {"Kids"s, std::move(pagesKids)},
            }
        );

        // Catalog
        auto catalogRef = file.add(
            alloc.alloc(),
            Pdf::Dict{
                {"Type"s, Pdf::Name{"Catalog"s}},
                {"Pages"s, pagesRef},
            }
        );

        // Trailer
        file.trailer = Pdf::Dict{
            {"Size"s, file.body.len() + 1},
            {"Root"s, catalogRef},
        };

        return file;
    }

    void write(Io::Emit& e) {
        pdf().write(e);
    }

    Res<> write(Io::Writer& w) override {
        Io::TextEncoder<> encoder{w};
        Io::Emit e{encoder};
        write(e);
        try$(e.flush());
        return Ok();
    }
};

} // namespace Karm::Print
