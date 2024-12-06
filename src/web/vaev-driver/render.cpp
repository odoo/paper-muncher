#include <karm-scene/page.h>
#include <karm-scene/stack.h>
#include <karm-sys/time.h>
#include <vaev-layout/builder.h>
#include <vaev-layout/layout.h>
#include <vaev-layout/paint.h>
#include <vaev-layout/positioned.h>
#include <vaev-layout/values.h>
#include <vaev-markup/dom.h>
#include <vaev-style/computer.h>

#include "fetcher.h"
#include "render.h"

namespace Vaev::Driver {

static constexpr bool DEBUG_RENDER = false;

static void _collectStyle(Markup::Node const &node, Style::StyleBook &sb) {
    auto el = node.is<Markup::Element>();
    if (el and el->tagName == Html::STYLE) {
        auto text = el->textContent();
        Io::SScan textScan{text};
        auto sheet = Style::StyleSheet::parse(textScan);
        sb.add(std::move(sheet));
    } else if (el and el->tagName == Html::LINK) {
        auto rel = el->getAttribute(Html::REL_ATTR);
        if (rel == "stylesheet"s) {
            auto href = el->getAttribute(Html::HREF_ATTR);
            if (not href) {
                logWarn("link element missing href attribute");
                return;
            }

            auto url = Mime::parseUrlOrPath(*href);
            if (not url) {
                logWarn("link element href attribute is not a valid URL: {}", *href);
                return;
            }

            auto sheet = fetchStylesheet(url.take(), Style::Origin::AUTHOR);
            if (not sheet) {
                logWarn("failed to fetch stylesheet: {}", sheet);
                return;
            }

            sb.add(sheet.take());
        }
    } else {
        for (auto &child : node.children())
            _collectStyle(*child, sb);
    }
}

RenderResult render(Markup::Document const &dom, Style::Media const &media, Layout::Viewport viewport) {
    Style::StyleBook stylebook;
    stylebook.add(
        fetchStylesheet("bundle://vaev-driver/html.css"_url, Style::Origin::USER_AGENT)
            .take("user agent stylesheet not available")
    );

    auto start = Sys::now();
    _collectStyle(dom, stylebook);
    auto elapsed = Sys::now() - start;
    logDebugIf(DEBUG_RENDER, "style collection time: {}", elapsed);

    start = Sys::now();

    Style::Computer computer{media, stylebook};
    Layout::Tree tree = {
        Layout::build(computer, dom),
        viewport,
    };

    elapsed = Sys::now() - start;

    logDebugIf(DEBUG_RENDER, "layout tree build time: {}", elapsed);

    start = Sys::now();

    elapsed = Sys::now() - start;

    logDebugIf(DEBUG_RENDER, "layout tree measure time: {}", elapsed);

    start = Sys::now();

    auto [outDiscovery, root] = Layout::layoutCreateFragment(
        tree,
        {
            .knownSize = {viewport.small.width, NONE},
            .availableSpace = {viewport.small.width, 0_px},
            .containingBlock = {viewport.small.width, viewport.small.height},
        }
    );

    auto sceneRoot = makeStrong<Scene::Stack>();

    elapsed = Sys::now() - start;
    logDebugIf(DEBUG_RENDER, "layout tree layout time: {}", elapsed);

    auto paintStart = Sys::now();
    Layout::paint(root, *sceneRoot);
    sceneRoot->prepare();

    elapsed = Sys::now() - paintStart;
    logDebugIf(DEBUG_RENDER, "layout tree paint time: {}", elapsed);

    return {
        std::move(stylebook),
        makeStrong<Layout::Box>(std::move(tree.root)),
        sceneRoot,
        makeStrong<Layout::Frag>(root)
    };
}

static void _paintMargins(Style::PageComputedStyle &pageStyle, RectPx pageRect, RectPx pageContent, Scene::Stack &stack) {
    // MARK: Top Left Corner ---------------------------------------------------

    auto topLeftMarginCornerRect = RectPx::fromTwoPoint(
        pageRect.topStart(),
        pageContent.topStart()
    );
    Layout::Tree topLeftMarginCornerTree{
        .root = Layout::buildForPseudoElement(pageStyle.area(Style::PageArea::TOP_LEFT_CORNER)),
        .viewport = Layout::Viewport{.small = topLeftMarginCornerRect.size()}
    };

    auto [_, topLeftMarginCornerFrag] = Layout::layoutCreateFragment(
        topLeftMarginCornerTree,
        {
            .knownSize = topLeftMarginCornerRect.size().cast<Opt<Px>>(),
            .position = topLeftMarginCornerRect.topStart(),
            .availableSpace = topLeftMarginCornerRect.size(),
            .containingBlock = topLeftMarginCornerRect.size(),
        }
    );
    Layout::paint(topLeftMarginCornerFrag, stack);
    // MARK: Top Right Corner --------------------------------------------------

    auto topRightMarginCornerRect = RectPx::fromTwoPoint(
        pageRect.topEnd(),
        pageContent.topEnd()
    );
    Layout::Tree topRightMarginCornerTree{
        .root = Layout::buildForPseudoElement(pageStyle.area(Style::PageArea::TOP_RIGHT_CORNER)),
        .viewport = Layout::Viewport{.small = topRightMarginCornerRect.size()}
    };

    auto [_, topRightMarginCornerFrag] = Layout::layoutCreateFragment(
        topRightMarginCornerTree,
        {
            .knownSize = topRightMarginCornerRect.size().cast<Opt<Px>>(),
            .position = topRightMarginCornerRect.topStart(),
            .availableSpace = topRightMarginCornerRect.size(),
            .containingBlock = topRightMarginCornerRect.size(),
        }
    );
    Layout::paint(topRightMarginCornerFrag, stack);

    // MARK: Bottom Left Corner ------------------------------------------------

    auto bottomLeftMarginCornerRect = RectPx::fromTwoPoint(
        pageRect.bottomStart(),
        pageContent.bottomStart()
    );
    Layout::Tree bottomLeftMarginCornerTree{
        .root = Layout::buildForPseudoElement(pageStyle.area(Style::PageArea::BOTTOM_LEFT_CORNER)),
        .viewport = Layout::Viewport{.small = bottomLeftMarginCornerRect.size()}
    };

    auto [_, bottomLeftMarginCornerFrag] = Layout::layoutCreateFragment(
        bottomLeftMarginCornerTree,
        {
            .knownSize = bottomLeftMarginCornerRect.size().cast<Opt<Px>>(),
            .position = bottomLeftMarginCornerRect.topStart(),
            .availableSpace = bottomLeftMarginCornerRect.size(),
            .containingBlock = bottomLeftMarginCornerRect.size(),
        }
    );
    Layout::paint(bottomLeftMarginCornerFrag, stack);

    // MARK: Bottom Right Corner -----------------------------------------------

    auto bottomRightMarginCornerRect = RectPx::fromTwoPoint(
        pageRect.bottomEnd(),
        pageContent.bottomEnd()
    );
    Layout::Tree bottomRightMarginCornerTree{
        .root = Layout::buildForPseudoElement(pageStyle.area(Style::PageArea::BOTTOM_RIGHT_CORNER)),
        .viewport = Layout::Viewport{.small = bottomRightMarginCornerRect.size()}
    };

    auto [_, bottomRightMarginCornerFrag] = Layout::layoutCreateFragment(
        bottomRightMarginCornerTree,
        {
            .knownSize = bottomRightMarginCornerRect.size().cast<Opt<Px>>(),
            .position = bottomRightMarginCornerRect.topStart(),
            .availableSpace = bottomRightMarginCornerRect.size(),
            .containingBlock = bottomRightMarginCornerRect.size(),
        }
    );
    Layout::paint(bottomRightMarginCornerFrag, stack);

    // MARK: Top ---------------------------------------------------------------

    auto topRect = RectPx::fromTwoPoint(
        topLeftMarginCornerRect.topEnd(),
        topRightMarginCornerRect.bottomStart()
    );

    auto topBox = Layout::buildForPseudoElement(pageStyle.area(Style::PageArea::TOP));
    topBox.add(Layout::buildForPseudoElement(pageStyle.area(Style::PageArea::TOP_LEFT)));
    topBox.add(Layout::buildForPseudoElement(pageStyle.area(Style::PageArea::TOP_CENTER)));
    topBox.add(Layout::buildForPseudoElement(pageStyle.area(Style::PageArea::TOP_RIGHT)));

    Layout::Tree topTree{
        .root = std::move(topBox),
        .viewport = Layout::Viewport{.small = topRect.size()}
    };

    auto [_, topFrag] = Layout::layoutCreateFragment(
        topTree,
        {
            .knownSize = topRect.size().cast<Opt<Px>>(),
            .position = topRect.topStart(),
            .availableSpace = topRect.size(),
            .containingBlock = topRect.size(),
        }
    );
    Layout::paint(topFrag, stack);

    // MARK: Bottom ------------------------------------------------------------

    auto bottomRect = RectPx::fromTwoPoint(
        bottomLeftMarginCornerRect.topEnd(),
        bottomRightMarginCornerRect.bottomStart()
    );

    auto bottomBox = Layout::buildForPseudoElement(pageStyle.area(Style::PageArea::BOTTOM));
    bottomBox.add(Layout::buildForPseudoElement(pageStyle.area(Style::PageArea::BOTTOM_LEFT)));
    bottomBox.add(Layout::buildForPseudoElement(pageStyle.area(Style::PageArea::BOTTOM_CENTER)));
    bottomBox.add(Layout::buildForPseudoElement(pageStyle.area(Style::PageArea::BOTTOM_RIGHT)));

    Layout::Tree bottomTree{
        .root = std::move(bottomBox),
        .viewport = Layout::Viewport{.small = bottomRect.size()}
    };

    auto [_, bottomFrag] = Layout::layoutCreateFragment(
        bottomTree,
        {
            .knownSize = bottomRect.size().cast<Opt<Px>>(),
            .position = bottomRect.topStart(),
            .availableSpace = bottomRect.size(),
            .containingBlock = bottomRect.size(),
        }
    );

    Layout::paint(bottomFrag, stack);

    // MARK: Left --------------------------------------------------------------
    auto leftRect = RectPx::fromTwoPoint(
        topLeftMarginCornerRect.bottomEnd(),
        bottomLeftMarginCornerRect.topStart()
    );

    auto leftBox = Layout::buildForPseudoElement(pageStyle.area(Style::PageArea::LEFT));
    leftBox.add(Layout::buildForPseudoElement(pageStyle.area(Style::PageArea::LEFT_TOP)));
    leftBox.add(Layout::buildForPseudoElement(pageStyle.area(Style::PageArea::LEFT_MIDDLE)));
    leftBox.add(Layout::buildForPseudoElement(pageStyle.area(Style::PageArea::LEFT_BOTTOM)));

    Layout::Tree leftTree{
        .root = std::move(leftBox),
        .viewport = Layout::Viewport{.small = leftRect.size()}
    };

    auto [_, leftFrag] = Layout::layoutCreateFragment(
        leftTree,
        {
            .knownSize = leftRect.size().cast<Opt<Px>>(),
            .position = leftRect.topStart(),
            .availableSpace = leftRect.size(),
            .containingBlock = leftRect.size(),
        }
    );

    Layout::paint(leftFrag, stack);

    // MARK: Right -------------------------------------------------------------

    auto rightRect = RectPx::fromTwoPoint(
        topRightMarginCornerRect.bottomEnd(),
        bottomRightMarginCornerRect.topStart()
    );

    auto rightBox = Layout::buildForPseudoElement(pageStyle.area(Style::PageArea::RIGHT));
    rightBox.add(Layout::buildForPseudoElement(pageStyle.area(Style::PageArea::RIGHT_TOP)));
    rightBox.add(Layout::buildForPseudoElement(pageStyle.area(Style::PageArea::RIGHT_MIDDLE)));
    rightBox.add(Layout::buildForPseudoElement(pageStyle.area(Style::PageArea::RIGHT_BOTTOM)));

    Layout::Tree rightTree{
        .root = std::move(rightBox),
        .viewport = Layout::Viewport{.small = rightRect.size()}
    };

    auto [_, rightFrag] = Layout::layoutCreateFragment(
        rightTree,
        {
            .knownSize = rightRect.size().cast<Opt<Px>>(),
            .position = rightRect.topStart(),
            .availableSpace = rightRect.size(),
            .containingBlock = rightRect.size(),
        }
    );

    Layout::paint(rightFrag, stack);
}

Vec<Strong<Scene::Page>> print(Markup::Document const &dom, Style::Media const &media) {
    Style::StyleBook stylebook;
    stylebook.add(
        fetchStylesheet("bundle://vaev-driver/html.css"_url, Style::Origin::USER_AGENT)
            .take("user agent stylesheet not available")
    );
    stylebook.add(
        fetchStylesheet("bundle://vaev-driver/print.css"_url, Style::Origin::USER_AGENT)
            .take("print stylesheet not available")
    );

    _collectStyle(dom, stylebook);

    Style::Computer computer{media, stylebook};

    // MARK: Page and Margins --------------------------------------------------

    auto scaleMatrix = Math::Trans2f::makeScale(media.resolution.toDppx());

    Vec<Strong<Scene::Page>> pages;

    // ideally prev breakpoint would be an OPT here or a pointer for the first iteration, where it doesnt exist
    Layout::Breakpoint prevBreakpoint{.endIdx = 0, .advanceCase = Layout::Breakpoint::ADVANCE_CASE::ADVANCE_WITHOUT_CHILDREN}, currBreakpoint;
    auto rootFragment = Layout::Frag();

    Layout::Tree contentTree = {
        Layout::build(computer, dom),
        {},
        Layout::FragmentationContext({
            media.width / Px{media.resolution.toDppx()},
            media.height / Px{media.resolution.toDppx()},
        })
    };

    while (true) {
        Layout::Resolver resolver{};
        resolver.viewport.dpi = media.resolution;

        Style::Page page{.name = ""s, .number = pages.len(), .blank = false};
        auto pageStyle = computer.computeFor(page);
        RectPx pageRect{
            media.width / Px{media.resolution.toDppx()},
            media.height / Px{media.resolution.toDppx()}
        };

        auto pageScene = pages.emplaceBack(
            // FIXME: it can be that specific pages have different dimensions
            makeStrong<Scene::Page>(pageRect.size().cast<isize>(), scaleMatrix)
        );

        InsetsPx pageMargin = {
            resolver.resolve(pageStyle->style->margin->top, pageRect.height),
            resolver.resolve(pageStyle->style->margin->end, pageRect.width),
            resolver.resolve(pageStyle->style->margin->bottom, pageRect.height),
            resolver.resolve(pageStyle->style->margin->start, pageRect.width),
        };

        RectPx pageContent = pageRect.shrink(pageMargin);

        _paintMargins(*pageStyle, pageRect, pageContent, *pageScene);

        // MARK: Page Content ------------------------------------------------------

        Layout::Viewport vp{
            .small = pageContent.size(),
            .large = pageRect.size(),
            .dynamic = pageRect.size(),
        };

        Layout::Input pageLayoutInput{
            .knownSize = {pageContent.width, NONE},
            .position = pageContent.topStart(),
            .availableSpace = pageContent.size(),
            .containingBlock = pageContent.size(),
        };

        contentTree.fc.setDiscovery();
        auto outDiscovery = Layout::layout(
            contentTree,
            contentTree.root,
            pageLayoutInput.withBreakpointTraverser(Layout::BreakpointTraverser(&prevBreakpoint))
        );

        currBreakpoint = outDiscovery.completelyLaidOut
                             ? Layout::Breakpoint::buildClassB(1, false)
                             : outDiscovery.breakpoint.unwrap();

        contentTree.fc.unsetDiscovery();
        auto outReal = Layout::layout(
            contentTree,
            contentTree.root,
            pageLayoutInput
                .withFragment(&rootFragment)
                .withBreakpointTraverser(Layout::BreakpointTraverser(&prevBreakpoint, &currBreakpoint))
        );

        Layout::paint(last(rootFragment.children), *pageScene);
        pageScene->prepare();

        if (outReal.completelyLaidOut)
            break;

        std::swap(prevBreakpoint, currBreakpoint);
    }

    return pages;
}

} // namespace Vaev::Driver
