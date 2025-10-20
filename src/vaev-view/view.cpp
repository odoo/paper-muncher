export module Vaev.View:view;

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
    Gc::Ptr<Dom::Node> selected = nullptr;
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

    void paint(Gfx::Canvas& g, Math::Recti rect) override {
        // Painting browser's viewport.
        g.push();
        g.clip(_listener.containerBound());
        g.origin(_listener.scroll() + _listener.containerBound().xy);

        auto paintRect = rect.offset(-_listener.containerBound().xy - _listener.scroll().cast<isize>());
        _window->render()->paint(g, paintRect.cast<f64>());

        if (_props.wireframe)
            Paint::wireframe(*_window->ensureRender().frag, g);

        if (_props.selected)
            Paint::overlay(*_window->ensureRender().frag, g, _props.selected.upgrade());

        g.pop();

        _listener.paint(g);
    }

    void event(App::Event& e) override {
        _listener.listen(*this, e);
    }

    void layout(Math::Recti bound) override {
        _listener.updateContainerBound(bound);
        _window->changeViewport(bound.size().cast<Au>());
        _listener.updateContentBound(_window->ensureRender().frag->scrollableOverflow().cast<isize>());
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
