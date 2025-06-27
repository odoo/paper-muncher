#include <karm-sys/chan.h>
#include <vaev-style/decls.h>

#include "computed.h"
#include "computer.h"

namespace Vaev::Style {

void RuleLookup::build(Media media, StyleSheetList const& styleBook) {
    for (auto const& sheet : styleBook.styleSheets) {
        for (auto const& rule : sheet.rules) {
            _buildRule(&rule, media);
        }
    }
}

void Computer::_evalRule(Rule const& rule, Gc::Ref<Dom::Element> el, MatchingRules& matches) {
    rule.visit(Visitor{
        [&](StyleRule const& r) {
            if (auto specificity = r.match(el))
                matches.pushBack({&r, specificity.unwrap()});
        },
        [&](MediaRule const& r) {
            if (r.match(_media))
                for (auto const& subRule : r.rules)
                    _evalRule(subRule, el, matches);
        },
        [&](auto const&) {
            // Ignore other rule types
        },
    });
}

void Computer::_evalRule(Rule const& rule, Page const& page, PageComputedStyle& c) {
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

void Computer::_evalRule(Rule const& rule, Vec<FontFace>& fontFaces) {
    rule.visit(Visitor{
        [&](FontFaceRule const& r) {
            auto& fontFace = fontFaces.emplaceBack();
            for (auto const& decl : r.descs)
                decl.apply(fontFace);
        },
        [&](MediaRule const& r) {
            if (r.match(_media))
                for (auto const& subRule : r.rules)
                    _evalRule(subRule, fontFaces);
        },
        [&](auto const&) {
            // Ignore other rule types
        },
    });
}

Rc<SpecifiedValues> Computer::_evalCascade(SpecifiedValues const& parent, MatchingRules& matchingRules) {
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

static Rc<Karm::Text::Fontface> _lookupFontface(Text::FontBook& fontBook, Style::SpecifiedValues& style) {
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

// https://svgwg.org/specs/integration/#svg-css-sizing
void _applySVGElementSizingRules(Gc::Ref<Dom::Element> svgEl, Vec<Style::StyleProp>& styleProps) {
    if (auto parentEl = svgEl->parentNode()->is<Dom::Element>()) {
        // **If we have an <svg> element inside a CSS context**
        if (parentEl->tagName.ns == SVG)
            return;

        // To resolve 'auto' value on ‘svg’ element if the ‘viewBox’ attribute is not specified:
        // - ...
        // - If any of the sizing attributes are missing, resolve the missing ‘svg’ element width to '300px' and missing
        // height to '150px' (using CSS 2.1 replaced elements size calculation).
        if (not svgEl->hasAttribute(AttrName::make("viewBox", SVG))) {
            if (not svgEl->hasAttribute(AttrName::make("width", SVG))) {
                styleProps.pushBack(WidthProp{CalcValue<PercentOr<Length>>{PercentOr<Length>{Length{Au{300}}}}});
            }
            if (not svgEl->hasAttribute(AttrName::make("height", SVG))) {
                styleProps.pushBack(HeightProp{CalcValue<PercentOr<Length>>{PercentOr<Length>{Length{Au{150}}}}});
            }
        }
    }
}

// https://svgwg.org/svg2-draft/styling.html#PresentationAttributes
Vec<Style::StyleProp> _considerPresentationAttributes(Gc::Ref<Dom::Element> el) {
    if (el->tagName.ns != SVG)
        return {};

    Vec<Style::StyleProp> styleProps;
    for (auto [attr, attrValue] : el->attributes.iter()) {
        parseSVGPresentationAttribute(attr.name(), attrValue->value, styleProps);
    }

    if (el->tagName == Svg::SVG)
        _applySVGElementSizingRules(el, styleProps);

    return styleProps;
}

Computer::MatchingRules Computer::mergeMatchedRules(Vec<Cursor<Tuple<usize, Cursor<Rule>>>>&& cursors, Gc::Ref<Dom::Element> el) {
    MatchingRules matchingRules;

    // Classic application for merging sorted lists, but we don't expect `cursors` to be big,
    // so we use Vec instead of PriorityQueue.

    usize countMatchesWithCurrentRule = 0;
    usize lastRuleId = 0;
    Cursor<Rule> lastStyleRule = nullptr;

    auto maybeFinalizeNfixOrRule = [&]() {
        if (not lastStyleRule)
            return;

        auto const& asStyleRule = lastStyleRule->is<StyleRule>();

        if (auto nfix = asStyleRule->selector.is<Nfix>()) {
            if (nfix->type != Nfix::OR)
                return;

            if (countMatchesWithCurrentRule == 1) {
                _evalRule(*lastStyleRule, el, matchingRules);
            } else {
                matchingRules.pushBack({asStyleRule, spec(asStyleRule->selector)});
            }
        }
    };

    while (cursors.len() > 0) {
        usize bestCursorIdx = 0;
        for (usize i = 1; i < cursors.len(); i++) {
            if (cursors[i]->v0 < cursors[bestCursorIdx]->v0) {
                bestCursorIdx = i;
            }
        }

        auto& rule = *cursors[bestCursorIdx]->v1;

        // TODO: not sure this is better for performance, TBD
        if (lastRuleId != cursors[bestCursorIdx]->v0) {
            maybeFinalizeNfixOrRule();
            countMatchesWithCurrentRule = 1;
        } else {
            countMatchesWithCurrentRule++;
        }

        if (auto styleRule = rule.is<StyleRule>()) {
            if (RuleLookup::lookupIsMatch(styleRule->selector)) {
                matchingRules.pushBack({styleRule, spec(styleRule->selector)});
            } else if (auto nfix = styleRule->selector.is<Nfix>()) {
                if (nfix->type == Nfix::AND) {
                    auto& neededCount = _ruleLookup.ruleIdToNeededCount.get(cursors[bestCursorIdx]->v0);
                    if (countMatchesWithCurrentRule == neededCount) {
                        if (nfix->inners.len() == countMatchesWithCurrentRule) {
                            matchingRules.pushBack({styleRule, spec(styleRule->selector)});
                        } else {
                            _evalRule(rule, el, matchingRules);
                        }
                    }
                } else if (nfix->type == Nfix::OR) {
                    // Do nothing.
                    // Deffering the evaluation to after we know how many times this rule was matched.
                } else {
                    _evalRule(rule, el, matchingRules);
                }
            } else {
                _evalRule(rule, el, matchingRules);
            }

            lastStyleRule = &rule;
        } else {
            lastStyleRule = nullptr;
            _evalRule(rule, el, matchingRules);
        }

        lastRuleId = cursors[bestCursorIdx]->v0;

        cursors[bestCursorIdx].next();
        if (cursors[bestCursorIdx].ended()) {
            std::swap(cursors[bestCursorIdx], last(cursors));
            cursors.popBack();
        }
    }

    maybeFinalizeNfixOrRule();

    return matchingRules;
}

Computer::MatchingRules Computer::_buildMatchingRules(Gc::Ref<Dom::Element> el) {
    return mergeMatchedRules(_ruleLookup.collectMatchedRulesCursors(el), el);
}

// https://drafts.csswg.org/css-cascade/#cascade-origin
Rc<SpecifiedValues> Computer::computeFor(SpecifiedValues const& parent, Gc::Ref<Dom::Element> el) {
    auto matchingRules = _buildMatchingRules(el);

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

    return _evalCascade(parent, matchingRules);
}

Rc<PageComputedStyle> Computer::computeFor(SpecifiedValues const& parent, Page const& page) {
    auto computed = makeRc<PageComputedStyle>(parent);

    for (auto const& sheet : _styleBook.styleSheets)
        for (auto const& rule : sheet.rules)
            _evalRule(rule, page, *computed);

    for (auto& area : computed->_areas) {
        auto font = _lookupFontface(fontBook, *area.specifiedValues());
        area._computedValues = makeRc<ComputedValues>(font);
    }

    return computed;
}

void Computer::styleElement(SpecifiedValues const& parentSpecifiedValues, ComputedValues const& parentComputedValues, Dom::Element& el) {
    auto specifiedValues = computeFor(parentSpecifiedValues, el);
    el._specifiedValues = specifiedValues;

    // HACK: This is basically nonsense to avoid doing too much font lookup,
    //       and it should be remove once the style engine get refactored
    //       and computed values are properly handled.
    if (not parentSpecifiedValues.font.sameInstance(specifiedValues->font) and
        (parentSpecifiedValues.font->families != specifiedValues->font->families or
         parentSpecifiedValues.font->weight != specifiedValues->font->weight)) {
        auto font = _lookupFontface(fontBook, *specifiedValues);
        el._computedValues = makeRc<ComputedValues>(font);
    } else {
        el._computedValues = makeRc<ComputedValues>(parentComputedValues.fontFace);
    }

    for (auto child = el.firstChild(); child; child = child->nextSibling()) {
        if (auto childEl = child->is<Dom::Element>())
            styleElement(*specifiedValues, *el.computedValues(), *childEl);
    }
}

void Computer::styleDocument(Dom::Document& doc) {
    if (auto el = doc.documentElement()) {
        auto rootSpecifiedValues = makeRc<SpecifiedValues>(SpecifiedValues::initial());
        auto font = _lookupFontface(fontBook, *rootSpecifiedValues);
        auto rootComputedValues = makeRc<ComputedValues>(font);
        styleElement(*rootSpecifiedValues, *rootComputedValues, *el);
    }
}

void Computer::loadFontFaces() {
    for (auto const& sheet : _styleBook.styleSheets) {

        Vec<FontFace> fontFaces;
        for (auto const& rule : sheet.rules)
            _evalRule(rule, fontFaces);

        for (auto const& ff : fontFaces) {
            for (auto const& src : ff.sources) {
                if (src.identifier.is<Mime::Url>()) {
                    auto fontUrl = src.identifier.unwrap<Mime::Url>();

                    auto resolvedUrl = Mime::Url::resolveReference(sheet.href, fontUrl);

                    if (not resolvedUrl) {
                        logWarn("Cannot resolve urls when loading fonts: {} {}", fontUrl, sheet.href);
                        continue;
                    }

                    // FIXME: use attrs from style::FontFace
                    if (fontBook.load(resolvedUrl.unwrap()))
                        break;

                    logWarn("Failed to load font at {}", resolvedUrl);
                } else {
                    if (
                        fontBook.queryExact(Text::FontQuery{.family = src.identifier.unwrap<Text::Family>()})
                    )
                        break;

                    logWarn("Failed to assets font {}", src.identifier.unwrap<Text::Family>());
                }
            }
        }
    }
}

} // namespace Vaev::Style
