module;

#include <karm/macros>

export module Html5LibTest:treeConstruction;

import Karm.Core;
import Karm.Diag;
import Karm.Gc;
import Karm.Ref;

import Vaev.Engine;

import :testResult;

using namespace Karm;
using namespace Vaev;

namespace Html5LibTest::TreeConstruction {

struct DocumentEmit {
    Io::TextWriter& _writer;
    usize _ident = 0;

    DocumentEmit(Io::TextWriter& writer)
        : _writer(writer) {
    }

    void _indent() {
        _ident++;
    }

    void _deindent() {
        if (_ident == 0) [[unlikely]]
            panic("_deident() underflow");
        _ident--;
    }

    Res<> _insertIndent() {
        for (usize i = 0; i < _ident; i++)
            try$(_writer.writeStr("  "s));
        return Ok();
    }

    Res<> write(Gc::Ref<Dom::Node> node) {
        if (node->nodeType() == Dom::NodeType::DOCUMENT) {
            for (auto child = node->firstChild(); child; child = child->nextSibling()) {
                try$(write(*child));
            }

            return Ok();
        }

        try$(_emit("| "));
        try$(_insertIndent());

        if (node->nodeType() == Dom::NodeType::TEXT) {
            auto text = node->is<Dom::Text>();

            try$(_emit("\"{}\"\n", text->data()));
        } else if (node->nodeType() == Dom::NodeType::COMMENT) {
            auto comment = node->is<Dom::Comment>();
            try$(_emit("<!-- {} -->\n", comment->data()));
        } else if (node->nodeType() == Dom::NodeType::DOCUMENT_TYPE) {
            auto doctype = node->is<Dom::DocumentType>();
            try$(_emit("<!DOCTYPE {}>\n", doctype->name));
        } else if (node->nodeType() == Dom::NodeType::ELEMENT) {
            auto element = node->is<Dom::Element>();

            try$(_emit("<{}>\n", element->qualifiedName.name));

            _indent();

            auto attributes = element->attributes.iterItems() |
                              Collect<Vec<Tuple<Dom::QualifiedName, Rc<Dom::Attr>>>>();

            sort(attributes, [](auto const& a, auto const& b) {
                return a.v0.name <=> b.v0.name;
            });

            for (usize i = 0; i < attributes.len(); i++) {
                auto const& attribute = attributes[i];

                try$(_emit("| "));
                try$(_insertIndent());
                try$(_emit("{}=\"{}\"\n", attribute.v0.name, attribute.v1->value));
            }

            for (auto child = element->firstChild(); child; child = child->nextSibling()) {
                try$(write(*child));
            }

            _deindent();
        } else {
            return Error::unsupported("unsupported node type");
        }

        return Ok();
    }

    template <typename... Ts>
    Res<> _emit(Ts&&... ts) {
        return Io::format(_writer, std::forward<Ts>(ts)...);
    }
};

// https://github.com/html5lib/html5lib-tests/blob/master/tree-construction/README.md
export Res<Result> run(Str input) {
    auto r = Io::BufReader{bytes(input)};
    auto w = Io::BufferWriter{};

    auto data = StringBuilder{};
    auto document = StringBuilder{};

    enum TestParserState {
        IN_DATA_HEADER,
        IN_DATA_FIRST_LINE,
        IN_DATA,
        IN_ERRORS,
        IN_DOCUMENT,
    };

    TestParserState state = IN_DATA_HEADER;

    while (true) {
        Array<u8, 1> newlineDelim = {'\n'};
        auto [read, ok] = try$(Io::readLine(r, w, newlineDelim));

        if (not ok) {
            if (state != IN_DOCUMENT) {
                return Error::invalidData();
            }
            break;
        }

        if (state == IN_DATA_HEADER) {
            if (w.bytes() != "#data\n"_bytes) {
                return Error::invalidData("expected #data header");
            }

            state = IN_DATA_FIRST_LINE;
        } else if (state == IN_DATA_FIRST_LINE) {
            if (w.bytes() == "#errors\n"_bytes) {
                state = IN_ERRORS;
            } else {
                data.append(sub(w.bytes().cast<char>(), 0, w.bytes().len() - 1));
                state = IN_DATA;
            }
        } else if (state == IN_DATA) {
            if (w.bytes() == "#errors\n"_bytes) {
                state = IN_ERRORS;
            } else {
                data.append('\n');
                data.append(sub(w.bytes().cast<char>(), 0, w.bytes().len() - 1));
            }
        } else if (state == IN_ERRORS) {
            if (w.bytes() == "#document\n"_bytes) {
                state = IN_DOCUMENT;
            }
            // TODO: Parse optional sections
        } else if (state == IN_DOCUMENT) {
            document.append(Str{w.bytes().cast<char>()});
        }

        w.clear();
    }

    Gc::Heap gc;
    auto dom = gc.alloc<Dom::Document>(Ref::Url());
    Html::HtmlParser parser{gc, dom};

    auto diags = Diag::Collector{};

    // NOTE: Partial writes are not very well supported for now so we write in one pass.
    parser.write(data.str(), diags);

    auto sw = Io::StringWriter{};
    auto e = DocumentEmit{sw};

    try$(e.write(*dom));

    return Ok(Result{
        .reference = document.str(),
        .actual = {sw.str()},
    });
}

} // namespace Html5LibTest::TreeConstruction
