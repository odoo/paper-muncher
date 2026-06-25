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

export struct PageLayoutInfos {
    usize pageNumber;
    RectAu pageRect;
    RectAu pageDecoration;
    RectAu pageContent;
    Rc<Style::PageComputedValues> pageStyle;
    Layout::Breakpoint breakpoint;
};

export struct PageDecorator {
    virtual ~PageDecorator() = default;

    virtual RectAu layout([[maybe_unused]] Style::Media const& media, [[maybe_unused]] PageLayoutInfos const& infos) {
        return infos.pageDecoration;
    }

    virtual void decorate([[maybe_unused]] Style::Media const& media, [[maybe_unused]] PageLayoutInfos const& infos, [[maybe_unused]] usize pageCount, [[maybe_unused]] Scene::Stack& pageStack) {
    }
};

void _paintCornerMargin(PageLayoutInfos& infos, Scene::Stack& stack, RectAu const& rect, Style::PageArea area, Layout::RunningPositionMap& runningPosition) {
    Layout::Tree tree{
        .root = Layout::buildElement(infos.pageStyle->area(area), infos.pageNumber, runningPosition),
        .viewport = Layout::Viewport{.small = rect.size()}
    };
    auto output = Layout::layoutRoot(
        tree,
        {
            .generateFragment = true,
            .knownSize = rect.size().cast<Opt<Au>>(),
            .position = rect.topStart(),
            .availableSpace = rect.size(),
            .containingBlock = rect.size(),
        }
    );
    Layout::paint(*output.fragment, stack);
}

void _paintMainMargin(PageLayoutInfos& infos, Scene::Stack& stack, RectAu const& rect, Style::PageArea mainArea, Array<Style::PageArea, 3> subAreas, Layout::RunningPositionMap& runningPosition) {
    auto box = Layout::buildElement(infos.pageStyle->area(mainArea), infos.pageNumber, runningPosition);
    for (auto subArea : subAreas) {
        box.add(Layout::buildElement(infos.pageStyle->area(subArea), infos.pageNumber, runningPosition));
    }
    Layout::Tree tree{
        .root = std::move(box),
        .viewport = Layout::Viewport{.small = rect.size()}
    };
    auto output = Layout::layoutRoot(
        tree,
        {
            .generateFragment = true,
            .knownSize = rect.size().cast<Opt<Au>>(),
            .position = rect.topStart(),
            .availableSpace = rect.size(),
            .containingBlock = rect.size(),
        }
    );
    Layout::paint(*output.fragment, stack);
}

void _paintMargins(PageLayoutInfos& infos, Scene::Stack& stack, Layout::RunningPositionMap& runningPosition) {
    // Compute all corner rects
    auto topLeftMarginCornerRect = RectAu::fromTwoPoint(infos.pageRect.topStart(), infos.pageContent.topStart());
    auto topRightMarginCornerRect = RectAu::fromTwoPoint(infos.pageRect.topEnd(), infos.pageContent.topEnd());
    auto bottomLeftMarginCornerRect = RectAu::fromTwoPoint(infos.pageRect.bottomStart(), infos.pageContent.bottomStart());
    auto bottomRightMarginCornerRect = RectAu::fromTwoPoint(infos.pageRect.bottomEnd(), infos.pageContent.bottomEnd());

    // Paint corners
    _paintCornerMargin(infos, stack, topLeftMarginCornerRect, Style::PageArea::TOP_LEFT_CORNER, runningPosition);
    _paintCornerMargin(infos, stack, topRightMarginCornerRect, Style::PageArea::TOP_RIGHT_CORNER, runningPosition);
    _paintCornerMargin(infos, stack, bottomLeftMarginCornerRect, Style::PageArea::BOTTOM_LEFT_CORNER, runningPosition);
    _paintCornerMargin(infos, stack, bottomRightMarginCornerRect, Style::PageArea::BOTTOM_RIGHT_CORNER, runningPosition);

    // Compute main area rects
    auto topRect = RectAu::fromTwoPoint(topLeftMarginCornerRect.topEnd(), topRightMarginCornerRect.bottomStart());
    auto bottomRect = RectAu::fromTwoPoint(bottomLeftMarginCornerRect.topEnd(), bottomRightMarginCornerRect.bottomStart());
    auto leftRect = RectAu::fromTwoPoint(topLeftMarginCornerRect.bottomEnd(), bottomLeftMarginCornerRect.topStart());
    auto rightRect = RectAu::fromTwoPoint(topRightMarginCornerRect.bottomEnd(), bottomRightMarginCornerRect.topStart());

    // Paint main areas
    _paintMainMargin(infos, stack, topRect, Style::PageArea::TOP, {Style::PageArea::TOP_LEFT, Style::PageArea::TOP_CENTER, Style::PageArea::TOP_RIGHT}, runningPosition);
    _paintMainMargin(infos, stack, bottomRect, Style::PageArea::BOTTOM, {Style::PageArea::BOTTOM_LEFT, Style::PageArea::BOTTOM_CENTER, Style::PageArea::BOTTOM_RIGHT}, runningPosition);
    _paintMainMargin(infos, stack, leftRect, Style::PageArea::LEFT, {Style::PageArea::LEFT_TOP, Style::PageArea::LEFT_MIDDLE, Style::PageArea::LEFT_BOTTOM}, runningPosition);
    _paintMainMargin(infos, stack, rightRect, Style::PageArea::RIGHT, {Style::PageArea::RIGHT_TOP, Style::PageArea::RIGHT_MIDDLE, Style::PageArea::RIGHT_BOTTOM}, runningPosition);
}

struct PaginationContext {
    Layout::Tree& contentTree;
    Style::Media& media;
    Print::Settings const& settings;
    Style::Computer& computer;
    Rc<Style::ComputedValues> initialStyle;
    Opt<PageDecorator&> decorator = NONE;
};

static InsetsAu _resolvePageMargin(Print::Margins marginSetting, Margin const& margin, RectAu pageRect) {
    switch (marginSetting.named) {
    case Print::Margins::NONE:
        return {};

    case Print::Margins::DEFAULT: {
        Layout::Resolver resolver{};
        return {
            resolver.resolve(margin.top, pageRect.height),
            resolver.resolve(margin.end, pageRect.width),
            resolver.resolve(margin.bottom, pageRect.height),
            resolver.resolve(margin.start, pageRect.width),
        };
    }

    case Print::Margins::MINIMUM:
        // FIXME: Supposed to be the minimum supported by the printer
        return {};

    case Print::Margins::CUSTOM:
        return marginSetting.custom.cast<Au>();

    default:
        unreachable();
    }
}

Vec<PageLayoutInfos> collectBreakPointsAndRunningPositions(PaginationContext& context) {
    auto startOfDocument = Layout::Breakpoint::startOfDocument();
    Vec<PageLayoutInfos> pageInfos = {};

    while (true) {
        Style::Page page{
            .name = ""s,
            .number = pageInfos.len() + 1,
            .blank = false,
        };

        auto pageStyle = context.computer.computeFor(*context.initialStyle, page);
        auto pageRect = RectAu{context.media.scaledViewport()};
        auto pageDecoration = pageRect.shrink(
            _resolvePageMargin(context.settings.margins, *pageStyle->style->margin, pageRect)
        );
        PageLayoutInfos infos{
            page.number,
            pageRect,
            pageDecoration,
            pageDecoration,
            pageStyle,
            {}
        };

        if (auto& [decorator] = context.decorator)
            infos.pageContent = decorator.layout(context.media, infos);

        context.contentTree.viewport = {
            .small = infos.pageContent.size(),
        };
        context.contentTree.fc = {infos.pageContent.size()};
        context.contentTree.fc.enterDiscovery();

        auto outDiscovery = Layout::layoutRoot(
            context.contentTree,
            {
                .knownSize = {infos.pageContent.width, NONE},
                .position = infos.pageContent.topStart(),
                .availableSpace = infos.pageContent.size(),
                .containingBlock = infos.pageContent.size(),
                .pageNumber = page.number,
                .breakpointTraverser = Layout::BreakpointTraverser(
                    pageInfos ? &last(pageInfos).breakpoint : &startOfDocument
                ),
            }
        );

        context.contentTree.fc.leaveDiscovery();

        infos.breakpoint =
            outDiscovery.completelyLaidOut
                ? Layout::Breakpoint::classB(1, false)
                : outDiscovery.breakpoint.unwrap();

        pageInfos.pushBack(std::move(infos));

        if (outDiscovery.completelyLaidOut)
            return pageInfos;
    }
}

export Yield<Print::Page> print(Gc::Heap& heap, Gc::Ref<Dom::Document> dom, Print::Settings const& settings, Opt<PageDecorator&> decorator) {
    auto media = Style::Media::forPrint(settings);

    Style::Computer computer{
        heap,
        media,
        dom->registeredPropertySet,
        *dom->styleSheets,
        *dom->fontDatabase,
    };
    computer.build();
    computer.styleDocument(*dom);

    auto initialStyle = dom->initialComputedValues();

    Layout::Tree contentTree = {
        Layout::buildDocument(dom),
    };

    PaginationContext paginationContext{
        .contentTree = contentTree,
        .media = media,
        .settings = settings,
        .computer = computer,
        .initialStyle = initialStyle,
        .decorator = decorator,
    };

    auto startOfDocument = Layout::Breakpoint::startOfDocument();
    auto pageInfos = collectBreakPointsAndRunningPositions(paginationContext);

    for (auto [infos, i] : iter(pageInfos) | Index()) {
        Layout::RunningPositionMap runningPosition = {};

        contentTree.viewport = {
            .small = infos.pageContent.size(),
        };
        auto output = Layout::layoutRoot(
            contentTree,
            {
                .generateFragment = true,
                .knownSize = {infos.pageContent.width, NONE},
                .position = infos.pageContent.topStart(),
                .availableSpace = infos.pageContent.size(),
                .containingBlock = infos.pageContent.size(),
                .runningPosition = &runningPosition,
                .pageNumber = infos.pageNumber,
                .breakpointTraverser = {
                    i == 0 ? &startOfDocument : &pageInfos[i - 1].breakpoint,
                    &infos.breakpoint,
                },
            }
        );

        auto pageStack = makeRc<Scene::Stack>();
        if (settings.headerFooter and settings.margins != Print::Margins::NONE)
            _paintMargins(
                infos,
                *pageStack,
                runningPosition
            );

        if (auto& [decorator] = paginationContext.decorator)
            decorator.decorate(paginationContext.media, infos, pageInfos.len(), *pageStack);

        Layout::paint(*output.fragment, *pageStack);
        pageStack->prepare();

        co_yield Print::Page(
            settings.pageSize().cast<f64>(),
            makeRc<Scene::Transform>(
                pageStack,
                Math::Trans2f::scale(media.scale())
            )
        );
    }
}

} // namespace Vaev::Driver
