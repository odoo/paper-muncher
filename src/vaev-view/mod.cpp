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

export struct ViewportProps {
    bool wireframe = false;
    Opt<Dom::OriginatingElement> selected;
};

struct Viewport : Ui::View<Viewport> {
    Rc<Dom::Window> _window;
    ViewportProps _props;
    Ui::ScrollListener _listener;

    Viewport(Rc<Dom::Window> window, ViewportProps props)
        : _window(window), _props(props) {}

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
            render.fragments->paintWireframe(g);
        if (_props.selected)
            render.fragments->paintOverlay(g, _props.selected.unwrap(), _window->scrollableOverflow().cast<f64>());

        g.pop();

        _listener.paint(g);
    }

    void event(App::Event& e) override {
        _listener.listen(*this, e);
    }

    void layout(Math::Recti bound) override {
        _listener.updateContainerBound(bound);
        _window->changeViewport(bound.size().cast<Au>());
        _listener.updateContentBound(_window->scrollableOverflow().cast<isize>());
        Ui::View<Viewport>::layout(bound);
    }

    Math::Vec2i size(Math::Vec2i size, Ui::Hint hint) override {
        if (hint == Ui::Hint::MAX)
            return size;
        return {};
    }
};

export Ui::Child viewport(Rc<Dom::Window> window, ViewportProps props) {
    return makeRc<Viewport>(window, props);
}

} // namespace Vaev::View
