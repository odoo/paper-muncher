module;

#include <karm-math/au.h>
#include <karm-math/rect.h>

export module Vaev.Engine:paint;

import Karm.Scene;

import :layout.base;
import :layout.values;
import :values.transform;

namespace Vaev::Paint {

Math::Radiif _resolveRadii(Layout::Resolver& resolver, Math::Radii<CalcValue<PercentOr<Length>>> const& baseRadii, RectAu const& referenceBox);
Math::Vec2f _resolveBackgroundPosition(Layout::Resolver& resolver, BackgroundPosition const& position, RectAu const& referenceBox);
void _paintBackgroundColor(Layout::Frag& frag, Scene::Stack& stack);
void _paintBackgroundColor(Layout::Frag& frag, Scene::Stack& stack, Math::Rectf bound);

void _paintBorders(Layout::Frag& frag, Scene::Stack& stack);

void _paintInlineLevel(Layout::Frag& frag, Scene::Stack& stack);
void _paintBlockLevel(Layout::Frag& frag, Scene::Stack& stack);

Rc<Scene::Node> _applyClip(Layout::Frag const& frag, Rc<Scene::Node> content);

void _paintOutline(Layout::Frag& frag, Scene::Stack& stack);

export struct Options {
    Math::Rectf canvasBound;
    bool rootElement = false;
};

export void _paintStackingContext(Layout::Frag& frag, Scene::Stack& stack, Options const& options = {});
export void _paintStackingContextInternal(Layout::Frag& frag, Scene::Stack& stack, Options const& options);
export void _establishStackingContext(Layout::Frag& frag, Scene::Stack& stack, Options const& options);

Rc<Scene::Node> _applyTransform(Style::TransformProps const& transform, RectAu referenceBox, Rc<Scene::Node> content);
Rc<Scene::Node> _applyTransform(Layout::Frag const& frag, Rc<Scene::Node> content);

Rc<Scene::Node> _paintSvg(Layout::SVGRootFrag& svgRoot, Gfx::Color currentColor);

// MARK: Public API ------------------------------------------------------------

export void paint(Layout::Frag& frag, Scene::Stack& stack, Math::Rectf canvasBound);

} // namespace Vaev::Paint