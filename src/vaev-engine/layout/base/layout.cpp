export module Vaev.Engine:layout.layout;

import Karm.Math;

import :layout.box;
import :layout.input;
import :layout.output;
import :layout.formating;

namespace Vaev::Layout {

export InsetsAu computeMargins(Tree& tree, Box& box, Vec2Au containingBlock);

export InsetsAu computeBorders(Tree& tree, Box& box);

export InsetsAu computePaddings(Tree& tree, Box& box, Vec2Au containingBlock);

export Math::Radii<Au> computeRadii(Tree& tree, Box& box, Vec2Au size);

export Opt<Au> computeSpecifiedBorderBoxWidth(Tree& tree, Box& box, Size size, Vec2Au containingBlock, Au horizontalBorderBox, Opt<Au> capmin = NONE);

export Opt<Au> computeSpecifiedBorderBoxHeight(Tree& tree, Box& box, Size size, Vec2Au containingBlock, Au verticalBorderBox);

export IntrinsicSizes computeIntrinsicInlineSizes(Tree& tree, Box& box);
export Au computeIntrinsicBlockSize(Tree& tree, Box& box, Au inlineSize);

export BoxMetrics computeBoxMetrics(Tree& tree, Box& box, Vec2Au position, Vec2Au size, UsedSpacings const& usedSpacings);

// MARK: Layout ---------------------------------------------------------------------

// Main function for laying out a box and its children.
export Output layoutContentBox(Tree& tree, Box& box, Input input);
export Output layoutBorderBox(Tree& tree, Box& box, Input input);

// Layout wrappers for root elements
export Output layoutRoot(Tree& tree, Input input);

// MARK: Frag Builder ----------------------------------------------------------------

export struct FragmentBuilder {
    Tree& _tree;
    Box& _box;
    Vec<Rc<Fragment>> _children;

    FragmentBuilder(Tree& tree, Box& box)
        : _tree(tree),
          _box(box) {}

    void addChild(Rc<Fragment> child) {
        _children.pushBack(child);
    }

    Rc<Fragment> buildSvgShape(SvgShape shape, Au strokeWidth) {
        return makeRc<SvgShapeFragment>(_box, shape, strokeWidth, std::move(_children));
    }

    Rc<Fragment> buildSvgGroup() {
        return makeRc<SvgGroupFragment>(_box, std::move(_children));
    }

    Rc<Fragment> buildSvgRoot(Math::Trans2f transf, RectAu boundingBox) {
        return makeRc<SvgRootFragment>(_box, transf, boundingBox, std::move(_children));
    }

    Rc<Fragment> buildBox(Vec2Au position, Vec2Au size, UsedSpacings usedSpacings) {
        auto metrics = computeBoxMetrics(_tree, _box, position, size, usedSpacings);
        return makeRc<BoxFragment>(_box, metrics, std::move(_children));
    }

    Opt<Rc<Fragment>> buildBoxFromInput(Input const& input, Vec2Au size) {
        if (not input.generateFragment)
            return NONE;

        return buildBox(input.position, size, input.usedSpacings);
    }
};


} // namespace Vaev::Layout
