export module Vaev.Engine:driver.print;

import Karm.Core;
import Karm.Gc;
import Karm.Print;
import Karm.Scene;
import Karm.Font;
import Karm.Sys;
import Karm.Gfx;
import Karm.Math;
import Karm.Logger;

import :style;
import :layout;
import :values;
import :dom.document;
import :css;

using namespace Karm;

namespace Vaev::Driver {

void _paintCornerMargin(Style::PageSpecifiedValues& pageStyle, Scene::Stack& stack, RectAu const& rect, Style::PageArea area, usize currentPage, Layout::RunningPositionMap& runningPosition) {
    Layout::Tree tree{
        .root = Layout::buildForPseudoElement(pageStyle.area(area), currentPage, runningPosition),
        .viewport = Layout::Viewport{.small = rect.size()}
    };
    auto [_, frag] = Layout::layoutAndCommitRoot(
        tree,
        {
            .knownSize = rect.size().cast<Opt<Au>>(),
            .position = rect.topStart(),
            .availableSpace = rect.size(),
            .containingBlock = rect.size(),
        }
    );
    Layout::paint(frag, stack);
}

void _paintMainMargin(Style::PageSpecifiedValues& pageStyle, Scene::Stack& stack, RectAu const& rect, Style::PageArea mainArea, Array<Style::PageArea, 3> subAreas, usize currentPage, Layout::RunningPositionMap& runningPosition) {
    auto box = Layout::buildForPseudoElement(pageStyle.area(mainArea), currentPage, runningPosition);
    for (auto subArea : subAreas) {
        box.add(Layout::buildForPseudoElement(pageStyle.area(subArea), currentPage, runningPosition));
    }
    Layout::Tree tree{
        .root = std::move(box),
        .viewport = Layout::Viewport{.small = rect.size()}
    };
    auto [_, frag] = Layout::layoutAndCommitRoot(
        tree,
        {
            .knownSize = rect.size().cast<Opt<Au>>(),
            .position = rect.topStart(),
            .availableSpace = rect.size(),
            .containingBlock = rect.size(),
        }
    );
    Layout::paint(frag, stack);
}

void _paintMargins(Style::PageSpecifiedValues& pageStyle, RectAu pageRect, RectAu pageContent, Scene::Stack& stack, usize currentPage, Layout::RunningPositionMap& runningPosition) {
    // Compute all corner rects
    auto topLeftMarginCornerRect = RectAu::fromTwoPoint(pageRect.topStart(), pageContent.topStart());
    auto topRightMarginCornerRect = RectAu::fromTwoPoint(pageRect.topEnd(), pageContent.topEnd());
    auto bottomLeftMarginCornerRect = RectAu::fromTwoPoint(pageRect.bottomStart(), pageContent.bottomStart());
    auto bottomRightMarginCornerRect = RectAu::fromTwoPoint(pageRect.bottomEnd(), pageContent.bottomEnd());

    // Paint corners
    _paintCornerMargin(pageStyle, stack, topLeftMarginCornerRect, Style::PageArea::TOP_LEFT_CORNER, currentPage, runningPosition);
    _paintCornerMargin(pageStyle, stack, topRightMarginCornerRect, Style::PageArea::TOP_RIGHT_CORNER, currentPage, runningPosition);
    _paintCornerMargin(pageStyle, stack, bottomLeftMarginCornerRect, Style::PageArea::BOTTOM_LEFT_CORNER, currentPage, runningPosition);
    _paintCornerMargin(pageStyle, stack, bottomRightMarginCornerRect, Style::PageArea::BOTTOM_RIGHT_CORNER, currentPage, runningPosition);

    // Compute main area rects
    auto topRect = RectAu::fromTwoPoint(topLeftMarginCornerRect.topEnd(), topRightMarginCornerRect.bottomStart());
    auto bottomRect = RectAu::fromTwoPoint(bottomLeftMarginCornerRect.topEnd(), bottomRightMarginCornerRect.bottomStart());
    auto leftRect = RectAu::fromTwoPoint(topLeftMarginCornerRect.bottomEnd(), bottomLeftMarginCornerRect.topStart());
    auto rightRect = RectAu::fromTwoPoint(topRightMarginCornerRect.bottomEnd(), bottomRightMarginCornerRect.topStart());

    // Paint main areas
    _paintMainMargin(pageStyle, stack, topRect, Style::PageArea::TOP, {Style::PageArea::TOP_LEFT, Style::PageArea::TOP_CENTER, Style::PageArea::TOP_RIGHT}, currentPage, runningPosition);
    _paintMainMargin(pageStyle, stack, bottomRect, Style::PageArea::BOTTOM, {Style::PageArea::BOTTOM_LEFT, Style::PageArea::BOTTOM_CENTER, Style::PageArea::BOTTOM_RIGHT}, currentPage, runningPosition);
    _paintMainMargin(pageStyle, stack, leftRect, Style::PageArea::LEFT, {Style::PageArea::LEFT_TOP, Style::PageArea::LEFT_MIDDLE, Style::PageArea::LEFT_BOTTOM}, currentPage, runningPosition);
    _paintMainMargin(pageStyle, stack, rightRect, Style::PageArea::RIGHT, {Style::PageArea::RIGHT_TOP, Style::PageArea::RIGHT_MIDDLE, Style::PageArea::RIGHT_BOTTOM}, currentPage, runningPosition);
}

Style::PageContainers _computePageContainers(Style::PageContainers initialPageContainers, Style::PageSpecifiedValues const& pageStyle) {
    auto font = Gfx::Font{pageStyle.style->fontFace, 16.0};

    Layout::Resolver marginResolver{
        .rootFont = font,
        .boxFont = font,
        .viewport = {.small = initialPageContainers.pageArea},
    };

    InsetsAu pageMargin = {
        marginResolver.resolve(pageStyle.style->margin->top, initialPageContainers.pageBox.height),
        marginResolver.resolve(pageStyle.style->margin->end, initialPageContainers.pageBox.width),
        marginResolver.resolve(pageStyle.style->margin->bottom, initialPageContainers.pageBox.height),
        marginResolver.resolve(pageStyle.style->margin->start, initialPageContainers.pageBox.width),
    };

    InsetsAu pagePadding = {
        marginResolver.resolve(pageStyle.style->padding->top, initialPageContainers.pageBox.height),
        marginResolver.resolve(pageStyle.style->padding->end, initialPageContainers.pageBox.width),
        marginResolver.resolve(pageStyle.style->padding->bottom, initialPageContainers.pageBox.height),
        marginResolver.resolve(pageStyle.style->padding->start, initialPageContainers.pageBox.width),
    };

    auto ctx = ComputationContext{
        .rootFont = font,
        .font = font,
        .viewport = {.small = initialPageContainers.pageArea},
        .displayArea = initialPageContainers.pageBox,
    };

    Vec2Au pageBox = pageStyle.size.toComputed(ctx);
    logInfo("PAGE BOX: {}", pageBox);
    Vec2Au pageArea = RectAu{pageBox}.shrink(pageMargin).shrink(pagePadding).size();
    logInfo("PAGE AREA: {}", pageArea);

    return Style::PageContainers{
        .pageBox = pageBox,
        .pageArea = pageArea,
    };
}

struct PaginationContext {
    Opt<Layout::Tree> contentTree;
    Print::Settings const& settings;
    Style::Computer& computer;
    Style::SpecifiedValues& initialStyle;
    Style::PageContainers initialPageContainers;
    Gc::Ref<Dom::Document> dom;
};

struct PageLayoutInfos {
    RectAu pageRect;
    RectAu pageContent;
    Rc<Style::PageSpecifiedValues> pageStyle;
};

Pair<Vec<Layout::Breakpoint>, Vec<PageLayoutInfos>> collectBreakPointsAndRunningPositions(Layout::RunningPositionMap& runningPosition, PaginationContext& context) {
    usize pageNumber = 0;
    Layout::Breakpoint prevBreakpoint{
        .endIdx = 0,
        .advance = Layout::Breakpoint::Advance::WITHOUT_CHILDREN
    };

    Vec<Layout::Breakpoint> breakpoints = {prevBreakpoint};
    Vec<PageLayoutInfos> pageInfos = {};

    bool first = true;

    while (true) {
        Style::Page page{
            .name = ""s,
            .number = pageNumber++,
            .blank = false,
        };

        auto initialPageSize = Pair<Length, Length>(context.initialPageContainers.pageBox.width, context.initialPageContainers.pageBox.height);
        Rc<Style::PageSpecifiedValues> pageStyle = context.computer.computeFor(context.initialStyle, initialPageSize, page);

        auto pageStack = makeRc<Scene::Stack>();

        auto [pageBox, pageArea] = _computePageContainers(context.initialPageContainers, *pageStyle);

        // FIXME: styleDocument() should not be hidden in a trench inside a function that should probably get inlined.
        // FIXME: Accessing internals of computer.
        if (first) {
            context.computer._media.changeDisplayArea(pageBox);
            context.computer.styleDocument(*context.dom);

            context.contentTree = {
                Layout::build(context.dom),
            };

            first = false;
        }

        pageInfos.pushBack({pageBox, pageArea, pageStyle});

        Layout::Viewport vp{
            .small = pageArea,
        };

        context.contentTree->viewport = vp;
        context.contentTree->fc = {pageArea};

        Layout::Input pageLayoutInput{
            .knownSize = {pageArea.width, NONE},
            .position = {(pageBox.width - pageArea.width) / 2, (pageBox.height - pageArea.height) / 2},
            .availableSpace = pageArea,
            .containingBlock = pageArea,
            .pageNumber = pageNumber,
        };
        pageLayoutInput.runningPosition = {&runningPosition};
        context.contentTree->fc.enterDiscovery();

        auto outDiscovery = Layout::layoutRoot(
            *context.contentTree,
            pageLayoutInput.withBreakpointTraverser(Layout::BreakpointTraverser(&prevBreakpoint))
        );

        Layout::Breakpoint currBreakpoint =
            outDiscovery.completelyLaidOut
                ? Layout::Breakpoint::classB(1, false)
                : outDiscovery.breakpoint.unwrap();

        context.contentTree->fc.leaveDiscovery();

        breakpoints.pushBack(currBreakpoint);
        if (outDiscovery.completelyLaidOut)
            break;

        prevBreakpoint = std::move(currBreakpoint);
    }

    return {breakpoints, pageInfos};
}

export Yield<Print::Page> print(Gc::Ref<Dom::Document> dom, Print::Settings const& settings) {
    auto media = Style::Media::forPrint(settings);

    Style::Computer computer{
        media,
        dom->registeredPropertySet,
        *dom->styleSheets,
        *dom->fontDatabase,
    };
    computer.build();

    // MARK: Page and Margins --------------------------------------------------
    InsetsAu initialMargins = {};
    if (settings.margins == Print::Margins::DEFAULT) {
        auto ctx = ComputationContext{};
        initialMargins = Length(0.5, Length::IN).toComputed(ctx);
    } else if (settings.margins == Print::Margins::CUSTOM) {
        initialMargins = settings.margins.custom;
    } else {
        initialMargins = {};
    }

    auto rect = RectAu{media.displayArea()};

    Style::PageContainers initialPageContainers = {
        .pageBox = media.displayArea(),
        .pageArea = rect.shrink(initialMargins).size(),
    };

    logInfo("(init-page-container box: {} area: {})", initialPageContainers.pageBox, initialPageContainers.pageArea);

    // MARK: Initial Values ---------------------------------------------------
    Style::SpecifiedValues initialStyle = Style::SpecifiedValues::initial();
    initialStyle.color = Gfx::BLACK;
    initialStyle.margin.cow() = initialMargins.cast<Length>().cast<CalcValue<PercentOr<Length>>>().cast<Width>();

    initialStyle.setCustomProp("-vaev-url", {Css::Token::string(Io::format("\"{}\"", dom->url()))});
    initialStyle.setCustomProp("-vaev-title", {Css::Token::string(Io::format("\"{}\"", dom->title()))});
    initialStyle.setCustomProp("-vaev-datetime", {Css::Token::string(Io::format("\"{}\"", Sys::now()))});

    // MARK: Page Content ------------------------------------------------------

    Layout::RunningPositionMap runningPosition = {}; // Mapping the different Running positions to their respective names and their page.
    PaginationContext paginationContext{
        .contentTree = NONE,
        .settings = settings,
        .computer = computer,
        .initialStyle = initialStyle,
        .initialPageContainers = initialPageContainers,
        .dom = dom,
    };
    auto [breakpoints, pageInfos] = collectBreakPointsAndRunningPositions(runningPosition, paginationContext);

    for (usize pageNumber = 0; pageNumber < breakpoints.len() - 1; pageNumber++) {
        Style::Page page{
            .name = ""s,
            .number = pageNumber,
            .blank = false,
        };
        auto pageStack = makeRc<Scene::Stack>();

        auto infos = pageInfos[pageNumber];
        Layout::Input pageLayoutInput{
            .knownSize = {infos.pageContent.width, NONE},
            .position = {
                (infos.pageRect.width - infos.pageContent.width) / 2,
                (infos.pageRect.height - infos.pageContent.height) / 2,
            },
            .availableSpace = infos.pageContent.size(),
            .containingBlock = infos.pageContent.size(),
            .pageNumber = pageNumber,
        };
        auto [outFragmentation, fragment] = Layout::layoutAndCommitRoot(
            *paginationContext.contentTree,
            pageLayoutInput
                .withBreakpointTraverser(Layout::BreakpointTraverser(&breakpoints[pageNumber], &breakpoints[pageNumber + 1]))
        );

        if (settings.headerFooter and settings.margins != Print::Margins::NONE)
            _paintMargins(*pageInfos[pageNumber].pageStyle, pageInfos[pageNumber].pageRect, pageInfos[pageNumber].pageContent, *pageStack, page.number, runningPosition);

        Layout::paint(fragment, *pageStack);
        pageStack->prepare();

        co_yield Print::Page(
            infos.pageRect.size().cast<f64>(),
            makeRc<Scene::Transform>(
                pageStack,
                Math::Trans2f::scale(media.resolution.toDppx())
            )
        );
    }
}

} // namespace Vaev::Driver
