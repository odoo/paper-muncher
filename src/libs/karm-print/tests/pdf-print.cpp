#include <karm-print/page.h>
#include <karm-print/pdf-printer.h>
#include <karm-scene/transform.h>
#include <karm-test/macros.h>
#include <karm-ui/view.h>
#include <vaev-base/resolution.h>

#include "karm-scene/text.h"

namespace Karm::Print::Tests {

test$("karm-pdf-hello-world") {
    Mime::Uti outputFormat = Mime::Uti::PUBLIC_PDF;
    Print::PaperStock paper = Print::A4;
    Vaev::Resolution resolution{1, Vaev::Resolution::X};

    Rc<Text::Prose> prose{makeRc<Text::Prose>(Ui::TextStyles::bodyMedium(), "Hello, world!"s)};
    prose->_style.color = Gfx::BLACK;
    prose->layout(300);

    auto pdfPrinter = try$(FilePrinter::create(
        outputFormat,
        FilePrinterProps{
            .density = 1,
        }
    ));

    auto pageStack = makeRc<Scene::Stack>();
    pageStack->add(makeRc<Scene::Text>(
        Math::Vec2f{100, 100},
        prose
    ));

    Print::Page page{
        paper,
        makeRc<Scene::Transform>(
            pageStack,
            Math::Trans2f::makeScale(resolution.toDppx())
        )
    };

    page.print(
        *pdfPrinter,
        {
            .showBackgroundGraphics = true,
        }
    );

    MutCursor<Io::Writer> output = &Sys::out();
    try$(pdfPrinter->write(*output));

    return Ok();
}

} // namespace Karm::Print::Tests
