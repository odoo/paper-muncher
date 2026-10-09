export module Vaev.View;

import Karm.Gc;
import Karm.Print;
import Karm.Ui;
import Karm.Gfx;
import Karm.Math;
import Karm.Core;
import Karm.App;

import Vaev.Engine;

using namespace Karm;

namespace Vaev::View {

export using DispatchEvent = Ui::Send<Dom::Event&>;

export struct ViewportProps {
    bool wireframe = false;
    Opt<Dom::OriginatingElement> selected;
};

struct Viewport : Ui::View<Viewport> {
    Rc<WebView> _webview;
    DispatchEvent _dispatchEvent;
    ViewportProps _props;
    Ui::ScrollListener _listener;

    Viewport(Rc<WebView> webview, DispatchEvent dispatchEvent, ViewportProps props)
        : _webview(webview), _dispatchEvent(dispatchEvent), _props(props) {}

    void reconcile(Viewport& o) override {
        _webview = o._webview;
        _props = o._props;
    }

    void paint(Gfx::Canvas& g, Math::RectAu) override {
        g.push();
        g.clip(_listener.containerBound().cast<f64>());
        g.origin((_listener.scroll() + _listener.containerBound().xy).cast<f64>());
        _webview->paint(g);

        auto& render = _webview->ensureRender();

        if (_props.wireframe)
            render.stacking->paintWireframe(g, {});
        if (_props.selected)
            render.stacking->paintOverlay(g, _props.selected.expect(), _webview->scrollableOverflow().cast<f64>());

        g.pop();

        _listener.paint(g);
    }

    void event(App::Event& event) override {
        if (event.accepted())
            return;

        _listener.listen(*this, event);

        if (auto e = event.is<App::MouseEvent>()) {
            if (e->type == App::MouseEvent::PRESS and
                e->button == App::MouseButton::RIGHT and
                bound().contains(e->pos.cast<Au>())) {
                auto mousePosition = e->pos.cast<Au>() - bound().topStart() - _listener.scroll();

                auto hit = _webview->hittest(mousePosition.cast<f64>());
                Dom::MouseEvent domEvent{};
                domEvent.type = Dom::EventType::CONTEXTMENU;
                domEvent.target = hit->originatingElement().map(Dom::EventTarget::fromOriginatingElement);
                domEvent.screen = e->pos - bound().topStart().cast<isize>();
                _dispatchEvent(*this, domEvent);
                event.accept();
            }
        }
    }

    void layout(Math::RectAu bound) override {
        _listener.updateContainerBound(bound);
        _webview->changeViewport(bound.size());
        _listener.updateContentBound(_webview->scrollableOverflow());
        View::layout(bound);
    }

    Math::Vec2Au size(Math::Vec2Au size, Ui::Hint hint) override {
        if (hint == Ui::Hint::MAX)
            return size;
        return {};
    }
};

export Ui::Child viewport(Rc<WebView> webview, DispatchEvent dispatchEvent, ViewportProps props) {
    return makeRc<Viewport>(webview, dispatchEvent, props);
}

} // namespace Vaev::View
