#include <karm-base/clamp.h>
#include <karm-text/loader.h>

#include "frag.h"

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

} // namespace Vaev::Layout
