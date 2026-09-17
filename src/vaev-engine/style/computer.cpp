export module Vaev.Engine:style.computer;

import Karm.Debug;
import Karm.Font;
import Karm.Gc;
import Karm.Gfx;
import Karm.Logger;
import Karm.Math;
import Karm.Ref;

import :dom.document;
import :dom.element;
import :style.ancestorFilter;
import :style.cascaded;
import :style.computed;
import :style.counter;
import :style.ruleIndex;
import :style.stylesheet;

namespace Vaev::Style {

static auto debugCounters = Debug::Flag::debug("web-css-counters", "Log all the registered CSS counters");

export struct Computer {
    Gc::Heap& _heap;
    Media _media;
    RegisteredPropertySet& _registeredPropertySet;
    StyleSheetList const& _stylesheets;
    Rc<Font::Database> _fontDatabase;
    RuleIndex _ruleIndex = {};
    AncestorFilter _ancestorFilter = {};
    Viewport _viewport{.small = _media.viewportSize()};
    Opt<Rc<ComputedValues>> _rootComputedValues = NONE;

    struct MpcKey {
        struct NonOwned {
            ComputedValues const& parent;
            Slice<MatchingRule> matchingRules;

            void hash(Meta::Derive<Hasher> auto& h) const {
                Karm::hash(h, reinterpret_cast<usize>(&parent));

                for (auto const& m : matchingRules) {
                    Karm::hash(h, reinterpret_cast<usize>(&m.rule));
                    Karm::hash(h, m.specificity.a);
                    Karm::hash(h, m.specificity.b);
                    Karm::hash(h, m.specificity.c);
                }
            }
        };

        ComputedValues const* parent;
        Vec<Pair<StyleRule const*, Specificity>> matchingRules = {};

        static MpcKey fromNonOwned(NonOwned key) {
            Vec<Pair<StyleRule const*, Specificity>> tempMatching(key.matchingRules.len());
            for (auto const& m : key.matchingRules)
                tempMatching.pushBack({&m.rule, m.specificity});

            return MpcKey {
                .parent = &key.parent,
                .matchingRules = std::move(tempMatching),
            };
        }

        bool operator==(MpcKey const& other) const = default;

        bool operator==(NonOwned other) const {
            if (parent != &other.parent or matchingRules.len() != other.matchingRules.len())
                return false;

            for (usize i = 0; i < matchingRules.len(); i++) {
                auto const [rule, specificity] = matchingRules[i];
                auto const& otherMatch = other.matchingRules[i];
                if (rule != &otherMatch.rule or specificity != otherMatch.specificity)
                    return false;
            }

            return true;
        }

        void hash(Meta::Derive<Hasher> auto& h) const {
            Karm::hash(h, reinterpret_cast<usize>(parent));

            for (auto const& [rule, spec] : matchingRules) {
                Karm::hash(h, reinterpret_cast<usize>(rule));
                Karm::hash(h, spec.a);
                Karm::hash(h, spec.b);
                Karm::hash(h, spec.c);
            }
        }
    };

    // Matched property cache to avoid ComputedValues dedup when not necessary.
    // FIXME: Investigate if an LRU would be better.
    Map<MpcKey, Cow<ComputedValues>> _mpc = {};

    // MARK: Counters ----------------------------------------------------------

    // https://drafts.csswg.org/css-lists/#counter-scope
    Yield<Dom::OriginatingElement> _iterElementInScope(Dom::Element& el) {
        for (Gc::Ptr<Dom::Node> sibling = el; sibling; sibling = sibling->nextSibling()) {
            if (auto element = sibling->as<Dom::Element>()) {
                co_yield element.upgrade();
                for (Gc::Ref<Dom::Node> child : element->iterDepthFirst())
                    if (auto childElement = child->as<Dom::Element>())
                        co_yield Dom::OriginatingElement{childElement.upgrade()};
            }
        }
    }

    // https://drafts.csswg.org/css-lists/#instantiate-counter:~:text=dynamically%20calculate%20the%20initial%20value
    Integer _dynamicallyCalculateCounterInitialValue(CustomIdent counter, Dom::Element& element) {
        // 1. Let num be 0.
        Integer num = 0;

        // 2. Let lastNonZeroIncrementNegated be 0.
        Integer lastNonZeroIncrementNegated = 0;

        // 3. For each element or pseudo-element el that increments or sets the same counter in the same scope:
        for (auto el : _iterElementInScope(element)) {
            auto maybeCounterIncrement =
                iter(el.computedValues()->counters->increment) |
                FindFirst([&](CounterProps::Increment const& increment) {
                    return increment.name == counter;
                });

            // 1. Let incrementNegated be el’s counter-increment integer value for this counter, multiplied by -1.
            Integer incrementNegated =
                maybeCounterIncrement
                    .unwrapOr({counter, Some(1)})
                    .value.unwrapOr(1) *
                -1;

            // 2. If incrementNegated is not zero, then set lastNonZeroIncrementNegated to incrementNegated.
            if (incrementNegated != 0)
                lastNonZeroIncrementNegated = incrementNegated;

            // 3. If el sets this counter with counter-set, then add that integer value to num and break this loop.
            auto maybeCounterSet =
                iter(el.computedValues()->counters->set) |
                FindFirst([&](CounterProps::Set const& set) {
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
    CounterSet _resolveCounter(CounterSet& parent, CounterSet& sibling, Dom::Element& element, ComputedValues const& style) {
        auto const& countersStyle = *style.counters;
        Dom::ElementHandle elementHandle = &element;

        // 1. Existing counters are inherited from previous elements.
        auto counters = CounterSet::inherits(parent, sibling);

        // 2. New counters are instantiated (counter-reset).
        for (auto& counterReset : countersStyle.reset) {
            Integer initial = counterReset.value.unwrapOrElse([&] {
                return counterReset.reversed ? _dynamicallyCalculateCounterInitialValue(counterReset.name, element) : 0;
            });
            counters.instantiateCounter(elementHandle, counterReset, initial);
        }

        // 3. Counter values are incremented (counter-increment).
        if (countersStyle.increment) {
            for (auto& counterIncrement : countersStyle.increment) {
                counters.increment(elementHandle, counterIncrement);
            }
        } else if (style.display == Display::Item::YES) {
            // https://www.w3.org/TR/css-lists-3/#list-item-counter
            counters.increment(elementHandle, {.name = CustomIdent{"list-item"_sym}, .value = Some(1)});
        }

        // 4. Counter values are explicitly set (counter-set).
        for (auto& counterSet : countersStyle.set)
            counters.set(elementHandle, counterSet);

        return counters;
    }

    CounterSet _resolveCounters(CounterSet& parentCounters, CounterSet& siblingCounters, Dom::Element& el) {
        CounterSet currentCounters = _resolveCounter(
            parentCounters,
            siblingCounters,
            el,
            *el.computedValues()
        );
        CounterSet childSiblingCounters = {};

        for (auto child = el.firstChild(); child; child = child->nextSibling()) {
            if (auto childEl = child->as<Dom::Element>()) {
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

    Rc<Gfx::Fontface> _lookupFontface(ComputedValues const& style) {
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

    void _updateFontface(ComputedValues const& parent, Cow<ComputedValues>& values) {
        // FIXME: Use a font-dirty flag instead.
        if (not parent.font.sameInstance(values->font) and
            (parent.font->families != values->font->families or
             parent.font->weight != values->font->weight or
             parent.font->style != values->font->style or
             parent.font->width != values->font->width)) {
            auto font = _lookupFontface(*values);
            values.cow().fontFace = font;
        } else {
            values.cow().fontFace = parent.fontFace;
        }
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
                cascadedValues.putStyleAttribute(property.take(), Origin::AUTHOR_PRESENTATIONAL_HINT, PRESENTATION_HINT_SPEC);
        }

        // https://html.spec.whatwg.org/multipage/obsolete.html#dom-document-bgcolor
        if (auto const& [bgcolor] = el->getAttribute(Html::BGCOLOR_ATTR)) {
            if (auto property = _registeredPropertySet.parseValue(
                    Properties::BACKGROUND_COLOR, bgcolor, {}
                ))
                cascadedValues.putStyleAttribute(property.take(), Origin::AUTHOR_PRESENTATIONAL_HINT, PRESENTATION_HINT_SPEC);
        }

        // https://html.spec.whatwg.org/multipage/images.html#sizes-attributes
        if (auto const& [width] = el->getAttribute(Html::WIDTH_ATTR)) {
            if (auto property = _registeredPropertySet.parseValue(
                    Properties::WIDTH, width, {}
                ))
                cascadedValues.putStyleAttribute(property.take(), Origin::AUTHOR_PRESENTATIONAL_HINT, PRESENTATION_HINT_SPEC);
        }

        // https://html.spec.whatwg.org/multipage/images.html#sizes-attributes
        if (auto const& [height] = el->getAttribute(Html::HEIGHT_ATTR)) {
            if (auto property = _registeredPropertySet.parseValue(
                    Properties::HEIGHT, height, {}
                ))
                cascadedValues.putStyleAttribute(property.take(), Origin::AUTHOR_PRESENTATIONAL_HINT, PRESENTATION_HINT_SPEC);
        }

        // https://html.spec.whatwg.org/multipage/input.html#the-size-attribute
        if (auto const& [size] = el->getAttribute(Html::SIZE_ATTR)) {
            if (auto property = _registeredPropertySet.parseValue(
                    Properties::WIDTH, Io::format("{}ch", size), {}
                ))
                cascadedValues.putStyleAttribute(property.take(), Origin::AUTHOR_PRESENTATIONAL_HINT, PRESENTATION_HINT_SPEC);
        }
    }

    void _considerInlineStyleAttribute(Gc::Ref<Dom::Element> el, CascadedValues& cascadedValues) {
        auto styleAttr = el->style();

        auto declarations = _registeredPropertySet.parseDeclarations(
            styleAttr ? *styleAttr : "",
            RegisteredPropertySet::TOP_LEVEL
        );
        for (auto& decl : declarations)
            cascadedValues.putStyleAttribute(decl, Origin::INLINE, INLINE_SPEC);
    }

    static void _considerElementAttributes(Cow<ComputedValues>& values, Gc::Ref<Dom::Element> el) {
        // https://html.spec.whatwg.org/multipage/obsolete.html#attr-table-align
        if (auto const& [align] = el->getAttribute(Html::ALIGN_ATTR)) {
            if (align == "left") {
                values.cow().text.cow().align = TextAlign::LEFT;
            } else if (align == "right") {
                values.cow().text.cow().align = TextAlign::RIGHT;
            } else if (align == "center") {
                values.cow().text.cow().align = TextAlign::BLOCK_CENTER;
            } else if (align == "justify") {
                values.cow().text.cow().align = TextAlign::JUSTIFY;
            }
        }
    }

    // https://svgwg.org/specs/integration/#svg-css-sizing
    void _applySvgElementSizingRules(Gc::Ref<Dom::Element> svgEl, CascadedValues& cascadedValues) {
        if (auto parentEl = svgEl->parentNode()->as<Dom::Element>()) {
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
                cascadedValues.putStyleAttribute(
                    _registeredPropertySet.parseValue(Properties::WIDTH, "300px", {}).unwrap(),
                    Origin::AUTHOR_PRESENTATIONAL_HINT, PRESENTATION_HINT_SPEC
                );

            if (not svgEl->hasAttribute(Svg::HEIGHT_ATTR))
                cascadedValues.putStyleAttribute(
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

        for (auto const& attr : el->attributes)
            if (auto const& [property] = _registeredPropertySet.parsePresentationAttribute(attr.qualifiedName.name, attr.value))
                cascadedValues.putStyleAttribute(property, Origin::AUTHOR_PRESENTATIONAL_HINT, PRESENTATION_HINT_SPEC);

        if (el->qualifiedName == Svg::SVG_TAG)
            _applySvgElementSizingRules(el, cascadedValues);
    }

    bool _isMpcEligible(Gc::Ref<Dom::Element> el, Opt<Symbol> pseudoElement) {
        return not pseudoElement and el->namespaceUri() == Html::NAMESPACE and not el->containsStylingAttribute();
    }

    // https://drafts.csswg.org/css-cascade/#cascade-origin
    Rc<ComputedValues const> computeValues(ComputedValues const& parent, Gc::Ref<Dom::Element> el, Opt<Symbol> pseudoElement = NONE) {
        auto matchingRules = _ruleIndex.match(el, pseudoElement, _ancestorFilter);

        bool isRootElement = pseudoElement == NONE and el->parentNode()->is<Dom::Document>();
        bool cacheEligible = _isMpcEligible(el, pseudoElement);

        CascadedValues cascadedValues;

        Opt<Cow<ComputedValues>> cached = NONE;
        if (cacheEligible)
            cached = _mpc.lookup(MpcKey::NonOwned{parent, matchingRules});

        Cow<ComputedValues> values = cached.unwrapOrElse([&] {
            auto v = _registeredPropertySet.inheritsComputedValues(parent);

            for (auto const& [styleRule, specificity, order] : matchingRules)
                cascadedValues.putStyleRule(styleRule, specificity, order);

            return Cow{v};
        });

        if (not cached) {
            if (not pseudoElement) {
                _considerHtmlPresentationalHint(el, cascadedValues);
                _considerInlineStyleAttribute(el, cascadedValues);
                _considerSvgPresentationAttributes(el, cascadedValues);
            }

            ComputationContext cx;
            cx.populateUsingViewport(_viewport);

            if (isRootElement)
                cx.populateUsingRootComputedValues(*values);
            else if (_rootComputedValues)
                cx.populateUsingRootComputedValues(**_rootComputedValues);

            cx.populateUsingParentComputedValues(parent);
            cx.populateUsingOwnComputedValues(parent);

            cascadedValues.apply(Property::ComputationPhase::CUSTOM_PROPERTY, parent, values, cx);
            cascadedValues.expandShorthands(parent, values.cow(), _registeredPropertySet);

            cascadedValues.apply(Property::ComputationPhase::PRE_FONT, parent, values, cx);
            cascadedValues.apply(Property::ComputationPhase::FONT, parent, values, cx);

            _updateFontface(parent, values);

            // NOTE: Correct as long as the computation context doesn't need any NORMAL or LATE property.
            if (isRootElement) {
                _rootComputedValues = Some(makeRc<ComputedValues>(*values));
                cx.populateUsingRootComputedValues(*values);
            }
            cx.populateUsingOwnComputedValues(*values);

            cascadedValues.apply(Property::ComputationPhase::NORMAL, parent, values, cx);
            cascadedValues.apply(Property::ComputationPhase::LATE, parent, values, cx);

            if (cacheEligible)
                _mpc.put(MpcKey::fromNonOwned({
                    .parent = parent,
                    .matchingRules = matchingRules,
                }), values);
        }

        if (not pseudoElement)
            _considerElementAttributes(values, el);

        // NOSPEC: By default Chrome and other browsers render disc/circle/square
        // list markers a bit larger than their font size would otherwise imply.
        if (pseudoElement == Dom::PseudoElement::MARKER and values->content.is<Keywords::Normal>()) {
            auto listStyleType = parent.list->type;
            if (listStyleType == CustomIdent{"disc"_sym} or
                listStyleType == CustomIdent{"circle"_sym} or
                listStyleType == CustomIdent{"square"_sym}) {
                values.cow().transform.cow().transform = TransformList{ScaleTransform{1.25, 1.25}};
            }
        }

        return values._inner;
    }

    Rc<PageComputedValues> computeValues(ComputedValues const& parent, Page const& page) {
        auto computed = makeRc<PageComputedValues>(_heap, parent);

        for (auto const& sheet : _stylesheets.items)
            for (auto const& rule : sheet.rules)
                _evalRule(rule, page, *computed);

        for (auto& area : computed->_areas) {
            auto font = _lookupFontface(*area->computedValues());
            area->_computedValues.unwrap()->fontFace = font;
        }

        return computed;
    }

    // MARK: Styling -----------------------------------------------------------

    void generatePseudoElement(ComputedValues const& parentComputedValues, Dom::Element& el, Symbol type) {
        auto computedValues = computeValues(parentComputedValues, el, Some(type));

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
        auto computedValues = computeValues(parentComputedValues, el);
        el._computedValues = Some(computedValues);

        if (computedValues->display == Display::Item::YES)
            generatePseudoElement(*computedValues, el, Dom::PseudoElement::MARKER);

        generatePseudoElement(*computedValues, el, Dom::PseudoElement::AFTER);
        generatePseudoElement(*computedValues, el, Dom::PseudoElement::BEFORE);

        _ancestorFilter.push(el);
        for (auto child = el.firstChild(); child; child = child->nextSibling()) {
            if (auto childEl = child->as<Dom::Element>())
                styleElement(*computedValues, *childEl);
        }
        _ancestorFilter.pop();
    }

    // MARK: Body Background ---------------------------------------------------

    // https://www.w3.org/TR/css-backgrounds-3/#body-background
    static void _propagateBodyBackgroundToHtml(Dom::Document& doc) {
        // For documents whose root element is an HTML HTML element or an XHTML html element
        auto htmlElement = doc.documentElement();
        if (htmlElement->namespaceUri() != Html::NAMESPACE)
            return;
        auto rootBackground = htmlElement->computedValues()->backgrounds;

        auto bodyElement = doc.body();
        if (bodyElement == nullptr)
            return;
        auto bodyBackground = bodyElement->computedValues()->backgrounds;

        // If the computed value of background-image on the
        // root element is none and its background-color is transparent
        if (rootBackground->color == TRANSPARENT and rootBackground->imageIsNone()) {
            // User agents must instead propagate the computed values of the
            // background properties from that element’s first HTML BODY
            // or XHTML body child element.
            auto newRootValues = makeRc<ComputedValues>(*htmlElement->computedValues());
            newRootValues->backgrounds = bodyBackground;
            htmlElement->_computedValues = Some(newRootValues);

            auto newBodyValues = makeRc<ComputedValues>(*bodyElement->computedValues());
            newBodyValues->backgrounds = {};
            bodyElement->_computedValues = Some(newBodyValues);
        }
    }

    CounterStyleSet _resolveCounterStyle(StyleSheetList const& stylesheets) {
        CounterDescriptorSet counters;
        for (auto const& sheet : stylesheets.items) {
            for (auto const& rule : sheet.rules) {
                if (auto it = rule.is<CounterRule>()) {
                    CounterDescriptors descriptor;
                    for (auto const& d : it->descriptors)
                        d.apply(descriptor);
                    counters.put(it->name, descriptor);
                }
            }
        }
        return resolveExtends(counters);
    }

    void styleDocument(Dom::Document& doc) {
        _rootComputedValues = NONE;

        doc.counters = _resolveCounterStyle(*doc.styleSheets);
        logDebugIf(debugCounters, "counters: {}", doc.counters);

        if (auto el = doc.documentElement()) {
            auto initialComputedValues = doc.initialComputedValues();
            initialComputedValues->fontFace = _lookupFontface(*initialComputedValues);
            styleElement(*initialComputedValues, *el);
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
        for (auto const& sheet : _stylesheets.items) {
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
