#pragma once

#include <karm-pdf/canvas.h>
#include <karm-pdf/values.h>
#include <karm-text/ttf.h>

namespace Karm::Print {

// https://adobe-type-tools.github.io/font-tech-notes/pdfs/5014.CIDFont_Spec.pdf
struct CMapAdapter {

    String registry = "Adobe"s;
    String ordering = "Identity"s;
    Pdf::Name CMapName = "maca"s;

    StringBuilder sb;
    MutCursor<Ttf::Cmap::Table> cmapTable;

    Vec<Math::Vec2<usize>> codespaces;
    Vec<Math::Vec2<usize>> codeMappings;

    CMapAdapter(MutCursor<Ttf::Cmap::Table> table) : cmapTable{table} {}

    void add(String s) {
        sb.append(s);
    }

    Bytes print() {
        sb.clear();

        if (cmapTable->type == 4)
            extractCodespaceAndMappingForType4();
        else if (cmapTable->type == 12)
            extractCodespaceAndMappingForType12();
        else
            panic("");

        commentConventions();
        CIDProcset();
        add("%%EndResource\n"s);
        add("%%EOF\n"s);

        return sb.bytes();
    }

    void commentConventions() {
        add("%!PS-Adobe-3.0 Resource-CMap\n"s);
        add("%%DocumentNeededResources: procset CIDInit\n"s);
        add("%%IncludeResource: procset CIDInit\n"s);
        add(Io::format("%%BeginResource: CMap {}\n"s, CMapName).unwrap());
        add(Io::format("%Title: ({} {} {} 0)"s, CMapName, registry, ordering).unwrap());
        add("%%Version: 1"s);
    }

    void CIDProcset() {
        add("/CIDInit /ProcSet findresource begin\n"s);
        CMapResourceDictionary();
        add("end\n"s);
    }

    void CMapResourceDictionary() {
        add("12 dict begin\n"s);
        CMap();
        add("CMapName currentdict /CMap defineresource pop"s);
        add("end\n"s);
    }

    void CMap() {
        add("begincmap\n"s);

        versionControl();
        CMAPNameVersionType();
        uniqueIdentificationNumbers();
        writingMode();
        codespace();
        codeMapping();

        add("endcmap\n"s);
    }

    void versionControl() {
        add("/CIDSystemInfo 3 dict dup begin\n"s);
        add(Io::format("/Registry ({}) def\n"s, registry).unwrap());
        add(Io::format("/Ordering ({}) def\n"s, ordering).unwrap());
        add("/Supplement 0 def\n"s);
        add("end def\n"s);
    }

    void CMAPNameVersionType() {
        add(Io::format("/CMapName /{} def\n"s, CMapName).unwrap());
        // Version is optional
        add("/CMapType 0 def\n"s);
    }

    void uniqueIdentificationNumbers() {
    }

    void writingMode() {
        add("/WMode 0 def\n"s);
    }

    void extractCodespaceAndMappingForType12() {
        auto s = cmapTable->begin().skip(12);
        u32 nGroups = s.nextU32be();

        for (u32 i = 0; i < nGroups; i++) {
            u32 startCode = s.nextU32be();
            u32 endCode = s.nextU32be();

            codespaces.pushBack({startCode, endCode});

            u32 glyphOffset = s.nextU32be();

            for (u32 r = startCode; r <= endCode; ++r) {
                codeMappings.pushBack({r, (r - startCode) + glyphOffset});
            }
        }
    }

    void extractCodespaceAndMappingForType4() {
        u16 segCountX2 = cmapTable->begin().skip(6).nextU16be();
        u16 segCount = segCountX2 / 2;

        for (usize i = 0; i < segCount; i++) {
            auto s = cmapTable->begin().skip(14);

            u16 endCode = s.skip(i * 2).peekU16be();

            // + 2 for reserved padding
            u16 startCode = s.skip(segCountX2 + 2).peekU16be();

            codespaces.pushBack({startCode, endCode});

            u16 idDelta = s.skip(segCountX2).peekI16be();
            u16 idRangeOffset = s.skip(segCountX2).peekU16be();

            if (idRangeOffset == 0) {
                for (u16 code = startCode; code <= endCode; ++code) {
                    codeMappings.pushBack({code, (u16)((code + idDelta) & 0xFFFF)});
                }
            } else {
                for (u16 code = startCode; code <= endCode; ++code) {
                    auto offset = idRangeOffset + (code - startCode) * 2;
                    codeMappings.pushBack({code, s.skip(offset).nextU16be()});
                }
            }
        }
    }

    void codespace() {
        Vec<Tuple<usize, usize>> compressedCodespaces;

        sort(codespaces);

        usize start = 0;
        while (start < codespaces.len()) {
            usize end = start + 1;

            while (end < codespaces.len()) {
                auto lastCodeEnd = codespaces[end - 1].y;
                auto currCodeStart = codespaces[end].x;

                if (lastCodeEnd + 1 == currCodeStart)
                    end++;
                else
                    break;
            }

            compressedCodespaces.pushBack({codespaces[start].x, codespaces[end - 1].y});

            start = end;
        }

        for (usize i = 0; i < (compressedCodespaces.len() + 99) / 100; ++i) {
            usize start = i * 100;
            usize end = min((i + 1) * 100, compressedCodespaces.len());

            add(Io::format("{} begincodespacerange\n", end - start).unwrap());
            for (usize j = start; j < end; ++j) {
                auto [start, end] = compressedCodespaces[j];
                add(Io::format("<{y}> <{y}>\n", start, end).unwrap());
            }
            add("endcodespacerange\n"s);
        }
    }

    void codeMapping() {
        Vec<Tuple<usize, usize, usize>> compressedMappings;

        sort(codeMappings);

        usize start = 0;
        while (start < codeMappings.len()) {
            usize end = start + 1;

            while (end < codeMappings.len()) {
                auto pastCode = codeMappings[end - 1].x;
                auto pastCID = codeMappings[end - 1].y;

                auto currCode = codeMappings[end].x;
                auto currCID = codeMappings[end].y;

                if (pastCode + 1 == currCode and pastCID + 1 == currCID)
                    end++;
                else
                    break;
            }

            compressedMappings.pushBack({
                codeMappings[start].x,
                codeMappings[end - 1].x,
                codeMappings[start].y,
            });

            start = end;
        }

        for (usize i = 0; i < (compressedMappings.len() + 99) / 100; ++i) {
            usize start = i * 100;
            usize end = min((i + 1) * 100, compressedMappings.len());

            add(Io::format("{} begincidrange\n", end - start).unwrap());
            for (usize j = start; j < end; ++j) {
                auto [startCode, endCode, cid] = compressedMappings[j];
                add(Io::format("<{y}> <{y}> {d}\n", startCode, endCode, cid).unwrap());
            }
            add("endcidrange\n"s);
        }
    }
};

struct TrueTypeFontAdapter {

    Rc<Text::TtfFontface> _font;

    // TODO The PostScript language name for the value of BaseFont should be determined in one of two ways:
    Pdf::Name CIDFontName = "banana"s;
    Pdf::Name CMapName = "maca"s;
    String registry = "Adobe"s;
    String ordering = "Identity"s;

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
        // FIXME: copied values from Weasyprint
        return Pdf::Dict{
            {"Registry"s, registry},
            {"Ordering"s, ordering},
            {"Supplement"s, usize{0}},
        };
    }

    Pdf::Stream CMap() {
        // 9.7.5 CMaps
        // 9.7.5.3 Embedded CMap files

        auto cmapTable = _font->_parser.chooseCmap(_font->_parser).unwrap();
        CMapAdapter adapter{&cmapTable};

        auto cmapStream = adapter.print();

        // Embedded CMap files shall conform to the format documented in Adobe Technical Note #5014, subject to these additional constraints:
        // The data shall follow the syntax defined in Adobe Technical Note #5014, Adobe CMap and CIDFont Files Specification.
        return Pdf::Stream{
            .dict = Pdf::Dict{
                {"Type"s, Pdf::Name{"CMap"s}},
                {"Name"s, CMapName},
                {"CIDSystemInfo"s, CIDSystemInfoRef},
                {"Length"s, cmapStream.len()},
            },
            .data = cmapStream
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

} // namespace Karm::Print
