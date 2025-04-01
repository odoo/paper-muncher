#include "anim.h"

namespace marK::Ui {

// MARK: Slide In --------------------------------------------------------------

struct SlideIn : public ProxyNode<SlideIn> {
    SlideFrom _from;
    Easedf _slide{};

    SlideIn(SlideFrom from, Ui::Child child)
        : ProxyNode(std::move(child)),
          _from(from) {
    }

    Math::Vec2f outside() {
        switch (_from) {
        case SlideFrom::START:
            return {(f64)-bound().width, 0};

        case SlideFrom::END:
            return {(f64)bound().width, 0};

        case SlideFrom::TOP:
            return {0, (f64)-bound().height};

        case SlideFrom::BOTTOM:
            return {0, (f64)bound().height};
        }
    }

    auto translation() {
        return lerp(outside(), Math::Vec2f{}, _slide.value()).cast<isize>();
    }

    void paint(Gfx::Canvas& g, Math::Recti r) override {
        g.push();

        g.clip(bound());
        auto anim = translation();
        g.origin(anim.cast<f64>());
        r.xy = r.xy - anim;
        child().paint(g, r);

        g.pop();
    }

    void event(App::Event& e) override {
        if (_slide.needRepaint(*this, e)) {
            auto repaintBound =
                bound().clipTo(
                    child().bound().offset(translation())
                );

            Ui::shouldRepaint(*this, repaintBound);
        }

        Ui::ProxyNode<SlideIn>::event(e);
    }

    void attach(Node* parent) override {
        Ui::ProxyNode<SlideIn>::attach(parent);
        _slide.animate(*this, 1.0, 0.25, Math::Easing::cubicOut);
    }
};

Child slideIn(SlideFrom from, Ui::Child child) {
    return makeRc<SlideIn>(from, std::move(child));
}

// MARK: Scale In --------------------------------------------------------------

struct ScaleIn : public ProxyNode<ScaleIn> {
    Easedf _scale{};

    ScaleIn(Ui::Child child)
        : ProxyNode(std::move(child)) {
    }

    Math::Vec2f scale() {
        return Math::Vec2f{0.9} + Math::Vec2f{_scale.value() * 0.1};
    }

    void paint(Gfx::Canvas& g, Math::Recti r) override {
        g.push();
        g.clip(bound());
        g.origin(bound().center().cast<f64>());
        g.scale(scale());
        g.origin(-bound().center().cast<f64>());
        child().paint(g, r);
        g.pop();
    }

    void event(App::Event& e) override {
        if (_scale.needRepaint(*this, e))
            Ui::shouldRepaint(*this, bound());

        Ui::ProxyNode<ScaleIn>::event(e);
    }

    void attach(Node* parent) override {
        Ui::ProxyNode<ScaleIn>::attach(parent);
        _scale.animate(*this, 1.0, 0.25, Math::Easing::cubicOut);
    }
};

Child scaleIn(Child child) {
    return makeRc<ScaleIn>(std::move(child));
}

// MARK: Carousel --------------------------------------------------------------

struct Carousel : public GroupNode<Carousel> {
    usize _selected;
    Math::Flow _flow;
    Easedf _slide{};

    Carousel(usize selected, Children children, Math::Flow flow)
        : GroupNode(children), _selected(selected), _flow(flow) {
    }

    void reconcile(Carousel& o) override {
        GroupNode::reconcile(o);
        if (_selected != o._selected) {
            _selected = o._selected;
            _slide.animate(*this, _selected, 0.3, Math::Easing::cubicOut);
        }
    }

    Math::Vec2i translation() {
        return {
            (int)(-_slide.value() * bound().width),
            0,
        };
    }

    void paint(Gfx::Canvas& g, Math::Recti r) override {
        g.push();
        g.clip(bound());
        auto anim = translation();
        g.origin(anim.cast<f64>());
        for (auto& child : children()) {
            child->paint(g, r);
        }
        g.pop();
    }

    void event(App::Event& e) override {
        if (_slide.needRepaint(*this, e)) {
            Ui::shouldRepaint(*this, bound());
        }

        GroupNode::event(e);
    }

    void layout(Math::Recti r) override {
        _bound = r;
        for (auto& child : children()) {
            child->layout(r);
            r = r.offset({r.width, 0});
        }
    }
};

Child carousel(usize selected, Children children, Math::Flow flow) {
    return makeRc<Carousel>(selected, std::move(children), flow);
}

} // namespace marK::Ui
