#include <karm-print/page.h>
#include <karm-print/pdf-printer.h>
#include <karm-scene/transform.h>
#include <karm-sys/file.h>
#include <karm-test/macros.h>
#include <karm-ui/view.h>
#include <vaev-base/resolution.h>

#include "karm-scene/text.h"

namespace Karm::Print::Tests {

test$("karm-pdf-hello-world") {
    Rc<Text::Prose> prose{makeRc<Text::Prose>(Ui::TextStyles::bodyMedium(), "Hello, world!"s)};
    prose->_style.color = Gfx::BLACK;
    prose->layout(300);

    auto pageStack = makeRc<Scene::Stack>();
    pageStack->add(makeRc<Scene::Text>(
        Math::Vec2f{100, 100},
        prose
    ));

    // ----------------

    Print::PaperStock paper = Print::A4;
    Vaev::Resolution resolution{1, Vaev::Resolution::X};
    Print::Page page{
        paper,
        makeRc<Scene::Transform>(
            pageStack,
            Math::Trans2f::makeScale(resolution.toDppx())
        )
    };

    // -----------------

    auto pdfPrinter = makeRc<PdfPrinter>();
    page.print(
        *pdfPrinter,
        {
            .showBackgroundGraphics = true,
        }
    );

    // -----------------

    // MutCursor<Io::Writer> output = &Sys::out();
    auto outputUrl = try$(Mime::parseUrlOrPath("/home/paulo/Repos/paper-muncher/test.pdf"));
    auto x = try$(Sys::File::create(outputUrl));
    MutCursor<Io::Writer> output = &x;
    try$(pdfPrinter->write(*output));

    return Ok();
}

} // namespace Karm::Print::Tests
