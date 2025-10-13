export module Vaev.Engine:layout.layout;

import Karm.Math;

import :layout.box;
import :layout.input;
import :layout.output;
import :layout.formating;

namespace Vaev::Layout {

export InsetsAu computeMargins(Tree& tree, Box& box, Input input);

export InsetsAu computeBorders(Tree& tree, Box& box);

export InsetsAu computePaddings(Tree& tree, Box& box, Vec2Au containingBlock);

export Math::Radii<Au> computeRadii(Tree& tree, Box& box, Vec2Au size);

export Opt<Au> computeSpecifiedSize(Tree& tree, Box& box, Size size, Vec2Au containingBlock, bool isWidth);

export Vec2Au computeIntrinsicSize(Tree& tree, Box& box, IntrinsicSize intrinsic, Vec2Au containingBlock);

export Output layout(Tree& tree, Box& box, Input input);

export Output layout(Tree& tree, Input input);

export Tuple<Output, Frag> layoutCreateFragment(Tree& tree, Input input);

export void fillKnownSizeWithSpecifiedSizeIfEmpty(Tree& tree, Box& box, Input& input);

} // namespace Vaev::Layout
