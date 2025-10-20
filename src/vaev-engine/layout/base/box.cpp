export module Vaev.Engine:layout.box;

import Karm.Gfx;
import :style.specified;
import :dom.element;
import :layout.svg;

using namespace Karm;

namespace Vaev::Layout {

export struct FormatingContext;
export struct Box;

export struct LineBoxes {
    /* NOTE:
    This is a sketch implementation of the data model for LineBoxes. We should be able to:
        -   add different inline elements to it, from different types (Prose, Image, inline-block)
        -   retrieve the added data to be displayed in the same Inline Formatting Context (break lines and display
            into line boxes)
        -   respect different styling for the same line (font, fontsize, color, etc)
    */
    Rc<Gfx::Prose> prose;
    Vec<::Box<Box>> atomicBoxes;

    LineBoxes(Gfx::ProseStyle style) : prose(makeRc<Gfx::Prose>(style)) {}

    LineBoxes(Rc<Gfx::Prose> prose) : prose(prose) {}

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

    static LineBoxes fromInterruptedInlineBox(LineBoxes const& inlineBox) {
        auto oldProse = inlineBox.prose;

        auto newInlineBox = LineBoxes{oldProse->_style};
        newInlineBox.prose->overrideSpanStackWith(*oldProse);

        return newInlineBox;
    }
};

struct SvgRoot;

namespace Svg {

struct Group {
    using Element = Union<Shape, SvgRoot, Karm::Box<Vaev::Layout::Box>, Group>;
    Vec<Element> elements = {};
    Rc<Style::SpecifiedValues> style;

    Group(Rc<Style::SpecifiedValues> style)
        : style(style) {}

    void add(Element&& element);
    void add(Vaev::Layout::Box&& box);

    void repr(Io::Emit& e) const;
};
} // namespace Svg

struct SvgRoot : Svg::Group {
    Opt<ViewBox> viewBox;

    SvgRoot(Rc<Style::SpecifiedValues> style)
        : Svg::Group(style), viewBox(style->svg->viewBox) {}

    void repr(Io::Emit& e) const {
        e("(SVG {} viewBox:{}", Svg::buildRectangle(*style), viewBox);
        e.indentNewline();
        for (auto const& el : elements) {
            e("{}", el);
            e.newline();
        }
        e(")");
    }
};

void Svg::Group::repr(Io::Emit& e) const {
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
    LineBoxes,
    Rc<Scene::Node>,
    SvgRoot>;

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

    bool isRootElementPrincipalBox() {
        if (not origin)
            return false;

        auto doc = origin->ownerDocument();
        if (not doc)
            return false;

        return doc->documentElement() == origin;
    }

    bool isReplaced() const {
        return content.is<Rc<Scene::Node>>() or content.is<SvgRoot>();
    }

    bool isPositioned() const {
        return style->position != Keywords::STATIC;
    }

    bool isFloating() const {
        return style->float_ != Float::NONE;
    }

    bool isBlockLevel() const {
        return not isPositioned() and
               not isFloating() and
               (style->display == Display::BLOCK or
                style->display == Display::TABLE or
                style->display == Display::Item::YES);
    }

    bool isBlockEquivalent() const {
        return (
            style->display == Display::FLOW or
            style->display == Display::FLOW_ROOT or
            style->display == Display::FLEX or
            style->display == Display::GRID or
            style->display == Display::Item::YES or
            style->display == Display::TABLE_CELL or
            isReplaced()
        );
    }

    bool isInlineLevel() const {
        return style->display == Display::INLINE;
    }

    bool hasLineBoxes() const {
        return content.is<LineBoxes>();
    }

    LineBoxes& lineBoxes() {
        return content.unwrap<LineBoxes>("box doesn't have lines boxes");
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
        } else if (content.is<LineBoxes>()) {
            e.indentNewline();
            e("{}", content.unwrap<LineBoxes>());
            e.deindent();
        } else if (content.is<SvgRoot>()) {
            e.indentNewline();
            e("{}", content.unwrap<SvgRoot>());
            e.deindent();
        }
        e(")");
    }
};

void Svg::Group::add(Element&& element) {
    elements.pushBack(std::move(element));
}

void Svg::Group::add(Vaev::Layout::Box&& box) {
    add(Element{makeBox<Vaev::Layout::Box>(std::move(box))});
}

void LineBoxes::add(Box&& b) {
    prose->append(Gfx::Prose::StrutCell{atomicBoxes.len()});
    atomicBoxes.pushBack(makeBox<Box>(std::move(b)));
}

} // namespace Vaev::Layout
