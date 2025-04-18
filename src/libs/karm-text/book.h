#pragma once

#include <karm-base/set.h>
#include <karm-mime/url.h>
#include <karm-sys/mmap.h>

#include "base.h"
#include "font.h"

namespace Karm::Text {

struct FontQuery {
    Family family = GenericFamily::SYSTEM;
    FontWeight weight = FontWeight::REGULAR;
    FontStretch stretch = FontStretch::NORMAL;
    FontStyle style = FontStyle::NORMAL;

    void repr(Io::Emit& e) const {
        e.ln("family: {#}", family);
        e.ln("weight: {}", weight);
        e.ln("stretch: {}", stretch);
        e.ln("style: {}", style);
    }
};

struct FontInfo {
    Mime::Url url;
    FontAttrs attrs;
    Rc<Fontface> face;
};

Str commonFamily(Str lhs, Str rhs);

struct FontBook {
    Vec<FontInfo> _faces;

    // FIXME: these value depend on the correct loading of the bundle
    Array<String, toUnderlyingType(GenericFamily::_LEN)> _genericFamily = {
        /* SERIF */ "Noto Serif"s,
        /* SANS_SERIF */ "Noto Sans"s,
        /* MONOSPACE */ "Fira Code"s,
        /* CURSIVE */ "Dancing Script"s,
        /* FANTASY */ "Excalibur"s,
        /* SYSTEM */ "Noto Serif"s,
        /* EMOJI */ "Noto Emoji"s,
        /* MATH */ "Noto Sans Math"s,
        /* FANGSONG */ "Noto"s,
    };

    void add(FontInfo info) {
        _faces.pushBack(info);
    }

    Res<> load(Mime::Url const& url, Opt<FontAttrs> attrs = NONE);

    Res<> loadAll();

    Vec<String> families() const;

    Str _resolveFamily(Family const& family) const;

    Opt<Rc<Fontface>> queryExact(FontQuery query) const;

    Opt<Rc<Fontface>> queryClosest(FontQuery query) const;

    Vec<Rc<Fontface>> queryFamily(String family) const;
};

} // namespace Karm::Text
