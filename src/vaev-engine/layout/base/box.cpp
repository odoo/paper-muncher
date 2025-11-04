export module Vaev.Engine:layout.box;

import Karm.Gfx;
import :style.specified;
import :dom.element;
import :layout.svg;

using namespace Karm;

namespace Vaev::Layout {

export struct FormatingContext;
export struct Box;

export struct InlineBox {
    /* NOTE:
    This is a sketch implementation of the data model for InlineBox. We should be able to:
        -   add different inline elements to it, from different types (Prose, Image, inline-block)
        -   retrieve the added data to be displayed in the same Inline Formatting Context (break lines and display
            into line boxes)
        -   respect different styling for the same line (font, fontsize, color, etc)
    */
    Rc<Gfx::Prose> prose;
    Vec<::Box<Box>> atomicBoxes;

    InlineBox(Gfx::ProseStyle style) : prose(makeRc<Gfx::Prose>(style)) {}

    InlineBox(Rc<Gfx::Prose> prose) : prose(prose) {}

    void startInlineBox(Gfx::ProseStyle proseStyle) {
        // FIXME: ugly workaround while we dont fix the Prose data structure
        prose->pushSpan();
        if (proseStyle.color)
            prose->spanColor(proseStyle.color.unwrap());
    }

    void endInlineBox() {
        prose->popSpan();
    }

    void add(Box&& b);

    bool active() {
        return prose->_runes.len() or atomicBoxes.len();
    }

    void repr(Io::Emit& e) const {
        e("(inline box {}", prose->_runes);
        e.indentNewline();
        for (auto& c : atomicBoxes) {
            e("{}", c);
            e.newline();
        }
        e.deindent();
        e(")");
    }

    static InlineBox fromInterruptedInlineBox(InlineBox const& inlineBox) {
        auto oldProse = inlineBox.prose;

        auto newInlineBox = InlineBox{oldProse->_style};
        newInlineBox.prose->overrideSpanStackWith(*oldProse);

        return newInlineBox;
    }
};

struct SVGRoot;

namespace SVG {

struct Group {
    using Element = Union<Shape, SVGRoot, Karm::Box<Vaev::Layout::Box>, Group>;
    Vec<Element> elements = {};
    Rc<Style::SpecifiedValues> style;

    Group(Rc<Style::SpecifiedValues> style)
        : style(style) {}

    void add(Element&& element);
    void add(Vaev::Layout::Box&& box);

    void repr(Io::Emit& e) const;
};
} // namespace SVG

struct SVGRoot : SVG::Group {
    Opt<ViewBox> viewBox;

    SVGRoot(Rc<Style::SpecifiedValues> style)
        : SVG::Group(style), viewBox(style->svg->viewBox) {}

    void repr(Io::Emit& e) const {
        e("(SVG {} viewBox:{}", SVG::buildRectangle(*style), viewBox);
        e.indentNewline();
        for (auto const& el : elements) {
            e("{}", el);
            e.newline();
        }
        e(")");
    }
};

void SVG::Group::repr(Io::Emit& e) const {
    e("(Group {} ");
    e.indentNewline();
    for (auto const& el : elements) {
        e("{}", el);
        e.newline();
    }
    e(")");
}

export using Content = Union<
    None,
    Vec<Box>,
    InlineBox,
    Rc<Scene::Node>,
    SVGRoot>;

struct Box : Meta::NoCopy {
    Rc<Style::SpecifiedValues> style;
    Content content = NONE;
    Opt<Rc<FormatingContext>> formatingContext = NONE;
    Gc::Ptr<Dom::Element> origin;

    Box(Rc<Style::SpecifiedValues> style, Gc::Ptr<Dom::Element> og)
        : style{std::move(style)}, origin{og} {}

    Box(Rc<Style::SpecifiedValues> style, Content content, Gc::Ptr<Dom::Element> og)
        : style{std::move(style)}, content{std::move(content)}, origin{og} {}

    Slice<Box> children() const {
        if (auto children = content.is<Vec<Box>>())
            return *children;
        return {};
    }

    MutSlice<Box> children() {
        if (auto children = content.is<Vec<Box>>()) {
            return *children;
        }
        return {};
    }

    void add(Box&& box) {
        if (content.is<None>())
            content = Vec<Box>{};

        if (auto children = content.is<Vec<Box>>()) {
            children->pushBack(std::move(box));
        }
    }

    bool isReplaced() {
        return content.is<Rc<Scene::Node>>() or content.is<SVGRoot>();
    }

    bool isPositioned() const {
        return style->position != Keywords::STATIC;
    }

    void repr(Io::Emit& e) const {
        e("(box {} {}", style->display, style->position);
        if (children()) {
            e.indentNewline();
            for (auto& c : children()) {
                c.repr(e);
                e.newline();
            }
            e.deindent();
        } else if (content.is<InlineBox>()) {
            e.indentNewline();
            e("{}", content.unwrap<InlineBox>());
            e.deindent();
        } else if (content.is<SVGRoot>()) {
            e.indentNewline();
            e("{}", content.unwrap<SVGRoot>());
            e.deindent();
        }
        e(")");
    }
};

void SVG::Group::add(Element&& element) {
    elements.pushBack(std::move(element));
}

void SVG::Group::add(Vaev::Layout::Box&& box) {
    add(Element{makeBox<Vaev::Layout::Box>(std::move(box))});
}

void InlineBox::add(Box&& b) {
    prose->append(Gfx::Prose::StrutCell{atomicBoxes.len()});
    atomicBoxes.pushBack(makeBox<Box>(std::move(b)));
}

} // namespace Vaev::Layout
