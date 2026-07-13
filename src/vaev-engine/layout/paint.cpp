export module Vaev.Engine:layout.paint;

import Karm.Core;
import Karm.Debug;
import Karm.Gc;
import Karm.Gfx;
import Karm.Image;
import Karm.Logger;
import Karm.Math;
import Karm.Scene;

import :style;
import :values;
import :layout.base;
import :layout.values;
import :dom.node;
import :dom.element;
import :layout.table;

namespace Vaev::Layout {

Opt<Gfx::Borders> buildBorders(BoxMetrics const& metrics, Style::ComputedValues const& style) {
    if (metrics.borders.zero())
        return NONE;

    Gfx::Borders borders;

    auto const& bordersLayout = metrics.borders;
    borders.widths.top = bordersLayout.top.cast<f64>();
    borders.widths.bottom = bordersLayout.bottom.cast<f64>();
    borders.widths.start = bordersLayout.start.cast<f64>();
    borders.widths.end = bordersLayout.end.cast<f64>();

    auto const& bordersStyle = *style.borders;
    borders.styles[0] = bordersStyle.top.style;
    borders.styles[1] = bordersStyle.end.style;
    borders.styles[2] = bordersStyle.bottom.style;
    borders.styles[3] = bordersStyle.start.style;

    borders.fills[0] = resolve(bordersStyle.top.color, style.color);
    borders.fills[1] = resolve(bordersStyle.end.color, style.color);
    borders.fills[2] = resolve(bordersStyle.bottom.color, style.color);
    borders.fills[3] = resolve(bordersStyle.start.color, style.color);

    return borders;
}

Opt<Gfx::Borders> buildBorders(UsedBorders const& border) {
    if (border.map(
                  [](auto b) {
                      return b.width;
                  }
        )
            .zero())
        return NONE;

    Gfx::Borders borders;

    borders.widths.top = border.top.width.cast<f64>();
    borders.widths.bottom = border.bottom.width.cast<f64>();
    borders.widths.start = border.start.width.cast<f64>();
    borders.widths.end = border.end.width.cast<f64>();

    borders.styles[0] = border.top.style;
    borders.styles[1] = border.end.style;
    borders.styles[2] = border.bottom.style;
    borders.styles[3] = border.start.style;

    borders.fills[0] = border.top.color;
    borders.fills[1] = border.end.color;
    borders.fills[2] = border.bottom.color;
    borders.fills[3] = border.start.color;

    return borders;
}

Opt<Gfx::Outline> buildOutline(BoxMetrics const& metrics, Style::ComputedValues const& style) {
    if (metrics.outlineWidth == 0_au)
        return NONE;

    Gfx::Outline outline;

    outline.width = metrics.outlineWidth.cast<f64>();
    outline.offset = metrics.outlineOffset.cast<f64>();

    auto const& outlineStyle = *style.outline;
    if (outlineStyle.style.is<Keywords::Auto>()) {
        outline.style = Gfx::BorderStyle::SOLID;
    } else {
        outline.style = outlineStyle.style.unwrap<Gfx::BorderStyle>();
    }

    outline.fill = resolve(outlineStyle.color, style.color);

    return outline;
}

static void _paintFragBordersAndBackgrounds(BoxFragment& frag, Scene::Stack& stack, Opt<UsedBorders> usedBorders = NONE) {
    auto const& cssBackground = frag.style().backgrounds;

    Vec<Gfx::Fill> backgrounds;
    auto color = resolve(cssBackground->color, frag.style().color);

    if (color.alpha != 0) {
        if (not frag.originatingBox().isRootElementPrincipalBox())
            backgrounds.pushBack(color);
    }

    auto bordersWithoutRadii = usedBorders
                                   ? buildBorders(*usedBorders)
                                   : buildBorders(frag.metrics, frag.style());
    auto outline = buildOutline(frag.metrics, frag.style());
    Math::Rectf bound = frag.borderBox().round().cast<f64>();

    if (any(backgrounds) or bordersWithoutRadii or outline) {
        // FIXME: In karm-scene, outline expects radii field to be set, even if we have zero-sized borders.
        auto borders = bordersWithoutRadii ? bordersWithoutRadii.take() : Gfx::Borders{};
        borders.radii = frag.metrics.radii.cast<f64>();

        auto box = makeRc<Scene::Box>(bound, std::move(borders), outline.unwrapOr(Gfx::Outline{}), std::move(backgrounds));
        box->zIndex = frag.originatingBox().impliesNewStackingContext() ? Limits<isize>::MIN : 0;
        if (frag.originatingBox().isRootElementPrincipalBox()) {
            stack.add(makeRc<Scene::Clear>(box, color));
        } else {
            stack.add(box);
        }
    }
}

static void _establishStackingContext(Rc<Fragment> frag, Scene::Stack& stack);
static void _paintStackingContext(Rc<Fragment> frag, Scene::Stack& stack);

Rc<Scene::Node> _paintSVGRoot(SvgRootFragment& svgRoot);
Rc<Scene::Stack> _paintSVGAggregate(Fragment& group, RectAu viewBox);

static RectAu _resolveTransformReferenceSVG(Fragment& svgFrag, RectAu viewBox, TransformBox box);
static Rc<Scene::Node> _applyTransform(Vaev::Style::TransformProps const& transform, RectAu referenceBox, Rc<Scene::Node> content);

// FIXME: move this closer to transform painting?
Opt<Rc<Scene::Node>> _applyTransformIfNeeded(Fragment& svgFrag, RectAu viewBox, Opt<Rc<Scene::Node>> content) {
    if (not content)
        return NONE;
    auto const& transform = *svgFrag.style().transform;
    if (not transform.has())
        return content;
    auto referenceBox = _resolveTransformReferenceSVG(svgFrag, viewBox, transform.box);
    return _applyTransform(transform, referenceBox, content.unwrap());
}

Opt<Gfx::Stroke> _resolveStroke(SvgShapeFragment& frag, SvgProps const& style) {
    if (Math::epsilonEq(style.strokeOpacity, 0.))
        return NONE;

    if (frag.strokeWidth == 0_au)
        return NONE;

    if (not style.stroke)
        return NONE;

    if (auto [color] = resolve(style.stroke, frag.style().color)) {
        color = color.withOpacity(style.strokeOpacity);
        if (color.transparent())
            return NONE;

        return Gfx::Stroke{color, static_cast<f64>(frag.strokeWidth)};
    }

    return NONE;
}

Opt<Gfx::Fill> _resolveFill(Fragment& frag, SvgProps const& style) {
    if (Math::epsilonEq(style.fillOpacity, 0.))
        return NONE;

    if (not style.fill)
        return NONE;

    if (auto [color] = resolve(style.fill, frag.style().color)) {
        color = color.withOpacity(style.fillOpacity);
        if (color.transparent())
            return NONE;
        return Gfx::Fill{color};
    }

    return NONE;
}

Rc<Scene::Node> _paintSvgRectangle(Math::Rectf rect, Opt<Gfx::Fill> fill, Opt<Gfx::Stroke> const& stroke) {
    Gfx::Borders borders;
    if (stroke) {
        borders = Gfx::Borders{
            Math::Radiif{},
            Math::Insetsf{stroke->width},
            Array<Gfx::Fill, 4>::fill(stroke->fill),
            Array<Gfx::BorderStyle, 4>::fill(Gfx::BorderStyle::SOLID),
        };

        // FIXME: needed due to mismatch between SVG's and Scene's box model
        // svg's stroke's center is at the shape's edges
        rect.width += stroke->width;
        rect.height += stroke->width;
        rect.x -= stroke->width / 2;
        rect.y -= stroke->width / 2;
    }

    Vec<Gfx::Fill> fills;
    if (fill)
        fills.pushBack(*fill);

    return makeRc<Scene::Box>(
        Math::Rectf{rect.x, rect.y, rect.width, rect.height},
        borders,
        Gfx::Outline{},
        fills
    );
}

Rc<Scene::Node> _paintSvgCircle(Math::Ellipsef circle, Opt<Gfx::Fill> fill, Opt<Gfx::Stroke> stroke) {
    Math::Path path;
    path.ellipse(circle);
    return makeRc<Scene::Shape>(path, stroke, fill);
}

Opt<Rc<Scene::Node>> _paintSvgShapeElement(SvgShapeFragment& frag) {
    auto const& style = *frag.originatingBox().style->svg;

    Opt<Gfx::Fill> resolvedFill = _resolveFill(frag, style);
    Opt<Gfx::Stroke> resolvedStroke = _resolveStroke(frag, style);

    if (not(resolvedFill or resolvedStroke))
        return NONE;

    if (auto rect = frag.shape.is<RectAu>()) {
        return _paintSvgRectangle(rect->cast<f64>(), resolvedFill, resolvedStroke);
    } else if (auto circle = frag.shape.is<EllipseAu>()) {
        return _paintSvgCircle(circle->cast<f64>(), resolvedFill, resolvedStroke);
    } else if (auto path = frag.shape.is<Math::Path>()) {
        return makeRc<Scene::Shape>(*path, resolvedStroke, resolvedFill);
    };
    unreachable();
}

Opt<Rc<Scene::Node>> _paintSVGElement(Rc<Fragment> element, RectAu viewBox) {
    if (auto shape = element.is<SvgShapeFragment>()) {
        return _applyTransformIfNeeded(
            *shape, viewBox,
            _paintSvgShapeElement(*shape)
        );
    } else if (auto nestedGroup = element.is<SvgGroupFragment>()) {
        return _applyTransformIfNeeded(
            *nestedGroup, viewBox,
            _paintSVGAggregate(*nestedGroup, viewBox)
        );
    } else if (auto nestedRoot = element.is<SvgRootFragment>()) {
        return _applyTransformIfNeeded(
            *nestedRoot, viewBox,
            makeRc<Scene::Clip>(
                _paintSVGRoot(*nestedRoot),
                nestedRoot->boundingBox.cast<f64>()
            )
        );
    } else if (auto const& [foreignObject] = element.cast<BoxFragment>()) {
        Scene::Stack stackForeignObj;
        _establishStackingContext(foreignObject, stackForeignObj);

        return makeRc<Scene::Clip>(
            makeRc<Scene::Stack>(std::move(stackForeignObj)),
            foreignObject->metrics.borderBox().cast<f64>()
        );
    }
    unreachable();
}

Rc<Scene::Stack> _paintSVGAggregate(Fragment& group, RectAu viewBox) {
    // NOTE: A SVG group does not create a stacking context, but its easier to manipulate a group if itself is its own node
    Scene::Stack stack;
    for (auto& element : group.children()) {
        if (auto [node] = _paintSVGElement(element, viewBox))
            stack.add(node);
    }
    return makeRc<Scene::Stack>(std::move(stack));
}

Rc<Scene::Node> _paintSVGRoot(SvgRootFragment& svgRoot) {
    auto rootBoxViewBox = svgRoot.originatingBox().style->svg->viewBox;

    // https://drafts.csswg.org/css-transforms/#transform-box
    // SPEC: The reference box is positioned at the origin of the coordinate system established
    // by the viewBox attribute.
    // NOTE: It is not clear whether '(SPEC) the nearest SVG viewport' includes the root's viewBox itself.
    // We assume it doesn't.
    RectAu viewBox =
        rootBoxViewBox
            ? Math::Rectf{rootBoxViewBox->width, rootBoxViewBox->height}.cast<Au>()
            : svgRoot.boundingBox;

    auto content = _paintSVGAggregate(svgRoot, viewBox);
    return makeRc<Scene::Transform>(content, svgRoot.transform);
}

static void _paintFrag(Rc<Fragment> frag, Scene::Stack& stack, Opt<UsedBorders> usedBorders = NONE) {
    auto& s = frag->style();

    if (s.visibility == Visibility::HIDDEN)
        return;

    if (auto boxFragment = frag.is<BoxFragment>()) {
        _paintFragBordersAndBackgrounds(*boxFragment, stack, usedBorders);

        if (auto prose = boxFragment->originatingBox().content.is<Rc<Gfx::Prose>>()) {
            stack.add(makeRc<Scene::Text>(boxFragment->contentBox().topStart().cast<f64>(), *prose));
        } else if (auto image = boxFragment->originatingBox().content.is<Rc<Scene::Node>>()) {
            auto bound = (*image)->bound();

            auto contentBox = boxFragment->contentBox().cast<f64>();
            auto trans = Math::Trans2f::map(bound, contentBox);
            Rc<Scene::Node> node = makeRc<Scene::Transform>(*image, trans);

            RadiiAu metricsRadii = boxFragment->metrics.radii;
            if (metricsRadii.zero()) {
                node = makeRc<Scene::Clip>(node, contentBox);
            } else {
                Math::Path path;
                path.rect(contentBox, metricsRadii.cast<f64>());
                node = makeRc<Scene::Clip>(node, std::move(path));
            }
            stack.add(node);
        }
    } else if (auto svgRoot = frag.is<SvgRootFragment>()) {
        if (frag->borderBox().wh.min() == 0_au)
            return;

        stack.add(
            makeRc<Scene::Clip>(
                _paintSVGRoot(*svgRoot),
                frag->contentBox().cast<f64>()
            )
        );
    }
}

static void _paintChildren(Fragment& frag, Scene::Stack& stack, auto predicate) {
    Opt<Map<usize, UsedBorders> const&> tableBoxBorderMapping;
    if (frag.style().display == Display::TABLE_BOX) {
        // FIXME: downcasting like this?
        TableFormatingContext const* tableFormattingContext = static_cast<TableFormatingContext const*>(&*frag.originatingBox().formatingContext.unwrap());
        tableBoxBorderMapping = tableFormattingContext->boxBorderMapping;
    }

    for (auto& c : frag.children()) {
        if (c->flags().has(Fragment::OOF))
            continue;

        Rc<Fragment> node = c;
        if (auto ph = c.is<PlaceholderFragment>()) {
            if (not ph->fragment)
                continue;
            node = ph->fragment.unwrap();
        }

        auto& s = node->style();

        if (node->originatingBox().impliesNewStackingContext()) {
            if (predicate(s))
                _establishStackingContext(node, stack);
            continue;
        }

        // NOTE: Positioned elements act as if they establish a stacking context
        auto position = s.position;
        if (position != Keywords::STATIC) {
            if (predicate(s))
                _paintStackingContext(node, stack);
            continue;
        }

        if (predicate(s)) {
            _paintFrag(
                node,
                stack,
                tableBoxBorderMapping ? (Opt<UsedBorders>)tableBoxBorderMapping->lookup((usize)&node->originatingBox()) : Opt<UsedBorders>{NONE}
            );
        }

        if (not c.is<SvgRootFragment>())
            _paintChildren(*node, stack, predicate);
    }
}

static Math::Radiif _resolveRadii(Resolver& resolver, Math::Radii<CalcValue<PercentOr<Length>>> const& baseRadii, RectAu const& referenceBox) {
    Math::Radiif radii;
    radii.a = resolver.resolve(baseRadii.a, referenceBox.height).cast<f64>();
    radii.b = resolver.resolve(baseRadii.b, referenceBox.width).cast<f64>();
    radii.c = resolver.resolve(baseRadii.c, referenceBox.width).cast<f64>();
    radii.d = resolver.resolve(baseRadii.d, referenceBox.height).cast<f64>();
    radii.e = resolver.resolve(baseRadii.e, referenceBox.height).cast<f64>();
    radii.f = resolver.resolve(baseRadii.f, referenceBox.width).cast<f64>();
    radii.g = resolver.resolve(baseRadii.g, referenceBox.width).cast<f64>();
    radii.h = resolver.resolve(baseRadii.h, referenceBox.height).cast<f64>();
    return radii;
}

// MARK: Background ------------------------------------------------------------

static Math::Vec2f _resolveBackgroundPosition(Resolver& resolver, BackgroundPosition const& position, RectAu const& referenceBox) {
    Math::Vec2f result;

    if (position.horizontalAnchor.is<Keywords::Left>()) {
        result.x = resolver.resolve(position.horizontal, referenceBox.width).cast<f64>();
    } else if (position.horizontalAnchor.is<Keywords::Right>()) {
        result.x = (referenceBox.width - resolver.resolve(position.horizontal, referenceBox.width)).cast<f64>();
    } else if (position.horizontalAnchor.is<Keywords::Center>()) {
        result.x = referenceBox.width.cast<f64>() / 2.0;
    }

    if (position.verticalAnchor.is<Keywords::Top>()) {
        result.y = resolver.resolve(position.vertical, referenceBox.height).cast<f64>();
    } else if (position.verticalAnchor.is<Keywords::Bottom>()) {
        result.y = (referenceBox.height - resolver.resolve(position.vertical, referenceBox.height)).cast<f64>();
    } else if (position.verticalAnchor.is<Keywords::Center>()) {
        result.y = referenceBox.height.cast<f64>() / 2.0;
    }

    return result;
}

// MARK: Clipping --------------------------------------------------------------

static Rc<Scene::Clip> _applyClip(Rc<Fragment> frag, Rc<Scene::Node> content) {
    Math::Path result;
    auto& clip = frag->style().clip->unwrap();

    RadiiAu metricsRadii = 0_au;
    if (auto boxFragment = frag.is<BoxFragment>())
        metricsRadii = boxFragment->metrics.radii;

    // TODO: handle SVG cases (https://www.w3.org/TR/css-masking-1/#typedef-geometry-box)
    auto [referenceBox, radii] = clip.referenceBox.visit(
        [&](Keywords::BorderBox const&) -> Pair<RectAu, RadiiAu> {
            return {frag->borderBox(), metricsRadii};
        },
        [&](Keywords::PaddingBox const&) -> Pair<RectAu, RadiiAu> {
            return {frag->paddingBox(), {0_au}};
        },
        [&](Keywords::ContentBox const&) -> Pair<RectAu, RadiiAu> {
            return {frag->contentBox(), {0_au}};
        },
        [&](Keywords::MarginBox const&) -> Pair<RectAu, RadiiAu> {
            return {frag->marginBox(), {0_au}};
        },
        [&](Keywords::FillBox const&) -> Pair<RectAu, RadiiAu> {
            return {frag->contentBox(), {0_au}};
        },
        [&](Keywords::StrokeBox const&) -> Pair<RectAu, RadiiAu> {
            return {frag->borderBox(), metricsRadii};
        },
        [&](Keywords::ViewBox const&) -> Pair<RectAu, RadiiAu> {
            return {frag->borderBox(), {0_au}};
        }
    );

    if (not clip.shape) {
        result.rect(referenceBox.round().cast<f64>(), radii.cast<f64>());
        return makeRc<Scene::Clip>(content, result);
    }

    auto resolver = Resolver();
    return clip.shape.unwrap().visit(
        [&](Polygon const& polygon) {
            result.moveTo(
                referenceBox.xy.cast<f64>() +
                Math::Vec2f(
                    resolver.resolve(first(polygon.points).v0, referenceBox.width).cast<f64>(),
                    resolver.resolve(first(polygon.points).v1, referenceBox.height).cast<f64>()
                )
            );
            for (auto& point : next(polygon.points)) {
                result.lineTo(
                    referenceBox.xy.cast<f64>() +
                    Math::Vec2f(
                        resolver.resolve(point.v0, referenceBox.width).cast<f64>(),
                        resolver.resolve(point.v1, referenceBox.height).cast<f64>()
                    )
                );
            }

            return makeRc<Scene::Clip>(content, result, polygon.fillRule);
        },
        [&](Circle const& circle) {
            auto center = _resolveBackgroundPosition(resolver, circle.position, referenceBox);
            f64 radius;
            if (circle.radius.is<Keywords::ClosestSide>()) {
                radius = min(
                    Math::abs(referenceBox.width.cast<f64>() - center.x),
                    center.x,
                    center.y,
                    Math::abs(referenceBox.height.cast<f64>() - center.y)
                );
            } else if (circle.radius.is<Keywords::FarthestSide>()) {
                radius = max(
                    Math::abs(referenceBox.width.cast<f64>() - center.x),
                    center.x,
                    center.y,
                    Math::abs(referenceBox.height.cast<f64>() - center.y)
                );
            } else {
                auto hSquared = Math::pow2(referenceBox.height.cast<f64>());
                auto wSquared = Math::pow2(referenceBox.width.cast<f64>());
                radius = resolver.resolve(
                                     circle.radius.unwrap<CalcValue<PercentOr<Length>>>(),
                                     Au(Math::sqrt(hSquared + wSquared) / Math::sqrt(2.0))
                )
                             .cast<f64>();
            }
            result.ellipse(Math::Ellipsef(center + referenceBox.xy.cast<f64>(), radius));

            return makeRc<Scene::Clip>(content, result);
        },
        [&](Inset const& inset) {
            Math::Insetsf resolved;
            resolved.start = resolver.resolve(inset.insets.start, referenceBox.width).cast<f64>();
            resolved.end = resolver.resolve(inset.insets.end, referenceBox.width).cast<f64>();
            resolved.top = resolver.resolve(inset.insets.top, referenceBox.height).cast<f64>();
            resolved.bottom = resolver.resolve(inset.insets.bottom, referenceBox.height).cast<f64>();

            Math::Radiif resolvedRadii = _resolveRadii(resolver, inset.borderRadius, referenceBox);

            result.rect(referenceBox.cast<f64>().shrink(resolved), resolvedRadii);

            return makeRc<Scene::Clip>(content, result);
        },
        [&](Xywh const& xywh) {
            Math::Rectf resolvedRect;
            resolvedRect.x = resolver.resolve(xywh.rect.x, referenceBox.width).cast<f64>();
            resolvedRect.y = resolver.resolve(xywh.rect.y, referenceBox.height).cast<f64>();
            resolvedRect.width = resolver.resolve(xywh.rect.width, referenceBox.width).cast<f64>();
            resolvedRect.height = resolver.resolve(xywh.rect.height, referenceBox.height).cast<f64>();

            Math::Radiif resolvedRadii = _resolveRadii(resolver, xywh.borderRadius, referenceBox);

            result.rect(resolvedRect.offset(referenceBox.xy.cast<f64>()), resolvedRadii);

            return makeRc<Scene::Clip>(content, result);
        },
        [&](Rect const& rect) {
            Math::Insetsf resolvedInsets;
            resolvedInsets.top = resolver.resolve(rect.insets.top, referenceBox.height).cast<f64>();
            resolvedInsets.end = resolver.resolve(rect.insets.end, referenceBox.width).cast<f64>();
            resolvedInsets.bottom = resolver.resolve(rect.insets.bottom, referenceBox.height).cast<f64>();
            resolvedInsets.start = resolver.resolve(rect.insets.start, referenceBox.width).cast<f64>();

            Math::Radiif resolvedRadii = _resolveRadii(resolver, rect.borderRadius, referenceBox);

            auto resultBox = referenceBox.cast<f64>();
            resultBox.width = max(resolvedInsets.end - resolvedInsets.start, 0);
            resultBox.height = max(resolvedInsets.bottom - resolvedInsets.top, 0);
            resultBox.x += resolvedInsets.start;
            resultBox.y += resolvedInsets.top;

            result.rect(resultBox, resolvedRadii);

            return makeRc<Scene::Clip>(content, result);
        },
        [&](Ellipse const& ellipse) {
            auto center = _resolveBackgroundPosition(resolver, ellipse.position, referenceBox);

            f64 rx;
            if (ellipse.rx.is<Keywords::ClosestSide>()) {
                rx = min(Math::abs(referenceBox.width.cast<f64>() - center.x), center.x);
            } else if (ellipse.rx.is<Keywords::FarthestSide>()) {
                rx = max(Math::abs(referenceBox.width.cast<f64>() - center.x), center.x);
            } else {
                rx = resolver.resolve(
                                 ellipse.rx.unwrap<CalcValue<PercentOr<Length>>>(),
                                 referenceBox.width
                )
                         .cast<f64>();
            }

            f64 ry;
            if (ellipse.ry.is<Keywords::ClosestSide>()) {
                ry = min(Math::abs(referenceBox.height.cast<f64>() - center.y), center.y);
            } else if (ellipse.ry.is<Keywords::FarthestSide>()) {
                ry = max(Math::abs(referenceBox.height.cast<f64>() - center.y), center.y);
            } else {
                ry = resolver.resolve(
                                 ellipse.ry.unwrap<CalcValue<PercentOr<Length>>>(),
                                 referenceBox.height
                )
                         .cast<f64>();
            }
            result.ellipse(Math::Ellipsef(center + referenceBox.xy.cast<f64>(), Math::Vec2f(rx, ry)));

            return makeRc<Scene::Clip>(content, result);
        },
        [&](Path const& path) {
            result.path(path.path);
            result.offset(referenceBox.xy.cast<f64>());
            return makeRc<Scene::Clip>(content, result, path.fillRule);
        }
    );
}

// MARK: Transformations -------------------------------------------------------

// https://www.w3.org/TR/css-transforms-1/#transform-box
static RectAu _resolveTransformReferenceSVG(Fragment& svgFrag, RectAu viewBox, TransformBox box) {
    // For SVG elements without associated CSS layout box, the used value
    // for content-box is fill-box and for border-box is stroke-box.
    return box.visit(
        [&](Keywords::ContentBox const&) {
            return svgFrag.objectBoundingBox();
        },
        [&](Keywords::BorderBox const&) {
            return svgFrag.strokeBoundingBox();
        },
        [&](Keywords::FillBox const&) {
            return svgFrag.objectBoundingBox();
        },
        [&](Keywords::StrokeBox const&) {
            return svgFrag.strokeBoundingBox();
        },
        [&](Keywords::ViewBox const&) {
            return viewBox;
        }
    );
}

// https://www.w3.org/TR/css-transforms-1/#transform-box
static RectAu _resolveTransformReferenceCSS(Fragment const& fragment, TransformBox box) {
    // For elements with associated CSS layout box, the used value for fill-box
    // is content-box and for stroke-box and view-box is border-box.
    return box.visit(
        [&](Keywords::ContentBox const&) {
            return fragment.contentBox();
        },
        [&](Keywords::BorderBox const&) {
            return fragment.borderBox();
        },
        [&](Keywords::FillBox const&) {
            return fragment.contentBox();
        },
        [&](Keywords::StrokeBox const&) {
            return fragment.borderBox();
        },
        [&](Keywords::ViewBox const&) {
            return fragment.borderBox();
        }
    );
}

static Vec2Au _resolveTransformOrigin(RectAu referenceBox, TransformOrigin origin) {
    Resolver resolver{};

    auto x = origin.xOffset.visit(
        [&](Keywords::Left) {
            return referenceBox.start();
        },
        [&](Keywords::Right) {
            return referenceBox.end();
        },
        [&](Keywords::Center) {
            return referenceBox.center().x;
        },
        [&](CalcValue<PercentOr<Length>> value) {
            return referenceBox.start() + resolver.resolve(value, referenceBox.width);
        }
    );

    auto y = origin.yOffset.visit(
        [&](Keywords::Top) {
            return referenceBox.top();
        },
        [&](Keywords::Bottom) {
            return referenceBox.bottom();
        },
        [&](Keywords::Center) {
            return referenceBox.center().y;
        },
        [&](CalcValue<PercentOr<Length>> value) {
            return referenceBox.top() + resolver.resolve(value, referenceBox.height);
        }
    );

    return {x, y};
}

static Math::Trans2f _resolveTransform(RectAu referenceBox, Vec2Au origin, Slice<TransformFunction> transforms) {
    auto result = Math::Trans2f::translate(
        origin.cast<f64>()
    );
    Resolver resolver{};

    for (auto const& transform : transforms) {
        auto trans = transform.visit(
            [&](MatrixTransform const& t) {
                return Math::Trans2f{
                    resolver.resolve(t.values[0]),
                    resolver.resolve(t.values[1]),
                    resolver.resolve(t.values[2]),
                    resolver.resolve(t.values[3]),
                    resolver.resolve(t.values[4]),
                    resolver.resolve(t.values[5]),
                };
            },
            [&](TranslateTransform const& t) {
                return Math::Trans2f::translate({
                    resolver.resolve(t.x, referenceBox.width).cast<f64>(),
                    resolver.resolve(t.y, referenceBox.height).cast<f64>(),
                });
            },
            [&](ScaleTransform const& t) {
                return Math::Trans2f::scale({
                    resolver.resolve(t.x),
                    resolver.resolve(t.y),
                });
            },
            [&](RotateTransform const& t) {
                return Math::Trans2f::rotate(resolver.resolve(t.value).value());
            },
            [&](SkewTransform const& t) {
                return Math::Trans2f::skew({
                    Math::tan(resolver.resolve(t.x).value()),
                    Math::tan(resolver.resolve(t.y).value()),
                });
            },
            [&](SkewXTransform const& t) {
                return Math::Trans2f::skew({
                    Math::tan(resolver.resolve(t.value).value()),
                    0,
                });
            },
            [&](SkewYTransform const& t) {
                return Math::Trans2f::skew({
                    0,
                    Math::tan(resolver.resolve(t.value).value()),
                });
            }
        );

        result = trans.multiply(result);
    }

    return Math::Trans2f::translate(-origin.cast<f64>()).multiply(result);
}

static Rc<Scene::Node> _applyTransform(Vaev::Style::TransformProps const& transform, RectAu referenceBox, Rc<Scene::Node> content) {
    auto origin = _resolveTransformOrigin(referenceBox, transform.origin);
    auto const& transformFunctions = transform.transform.unwrap<Vec<TransformFunction>>();
    auto trans = _resolveTransform(referenceBox, origin, transformFunctions);
    return makeRc<Scene::Transform>(content, trans);
}

static Rc<Scene::Node> _applyTransform(Fragment const& frag, Rc<Scene::Node> content) {
    auto const& transform = *frag.style().transform;
    auto referenceBox = _resolveTransformReferenceCSS(frag, transform.box);
    return _applyTransform(transform, referenceBox, content);
}

// MARK: Stacking Context ------------------------------------------------------

// 9.9 Layered presentation
// https://www.w3.org/TR/CSS22/visuren.html
static void _paintStackingContext(Rc<Fragment> frag, Scene::Stack& stack) {
    // 1. the background and borders of the element forming the stacking context.
    _paintFrag(frag, stack);

    if (frag.is<SvgRootFragment>())
        return;

    // 2. the child stacking contexts with negative stack levels (most negative first).
    _paintChildren(*frag, stack, [](Style::ComputedValues const& cv) -> bool {
        return cv.zIndex.unwrapOr<isize>(0) < 0;
    });

    // 3. the in-flow, non-inline-level, non-positioned descendants.
    _paintChildren(*frag, stack, [](Style::ComputedValues const& cv) {
        return cv.zIndex == Keywords::AUTO and cv.display != Display::INLINE and cv.position == Keywords::STATIC;
    });

    // 4. the non-positioned floats.
    _paintChildren(*frag, stack, [](Style::ComputedValues const& cv) {
        return cv.zIndex == Keywords::AUTO and cv.position == Keywords::STATIC and cv.float_ != Float::NONE;
    });

    // 5. the in-flow, inline-level, non-positioned descendants, including inline tables and inline blocks.
    _paintChildren(*frag, stack, [](Style::ComputedValues const& cv) {
        return cv.zIndex == Keywords::AUTO and cv.display == Display::INLINE and cv.position == Keywords::STATIC;
    });

    // 6. the child stacking contexts with stack level 0 and the positioned descendants with stack level 0.
    _paintChildren(*frag, stack, [](Style::ComputedValues const& cv) {
        return cv.zIndex.unwrapOr<isize>(0) == 0 and cv.position != Keywords::STATIC;
    });

    // 7. the child stacking contexts with positive stack levels (least positive first).
    _paintChildren(*frag, stack, [](Style::ComputedValues const& cv) {
        return cv.zIndex.unwrapOr<isize>(0) > 0;
    });
}

static void _establishStackingContext(Rc<Fragment> frag, Scene::Stack& stack) {
    Rc<Scene::Stack> innerStack = makeRc<Scene::Stack>();
    innerStack->zIndex = frag->style().zIndex.unwrapOr<isize>(0);
    _paintStackingContext(frag, *innerStack);

    Rc<Scene::Node> out = innerStack;
    if (frag->style().clip->has())
        out = _applyClip(frag, out);
    if (frag->style().transform->has())
        out = _applyTransform(*frag, out);
    if (frag->style().opacity != 1.0)
        out = makeRc<Scene::Opacity>(out, frag->style().opacity);

    stack.add(std::move(out));
}

export void paint(Rc<Fragment> frag, Scene::Stack& stack) {
    // The root element forms the root stacking context.
    _establishStackingContext(frag, stack);
}

} // namespace Vaev::Layout
