module;

#include <karm-base/base.h>
#include <karm-io/emit.h>
#include <karm-mime/mime.h>

export module Vaev.Engine:markdown;

namespace Vaev::Markdown {

struct Node;

struct Document {
    Vec<Node> text;

    void repr(Io::Emit& e) {
        e("(document {})", text);
    }
};

struct Heading {
    Box<Node> text;
    usize hLevel;

    void repr(Io::Emit& e) {
        e("(h{} {})", hLevel, text);
    }
};

struct Code {
    String language;
    String code;

    void repr(Io::Emit& e) {
        e("(code {} {})", language, code);
    }
};

struct Span {
    enum struct Type {
        NORMAL,
        BOLD,
        ITALIC,
        UNDERLINE,
        STRIKETHROUGH,

        _LEN,
    };
    Type type;
    Vec<Node> text;

    void repr(Io::Emit& e) {
        e("(span {} {})", type, text);
    }
};

struct Paragraph {
    Box<Node> text;

    void repr(Io::Emit& e) {
        e("(p {})", text);
    }
};

struct Link {
    Box<Node> text;
    Mime::Url url;

    void repr(Io::Emit& e) {
        e("(link {} {})", text, url);
    }
};

struct Img {
    Box<Node> text;
    String url;

    void repr(Io::Emit& e) {}
};

struct Quote {
    Box<Node> text;

    void repr(Io::Emit& e) {}
};

struct Item {
    enum struct Type {
        UNORDERED,
        ORDERED,
        CHECKED,
        UNCHECKED
    };

    Type type;
    usize index;
    Box<Node> text;

    void repr(Io::Emit& e) {}
};

struct List {
    Vec<Item> items;

    void repr(Io::Emit& e) {}
};

using _Node = Union<
    Heading,
    Code,
    Span,
    Paragraph,
    Link,
    Quote,
    Item,
    List,
    String>;

struct Node : _Node {
    using _Node::_Node;

    void repr(Io::Emit& e) {}
};

export Document parse(Str s);
export Document parse(Io::SScan& s);

export void unparse(Document, Io::TextWriter& tw);

Res<> _renderNode(Node const& node, Io::TextWriter& tw) {
    node.visit(Visitor{
        [&](Heading const& n) -> Res<> {
            try$(Io::format(tw, "<h{}>", n.hLevel));
            try$(_renderNode(*n.text, tw));
            try$(Io::format(tw, "</h{}>", n.hLevel));
        },
        [&](Code const& n) -> Res<> {
            try$(Io::format(tw, "<code data-lang=\"{}\">{}</code>", n.language, n.code));
        },
    });
}

export Res<> render(Document const& doc, Io::TextWriter& tw) {
}

} // namespace Vaev::Markdown