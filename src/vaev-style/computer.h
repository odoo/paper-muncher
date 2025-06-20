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

    struct RuleLookup {
        Map<String, Vec<Tuple<usize, Cursor<Rule>>>> typeRules;
        Map<String, Vec<Tuple<usize, Cursor<Rule>>>> iDRules;
        Map<String, Vec<Tuple<usize, Cursor<Rule>>>> classRules;

        Vec<Tuple<usize, Cursor<Rule>>> nonLookupRules;
        usize _ruleCount = 0;

        void _buildRule(Cursor<Rule> rule, Media media) {
            // return;
            rule->visit(Visitor{
                [&](StyleRule const& r) {
                    _ruleCount++;
                    r.selector.visit(Visitor{
                        [&](TypeSelector const& s) {
                            if (not typeRules.has(s.elementName))
                                typeRules.put(s.elementName, Vec<Tuple<usize, Cursor<Rule>>>{});
                            typeRules.get(s.elementName).pushBack({_ruleCount, rule});
                        },
                        [&](IdSelector const& s) {
                            if (not iDRules.has(s.id))
                                iDRules.put(s.id, Vec<Tuple<usize, Cursor<Rule>>>{});
                            iDRules.get(s.id).pushBack({_ruleCount, rule});
                        },
                        [&](ClassSelector const& s) {
                            if (not classRules.has(s.class_))
                                classRules.put(s.class_, Vec<Tuple<usize, Cursor<Rule>>>{});
                            classRules.get(s.class_).pushBack({_ruleCount, rule});
                        },
                        [&](auto const&) {
                            nonLookupRules.pushBack({_ruleCount, rule});
                        },
                    });
                },
                [&](MediaRule const& r) {
                    if (r.match(media))
                        for (auto const& subRule : r.rules)
                            _buildRule(&subRule, media);
                },
                [&](auto const&) {
                    // Ignore other rule types
                },
            });
        }

        void build(Media media, StyleSheetList const& styleBook) {
            // Add rules from the style book
            for (auto const& sheet : styleBook.styleSheets) {
                for (auto const& rule : sheet.rules) {
                    _buildRule(&rule, media);
                }
            }
            Karm::logDebug("just visited {} rules", _ruleCount);
        }
    };

    RuleLookup _ruleLookup{};

    void build() {
        _ruleLookup.build(_media, _styleBook);
        loadFontFaces();
    }

    Vec<Cursor<Tuple<usize, Cursor<Rule>>>> partA(Gc::Ref<Dom::Element> el);
    MatchingRules partB(Vec<Cursor<Tuple<usize, Cursor<Rule>>>>& cursors, Gc::Ref<Dom::Element> el);

    // MARK: Cascading ---------------------------------------------------------

    MatchingRules _buildMatchingRules(Gc::Ref<Dom::Element> el);

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
