module;

#include <karm-image/loader.h>
#include <karm-text/loader.h>
#include <karm-text/prose.h>
#include <vaev-dom/document.h>
#include <vaev-style/computer.h>

export module Vaev.Layout:builder;

import :values;

namespace Vaev::Layout {

static void _buildImage(Style::Computer& c, Gc::Ref<Dom::Element> el, Rc<Style::Computed> parentStyle, InlineContent& ic);

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

auto RE_SEGMENT_BREAK = Re::single('\n', '\r', '\f', '\v');

// MARK: Build Block and Inline -----------------------------------------------------------

export struct InlineBuilder {
    Box& rootBox;
    InlineContent& ic;

    InlineBuilder(Box& rootBox, InlineContent& ic)
        : rootBox(rootBox), ic(ic) {}

    void buildFromText(Style::Computer& c, Gc::Ref<Dom::Text> node, Rc<Style::Computed> parentStyle) {
        auto style = makeRc<Style::Computed>(Style::Computed::initial());
        style->inherit(*parentStyle);

        auto fontFace = _lookupFontface(c.fontBook, *style);
        Io::SScan scan{node->data()};
        scan.eat(Re::space());
        if (scan.ended())
            return;

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

        switch (style->text->align) {
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

        auto prose = makeRc<Text::Prose>(proseStyle);
        auto whitespace = style->text->whiteSpace;

        while (not scan.ended()) {
            switch (style->text->transform) {
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

        ic.add(std::move(prose), style, fontFace);
    }

    void _buildChildBoxDisplay(Style::Computer& c, Gc::Ref<Dom::Element> child, Rc<Style::Computed> childStyle, Display display);

    void _buildChildInternalDisplay(Style::Computer& c, Gc::Ref<Dom::Element> child, Rc<Style::Computed> childStyle);

    void _buildChildDefaultDisplay(Style::Computer& c, Gc::Ref<Dom::Element> child, Rc<Style::Computed> childStyle, Display display);

    void _dispatchChildren(Style::Computer& c, Gc::Ref<Dom::Element> el, Rc<Style::Computed> style) {
        for (auto child = el->firstChild(); child; child = child->nextSibling()) {
            if (auto el = child->is<Dom::Element>()) {
                auto childStyle = c.computeFor(*style, *el);
                auto display = childStyle->display;

                if (display.type() == Display::Type::BOX) {
                    _buildChildBoxDisplay(c, *el, style, display);
                } else if (display.type() == Display::Type::INTERNAL) {
                    _buildChildInternalDisplay(c, *el, childStyle);
                } else {
                    _buildChildDefaultDisplay(c, *el, childStyle, display);
                }
            } else if (auto text = child->is<Dom::Text>()) {
                buildFromText(c, *text, style);
            }
        }
    }

    void buildFromElement(Style::Computer& c, Gc::Ref<Dom::Element> el, Rc<Style::Computed>& style) {
        if (el->tagName == Html::IMG)
            _buildImage(c, *el, rootBox.style, ic);
        else if (el->tagName == Html::BR) {
            ic.breakLine();
        } else
            _dispatchChildren(c, el, style);
    }
};

export struct BlockBuilder {
    InlineContent ic;
    Box& box;
    Gc::Ref<Dom::Node> node;

    BlockBuilder(Box& box, Gc::Ref<Dom::Node> node)
        : box(box), node(node) {}

    void _buildChildBoxDisplay(Style::Computer& c, Gc::Ref<Dom::Node> child, Display display) {
        if (display == Display::NONE) {
            return;
        } else {
            _dispatchChildren(c, child);
        }
    }

    void _buildChildInternalDisplay(Style::Computer& c, Gc::Ref<Dom::Element> child, Rc<Style::Computed> childStyle) {
        // FIXME: Boxes with layout-specific display types generate anonymous wrapper boxes around themselves when
        // placed in an incompatible parent, as defined by their respective specifications.
        BlockBuilder::buildFromBlockElement(c, box, childStyle, child);
    }

    void _buildChildDefaultDisplay(Style::Computer& c, Gc::Ref<Dom::Element> child, Rc<Style::Computed> childStyle, Display display) {
        if (display == Display::Outside::INLINE) {
            if (display == Display::Inside::FLOW) {
                InlineBuilder{box, ic}.buildFromElement(c, child, childStyle);
            } else if (display == Display::Inside::FLOW_ROOT) {
                BlockBuilder::buildFromBlockElement(c, box, childStyle, child);
            } else {
                // FIXME: fallback to FLOW since not implemented
                InlineBuilder{box, ic}.buildFromElement(c, child, childStyle);
            }
        } else {
            InlineContent::savePendingInline(box, ic);

            if (display == Display::TABLE) {
                BlockBuilder::buildFromTableElement(c, box, childStyle, child);
            } else if (display == Display::Inside::FLOW or display == Display::Inside::FLEX) {
                BlockBuilder::buildFromBlockElement(c, box, childStyle, child);
            } else {
                BlockBuilder::buildFromBlockElement(c, box, childStyle, child);
            }
        }
    }

    void _dispatchChildren(Style::Computer& c, Gc::Ref<Dom::Node> parent) {
        for (auto child = parent->firstChild(); child; child = child->nextSibling()) {
            if (auto el = child->is<Dom::Element>()) {
                auto childStyle = c.computeFor(*box.style, *el);
                auto display = childStyle->display;

                if (display.type() == Display::Type::BOX) {
                    _buildChildBoxDisplay(c, *el, display);
                } else if (display.type() == Display::Type::INTERNAL) {
                    _buildChildInternalDisplay(c, *el, childStyle);
                } else {
                    _buildChildDefaultDisplay(c, *el, childStyle, display);
                }
            } else if (auto text = child->is<Dom::Text>()) {
                InlineBuilder{box, ic}.buildFromText(c, *text, box.style);
            }
        }
    }

    void build(Style::Computer& c) {
        auto el = node->is<Dom::Element>();
        if (not el) {
            _dispatchChildren(c, node);
        } else {
            if (el->tagName == Html::IMG)
                _buildImage(c, *el, box.style, ic);
            else if (el->tagName == Html::SVG) {
                // FIXME
                return;
            } else if (el->tagName == Html::BR) {
                ic.breakLine();
            } else
                _dispatchChildren(c, node);
        }
        InlineContent::savePendingInline(box, ic);
    }

    static Box buildFromRoot(Style::Computer& c, Gc::Ref<Dom::Document> doc) {
        auto style = c.computeFor(Style::Computed::initial(), *doc->documentElement());
        auto font = _lookupFontface(c.fontBook, *style);
        Box box = {style, font};

        BlockBuilder{box, doc}.build(c);

        return box;
    }

    static void buildFromBlockElement(Style::Computer& c, Box& parent, Rc<Style::Computed> style, Gc::Ref<Dom::Element> el) {
        auto font = _lookupFontface(c.fontBook, *style);
        Box box = {style, font};

        BlockBuilder{box, el}.build(c);

        box.attrs = _parseDomAttr(el);
        parent.add(std::move(box));
    }

    // MARK: Build Table -----------------------------------------------------------

    void _buildTableBoxes(Style::Computer& c, Rc<Style::Computed> tableBoxStyle) {
        auto& tableWrapperBox = box;
        Box tableBox{
            tableBoxStyle,
            tableWrapperBox.fontFace
        };

        tableBox.style->display = Display::Internal::TABLE_BOX;

        bool captionsOnTop = tableBox.style->table->captionSide == CaptionSide::TOP;

        if (captionsOnTop) {
            for (auto child = node->firstChild(); child; child = child->nextSibling()) {
                if (auto el = child->is<Dom::Element>()) {
                    if (el->tagName == Html::CAPTION) {
                        BlockBuilder::buildFromBlockElement(
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
                    BlockBuilder::buildFromBlockElement(
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
                        BlockBuilder::buildFromBlockElement(
                            c, tableWrapperBox,
                            c.computeFor(*tableWrapperBox.style, *el), *el
                        );
                    }
                }
            }
        }
    }

    static void buildFromTableElement(Style::Computer& c, Box& parent, Rc<Style::Computed> style, Gc::Ref<Dom::Element> el) {
        auto font = _lookupFontface(c.fontBook, *style);

        auto wrapperStyle = makeRc<Style::Computed>(Style::Computed::initial());
        wrapperStyle->display = style->display;
        wrapperStyle->margin = style->margin;
        Box wrapper = {wrapperStyle, font};

        BlockBuilder{wrapper, el}._buildTableBoxes(c, style);

        wrapper.attrs = _parseDomAttr(el);
        parent.add(std::move(wrapper));
    }
};

void InlineBuilder::_buildChildBoxDisplay(Style::Computer& c, Gc::Ref<Dom::Element> child, Rc<Style::Computed> parentStyle, Display display) {
    if (display == Display::NONE) {
        return;
    } else {
        _dispatchChildren(c, child, parentStyle);
    }
}

void InlineBuilder::_buildChildInternalDisplay(Style::Computer& c, Gc::Ref<Dom::Element> child, Rc<Style::Computed> childStyle) {
    // FIXME: Boxes with layout-specific display types generate anonymous wrapper boxes around themselves when
    // placed in an incompatible parent, as defined by their respective specifications.
    BlockBuilder::buildFromBlockElement(c, rootBox, childStyle, child);
}

void InlineBuilder::_buildChildDefaultDisplay(Style::Computer& c, Gc::Ref<Dom::Element> child, Rc<Style::Computed> childStyle, Display display) {
    if (display == Display::Outside::INLINE) {
        if (display == Display::Inside::FLOW) {
            buildFromElement(c, child, childStyle);
        } else if (display == Display::Inside::FLOW_ROOT) {
            BlockBuilder::buildFromBlockElement(c, rootBox, childStyle, child);
        } else {
            // FIXME: fallback to FLOW since not implemented
            buildFromElement(c, child, childStyle);
        }
    } else {
        InlineContent::savePendingInline(rootBox, ic);

        if (display == Display::Inside::TABLE) {
            BlockBuilder::buildFromTableElement(c, rootBox, childStyle, child);
        } else if (display == Display::Inside::FLOW or display == Display::Inside::FLEX) {
            BlockBuilder::buildFromBlockElement(c, rootBox, childStyle, child);
        } else {
            // FIXME: fallback to FLOW since not implemented
            BlockBuilder::buildFromBlockElement(c, rootBox, childStyle, child);
        }
    }
}

// MARK: Build Replace ---------------------------------------------------------

static void _buildImage(Style::Computer& c, Gc::Ref<Dom::Element> el, Rc<Style::Computed> parentStyle, InlineContent& ic) {
    auto style = c.computeFor(*parentStyle, el);
    auto font = _lookupFontface(c.fontBook, *style);

    auto src = el->getAttribute(Html::SRC_ATTR).unwrapOr(""s);
    auto url = Mime::Url::resolveReference(el->baseURI(), Mime::parseUrlOrPath(src))
                   .unwrapOr("bundle://vaev-driver/missing.qoi"_url);

    auto img = Karm::Image::load(url).unwrapOrElse([] {
        return Karm::Image::loadOrFallback("bundle://vaev-driver/missing.qoi"_url).unwrap();
    });
    ic.add(img, style, font);
}

// MARK: Build -----------------------------------------------------------------

export Box build(Style::Computer& c, Gc::Ref<Dom::Document> doc) {
    if (auto el = doc->documentElement())
        return BlockBuilder::buildFromRoot(c, doc);

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
        return {style, fontFace, InlineContent{prose}};
    }

    return {style, fontFace};
}

} // namespace Vaev::Layout
