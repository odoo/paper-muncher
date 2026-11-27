#include <karm/test>

import Karm.Gc;
import Karm.Ref;
import Vaev.Engine;

using namespace Karm;
using namespace Karm::Literals;
using namespace Karm::Ref::Literals;

namespace Vaev::Style::Tests {

static Res<bool> _matches(Str selector, Gc::Ref<Dom::Element> el, bool inHtmlDocument) {
    auto selectors = try$(parseSelector(selector));
    return Ok(match(selectors, MatchSubject{el, NONE, inHtmlDocument}) != NONE);
}

test$("select-case-sensitivity-of-selectors") {
    Gc::Heap gc;
    auto make = [&](Dom::QualifiedName tag, Gc::Ptr<Dom::Element> parent = nullptr) {
        auto el = gc.alloc<Dom::Element>(tag);
        if (parent)
            parent->appendChild(el);
        return el;
    };

    auto div = make(Html::DIV_TAG);
    div->setAttribute(Dom::QualifiedName{NONE, "data-x"_sym}, "foo"s);
    auto span = make(Html::SPAN_TAG, div);
    auto svg = make(Svg::SVG_TAG, div);
    svg->setAttribute(Svg::VIEW_BOX_ATTR, "0 0 1 1"s);
    auto foreignObject = make(Svg::FOREIGN_OBJECT_TAG, svg);

    assert$(try$(_matches("DIV", div, true)));
    assert$(try$(_matches("[DATA-X]", div, true)));
    assert$(try$(_matches("[DATA-X=foo]", div, true)));
    assert$(try$(_matches("DIV > SPAN", span, true)));
    assert$(try$(_matches("DIV:has(> SPAN)", div, true)));
    assert$(try$(_matches(":nth-child(1 of SPAN)", span, true)));
    assert$(try$(_matches("foreignObject", foreignObject, true)));
    assert$(try$(_matches("[viewBox]", svg, true)));

    assertNot$(try$(_matches("[data-x=FOO]", div, true)));
    assertNot$(try$(_matches("FOREIGNOBJECT", foreignObject, true)));
    assertNot$(try$(_matches("foreignobject", foreignObject, true)));
    assertNot$(try$(_matches("[viewbox]", svg, true)));

    assertNot$(try$(_matches("DIV", div, false)));
    assertNot$(try$(_matches("[DATA-X]", div, false)));
    assertNot$(try$(_matches("DIV > SPAN", span, false)));

    return Ok();
}

static Async::Task<Gc::Ref<Dom::Element>> _styledElementAsync(Rc<WebView> webview, Ref::Mime mime, Str source, Str id, Async::CancellationToken ct) {
    co_trya$(webview->loadLocationAsync(Ref::Url::data(mime, bytes(source)), Ref::Uti::PUBLIC_OPEN, ct));
    webview->ensureRender();
    for (auto node : webview->document().upgrade()->iterDepthFirst())
        if (auto el = node->as<Dom::Element>(); el and el->id() == id)
            co_return Ok(el.upgrade());
    co_return Error::notFound("element not found");
}

testAsync$("select-case-sensitivity-in-documents") {
    auto source = R"(<html xmlns="http://www.w3.org/1999/xhtml"><head><style>
        DIV P[DATA-X] { display: none; }
    </style></head><body><div><p id="p" data-x="">x</p></div></body></html>)"s;

    auto htmlView = WebView::create();
    auto htmlP = co_trya$(_styledElementAsync(htmlView, "text/html"_mime, source, "p", ct));
    co_assert$(htmlP->computedValues()->display == Display::NONE);

    auto xhtmlView = WebView::create();
    auto xhtmlP = co_trya$(_styledElementAsync(xhtmlView, "application/xhtml+xml"_mime, source, "p", ct));
    co_assertNot$(xhtmlP->computedValues()->display == Display::NONE);

    co_return Ok();
}

} // namespace Vaev::Style::Tests
