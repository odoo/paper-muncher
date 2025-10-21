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

struct Page {
    Rc<Style::PageSpecifiedValues> style;
    usize index = 0;
    RectAu borderBox = {};
    RectAu contentBox = {};
    Layout::Frag fragment = {};
    Opt<Rc<Scene::Node>> canvas = {};
    Layout::Breakpoint breakpoint = {};
};

export struct PaginationContext {
    Layout::Tree _tree;
    Style::Media _media;
    Print::Settings _settings;
    Style::Computer _computer;
    Style::SpecifiedValues _style;
    Layout::Runnings _runnings = {};
    Vec<Page> _pages = {};

    static PaginationContext create(Gc::Ref<Dom::Document> dom, Print::Settings const& settings) {
        auto media = Style::Media::forPrint(settings);
        Font::Database database;
        if (not database.loadSystemFonts())
            logWarn("not all fonts were properly loaded into database");
        Style::Computer computer{
            media, *dom->styleSheets, std::move(database)
        };
        computer.build();
        computer.styleDocument(*dom);

        Style::SpecifiedValues initialStyle = Style::SpecifiedValues::initial();
        initialStyle.color = Gfx::BLACK;
        initialStyle.setCustomProp("-vaev-url", {Css::Token::string(Io::format("\"{}\"", dom->url()))});
        initialStyle.setCustomProp("-vaev-title", {Css::Token::string(Io::format("\"{}\"", dom->title()))});
        initialStyle.setCustomProp("-vaev-datetime", {Css::Token::string(Io::format("\"{}\"", Sys::now()))});

        return {
            ._tree = {Layout::build(dom)},
            ._media = media,
            ._settings = settings,
            ._computer = std::move(computer),
            ._style = initialStyle,
        };
    }

    // MARK: Page Creation ---------------------------------------------------------

    InsetsAu _resolvePageMargins(Page& page) {
        InsetsAu margins;

        if (_settings.margins == Print::Margins::DEFAULT) {
            Layout::Resolver resolver{};
            margins = {
                resolver.resolve(page.style->style->margin->top, page.borderBox.height),
                resolver.resolve(page.style->style->margin->end, page.borderBox.width),
                resolver.resolve(page.style->style->margin->bottom, page.borderBox.height),
                resolver.resolve(page.style->style->margin->start, page.borderBox.width),
            };
        } else if (_settings.margins == Print::Margins::CUSTOM) {
            margins = _settings.margins.custom.cast<Au>();
        } else if (_settings.margins == Print::Margins::MINIMUM) {
            margins = {};
        }

        return margins;
    }

    Page _createPage(usize index) {
        Page page = {
            _computer.computeFor(
                _style,
                {
                    .name = ""s,
                    .number = index,
                    .blank = false,
                }
            )
        };

        page.borderBox = {
            _media.width / Au{_media.resolution.toDppx()},
            _media.height / Au{_media.resolution.toDppx()}
        };

        auto size = page.borderBox.size().cast<f64>();
        auto margins = _resolvePageMargins(page);
        page.contentBox = page.borderBox.shrink(margins);
        return page;
    }

    // MARK: Page Breaking ---------------------------------------------------------

    Pair<bool, Layout::Breakpoint> _pageBreak(Page& page, Cursor<Layout::Breakpoint> previousBreakpoint) {
        Layout::Viewport vp{.small = page.contentBox.size()};

        _tree.viewport = vp;
        _tree.fc = {page.contentBox.size()};

        _tree.fc.enterDiscovery();

        auto output = Layout::layout(
            _tree,
            {
                .knownSize = {page.contentBox.width, NONE},
                .position = page.contentBox.topStart(),
                .availableSpace = page.contentBox.size(),
                .containingBlock = page.contentBox.size(),
                .runningPosition = &_runnings,
                .pageIndex = page.index,
                .breakpointTraverser = {
                    previousBreakpoint
                },
            }
        );

        _tree.fc.leaveDiscovery();

        return {output.completelyLaidOut, output.breakpoint.unwrap()};
    }

    void _splitPages() {
        usize pageNumber = 0;

        while (true) {
            auto page = _createPage(pageNumber++);
            Cursor previousBreakpoint =
                _pages ? &last(_pages).breakpoint
                       : &Layout::Breakpoint::START_OF_DOCUMENT;

            auto [done, breakpoint] = _pageBreak(page, previousBreakpoint);
            page.breakpoint = std::move(breakpoint);
            _pages.pushBack(page);
            if (done)
                break;
        }
    }

    // MARK: Page Painting ---------------------------------------------------------

    void _paintCornerMargin(Style::PageSpecifiedValues& pageStyle, Scene::Stack& stack, RectAu const& rect, Style::PageArea area, usize currentPage, Layout::Runnings& runningPosition) {
        Layout::Tree tree{
            .root = Layout::buildForPseudoElement(pageStyle.area(area), currentPage, runningPosition),
            .viewport = Layout::Viewport{.small = rect.size()}
        };
        auto [_, frag] = Layout::layoutCreateFragment(
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

    void _paintMainMargin(Style::PageSpecifiedValues& pageStyle, Scene::Stack& stack, RectAu const& rect, Style::PageArea mainArea, Array<Style::PageArea, 3> subAreas, usize currentPage, Layout::Runnings& runningPosition) {
        auto box = Layout::buildForPseudoElement(pageStyle.area(mainArea), currentPage, runningPosition);
        for (auto subArea : subAreas) {
            box.add(Layout::buildForPseudoElement(pageStyle.area(subArea), currentPage, runningPosition));
        }
        Layout::Tree tree{
            .root = std::move(box),
            .viewport = Layout::Viewport{.small = rect.size()}
        };
        auto [_, frag] = Layout::layoutCreateFragment(
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

    void _paintMargins(Page& page, Scene::Stack& stack, Layout::Runnings& runningPosition) {
        // Compute all corner rects
        auto topLeftMarginCornerRect = RectAu::fromTwoPoint(page.borderBox.topStart(), page.contentBox.topStart());
        auto topRightMarginCornerRect = RectAu::fromTwoPoint(page.borderBox.topEnd(), page.contentBox.topEnd());
        auto bottomLeftMarginCornerRect = RectAu::fromTwoPoint(page.borderBox.bottomStart(), page.contentBox.bottomStart());
        auto bottomRightMarginCornerRect = RectAu::fromTwoPoint(page.borderBox.bottomEnd(), page.contentBox.bottomEnd());

        // Paint corners
        _paintCornerMargin(*page.style, stack, topLeftMarginCornerRect, Style::PageArea::TOP_LEFT_CORNER, page.index, runningPosition);
        _paintCornerMargin(*page.style, stack, topRightMarginCornerRect, Style::PageArea::TOP_RIGHT_CORNER, page.index, runningPosition);
        _paintCornerMargin(*page.style, stack, bottomLeftMarginCornerRect, Style::PageArea::BOTTOM_LEFT_CORNER, page.index, runningPosition);
        _paintCornerMargin(*page.style, stack, bottomRightMarginCornerRect, Style::PageArea::BOTTOM_RIGHT_CORNER, page.index, runningPosition);

        // Compute main area rects
        auto topRect = RectAu::fromTwoPoint(topLeftMarginCornerRect.topEnd(), topRightMarginCornerRect.bottomStart());
        auto bottomRect = RectAu::fromTwoPoint(bottomLeftMarginCornerRect.topEnd(), bottomRightMarginCornerRect.bottomStart());
        auto leftRect = RectAu::fromTwoPoint(topLeftMarginCornerRect.bottomEnd(), bottomLeftMarginCornerRect.topStart());
        auto rightRect = RectAu::fromTwoPoint(topRightMarginCornerRect.bottomEnd(), bottomRightMarginCornerRect.topStart());

        // Paint main areas
        _paintMainMargin(*page.style, stack, topRect, Style::PageArea::TOP, {Style::PageArea::TOP_LEFT, Style::PageArea::TOP_CENTER, Style::PageArea::TOP_RIGHT}, page.index, runningPosition);
        _paintMainMargin(*page.style, stack, bottomRect, Style::PageArea::BOTTOM, {Style::PageArea::BOTTOM_LEFT, Style::PageArea::BOTTOM_CENTER, Style::PageArea::BOTTOM_RIGHT}, page.index, runningPosition);
        _paintMainMargin(*page.style, stack, leftRect, Style::PageArea::LEFT, {Style::PageArea::LEFT_TOP, Style::PageArea::LEFT_MIDDLE, Style::PageArea::LEFT_BOTTOM}, page.index, runningPosition);
        _paintMainMargin(*page.style, stack, rightRect, Style::PageArea::RIGHT, {Style::PageArea::RIGHT_TOP, Style::PageArea::RIGHT_MIDDLE, Style::PageArea::RIGHT_BOTTOM}, page.index, runningPosition);
    }

    // https://drafts.csswg.org/css-page/#painting
    Rc<Scene::Node> _paintPage(Page& page, Layout::Frag& fragment) {
        auto pageCanvas = makeRc<Scene::Stack>();

        // When drawing a page of content, the page layers are painted in
        // the following painting order (bottommost first):

        // 1. Page background
        // TODO

        // 2. document canvas
        // TODO

        // 3. page borders
        // TODO

        // 4. document contents
        Layout::paint(fragment, *pageCanvas);

        // 5. page-margin boxes
        if (_settings.headerFooter and _settings.margins != Print::Margins::NONE)
            _paintMargins(page, *pageCanvas, _runnings);

        pageCanvas->prepare();
        return makeRc<Scene::Transform>(
            pageCanvas,
            Math::Trans2f::scale(_media.resolution.toDppx())
        );
    }

    // MARK: Page Rendering ----------------------------------------------------

    Generator<Print::Page> _renderPages() {
        for (auto& page : _pages) {
            auto [_, fragment] = Layout::layoutCreateFragment(
                _tree,
                {
                    .knownSize = {page.contentBox.width, NONE},
                    .position = page.contentBox.topStart(),
                    .availableSpace = page.contentBox.size(),
                    .containingBlock = page.contentBox.size(),
                    .pageIndex = page.index,
                    .breakpointTraverser = {
                        page.index
                            ? &_pages[page.index - 1].breakpoint
                            : &Layout::Breakpoint::START_OF_DOCUMENT,
                        &page.breakpoint,
                    },
                }
            );

            page.fragment = std::move(fragment);

            co_yield Print::Page(
                _settings.paper,
                _paintPage(page, fragment)
            );
        }
    }

    [[clang::coro_wrapper]]
    Generator<Print::Page> iterPages() {
        _splitPages();
        return _renderPages();
    }
};

} // namespace Vaev::Driver
