export module Vaev.Engine:layout.builder2;

import Karm.Core;
import :dom.document;
import :dom.element;

using namespace Karm;

namespace Vaev::Layout {

struct Box2 {
    Rc<Style::SpecifiedValues> style();

    static Rc<Box2> createRoot();

    static Rc<Box2> createInline();
};

// https://www.w3.org/TR/css-display-3/#the-display-properties
struct Builder {
    Vec<Rc<Box2>> _ancestors;

    void pushBox(Rc<Box2> box) {
        _ancestors.pushBack(box);
    }

    Rc<Box2> popBox() {
        _ancestors.popBack();
    }

    Rc<Box2> peekBox() {
        return last(_ancestors);
    }

    // https://www.w3.org/TR/css-display-3/#anonymous
    Rc<Box2> pushAnonymousBox() {
        auto style = makeRc<Style::SpecifiedValues>(Style::SpecifiedValues::initial());
        style->inherit(*peekBox()->style());
        style->display = Display{Display::Inside::FLOW, Display::Outside::BLOCK};
    }

    // MARK: Pseudo Element ----------------------------------------------------
    // https://www.w3.org/TR/css-pseudo-4/#generated-content

    // MARK: List Item ---------------------------------------------------------
    // https://www.w3.org/TR/css-display-3/#list-items

    // MARK: Void Element ------------------------------------------------------
    // https://html.spec.whatwg.org/multipage/syntax.html#void-elements

    void buildVoidElement(Gc::Ref<Dom::Element> el) {
        if (el->qualifiedName == Html::BR_TAG) {
            // do nothing
        }
    }

    // MARK: Table Flow --------------------------------------------------------
    // https://www.w3.org/TR/css-tables-3/#fixup-algorithm

    // MARK: Inline Flow -------------------------------------------------------

    void pushAnonymouseInlineBox();

    void interuptInlineFlow();

    void resumeInlineFlow();

    // https://drafts.csswg.org/css-text-4/#white-space-property
    void appendText(Gc::Ref<Dom::Text> tx) {
        if (not) {
            pushAnonymouseInlineBox();
            appendText(tx);
            popBox();
            return;
        }
    }

    // MARK: Normal Flow -------------------------------------------------------

    void pushBlockLevelBox();

    void pushInlineLevelBox();

    void buildFlow(Gc::Ref<Dom::Element> el) {
        if (el->isVoidElement())
            buildVoidElement(el);
        else
    }

    void buildFlowRoot(Gc::Ref<Dom::Element> el) {
        pushBox(Box2::createRoot());
        pushBox(Box2::createInline());
        buildFlow(el);
        popBox();
        popBox();
    }

    // MARK: Build Element -----------------------------------------------------

    void buildChildren(Gc::Ref<Dom::Element> el) {
        for (auto child : el->iterChildren())
            buildNode(child);
    }

    // https://www.w3.org/TR/css-display-3/#inner-model
    void buildInnerDisplay(Gc::Ref<Dom::Element> el, Display display) {
        buildChildren(el);
    }

    // https://www.w3.org/TR/css-display-3/#outer-role
    void buildOuterDisplay(Gc::Ref<Dom::Element> el, Display display) {
        if (display.outside() == Display::BLOCK) {
            pushBlockLevelBox();
        } else if (display.outside() == Display::INLINE or
                   display.outside() == Display::RUN_IN) {
            pushInlineLevelBox();
        } else {
            unreachable();
        }
        buildInnerDisplay(el, display);
        popBox();
    }

    void buildElement(Gc::Ref<Dom::Element> el, Display display) {
        if (display == Display::NONE)
            return;
        else if (display == Display::CONTENTS)
            buildChildren(el);
        else
            buildOuterDisplay(el, display);
    }

    Rc<Box2> buildElement(Gc::Ref<Dom::Element> el) {
        return buildElement(el, el->specifiedValues()->display);
    }

    void buildNode(Gc::Ref<Dom::Node> node) {
        if (auto it = node->is<Dom::Element>())
            buildElement(it.upgrade());
        else if (auto it = node->is<Dom::Text>())
            appendText(it.upgrade());
    }

    // MARK: Entry Point -------------------------------------------------------

    // https://www.w3.org/TR/css-display-3/#root
    Rc<Box2> build(Gc::Ref<Dom::Document> doc) {
        if (auto el = doc->documentElement()) {
            // The root element’s display type is always blockified, and its
            // principal box always establishes an independent formatting context.
            auto rootElementDisplay = el->specifiedValues()->display.blockify();
            return buildElement(el.upgrade(), rootElementDisplay);
        }
        pushBox(Box2::createRoot());
        return popBox();
    }
};

} // namespace Vaev::Layout