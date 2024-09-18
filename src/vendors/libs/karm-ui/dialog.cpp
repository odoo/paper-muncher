#include "dialog.h"

#include "anim.h"
#include "funcs.h"

namespace Karm::Ui {

struct ShowDialogEvent {
    Child child;
};

void showDialog(Node &n, Child child) {
    bubble<ShowDialogEvent>(n, child);
}

struct CloseDialogEvent {
};

void closeDialog(Node &n) {
    bubble<CloseDialogEvent>(n);
}

struct DialogLayer : public LeafNode<DialogLayer> {
    Easedf _visibility{};

    Child _child;
    Opt<Child> _dialog;
    Opt<Child> _shouldShow;

    bool _shouldDialogClose = false;

    DialogLayer(Child child) : _child(child) {
        _child->attach(this);
    }

    ~DialogLayer() {
        if (_dialog)
            (*_dialog)->detach(this);
        _child->detach(this);
    }

    void _showDialog(Child child) {
        // We need to defer showing the dialog until the next frame,
        // otherwise replacing the dialog might cause some use after free down the tree
        _shouldShow = child;
        mouseLeave(*_child);
        shouldLayout(*this);
        _visibility.animate(*this, 1, 0.3, Math::Easing::exponentialOut);
    }

    void _closeDialog() {
        // We need to defer closing the dialog until the next frame,
        // otherwise we might cause some use after free down the tree
        _shouldDialogClose = true;
        shouldLayout(*this);
        _visibility.animate(*this, 0, 0.1);
    }

    void reconcile(DialogLayer &o) override {
        _child = _child->reconcile(o._child).unwrapOr(_child);
        _child->attach(this);
    }

    void paint(Gfx::Canvas &g, Math::Recti r) override {
        _child->paint(g, r);

        if (_visibility.value() > 0.001) {
            g.push();
            g.fillStyle(Ui::GRAY950.withOpacity(0.8 * _visibility.value()));
            g.fill(bound());
            g.pop();
        }

        if (_dialog) {
            g.push();
            // change the orgin to the center of the screen
            g.translate(bound().center().cast<f64>());
            g.scale(Math::lerp(0.9, 1, _visibility.value()));
            g.translate(-bound().center().cast<f64>());

            (*_dialog)->paint(g, r);

            g.pop();
        }
    }

    void event(App::Event &e) override {
        if (_visibility.needRepaint(*this, e))
            Ui::shouldRepaint(*this);

        auto *ke = e.is<App::KeyboardEvent>();

        if (ke and ke->type == App::KeyboardEvent::PRESS and ke->key == App::Key::ESC) {
            _closeDialog();
            e.accept();
        } else if (auto *se = e.is<ShowDialogEvent>()) {
            _showDialog(se->child);
            e.accept();
        } else if (e.is<CloseDialogEvent>()) {
            _closeDialog();
            e.accept();
        } else if (_dialog) {
            (*_dialog)->event(e);
        } else {
            _child->event(e);
        }
    }

    void bubble(App::Event &e) override {
        if (auto *se = e.is<ShowDialogEvent>()) {
            _showDialog(se->child);
            e.accept();
        } else if (e.is<CloseDialogEvent>()) {
            _closeDialog();
            e.accept();
        }

        LeafNode<DialogLayer>::bubble(e);
    }

    void layout(Math::Recti r) override {
        if (_shouldDialogClose) {
            if (_dialog) {
                (*_dialog)->detach(this);
                _dialog = NONE;
            }
            _shouldDialogClose = false;
        }

        if (_shouldShow) {
            if (_dialog) {
                (*_dialog)->detach(this);
            }
            _dialog = _shouldShow;
            (*_dialog)->attach(this);
            _shouldShow = NONE;
        }

        _child->layout(r);

        if (_dialog)
            (*_dialog)->layout(r);
    }

    Math::Vec2i size(Math::Vec2i s, Hint hint) override {
        return _child->size(s, hint);
    }

    Math::Recti bound() override {
        return _child->bound();
    }
};

Child dialogLayer(Child child) {
    return makeStrong<DialogLayer>(child);
}

} // namespace Karm::Ui