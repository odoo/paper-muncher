module;

#include <karm-image/loader.h>
#include <karm-text/loader.h>
#include <karm-text/prose.h>
#include <vaev-dom/document.h>
#include <vaev-style/computer.h>

export module Vaev.Layout:builder;

import :values;

namespace Vaev::Layout {

static void _buildBlockLevelBox(Style::Computer& c, Gc::Ref<Dom::Element> el, Rc<Style::Computed> style, Box& parent, Display display);
static void _buildTable(Style::Computer& c, Rc<Style::Computed> style, Gc::Ref<Dom::Element> el, Box& parent);
static void _buildImage(Style::Computer& c, Gc::Ref<Dom::Element> el, Rc<Style::Computed> parentStyle, InlineBox& rootInlineBox);
static void _buildChildInternalDisplay(Style::Computer& c, Gc::Ref<Dom::Element> child, Rc<Style::Computed> childStyle, Box& parent);

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

// MARK: Build Inline ----------------------------------------------------------

static Opt<Rc<Karm::Text::Fontface>> _monospaceFontface = NONE;
static Opt<Rc<Karm::Text::Fontface>> _regularFontface = NONE;
static Opt<Rc<Karm::Text::Fontface>> _boldFontface = NONE;

static Rc<Karm::Text::Fontface> _lookupFontface(Text::FontBook& fontBook, Style::Computed& style) {
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

bool isSegmentBreak(Rune rune) {
    return rune == '\n' or rune == '\r' or rune == '\f' or rune == '\v';
}

Text::ProseStyle _buildProseStyle(Style::Computer& c, Rc<Style::Computed> parentStyle) {
    auto fontFace = _lookupFontface(c.fontBook, *parentStyle);

    // FIXME: We should pass this around from the top in order to properly resolve rems
    Resolver resolver{
        .rootFont = Text::Font{fontFace, 16},
        .boxFont = Text::Font{fontFace, 16},
    };
    Text::ProseStyle proseStyle{
        .font = {
            fontFace,
            resolver.resolve(parentStyle->font->size).cast<f64>(),
        },
        .color = parentStyle->color,
        .multiline = true,
    };

    switch (parentStyle->text->align) {
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

void _transformAndAppendRuneToProse(Rc<Text::Prose> prose, Rune rune, TextTransform transform) {
    switch (transform) {
    case TextTransform::UPPERCASE:
        prose->append(toAsciiUpper(rune));
        break;

    case TextTransform::LOWERCASE:
        prose->append(toAsciiLower(rune));
        break;

    case TextTransform::NONE:
    default:
        prose->append(rune);
        break;
    }
}

// https://www.w3.org/TR/css-text-3/#white-space-phase-1
// https://www.w3.org/TR/css-text-3/#white-space-phase-2
void _appendTextToInlineBox(Io::SScan scan, Rc<Style::Computed> parentStyle, Rc<Text::Prose> prose) {
    auto whitespace = parentStyle->text->whiteSpace;
    bool whiteSpacesAreCollapsible =
        whitespace == WhiteSpace::NORMAL or
        whitespace == WhiteSpace::NOWRAP or
        whitespace == WhiteSpace::PRE_LINE;

    // A sequence of collapsible spaces at the beginning of a line is removed.
    if (not prose->_runes.len())
        scan.eat(Re::space());

    while (not scan.ended()) {
        auto rune = scan.next();

        if (not isAsciiSpace(rune)) {
            _transformAndAppendRuneToProse(prose, rune, parentStyle->text->transform);
            continue;
        }

        // https://www.w3.org/TR/css-text-3/#collapse
        if (whiteSpacesAreCollapsible) {
            // Any sequence of collapsible spaces and tabs immediately preceding or following a segment break is removed.
            bool visitedSegmentBreak = false;
            while (true) {
                if (isSegmentBreak(rune))
                    visitedSegmentBreak = true;

                if (scan.ended() or not isAsciiSpace(scan.peek()))
                    break;

                rune = scan.next();
            }

            // Any collapsible space immediately following another collapsible space—​even one outside the boundary
            // of the inline containing that space, provided both spaces are within the same inline formatting
            // context—​is collapsed to have zero advance width. (It is invisible, but retains its soft wrap
            // opportunity, if any.)
            // TODO: not compliant regarding wrap opportunity

            // https://www.w3.org/TR/css-text-3/#valdef-white-space-pre-line
            // Collapsible segment breaks are transformed for rendering according to the segment
            // break transformation rules.
            if (whitespace == WhiteSpace::PRE_LINE and visitedSegmentBreak)
                prose->append('\n');
            else
                prose->append(' ');
        } else if (whitespace == WhiteSpace::PRE) {
            prose->append(rune);
        } else {
            panic("unimplemented whitespace case");
        }
    }
}

void _buildText(Gc::Ref<Dom::Text> node, Rc<Style::Computed> parentStyle, InlineBox& rootInlineBox) {
    Io::SScan scan{node->data()};
    scan.eat(Re::space());
    if (scan.ended())
        return;

    _appendTextToInlineBox(node->data(), parentStyle, rootInlineBox.prose);
}

// https://www.w3.org/TR/css-inline-3/#model
void _flushRootInlineBoxIntoAnonymousBox(Box& parent, Rc<InlineBox>& rootInlineBox) {
    if (not rootInlineBox->active())
        return;

    // The root inline box inherits from its parent block container, but is otherwise unstyleable.
    auto style = makeRc<Style::Computed>(Style::Computed::initial());
    style->inherit(*parent.style);
    style->display = Display{Display::Inside::FLOW, Display::Outside::BLOCK};

    auto newInlineBox = makeRc<InlineBox>(InlineBox::fromInterruptedInlineBox(*rootInlineBox));
    parent.add({style, parent.fontFace, std::move(*rootInlineBox)});
    rootInlineBox = newInlineBox;
}

// Similar abstraction to https://webkit.org/blog/115/webcore-rendering-ii-blocks-and-inlines/
export struct InlineFlowBuilder {
    Box& rootBox;
    Rc<InlineBox>& rootInlineBox;

    InlineFlowBuilder(Box& rootBox, Rc<InlineBox>& rootInlineBox)
        : rootBox(rootBox), rootInlineBox(rootInlineBox) {}

    // https://www.w3.org/TR/css-display-3/#box-generation
    void _buildChildBoxDisplay(Style::Computer& c, Gc::Ref<Dom::Element> child, Rc<Style::Computed> childStyle, Display display) {
        if (display == Display::NONE)
            return;
        else
            _buildChildren(c, child, childStyle);
    }

    void _buildChildDefaultDisplay(Style::Computer& c, Gc::Ref<Dom::Element> child, Rc<Style::Computed> childStyle, Display display);

    // Dispatching children from block-level context based on their node type and display property in case of element
    // https://www.w3.org/TR/css-display-3/#the-display-properties
    void _buildChildren(Style::Computer& c, Gc::Ref<Dom::Element> el, Rc<Style::Computed> style) {
        for (auto child = el->firstChild(); child; child = child->nextSibling()) {
            if (auto el = child->is<Dom::Element>()) {
                auto childStyle = c.computeFor(*style, *el);
                auto display = childStyle->display;

                if (display.type() == Display::Type::BOX) {
                    _buildChildBoxDisplay(c, *el, style, display);
                } else if (display.type() == Display::Type::INTERNAL) {
                    _buildChildInternalDisplay(c, *el, childStyle, rootBox);
                } else {
                    _buildChildDefaultDisplay(c, *el, childStyle, display);
                }
            } else if (auto text = child->is<Dom::Text>()) {
                _buildText(*text, style, *rootInlineBox);
            }
        }
    }

    void buildFromElement(Style::Computer& c, Gc::Ref<Dom::Element> el, Rc<Style::Computed>& style) {
        if (el->tagName == Html::IMG) {
            _buildImage(c, *el, rootBox.style, *rootInlineBox);
        } else if (el->tagName == Html::BR) {
            _flushRootInlineBoxIntoAnonymousBox(rootBox, rootInlineBox);
        } else {
            auto proseStyle = _buildProseStyle(c, style);
            rootInlineBox->startInlineBox(proseStyle);
            _buildChildren(c, el, style);
            rootInlineBox->endInlineBox();
        }
    }
};

export struct BlockFlowBuilder {
    Box& box;
    Rc<InlineBox> rootInlineBox;

    BlockFlowBuilder(Style::Computer& c, Box& box)
        : box(box), rootInlineBox(makeRc<InlineBox>(_buildProseStyle(c, box.style))) {}

    // https://www.w3.org/TR/css-display-3/#box-generation
    void _buildChildBoxDisplay(Style::Computer& c, Gc::Ref<Dom::Node> child, Display display) {
        if (display == Display::NONE)
            return;
        else
            _buildChildren(c, child);
    }

    // Dispatching children from an Inline Flow context based on their outer and inner roles
    // https://www.w3.org/TR/css-display-3/#outer-role
    // https://www.w3.org/TR/css-display-3/#inner-model
    void _buildChildDefaultDisplay(Style::Computer& c, Gc::Ref<Dom::Element> child, Rc<Style::Computed> childStyle, Display display) {
        if (display != Display::Outside::INLINE) {
            _flushRootInlineBoxIntoAnonymousBox(box, rootInlineBox);
            _buildBlockLevelBox(c, child, childStyle, box, display);
            return;
        }

        if (display == Display::Inside::FLOW) {
            InlineFlowBuilder{box, rootInlineBox}.buildFromElement(c, child, childStyle);
        } else if (display == Display::Inside::FLOW_ROOT) {
            auto font = _lookupFontface(c.fontBook, *childStyle);
            Box box = {childStyle, font};
            BlockFlowBuilder{c, box}.buildFromElement(c, child);

            box.attrs = _parseDomAttr(child);

            rootInlineBox->add(std::move(box));
        } else {
            // FIXME: fallback to FLOW since not implemented
            InlineFlowBuilder{box, rootInlineBox}.buildFromElement(c, child, childStyle);
        }
    }

    // Dispatching children from block-level context based on their node type and display property in case of element
    // https://www.w3.org/TR/css-display-3/#the-display-properties
    void _buildChildren(Style::Computer& c, Gc::Ref<Dom::Node> parent) {
        for (auto child = parent->firstChild(); child; child = child->nextSibling()) {
            if (auto el = child->is<Dom::Element>()) {
                auto childStyle = c.computeFor(*box.style, *el);
                auto display = childStyle->display;

                if (display.type() == Display::Type::BOX) {
                    _buildChildBoxDisplay(c, *el, display);
                } else if (display.type() == Display::Type::INTERNAL) {
                    _buildChildInternalDisplay(c, *el, childStyle, box);
                } else {
                    _buildChildDefaultDisplay(c, *el, childStyle, display);
                }
            } else if (auto text = child->is<Dom::Text>()) {
                _buildText(*text, box.style, *rootInlineBox);
            }
        }
    }

    void _finalizeParentBoxAndFlushInline(Box& parent, Rc<InlineBox>& rootInlineBox) {
        if (not rootInlineBox->active())
            return;

        if (parent.children()) {
            _flushRootInlineBoxIntoAnonymousBox(parent, rootInlineBox);
            return;
        }

        auto newRootInlineBox = makeRc<InlineBox>(InlineBox::fromInterruptedInlineBox(*rootInlineBox));
        parent.content = std::move(*rootInlineBox);
        rootInlineBox = newRootInlineBox;
    }

    void buildFromElement(Style::Computer& c, Gc::Ref<Dom::Element> el) {
        if (el->tagName == Html::IMG) {
            _buildImage(c, *el, box.style, *rootInlineBox);
        } else if (el->tagName == Html::SVG) {
            // TODO: _buildSVG
        } else if (el->tagName == Html::BR) {
            // do nothing
        } else {
            _buildChildren(c, el);
        }
        _finalizeParentBoxAndFlushInline(box, rootInlineBox);
    }

    void build(Style::Computer& c, Gc::Ref<Dom::Node> node) {
        if (auto el = node->is<Dom::Element>()) {
            buildFromElement(c, *el);
            return;
        }

        _buildChildren(c, node);
        _finalizeParentBoxAndFlushInline(box, rootInlineBox);
    }

    static void fromElement(Style::Computer& c, Box& parent, Rc<Style::Computed> style, Gc::Ref<Dom::Element> el) {
        auto font = _lookupFontface(c.fontBook, *style);
        Box box = {style, font};
        BlockFlowBuilder{c, box}.buildFromElement(c, el);

        box.attrs = _parseDomAttr(el);
        parent.add(std::move(box));
    }
};

// https://www.w3.org/TR/css-display-3/#layout-specific-display
void _buildChildInternalDisplay(Style::Computer& c, Gc::Ref<Dom::Element> child, Rc<Style::Computed> childStyle, Box& parent) {
    // FIXME: We should create wrapping boxes related to table or ruby, following the FC specification. However, for now,
    // we just wrap it in a single box.
    BlockFlowBuilder::fromElement(c, parent, childStyle, child);
}

// Dispatching children from an Inline Flow context based on their outer and inner roles
// https://www.w3.org/TR/css-display-3/#outer-role
// https://www.w3.org/TR/css-display-3/#inner-model
void InlineFlowBuilder::_buildChildDefaultDisplay(Style::Computer& c, Gc::Ref<Dom::Element> child, Rc<Style::Computed> childStyle, Display display) {
    if (display != Display::Outside::INLINE) {
        _flushRootInlineBoxIntoAnonymousBox(rootBox, rootInlineBox);
        _buildBlockLevelBox(c, child, childStyle, rootBox, display);
        return;
    }

    if (display == Display::Inside::FLOW) {
        buildFromElement(c, child, childStyle);
    } else if (display == Display::Inside::FLOW_ROOT) {
        auto font = _lookupFontface(c.fontBook, *childStyle);
        Box box = {childStyle, font};
        BlockFlowBuilder{c, box}.buildFromElement(c, child);

        box.attrs = _parseDomAttr(child);

        rootInlineBox->add(std::move(box));
    } else {
        // FIXME: fallback to FLOW since not implemented
        buildFromElement(c, child, childStyle);
    }
}

// MARK: Build Replace ---------------------------------------------------------

static void _buildImage(Style::Computer& c, Gc::Ref<Dom::Element> el, Rc<Style::Computed> parentStyle, InlineBox& rootInlineBox) {
    auto style = c.computeFor(*parentStyle, el);
    auto font = _lookupFontface(c.fontBook, *style);

    auto src = el->getAttribute(Html::SRC_ATTR).unwrapOr(""s);
    auto url = Mime::Url::resolveReference(el->baseURI(), Mime::parseUrlOrPath(src))
                   .unwrapOr("bundle://vaev-driver/missing.qoi"_url);

    auto img = Karm::Image::load(url).unwrapOrElse([] {
        return Karm::Image::loadOrFallback("bundle://vaev-driver/missing.qoi"_url).unwrap();
    });

    rootInlineBox.add({style, font});
}

// MARK: Build Table -----------------------------------------------------------

static void _buildTableChildren(Style::Computer& c, Gc::Ref<Dom::Node> node, Box& tableWrapperBox, Rc<Style::Computed> tableBoxStyle) {
    Box tableBox{
        tableBoxStyle,
        tableWrapperBox.fontFace,
    };

    tableBox.style->display = Display::Internal::TABLE_BOX;

    bool captionsOnTop = tableBox.style->table->captionSide == CaptionSide::TOP;

    if (captionsOnTop) {
        for (auto child = node->firstChild(); child; child = child->nextSibling()) {
            if (auto el = child->is<Dom::Element>()) {
                if (el->tagName == Html::CAPTION) {
                    BlockFlowBuilder::fromElement(
                        c, tableWrapperBox,
                        c.computeFor(*tableWrapperBox.style, *el), *el
                    );
                }
            }
        }
    }

    for (auto child = node->firstChild(); child; child = child->nextSibling()) {
        if (auto el = child->is<Dom::Element>()) {
            if (el->tagName != Html::CAPTION) {
                BlockFlowBuilder::fromElement(
                    c, tableBox,
                    c.computeFor(*tableBox.style, *el), *el
                );
            }
        }
    }
    tableWrapperBox.add(std::move(tableBox));

    if (not captionsOnTop) {
        for (auto child = node->firstChild(); child; child = child->nextSibling()) {
            if (auto el = child->is<Dom::Element>()) {
                if (el->tagName == Html::CAPTION) {
                    BlockFlowBuilder::fromElement(
                        c, tableWrapperBox,
                        c.computeFor(*tableWrapperBox.style, *el), *el
                    );
                }
            }
        }
    }
}

// https://www.w3.org/TR/css-display-3/#outer-role
static void _buildBlockLevelBox(Style::Computer& c, Gc::Ref<Dom::Element> el, Rc<Style::Computed> style, Box& parent, Display display) {
    if (display == Display::Inside::TABLE) {
        _buildTable(c, style, el, parent);
    } else if (display == Display::Inside::FLOW or display == Display::Inside::FLEX) {
        BlockFlowBuilder::fromElement(c, parent, style, el);
    } else {
        // FIXME: fallback to FLOW since not implemented
        BlockFlowBuilder::fromElement(c, parent, style, el);
    }
}

static void _buildTable(Style::Computer& c, Rc<Style::Computed> style, Gc::Ref<Dom::Element> el, Box& parent) {
    auto font = _lookupFontface(c.fontBook, *style);

    auto wrapperStyle = makeRc<Style::Computed>(Style::Computed::initial());
    wrapperStyle->display = style->display;
    wrapperStyle->margin = style->margin;

    Box wrapper = {wrapperStyle, font};
    _buildTableChildren(c, el, wrapper, style);
    wrapper.attrs = _parseDomAttr(el);

    parent.add(std::move(wrapper));
}

// MARK: Build -----------------------------------------------------------------

export Box build(Style::Computer& c, Gc::Ref<Dom::Document> doc) {
    if (auto el = doc->documentElement()) {
        auto style = c.computeFor(Style::Computed::initial(), *el);
        auto font = _lookupFontface(c.fontBook, *style);

        Box root = {style, font};
        BlockFlowBuilder{c, root}.build(c, doc);
        return root;
    }
    // NOTE: Fallback in case of an empty document
    auto style = makeRc<Style::Computed>(Style::Computed::initial());
    return {
        style,
        _lookupFontface(c.fontBook, *style),
    };
}

export Box buildForPseudoElement(Text::FontBook& fontBook, Rc<Style::Computed> style) {
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
        return {style, fontFace, InlineBox{prose}};
    }

    return {style, fontFace};
}

} // namespace Vaev::Layout
