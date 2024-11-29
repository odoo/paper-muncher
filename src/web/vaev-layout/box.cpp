#include <karm-base/clamp.h>
#include <karm-text/loader.h>

#include "box.h"

namespace Vaev::Layout {

// MARK: Inline ----------------------------------------------------------------

Inline::Inline(Text::ProseStyle style)
    : prose(makeStrong<Text::Prose>(style)) {}

void Inline::append(Box &&box) {
    prose->appendBox();
    boxes.pushBack(std::move(box));
}

void Inline::append(Rune rune) {
    prose->append(rune);
}

void Inline::append(Str text) {
    prose->append(text);
}

// MARK: Box -------------------------------------------------------------------

Box::Box(Strong<Style::Computed> style, Strong<Text::Fontface> font)
    : style{std::move(style)}, fontFace{font} {}

Box::Box(Strong<Style::Computed> style, Strong<Text::Fontface> font, Content content)
    : style{std::move(style)}, fontFace{font}, content{std::move(content)} {}

Slice<Box> Box::children() const {
    if (auto children = content.is<Vec<Box>>())
        return *children;
    return {};
}

MutSlice<Box> Box::children() {
    if (auto children = content.is<Vec<Box>>()) {
        return *children;
    }

    if (auto inline_ = content.is<Inline>()) {
        return inline_->boxes;
    }

    return {};
}

void Box::append(Box &&box) {
    if (content.is<None>())
        content = Vec<Box>{};

    if (auto inline_ = content.is<Inline>()) {
        inline_->append(std::move(box));
    } else if (auto children = content.is<Vec<Box>>()) {
        children->pushBack(std::move(box));
    }
}

void Box::append(Rune rune) {
    if (auto inline_ = content.is<Inline>()) {
        inline_->append(rune);
    }
}

void Box::append(Str text) {
    if (auto inline_ = content.is<Inline>()) {
        inline_->append(text);
    }
}

void Box::repr(Io::Emit &e) const {
    if (children()) {
        e("(box {} {} {} {}", attrs, style->display, style->position, layout.borderBox());
        e.indentNewline();
        for (auto &c : children()) {
            c.repr(e);
            e.newline();
        }
        e.deindent();
        e(")");
    } else {
        e("(box {} {} {} {})", attrs, style->display, style->position, layout.borderBox());
    }
}

} // namespace Vaev::Layout
