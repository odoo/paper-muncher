module;

#include <karm-image/loader.h>
#include <karm-text/loader.h>
#include <karm-text/prose.h>
#include <vaev-dom/document.h>
#include <vaev-style/computer.h>

export module Vaev.Layout:builder;

import :values;

namespace Vaev::Layout {

static void _createAndBuildBlockLevelBox(Style::Computer& c, Gc::Ref<Dom::Element> el, Rc<Style::ComputedStyle> style, Box& parent, Display display);
static void _createAndBuildInlineLevelBox(Style::Computer& c, Gc::Ref<Dom::Element> el, Rc<Style::ComputedStyle> style, InlineBox& rootInlineBox, Display display);

static Box _createTableWrapperAndBuildTable(Style::Computer& c, Rc<Style::ComputedStyle> tableStyle, Gc::Ref<Dom::Element> tableBoxEl);
static Box _createFlexContainerAndBuildFlex(Style::Computer& c, Gc::Ref<Dom::Element> flexContainerEl, Rc<Style::ComputedStyle> flexContainerStyle);

static void _buildImage(Style::Computer& c, Gc::Ref<Dom::Element> el, Rc<Style::ComputedStyle> parentStyle, InlineBox& rootInlineBox);

static void _buildChildInternalDisplay(Style::Computer& c, Gc::Ref<Dom::Element> child, Rc<Style::ComputedStyle> childStyle, Box& parent);

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

bool isSegmentBreak(Rune rune) {
    return rune == '\n' or rune == '\r' or rune == '\f' or rune == '\v';
}

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
void _appendTextToInlineBox(Io::SScan scan, Rc<Style::ComputedStyle> parentStyle, InlineBox& rootInlineBox) {
    auto whitespace = parentStyle->text->whiteSpace;
    bool whiteSpacesAreCollapsible =
        whitespace == WhiteSpace::NORMAL or
        whitespace == WhiteSpace::NOWRAP or
        whitespace == WhiteSpace::PRE_LINE;

    // A sequence of collapsible spaces at the beginning of a line is removed.
    if (not rootInlineBox.prose->_runes.len())
        scan.eat(Re::space());

    while (not scan.ended()) {
        auto rune = scan.next();

        if (not isAsciiSpace(rune)) {
            _transformAndAppendRuneToProse(rootInlineBox.prose, rune, parentStyle->text->transform);
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
                rootInlineBox.prose->append('\n');
            else
                rootInlineBox.prose->append(' ');
        } else if (whitespace == WhiteSpace::PRE) {
            rootInlineBox.prose->append(rune);
        } else {
            panic("unimplemented whitespace case");
        }
    }
}

bool _buildText(Gc::Ref<Dom::Text> node, Rc<Style::ComputedStyle> parentStyle, InlineBox& rootInlineBox) {
    Io::SScan scan{node->data()};
    scan.eat(Re::space());
    if (scan.ended())
        return false;

    _appendTextToInlineBox(node->data(), parentStyle, rootInlineBox);
    return true;
}

// MARK: Build Input -----------------------------------------------------------

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

// MARK: Build Block -----------------------------------------------------------


export struct BlockFlowBuilder {
    Box& box;
    InlineBox rootInlineBox;

    BlockFlowBuilder(Style::Computer& c, Box& box)
        : box(box), rootInlineBox(_proseStyleFomStyle(c, *box.style)) {}

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
    void _buildChildDefaultDisplay(Style::Computer& c, Gc::Ref<Dom::Element> child, Rc<Style::ComputedStyle> childStyle, Display display);

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
                _buildText(*text, box.style, rootInlineBox);
            }
        }
    }

    // https://www.w3.org/TR/css-inline-3/#model
    void _flushRootInlineBoxIntoAnonymousBox() {
        if (not rootInlineBox.active())
            return;

        // The root inline box inherits from its parent block container, but is otherwise unstyleable.
        auto style = makeRc<Style::ComputedStyle>(Style::ComputedStyle::initial());
        style->inherit(*box.style);
        style->display = Display{Display::Inside::FLOW, Display::Outside::BLOCK};

        auto newInlineBox = InlineBox::fromInterruptedInlineBox(rootInlineBox);
        box.add({style, box.fontFace, std::move(rootInlineBox), nullptr});
        rootInlineBox = std::move(newInlineBox);
    }


    void _finalizeParentBoxAndFlushInline() {
        if (not rootInlineBox.active())
            return;

        if (box.children()) {
            _flushRootInlineBoxIntoAnonymousBox();
            return;
        }

        auto newRootInlineBox = InlineBox::fromInterruptedInlineBox(rootInlineBox);
        box.content = std::move(rootInlineBox);
        rootInlineBox = std::move(newRootInlineBox);
    }

    void buildFromElement(Style::Computer& c, Gc::Ref<Dom::Element> el) {
        if (el->tagName == Html::IMG) {
            _buildImage(c, *el, box.style, rootInlineBox);
        } else if (el->tagName == Html::INPUT) {
            _buildInputProse(c, el, box);
        } else if (el->tagName == Html::SVG) {
            // TODO: _buildSVG
        } else if (el->tagName == Html::BR) {
            // do nothing
        } else {
            _buildChildren(c, el);
        }
        _finalizeParentBoxAndFlushInline();
    }

    static void createBoxAndBuildfromElement(Style::Computer& c, Box& parent, Rc<Style::ComputedStyle> style, Gc::Ref<Dom::Element> el) {
        auto font = _lookupFontface(c.fontBook, *style);
        Box box = {style, font, el};
        BlockFlowBuilder{c, box}.buildFromElement(c, el);

        box.attrs = _parseDomAttr(el);
        parent.add(std::move(box));
    }
};

// Similar abstraction to https://webkit.org/blog/115/webcore-rendering-ii-blocks-and-inlines/
export struct InlineFlowBuilder {
    Box& rootBox;
    MutCursor<BlockFlowBuilder> parentBlockBuilder;

    InlineFlowBuilder(Box& rootBox, MutCursor<BlockFlowBuilder> parentBlockBuilder)
        : rootBox(rootBox), parentBlockBuilder(parentBlockBuilder) {}

    // https://www.w3.org/TR/css-display-3/#box-generation
    void _buildChildBoxDisplay(Style::Computer& c, Gc::Ref<Dom::Element> child, Rc<Style::ComputedStyle> childStyle, Display display) {
        if (display == Display::NONE)
            return;
        else
            _buildChildren(c, child, childStyle);
    }

    // Dispatching children from an Inline Flow context based on their outer and inner roles
    // https://www.w3.org/TR/css-display-3/#outer-role
    // https://www.w3.org/TR/css-display-3/#inner-model
    void _buildChildDefaultDisplay(Style::Computer& c, Gc::Ref<Dom::Element> child, Rc<Style::ComputedStyle> childStyle, Display display) {
        if (display == Display::Outside::BLOCK) {
            parentBlockBuilder->_flushRootInlineBoxIntoAnonymousBox();
            _createAndBuildBlockLevelBox(c, child, childStyle, rootBox, display);
        } else {
            // No new boxes need to be created in this case, just spans
            if (display == Display::Inside::FLOW) {
                buildFromElement(c, child, childStyle);
                return;
            }
            _createAndBuildInlineLevelBox(c, child, childStyle, parentBlockBuilder->rootInlineBox, display);
        }
    }

    // Dispatching children from block-level context based on their node type and display property in case of element
    // https://www.w3.org/TR/css-display-3/#the-display-properties
    void _buildChildren(Style::Computer& c, Gc::Ref<Dom::Element> el, Rc<Style::ComputedStyle> style) {
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
                _buildText(*text, style, parentBlockBuilder->rootInlineBox);
            }
        }
    }

    void buildFromElement(Style::Computer& c, Gc::Ref<Dom::Element> el, Rc<Style::ComputedStyle>& style) {
        if (el->tagName == Html::IMG) {
            _buildImage(c, *el, rootBox.style, parentBlockBuilder->rootInlineBox);
        } else if (el->tagName == Html::BR) {
            parentBlockBuilder->_flushRootInlineBoxIntoAnonymousBox();
        } else if (el->tagName == Html::INPUT) {
            _buildInputProse(c, el, rootBox);
        } else {
            auto proseStyle = _proseStyleFomStyle(c, *style);
            parentBlockBuilder->rootInlineBox.startInlineBox(proseStyle);
            _buildChildren(c, el, style);
            parentBlockBuilder->rootInlineBox.endInlineBox();
        }
    }
};


void BlockFlowBuilder::_buildChildDefaultDisplay(Style::Computer& c, Gc::Ref<Dom::Element> child, Rc<Style::ComputedStyle> childStyle, Display display) {
    if (display == Display::Outside::BLOCK) {
        _flushRootInlineBoxIntoAnonymousBox();
        _createAndBuildBlockLevelBox(c, child, childStyle, box, display);
    } else {
        // No need to create box in this case
        if (display == Display::Inside::FLOW) {
            InlineFlowBuilder{box, this}.buildFromElement(c, child, childStyle);
            return;
        }
        _createAndBuildInlineLevelBox(c, child, childStyle, rootInlineBox, display);
    }
}


// https://www.w3.org/TR/css-display-3/#layout-specific-display
void _buildChildInternalDisplay(Style::Computer& c, Gc::Ref<Dom::Element> child, Rc<Style::ComputedStyle> childStyle, Box& parent) {
    // FIXME: We should create wrapping boxes related to table or ruby, following the FC specification. However, for now,
    // we just wrap it in a single box.
    BlockFlowBuilder::createBoxAndBuildfromElement(c, parent, childStyle, child);
}

// https://www.w3.org/TR/css-display-3/#outer-role
static void _createAndBuildBlockLevelBox(Style::Computer& c, Gc::Ref<Dom::Element> el, Rc<Style::ComputedStyle> style, Box& parent, Display display) {
    if (display == Display::Inside::TABLE) {
        auto wrapper = _createTableWrapperAndBuildTable(c, style, el);
        parent.add(std::move(wrapper));
    } else if (display == Display::Inside::FLEX) {
        auto container = _createFlexContainerAndBuildFlex(c, el, style);
        parent.add(std::move(container));
    } else if (display == Display::Inside::FLOW) {
        BlockFlowBuilder::createBoxAndBuildfromElement(c, parent, style, el);
    } else {
        // FIXME: fallback to FLOW since not implemented
        BlockFlowBuilder::createBoxAndBuildfromElement(c, parent, style, el);
    }
}

// https://www.w3.org/TR/css-display-3/#outer-role
static void _createAndBuildInlineLevelBox(Style::Computer& c, Gc::Ref<Dom::Element> el, Rc<Style::ComputedStyle> style, InlineBox& rootInlineBox, Display display) {
    if (display == Display::Inside::TABLE) {
        auto wrapper = _createTableWrapperAndBuildTable(c, style, el);
        rootInlineBox.add(std::move(wrapper));
    } else if (display == Display::Inside::FLEX) {
        auto container = _createFlexContainerAndBuildFlex(c, el, style);
        rootInlineBox.add(std::move(container));
    } else {
        // FLOW-ROOT and fallback
        auto font = _lookupFontface(c.fontBook, *style);
        Box box = {style, font, el};
        BlockFlowBuilder{c, box}.buildFromElement(c, el);
        box.attrs = _parseDomAttr(el);
        rootInlineBox.add(std::move(box));
    }
}

// MARK: Build Replace ---------------------------------------------------------

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

// MARK: Build Flex -----------------------------------------------------------

static void _buildFlexChildren(Style::Computer& c, Gc::Ref<Dom::Element> flexContainerEl, Box& flexContainer);

static void _buildFlexChildBoxDisplay(Style::Computer& c, Gc::Ref<Dom::Element> child, Display display, Box& flexContainer) {
    if (display == Display::NONE)
        return;
    else
        _buildFlexChildren(c, child, flexContainer);
}

static void _buildFlexChildDefaultDisplay(Style::Computer& c, Gc::Ref<Dom::Element> child, Rc<Style::ComputedStyle> childStyle, Display display, Box& flexContainer) {
    // The display value of a flex item is blockified: if the specified display of an in-flow child of an element
    // generating a flex container is an inline-level value, it computes to its block-level equivalent.
    childStyle->display = childStyle->display.blockify();
    _createAndBuildBlockLevelBox(c, child, childStyle, flexContainer, display);
}

// https://www.w3.org/TR/css-flexbox-1/#flex-items
static void _buildFlexTextNodeChild(Style::Computer& c, Gc::Ref<Dom::Text> text, Box& flexContainer) {
    // // Each in-flow child of a flex container becomes a flex item,
    // and each contiguous sequence of child text runs is wrapped in an anonymous block container flex item.
    auto anonymousBoxStyle = makeRc<Style::ComputedStyle>(Style::ComputedStyle::initial());
    anonymousBoxStyle->inherit(*flexContainer.style);
    anonymousBoxStyle->display = anonymousBoxStyle->display.blockify();

    Box textBox{anonymousBoxStyle, flexContainer.fontFace, nullptr};
    InlineBox rootInlineBox{_proseStyleFomStyle(c, *anonymousBoxStyle)};

    // However, if the entire sequence of child text runs contains only white space
    // (i.e. characters that can be affected by the white-space property) it is instead not rendered
    // (just as if its text nodes were display:none).
    if (not _buildText(*text, textBox.style, rootInlineBox))
        return;

    textBox.content = std::move(rootInlineBox);
    flexContainer.add(std::move(textBox));
}

static void _buildFlexChildren(Style::Computer& c, Gc::Ref<Dom::Element> flexContainerEl, Box& flexContainer) {
    for (auto child = flexContainerEl->firstChild(); child; child = child->nextSibling()) {
        if (auto el = child->is<Dom::Element>()) {
            auto childStyle = c.computeFor(*flexContainer.style, *el);
            auto display = childStyle->display;

            if (display.type() == Display::Type::BOX) {
                _buildFlexChildBoxDisplay(c, *el, display, flexContainer);
            } else if (display.type() == Display::Type::INTERNAL) {
                _buildChildInternalDisplay(c, *el, childStyle, flexContainer);
            } else {
                _buildFlexChildDefaultDisplay(c, *el, childStyle, display, flexContainer);
            }
        } else if (auto text = child->is<Dom::Text>()) {
            _buildFlexTextNodeChild(c, *text, flexContainer);
        }
    }
}

static Box _createFlexContainerAndBuildFlex(Style::Computer& c, Gc::Ref<Dom::Element> flexContainerEl, Rc<Style::ComputedStyle> flexContainerStyle) {
    auto font = _lookupFontface(c.fontBook, *flexContainerStyle);

    Box flexContainer = {flexContainerStyle, font, flexContainerEl};
    _buildFlexChildren(c, flexContainerEl, flexContainer);
    flexContainer.attrs = _parseDomAttr(flexContainerEl);

    return flexContainer;
}

// MARK: Build Table -----------------------------------------------------------

static void _buildTableChildren(Style::Computer& c, Gc::Ref<Dom::Node> node, Box& tableWrapperBox, Rc<Style::ComputedStyle> tableBoxStyle) {
    Box tableBox{
        tableBoxStyle,
        tableWrapperBox.fontFace,
        node->is<Dom::Element>()
    };

    tableBox.style->display = Display::Internal::TABLE_BOX;

    bool captionsOnTop = tableBox.style->table->captionSide == CaptionSide::TOP;

    if (captionsOnTop) {
        for (auto child = node->firstChild(); child; child = child->nextSibling()) {
            if (auto el = child->is<Dom::Element>()) {
                if (el->tagName == Html::CAPTION) {
                    BlockFlowBuilder::createBoxAndBuildfromElement(
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
                BlockFlowBuilder::createBoxAndBuildfromElement(
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
                    BlockFlowBuilder::createBoxAndBuildfromElement(
                        c, tableWrapperBox,
                        c.computeFor(*tableWrapperBox.style, *el), *el
                    );
                }
            }
        }
    }
}

static Box _createTableWrapperAndBuildTable(Style::Computer& c, Rc<Style::ComputedStyle> tableStyle, Gc::Ref<Dom::Element> tableBoxEl) {
    auto font = _lookupFontface(c.fontBook, *tableStyle);

    auto wrapperStyle = makeRc<Style::ComputedStyle>(Style::ComputedStyle::initial());
    wrapperStyle->display = tableStyle->display;
    wrapperStyle->margin = tableStyle->margin;

    Box wrapper = {wrapperStyle, font, tableBoxEl};
    _buildTableChildren(c, tableBoxEl, wrapper, tableStyle);
    wrapper.attrs = _parseDomAttr(tableBoxEl);

    return wrapper;
}

// MARK: Build -----------------------------------------------------------------

export Box build(Style::Computer& c, Gc::Ref<Dom::Document> doc) {
    if (auto el = doc->documentElement()) {
        auto style = c.computeFor(Style::ComputedStyle::initial(), *el);
        auto font = _lookupFontface(c.fontBook, *style);

        Box root = {style, font, el};
        BlockFlowBuilder{c, root}.buildFromElement(c, *el);
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
