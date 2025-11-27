module;

#include <karm-core/macros.h>
#include <utility>

export module Vaev.Engine:layout.builder;

import Karm.Image;
import Karm.Gc;
import Karm.Debug;
import Karm.Font;
import Karm.Ref;
import Karm.Gfx;
import Karm.Logger;

import :values;
import :style;
import :dom.element;
import :layout.values;
import :layout.svg;

namespace Vaev::Layout {

static constexpr bool DEBUG_BUILDER = false;

// MARK: Build Prose ----------------------------------------------------------

bool isSegmentBreak(Rune rune) {
    return rune == '\n' or rune == '\r' or rune == '\f' or rune == '\v';
}

static Gfx::ProseStyle _proseStyleFromStyle(Style::SpecifiedValues& style, Rc<Gfx::Fontface> fontFace) {
    // FIXME: We should pass this around from the top in order to properly resolve rems
    Resolver resolver{
        .rootFont = Gfx::Font{fontFace, 16},
        .boxFont = Gfx::Font{fontFace, 16},
    };
    Gfx::ProseStyle proseStyle{
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
        proseStyle.align = Gfx::TextAlign::LEFT;
        break;

    case TextAlign::END:
    case TextAlign::RIGHT:
        proseStyle.align = Gfx::TextAlign::RIGHT;
        break;

    case TextAlign::CENTER:
        proseStyle.align = Gfx::TextAlign::CENTER;
        break;

    default:
        // FIXME: Implement the rest
        break;
    }

    return proseStyle;
}

void _transformAndAppendRuneToProse(Rc<Gfx::Prose> prose, Rune rune, TextTransform transform) {
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
void _appendTextToInlineBox(Io::SScan scan, Rc<Style::SpecifiedValues> parentStyle, InlineBox& rootInlineBox) {
    auto whitespace = parentStyle->text->whiteSpace;
    bool whiteSpacesAreCollapsible =
        whitespace == WhiteSpace::NORMAL or
        whitespace == WhiteSpace::NOWRAP or
        whitespace == WhiteSpace::PRE_LINE;

    // A sequence of collapsible spaces at the beginning of a line is removed.
    if (not rootInlineBox.active())
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

bool _buildText(Str text, Rc<Style::SpecifiedValues> parentStyle, InlineBox& rootInlineBox, bool skipIfWhitespace) {
    if (skipIfWhitespace) {
        Io::SScan scan{text};
        scan.eat(Re::space());
        if (scan.ended())
            return false;
    }

    _appendTextToInlineBox(text, parentStyle, rootInlineBox);
    return true;
}

// MARK: Build Block -----------------------------------------------------------

struct BuilderContext {
    enum struct From {
        BLOCK,
        INLINE,
        FLEX,
        TABLE
    };

    From const from;
    Rc<Style::SpecifiedValues> const parentStyle;

    Box& _parent;
    MutCursor<InlineBox> _rootInlineBox;

    // https://www.w3.org/TR/css-inline-3/#model
    void flushRootInlineBoxIntoAnonymousBox() {
        if (not _rootInlineBox or not rootInlineBox().active())
            return;

        // The root inline box inherits from its parent block container, but is otherwise unstyleable.
        auto style = makeRc<Style::SpecifiedValues>(Style::SpecifiedValues::initial());
        style->inherit(*parentStyle);
        style->display = Display{Display::Inside::FLOW, Display::Outside::BLOCK};

        auto newInlineBox = InlineBox::fromInterruptedInlineBox(*_rootInlineBox);
        _parent.add({style, std::move(*_rootInlineBox), nullptr});
        *_rootInlineBox = std::move(newInlineBox);
    }

    void finalizeParentBoxAndFlushInline() {
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

    Rc<Style::SpecifiedValues> style() {
        return parentStyle;
    }

    void addToParentBox(Box&& box) {
        _parent.add(std::move(box));
    }

    InlineBox& rootInlineBox() {
        if (_rootInlineBox == nullptr)
            panic("no root inline box set for the current builder context");
        return *_rootInlineBox;
    }

    void addToInlineRoot(Box&& box) {
        rootInlineBox().add(std::move(box));
    }

    // FIXME: find me a better name
    void startInlineBox(Gfx::ProseStyle proseStyle) {
        rootInlineBox().startInlineBox(proseStyle);
    }

    void endInlineBox() {
        rootInlineBox().endInlineBox();
    }

    Content& content() {
        return _parent.content;
    }

    BuilderContext toBlockContext(Box& parent, InlineBox& rootInlineBox) {
        return {
            From::BLOCK,
            parent.style,
            parent,
            &rootInlineBox,
        };
    }

    // NOTE: although all inline elements from FLEX containers are blockified, its less complex to have a
    // rootInlineBox setted for it and then calling `_flushRootInlineBoxIntoAnonymousBox` right after a text is added
    BuilderContext toFlexContext(Box& parent, InlineBox& rootInlineBox) {
        return {
            From::FLEX,
            parent.style,
            parent,
            &rootInlineBox,
        };
    }

    BuilderContext toBlockContextWithoutRootInline(Box& parent) {
        return {
            From::BLOCK,
            parent.style,
            parent,
            nullptr,
        };
    }

    BuilderContext toInlineContext(Rc<Style::SpecifiedValues> parentStyle) {
        return {
            From::INLINE,
            parentStyle,
            _parent,
            _rootInlineBox,
        };
    }

    // https://www.w3.org/TR/css-tables-3/#fixup-algorithm
    // SPEC: All inline elements are wrapped by cell boxes. Thus, only the cell table content will have a inline box
    // set, while other internal table contexts won't have a inline box
    BuilderContext toTableContent(Box& parent) {
        return {
            From::TABLE,
            parent.style,
            parent,
            nullptr,
        };
    }

    BuilderContext toTableCellContent(Box& parent, InlineBox& rootInlineBox) {
        return {
            From::TABLE,
            parent.style,
            parent,
            &rootInlineBox,
        };
    }
};

static void _buildNode(BuilderContext bc, Gc::Ref<Dom::Node> node);
static void _buildPseudoElement(BuilderContext bc, Rc<Dom::PseudoElement> pseudoElement);

// MARK: Build void/leaves ---------------------------------------------------------

// https://developer.mozilla.org/en-US/docs/Web/API/Document_Object_Model/Whitespace#how_does_css_process_whitespace/
static void _buildText(BuilderContext bc, Str text, Rc<Style::SpecifiedValues> parentStyle) {
    // https://www.w3.org/TR/css-tables-3/#fixup-algorithm
    // TODO: For tables, the default case is to skip whitespace text, but there are some extra checks to be done

    // https://www.w3.org/TR/css-flexbox-1/#flex-items
    // However, if the entire sequence of child text runs contains only white space
    // (i.e. characters that can be affected by the white-space property) it is instead not rendered
    // (just as if its text nodes were display:none).

    bool shouldSkipWhitespace =
        bc.from == BuilderContext::From::TABLE or
        bc.from == BuilderContext::From::FLEX or
        bc.from == BuilderContext::From::BLOCK;

    bool addedNonWhitespace = _buildText(text, parentStyle, bc.rootInlineBox(), shouldSkipWhitespace);

    // https://www.w3.org/TR/css-flexbox-1/#algo-anon-box
    // https://www.w3.org/TR/css-flexbox-1/#flex-items
    // Each in-flow child of a flex container becomes a flex item,
    // and each contiguous sequence of child text runs is wrapped in an anonymous block container flex item.
    // However, if the entire sequence of child text runs contains only white space
    // (i.e. characters that can be affected by the white-space property) it is instead not rendered
    // (just as if its text nodes were display:none).
    if (addedNonWhitespace and bc.from == BuilderContext::From::FLEX) {
        bc.flushRootInlineBoxIntoAnonymousBox();
    }
}

static void _buildImage(BuilderContext bc, Gc::Ref<Dom::Element> el) {
    bc.content() = el->imageContent.unwrap();
}

static void _buildInputProse(BuilderContext bc, Gc::Ref<Dom::Element> el) {
    auto font = el->specifiedValues()->fontFace;
    Resolver resolver{
        .rootFont = Gfx::Font{font, 16},
        .boxFont = Gfx::Font{font, 16},
    };
    Gfx::ProseStyle proseStyle = _proseStyleFromStyle(*el->specifiedValues(), font);

    auto value = ""s;
    if (el->hasAttribute(Html::VALUE_ATTR))
        value = el->getAttribute(Html::VALUE_ATTR).unwrap();
    else if (el->hasAttribute(Html::PLACEHOLDER_ATTR))
        value = el->getAttribute(Html::PLACEHOLDER_ATTR).unwrap();

    auto prose = makeRc<Gfx::Prose>(proseStyle, value);

    // FIXME: we should guarantee that input has no children (not added before nor to add after)
    bc.content() = InlineBox{prose};
}

static void buildBlockFlowFromElement(BuilderContext bc, Gc::Ref<Dom::Element> el);
void buildSVGAggregate(Gc::Ref<Dom::Element> el, SVG::Group& group);

void buildSVGElement(Gc::Ref<Dom::Element> el, SVG::Group& group) {
    if (SVG::isShape(el->qualifiedName)) {
        group.add(SVG::Shape::build(el->specifiedValues(), el->qualifiedName));
    } else if (el->qualifiedName == Svg::G_TAG) {
        SVG::Group nestedGroup{el->specifiedValues()};
        buildSVGAggregate(el, nestedGroup);
        group.add(std::move(nestedGroup));
    } else if (el->qualifiedName == Svg::SVG_TAG) {
        SVGRoot newSvgRoot{el->specifiedValues()};
        buildSVGAggregate(el, newSvgRoot);
        group.add(std::move(newSvgRoot));
    } else if (el->qualifiedName == Svg::FOREIGN_OBJECT_TAG) {
        Box box{el->specifiedValues(), el};

        InlineBox rootInlineBox{_proseStyleFromStyle(
            *el->specifiedValues(),
            el->specifiedValues()->fontFace
        )};

        BuilderContext bc{
            BuilderContext::From::BLOCK,
            el->specifiedValues(),
            box,
            &rootInlineBox,
        };

        buildBlockFlowFromElement(bc, *el);

        group.add(std::move(box));
    } else {
        // TODO
        logWarn("cannot build element into svg tree: {}", el->qualifiedName);
    }
}

void buildSVGAggregate(Gc::Ref<Dom::Element> el, SVG::Group& group) {
    for (auto child = el->firstChild(); child; child = child->nextSibling()) {
        if (auto el = child->is<Dom::Element>()) {
            buildSVGElement(*el, group);
        }
        // TODO: process text into svg tree
    }
}

SVGRoot _buildSVG(Gc::Ref<Dom::Element> el) {
    SVGRoot svgRoot{el->specifiedValues()};
    buildSVGAggregate(el, svgRoot);
    return svgRoot;
}

static void _buildVoidElement(BuilderContext bc, Gc::Ref<Dom::Element> el) {
    if (el->qualifiedName == Html::INPUT_TAG) {
        // FIXME: Only support appearance: none for now
        // https://www.w3.org/TR/css-ui-4/#valdef-appearance-none

        auto type = el->getAttribute(Html::TYPE_ATTR).unwrapOr(""s);
        if (type == "hidden") {
            // Don't generate a box
        } else if (type == "radio" or type == "checkbox") {
            // NOTE: The UA may however give them a different look and feel as long as it remains possible to operate the widget.
            // https://www.w3.org/TR/css-ui-4/#appearance-semantics
            Math::Rectf rect = {14, 14};

            Rc<Scene::Stack> box = makeRc<Scene::Stack>();

            box->add(
                makeRc<Scene::Box>(
                    rect,
                    Gfx::Borders{
                        .radii = type == "checkbox" ? 2 : 99,
                        .widths = 1,
                        .fills = {
                            Gfx::BLACK,
                            Gfx::BLACK,
                            Gfx::BLACK,
                            Gfx::BLACK,
                        },
                        .styles = {
                            Gfx::BorderStyle::SOLID,
                            Gfx::BorderStyle::SOLID,
                            Gfx::BorderStyle::SOLID,
                            Gfx::BorderStyle::SOLID,
                        },
                    },
                    Gfx::Outline{}, Vec<Gfx::Fill>{Gfx::WHITE}
                )
            );

            bool checked = el->hasAttribute(Html::CHECKED_ATTR) or el->getAttribute(Html::VALUE_ATTR).unwrapOr(""s) == "on";

            if (checked) {
                box->add(
                    makeRc<Scene::Box>(
                        rect.shrink(2),
                        Gfx::Borders{
                            .radii = type == "checkbox" ? 2 : 99,
                            .widths = 0,
                            .fills = {
                            },
                            .styles = {},
                        },
                        Gfx::Outline{}, Vec<Gfx::Fill>{Gfx::BLUE500}
                    )
                );
            }

            bc.content() = Rc<Scene::Node>{box};
        } else {
            _buildInputProse(bc, el);
        }
    } else if (el->qualifiedName == Html::IMG_TAG) {
        _buildImage(bc, el);
    }
}

// NOTE: When inlines, <svg> and <img> should not be treated as inline-flow (<span>) and should thus be inside a strut,
// that is, should be inline-block, where we always create a box to wrap around.
// These els are special cases since all other els can take whichever display is set in their style. However, for these
// els, an internal display value will never be valid. This relates to a somewhat "replaced" property from these
// elements, but <img> is replaced whereas <svg> per se is not.
bool _alwaysInlineBlock(Gc::Ref<Dom::Element> el) {
    return el->qualifiedName == Html::IMG_TAG or el->qualifiedName == Svg::SVG_TAG;
}

// MARK: Build flow -------------------------------------------------------------------------------

static void _buildChildren(BuilderContext bc, Gc::Ref<Dom::Node> parent);

static void createAndBuildInlineFlowfromElement(BuilderContext bc, Rc<Style::SpecifiedValues> style, Gc::Ref<Dom::Element> el) {
    if (el->qualifiedName == Html::BR_TAG) {
        bc.flushRootInlineBoxIntoAnonymousBox();
        return;
    }

    if (el->isVoidElement()) {
        _buildVoidElement(bc, el);
        return;
    }

    auto proseStyle = _proseStyleFromStyle(*style, el->specifiedValues()->fontFace);

    bc.startInlineBox(proseStyle);
    _buildChildren(bc.toInlineContext(style), el);
    bc.endInlineBox();
}

static void buildBlockFlowFromElement(BuilderContext bc, Gc::Ref<Dom::Element> el) {
    if (el->qualifiedName == Html::BR_TAG) {
        // do nothing
    } else if (el->qualifiedName == Svg::SVG_TAG) {
        bc.content() = _buildSVG(el);
    } else if (el->isVoidElement()) {
        _buildVoidElement(bc, el);
    } else {
        _buildChildren(bc, el);
    }
    bc.finalizeParentBoxAndFlushInline();
}

static Box createAndBuildBoxFromElement(BuilderContext bc, Rc<Style::SpecifiedValues> style, Gc::Ref<Dom::Element> el, Display display) {
    Box box = {style, el};
    InlineBox rootInlineBox{_proseStyleFromStyle(*style, el->specifiedValues()->fontFace)};

    auto newBc = display == Display::Inside::FLEX
                     ? bc.toFlexContext(box, rootInlineBox)
                     : bc.toBlockContext(box, rootInlineBox);

    buildBlockFlowFromElement(newBc, el);

    return box;
}

// MARK: Build Table -----------------------------------------------------------

static void _buildTableInternal(BuilderContext bc, Gc::Ref<Dom::Element> el, Rc<Style::SpecifiedValues> style, Display display);

struct AnonymousTableBoxWrapper {
    Opt<Box> rowBox, cellBox;
    Opt<InlineBox> rootInlineBoxForCell;

    BuilderContext& bc;

    AnonymousTableBoxWrapper(BuilderContext& bc) : bc(bc) {}

    void createRowIfNone(Rc<Style::SpecifiedValues> style) {
        if (rowBox)
            return;

        auto rowStyle = makeRc<Style::SpecifiedValues>(Style::SpecifiedValues::initial());
        rowStyle->inherit(*style);
        rowStyle->display = Display::Internal::TABLE_ROW;

        rowBox = Box{rowStyle, nullptr};
    }

    void createCellIfNone(Rc<Style::SpecifiedValues> style) {
        if (cellBox)
            return;

        auto cellStyle = makeRc<Style::SpecifiedValues>(Style::SpecifiedValues::initial());
        cellStyle->inherit(*style);
        cellStyle->display = Display::Internal::TABLE_CELL;

        cellBox = Box{cellStyle, nullptr};
        rootInlineBoxForCell = InlineBox{_proseStyleFromStyle(*style, style->fontFace)};
    }

    void finalizeAndResetCell() {
        if (not cellBox)
            return;

        cellBuilderContext().finalizeParentBoxAndFlushInline();
        if (cellBox->content != NONE) {
            if (rowBox)
                bc.toTableContent(*rowBox).addToParentBox(std::move(*cellBox));
            else
                bc.addToParentBox(std::move(*cellBox));
        }

        cellBox = NONE;
    }

    void finalizeAndResetRow() {
        if (not rowBox)
            return;

        if (rowBox->content != NONE)
            bc.addToParentBox(std::move(*rowBox));

        rowBox = NONE;
    }

    BuilderContext cellBuilderContext() {
        return bc.toTableCellContent(*cellBox, *rootInlineBoxForCell);
    }

    BuilderContext rowBuilderContext() {
        return bc.toTableContent(*rowBox);
    }
};

static void _buildNodeWrappedByCell(BuilderContext cellBuilderContext, Gc::Ref<Dom::Element> wrappedEl) {
    _buildNode(cellBuilderContext, *wrappedEl);
}

static void _buildCell(BuilderContext rowBuilderContext, Gc::Ref<Dom::Element> cellEl) {
    _buildNode(rowBuilderContext, *cellEl);
}

static void _buildTableChildrenWhileWrappingIntoAnonymousBox(BuilderContext bc, Gc::Ref<Dom::Element> parentEl, Rc<Style::SpecifiedValues> style, bool skipCaption, auto predForAccepted) {
    AnonymousTableBoxWrapper anonTableWrapper{bc};

    for (auto child = parentEl->firstChild(); child; child = child->nextSibling()) {
        if (auto childEL = child->is<Dom::Element>()) {
            auto childStyle = childEL->specifiedValues();
            auto display = childStyle->display;

            if (skipCaption and display == Display::Internal::TABLE_CAPTION)
                continue;

            if (predForAccepted(display)) {
                anonTableWrapper.finalizeAndResetCell();
                anonTableWrapper.finalizeAndResetRow();

                if (display == Display::Internal::TABLE_CELL) {
                    _buildCell(bc, *childEL);
                } else {
                    _buildTableInternal(bc, *childEL, childStyle, display);
                }
            } else {
                // Unexpected display type for children, it will be wrapped by anonymous box
                // First dispatching based on the parent's display type
                if (bc.parentStyle->display == Display::Internal::TABLE_ROW) {
                    anonTableWrapper.createCellIfNone(style);
                    _buildNodeWrappedByCell(anonTableWrapper.cellBuilderContext(), *childEL);
                } else {
                    if (display == Display::Internal::TABLE_CELL) {
                        anonTableWrapper.finalizeAndResetCell();
                        anonTableWrapper.createRowIfNone(style);

                        _buildCell(anonTableWrapper.rowBuilderContext(), *childEL);
                    } else {
                        anonTableWrapper.createRowIfNone(style);
                        anonTableWrapper.createCellIfNone(style);

                        _buildNodeWrappedByCell(anonTableWrapper.cellBuilderContext(), *childEL);
                    }
                }
            }
        } else if (auto text = child->is<Dom::Text>()) {
            if (bc.parentStyle->display != Display::Internal::TABLE_ROW)
                anonTableWrapper.createRowIfNone(style);
            anonTableWrapper.createCellIfNone(style);
            _buildText(anonTableWrapper.cellBuilderContext(), text->data(), style);
        }
    }

    anonTableWrapper.finalizeAndResetCell();
    anonTableWrapper.finalizeAndResetRow();
}

// https://www.w3.org/TR/css-tables-3/#fixup-algorithm
static void _buildTableInternal(BuilderContext bc, Gc::Ref<Dom::Element> el, Rc<Style::SpecifiedValues> style, Display display) {
    Box tableInternalBox = {style, el};

    switch (display.internal()) {
    case Display::Internal::TABLE_HEADER_GROUP:
    case Display::Internal::TABLE_FOOTER_GROUP:
    case Display::Internal::TABLE_ROW_GROUP: {
        // An anonymous table-row box must be generated around each sequence of consecutive children of a
        // table-row-group box which are not table-row boxes.
        _buildTableChildrenWhileWrappingIntoAnonymousBox(bc.toTableContent(tableInternalBox), el, style, false, [](Display const& display) {
            return display == Display::Internal::TABLE_ROW;
        });
        break;
    }
    case Display::Internal::TABLE_COLUMN_GROUP: {
        // "Children of a table-column-group which are not a table-column." should be discarded
        for (auto child = el->firstChild(); child; child = child->nextSibling()) {
            if (auto el = child->is<Dom::Element>()) {
                auto childStyle = el->specifiedValues();
                if (childStyle->display != Display::Internal::TABLE_COLUMN)
                    continue;
                _buildTableInternal(bc.toTableContent(tableInternalBox), *el, childStyle, display);
            }
        }
        break;
    }
    case Display::Internal::TABLE_ROW: {
        // An anonymous table-cell box must be generated around each sequence of consecutive children of a
        // table-row box which are not table-cell boxes.
        _buildTableChildrenWhileWrappingIntoAnonymousBox(bc.toTableContent(tableInternalBox), el, style, false, [](Display const& display) {
            return display == Display::Internal::TABLE_CELL;
        });
    }
    case Display::Internal::TABLE_COLUMN: {
        // "Children of a table-column." should be discarded
        // do nothing
        break;
    }
    case Display::Internal::TABLE_BOX:
    case Display::Internal::TABLE_CAPTION:
    case Display::Internal::TABLE_CELL: {
        panic("table internal display in wrong flow");
        break;
    }
    default:
        panic("non-table internal display in wrong flow");
    }

    bc.addToParentBox(std::move(tableInternalBox));
}

static void _buildTableBox(BuilderContext tableWrapperBc, Gc::Ref<Dom::Element> el, Rc<Style::SpecifiedValues> tableBoxStyle) {
    auto searchAndBuildCaption = [&]() {
        for (auto child = el->firstChild(); child; child = child->nextSibling()) {
            if (auto childEl = child->is<Dom::Element>()) {
                auto childStyle = childEl->specifiedValues();
                if (childStyle->display != Display::Internal::TABLE_CAPTION)
                    continue;
                _buildNode(tableWrapperBc, *childEl);
            }
        }
    };

    tableBoxStyle->display = Display::Internal::TABLE_BOX;
    Box tableBox = {tableBoxStyle, el};

    bool captionsOnTop = tableBoxStyle->table->captionSide == CaptionSide::TOP;
    if (captionsOnTop) {
        searchAndBuildCaption();
    }

    if (auto before = el->getPseudoElement(Dom::PseudoElement::BEFORE)) {
        _buildPseudoElement(tableWrapperBc, before.unwrap());
    }

    // An anonymous table-row box must be generated around each sequence of consecutive children of a table-root
    // box which are not proper table child boxes.
    _buildTableChildrenWhileWrappingIntoAnonymousBox(tableWrapperBc.toTableContent(tableBox), *el, tableBoxStyle, true, [](Display const& display) {
        return display.isProperTableChild();
    });
    tableWrapperBc.addToParentBox(std::move(tableBox));

    if (not captionsOnTop) {
        searchAndBuildCaption();
    }

    if (auto before = el->getPseudoElement(Dom::PseudoElement::AFTER)) {
        _buildPseudoElement(tableWrapperBc, before.unwrap());
    }
}

static Box _createTableWrapperAndBuildTable(BuilderContext bc, Rc<Style::SpecifiedValues> tableStyle, Gc::Ref<Dom::Element> tableBoxEl) {
    // The computed values of properties 'position', 'float', 'margin-*', 'top',
    // 'right', 'bottom', and 'left' on the table element are used on the table
    // wrapper box and not the table box; all other values of non-inheritable
    // properties are used on the table box and not the table wrapper box.
    // https://www.w3.org/TR/CSS22/tables.html#model

    auto const& initialStyle = Style::SpecifiedValues::initial();

    auto wrapperStyle = makeRc<Style::SpecifiedValues>(initialStyle);
    wrapperStyle->display = tableStyle->display;

    wrapperStyle->margin = tableStyle->margin;
    wrapperStyle->position = tableStyle->position;
    wrapperStyle->float_ = tableStyle->float_;
    wrapperStyle->offsets = tableStyle->offsets;

    Box wrapper = {wrapperStyle, tableBoxEl};
    InlineBox rootInlineBox{_proseStyleFromStyle(*wrapperStyle, tableBoxEl->specifiedValues()->fontFace)};

    // SPEC: The table wrapper box establishes a block formatting context.
    auto innerStyle = makeRc<Style::SpecifiedValues>(*tableStyle);

    innerStyle->margin = initialStyle.margin;
    innerStyle->position = initialStyle.position;
    innerStyle->float_ = initialStyle.float_;
    innerStyle->offsets = initialStyle.offsets;

    _buildTableBox(bc.toBlockContextWithoutRootInline(wrapper), tableBoxEl, innerStyle);

    return wrapper;
}

// MARK: Dispatch based on outside role -------------------------------------------------------------------------------

// https://www.w3.org/TR/css-display-3/#outer-role
static void _innerDisplayDispatchCreationOfBlockLevelBox(BuilderContext bc, Gc::Ref<Dom::Element> el, Rc<Style::SpecifiedValues> style, Display display) {
    if (display == Display::Inside::TABLE) {
        auto wrapper = _createTableWrapperAndBuildTable(bc, style, el);
        bc.addToParentBox(std::move(wrapper));
    } else {
        // NOTE: FLOW-ROOT, FLEX and fallback
        auto box = createAndBuildBoxFromElement(bc, style, el, display);
        bc.addToParentBox(std::move(box));
    }
}

// https://www.w3.org/TR/css-display-3/#outer-role
static void _innerDisplayDispatchCreationOfInlineLevelBox(BuilderContext bc, Gc::Ref<Dom::Element> el, Rc<Style::SpecifiedValues> style, Display display) {
    if (display == Display::Inside::TABLE) {
        auto wrapper = _createTableWrapperAndBuildTable(bc, style, el);
        bc.addToInlineRoot(std::move(wrapper));
    } else {
        // NOTE: FLOW, FLOW-ROOT, FLEX and fallback
        auto box = createAndBuildBoxFromElement(bc, style, el, display);
        bc.addToInlineRoot(std::move(box));
    }
}

// MARK: Dispatching from Node to builder based on outside role ------------------------------------------------------

static void _buildChildren(BuilderContext bc, Gc::Ref<Dom::Node> parent) {
    auto el = parent->is<Dom::Element>();
    if (auto before = el ? el->getPseudoElement(Dom::PseudoElement::BEFORE) : NONE) {
        _buildPseudoElement(bc, before.unwrap());
    }

    for (auto child = parent->firstChild(); child; child = child->nextSibling()) {
        _buildNode(bc, *child);
    }

    if (auto after = el ? el->getPseudoElement(Dom::PseudoElement::AFTER) : NONE) {
        _buildPseudoElement(bc, after.unwrap());
    }
}

static void _buildChildBoxDisplay(BuilderContext bc, Gc::Ref<Dom::Node> node, Display display) {
    if (display == Display::NONE)
        return;

    // Display::CONTENTS
    _buildChildren(bc, node);
}

// https://www.w3.org/TR/css-display-3/#layout-specific-display
static void _buildChildInternalDisplay(BuilderContext bc, Gc::Ref<Dom::Element> child, Rc<Style::SpecifiedValues> childStyle) {
    // FIXME: We should create wrapping boxes related to table or ruby, following the FC specification. However, for now,
    // we just wrap it in a single box.
    _innerDisplayDispatchCreationOfBlockLevelBox(bc, child, childStyle, childStyle->display);
}

static void _buildChildDefaultDisplay(BuilderContext bc, Gc::Ref<Dom::Element> child, Rc<Style::SpecifiedValues> childStyle, Display display) {
    if (bc.from == BuilderContext::From::FLEX) {
        display = childStyle->display = childStyle->display.blockify();
        _innerDisplayDispatchCreationOfBlockLevelBox(bc, child, childStyle, display);
        return;
    }

    // NOTE: Flow for From::BLOCK and From::INLINE
    if (display == Display::Outside::BLOCK) {
        bc.flushRootInlineBoxIntoAnonymousBox();
        _innerDisplayDispatchCreationOfBlockLevelBox(bc, child, childStyle, display);
    } else {
        // NOTE: <img> and <svg>, for example, should always be treated as inline-block instead of inline-flow (spans)
        if (_alwaysInlineBlock(child) or display != Display::Inside::FLOW)
            _innerDisplayDispatchCreationOfInlineLevelBox(bc, child, childStyle, display);
        else
            createAndBuildInlineFlowfromElement(bc, childStyle, child);
    }
}

// https://www.w3.org/TR/css-display-3/#box-generation
static void _buildNode(BuilderContext bc, Gc::Ref<Dom::Node> node) {
    logDebugIf(DEBUG_BUILDER, "building node {} at context {}", node, bc.from);
    if (auto el = node->is<Dom::Element>()) {
        auto childStyle = el->specifiedValues();
        auto display = childStyle->display;

        if (display.type() == Display::Type::BOX) {
            _buildChildBoxDisplay(bc, *el, display);
        } else if (display.type() == Display::Type::INTERNAL) {
            _buildChildInternalDisplay(bc, *el, childStyle);
        } else {
            _buildChildDefaultDisplay(bc, *el, childStyle, display);
        }
    } else if (auto text = node->is<Dom::Text>()) {
        _buildText(bc, text->data(), bc.parentStyle);
    }
}

export Box _buildBlockPseudoElement(Dom::PseudoElement& el) {
    auto proseStyle = _proseStyleFromStyle(*el.specifiedValues(), el.specifiedValues()->fontFace);

    if (el.specifiedValues()->content.is<String>()) {
        auto prose = makeRc<Gfx::Prose>(proseStyle);
        prose->append(el.specifiedValues()->content.unwrap<String>().str());
        return {el.specifiedValues(), InlineBox{prose}, nullptr};
    }

    return {el.specifiedValues(), nullptr};
}

static void _buildPseudoElement(BuilderContext bc, Rc<Dom::PseudoElement> pseudoElement) {
    auto style = pseudoElement->specifiedValues();
    auto display = style->display;
    if (display == Display::NONE)
        return;

    if (display == Display::INLINE or
        display == Display::CONTENTS) {
        bc.startInlineBox(_proseStyleFromStyle(*style, style->fontFace));
        if (auto maybeStr = style->content.is<String>())
            _buildText(bc, maybeStr->str(), style);
        bc.endInlineBox();
    } else {
        bc.flushRootInlineBoxIntoAnonymousBox();
        bc.addToParentBox(_buildBlockPseudoElement(*pseudoElement));
    }
}

// MARK: Entry points -----------------------------------------------------------------

static auto dumpBoxes = Debug::Flag::debug("web-boxes"s, "Dump the constructed boxes"s);

export Box build(Gc::Ref<Dom::Document> doc) {
    Box root{
        makeRc<Style::SpecifiedValues>(Style::SpecifiedValues::initial()),
        nullptr
    };

    if (auto el = doc->documentElement()) {
        root = {el->specifiedValues(), el};
        InlineBox rootInlineBox{_proseStyleFromStyle(*el->specifiedValues(), el->specifiedValues()->fontFace)};

        BuilderContext bc{
            BuilderContext::From::BLOCK,
            el->specifiedValues(),
            root,
            &rootInlineBox,
        };

        buildBlockFlowFromElement(bc, *el);
    }

    logDebugIf(dumpBoxes, "document boxes: {}", root);

    return root;
}

export Box buildElement(Gc::Ref<Dom::Element> elt) {
    Box box = {elt->specifiedValues(), elt};
    InlineBox rootInlineBox{_proseStyleFromStyle(*elt->specifiedValues(), elt->specifiedValues()->fontFace)};

    BuilderContext bc{
        BuilderContext::From::BLOCK,
        elt->specifiedValues(),
        box,
        &rootInlineBox,
    };

    buildBlockFlowFromElement(bc, *elt);

    return box;
}

export Box buildForPseudoElement(Dom::PseudoElement& el, usize currentPage, RunningPositionMap& runningPos) {
    auto proseStyle = _proseStyleFromStyle(*el.specifiedValues(), el.specifiedValues()->fontFace);

    if (el.specifiedValues()->content.is<String>()) {
        auto prose = makeRc<Gfx::Prose>(proseStyle);
        prose->append(el.specifiedValues()->content.unwrap<String>().str());
        return {el.specifiedValues(), InlineBox{prose}, nullptr};
    } else if (el.specifiedValues()->content.is<ElementContent>()) {
        auto elt = el.specifiedValues()->content.unwrap<ElementContent>();
        if (auto infos = runningPos.match(elt, currentPage)) {
            Box marginBox = {el.specifiedValues(), nullptr};

            Box box = buildElement(infos.unwrap().element);
            box.style->position = Keywords::STATIC;
            marginBox.add(std::move(box));
            return marginBox;
        }
    } else if (el.specifiedValues()->content.is<Counter>()) {
        auto elt = el.specifiedValues()->content.unwrap<Counter>();
        if (elt.type == Counter::Type::PAGE) {
            auto prose = makeRc<Gfx::Prose>(proseStyle);

            prose->append(Io::toStr(currentPage + 1).str());
            return {el.specifiedValues(), InlineBox{prose}, nullptr};
        }
    }

    return {el.specifiedValues(), nullptr};
}

} // namespace Vaev::Layout
