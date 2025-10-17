#include <karm-sys/entry.h>

import Karm.Cli;
import PaperMuncher;
import Karm.Print;
import Karm.Logger;

import Vaev.Engine;

using namespace Karm;

template <>
struct Cli::ValueParser<PaperMuncher::Overflow> {
    static Res<> usage(Io::TextWriter& w) {
        return w.writeStr("auto|paginate|crop|visible"s);
    }

    static Res<PaperMuncher::Overflow> parse(Cursor<Token>& c) {
        if (c.ended() or c->kind != Token::OPERAND)
            return Error::invalidInput("expected overflow");

        auto value = c.next().value;

        if (value == "auto")
            return Ok(PaperMuncher::Overflow::AUTO);
        else if (value == "paginate")
            return Ok(PaperMuncher::Overflow::PAGINATE);
        else if (value == "crop")
            return Ok(PaperMuncher::Overflow::CROP);
        else if (value == "visible")
            return Ok(PaperMuncher::Overflow::VISIBLE);
        else
            return Error::invalidInput("expected overflow");
    }
};

template <>
struct Cli::ValueParser<Print::Margins> {
    static Res<> usage(Io::TextWriter& w) {
        return w.writeStr("default|none|minimum"s);
    }

    static Res<Print::Margins> parse(Cursor<Token>& c) {
        if (c.ended() or c->kind != Token::OPERAND)
            return Error::invalidInput("expected margin");

        auto value = c.next().value;

        if (value == "default")
            return Ok(Print::Margins::DEFAULT);
        else if (value == "none")
            return Ok(Print::Margins::NONE);
        else if (value == "minimum")
            return Ok(Print::Margins::MINIMUM);
        else
            return Error::invalidInput("expected margin");
    }
};

Async::Task<> entryPointAsync(Sys::Context& ctx) {
    auto sandboxedArg = Cli::flag(NONE, "sandboxed"s, "Disallow local file and http access"s);
    auto verboseArg = Cli::flag(NONE, "verbose"s, "Makes paper-muncher be more talkative, it might yap about how its day's going"s);
    Cli::Section runtimeSection{
        "Runtime Options"s,
        {sandboxedArg, verboseArg},
    };

    auto inputsArg = Cli::operand<Vec<Str>>("inputs"s, "Input files (default: stdin)"s, {"-"s});
    auto outputArg = Cli::option<Str>('o', "output"s, "Output file (default: stdout)"s, "-"s);
    auto formatArg = Cli::option<Str>('f', "format"s, "Override the output file format"s, ""s);
    auto densityArg = Cli::option<Str>(NONE, "density"s, "Density of the output document in css units (e.g. 96dpi)"s, "1x"s);
    auto overflowArg = Cli::option<PaperMuncher::Overflow>(NONE, "overflow"s, "What to do when content exceeds the page. (default: paginate for PDF, otherwise crop)"s, PaperMuncher::Overflow::AUTO);
    Cli::Section inOutSection{
        "Input/Output Options"s,
        {inputsArg, outputArg, formatArg, densityArg, overflowArg},
    };

    auto paperArg = Cli::option<Str>(NONE, "paper"s, "Paper size for printing (default: A4)"s, "A4"s);
    auto orientationArg = Cli::option<Str>(NONE, "orientation"s, "Page orientation (default: portrait)"s, "portrait"s);
    auto marginArg = Cli::option<Print::Margins>(NONE, "margins"s, "Page margins (default: default)"s, Print::Margins::DEFAULT);
    Cli::Section paperSection{
        "Paper Options"s,
        {paperArg, orientationArg, marginArg},
    };

    auto widthArg = Cli::option<Str>(NONE, "width"s, "Width of the output document in css units (e.g. 800px)"s, ""s);
    auto heightArg = Cli::option<Str>(NONE, "height"s, "Height of the output document in css units (e.g. 600px)"s, ""s);
    auto scaleArg = Cli::option<Str>(NONE, "scale"s, "Scale of the input document in css units (e.g. 1x)"s, "1x"s);
    Cli::Section viewportSection{
        "Viewport Options"s,
        {widthArg, heightArg, scaleArg},
    };

    Cli::Section formatSection{
        .title = "Supported Formats"s,
        .prolog =
            "Input: HTML, XHTML, SVG\n"
            "Output: PDF or image\n"
            "Image formats: BMP, PNG, JPEG, TGA, QOI, SVG\n"s
    };

    Cli::Command cmd{
        "paper-muncher"s,
        "Convert web pages (HTML, XHTML, or SVG) into printable or viewable documents like PDFs or images."s,
        {
            runtimeSection,
            inOutSection,
            paperSection,
            viewportSection,
            formatSection,
        },
        [=](Sys::Context&) -> Async::Task<> {
            setLogLevel(verboseArg.value() ? PRINT : ERROR);
            if (sandboxedArg.value())
                co_try$(Sys::enterSandbox());

            PaperMuncher::Option options{};

            options.scale = co_try$(Vaev::parseValue<Vaev::Resolution>(scaleArg.value()));
            options.density = co_try$(Vaev::parseValue<Vaev::Resolution>(densityArg.value()));

            if (widthArg.value())
                options.width = co_try$(Vaev::parseValue<Vaev::Length>(widthArg.value()));

            if (heightArg.value())
                options.height = co_try$(Vaev::parseValue<Vaev::Length>(heightArg.value()));

            options.paper = co_try$(Print::findPaperStock(paperArg.value()));
            options.orientation = co_try$(Vaev::parseValue<Print::Orientation>(orientationArg.value()));
            options.margins = marginArg.value();

            Vec<Ref::Url> inputs;
            for (auto& i : inputsArg.value())
                if (i == "-"s)
                    inputs.pushBack("fd:stdin"_url);
                else
                    inputs.pushBack(Ref::parseUrlOrPath(i, co_try$(Sys::pwd())));

            Ref::Url output = "fd:stdout"_url;
            if (outputArg.value() != "-"s)
                output = Ref::parseUrlOrPath(outputArg.value(), co_try$(Sys::pwd()));

            if (formatArg.value() != ""s) {
                options.outputFormat = co_try$(Ref::Uti::fromMime({formatArg.value()}));
            } else {
                auto mime = Ref::sniffSuffix(output.path.suffix());
                options.outputFormat = mime ? co_try$(Ref::Uti::fromMime(*mime)) : Ref::Uti::PUBLIC_PDF;
            }

            options.overflow = overflowArg.value();

            auto client = PaperMuncher::defaultHttpClient(sandboxedArg.value());
            co_return co_await PaperMuncher::run(client, inputs, output, options);
        }
    };

    co_return co_await cmd.execAsync(ctx);
}
