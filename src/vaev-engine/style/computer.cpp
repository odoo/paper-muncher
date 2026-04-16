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
import :style.stylesheet;
import :style.ruleIndex;

namespace Vaev::Style {

export struct Computer {
    Media _media;
    RegisteredPropertySet& _registeredPropertySet;
    StyleSheetList const& _stylesheets;
    Rc<Font::Database> _fontDatabase;
    RuleIndex _ruleIndex = {};

    // MARK: Counters ----------------------------------------------------------

    Yield<Dom::OriginatingElement> _iterElementInScope() {}

    // https://drafts.csswg.org/css-lists/#instantiate-counter:~:text=dynamically%20calculate%20the%20initial%20value
    Integer _dynamicallyCalculateCounterInitialValue(CustomIdent counter) {
        // 1. Let num be 0.
        Integer num = 0;

        // 2. Let lastNonZeroIncrementNegated be 0.
        Integer lastNonZeroIncrementNegated = 0;

        // 3. For each element or pseudo-element el that increments or sets the same counter in the same scope:
        for (auto el : _iterElementInScope()) {
            auto maybeCounterIncrement =
                iter(el.computedValues()->counters->increment) |
                Find([&](CounterProps::Increment const& increment) {
                    return increment.name == counter;
                });

            // 1. Let incrementNegated be el’s counter-increment integer value for this counter, multiplied by -1.
            Integer incrementNegated =
                maybeCounterIncrement
                    .unwrapOr({counter, 1})
                    .value.unwrapOr(1) *
                -1;

            // 2. If incrementNegated is not zero, then set lastNonZeroIncrementNegated to incrementNegated.
            if (incrementNegated != 0)
                lastNonZeroIncrementNegated = incrementNegated;

            // 3. If el sets this counter with counter-set, then add that integer value to num and break this loop.
            auto maybeCounterSet =
                iter(el.computedValues()->counters->set) |
                Find([&](CounterProps::Set const& set) {
                    return set.name == counter;
                });

            if (maybeCounterSet) {
                num += maybeCounterSet->value.unwrapOr(0);
                break;
            }

            // 4. Add incrementNegated to num.
            num += incrementNegated;
        }

        // 4. Add lastNonZeroIncrementNegated to num.
        num += lastNonZeroIncrementNegated;

        // 5. Return num.
        return num;
    }

    // https://drafts.csswg.org/css-lists/#auto-numbering
    CounterSet _resolveCounter(CounterSet& parent, CounterSet& sibling, ElementHandle el, ComputedValues const& style) {
        auto const& countersStyle = *style.counters;

        // 1. Existing counters are inherited from previous elements.
        auto counters = CounterSet::inherits(parent, sibling);

        // 2. New counters are instantiated (counter-reset).
        for (auto& counterReset : countersStyle.reset) {
            Integer initial = counterReset.value.unwrapOrElse([&] {
                return counterReset.reversed ? _dynamicallyCalculateCounterInitialValue(counterReset.name) : 0;
            });
            counters.instantiateCounter(el, counterReset, initial);
        }

        // 3. Counter values are incremented (counter-increment).
        if (countersStyle.increment) {
            for (auto& counterIncrement : countersStyle.increment) {
                counters.increment(el, counterIncrement);
            }
        } else if (style.display == Display::Item::YES) {
            // https://www.w3.org/TR/css-lists-3/#list-item-counter
            counters.increment(el, {.name = CustomIdent{"list-item"_sym}, .value = 1});
        }

        // 4. Counter values are explicitly set (counter-set).
        for (auto& counterSet : countersStyle.set)
            counters.increment(el, counterSet);

        return counters;
    }

    CounterSet _resolveCounters(CounterSet& parentCounters, CounterSet& siblingCounters, Dom::Element& el) {
        CounterSet currentCounters = _resolveCounter(
            parentCounters,
            siblingCounters,
            &el,
            *el.computedValues()
        );
        CounterSet childSiblingCounters = {};

        for (auto child = el.firstChild(); child; child = child->nextSibling()) {
            if (auto childEl = child->is<Dom::Element>()) {
                childSiblingCounters = _resolveCounters(
                    currentCounters,
                    childSiblingCounters,
                    *childEl
                );
                childEl->counters = childSiblingCounters;
            }
        }

        return currentCounters;
    }

    // MARK: Cascading ---------------------------------------------------------

    void _evalRule(Rule const& rule, Page const& page, PageComputedValues& c) {
        rule.visit(Visitor{
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
            },
        });
    }

    Rc<ComputedValues> _evalCascade(ComputedValues const& parentComputedValues, MatchingRules& matchingRules) {
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
        auto computedValues = _registeredPropertySet.inheritsComputedValues(parentComputedValues);
        Vec<Rc<Property>> importantProps;

        // HACK: Apply custom properties first
        for (auto const& [styleRule, _] : matchingRules) {
            for (auto& prop : styleRule->props) {
                if (prop->isCustomProperty())
                    prop->apply(parentComputedValues, *computedValues);
            }
        }

        for (auto const& [styleRule, _] : matchingRules) {
            for (auto& prop : styleRule->props) {
                if (prop->isBogusProperty())
                    continue;

                if (prop->isCustomProperty())
                    continue;

                if (prop->important == Css::Important::YES) {
                    importantProps.pushBack(prop);
                    continue;
                }

                if (prop->isShorthandProperty()) {
                    for (auto& longhand : prop->expandShorthand(_registeredPropertySet, parentComputedValues, *computedValues)) {
                        longhand->apply(parentComputedValues, *computedValues);
                    }
                    continue;
                }

                prop->apply(parentComputedValues, *computedValues);
            }
        }

        for (auto const& prop : importantProps) {
            if (prop->isBogusProperty())
                continue;

            if (prop->isShorthandProperty()) {
                for (auto& longhand : prop->expandShorthand(_registeredPropertySet, parentComputedValues, *computedValues)) {
                    longhand->apply(parentComputedValues, *computedValues);
                }
                continue;
            }

            prop->apply(parentComputedValues, *computedValues);
        }

        return computedValues;
    }

    // MARK: Computing ---------------------------------------------------------

    Rc<Gfx::Fontface> _lookupFontface(ComputedValues& style) {
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
    Vec<Rc<Property>> _considerPresentationalHint(Gc::Ref<Dom::Element> el) {
        if (el->namespaceUri() != Html::NAMESPACE)
            return {};

        Vec<Rc<Property>> res;
        // https://html.spec.whatwg.org/multipage/obsolete.html#dom-document-fgcolor
        if (auto fgcolor = el->getAttribute(Html::FGCOLOR_ATTR)) {
            if (auto property = _registeredPropertySet.parseValue(
                    Properties::COLOR, fgcolor.unwrap(), {}
                ))
                res.pushBack(property.take());
        }

        // https://html.spec.whatwg.org/multipage/obsolete.html#dom-document-bgcolor
        if (auto bgcolor = el->getAttribute(Html::BGCOLOR_ATTR)) {
            if (auto property = _registeredPropertySet.parseValue(
                    Properties::BACKGROUND_COLOR, bgcolor.unwrap(), {}
                ))
                res.pushBack(property.take());
        }

        // https://html.spec.whatwg.org/multipage/images.html#sizes-attributes
        if (auto width = el->getAttribute(Html::WIDTH_ATTR)) {
            if (auto property = _registeredPropertySet.parseValue(
                    Properties::WIDTH, width.unwrap(), {}
                ))
                res.pushBack(property.take());
        }

        // https://html.spec.whatwg.org/multipage/images.html#sizes-attributes
        if (auto height = el->getAttribute(Html::HEIGHT_ATTR)) {
            if (auto property = _registeredPropertySet.parseValue(
                    Properties::HEIGHT, height.unwrap(), {}
                ))
                res.pushBack(property.take());
        }

        // https://html.spec.whatwg.org/multipage/input.html#the-size-attribute
        if (auto size = el->getAttribute(Html::SIZE_ATTR)) {
            if (auto property = _registeredPropertySet.parseValue(
                    Properties::WIDTH, Io::format("{}ch", size), {}
                ))
                res.pushBack(property.take());
        }

        return res;
    }

    static void _considerElementAttributes(ComputedValues& values, Gc::Ref<Dom::Element> el) {
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
    void _applySVGElementSizingRules(Gc::Ref<Dom::Element> svgEl, Vec<Rc<Property>>& styleProps) {
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
                styleProps.pushBack(
                    _registeredPropertySet.parseValue(Properties::WIDTH, "300px", {}).unwrap()
                );

            if (not svgEl->hasAttribute(Svg::HEIGHT_ATTR))
                styleProps.pushBack(
                    _registeredPropertySet.parseValue(Properties::HEIGHT, "150px", {}).unwrap()
                );
        }
    }

    // https://svgwg.org/svg2-draft/styling.html#PresentationAttributes
    Vec<Rc<Property>> _considerPresentationAttributes(Gc::Ref<Dom::Element> el) {
        if (el->qualifiedName.ns != Svg::NAMESPACE)
            return {};

        Vec<Rc<Property>> styleProps;
        for (auto [attr, attrValue] : el->attributes.iterItems()) {
            if (auto property = _registeredPropertySet.parsePresentationAttribute(attr.name, attrValue->value)) {
                styleProps.pushBack(property.take());
            }
        }

        if (el->qualifiedName == Svg::SVG_TAG)
            _applySVGElementSizingRules(el, styleProps);

        return styleProps;
    }

    // https://drafts.csswg.org/css-cascade/#cascade-origin
    Rc<ComputedValues> computeFor(ComputedValues const& parent, Gc::Ref<Dom::Element> el) {
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
            .props = _registeredPropertySet.parseDeclarations(
                styleAttr ? *styleAttr : "",
                RegisteredPropertySet::TOP_LEVEL
            ),
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

    Rc<ComputedValues> computeFor(ComputedValues const& parent, Gc::Ref<Dom::Element> el, Symbol pseudoElement) {
        MatchingRules matchingRules = _ruleIndex.match(el, pseudoElement);
        return _evalCascade(parent, matchingRules);
    }

    Rc<PageComputedValues> computeFor(ComputedValues const& parent, Page const& page) {
        auto computed = makeRc<PageComputedValues>(parent);

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

        // HACK: This is basically nonsense to avoid doing too much font lookup,
        //       and it should be remove once the style engine get refactored
        //       and computed values are properly handled.
        if (not parentComputedValues.font.sameInstance(computedValues->font) and
            (parentComputedValues.font->families != computedValues->font->families or
             parentComputedValues.font->weight != computedValues->font->weight)) {
            auto font = _lookupFontface(*computedValues);
            computedValues->fontFace = font;
        } else {
            computedValues->fontFace = parentComputedValues.fontFace;
        }

        // https://drafts.csswg.org/css-content/#valdef-content-none
        // On pseudo-elements it inhibits the creation of the pseudo-element as if it had display: none.
        if (computedValues->content == Keywords::NONE)
            return;

        // https://drafts.csswg.org/css-content/#valdef-content-normal
        if (computedValues->content == Keywords::NORMAL and
            (type == Dom::PseudoElement::BEFORE or
             type == Dom::PseudoElement::AFTER))
            return;

        el.addPseudoElement(makeRc<Dom::PseudoElement>(type, computedValues));
    }

    void styleElement(ComputedValues const& parentComputedValues, Dom::Element& el) {
        auto computedValues = computeFor(parentComputedValues, el);
        el._computedValues = computedValues;

        // HACK: This is basically nonsense to avoid doing too much font lookup,
        //       and it should be remove once the style engine get refactored
        //       and computed values are properly handled.
        if (not parentComputedValues.font.sameInstance(computedValues->font) and
            (parentComputedValues.font->families != computedValues->font->families or
             parentComputedValues.font->weight != computedValues->font->weight)) {
            auto font = _lookupFontface(*computedValues);
            computedValues->fontFace = font;
        } else {
            computedValues->fontFace = parentComputedValues.fontFace;
        }

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
            CounterSet rootParentCounters = {};
            CounterSet rootSiblingCounters = {};
            _resolveCounters(
                rootParentCounters,
                rootSiblingCounters,
                *el
            );
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
