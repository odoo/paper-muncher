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
    Rc<Dom::Window> _window;
    DispatchEvent _dispatchEvent;
    ViewportProps _props;
    Ui::ScrollListener _listener;

    Viewport(Rc<Dom::Window> window, DispatchEvent dispatchEvent, ViewportProps props)
        : _window(window), _dispatchEvent(dispatchEvent), _props(props) {}

    void reconcile(Viewport& o) override {
        _window = o._window;
        _props = o._props;
    }

    void paint(Gfx::Canvas& g, Math::Recti) override {
        g.push();
        g.clip(_listener.containerBound());
        g.origin(_listener.scroll() + _listener.containerBound().xy);
        _window->paint(g);

        auto& render = _window->ensureRender();

        if (_props.wireframe)
            render.stacking->paintWireframe(g, {});
        if (_props.selected)
            render.stacking->paintOverlay(g, _props.selected.unwrap(), _window->scrollableOverflow().cast<f64>());

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
                bound().contains(e->pos)) {
                auto mousePosition = e->pos - bound().topStart() - _listener.scroll().cast<isize>();

                auto hit = _window->hittest(mousePosition.cast<f64>());
                Dom::MouseEvent domEvent{};
                domEvent.type = Dom::EventType::CONTEXTMENU;
                domEvent.target = hit->originatingElement().map(Dom::EventTarget::fromOriginatingElement);
                domEvent.screen = e->pos - bound().topStart();
                _dispatchEvent(*this, domEvent);
                event.accept();
            }
        }
    }

    void layout(Math::Recti bound) override {
        _listener.updateContainerBound(bound);
        _window->changeViewport(bound.size().cast<Au>());
        _listener.updateContentBound(_window->scrollableOverflow().cast<isize>());
        View::layout(bound);
    }

    Math::Vec2i size(Math::Vec2i size, Ui::Hint hint) override {
        if (hint == Ui::Hint::MAX)
            return size;
        return {};
    }
};

export Ui::Child viewport(Rc<Dom::Window> window, DispatchEvent dispatchEvent, ViewportProps props) {
    return makeRc<Viewport>(window, dispatchEvent, props);
}

} // namespace Vaev::View
