#include <karm-io/emit.h>
#include <karm-io/funcs.h>
#include <karm-print/pdf.h>
#include <karm-sys/entry.h>
#include <karm-sys/file.h>
#include <karm-sys/time.h>
#include <karm-ui/app.h>
#include <vaev-driver/fetcher.h>
#include <vaev-driver/render.h>
#include <vaev-markup/html.h>
#include <vaev-markup/xml.h>

#include "inspector.h"

namespace PaperMuncher {

namespace Css {

Res<> dumpStylesheet(Mime::Url const &url) {
    auto start = Sys::now();
    auto stylesheet = try$(Vaev::Driver::fetchStylesheet(url));
    auto elapsed = Sys::now() - start;
    logInfo("fetched in {}ms", elapsed.toUSecs() / 1000.0);
    Sys::println("{#}", stylesheet);
    return Ok();
}

Res<> dumpSst(Mime::Url const &url) {
    auto file = try$(Sys::File::open(url));
    auto buf = try$(Io::readAllUtf8(file));

    auto start = Sys::now();

    Io::SScan s{buf};
    Vaev::Css::Lexer lex{s};
    Vaev::Css::Sst sst = Vaev::Css::consumeRuleList(lex, true);

    auto elapsed = Sys::now() - start;
    logInfo("parsed in {}ms", elapsed.toUSecs() / 1000.0);

    Sys::println("{}", sst);

    return Ok();
}

Res<> dumpTokens(Mime::Url const &url) {
    auto file = try$(Sys::File::open(url));
    auto buf = try$(Io::readAllUtf8(file));
    Io::SScan s{buf};
    Vaev::Css::Lexer lex{s};
    while (not lex.ended())
        Sys::println("{}", lex.next());
    return Ok();
}

} // namespace Css

namespace Style {

Res<> listProps() {
    Vaev::Style::StyleProp::any([]<typename T>(Meta::Type<T>) {
        Sys::println("{}", T::name());
        return false;
    });
    return Ok();
}

} // namespace Style

namespace Markup {

Res<> dumpDom(Mime::Url const &url) {
    auto dom = try$(Vaev::Driver::fetchDocument(url));
    Sys::println("{}", dom);
    return Ok();
}

Res<> dumpTokens(Mime::Url const &url) {
    auto file = try$(Sys::File::open(url));
    auto buf = try$(Io::readAllUtf8(file));

    Vec<Vaev::Markup::HtmlToken> tokens;

    struct VecSink : public Vaev::Markup::HtmlSink {
        Vec<Vaev::Markup::HtmlToken> &tokens;

        VecSink(Vec<Vaev::Markup::HtmlToken> &tokens)
            : tokens(tokens) {
        }

        void accept(Vaev::Markup::HtmlToken const &token) override {
            tokens.pushBack(token);
        }
    };

    VecSink sink{tokens};
    Vaev::Markup::HtmlLexer lexer{};
    lexer.bind(sink);

    for (auto r : iterRunes(buf))
        lexer.consume(r);

    for (auto &t : tokens)
        Sys::println("{}", t);

    return Ok();
}

} // namespace Markup

namespace Html2pdf {

Vaev::Style::Media constructMedia(Print::PaperStock paper) {
    return {
        .type = Vaev::MediaType::SCREEN,
        .width = Vaev::Px{paper.width},
        .height = Vaev::Px{paper.height},
        .aspectRatio = paper.width / paper.height,
        .orientation = Vaev::Orientation::LANDSCAPE,

        .resolution = Vaev::Resolution::fromDpi(96),
        .scan = Vaev::Scan::PROGRESSIVE,
        .grid = false,
        .update = Vaev::Update::NONE,

        .overflowBlock = Vaev::OverflowBlock::PAGED,
        .overflowInline = Vaev::OverflowInline::NONE,

        .color = 8,
        .colorIndex = 0,
        .monochrome = 0,
        .colorGamut = Vaev::ColorGamut::SRGB,
        .pointer = Vaev::Pointer::NONE,
        .hover = Vaev::Hover::NONE,
        .anyPointer = Vaev::Pointer::NONE,
        .anyHover = Vaev::Hover::NONE,

        .prefersReducedMotion = Vaev::ReducedMotion::REDUCE,
        .prefersReducedTransparency = Vaev::ReducedTransparency::REDUCE,
        .prefersContrast = Vaev::Contrast::MORE,
        .forcedColors = Vaev::Colors::NONE,
        .prefersColorScheme = Vaev::ColorScheme::LIGHT,
        .prefersReducedData = Vaev::ReducedData::NO_PREFERENCE,
    };
}

Res<> html2pdf(Mime::Url const &input, Mime::Url const &output) {
    auto dom = try$(Vaev::Driver::fetchDocument(input));

    auto paper = Print::A4;
    auto media = constructMedia(paper);

    auto start = Sys::now();
    auto [layout, paint] = Vaev::Driver::render(*dom, media, paper);
    auto elapsed = Sys::now() - start;
    logInfo("render time: {}", elapsed);

    logDebug("layout tree: {}", layout);
    logDebug("paint tree: {}", paint);

    Print::PdfPrinter printer{Print::A4, Print::Density::DEFAULT};
    paint->print(printer);

    auto file = try$(Sys::File::create(output));
    Io::TextEncoder<> encoder{file};
    Io::Emit e{encoder};
    printer.write(e);
    try$(e.flush());

    return Ok();
}

} // namespace Html2pdf

} // namespace PaperMuncher

Async::Task<> entryPointAsync(Sys::Context &ctx) {
    auto args = Sys::useArgs(ctx);

    if (args.len() == 0) {
        Sys::errln("usage: paper-muncher <verb> [OPTIONS...]\n");
        co_return Error::invalidInput();
    }

    auto verb = args[0];

    if (verb == "css-dump-stylesheet") {
        if (args.len() != 2) {
            Sys::errln("usage: paper-muncher css-dump-stylesheet <style-sheet url>");
            co_return Error::invalidInput();
        }

        auto input = co_try$(Mime::parseUrlOrPath(args[1]));
        co_return PaperMuncher::Css::dumpStylesheet(input);

    } else if (verb == "css-dump-sst") {
        if (args.len() != 2) {
            Sys::errln("usage: paper-muncher css-dump-sst <style-sheet url>");
            co_return Error::invalidInput();
        }

        auto input = co_try$(Mime::parseUrlOrPath(args[1]));
        co_return PaperMuncher::Css::dumpSst(input);
    } else if (verb == "css-dump-tokens") {
        if (args.len() != 2) {
            Sys::errln("usage: paper-muncher css-dump-tokens <style-sheet url>");
            co_return Error::invalidInput();
        }

        auto input = co_try$(Mime::parseUrlOrPath(args[1]));
        co_return PaperMuncher::Css::dumpTokens(input);
    } else if (verb == "style-list-props") {
        co_return PaperMuncher::Style::listProps();
    } else if (verb == "markup-dump-dom") {
        if (args.len() != 2) {
            Sys::errln("usage: paper-muncher markup-dump-dom <markup file url>");
            co_return Error::invalidInput();
        }

        auto input = co_try$(Mime::parseUrlOrPath(args[1]));
        co_return PaperMuncher::Markup::dumpDom(input);
    } else if (verb == "markup-dump-tokens") {
        if (args.len() != 2) {
            Sys::errln("usage: paper-muncher markup-dump-token <html file url>");
            co_return Error::invalidInput();
        }

        auto input = co_try$(Mime::parseUrlOrPath(args[1]));
        co_return PaperMuncher::Markup::dumpTokens(input);
    } else if (verb == "html2pdf") {
        if (args.len() != 3) {
            Sys::errln("usage: paper-muncher html2pdf <input.html> <output.pdf>\n");
            co_return Error::invalidInput();
        }

        auto input = co_try$(Mime::parseUrlOrPath(args[1]));
        auto output = co_try$(Mime::parseUrlOrPath(args[2]));
        co_return PaperMuncher::Html2pdf::html2pdf(input, output);
    } else if (verb == "inspector") {
        if (args.len() != 2) {
            Sys::errln("usage: paper-muncher inspector <html file url>");
            co_return Error::invalidInput();
        }

        auto input = args.len()
                         ? co_try$(Mime::parseUrlOrPath(args[1]))
                         : "about:start"_url;

        auto dom = Vaev::Driver::fetchDocument(input);

        co_return Ui::runApp(
            ctx,
            PaperMuncher::Inspector::app(input, dom) | Ui::inspector
        );
    } else {
        Sys::errln("unknown verb: {} (expected: css-dump-stylesheet, css-dump-sst, css-dump-tokens, style-list-props, markup-dump-dom, markup-dump-tokens, inspector)\n");
        co_return Error::invalidInput();
    }
}
