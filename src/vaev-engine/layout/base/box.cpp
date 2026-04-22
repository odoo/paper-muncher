export module Vaev.Engine:layout.box;

import Karm.Gfx;
import :style.computed;
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

struct SvgRootBox;

struct SvgGroupBox {
    using Element = Union<SvgShapeBox, SvgRootBox, Karm::Box<Vaev::Layout::Box>, SvgGroupBox>;
    Vec<Element> elements = {};
    Rc<Style::ComputedValues> style;

    SvgGroupBox(Rc<Style::ComputedValues> style)
        : style(style) {}

    void add(Element&& element);
    void add(Vaev::Layout::Box&& box);

    void repr(Io::Emit& e) const;
};

struct SvgRootBox : SvgGroupBox {
    Opt<SvgViewBox> viewBox;

    SvgRootBox(Rc<Style::ComputedValues> style)
        : SvgGroupBox(style), viewBox(style->svg->viewBox) {}

    void repr(Io::Emit& e) const {
        e("(SVG {} viewBox:{}", buildRectangle(*style), viewBox);
        e.indentNewline();
        for (auto const& el : elements) {
            e("{}", el);
            e.newline();
        }
        e(")");
    }
};

void SvgGroupBox::repr(Io::Emit& e) const {
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
    SvgRootBox>;

struct Box : Meta::NoCopy {
    Rc<Style::ComputedValues> style;
    Content content = NONE;
    Opt<Rc<FormatingContext>> formatingContext = NONE;
    Opt<Dom::OriginatingElement> origin;

    Box(Rc<Style::ComputedValues> style, Opt<Dom::OriginatingElement> og)
        : style{std::move(style)}, origin{og} {}

    Box(Rc<Style::ComputedValues> style, Content content, Opt<Dom::OriginatingElement> og)
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
        return content.is<Rc<Scene::Node>>() or content.is<SvgRootBox>();
    }

    bool isRootElementPrincipalBox() {
        if (not origin)
            return false;
        if (origin->is<Gc::Ref<Dom::PseudoElement>>())
            return false;

        auto el = origin->unwrap<Gc::Ref<Dom::Element>>();
        auto doc = el->ownerDocument();
        if (not doc)
            return false;

        return doc->documentElement() == el;
    }

    bool isRemovedFromFlow() const {
        return style->position == Keywords::ABSOLUTE or
               style->position == Keywords::FIXED or
               style->position.is<RunningPosition>();
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
        } else if (content.is<SvgRootBox>()) {
            e.indentNewline();
            e("{}", content.unwrap<SvgRootBox>());
            e.deindent();
        }
        e(")");
    }
};

void SvgGroupBox::add(Element&& element) {
    elements.pushBack(std::move(element));
}

void SvgGroupBox::add(Vaev::Layout::Box&& box) {
    add(Element{makeBox<Vaev::Layout::Box>(std::move(box))});
}

void InlineBox::add(Box&& b) {
    prose->append(Gfx::Prose::StrutCell{atomicBoxes.len()});
    atomicBoxes.pushBack(makeBox<Box>(std::move(b)));
}

} // namespace Vaev::Layout
