#pragma once

#include <karm-text/book.h>
#include <vaev-dom/document.h>
#include <vaev-dom/element.h>

#include "specified.h"
#include "stylesheet.h"

namespace Vaev::Style {

struct Computer {
    Media _media;
    StyleSheetList const& _styleBook;
    Text::FontBook& fontBook;

    using MatchingRules = Vec<Tuple<Cursor<StyleRule>, Spec>>;

    // MARK: Cascading ---------------------------------------------------------

    void _evalRule(Rule const& rule, Gc::Ref<Dom::Element> el, MatchingRules& matches);

    void _evalRule(Rule const& rule, Page const& page, PageComputedStyle& c);

    void _evalRule(Rule const& rule, Vec<FontFace>& fontFaces);

    Rc<SpecifiedValues> _evalCascade(SpecifiedValues const& parent, MatchingRules& matches);

    // MARK: Computing ---------------------------------------------------------

    Rc<SpecifiedValues> computeFor(SpecifiedValues const& parent, Gc::Ref<Dom::Element> el);

    Rc<PageComputedStyle> computeFor(SpecifiedValues const& parent, Page const& page);

    // MARK: Styling -----------------------------------------------------------

    void styleElement(SpecifiedValues const& parentSpecifiedValues, ComputedValues const& parentComputedValues, Dom::Element& el);

    void styleDocument(Dom::Document& doc);

    void loadFontFaces();
};

} // namespace Vaev::Style
