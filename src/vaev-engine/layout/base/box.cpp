export module Vaev.Engine:layout.box;

import Karm.Gfx;
import :style.specified;
import :dom.element;
import :layout.svg;

using namespace Karm;

namespace Vaev::Layout {

export struct FormatingContext;
export struct Box;

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
    Rc<Gfx::Prose>,
    Rc<Scene::Node>,
    SVGRoot>;

struct Box : Meta::NoCopy {

    Rc<Style::SpecifiedValues> style;
    Content content = NONE;
    Opt<Rc<FormatingContext>> formatingContext = NONE;
    Gc::Ptr<Dom::Element> origin;
    Vec<Box> _children;

    static Box fromInterruptedInlineBox(Box const& inlineBox) {
        auto oldProse = inlineBox.content.unwrap<Rc<Gfx::Prose>>();
        auto newProse = makeRc<Gfx::Prose>(oldProse->_style);
        newProse->overrideSpanStackWith(*oldProse);
        return {inlineBox.style, std::move(newProse), nullptr};
    }

    Box(Rc<Style::SpecifiedValues> style, Gc::Ptr<Dom::Element> og)
        : style{std::move(style)}, origin{og} {}

    Box(Rc<Style::SpecifiedValues> style, Content content, Gc::Ptr<Dom::Element> og)
        : style{std::move(style)}, content{std::move(content)}, origin{og} {}

    Slice<Box> children() const {
        return _children;
    }

    MutSlice<Box> children() {
        return _children;
    }

    void startInlineBox(Gfx::ProseStyle proseStyle) {
        auto& prose = content.unwrap<Rc<Gfx::Prose>>();
        prose->pushSpan();
        if (proseStyle.color)
            prose->spanColor(proseStyle.color.unwrap());
    }

    void endInlineBox() {
        auto& prose = content.unwrap<Rc<Gfx::Prose>>();
        prose->popSpan();
    }

    void add(Box&& box) {
        if (auto prose = content.is<Rc<Gfx::Prose>>()) {
            (*prose)->append(Gfx::Prose::StrutCell{_children.len()});
        }
        _children.pushBack(std::move(box));
    }

    bool isActiveInlineBox() {
        auto prose = content.is<Rc<Gfx::Prose>>();
        return (prose and (*prose)->_runes.len()) or children().len();
    }

    bool isReplaced() {
        return content.is<Rc<Scene::Node>>() or content.is<SVGRoot>();
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
        } else if (auto prose = content.is<Rc<Gfx::Prose>>()) {
            e.indentNewline();
            e("{}", *prose);
            e.deindent();
        } else if (auto root = content.is<SVGRoot>()) {
            e.indentNewline();
            e("{}", *root);
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

} // namespace Vaev::Layout
