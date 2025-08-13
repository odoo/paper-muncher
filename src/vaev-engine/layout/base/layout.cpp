export module Vaev.Engine:layout.layout;

import Karm.Math;

import :layout.box;
import :layout.input;
import :layout.output;
import :layout.formating;

namespace Vaev::Layout {

struct UsedSpacings {
    InsetsAu padding{};
    InsetsAu borders{};
    InsetsAu margin{};

    void repr(Io::Emit& e) const {
        e("(used spacings paddings: {} borders: {} margin: {})",
          padding, borders, margin);
    }
};

export InsetsAu computeMargins(Tree& tree, Box& box, Input input);

export InsetsAu computeBorders(Tree& tree, Box& box);

export InsetsAu computePaddings(Tree& tree, Box& box, Vec2Au containingBlock);

export Math::Radii<Au> computeRadii(Tree& tree, Box& box, Vec2Au size);

export Opt<Au> computeSpecifiedWidth(Tree& tree, Box& box, Size size, Vec2Au containingBlock);

export Opt<Au> computeSpecifiedHeight(Tree& tree, Box& box, Size size, Vec2Au containingBlock);

export Vec2Au computeIntrinsicContentSize(Tree& tree, Box& box, IntrinsicSize intrinsic);

// MARK: Layout ---------------------------------------------------------------------

// Main function for laying out a box and its children.
export Output layoutContentBox(Tree& tree, Box& box, Input input);

// Fragment/commit wrapper for content box layout
export Output layoutAndCommitContentBox(Tree& tree, Box& box, Input input, Frag& parentFrag, UsedSpacings const& usedSpacings);

// Border box wrappers for content box layout functions
export Output layoutBorderBox(Tree& tree, Box& box, Input input, UsedSpacings const& usedSpacings);
export Output layoutAndCommitBorderBox(Tree& tree, Box& box, Input input, Frag& parentFrag, UsedSpacings const& usedSpacings);

// Layout wrappers for root elements
export Output layoutRoot(Tree& tree, Input input);
export Tuple<Output, Frag> layoutAndCommitRoot(Tree& tree, Input input);

} // namespace Vaev::Layout
