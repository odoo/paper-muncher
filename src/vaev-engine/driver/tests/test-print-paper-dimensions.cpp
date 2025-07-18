#include <karm-test/macros.h>
#include <karm-gfx/prose.h>

import Vaev.Engine;
import Karm.Gc;
import Karm.Print;

using namespace Karm;

namespace Vaev::Driver::Tests {

f64 const EPSILON = 1e-2;

void buildTestCase(Gc::Heap& gc, Gc::Ref<Dom::Document> dom, usize amountOfPages, Str styleCase) {
    dom->styleSheets = gc.alloc<Style::StyleSheetList>();
    Html::HtmlParser parser{gc, dom};

    Karm::StringBuilder styleBuilder;
    styleBuilder.append("html, body, div { display: block; }\n"s);
    styleBuilder.append("#break { break-after: page; }"s);
    styleBuilder.append(styleCase);
    auto finalStyle = styleBuilder.take();
    Io::SScan textScan{finalStyle};
    auto sheet = Style::StyleSheet::parse(textScan, ""_url);
    dom->styleSheets->add(std::move(sheet));

    Karm::StringBuilder contentBuilder;
    contentBuilder.append("<html><body>"s);
    for (usize i = 0; i < amountOfPages; ++i) {
        contentBuilder.append("<div id=\"break\">hi</div>"s);
    }
    contentBuilder.append("</body></html>"s);

    parser.write(contentBuilder.take());
}

Vec<Math::Vec2f> printedPaperSizes(Gc::Ref<Dom::Document> dom, Print::Settings settings) {
    auto pagesGen = Vaev::Driver::print(*dom, settings);

    Vec<Math::Vec2f> sizes;
    while (true) {
        auto next = pagesGen.next();
        if (not next)
            break;

        sizes.pushBack({next->_paper.width, next->_paper.height});
    }

    return sizes;
}

bool sizesAreEqual(
    Vec<Math::Vec2f> const& expected,
    Vec<Math::Vec2f> const& actual
) {
    if (expected.len() != actual.len())
        return false;

    for (usize i = 0; i < expected.len(); ++i) {
        auto diff = expected[i] - actual[i];
        if (not Math::epsilonEq(expected[i].x, actual[i].x, EPSILON) or
            not Math::epsilonEq(expected[i].y, actual[i].y, EPSILON)) {
            return false;
        }
    }

    return true;
}

test$("sanity-test") {
    Gc::Heap gc;

    auto dom = gc.alloc<Dom::Document>(Mime::Url());

    usize const amountOfPages = 3;

    buildTestCase(gc, dom, amountOfPages, "");

    Print::Settings settings = {
        .paper = Print::A4,
        .orientation = Print::Orientation::PORTRAIT,
    };

    Vec<Math::Vec2f> expectedPageSizes;
    for (usize i = 0; i < amountOfPages; ++i) {
        expectedPageSizes.pushBack({Print::A4.width, Print::A4.height});
    }

    auto actualPageSizes = printedPaperSizes(dom, settings);

    if (not sizesAreEqual(expectedPageSizes, actualPageSizes)) {
        logError("expected: {}, actual: {}", expectedPageSizes, actualPageSizes);
        expect$(false);
    }
    return Ok();
}

test$("page-as-landscape") {
    Gc::Heap gc;

    auto dom = gc.alloc<Dom::Document>(Mime::Url());

    usize const amountOfPages = 3;

    buildTestCase(gc, dom, amountOfPages, "@page { size: landscape; }"s);

    Print::Settings settings = {
        .paper = Print::A4,
        .orientation = Print::Orientation::PORTRAIT,
    };

    Vec<Math::Vec2f> expectedPageSizes;
    for (usize i = 0; i < amountOfPages; ++i) {
        expectedPageSizes.pushBack({Print::A4.height, Print::A4.width});
    }

    auto actualPageSizes = printedPaperSizes(dom, settings);

    if (not sizesAreEqual(expectedPageSizes, actualPageSizes)) {
        logError("expected: {}, actual: {}", expectedPageSizes, actualPageSizes);
        expect$(false);
    }

    return Ok();
}

test$("page-as-a5") {
    Gc::Heap gc;

    auto dom = gc.alloc<Dom::Document>(Mime::Url());

    usize const amountOfPages = 3;

    buildTestCase(gc, dom, amountOfPages, "@page { size: A5; }"s);

    Print::Settings settings = {
        .paper = Print::A4,
        .orientation = Print::Orientation::PORTRAIT,
    };

    Vec<Math::Vec2f> expectedPageSizes;
    for (usize i = 0; i < amountOfPages; ++i) {
        expectedPageSizes.pushBack({Print::A5.width, Print::A5.height});
    }

    auto actualPageSizes = printedPaperSizes(dom, settings);

    if (not sizesAreEqual(expectedPageSizes, actualPageSizes)) {
        logError("expected: {}, actual: {}", expectedPageSizes, actualPageSizes);
        expect$(false);
    }

    return Ok();
}

test$("page-left-right") {
    Gc::Heap gc;

    auto dom = gc.alloc<Dom::Document>(Mime::Url());

    usize const amountOfPages = 3;

    buildTestCase(
        gc, dom, amountOfPages,
        "@page:left { size: A3 portrait; }\n"s
        "@page:right { size: A5 landscape; }"s
    );

    Print::Settings settings = {
        .paper = Print::A4,
        .orientation = Print::Orientation::PORTRAIT,
    };

    Vec<Math::Vec2f> expectedPageSizes;
    for (usize i = 0; i < amountOfPages; ++i) {
        if (i % 2)
            expectedPageSizes.pushBack({Print::A3.width, Print::A3.height});
        else
            expectedPageSizes.pushBack({Print::A5.height, Print::A5.width});
    }

    auto actualPageSizes = printedPaperSizes(dom, settings);

    if (not sizesAreEqual(expectedPageSizes, actualPageSizes)) {
        logError("expected: {}, actual: {}", expectedPageSizes, actualPageSizes);
        expect$(false);
    }

    return Ok();
}

} // namespace Vaev::Driver::Tests
