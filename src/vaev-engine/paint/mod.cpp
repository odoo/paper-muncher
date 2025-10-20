export module Vaev.Engine:paint;

import Karm.Scene;
import Karm.Math;

import :layout.base;
import :layout.values;
import :values.transform;
import :values.background;
import :values.background;
import :style.specified;
export import :paint.debug;

namespace Vaev::Paint {

Math::Radiif _resolveRadii(Layout::Resolver& resolver, Math::Radii<CalcValue<PercentOr<Length>>> const& baseRadii, RectAu const& referenceBox);
Math::Vec2f _resolveBackgroundPosition(Layout::Resolver& resolver, BackgroundPosition const& position, RectAu const& referenceBox);
void _paintBackgroundColor(Layout::Frag& frag, Scene::Stack& stack);
void _paintBackgroundColor(Layout::Frag& frag, Scene::Stack& stack, Math::Rectf bound);

void _paintBorders(Layout::Frag& frag, Scene::Stack& stack);

void _paintReplaced(Layout::Frag& frag, Scene::Stack& stack);

Rc<Scene::Node> _applyClip(Layout::Frag const& frag, Rc<Scene::Node> content);

void _paintOutline(Layout::Frag& frag, Scene::Stack& stack);

// MARK: Stacking --------------------------------------------------------------

export struct Options {
    bool rootElement = false;
    Math::Rectf canvasBound = {};
    bool paintStackingContainer = false;
};

bool _requiresStackingContext(Style::SpecifiedValues const& s);
void _paintStackingContext(Layout::Frag& frag, Scene::Stack& stack, Options const& options = {});
void _paintStackingContextInternal(Layout::Frag& frag, Scene::Stack& stack, Options const& options);
void _establishStackingContext(Layout::Frag& frag, Scene::Stack& stack, Options const& options);

Rc<Scene::Node> _applyTransform(Style::TransformProps const& transform, RectAu referenceBox, Rc<Scene::Node> content);
Rc<Scene::Node> _applyTransform(Layout::Frag const& frag, Rc<Scene::Node> content);

Rc<Scene::Node> _paintSvg(Layout::SvgRootFrag& svgRoot, Gfx::Color currentColor);

// MARK: Public API ------------------------------------------------------------

export void paintDocument(Layout::Frag& root, Scene::Stack& canvas, Math::Rectf canvasBound);

} // namespace Vaev::Paint
