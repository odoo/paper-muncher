export module Vaev.Engine:layout.frag2;

import Karm.Core;
import Karm.Gfx;

using namespace Karm;

namespace Vaev::Layout {

struct Fragment {
    Opt<Box&> _box;

    virtual void hitTest();

    virtual void paint(Gfx::Canvas& g);

    virtual void add(Rc<Fragment>);

    // https://www.w3.org/TR/SVG2/coords.html#TermObjectBoundingBox
    virtual RectAu objectBoundingBox();

    // https://www.w3.org/TR/SVG2/coords.html#TermStrokeBoundingBox
    virtual RectAu strokeBoundingBox();

    // https://www.w3.org/TR/SVG2/coords.html#TermDecoratedBoundingBox
    virtual RectAu decoratedBoundingBox();

    // https://drafts.csswg.org/css-overflow-3/#scrollable
    virtual RectAu scrollableOverflow();

    virtual void repr(Io::Emit& e) {}
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
};

struct ProseFragment : Fragment {
    Gfx::Prose _prose;
};

struct ImageFragment : Fragment {
};

struct BoxMetrics {};

struct BoxFragment : Fragment {
    BoxMetrics _metrics;
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

struct ViewportFragment : Fragment {
    Rc<Fragment> _content;

    void paint(Gfx::Canvas& g) override {
        _content->paint(g);
    }
};

} // namespace Vaev::Layout