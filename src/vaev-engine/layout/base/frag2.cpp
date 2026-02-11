export module Vaev.Engine:layout.frag2;

import Karm.Core;
import Karm.Gfx;
import Karm.Gc;

import :dom.element;

using namespace Karm;

namespace Vaev::Layout {

struct Fragment {
    virtual Opt<Box&> box();

    virtual Gc::Ptr<Dom::Element> element();

    virtual void hitTest();

    virtual void paint(Gfx::Canvas& g);

    // https://www.w3.org/TR/SVG2/coords.html#TermObjectBoundingBox
    virtual RectAu objectBoundingBox();

    // https://www.w3.org/TR/SVG2/coords.html#TermStrokeBoundingBox
    virtual RectAu strokeBoundingBox();

    // https://www.w3.org/TR/SVG2/coords.html#TermDecoratedBoundingBox
    virtual RectAu decoratedBoundingBox();

    // https://drafts.csswg.org/css-overflow-3/#scrollable
    virtual RectAu scrollableOverflow();

    virtual void repr(Io::Emit& e) {}

    Rc<Gfx::Surface> snapshot() {}

    String svg(Math::Vec2i) {}
};

struct GroupFragment : Fragment {
    Vec<Rc<Fragment>> _children;

    void paint(Gfx::Canvas& g) override {
        for (auto& f : _children)
            f->paint(g);
    }

    void repr(Io::Emit& e) override {
        e("(group {})", _children);
    }
};

union Shape {
};

struct ShapeFragment : Fragment {
    Rc<Shape> _shape;
};

struct ProseFragment : Fragment {
    Gfx::Prose _prose;
};

struct ImageFragment : Fragment {
    Rc<Gfx::Surface> _surface;
};

// https://drafts.csswg.org/css-backgrounds/#background
struct BoxBackground {};

struct BoxMetrics {};

struct BoxFragment : Fragment {
    BoxMetrics _metrics;
    Vec<Rc<Fragment>> _children;
};

struct OverflowFragment : Fragment {};

struct StackFragment : Fragment {
    Vec<Rc<Fragment>> _children;

    void paint(Gfx::Canvas& g) override {
        for (auto& f : _children)
            f->paint(g);
    }
};

struct TransformFragment : Fragment {
    Math::Trans2f _tranform;
    Rc<Fragment> _content;

    void paint(Gfx::Canvas& g) override {
        _content->paint(g);
    }
};

struct TableGridFragment : Fragment {
};

struct PageFragment : Fragment {
    Rc<Fragment> _content;

    void paint(Gfx::Canvas& g) override {
        _content->paint(g);
    }
};

/// Represent the viewbox of an SVG
struct ViewboxFragment : Fragment {
};

/// Scrollable viewport, handle overflow too
struct ViewportFragment : Fragment {
    Rc<Fragment> _content;

    void paint(Gfx::Canvas& g) override {
        _content->paint(g);
    }
};

} // namespace Vaev::Layout