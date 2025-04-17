module;

#include <karm-image/loader.h>
#include <karm-text/loader.h>
#include <karm-text/prose.h>
#include <vaev-dom/document.h>
#include <vaev-style/computer.h>

export module Vaev.Layout:builder;

import :values;

namespace Vaev::Layout {

static constexpr bool DEBUG_BUILDER = false;

// MARK: Attributes ------------------------------------------------------------

static Opt<Str> _parseStrAttr(Gc::Ref<Dom::Element> el, AttrName name) {
    return el->getAttribute(name);
}

static Opt<usize> _parseUsizeAttr(Gc::Ref<Dom::Element> el, AttrName name) {
    auto str = _parseStrAttr(el, name);
    if (not str)
        return NONE;
    return Io::atoi(str.unwrap());
}

static Attrs _parseDomAttr(Gc::Ref<Dom::Element> el) {
    Attrs attrs;

    // https://html.spec.whatwg.org/multipage/tables.html#the-col-element

    // The element may have a span content attribute specified, whose value must
    // be a valid non-negative integer greater than zero and less than or equal to 1000.

    attrs.span = _parseUsizeAttr(el, Html::SPAN_ATTR).unwrapOr(1);
    if (attrs.span == 0 or attrs.span > 1000)
        attrs.span = 1;

    // https://html.spec.whatwg.org/multipage/tables.html#attributes-common-to-td-and-th-elements

    // The td and th elements may have a colspan content attribute specified,
    // whose value must be a valid non-negative integer greater than zero and less than or equal to 1000.
    attrs.colSpan = _parseUsizeAttr(el, Html::COLSPAN_ATTR).unwrapOr(1);
    if (attrs.colSpan == 0 or attrs.colSpan > 1000)
        attrs.colSpan = 1;

    // The td and th elements may also have a rowspan content attribute specified,
    // whose value must be a valid non-negative integer less than or equal to 65534.
    attrs.rowSpan = _parseUsizeAttr(el, Html::ROWSPAN_ATTR).unwrapOr(1);
    if (attrs.rowSpan > 65534)
        attrs.rowSpan = 65534;

    return attrs;
}

// MARK: Build Prose ----------------------------------------------------------

static Opt<Rc<Karm::Text::Fontface>> _monospaceFontface = NONE;
static Opt<Rc<Karm::Text::Fontface>> _regularFontface = NONE;
static Opt<Rc<Karm::Text::Fontface>> _boldFontface = NONE;

static Rc<Karm::Text::Fontface> _lookupFontface(Text::FontBook& fontBook, Style::ComputedStyle& style) {
    Text::FontQuery fq{
        .weight = style.font->weight,
        .style = style.font->style.val,
    };

    for (auto family : style.font->families) {
        fq.family = family;
        if (auto font = fontBook.queryClosest(fq))
            return font.unwrap();
    }

    if (
        auto font = fontBook.queryClosest({
            .family = String{"Inter"s},
        })
    )
        return font.unwrap();

    return Text::Fontface::fallback();
}

auto RE_SEGMENT_BREAK = Re::single('\n', '\r', '\f', '\v');

static Text::ProseStyle _proseStyleFomStyle(Style::ComputedStyle& style, Rc<Text::Fontface> fontFace) {
    // FIXME: We should pass this around from the top in order to properly resolve rems
    Resolver resolver{
        .rootFont = Text::Font{fontFace, 16},
        .boxFont = Text::Font{fontFace, 16},
    };
    Text::ProseStyle proseStyle{
        .font = {
            fontFace,
            resolver.resolve(style.font->size).cast<f64>(),
        },
        .color = style.color,
        .multiline = true,
    };

    switch (style.text->align) {
    case TextAlign::START:
    case TextAlign::LEFT:
        proseStyle.align = Text::TextAlign::LEFT;
        break;

    case TextAlign::END:
    case TextAlign::RIGHT:
        proseStyle.align = Text::TextAlign::RIGHT;
        break;

    case TextAlign::CENTER:
        proseStyle.align = Text::TextAlign::CENTER;
        break;

    default:
        // FIXME: Implement the rest
        break;
    }

    return proseStyle;
}

static Text::ProseStyle _proseStyleFomStyle(Style::Computer& c, Style::ComputedStyle& style) {
    auto fontFace = _lookupFontface(c.fontBook, style);
    return _proseStyleFomStyle(style, fontFace);
}

void _appendTextToInlineBox(Io::SScan scan, Rc<Style::ComputedStyle> parentStyle, InlineBox& rootInlineBox) {
    auto prose = rootInlineBox.prose;
    auto whitespace = parentStyle->text->whiteSpace;
    while (not scan.ended()) {
        switch (parentStyle->text->transform) {
        case TextTransform::UPPERCASE:
            prose->append(toAsciiUpper(scan.next()));
            break;

        case TextTransform::LOWERCASE:
            prose->append(toAsciiLower(scan.next()));
            break;

        case TextTransform::NONE:
            prose->append(scan.next());
            break;

        default:
            break;
        }

        if (whitespace == WhiteSpace::PRE) {
            auto tok = scan.token(Re::space());
            if (tok)
                prose->append(tok);
        } else if (whitespace == WhiteSpace::PRE_LINE) {
            bool hasBlank = false;
            if (scan.eat(Re::blank())) {
                hasBlank = true;
            }

            if (scan.eat(RE_SEGMENT_BREAK)) {
                prose->append('\n');
                scan.eat(Re::blank());
                hasBlank = false;
            }

            if (hasBlank)
                prose->append(' ');
        } else {
            // NORMAL
            if (scan.eat(Re::space()))
                prose->append(' ');
        }
    }
}

void _buildText(Gc::Ref<Dom::Text> node, Rc<Style::ComputedStyle> parentStyle, InlineBox& rootInlineBox) {
    Io::SScan scan{node->data()};
    scan.eat(Re::space());
    if (scan.ended())
        return;

    _appendTextToInlineBox(scan, parentStyle, rootInlineBox);
}

// MARK: Build Block -----------------------------------------------------------

struct BuilderContext {
    enum struct From {
        BLOCK,
        INLINE,
    };

    Style::Computer& computer;

    From const from;
    Rc<Style::ComputedStyle> const parentStyle;

    Box& _parent;
    MutCursor<InlineBox> _rootInlineBox;

    // https://www.w3.org/TR/css-inline-3/#model
    void flushRootInlineBoxIntoAnonymousBox() {
        if (not assertForRootInlineBox())
            return;

        if (not rootInlineBox().active())
            return;

        // The root inline box inherits from its parent block container, but is otherwise unstyleable.
        auto style = makeRc<Style::ComputedStyle>(Style::ComputedStyle::initial());
        style->inherit(*parentStyle);
        style->display = Display{Display::Inside::FLOW, Display::Outside::BLOCK};

        auto newInlineBox = InlineBox::fromInterruptedInlineBox(*_rootInlineBox);
        _parent.add({style, _parent.fontFace, std::move(*_rootInlineBox), nullptr});
        *_rootInlineBox = std::move(newInlineBox);
    }

    void finalizeParentBoxAndFlushInline() {
        if (not assertForRootInlineBox())
            return;

        if (not rootInlineBox().active())
            return;

        if (_parent.children()) {
            flushRootInlineBoxIntoAnonymousBox();
            return;
        }

        auto newRootInlineBox = InlineBox::fromInterruptedInlineBox(*_rootInlineBox);
        _parent.content = std::move(*_rootInlineBox);
        *_rootInlineBox = std::move(newRootInlineBox);
    }

    Rc<Style::ComputedStyle> style() {
        return parentStyle;
    }

    void addToParentBox(Box&& box) {
        _parent.add(std::move(box));
    }

    // FIXME: currently its a recoverable error since table is not correctly implemented, but should be a panic
    // once table build is properly done
    bool assertForRootInlineBox() {
        if (_rootInlineBox == nullptr) {
            logError("expected root inline box");
            return false;
        }
        return true;
    }

    InlineBox& rootInlineBox() {
        return *_rootInlineBox;
    }

    void addToInlineRoot(Box&& box) {
        if (not assertForRootInlineBox())
            return;
        rootInlineBox().add(std::move(box));
    }

    // FIXME: find me a better name
    void startInlineBox(Text::ProseStyle proseStyle) {
        if (not assertForRootInlineBox())
            return;
        rootInlineBox().startInlineBox(proseStyle);
    }

    void endInlineBox() {
        if (not assertForRootInlineBox())
            return;
        rootInlineBox().endInlineBox();
    }

    BuilderContext toBlockContext(Box& parent, InlineBox& rootInlineBox) {
        return {
            computer,
            From::BLOCK,
            parent.style,
            parent,
            &rootInlineBox,
        };
    }

    BuilderContext toBlockContextWithoutRootInline(Box& parent) {
        return {
            computer,
            From::BLOCK,
            parent.style,
            parent,
            nullptr,
        };
    }

    BuilderContext toInlineContext(Rc<Style::ComputedStyle> parentStyle) {
        return {
            computer,
            From::INLINE,
            parentStyle,
            _parent,
            _rootInlineBox,
        };
    }
};

static void _buildNode(BuilderContext bc, Gc::Ref<Dom::Node> node);

// MARK: Build void/leaves ---------------------------------------------------------

static void _buildText(BuilderContext bc, Gc::Ref<Dom::Text> node, Rc<Style::ComputedStyle> parentStyle) {
    if (not bc.assertForRootInlineBox())
        return;

    _buildText(node, parentStyle, bc.rootInlineBox());
}

static void _buildImage(Style::Computer& c, Gc::Ref<Dom::Element> el, Rc<Style::ComputedStyle> parentStyle, InlineBox& rootInlineBox) {
    auto style = c.computeFor(*parentStyle, el);
    auto font = _lookupFontface(c.fontBook, *style);

    auto src = el->getAttribute(Html::SRC_ATTR).unwrapOr(""s);
    auto url = Mime::Url::resolveReference(el->baseURI(), Mime::parseUrlOrPath(src))
                   .unwrapOr("bundle://vaev-driver/missing.qoi"_url);

    auto img = Karm::Image::load(url).unwrapOrElse([] {
        return Karm::Image::loadOrFallback("bundle://vaev-driver/missing.qoi"_url).unwrap();
    });

    rootInlineBox.add({style, font, el});
}

static void _buildInputProse(Style::Computer& c, Gc::Ref<Dom::Element> el, Box& parent) {
    auto style = c.computeFor(*parent.style, el);
    auto font = _lookupFontface(c.fontBook, *style);
    Resolver resolver{
        .rootFont = Text::Font{font, 16},
        .boxFont = Text::Font{font, 16},
    };
    Text::ProseStyle proseStyle = _proseStyleFomStyle(*style, font);

    auto value = ""s;
    if (el->hasAttribute(Html::VALUE_ATTR))
        value = el->getAttribute(Html::VALUE_ATTR).unwrap();
    else if (el->hasAttribute(Html::PLACEHOLDER_ATTR))
        value = el->getAttribute(Html::PLACEHOLDER_ATTR).unwrap();

    auto prose = makeRc<Text::Prose>(proseStyle, value);

    // FIXME: we should guarantee that input has no children (not added before nor to add after)
    parent.content = InlineBox{prose};
}

always_inline static bool isVoidElement(Gc::Ref<Dom::Element> el) {
    return contains(Html::VOID_TAGS, el->tagName);
}

static void _buildVoidElement(BuilderContext bc, Gc::Ref<Dom::Element> el) {
    if (el->tagName == Html::INPUT) {
        _buildInputProse(bc.computer, el, bc._parent);
    } else if (el->tagName == Html::IMG) {
        _buildImage(bc.computer, *el, bc.parentStyle, bc.rootInlineBox());
    }
}

// MARK: Build flow -------------------------------------------------------------------------------

static void _buildChildren(BuilderContext bc, Gc::Ref<Dom::Node> parent);

static void createAndBuildInlineFlowfromElement(BuilderContext bc, Rc<Style::ComputedStyle> style, Gc::Ref<Dom::Element> el) {
    if (el->tagName == Html::BR) {
        bc.flushRootInlineBoxIntoAnonymousBox();
        return;
    }

    if (isVoidElement(el)) {
        _buildVoidElement(bc, el);
        return;
    }

    auto proseStyle = _proseStyleFomStyle(bc.computer, *style);

    bc.startInlineBox(proseStyle);
    _buildChildren(bc.toInlineContext(style), el);
    bc.endInlineBox();
}

static void buildBlockFlowFromElement(BuilderContext bc, Gc::Ref<Dom::Element> el) {
    if (el->tagName == Html::BR) {
        // do nothing
    } else if (isVoidElement(el)) {
        _buildVoidElement(bc, el);
    } else {
        _buildChildren(bc, el);
    }
    bc.finalizeParentBoxAndFlushInline();
}

static Box createAndBuildBlockFlowfromElement(BuilderContext bc, Rc<Style::ComputedStyle> style, Gc::Ref<Dom::Element> el) {
    auto font = _lookupFontface(bc.computer.fontBook, *style);
    Box box = {style, font, el};
    InlineBox rootInlineBox{_proseStyleFomStyle(bc.computer, *style)};

    buildBlockFlowFromElement(bc.toBlockContext(box, rootInlineBox), el);

    box.attrs = _parseDomAttr(el);
    return box;
}

// MARK: Build Table -----------------------------------------------------------

static void _innerDisplayDispatchCreationOfBlockLevelBox(BuilderContext bc, Gc::Ref<Dom::Element> el, Rc<Style::ComputedStyle> style, Display display);

static void _buildTableChildren(BuilderContext bc, Gc::Ref<Dom::Node> node, Rc<Style::ComputedStyle> tableBoxStyle) {
    bool captionsOnTop = tableBoxStyle->table->captionSide == CaptionSide::TOP;

    Box tableBox{
        tableBoxStyle,
        bc._parent.fontFace,
        node->is<Dom::Element>()
    };

    tableBox.style->display = Display::Internal::TABLE_BOX;

    if (captionsOnTop) {
        for (auto child = node->firstChild(); child; child = child->nextSibling()) {
            if (auto el = child->is<Dom::Element>()) {
                if (el->tagName == Html::CAPTION) {
                    _buildNode(bc, *el);
                }
            }
        }
    }

    for (auto child = node->firstChild(); child; child = child->nextSibling()) {
        if (auto el = child->is<Dom::Element>()) {
            if (el->tagName != Html::CAPTION) {
                // FIXME: table internal elements should not have the same code-path as blocks
                auto childStyle = bc.computer.computeFor(*tableBoxStyle, *el);
                _innerDisplayDispatchCreationOfBlockLevelBox(
                    bc.toBlockContextWithoutRootInline(tableBox), *el,
                    childStyle, childStyle->display
                );
            }
        }
    }

    bc.addToParentBox(std::move(tableBox));

    if (not captionsOnTop) {
        for (auto child = node->firstChild(); child; child = child->nextSibling()) {
            if (auto el = child->is<Dom::Element>()) {
                if (el->tagName == Html::CAPTION) {
                    _buildNode(bc, *el);
                }
            }
        }
    }
}

static Box _createTableWrapperAndBuildTable(BuilderContext bc, Rc<Style::ComputedStyle> tableStyle, Gc::Ref<Dom::Element> tableBoxEl) {
    auto font = _lookupFontface(bc.computer.fontBook, *tableStyle);

    auto wrapperStyle = makeRc<Style::ComputedStyle>(Style::ComputedStyle::initial());
    wrapperStyle->display = tableStyle->display;
    wrapperStyle->margin = tableStyle->margin;

    Box wrapper = {wrapperStyle, font, tableBoxEl};
    InlineBox rootInlineBox{_proseStyleFomStyle(bc.computer, *wrapperStyle)};

    // SPEC: The table wrapper box establishes a block formatting context.
    _buildTableChildren(bc.toBlockContextWithoutRootInline(wrapper), tableBoxEl, tableStyle);
    wrapper.attrs = _parseDomAttr(tableBoxEl);

    return wrapper;
}

// MARK: Dispatch based on outside role -------------------------------------------------------------------------------

// https://www.w3.org/TR/css-display-3/#outer-role
static void _innerDisplayDispatchCreationOfBlockLevelBox(BuilderContext bc, Gc::Ref<Dom::Element> el, Rc<Style::ComputedStyle> style, Display display) {
    if (display == Display::Inside::TABLE) {
        auto wrapper = _createTableWrapperAndBuildTable(bc, style, el);
        bc.addToParentBox(std::move(wrapper));
    } else if (display == Display::Inside::FLOW) {
        auto blockBox = createAndBuildBlockFlowfromElement(bc, style, el);
        bc.addToParentBox(std::move(blockBox));
    } else {
        // FIXME: fallback to FLOW since not implemented
        auto blockBox = createAndBuildBlockFlowfromElement(bc, style, el);
        bc.addToParentBox(std::move(blockBox));
    }
}

// https://www.w3.org/TR/css-display-3/#outer-role
static void _innerDisplayDispatchCreationOfInlineLevelBox(BuilderContext bc, Gc::Ref<Dom::Element> el, Rc<Style::ComputedStyle> style, Display display) {
    if (display == Display::Inside::TABLE) {
        auto wrapper = _createTableWrapperAndBuildTable(bc, style, el);
        bc.addToInlineRoot(std::move(wrapper));
    } else {
        // FLOW-ROOT and fallback
        auto blockBox = createAndBuildBlockFlowfromElement(bc, style, el);
        bc.addToInlineRoot(std::move(blockBox));
    }
}

// MARK: Dispatching from Node to builder based on outside role ------------------------------------------------------

static void _buildChildren(BuilderContext bc, Gc::Ref<Dom::Node> parent) {
    for (auto child = parent->firstChild(); child; child = child->nextSibling()) {
        _buildNode(bc, *child);
    }
}

static void _buildChildBoxDisplay(BuilderContext bc, Gc::Ref<Dom::Node> node, Display display) {
    if (display == Display::NONE)
        return;

    // Display::CONTENTS
    _buildChildren(bc, node);
}

// https://www.w3.org/TR/css-display-3/#layout-specific-display
static void _buildChildInternalDisplay(BuilderContext bc, Gc::Ref<Dom::Element> child, Rc<Style::ComputedStyle> childStyle) {
    // FIXME: We should create wrapping boxes related to table or ruby, following the FC specification. However, for now,
    // we just wrap it in a single box.
    // FIXME: since table internal elements' code-path is the same than normal blocks, this method is also used in a
    // correctly formed table
    _innerDisplayDispatchCreationOfBlockLevelBox(bc, child, childStyle, childStyle->display);
}

static void _buildChildDefaultDisplay(BuilderContext bc, Gc::Ref<Dom::Element> child, Rc<Style::ComputedStyle> childStyle, Display display) {
    if (display == Display::Outside::BLOCK) {
        bc.flushRootInlineBoxIntoAnonymousBox();
        _innerDisplayDispatchCreationOfBlockLevelBox(bc, child, childStyle, display);
    } else {
        if (display == Display::Inside::FLOW) {
            createAndBuildInlineFlowfromElement(bc, childStyle, child);
            return;
        }
        _innerDisplayDispatchCreationOfInlineLevelBox(bc, child, childStyle, display);
    }
}

// https://www.w3.org/TR/css-display-3/#box-generation
static void _buildNode(BuilderContext bc, Gc::Ref<Dom::Node> node) {
    logDebugIf(DEBUG_BUILDER, "building node {} at context {}", node, bc.from);
    if (auto el = node->is<Dom::Element>()) {
        auto childStyle = bc.computer.computeFor(*bc.parentStyle, *el);
        auto display = childStyle->display;

        if (display.type() == Display::Type::BOX) {
            _buildChildBoxDisplay(bc, *el, display);
        } else if (display.type() == Display::Type::INTERNAL) {
            _buildChildInternalDisplay(bc, *el, childStyle);
        } else {
            _buildChildDefaultDisplay(bc, *el, childStyle, display);
        }
    } else if (auto text = node->is<Dom::Text>()) {
        _buildText(bc, *text, bc.parentStyle);
    }
}

// MARK: Entry points -----------------------------------------------------------------

export Box build(Style::Computer& c, Gc::Ref<Dom::Document> doc) {
    if (auto el = doc->documentElement()) {
        auto style = c.computeFor(Style::ComputedStyle::initial(), *el);
        auto font = _lookupFontface(c.fontBook, *style);

        Box root = {style, font, el};
        InlineBox rootInlineBox{_proseStyleFomStyle(c, *style)};

        BuilderContext bc{
            c,
            BuilderContext::From::BLOCK,
            style,
            root,
            &rootInlineBox,
        };

        buildBlockFlowFromElement(bc, *el);
        return root;
    }
    // NOTE: Fallback in case of an empty document
    auto style = makeRc<Style::ComputedStyle>(Style::ComputedStyle::initial());
    return {
        style,
        _lookupFontface(c.fontBook, *style),
        nullptr
    };
}

export Box buildForPseudoElement(Text::FontBook& fontBook, Rc<Style::ComputedStyle> style) {
    auto fontFace = _lookupFontface(fontBook, *style);

    // FIXME: We should pass this around from the top in order to properly resolve rems
    Resolver resolver{
        .rootFont = Text::Font{fontFace, 16},
        .boxFont = Text::Font{fontFace, 16},
    };
    Text::ProseStyle proseStyle{
        .font = {
            fontFace,
            resolver.resolve(style->font->size).cast<f64>(),
        },
        .multiline = true,
    };

    auto prose = makeRc<Text::Prose>(proseStyle);
    if (style->content) {
        prose->append(style->content.str());
        return {style, fontFace, InlineBox{prose}, nullptr};
    }

    return {style, fontFace, nullptr};
}

} // namespace Vaev::Layout
