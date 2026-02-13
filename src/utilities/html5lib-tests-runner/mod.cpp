module;

#include <karm/macros>

export module Html5LibTest;

import Karm.Core;
import Karm.Diag;
import Karm.Gc;
import Karm.Ref;
import Karm.Tty;
import Karm.Logger;

import Vaev.Engine;

using namespace Karm;
using namespace Vaev;

namespace Html5LibTest {

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

    Res<> write(Gc::Ref<Dom::Node> const& node) {
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

            auto attributes = element->attributes.iterUnordered().collect<Vec<Tuple<Dom::QualifiedName, Rc<Dom::Attr>>>>();

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

export struct TestResult {
    String reference;
    String actual;
    bool passed;
};

static auto const RE_DATA_HEADER = Re::word("#data");
static auto const RE_ERRORS_HEADER = Re::word("#errors");
static auto const RE_DOCUMENT_HEADER = Re::word("#document");

static auto const RE_DATA = Re::until(
    Re::single('\n') & RE_ERRORS_HEADER
);

static auto const RE_ERRORS = Re::until(
    Re::single('\n') & RE_DOCUMENT_HEADER & Re::single('\n')
);

// https://github.com/html5lib/html5lib-tests/blob/master/tree-construction/README.md
export Res<TestResult> run(Str input) {
    auto s = Io::SScan{input};

    if (not s.eat(RE_DATA_HEADER & Re::single('\n'))) {
        return Error::invalidData();
    }

    auto data = s.token(RE_DATA);

    if (not s.eat(Re::single('\n') & RE_ERRORS_HEADER)) {
        return Error::invalidData();
    }

    s.token(RE_ERRORS);

    if (not s.eat(Re::single('\n') & RE_DOCUMENT_HEADER & Re::single('\n'))) {
        return Error::invalidData();
    }

    auto document = s.remStr();

    Gc::Heap gc;
    auto dom = gc.alloc<Dom::Document>(Ref::Url());
    Html::HtmlParser parser{gc, dom};

    auto w = Io::StringWriter{};
    auto e = DocumentEmit{w};

    auto diags = Diag::Collector{};
    parser.write(data, diags);

    try$(e.write(*dom));

    return Ok(TestResult{
        .reference = document,
        .actual = w.str(),
        .passed = w.str() == document,
    });
}

} // namespace Html5LibTest
