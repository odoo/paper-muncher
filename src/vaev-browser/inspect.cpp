export module Vaev.Browser:inspect;

import Vaev.Engine;
import Karm.Kira;
import Karm.Ui;
import Karm.Print;
import Karm.Math;
import Karm.Gc;
import Karm.Gfx;
import Karm.Core;
import Karm.Logger;
import Mdi;

using namespace Karm;
using namespace Karm::Literals;
using namespace Karm::Fmt::Literals;
using namespace Vaev;

namespace Vaev::Browser {

export enum struct InspectTab {
    ELEMENTS,
    LAYOUT,
    MEDIA,
    _LEN,
};

export enum struct InspectStyleTab {
    CASCADED,
    COMPUTED,
    _LEN,
};

export struct InspectToggleVisible {};

export struct InspectToggleWireframe {};

export struct InspectToggleBoxModel {};

export struct InspectExpandNode {
    Gc::Ref<Dom::Node> node;
};

export struct InspectSelectNode {
    Gc::Ref<Dom::Node> node;
};

export struct InspectChangeFilter {
    String filter;
};

export struct InspectChangePaper {
    Print::PaperStock paper;
};

export struct InspectChangeOrientation {
    Print::Orientation orientation;
};

export struct InspectChangeMargin {
    Print::Margins margins;
};

export struct InspectChangeScale {
    f64 scale;
};

export struct InspectToggleHeaderFooter {};

export using InspectAction = Union<
    InspectToggleVisible,
    InspectToggleWireframe,
    InspectToggleBoxModel,
    InspectTab,
    InspectStyleTab,
    InspectExpandNode,
    InspectSelectNode,
    InspectChangeFilter,
    InspectChangePaper,
    InspectChangeOrientation,
    InspectChangeMargin,
    InspectChangeScale,
    InspectToggleHeaderFooter>;

export struct InspectState {
    bool visible = false;
    bool wireframe = false;
    bool boxModel = false;
    InspectTab tab = InspectTab::ELEMENTS;
    InspectStyleTab styleTab = InspectStyleTab::CASCADED;
    String filter = ""s;
    Set<Gc::Ref<Dom::Node>> expandedNodes = {};
    Gc::Ptr<Dom::Node> selectedNode = nullptr;
    Print::Settings settings;

    void apply(InspectAction action) {
        action.visit(
            [&](InspectToggleVisible const&) {
                visible = not visible;
            },
            [&](InspectToggleWireframe const&) {
                wireframe = not wireframe;
            },
            [&](InspectToggleBoxModel const&) {
                boxModel = not boxModel;
            },
            [&](InspectTab const& a) {
                tab = a;
            },
            [&](InspectStyleTab const& a) {
                styleTab = a;
            },
            [&](InspectExpandNode const& a) {
                if (not expandedNodes.remove(a.node))
                    expandedNodes.add(a.node);
            },
            [&](InspectSelectNode const& a) {
                if (a.node->hasChildren())
                    expandedNodes.add(a.node);
                selectedNode = a.node;
                for (auto current = selectedNode->parentNode(); current; current = current->parentNode()) {
                    expandedNodes.add(Gc::Ref{*current});
                }
            },
            [&](InspectChangeFilter const& a) {
                filter = a.filter;
            },
            [&](InspectChangePaper const& a) {
                settings.stock = a.paper;
            },
            [&](InspectChangeOrientation const& a) {
                settings.orientation = a.orientation;
            },
            [&](InspectChangeMargin const& a) {
                settings.margins = a.margins;
            },
            [&](InspectChangeScale const& a) {
                settings.scale = a.scale;
            },
            [&](InspectToggleHeaderFooter const&) {
                settings.headerFooter = not settings.headerFooter;
            }
        );
    }
};

auto guide() {
    return Ui::hflow(
        Ui::empty(8),
        Kr::separator(),
        Ui::empty(9)
    );
}

auto idented(isize ident) {
    return [ident](Ui::Child c) -> Ui::Child {
        Ui::Children res;
        res.pushBack(Ui::empty(4));
        for (isize i = 0; i < ident; i++) {
            res.pushBack(guide());
        }
        res.pushBack(c);
        return Ui::hflow(res);
    };
}

Opt<Str> directInnerText(Dom::Element const& el) {
    if (not el.hasChildren())
        return NONE;
    if (el.countChildren() != 1)
        return NONE;
    if (auto text = el.firstChild()->as<Dom::Text>()) {
        auto data = text->data();
        if (Re::match(Re::zeroOrMore(Re::space()), data) == Match::YES)
            return Some(""s);
        if (data.len() > 64 or contains(data, "\n"s))
            return Some("…");
        return Some(data);
    }
    return NONE;
}

Ui::Child elementStartTag(Dom::Element const& el, bool expanded) {
    auto style = Ui::TextStyles::codeSmall().withColor(Ui::ACCENT500).withMultiline(false);
    auto prose = makeRc<Gfx::Prose>(style, style);
    prose->append("<"s);
    prose->append(Io::toStr(el.qualifiedName));

    prose->pushSpan(prose->currentSpanStyle().withColor(Ui::ACCENT400));

    for (auto const& attr : el.attributes) {
        prose->append(" "s);
        prose->append(Io::toStr(attr.qualifiedName));
        prose->append("=\""s);

        prose->pushSpan(prose->currentSpanStyle().withColor(Gfx::AMBER500));
        prose->append(attr.value);
        prose->popSpan();

        prose->append("\""s);
    }
    prose->popSpan();

    auto text = directInnerText(el);
    if (el.hasChildren() and text != ""s) {
        prose->append(">"s);
        if (not expanded) {
            prose->pushSpan(prose->currentSpanStyle().withColor(Ui::GRAY300));
            prose->append(text.unwrapOr("…"));
            prose->popSpan();

            prose->append("</"s);
            prose->append(Io::toStr(el.qualifiedName));
            prose->append(">"s);
        }
    } else {
        prose->append("/>"s);
    }

    return Ui::text(
        prose
    );
}

Ui::Child elementEndTag(Dom::Element const& el) {
    return Ui::text(
        Ui::TextStyles::codeSmall().withColor(Ui::ACCENT500),
        "</{}>", el.qualifiedName
    );
}

Str displayToBadge(Display d) {
    if (d == Display::GRID)
        return "grid";
    else if (d == Display::FLEX)
        return "flex";
    else if (d == Display::TABLE)
        return "grid";
    else
        return "";
}

Opt<Ui::Child> itemHeader(Gc::Ref<Dom::Node> n, Ui::Action<InspectAction> a, bool expanded) {
    if (n->is<Dom::Document>()) {
        return Some(Ui::codeMedium("#document"));
    } else if (n->is<Dom::DocumentType>()) {
        return Some(Ui::codeMedium("#document-type"));
    } else if (auto tx = n->as<Dom::Text>()) {
        auto data = tx->data();
        if (Re::match(Re::zeroOrMore(Re::space()), data) == Match::YES)
            return NONE;
        return Some(Ui::codeMedium(Ui::GRAY300, "{}", data));
    } else if (auto el = n->as<Dom::Element>()) {
        if (not el->hasChildren())
            return Some(elementStartTag(*el, false));

        auto displayBagde = displayToBadge(el->computedValues()->display);
        return Some(
            Ui::hflow(
                Ui::icon(
                    expanded ? Mdi::CHEVRON_DOWN : Mdi::CHEVRON_RIGHT
                ) |
                    Ui::button(
                        Some([n, a](auto& btn) {
                            a(btn, InspectExpandNode{n});
                        }),
                        Ui::ButtonStyle::subtle()
                    ),
                elementStartTag(*el, expanded),
                Kr::badge(Ui::GRAY500, displayBagde) | Ui::cond(displayBagde != "")
            )
        );
    } else if (auto comment = n->as<Dom::Comment>()) {
        return Some(Ui::codeMedium(Gfx::GREEN, "<!-- {} -->", comment->data()));
    } else {
        unreachable();
    }
}

Ui::Child itemFooter(Gc::Ref<Dom::Node> n, isize ident) {
    if (auto el = n->as<Dom::Element>())
        return Ui::hflow(n->countChildren() ? guide() : Ui::empty(), elementEndTag(*el)) | idented(ident);
    return Ui::empty();
}

Ui::ButtonStyle selected() {
    return {
        .idleStyle = {
            .backgroundFill = Some(Ui::GRAY800),
            .foregroundFill = Ui::GRAY300,
        },
        .hoverStyle = {
            .borderWidth = 1,
            .backgroundFill = Some(Ui::GRAY600),
        },
        .pressStyle = {
            .borderWidth = 1,
            .backgroundFill = Some(Ui::GRAY700),
        },
    };
}

Opt<Ui::Child> item(Gc::Ref<Dom::Node> n, InspectState const& s, Ui::Action<InspectAction> a, bool expanded, isize ident) {
    auto style = s.selectedNode == n ? selected() : Ui::ButtonStyle::subtle().withRadii(0);
    auto header = itemHeader(n, a, expanded);
    if (not header)
        return NONE;
    return Some(
        Ui::button(
            Some([n, a](auto& btn) {
                a(btn, InspectSelectNode{n});
            }),
            style,
            header.expect() | idented(ident)
        )
    );
}

Opt<Ui::Child> node(Gc::Ref<Dom::Node> n, InspectState const& s, Ui::Action<InspectAction> a, isize ident = 0) {
    bool expanded = n->is<Dom::Document>() or s.expandedNodes.contains(n);
    auto i = item(n, s, a, expanded, ident);
    if (not i)
        return NONE;

    Ui::Children children{i.expect()};
    if (expanded) {
        for (auto child = n->firstChild(); child; child = child->nextSibling()) {
            if (auto const& [item] = node(child.upgrade(), s, a, n->is<Dom::Document>() ? 0 : ident + 1))
                children.pushBack(item);
        }
        children.pushBack(itemFooter(n, ident));
    }
    return Some(Ui::vflow(children));
}

Ui::Child noNodeSelected() {
    return Ui::labelMedium("No element selected") |
           Ui::insets({8, 16}) |
           Ui::center();
}

Ui::Child inspectProperty() {
    auto style = Ui::TextStyles::codeSmall().withColor(Ui::ACCENT400);
    auto prose = makeRc<Gfx::Prose>(style, style);
    prose->append("margin"s);
    prose->pushSpan(prose->currentSpanStyle().withColor(Ui::GRAY300));
    prose->append(": {}"_f("0 0 0 0"s));
    prose->popSpan();
    return Ui::text(prose);
}

Ui::Child inspectStyleRule() {
    return Ui::vflow(
        Ui::vflow(
            Ui::hflow(Ui::codeSmall(".fs-italic {"s), Ui::grow(NONE), Ui::labelSmall("user agent"s)),
            Ui::vflow(
                inspectProperty(),
                inspectProperty(),
                inspectProperty(),
                inspectProperty()
            ) | Ui::insets({0, 0, 0, 16}),
            Ui::codeSmall("}"s)
        ) | Ui::insets(6),
        Kr::separator()
    );
}

Ui::Child inspectStyleTabCascaded(InspectState const& s) {
    auto content = noNodeSelected();

    if (s.selectedNode) {
        content = Ui::vflow(
                      inspectStyleRule(),
                      inspectStyleRule(),
                      inspectStyleRule(),
                      inspectStyleRule(),
                      inspectStyleRule(),
                      inspectStyleRule(),
                      inspectStyleRule(),
                      inspectStyleRule(),
                      inspectStyleRule()
                  ) |
                  Ui::vscroll();
    }

    return content |
           Kr::scaffoldContent();
}

Ui::Child inspectStyleTabComputed(Gc::Ref<Dom::Document> dom, InspectState const& s, Ui::Action<InspectAction> send) {
    auto content = noNodeSelected();

    if (s.selectedNode)
        if (auto const el = s.selectedNode->as<Dom::Element>()) {
            Ui::Children children;

            for (auto const& [name, registration] : dom->registeredPropertySet.registrations().iterItems()) {
                if (s.filter and startWith(name.str(), s.filter) == Match::NO)
                    continue;

                auto property = registration->load(*el->computedValues());
                auto style = Ui::TextStyles::codeSmall().withColor(Ui::ACCENT400);
                auto prose = makeRc<Gfx::Prose>(style, style);
                prose->append(name.str());
                prose->pushSpan(prose->currentSpanStyle().withColor(Ui::GRAY300));
                prose->append(": {}"_f(*property));
                prose->popSpan();

                children.pushBack(
                    Ui::text(prose) |
                    Ui::insets({0, 0, 0, 8})
                );
            }

            content = Ui::vflow(children) | Ui::vhscroll();
        }

    return Ui::vflow(
               Ui::hflow(
                   4,
                   Kr::input(Mdi::FILTER, "Filter..."s, s.filter, [send](auto& n, auto text) {
                       send(n, InspectChangeFilter{text});
                   }) | Ui::grow(),
                   Kr::checkbox(false, Ui::SINK<bool>, "All"s), Kr::checkbox(false, Ui::SINK<bool>, "Variables"s)
               ) | Ui::insets(6),
               content | Ui::grow()
           ) |
           Kr::scaffoldContent() | Ui::pinSize(128);
}

Ui::Child inspectStyleTabContent(Gc::Ref<Dom::Document> dom, InspectState const& s, Ui::Action<InspectAction> send) {
    switch (s.styleTab) {
    case InspectStyleTab::CASCADED:
        return inspectStyleTabCascaded(s);
    case InspectStyleTab::COMPUTED:
        return inspectStyleTabComputed(dom, s, send);
    default:
        unreachable();
    }
}

Ui::Child inspectStyleBoxInset(Str name, Gfx::Color color, Math::Insetsf, Ui::Child inner) {
    return Ui::stack(
               Ui::vflow(
                   0,
                   Math::Align::CENTER,
                   Ui::labelSmall("0"s) | Ui::insets(4),
                   Ui::hflow(
                       0,
                       Math::Align::CENTER,
                       Ui::labelSmall("0"s) | Ui::insets(4),
                       inner,
                       Ui::labelSmall("0"s) | Ui::insets(4)
                   ),
                   Ui::labelSmall("0"s) | Ui::insets(4)
               ),
               Ui::labelSmall(name) | Ui::bound() | Ui::insets(4)
           ) |
           Ui::box({
               .borderRadii = 2,
               .backgroundFill = Some(color),
           });
}

Ui::Child inspectStyleBoxModel() {
    return inspectStyleBoxInset(
               "margin",
               Gfx::YELLOW800, {},
               inspectStyleBoxInset(
                   "border"s,
                   Gfx::YELLOW600, {},
                   inspectStyleBoxInset(
                       "padding"s,
                       Gfx::GREEN600, {},
                       Ui::labelSmall("100×100"s) | Ui::center() | Ui::bound() |
                           Ui::box({
                               .padding = {4, 12},
                               .borderRadii = 2,
                               .backgroundFill = Some(Gfx::BLUE600),
                           }) |
                           Ui::minSize({100, Ui::UNCONSTRAINED})
                   )
               )
           ) |
           Ui::insets(16) |
           Ui::center() | Ui::bound() | Kr::scaffoldContent();
}

Ui::Child inspectStyleTab(Gc::Ref<Dom::Document> dom, InspectState const& s, Ui::Action<InspectAction> send) {
    Ui::Children items;
    items.pushBack(
        Ui::hflow(
            Kr::tabbarContent({
                Kr::tabbarItem(
                    s.styleTab == InspectStyleTab::CASCADED,
                    rbind(send, InspectStyleTab::CASCADED),
                    Kr::tabarItemLabel(Some(Mdi::FORMAT_LIST_GROUP), "Cascaded"s)
                ),
                Kr::tabbarItem(
                    s.styleTab == InspectStyleTab::COMPUTED,
                    rbind(send, InspectStyleTab::COMPUTED),
                    Kr::tabarItemLabel(Some(Mdi::VARIABLE), "Computed"s)
                ),
            }),
            Ui::grow(NONE),
            Ui::button(Some(rbind(send, InspectToggleBoxModel{})), Ui::ButtonStyle::subtle(), Mdi::SELECT_ALL)
        )
    );
    if (s.boxModel)
        items.pushBack(inspectStyleBoxModel());
    items.pushBack(inspectStyleTabContent(dom, s, send) | Ui::grow());

    return Ui::vflow(
        2,
        std::move(items)
    );
}

Ui::Child inspectTabElement(Rc<WebView> webview, InspectState const& s, Ui::Action<InspectAction> send) {
    auto document = webview->document().upgrade();
    return Ui::vflow(
        node(document, s, send).expect() | Ui::vhscroll() | Kr::scaffoldContent() | Ui::grow(),
        inspectStyleTab(document, s, send) | Kr::resizable(Kr::ResizeHandlePosition::TOP, {256}, NONE)
    );
}

Ui::Child _paperSelect(InspectState const& s, Ui::Action<InspectAction> send) {
    return Kr::select(Kr::selectValue(s.settings.stock.name), [send] -> Ui::Children {
        Vec<Ui::Child> groups;

        bool first = false;
        for (auto& serie : Print::SERIES) {
            Vec<Ui::Child> items;
            items.pushBack(Kr::selectLabel(serie.name));
            for (auto const& stock : serie.stocks) {
                items.pushBack(Kr::selectItem(Some(rbind(send, InspectChangePaper{stock})), stock.name));
            }

            if (not first)
                groups.pushBack(Kr::separator());
            groups.pushBack(Kr::selectGroup(std::move(items)));

            first = false;
        }

        return groups;
    });
}

Ui::Child inspectTabLayout(InspectState const& s, Ui::Action<InspectAction> send) {
    return Ui::vflow(
               4,
               Ui::vflow(
                   Kr::checkboxRow(
                       s.wireframe,
                       [send](auto& n, ...) {
                           send(n, InspectToggleWireframe{});
                       },
                       "Show wireframe"s
                   ),
                   Kr::checkboxRow(
                       true,
                       Ui::SINK<bool>,
                       "Show breaks"s
                   )
               ) | Kr::scaffoldContent()
           ) |
           Ui::vscroll() |
           Ui::grow();
}

Ui::Child inspectTabMedia(InspectState const& s, Ui::Action<InspectAction> send) {
    return Ui::vflow(
               4,
               Ui::vflow(
                   Kr::tabRow(
                       "Color Scheme"s,
                       {
                           Kr::tabbarItem(true, Ui::SINK<>, Kr::tabarItemLabel(NONE, "Auto"s)),
                           Kr::tabbarItem(false, Ui::SINK<>, Kr::tabarItemLabel(NONE, "Light"s)),
                           Kr::tabbarItem(false, Ui::SINK<>, Kr::tabarItemLabel(NONE, "Dark"s)),
                       }
                   ),
                   Kr::numberRow(
                       s.settings.scale,
                       [send](auto& n, f64 scale) {
                           send(n, InspectChangeScale{scale});
                       },
                       0.1,
                       "Scale"s
                   ),
                   Kr::tabRow(
                       "Flow"s,
                       {
                           Kr::tabbarItem(true, Ui::SINK<>, Kr::tabarItemLabel(NONE, "Continuous"s)),
                           Kr::tabbarItem(false, Ui::SINK<>, Kr::tabarItemLabel(NONE, "Paginated"s)),
                       }
                   )
               ) | Kr::scaffoldContent(),
               Ui::vflow(
                   Kr::selectRow(
                       Kr::selectValue(
                           s.settings.orientation == Print::Orientation::PORTRAIT
                               ? "Portrait"s
                               : "Landscape"s
                       ),
                       [send] -> Ui::Children {
                           return {
                               Kr::selectItem(Some(rbind(send, InspectChangeOrientation{Print::Orientation::PORTRAIT})), "Portrait"s),
                               Kr::selectItem(Some(rbind(send, InspectChangeOrientation{Print::Orientation::LANDSCAPE})), "Landscape"s),
                           };
                       },
                       "Orientation"s
                   ),
                   Kr::rowContent(
                       NONE,
                       "Paper"s,
                       NONE,
                       Some(_paperSelect(s, send))
                   ),
                   Kr::selectRow(
                       Kr::selectValue(Io::format("{}", Io::cased(s.settings.margins, Io::Case::CAPITAL))),
                       [send] -> Ui::Children {
                           return {
                               Kr::selectItem(Some(rbind(send, InspectChangeMargin{Print::MarginOption::NONE})), "None"s),
                               Kr::selectItem(Some(rbind(send, InspectChangeMargin{Print::MarginOption::MINIMUM})), "Minimum"s),
                               Kr::selectItem(Some(rbind(send, InspectChangeMargin{Print::MarginOption::DEFAULT})), "Default"s),
                               Kr::selectItem(Some(rbind(send, InspectChangeMargin{Math::InsetsAu{}})), "Custom"s),
                           };
                       },
                       "Margins"s
                   ),

                   Kr::checkboxRow(
                       s.settings.headerFooter,
                       [send](auto& n, ...) {
                           send(n, InspectToggleHeaderFooter{});
                       },
                       "Header and footers"s
                   )
               ) | Kr::scaffoldContent()
           ) |
           Ui::vscroll() |
           Ui::grow();
    ;
}

Ui::Child inspectTabContent(Rc<WebView> webview, InspectState const& s, Ui::Action<InspectAction> send) {
    switch (s.tab) {
    case InspectTab::ELEMENTS:
        return inspectTabElement(webview, s, send);
    case InspectTab::LAYOUT:
        return inspectTabLayout(s, send);
    case InspectTab::MEDIA:
        return inspectTabMedia(s, send);
    default:
        unreachable();
    }
}

export Ui::Child inspect(Rc<WebView> webview, InspectState const& s, Ui::Action<InspectAction> send) {
    return Ui::vflow(
        4,
        Ui::hflow(
            Kr::tabbarContent({
                Kr::tabbarItem(
                    s.tab == InspectTab::ELEMENTS,
                    rbind(send, InspectTab::ELEMENTS),
                    Kr::tabarItemLabel(Some(Mdi::CODE_TAGS), "Elements"s)
                ),
                Kr::tabbarItem(
                    s.tab == InspectTab::LAYOUT,
                    rbind(send, InspectTab::LAYOUT),
                    Kr::tabarItemLabel(Some(Mdi::RULER_SQUARE), "Layout"s)
                ),
                Kr::tabbarItem(
                    s.tab == InspectTab::MEDIA,
                    rbind(send, InspectTab::MEDIA),
                    Kr::tabarItemLabel(Some(Mdi::DEVICES), "Media"s)
                ),
            }),
            Ui::grow(NONE),
            Ui::button(
                Some(rbind(send, InspectToggleVisible{})),
                Ui::ButtonStyle::subtle(),
                Mdi::CLOSE
            )
        ),
        inspectTabContent(webview, s, send) | Ui::grow()
    );
}

} // namespace Vaev::Browser
