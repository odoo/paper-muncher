export module Vaev.Engine:layout.box;

import Karm.Gfx;
import :style.computed;
import :dom.element;
import :dom.document;

using namespace Karm;

namespace Vaev::Layout {

export struct FormatingContext;
export struct Box;

export using Content = Union<
    None,
    Rc<Gfx::Prose>,
    Rc<Scene::Node>,
    SvgShapeElement,
    SvgViewBox>;

struct Box : Meta::NoCopy {
    Rc<Style::ComputedValues> style;
    Content content = NONE;
    Vec<Box> _children;
    Opt<Rc<FormatingContext>> formatingContext = NONE;
    Opt<Dom::OriginatingElement> origin;

    static Box fromInterruptedInlineBox(Box const& inlineBox) {
        auto oldProse = inlineBox.content.unwrap<Rc<Gfx::Prose>>();
        Rc<Gfx::Prose> prose = makeRc<Gfx::Prose>(oldProse->_style);
        prose->overrideSpanStackWith(*oldProse);
        return Box(inlineBox.style, prose, inlineBox.origin);
    }

    Box(Rc<Style::ComputedValues> style, Opt<Dom::OriginatingElement> og)
        : style{std::move(style)}, origin{og} {}

    Box(Rc<Style::ComputedValues> style, Content content, Opt<Dom::OriginatingElement> og)
        : style{std::move(style)}, content{std::move(content)}, origin{og} {}

    Slice<Box> children() const {
        return _children;
    }

    MutSlice<Box> children() {
        return _children;
    }

    void add(Box&& box) {
        if (auto it = content.is<Rc<Gfx::Prose>>())
            (*it)->append(Gfx::Prose::StrutCell{_children.len()});
        _children.pushBack(std::move(box));
    }

    void startInlineBox(Gfx::ProseStyle proseStyle) {
        auto prose = content.unwrap<Rc<Gfx::Prose>>();
        prose->pushSpan();
        if (proseStyle.color)
            prose->spanColor(proseStyle.color.unwrap());
    }

    void endInlineBox() {
        auto& prose = content.unwrap<Rc<Gfx::Prose>>();
        prose->popSpan();
    }

    bool isSvg() const {
        if (not origin)
            return false;

        if (auto it = origin->is<Gc::Ref<Dom::Element>>())
            return (*it)->qualifiedName.ns == Svg::NAMESPACE;
        return false;
    }

    bool isSvgRootBox() const {
        if (not origin)
            return false;

        if (auto el = origin->is<Gc::Ref<Dom::Element>>())
            return (*el)->qualifiedName == Svg::SVG_TAG;

        return false;
    }

    bool isSvgForeignObjectBox() const {
        if (not origin)
            return false;

        if (auto el = origin->is<Gc::Ref<Dom::Element>>())
            return (*el)->qualifiedName == Svg::FOREIGN_OBJECT_TAG;

        return false;
    }

    bool isReplaced() const {
        return content.is<Rc<Scene::Node>>();
    }

    bool isRunningPositionedBox() const {
        return style->position.is<RunningPosition>();
    }

    bool isRootElementPrincipalBox() const {
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

    // https://www.w3.org/TR/CSS22/zindex.html
    bool impliesNewStackingContext() const {
        return style->zIndex != Keywords::AUTO or
               style->clip->has() or
               style->transform->has() or
               style->opacity != 1.0;
    }

    bool isActive() const {
        if (auto it = content.is<Rc<Gfx::Prose>>())
            return (*it)->_runes.len() > 0;

        return children().len() > 0;
    }

    void repr(Io::Emit& e) const {
        e("(box {} {} {}", style->display, style->position, content);
        if (children()) {
            e.indentNewline();
            for (auto& c : children()) {
                c.repr(e);
                e.newline();
            }
            e.deindent();
        }
        e(")");
    }
};

} // namespace Vaev::Layout
