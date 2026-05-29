export module Vaev.Engine:style.computer;

import Karm.Gc;
import Karm.Gfx;
import Karm.Font;
import Karm.Ref;
import Karm.Math;
import Karm.Logger;

import :dom.document;
import :dom.element;
import :style.computed;
import :style.cascaded;
import :style.stylesheet;
import :style.ruleIndex;

namespace Vaev::Style {

export struct Computer {
    Gc::Heap& _heap;
    Media _media;
    RegisteredPropertySet& _registeredPropertySet;
    StyleSheetList const& _stylesheets;
    Rc<Font::Database> _fontDatabase;
    RuleIndex _ruleIndex = {};

    // MARK: Cascading ---------------------------------------------------------

    void _evalRule(Rule const& rule, Page const& page, PageComputedValues& c) {
        rule.visit(
            [&](PageRule const& r) {
                if (r.match(page))
                    r.apply(_registeredPropertySet, c);
            },
            [&](MediaRule const& r) {
                if (r.match(_media))
                    for (auto const& subRule : r.rules)
                        _evalRule(subRule, page, c);
            },
            [&](auto const&) {
                // Ignore other rule types
            }
        );
    }

    // MARK: Computing ---------------------------------------------------------

    Rc<Gfx::Fontface> _lookupFontface(ComputedValues& style) {
        Font::Query fq{
            .weight = style.font->weight,
            .stretch = Gfx::FontStretch{static_cast<u16>(Math::roundi(style.font->width.val().value() * 10.0))},
            .style = style.font->style.val,
        };

        for (auto family : style.font->families) {
            if (auto const& [font] = _fontDatabase->queryClosest(family.name, fq))
                return font;
        }

        if (auto const& [font] = _fontDatabase->queryClosest("system"_sym))
            return font;

        return Gfx::Fontface::fallback();
    }

    // https://www.w3.org/TR/css-cascade-4/#author-presentational-hint-origin
    void _considerHtmlPresentationalHint(Gc::Ref<Dom::Element> el, CascadedValues& cascadedValues) {
        if (el->namespaceUri() != Html::NAMESPACE)
            return;

        // https://html.spec.whatwg.org/multipage/obsolete.html#dom-document-fgcolor
        if (auto const& [fgcolor] = el->getAttribute(Html::FGCOLOR_ATTR)) {
            if (auto property = _registeredPropertySet.parseValue(
                    Properties::COLOR, fgcolor, {}
                ))
                cascadedValues.put(property.take(), Origin::AUTHOR_PRESENTATIONAL_HINT, PRESENTATION_HINT_SPEC);
        }

        // https://html.spec.whatwg.org/multipage/obsolete.html#dom-document-bgcolor
        if (auto const& [bgcolor] = el->getAttribute(Html::BGCOLOR_ATTR)) {
            if (auto property = _registeredPropertySet.parseValue(
                    Properties::BACKGROUND_COLOR, bgcolor, {}
                ))
                cascadedValues.put(property.take(), Origin::AUTHOR_PRESENTATIONAL_HINT, PRESENTATION_HINT_SPEC);
        }

        // https://html.spec.whatwg.org/multipage/images.html#sizes-attributes
        if (auto const& [width] = el->getAttribute(Html::WIDTH_ATTR)) {
            if (auto property = _registeredPropertySet.parseValue(
                    Properties::WIDTH, width, {}
                ))
                cascadedValues.put(property.take(), Origin::AUTHOR_PRESENTATIONAL_HINT, PRESENTATION_HINT_SPEC);
        }

        // https://html.spec.whatwg.org/multipage/images.html#sizes-attributes
        if (auto const& [height] = el->getAttribute(Html::HEIGHT_ATTR)) {
            if (auto property = _registeredPropertySet.parseValue(
                    Properties::HEIGHT, height, {}
                ))
                cascadedValues.put(property.take(), Origin::AUTHOR_PRESENTATIONAL_HINT, PRESENTATION_HINT_SPEC);
        }

        // https://html.spec.whatwg.org/multipage/input.html#the-size-attribute
        if (auto const& [size] = el->getAttribute(Html::SIZE_ATTR)) {
            if (auto property = _registeredPropertySet.parseValue(
                    Properties::WIDTH, Io::format("{}ch", size), {}
                ))
                cascadedValues.put(property.take(), Origin::AUTHOR_PRESENTATIONAL_HINT, PRESENTATION_HINT_SPEC);
        }
    }

    void _considerInlineStyleAttribute(Gc::Ref<Dom::Element> el, CascadedValues& cascadedValues) {
        auto styleAttr = el->style();

        auto declarations = _registeredPropertySet.parseDeclarations(
            styleAttr ? *styleAttr : "",
            RegisteredPropertySet::TOP_LEVEL
        );
        for (auto& decl : declarations)
            cascadedValues.put(decl, Origin::INLINE, INLINE_SPEC);
    }

    static void _considerElementAttributes(ComputedValues& values, Gc::Ref<Dom::Element> el) {
        // https://html.spec.whatwg.org/multipage/tables.html#the-col-element
        // The element may have a span content attribute specified, whose value must
        // be a valid non-negative integer greater than zero and less than or equal to 1000.
        if (auto const& [span] = el->getAttribute(Html::SPAN_ATTR)) {
            auto value = parseValue<Integer>(span).unwrapOr(0);
            if (value <= 0 or value > 1000)
                value = 1;
            values.table.cow().span = value;
        }

        // https://html.spec.whatwg.org/multipage/tables.html#attributes-common-to-td-and-th-elements
        // The td and th elements may have a colspan content attribute specified,
        // whose value must be a valid non-negative integer greater than zero and less than or equal to 1000.
        if (auto const& [colSpan] = el->getAttribute(Html::COLSPAN_ATTR)) {
            auto value = parseValue<Integer>(colSpan).unwrapOr(0);
            if (value <= 0 or value > 1000)
                value = 1;
            values.table.cow().colSpan = value;
        }

        // The td and th elements may also have a rowspan content attribute specified,
        // whose value must be a valid non-negative integer less than or equal to 65534.
        if (auto const& [rowSpan] = el->getAttribute(Html::ROWSPAN_ATTR)) {
            auto value = parseValue<Integer>(rowSpan).unwrapOr(0);
            if (value < 0)
                value = 0;
            if (value > 65534)
                value = 65534;
            values.table.cow().rowSpan = value;
        }
    }

    // https://svgwg.org/specs/integration/#svg-css-sizing
    void _applySvgElementSizingRules(Gc::Ref<Dom::Element> svgEl, CascadedValues& cascadedValues) {
        if (auto parentEl = svgEl->parentNode()->is<Dom::Element>()) {
            // **If we have an <svg> element inside a CSS context**
            if (parentEl->qualifiedName.ns == Svg::NAMESPACE)
                return;

            // To resolve 'auto' value on ‘svg’ element if the ‘viewBox’ attribute is not specified:
            // - ...
            // - If any of the sizing attributes are missing, resolve the missing ‘svg’ element width to '300px' and missing
            // height to '150px' (using CSS 2.1 replaced elements size calculation).
            if (svgEl->hasAttribute(Svg::VIEW_BOX_ATTR))
                return;

            if (not svgEl->hasAttribute(Svg::WIDTH_ATTR))
                cascadedValues.put(
                    _registeredPropertySet.parseValue(Properties::WIDTH, "300px", {}).unwrap(),
                    Origin::AUTHOR_PRESENTATIONAL_HINT, PRESENTATION_HINT_SPEC
                );

            if (not svgEl->hasAttribute(Svg::HEIGHT_ATTR))
                cascadedValues.put(
                    _registeredPropertySet.parseValue(Properties::HEIGHT, "150px", {}).unwrap(),
                    Origin::AUTHOR_PRESENTATIONAL_HINT, PRESENTATION_HINT_SPEC
                );
        }
    }

    // https://svgwg.org/svg2-draft/styling.html#PresentationAttributes
    void _considerSvgPresentationAttributes(Gc::Ref<Dom::Element> el, CascadedValues& cascadedValues) {
        // Presentation attributes contribute to the author level of the cascade, followed by all other author-level
        // style sheets, and have specificity 0.

        if (el->qualifiedName.ns != Svg::NAMESPACE)
            return;

        for (auto [attr, attrValue] : el->attributes.iterItems())
            if (auto const& [property] = _registeredPropertySet.parsePresentationAttribute(attr.name, attrValue->value))
                cascadedValues.put(property, Origin::AUTHOR_PRESENTATIONAL_HINT, PRESENTATION_HINT_SPEC);

        if (el->qualifiedName == Svg::SVG_TAG)
            _applySvgElementSizingRules(el, cascadedValues);
    }

    // https://drafts.csswg.org/css-cascade/#cascade-origin
    Rc<ComputedValues> computeFor(ComputedValues const& parent, Gc::Ref<Dom::Element> el) {
        auto values = _registeredPropertySet.inheritsComputedValues(parent);

        MatchingRules const matchingRules = _ruleIndex.match(el, NONE);
        CascadedValues cascadedValues;
        for (auto const& [styleRule, specificity] : matchingRules)
            for (auto& prop : styleRule->props)
                cascadedValues.put(prop, styleRule->origin, specificity);

        _considerHtmlPresentationalHint(el, cascadedValues);
        _considerInlineStyleAttribute(el, cascadedValues);
        _considerSvgPresentationAttributes(el, cascadedValues);

        cascadedValues.apply(Property::ComputationPhase::CUSTOM_PROPERTY, parent, *values);
        cascadedValues.expandShorthands(parent, *values, _registeredPropertySet);

        cascadedValues.apply(Property::ComputationPhase::PRE_FONT, parent, *values);
        cascadedValues.apply(Property::ComputationPhase::FONT, parent, *values);

        // FIXME: Use a font-dirty flag instead.
        if (not parent.font.sameInstance(values->font) and
            (parent.font->families != values->font->families or
             parent.font->weight != values->font->weight or
             parent.font->style != values->font->style or
             parent.font->width != values->font->width)) {
            auto font = _lookupFontface(*values);
            values->fontFace = font;
        } else {
            values->fontFace = parent.fontFace;
        }

        cascadedValues.apply(Property::ComputationPhase::NORMAL, parent, *values);
        cascadedValues.apply(Property::ComputationPhase::LATE, parent, *values);

        _considerElementAttributes(*values, el);

        return values;
    }

    Rc<ComputedValues> computeFor(ComputedValues const& parent, Gc::Ref<Dom::Element> el, Symbol pseudoElement) {
        auto values = _registeredPropertySet.inheritsComputedValues(parent);

        MatchingRules matchingRules = _ruleIndex.match(el, pseudoElement);

        CascadedValues cascadedValues;
        for (auto const& [styleRule, specificity] : matchingRules)
            for (auto& prop : styleRule->props)
                cascadedValues.put(prop, styleRule->origin, specificity);

        cascadedValues.apply(Property::ComputationPhase::CUSTOM_PROPERTY, parent, *values);
        cascadedValues.expandShorthands(parent, *values, _registeredPropertySet);

        cascadedValues.apply(Property::ComputationPhase::PRE_FONT, parent, *values);
        cascadedValues.apply(Property::ComputationPhase::FONT, parent, *values);

        // FIXME: Use a font-dirty flag instead.
        if (not parent.font.sameInstance(values->font) and
            (parent.font->families != values->font->families or
             parent.font->weight != values->font->weight or
             parent.font->style != values->font->style or
             parent.font->width != values->font->width)) {
            auto font = _lookupFontface(*values);
            values->fontFace = font;
        } else {
            values->fontFace = parent.fontFace;
        }

        cascadedValues.apply(Property::ComputationPhase::NORMAL, parent, *values);
        cascadedValues.apply(Property::ComputationPhase::LATE, parent, *values);

        return values;
    }

    Rc<PageComputedValues> computeFor(ComputedValues const& parent, Page const& page) {
        auto computed = makeRc<PageComputedValues>(_heap, parent);

        for (auto const& sheet : _stylesheets.styleSheets)
            for (auto const& rule : sheet.rules)
                _evalRule(rule, page, *computed);

        for (auto& area : computed->_areas) {
            auto font = _lookupFontface(*area->computedValues());
            area->computedValues()->fontFace = font;
        }

        return computed;
    }

    // MARK: Styling -----------------------------------------------------------

    void generatePseudoElement(ComputedValues const& parentComputedValues, Dom::Element& el, Symbol type) {
        auto computedValues = computeFor(parentComputedValues, el, type);

        // https://drafts.csswg.org/css-content/#valdef-content-none
        // On pseudo-elements it inhibits the creation of the pseudo-element as if it had display: none.
        if (computedValues->content == Keywords::NONE)
            return;

        // https://drafts.csswg.org/css-content/#valdef-content-normal
        if (computedValues->content == Keywords::NORMAL and
            (type == Dom::PseudoElement::BEFORE or
             type == Dom::PseudoElement::AFTER))
            return;

        el.addPseudoElement(_heap.alloc<Dom::PseudoElement>(type, computedValues));
    }

    void styleElement(ComputedValues const& parentComputedValues, Dom::Element& el) {
        auto computedValues = computeFor(parentComputedValues, el);
        el._computedValues = computedValues;

        if (computedValues->display == Display::Item::YES) {
            generatePseudoElement(*computedValues, el, Dom::PseudoElement::MARKER);
        }

        generatePseudoElement(*computedValues, el, Dom::PseudoElement::AFTER);
        generatePseudoElement(*computedValues, el, Dom::PseudoElement::BEFORE);

        for (auto child = el.firstChild(); child; child = child->nextSibling()) {
            if (auto childEl = child->is<Dom::Element>())
                styleElement(*computedValues, *childEl);
        }
    }

    // MARK: Body Brackground --------------------------------------------------------

    // https://www.w3.org/TR/css-backgrounds-3/#body-background
    static void _propagateBodyBackgroundToHtml(Dom::Document& doc) {
        // For documents whose root element is an HTML HTML element or an XHTML html element
        auto html = doc.documentElement();
        if (html->namespaceUri() != Html::NAMESPACE)
            return;
        auto htmlBg = html->computedValues()->backgrounds;

        auto body = doc.body();
        if (body == nullptr)
            return;
        auto bodyBg = body->computedValues()->backgrounds;

        // If the computed value of background-image on the
        // root element is none and its background-color is transparent
        if (htmlBg->color == TRANSPARENT and not htmlBg->layers) {
            // User agents must instead propagate the computed values of the
            // background properties from that element’s first HTML BODY
            // or XHTML body child element.
            html->computedValues()->backgrounds = bodyBg;

            // The used values of that BODY element’s background properties are
            // their initial values, and the propagated values are treated
            // as if they were specified on the root element.
            body->computedValues()->backgrounds = makeCow<BackgroundProps>();
        }
    }

    void styleDocument(Dom::Document& doc) {
        if (auto el = doc.documentElement()) {
            auto rootComputedValues = _registeredPropertySet.initialComputedValues();
            rootComputedValues->fontFace = _lookupFontface(*rootComputedValues);
            styleElement(*rootComputedValues, *el);
        }
        _propagateBodyBackgroundToHtml(doc);
    }

    void build() {
        for (auto const& sheet : _stylesheets.styleSheets) {
            for (auto const& rule : sheet.rules) {
                _addRuleToLookup(&rule);
            }
        }
    }

    void _addRuleToLookup(Cursor<Rule> rule) {
        rule->visit(
            [&](StyleRule const& r) {
                _ruleIndex.add(r);
            },
            [&](MediaRule const& r) {
                if (r.match(_media))
                    for (auto const& subRule : r.rules)
                        _addRuleToLookup(&subRule);
            },
            [&](auto const&) {
                // Ignore other rule types
            }
        );
    }
};

} // namespace Vaev::Style
