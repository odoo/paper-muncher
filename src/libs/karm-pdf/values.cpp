#include "values.h"

namespace Karm::Pdf {

void Name::write(Io::Emit& e) const {
    e("/{}", str());
}

void Array::write(Io::Emit& e) const {
    e('[');
    for (usize i = 0; i < len(); ++i) {
        if (i > 0) {
            e(' ');
        }
        buf()[i].write(e);
    }
    e(']');
}

void Dict::write(Io::Emit& e) const {
    e("<<\n");
    for (auto const& [k, v] : iter()) {
        e('/');
        e(k);
        e(' ');
        v.write(e);
        e('\n');
    }
    e(">>");
}

void Stream::write(Io::Emit& e) const {
    dict.write(e);
    e("stream\n");
    (void)e.flush();
    (void)e.write(data);
    e("\nendstream\n");
}

void Value::write(Io::Emit& e) const {
    visit(Visitor{
        [&](None) {
            e("null");
        },
        [&](Ref const& ref) {
            e("{} {} R", ref.num, ref.gen);
        },
        [&](bool b) {
            e(b ? "true" : "false");
        },
        [&](isize i) {
            e("{}", i);
        },
        [&](usize i) {
            e("{}", i);
        },
        [&](f64 f) {
            e("{}", f);
        },
        [&](String const& s) {
            e("({})", s);
        },
        [&](auto const& v) {
            v.write(e);
        }
    });
}

void File::write(Io::Emit& e) const {
    e("%{}\n", header);
    e("%Powered By Karm PDF 🐢🏳️‍⚧️🦔\n", header);

    XRef xref;

    for (auto const& [k, v] : body.iter()) {
        xref.add(e.total(), k.gen);
        e("{} {} obj\n", k.num, k.gen);
        v.write(e);
        e("\nendobj\n");
    }

    (void)e.flush();
    auto startxref = e.total();
    e("xref\n");
    xref.write(e);

    e("trailer\n");
    trailer.write(e);

    e("startxref\n");
    e("{}\n", startxref);
    e("%%EOF");
}

void XRef::write(Io::Emit& e) const {
    e("0 {}\n", entries.len() + 1);
    for (usize i = 0; i < entries.len(); ++i) {
        auto const& entry = entries[i];
        if (entry.used) {
            e("{:010} {:05} n\n", entry.offset, entry.gen);
        }
    }
}

} // namespace Karm::Pdf
