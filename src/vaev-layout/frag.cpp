#include <karm-base/clamp.h>
#include <karm-text/loader.h>

#include "block.h"
#include "flex.h"
#include "frag.h"
#include "grid.h"
#include "table.h"
#include "values.h"

namespace Vaev::Layout {

// MARK: Frag ------------------------------------------------------------------

Frag::Frag(Strong<Style::Computed> style, Strong<Karm::Text::Fontface> font)
    : style{std::move(style)}, fontFace{font} {}

Frag::Frag(Strong<Style::Computed> style, Strong<Karm::Text::Fontface> font, Content content)
    : style{std::move(style)}, fontFace{font}, content{std::move(content)} {}

Karm::Slice<Frag> Frag::children() const {
    if (auto frags = content.is<Vec<Frag>>())
        return *frags;
    return {};
}

Karm::MutSlice<Frag> Frag::children() {
    if (auto frags = content.is<Vec<Frag>>()) {
        return *frags;
    }
    return {};
}

void Frag::add(Frag &&frag) {
    if (content.is<None>())
        content = Vec<Frag>{};

    if (auto frags = content.is<Vec<Frag>>()) {
        frags->pushBack(std::move(frag));
    }
}

void Frag::repr(Io::Emit &e) const {
    if (children()) {
        e("(flow {} {} {}", style->display, style->position, layout.borderBox());
        e.indentNewline();
        for (auto &c : children()) {
            c.repr(e);
            e.newline();
        }
        e.deindent();
        e(")");
    } else {
        e("(frag {} {} {})", style->display, style->position, layout.borderBox());
    }
}

// MARK: Build -----------------------------------------------------------------

static Opt<Strong<Karm::Text::Fontface>> _regularFontface = NONE;
static Opt<Strong<Karm::Text::Fontface>> _boldFontface = NONE;

static Strong<Karm::Text::Fontface> _lookupFontface(Style::Computed &style) {
    if (style.font->weight != FontWeight::NORMAL) {
        if (not _boldFontface)
            _boldFontface = Karm::Text::loadFontfaceOrFallback("bundle://fonts-inter/fonts/Inter-Bold.ttf"_url).unwrap();
        return *_boldFontface;
    } else {
        if (not _regularFontface)
            _regularFontface = Karm::Text::loadFontfaceOrFallback("bundle://fonts-inter/fonts/Inter-Regular.ttf"_url).unwrap();
        return *_regularFontface;
    }
}

void _buildChildren(Style::Computer &c, Vec<Strong<Markup::Node>> const &children, Frag &parent) {
    for (auto &child : children) {
        _buildNode(c, *child, parent);
    }
}

static void _buildTableChildren(Style::Computer &c, Vec<Strong<Markup::Node>> const &children, Frag &tableWrapperBox, Strong<Style::Computed> tableBoxStyle) {
    Frag tableBox{
        tableBoxStyle, tableWrapperBox.fontFace
    };

    tableBox.style->display = Display::Internal::TABLE_BOX;

    for (auto &child : children) {
        if (auto el = child->is<Markup::Element>()) {
            if (el->tagName == Html::CAPTION) {
                _buildNode(c, *child, tableWrapperBox);
            } else {
                _buildNode(c, *child, tableBox);
            }
        }
    }
    tableWrapperBox.add(std::move(tableBox));
}

static Opt<Math::Vec2u> _parseTableSpans(Markup::Element const &el) {
    auto rowSpan = el.getAttribute(AttrName{Html::AttrId::ROWSPAN});

    Opt<Str> colSpan = el.getAttribute(
        el.tagName == Html::COL or el.tagName == Html::COLGROUP
            ? AttrName{Html::AttrId::SPAN}
            : AttrName{Html::AttrId::COLSPAN}
    );

    if (rowSpan == NONE and colSpan == NONE)
        return NONE;

    usize rowSpanValue = 1, colSpanValue = 1;

    if (rowSpan)
        rowSpanValue = min(65534, Io::atoi(rowSpan.unwrap()).unwrapOr(1));

    if (colSpan)
        colSpanValue = min(1000, Io::atoi(colSpan.unwrap()).unwrapOr(1));

    return Math::Vec2u{
        rowSpanValue,
        colSpanValue,
    };
}

static void _buildElement(Style::Computer &c, Markup::Element const &el, Frag &parent) {
    auto style = c.computeFor(*parent.style, el);
    auto font = _lookupFontface(*style);
    auto tableSpan = _parseTableSpans(el);

    if (el.tagName == Html::IMG) {
        Image::Picture img = Gfx::Surface::fallback();
        parent.add({style, font, img});
        return;
    }

    auto display = style->display;

    if (display == Display::NONE)
        return;

    if (display == Display::CONTENTS) {
        _buildChildren(c, el.children(), parent);
        return;
    }

    auto buildFrag = [](Style::Computer &c, Markup::Element const &el, Strong<Karm::Text::Fontface> font, Strong<Style::Computed> style) {
        if (el.tagName == Html::TagId::TABLE) {

            auto wrapperStyle = makeStrong<Style::Computed>(Style::Computed::initial());
            wrapperStyle->display = style->display;
            wrapperStyle->margin = style->margin;

            Frag wrapper = {wrapperStyle, font};
            _buildTableChildren(c, el.children(), wrapper, style);
            return wrapper;
        } else {
            Frag frag = {style, font};
            _buildChildren(c, el.children(), frag);
            return frag;
        }
    };

    auto frag = buildFrag(c, el, font, style);

    if (tableSpan)
        frag.tableSpan.cow() = {
            tableSpan.unwrap().x,
            tableSpan.unwrap().y,
        };

    parent.add(std::move(frag));
}

static void _buildRun(Style::Computer &, Markup::Text const &node, Frag &parent) {
    auto style = makeStrong<Style::Computed>(Style::Computed::initial());
    style->inherit(*parent.style);

    auto font = _lookupFontface(*style);
    Io::SScan scan{node.data};
    scan.eat(Re::space());
    if (scan.ended())
        return;
    Karm::Text::Run run;

    while (not scan.ended()) {
        switch (style->text->transform) {
        case TextTransform::UPPERCASE:
            run.append(toAsciiUpper(scan.next()));
            break;

        case TextTransform::LOWERCASE:
            run.append(toAsciiLower(scan.next()));
            break;

        case TextTransform::NONE:
            run.append(scan.next());
            break;

        default:
            break;
        }

        if (scan.eat(Re::space())) {
            run.append(' ');
        }
    }

    parent.add({style, font, std::move(run)});
}

void _buildNode(Style::Computer &c, Markup::Node const &node, Frag &parent) {
    if (auto el = node.is<Markup::Element>()) {
        _buildElement(c, *el, parent);
    } else if (auto text = node.is<Markup::Text>()) {
        _buildRun(c, *text, parent);
    } else if (auto doc = node.is<Markup::Document>()) {
        _buildChildren(c, doc->children(), parent);
    }
}

Frag build(Style::Computer &c, Markup::Document const &doc) {
    auto style = makeStrong<Style::Computed>(Style::Computed::initial());
    Frag root = {style, _lookupFontface(*style)};
    _buildNode(c, doc, root);
    return root;
}

// MARK: Layout ----------------------------------------------------------------

Output _contentLayout(Tree &t, Frag &f, Input input) {
    auto display = f.style->display;

    if (auto run = f.content.is<Karm::Text::Run>()) {
        f.layout.fontSize = resolve(t, f, f.style->font->size);
        Karm::Text::Font font = {
            f.fontFace,
            f.layout.fontSize.cast<f64>(),
        };
        run->shape(font);
        return Output::fromSize(run->size().cast<Px>());
    } else if (
        display == Display::FLOW or
        display == Display::FLOW_ROOT or
        display == Display::TABLE_CELL or
        display == Display::TABLE_CAPTION
    ) {
        return blockLayout(t, f, input);
    } else if (display == Display::FLEX) {
        return flexLayout(t, f, input);
    } else if (display == Display::GRID) {
        return gridLayout(t, f, input);
    } else if (display == Display::TABLE) {
        return tableLayout(t, f, input);
    } else if (display == Display::INTERNAL) {
        return Output{};
    } else {
        return blockLayout(t, f, input);
    }
}

InsetsPx computeMargins(Tree &t, Frag &f, Input input) {
    InsetsPx res;
    auto margin = f.style->margin;

    res.top = resolve(t, f, margin->top, input.containingBlock.height);
    res.end = resolve(t, f, margin->end, input.containingBlock.width);
    res.bottom = resolve(t, f, margin->bottom, input.containingBlock.height);
    res.start = resolve(t, f, margin->start, input.containingBlock.width);

    return res;
}

InsetsPx computeBorders(Tree &t, Frag &f) {
    InsetsPx res;
    auto borders = f.style->borders;

    if (borders->top.style != Gfx::BorderStyle::NONE)
        res.top = resolve(t, f, borders->top.width);

    if (borders->end.style != Gfx::BorderStyle::NONE)
        res.end = resolve(t, f, borders->end.width);

    if (borders->bottom.style != Gfx::BorderStyle::NONE)
        res.bottom = resolve(t, f, borders->bottom.width);

    if (borders->start.style != Gfx::BorderStyle::NONE)
        res.start = resolve(t, f, borders->start.width);

    return res;
}

static InsetsPx _computePaddings(Tree &t, Frag &f, Input input) {
    InsetsPx res;
    auto padding = f.style->padding;

    res.top = resolve(t, f, padding->top, input.containingBlock.height);
    res.end = resolve(t, f, padding->end, input.containingBlock.width);
    res.bottom = resolve(t, f, padding->bottom, input.containingBlock.height);
    res.start = resolve(t, f, padding->start, input.containingBlock.width);

    return res;
}

static Math::Radii<Px> _computeRadii(Tree &t, Frag &f, Vec2Px size) {
    auto radii = f.style->borders->radii;
    Math::Radii<Px> res;

    res.a = resolve(t, f, radii.a, size.height);
    res.b = resolve(t, f, radii.b, size.width);
    res.c = resolve(t, f, radii.c, size.width);
    res.d = resolve(t, f, radii.d, size.height);
    res.e = resolve(t, f, radii.e, size.height);
    res.f = resolve(t, f, radii.f, size.width);
    res.g = resolve(t, f, radii.g, size.width);
    res.h = resolve(t, f, radii.h, size.height);

    return res;
}

static Cons<Opt<Px>, IntrinsicSize> _computeSpecifiedSize(Tree &t, Frag &f, Input input, Size size, IntrinsicSize intrinsic) {
    if (size == Size::MIN_CONTENT or intrinsic == IntrinsicSize::MIN_CONTENT) {
        return {NONE, IntrinsicSize::MIN_CONTENT};
    } else if (size == Size::MAX_CONTENT or intrinsic == IntrinsicSize::MAX_CONTENT) {
        return {NONE, IntrinsicSize::MAX_CONTENT};
    } else if (size == Size::AUTO) {
        return {NONE, IntrinsicSize::AUTO};
    } else if (size == Size::FIT_CONTENT) {
        return {NONE, IntrinsicSize::STRETCH_TO_FIT};
    } else if (size == Size::LENGTH) {
        return {resolve(t, f, size.value, input.containingBlock.width), IntrinsicSize::AUTO};
    } else {
        logWarn("unknown specified size: {}", size);
        return {Px{0}, IntrinsicSize::AUTO};
    }
}

Output layout(Tree &t, Frag &f, Input input) {
    // FIXME: confirm how the preffered width/height parameters interacts with intrinsic size argument from input
    auto borders = computeBorders(t, f);
    auto padding = _computePaddings(t, f, input);
    auto sizing = f.style->sizing;

    auto [specifiedWidth, widthIntrinsicSize] = _computeSpecifiedSize(t, f, input, sizing->width, input.intrinsic.x);
    if (input.knownSize.width == NONE) {
        // FIXME: making prefered width as mandatory width; im not sure this is ok
        input.knownSize.width = specifiedWidth;
    }
    input.knownSize.width = input.knownSize.width.map([&](auto s) {
        // FIXME: Take box-sizing into account
        return s - padding.horizontal() - borders.horizontal();
    });
    input.intrinsic.x = widthIntrinsicSize;

    auto [specifiedHeight, heightIntrinsicSize] = _computeSpecifiedSize(t, f, input, sizing->height, input.intrinsic.y);
    if (input.knownSize.height == NONE) {
        input.knownSize.height = specifiedHeight;
    }

    input.knownSize.height = input.knownSize.height.map([&](auto s) {
        // FIXME: Take box-sizing into account
        return s - padding.vertical() - borders.vertical();
    });
    input.intrinsic.y = heightIntrinsicSize;

    input.position = input.position + borders.topStart() + padding.topStart();

    auto [size] = _contentLayout(t, f, input);

    size.width = input.knownSize.width.unwrapOr(size.width);
    size.height = input.knownSize.height.unwrapOr(size.height);

    size = size + padding.all() + borders.all();

    if (input.commit == Commit::YES) {
        f.layout.position = input.position - borders.topStart() - padding.topStart();
        f.layout.borderSize = size;
        f.layout.padding = padding;
        f.layout.borders = borders;
        f.layout.radii = _computeRadii(t, f, size);
    }

    return Output::fromSize(size);
}

void wireframe(Frag &frag, Gfx::Canvas &g) {
    for (auto &c : frag.children())
        wireframe(c, g);

    g.strokeStyle({
        .fill = Gfx::BLACK,
        .width = 1,
        .align = Gfx::INSIDE_ALIGN,
    });

    g.stroke(frag.layout.borderBox().cast<f64>());
}

} // namespace Vaev::Layout
