#pragma once

#include <karm-pdf/canvas.h>
#include <karm-pdf/values.h>
#include <karm-text/ttf.h>

namespace Karm::Print {

struct TtfGlyphInfoAdapter {

    usize const CODESPACE = 1 << 16;

    Rc<Text::TtfFontface> _font;

    Map<u16, u16> codeMappings;

    TtfGlyphInfoAdapter(Rc<Text::TtfFontface> font, Map<u16, u16> mappings)
        : _font{font}, codeMappings{mappings} {}

    Map<u16, u16> static extractMappingForType12(Rc<Text::TtfFontface> font) {
        Map<u16, u16> codeMappings;

        auto s = font->_parser._cmapTable.begin().skip(12);
        u32 nGroups = s.nextU32be();

        for (u32 i = 0; i < nGroups; i++) {
            u32 startCode = s.nextU32be();
            u32 endCode = s.nextU32be();

            u32 glyphOffset = s.nextU32be();

            for (u32 r = startCode; r <= endCode; ++r) {
                codeMappings.put(r, (r - startCode) + glyphOffset);
            }
        }
        return codeMappings;
    }

    Map<u16, u16> static extractMappingForType4(Rc<Text::TtfFontface> font) {
        Map<u16, u16> codeMappings;

        u16 segCountX2 = font->_parser._cmapTable.begin().skip(6).nextU16be();
        u16 segCount = segCountX2 / 2;

        for (u16 i = 0; i < segCount; i++) {
            auto s = font->_parser._cmapTable.begin().skip(14);

            u16 endCode = s.skip(i * 2).peekU16be();

            // + 2 for reserved padding
            u16 startCode = s.skip(segCountX2 + 2).peekU16be();

            u16 idDelta = s.skip(segCountX2).peekI16be();
            u16 idRangeOffset = s.skip(segCountX2).peekU16be();

            if (idRangeOffset == 0) {
                for (u16 code = startCode; code <= endCode; ++code) {
                    codeMappings.put(code, (u16)((code + idDelta) & 0xFFFF));
                }
            } else {
                for (u16 code = startCode; code <= endCode; ++code) {
                    auto offset = idRangeOffset + (code - startCode) * 2;
                    codeMappings.put(code, s.skip(offset).nextU16be());
                }
            }
        }

        return codeMappings;
    }

    static TtfGlyphInfoAdapter build(Rc<Text::TtfFontface> font) {
        Map<u16, u16> codeMappings;
        if (font->_parser._cmapTable.type == 4)
            codeMappings = extractMappingForType4(font);
        else if (font->_parser._cmapTable.type == 12)
            codeMappings = extractMappingForType12(font);
        return TtfGlyphInfoAdapter{font, codeMappings};
    }

    Pdf::Array fontBBox() {
        // Values in the glyph coordinate system
        f64 xMin = 0;
        f64 xMax = 0;
        f64 yMin = 0;
        f64 yMax = 0;

        for (auto [_, GID] : codeMappings.iter()) {
            Text::Glyph glyph{.index = GID, .font = 0};
            auto metrics = _font->_parser.glyphMetrics(glyph);

            xMin = min(xMin, metrics.x - metrics.width);
            xMax = max(xMax, metrics.x);
            yMin = min(yMin, metrics.y - metrics.height);
            yMax = max(yMax, metrics.y);
        }

        return Pdf::Array{xMin, yMin, xMax, yMax};
    }

    Pdf::Array widths() {
        // 9.7.4.3 Glyph metrics in CIDFonts

        Pdf::Array allWidths;

        u16 currGroupStart;
        Pdf::Array currGroupW;

        auto flushCollectedWidths = [&]() {
            if (currGroupW.len() == 0)
                return;
            allWidths.pushBack(usize{currGroupStart});
            allWidths.pushBack(Pdf::Array{currGroupW});
            currGroupW = {};
        };

        Opt<u16> prevCid;
        for (auto const& [cid, gid] : codeMappings.iter()) {

            if (prevCid and prevCid.unwrap() + 1 != cid) {
                flushCollectedWidths();
                continue;
            }

            if (currGroupW.len() == 0)
                currGroupStart = cid;

            Text::Glyph glyph{.index = gid, .font = 0};
            // We are using the Em unit to relate font design units and width pdf units
            // The unit in widths array is (Em/1000) (9.2.4 Glyph positioning and metrics)
            auto gliyphAdvInEm = _font->advance(glyph);
            currGroupW.pushBack(gliyphAdvInEm * 1000);

            prevCid = cid;
        }

        flushCollectedWidths();

        return allWidths;
    }

    Buf<Byte> CIDToGIDMap() {
        Buf<Byte> buf;

        u16 const BYTE_MASK = 255;

        usize consecutiveUnmappedCIDs = 0;
        for (usize i = 0; i < CODESPACE; ++i) {
            auto glyph = codeMappings.tryGet(i);

            if (not glyph) {
                consecutiveUnmappedCIDs++;
                continue;
            }

            for (usize j = 0; j < consecutiveUnmappedCIDs; ++j) {
                buf.insert(buf.len(), 0);
                buf.insert(buf.len(), 0);
            }
            consecutiveUnmappedCIDs = 0;

            auto gid = glyph.unwrap();
            buf.insert(buf.len(), (gid >> 8) & BYTE_MASK);
            buf.insert(buf.len(), gid & BYTE_MASK);
        }

        return buf;
    }
};

struct TrueTypeFontAdapter {

    Rc<Text::TtfFontface> _font;

    String registry = "Adobe"s;
    String ordering = "Identity"s;

    Pdf::Ref CIDFontRef;
    Pdf::Ref CIDSystemInfoRef;
    Pdf::Ref CIDToGIDMapRef;
    Pdf::Ref fontFileRef;
    Pdf::Ref fontDescriptorRef;
    Pdf::Ref fontRef;

    TtfGlyphInfoAdapter ttfGlyphInfoAdapter;

    Pdf::Name CIDFontName;

    TrueTypeFontAdapter(Rc<Text::TtfFontface> font, Pdf::Ref& alloc)
        : _font(font),
          CIDFontRef(alloc.alloc()),
          CIDSystemInfoRef(alloc.alloc()),
          CIDToGIDMapRef(alloc.alloc()),
          fontFileRef(alloc.alloc()),
          fontDescriptorRef(alloc.alloc()),
          fontRef(alloc.alloc()),
          ttfGlyphInfoAdapter{
              TtfGlyphInfoAdapter::build(font)
          },
          CIDFontName{
              font->_parser._name.string(font->_parser._name.lookupRecord(Ttf::Name::POSTSCRIPT)).str()
          } {
    }

    usize fontDescriptorFlags() {
        // 9.8.2 Font descriptor flags
        // FIXME

        usize flag = 0;

        auto attrs = _font->attrs();
        if (attrs.monospace == Text::Monospace::YES)
            flag ^= 1;

        if (attrs.style == Text::FontStyle::ITALIC)
            flag ^= (1 << 6);

        return flag;
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
        return Pdf::Dict{
            {"Registry"s, registry},
            {"Ordering"s, ordering},
            {"Supplement"s, usize{0}},
        };
    }

    Pdf::Dict font() {
        // 9.7.6 Type 0 font dictionaries
        // When text is selected in a PDF file, the value selected is after "Encoding" is applied
        return Pdf::Dict{
            {"Type"s, Pdf::Name{"Font"s}},
            {"Subtype"s, Pdf::Name{"Type0"s}},
            {"BaseFont"s, CIDFontName},
            {"Encoding"s, Pdf::Name{"Identity-H"}},
            {"DescendantFonts"s, Pdf::Array{CIDFontRef}},
        };
    }

    Pdf::Dict fontDescriptors() {
        auto metrics = _font->metrics();

        // 9.8 Font descriptors
        // 9.8.3 Font descriptors for CIDFonts
        return Pdf::Dict{
            {"Type"s, Pdf::Name{"FontDescriptor"s}},
            {"FontName"s, CIDFontName},
            {"Flags"s, fontDescriptorFlags()},
            {"FontBBox"s, ttfGlyphInfoAdapter.fontBBox()},
            {"FontFile2"s, fontFileRef},
            {"ItalicAngle"s, _font->_parser._post.italicAngle()},
            {"Ascent"s, metrics.ascend * 1000},
            {"Descent"s, metrics.descend * -1000},
            {"CapHeight"s, metrics.ascend * 1000}, // FIXME
            {"StemV"s, usize{0}},
        };
    }

    Pdf::Stream CIDToGIDMap() {
        auto stream = ttfGlyphInfoAdapter.CIDToGIDMap();
        return Pdf::Stream{
            .dict = Pdf::Dict{
                {"Length"s, stream.len()},
            },
            .data = std::move(stream)
        };
    }

    Pdf::Dict CIDFont() {
        // 9.7.4 CIDFonts
        return Pdf::Dict{
            {"Type"s, Pdf::Name{"Font"s}},
            {"Subtype"s, Pdf::Name{"CIDFontType2"s}},
            {"BaseFont"s, CIDFontName},
            {"CIDSystemInfo"s, CIDSystemInfoRef},
            {"W"s, ttfGlyphInfoAdapter.widths()},
            {"FontDescriptor"s, fontDescriptorRef},
            {"CIDToGIDMap"s, CIDToGIDMapRef},
        };
    }

    Pdf::Ref addToFile(Pdf::File& file) {
        file.add(CIDToGIDMapRef, CIDToGIDMap());
        file.add(CIDSystemInfoRef, CIDSystemInfo());
        file.add(fontFileRef, fontFile());
        file.add(CIDFontRef, CIDFont());
        file.add(fontDescriptorRef, fontDescriptors());

        file.add(fontRef, font());
        return fontRef;
    }
};

} // namespace Karm::Print
