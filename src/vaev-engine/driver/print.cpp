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

void _paintCornerMargin(Style::PageComputedValues& pageStyle, Scene::Stack& stack, RectAu const& rect, Style::PageArea area, usize currentPage, Layout::RunningPositionMap& runningPosition) {
    Layout::Tree tree{
        .root = Layout::buildElement(pageStyle.area(area), currentPage, runningPosition),
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

void _paintMainMargin(Style::PageComputedValues& pageStyle, Scene::Stack& stack, RectAu const& rect, Style::PageArea mainArea, Array<Style::PageArea, 3> subAreas, usize currentPage, Layout::RunningPositionMap& runningPosition) {
    auto box = Layout::buildElement(pageStyle.area(mainArea), currentPage, runningPosition);
    for (auto subArea : subAreas) {
        box.add(Layout::buildElement(pageStyle.area(subArea), currentPage, runningPosition));
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

void _paintMargins(Style::PageComputedValues& pageStyle, RectAu pageRect, RectAu pageContent, Scene::Stack& stack, usize currentPage, Layout::RunningPositionMap& runningPosition) {
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

struct PageLayoutInfos {
    RectAu pageRect;
    RectAu pageContent;
    Rc<Style::PageComputedValues> pageStyle;
};

export Yield<Print::Page> print(Gc::Heap& heap, Gc::Ref<Dom::Document> dom, Print::Settings const& settings) {
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

    // MARK: Page and Margins --------------------------------------------------

    auto initialStyle = dom->registeredPropertySet.initialComputedValues();
    initialStyle->color = Gfx::BLACK;
    initialStyle->setCustomProp("-vaev-url", {Css::Token::string(Io::format("\"{}\"", dom->url()))});
    initialStyle->setCustomProp("-vaev-title", {Css::Token::string(Io::format("\"{}\"", dom->title()))});
    initialStyle->setCustomProp("-vaev-datetime", {Css::Token::string(Io::format("\"{}\"", Sys::now()))});

    // MARK: Page Content ------------------------------------------------------

    Layout::Tree contentTree = {
        Layout::buildDocument(dom),
    };

    Layout::RunningPositionMap runningPosition = {}; // Mapping the different Running positions to their respective names and their page.

    RectAu pageRect{
        media.width / media.resolution.toDppx(),
        media.height / media.resolution.toDppx()
    };

    usize pageNumber = 1;
    Layout::Breakpoint prevBreakpoint{
        .endIdx = 0,
        .advance = Layout::Breakpoint::Advance::WITHOUT_CHILDREN
    };

    while (true) {
        Style::Page page{
            .name = ""s,
            .number = pageNumber,
            .blank = false,
        };

        Rc<Style::PageComputedValues> pageStyle = computer.computeFor(*initialStyle, page);

        InsetsAu pageMargin;

        if (settings.margins == Print::Margins::DEFAULT) {
            Layout::Resolver resolver{};
            pageMargin = {
                resolver.resolve(pageStyle->style->margin->top, pageRect.height),
                resolver.resolve(pageStyle->style->margin->end, pageRect.width),
                resolver.resolve(pageStyle->style->margin->bottom, pageRect.height),
                resolver.resolve(pageStyle->style->margin->start, pageRect.width),
            };
        } else if (settings.margins == Print::Margins::CUSTOM) {
            pageMargin = settings.margins.custom.template cast<Au>();
        } else if (settings.margins == Print::Margins::MINIMUM) {
            pageMargin = {};
        }

        RectAu pageContent = pageRect.shrink(pageMargin);

        contentTree.viewport = {.small = pageContent.size()};
        contentTree.fc = {pageContent.size()};

        Layout::Input pageLayoutInput{
            .knownSize = {pageContent.width, NONE},
            .position = pageContent.topStart(),
            .availableSpace = pageContent.size(),
            .containingBlock = pageContent.size(),
            .runningPosition = {&runningPosition},
            .pageNumber = pageNumber,
        };
        contentTree.fc.enterDiscovery();

        auto [output, frag] = Layout::layoutAndCommitRoot(
            contentTree,
            pageLayoutInput.withBreakpointTraverser(Layout::BreakpointTraverser(&prevBreakpoint))
        );

        Layout::Breakpoint currBreakpoint =
            output.completelyLaidOut
                ? Layout::Breakpoint::classB(1, false)
                : output.breakpoint.unwrap();

        contentTree.fc.leaveDiscovery();

        auto pageStack = makeRc<Scene::Stack>();
        if (settings.headerFooter and settings.margins != Print::Margins::NONE)
            _paintMargins(*pageStyle, pageRect, pageContent, *pageStack, page.number, runningPosition);

        Layout::paint(frag, *pageStack);

        pageStack->prepare();

        co_yield Print::Page(
            settings.pageSize().cast<f64>(),
            makeRc<Scene::Transform>(
                pageStack,
                Math::Trans2f::scale(media.resolution.toDppx())
            )
        );

        if (output.completelyLaidOut)
            break;

        prevBreakpoint = output.breakpoint.take();
        pageNumber++;
    }
}

} // namespace Vaev::Driver
