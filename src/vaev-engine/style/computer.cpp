export module Vaev.Engine:style.computer;

import Karm.Gc;
import Karm.Gfx;
import Karm.Font;
import Karm.Ref;
import Karm.Math;
import Karm.Logger;

import :dom.document;
import :dom.element;
import :style.specified;
import :style.stylesheet;
import :style.ruleIndex;

namespace Vaev::Style {

export struct Computer {
    Media _media;
    StyleSheetList const& _stylesheets;
    Rc<Font::Database> _fontDatabase;
    RuleIndex _ruleIndex = {};

    // MARK: Cascading ---------------------------------------------------------

    void _evalRule(Rule const& rule, Page const& page, PageSpecifiedValues& c) {
        rule.visit(Visitor{
            [&](PageRule const& r) {
                if (r.match(page))
                    r.apply(c);
            },
            [&](MediaRule const& r) {
                if (r.match(_media))
                    for (auto const& subRule : r.rules)
                        _evalRule(subRule, page, c);
            },
            [&](auto const&) {
                // Ignore other rule types
            },
        });
    }

    Rc<SpecifiedValues> _evalCascade(SpecifiedValues const& parent, MatchingRules& matchingRules) {
        // Sort origin and specificity
        stableSort(
            matchingRules,
            [](auto const& a, auto const& b) {
                if (a.v0->origin != b.v0->origin)
                    return a.v0->origin <=> b.v0->origin;
                return a.v1 <=> b.v1;
            }
        );

        // Compute computed style
        auto computed = makeRc<SpecifiedValues>(SpecifiedValues::initial());
        computed->inherit(parent);
        Vec<Cursor<StyleProp>> importantProps;

        // HACK: Apply custom properties first
        for (auto const& [styleRule, _] : matchingRules) {
            for (auto& prop : styleRule->props) {
                if (prop.is<CustomProp>())
                    prop.apply(parent, *computed);
            }
        }

        for (auto const& [styleRule, _] : matchingRules) {
            for (auto& prop : styleRule->props) {
                if (not prop.is<CustomProp>()) {
                    if (prop.important == Important::NO)
                        prop.apply(parent, *computed);
                    else
                        importantProps.pushBack(&prop);
                }
            }
        }

        for (auto const& prop : importantProps)
            prop->apply(parent, *computed);

        return computed;
    }

    // MARK: Computing ---------------------------------------------------------

    Rc<Gfx::Fontface> _lookupFontface(SpecifiedValues& style) {
        Font::Query fq{
            .weight = style.font->weight,
            .style = style.font->style.val,
        };

        for (auto family : style.font->families) {
            if (auto font = _fontDatabase->queryClosest(family.name, fq))
                return font.unwrap();
        }

        if (auto font = _fontDatabase->queryClosest("system"_sym))
            return font.unwrap();

        return Gfx::Fontface::fallback();
    }

    // https://www.w3.org/TR/css-cascade-4/#author-presentational-hint-origin
    static Vec<StyleProp> _considerPresentationalHint(Gc::Ref<Dom::Element> el) {
        if (el->namespaceUri() != Html::NAMESPACE)
            return {};

        Vec<StyleProp> res;
        // https://html.spec.whatwg.org/multipage/obsolete.html#dom-document-fgcolor
        if (auto fgcolor = el->getAttribute(Html::FGCOLOR_ATTR)) {
            auto value = parseValue<Color>(fgcolor.unwrap());
            if (value)
                res.pushBack(ColorProp{value.take()});
        }

        // https://html.spec.whatwg.org/multipage/obsolete.html#dom-document-bgcolor
        if (auto bgcolor = el->getAttribute(Html::BGCOLOR_ATTR)) {
            auto value = parseValue<Color>(bgcolor.unwrap());
            if (value)
                res.pushBack(BackgroundColorProp{value.take()});
        }

        // https://html.spec.whatwg.org/multipage/images.html#sizes-attributes
        if (auto width = el->getAttribute(Html::WIDTH_ATTR)) {
            auto value = parseValue<Size>(width.unwrap());
            if (value)
                res.pushBack(WidthProp{value.take()});
        }

        // https://html.spec.whatwg.org/multipage/images.html#sizes-attributes
        if (auto height = el->getAttribute(Html::HEIGHT_ATTR)) {
            auto value = parseValue<Size>(height.unwrap());
            if (value)
                res.pushBack(HeightProp{value.take()});
        }

        // https://html.spec.whatwg.org/multipage/input.html#the-size-attribute
        if (auto size = el->getAttribute(Html::SIZE_ATTR)) {
            auto value = parseValue<Integer>(size.unwrap());
            if (value)
                res.pushBack(WidthProp{CalcValue<PercentOr<Length>>{Length{static_cast<f64>(value.take()), Length::CH}}});
        }

        return res;
    }

    // https://www.w3.org/TR/css-backgrounds-3/#body-background
    static void _propagateBodyBackgroundToHtml(Dom::Document& doc) {
        // For documents whose root element is an HTML HTML element or an XHTML html element
        auto html = doc.documentElement();
        if (html->namespaceUri() != Html::NAMESPACE)
            return;
        auto htmlBg = html->specifiedValues()->backgrounds;

        auto body = doc.body();
        if (body == nullptr)
            return;
        auto bodyBg = body->specifiedValues()->backgrounds;

        // If the computed value of background-image on the
        // root element is none and its background-color is transparent
        if (htmlBg->color == TRANSPARENT and not htmlBg->layers) {
            // User agents must instead propagate the computed values of the
            // background properties from that element’s first HTML BODY
            // or XHTML body child element.
            html->specifiedValues()->backgrounds = bodyBg;

            // The used values of that BODY element’s background properties are
            // their initial values, and the propagated values are treated
            // as if they were specified on the root element.
            body->specifiedValues()->backgrounds = makeCow<BackgroundProps>();
        }
    }

    static void _considerElementAttributes(SpecifiedValues& values, Gc::Ref<Dom::Element> el) {
        // https://html.spec.whatwg.org/multipage/tables.html#the-col-element
        // The element may have a span content attribute specified, whose value must
        // be a valid non-negative integer greater than zero and less than or equal to 1000.
        if (auto span = el->getAttribute(Html::SPAN_ATTR)) {
            auto value = parseValue<Integer>(span.unwrap()).unwrapOr(0);
            if (value <= 0 or value > 1000)
                value = 1;
            values.table.cow().span = value;
        }

        // https://html.spec.whatwg.org/multipage/tables.html#attributes-common-to-td-and-th-elements
        // The td and th elements may have a colspan content attribute specified,
        // whose value must be a valid non-negative integer greater than zero and less than or equal to 1000.
        if (auto colSpan = el->getAttribute(Html::COLSPAN_ATTR)) {
            auto value = parseValue<Integer>(colSpan.unwrap()).unwrapOr(0);
            if (value <= 0 or value > 1000)
                value = 1;
            values.table.cow().colSpan = value;
        }

        // The td and th elements may also have a rowspan content attribute specified,
        // whose value must be a valid non-negative integer less than or equal to 65534.
        if (auto rowSpan = el->getAttribute(Html::ROWSPAN_ATTR)) {
            auto value = parseValue<Integer>(rowSpan.unwrap()).unwrapOr(0);
            if (value < 0)
                value = 0;
            if (value > 65534)
                value = 65534;
            values.table.cow().rowSpan = value;
        }
    }

    // https://svgwg.org/specs/integration/#svg-css-sizing
    static void _applySVGElementSizingRules(Gc::Ref<Dom::Element> svgEl, Vec<StyleProp>& styleProps) {
        if (auto parentEl = svgEl->parentNode()->is<Dom::Element>()) {
            // **If we have an <svg> element inside a CSS context**
            if (parentEl->qualifiedName.ns == Svg::NAMESPACE)
                return;

            // To resolve 'auto' value on ‘svg’ element if the ‘viewBox’ attribute is not specified:
            // - ...
            // - If any of the sizing attributes are missing, resolve the missing ‘svg’ element width to '300px' and missing
            // height to '150px' (using CSS 2.1 replaced elements size calculation).
            if (not svgEl->hasAttribute(Svg::VIEW_BOX_ATTR)) {
                if (not svgEl->hasAttribute(Svg::WIDTH_ATTR)) {
                    styleProps.pushBack(WidthProp{CalcValue<PercentOr<Length>>{PercentOr<Length>{Length{Au{300}}}}});
                }
                if (not svgEl->hasAttribute(Svg::HEIGHT_ATTR)) {
                    styleProps.pushBack(HeightProp{CalcValue<PercentOr<Length>>{PercentOr<Length>{Length{Au{150}}}}});
                }
            }
        }
    }

    // https://svgwg.org/svg2-draft/styling.html#PresentationAttributes
    Vec<StyleProp> _considerPresentationAttributes(Gc::Ref<Dom::Element> el) {
        if (el->qualifiedName.ns != Svg::NAMESPACE)
            return {};

        Vec<StyleProp> styleProps;
        for (auto [attr, attrValue] : el->attributes.iterUnordered()) {
            parseSVGPresentationAttribute(attr.name, attrValue->value, styleProps);
        }

        if (el->qualifiedName == Svg::SVG_TAG)
            _applySVGElementSizingRules(el, styleProps);

        return styleProps;
    }

    // https://drafts.csswg.org/css-cascade/#cascade-origin
    Rc<SpecifiedValues> computeFor(SpecifiedValues const& parent, Gc::Ref<Dom::Element> el) {
        MatchingRules matchingRules = _ruleIndex.match(el, NONE);

        // Non-CSS Presentational Hints
        auto hints = _considerPresentationalHint(el);
        StyleRule presentationHints{
            .props = std::move(hints),
            .origin = Origin::AUTHOR,
        };
        matchingRules.pushBack({&presentationHints, PRESENTATION_HINT_SPEC});

        // Get the style attribute if any
        auto styleAttr = el->style();
        StyleRule styleRule{
            .props = parseDeclarations<StyleProp>(styleAttr ? *styleAttr : ""),
            .origin = Origin::INLINE,
        };
        matchingRules.pushBack({&styleRule, INLINE_SPEC});

        // https://svgwg.org/svg2-draft/styling.html#PresentationAttributes
        // Presentation attributes contribute to the author level of the cascade, followed by all other author-level
        // style sheets, and have specificity 0.
        StyleRule presentationAttributes{
            .props = _considerPresentationAttributes(el),
            .origin = Origin::AUTHOR,
        };
        matchingRules.pushBack({&presentationAttributes, PRESENTATION_ATTR_SPEC});

        auto values = _evalCascade(parent, matchingRules);

        _considerElementAttributes(*values, el);

        return values;
    }

    Rc<SpecifiedValues> computeFor(SpecifiedValues const& parent, Gc::Ref<Dom::Element> el, Symbol pseudoElement) {
        MatchingRules matchingRules = _ruleIndex.match(el, pseudoElement);
        return _evalCascade(parent, matchingRules);
    }

    Rc<PageSpecifiedValues> computeFor(SpecifiedValues const& parent, Page const& page) {
        auto computed = makeRc<PageSpecifiedValues>(parent);

        for (auto const& sheet : _stylesheets.styleSheets)
            for (auto const& rule : sheet.rules)
                _evalRule(rule, page, *computed);

        for (auto& area : computed->_areas) {
            auto font = _lookupFontface(*area.specifiedValues());
            area.specifiedValues()->fontFace = font;
        }

        return computed;
    }

    // MARK: Styling -----------------------------------------------------------

    void generatePseudoElement(SpecifiedValues const& parentSpecifiedValues, Dom::Element& el, Symbol type) {
        auto specifiedValues = computeFor(parentSpecifiedValues, el, type);

        // HACK: This is basically nonsense to avoid doing too much font lookup,
        //       and it should be remove once the style engine get refactored
        //       and computed values are properly handled.
        if (not parentSpecifiedValues.font.sameInstance(specifiedValues->font) and
            (parentSpecifiedValues.font->families != specifiedValues->font->families or
             parentSpecifiedValues.font->weight != specifiedValues->font->weight)) {
            auto font = _lookupFontface(*specifiedValues);
            specifiedValues->fontFace = font;
        } else {
            specifiedValues->fontFace = parentSpecifiedValues.fontFace;
        }

        // https://drafts.csswg.org/css-content/#valdef-content-none
        // On pseudo-elements it inhibits the creation of the pseudo-element as if it had display: none.
        if (specifiedValues->content == Keywords::NONE)
            return;

        // https://drafts.csswg.org/css-content/#valdef-content-normal
        if (specifiedValues->content == Keywords::NORMAL and
            (type == Dom::PseudoElement::BEFORE or
             type == Dom::PseudoElement::AFTER))
            return;

        el.addPseudoElement(makeRc<Dom::PseudoElement>(type, specifiedValues));
    }

    void styleElement(SpecifiedValues const& parentSpecifiedValues, Dom::Element& el) {
        auto specifiedValues = computeFor(parentSpecifiedValues, el);
        el._specifiedValues = specifiedValues;

        // HACK: This is basically nonsense to avoid doing too much font lookup,
        //       and it should be remove once the style engine get refactored
        //       and computed values are properly handled.
        if (not parentSpecifiedValues.font.sameInstance(specifiedValues->font) and
            (parentSpecifiedValues.font->families != specifiedValues->font->families or
             parentSpecifiedValues.font->weight != specifiedValues->font->weight)) {
            auto font = _lookupFontface(*specifiedValues);
            specifiedValues->fontFace = font;
        } else {
            specifiedValues->fontFace = parentSpecifiedValues.fontFace;
        }

        if (specifiedValues->display == Display::Item::YES) {
            generatePseudoElement(*specifiedValues, el, Dom::PseudoElement::MARKER);
        }

        generatePseudoElement(*specifiedValues, el, Dom::PseudoElement::AFTER);
        generatePseudoElement(*specifiedValues, el, Dom::PseudoElement::BEFORE);

        for (auto child = el.firstChild(); child; child = child->nextSibling()) {
            if (auto childEl = child->is<Dom::Element>())
                styleElement(*specifiedValues, *childEl);
        }
    }

    void styleDocument(Dom::Document& doc) {
        if (auto el = doc.documentElement()) {
            auto rootSpecifiedValues = makeRc<SpecifiedValues>(SpecifiedValues::initial());
            rootSpecifiedValues->fontFace = _lookupFontface(*rootSpecifiedValues);
            styleElement(*rootSpecifiedValues, *el);
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
        rule->visit(Visitor{
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
            },
        });
    }
};

} // namespace Vaev::Style
