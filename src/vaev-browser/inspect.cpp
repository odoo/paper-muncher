export module Vaev.Browser:inspect;

import Vaev.Engine;
import Karm.Kira;
import Karm.Ui;
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

export struct ExpandNode {
    Gc::Ref<Dom::Node> node;
};

export struct SelectNode {
    Gc::Ref<Dom::Node> node;
};

export struct ChangeFilter {
    String filter;
};

export using InspectorAction = Union<ExpandNode, SelectNode, ChangeFilter>;

export struct InspectState {
    String filter = ""s;
    Set<Gc::Ref<Dom::Node>> expandedNodes = {};
    Gc::Ptr<Dom::Node> selectedNode = nullptr;

    void apply(InspectorAction& a) {
        a.visit(
            [&](ExpandNode const& e) {
                if (not expandedNodes.remove(e.node))
                    expandedNodes.add(e.node);
            },
            [&](SelectNode const& e) {
                if (e.node->hasChildren())
                    expandedNodes.add(e.node);
                selectedNode = e.node;
            },
            [&](ChangeFilter const& f) {
                filter = f.filter;
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
    if (auto text = el.firstChild()->is<Dom::Text>()) {
        auto data = text->data();
        if (Re::match(Re::zeroOrMore(Re::space()), data) == Match::YES)
            return ""s;
        if (data.len() > 64 or contains(data, "\n"s))
            return "…";
        return data;
    }
    return NONE;
}

Ui::Child elementStartTag(Dom::Element const& el, bool expanded) {
    auto style = Ui::TextStyles::codeSmall().withColor(Ui::ACCENT500).withMultiline(false);
    auto prose = makeRc<Gfx::Prose>(style, style);
    prose->append("<"s);
    prose->append(Io::toStr(el.qualifiedName));

    prose->pushSpan(prose->currentSpanStyle().withColor(Ui::ACCENT400));

    for (auto [k, attr] : el.attributes.iterItems()) {
        prose->append(" "s);
        prose->append(Io::toStr(k));
        prose->append("=\""s);

        prose->pushSpan(prose->currentSpanStyle().withColor(Gfx::AMBER500));
        prose->append(attr->value);
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

Opt<Ui::Child> itemHeader(Gc::Ref<Dom::Node> n, Ui::Action<InspectorAction> a, bool expanded) {
    if (n->is<Dom::Document>()) {
        return Ui::codeMedium("#document");
    } else if (n->is<Dom::DocumentType>()) {
        return Ui::codeMedium("#document-type");
    } else if (auto tx = n->is<Dom::Text>()) {
        auto data = tx->data();
        if (Re::match(Re::zeroOrMore(Re::space()), data) == Match::YES)
            return NONE;
        return Ui::codeMedium(Ui::GRAY300, "{}", data);
    } else if (auto el = n->is<Dom::Element>()) {
        if (not el->hasChildren())
            return elementStartTag(*el, false);

        auto displayBagde = displayToBadge(el->computedValues()->display);
        return Ui::hflow(
            Ui::icon(
                expanded ? Mdi::CHEVRON_DOWN : Mdi::CHEVRON_RIGHT
            ) |
                Ui::button(
                    [n, a](auto& btn) {
                        a(btn, ExpandNode{n});
                    },
                    Ui::ButtonStyle::subtle()
                ),
            elementStartTag(*el, expanded),
            Kr::badge(Ui::GRAY500, displayBagde) | Ui::cond(displayBagde != "")
        );
    } else if (auto c = n->is<Dom::Comment>()) {
        return Ui::codeMedium(Gfx::GREEN, "<!-- {} -->", c->data());
    } else {
        unreachable();
    }
}

Ui::Child itemFooter(Gc::Ref<Dom::Node> n, isize ident) {
    if (auto el = n->is<Dom::Element>())
        return Ui::hflow(n->countChildren() ? guide() : Ui::empty(), elementEndTag(*el)) | idented(ident);
    return Ui::empty();
}

Ui::ButtonStyle selected() {
    return {
        .idleStyle = {
            .backgroundFill = Ui::GRAY800,
            .foregroundFill = Ui::GRAY300,
        },
        .hoverStyle = {
            .borderWidth = 1,
            .backgroundFill = Ui::GRAY600,
        },
        .pressStyle = {
            .borderWidth = 1,
            .backgroundFill = Ui::GRAY700,
        },
    };
}

Opt<Ui::Child> item(Gc::Ref<Dom::Node> n, InspectState const& s, Ui::Action<InspectorAction> a, bool expanded, isize ident) {
    auto style = s.selectedNode == n ? selected() : Ui::ButtonStyle::subtle().withRadii(0);
    auto header = itemHeader(n, a, expanded);
    if (not header)
        return NONE;
    return Ui::button(
        [n, a](auto& btn) {
            a(btn, SelectNode{n});
        },
        style,
        header.unwrap() | idented(ident)
    );
}

Opt<Ui::Child> node(Gc::Ref<Dom::Node> n, InspectState const& s, Ui::Action<InspectorAction> a, isize ident = 0) {
    bool expanded = n->is<Dom::Document>() or s.expandedNodes.contains(n);
    auto i = item(n, s, a, expanded, ident);
    if (not i)
        return NONE;

    Ui::Children children{i.unwrap()};
    if (expanded) {
        for (auto child = n->firstChild(); child; child = child->nextSibling()) {
            if (auto [item] = node(child.upgrade(), s, a, n->is<Dom::Document>() ? 0 : ident + 1))
                children.pushBack(item);
        }
        children.pushBack(itemFooter(n, ident));
    }
    return Ui::vflow(children);
}

Ui::Child computedStyles(Gc::Ref<Dom::Document> dom, InspectState const& s, Ui::Action<InspectorAction> send) {
    auto content = Ui::labelMedium("No element selected") |
                   Ui::insets({8, 16}) |
                   Ui::center();

    if (s.selectedNode)
        if (auto const el = s.selectedNode->is<Dom::Element>()) {
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
                    Ui::insets({4, 8})
                );
            }

            content = Ui::vflow(children) | Ui::vhscroll();
        }

    return Ui::vflow(
               Kr::sidePanelTitle("Computed Styles") | Ui::dragRegion(),
               Kr::separator(),
               Kr::input(Mdi::FILTER, "Filter..."s, s.filter, [send](auto& n, auto text) {
                   send(n, ChangeFilter{text});
               }),
               content | Ui::grow()
           ) |
           Ui::pinSize(128);
}

export Ui::Child inspect(Rc<Dom::Window> window, InspectState const& s, Ui::Action<InspectorAction> send) {
    auto document = window->document().upgrade();
    return Ui::vflow(
        node(document, s, send).unwrap() | Ui::vhscroll() | Kr::scaffoldContent() | Ui::grow(),
        computedStyles(document, s, send) | Kr::scaffoldContent() | Kr::resizable(Kr::ResizeHandlePosition::TOP, {256}, NONE)
    );
}

} // namespace Vaev::Browser
