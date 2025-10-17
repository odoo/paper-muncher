# Usage

**Paper-Muncher** is a document rendering tool that converts web documents into high-quality **PDFs** or **rasterized images** using the Vaev layout engine. It supports HTTP and local I/O, and CSS units for layout configuration.


```
paper-muncher <input> -o <output> [options]
```

Renders a web document to a print-ready file (typically PDF).

**Options:**

- `--scale <scale>`: Logical resolution (default: `1x`)
- `--density <density>`: Output pixel density (default: `1x`)
- `--paper <name>`: Paper stock (default: `A4`)
- `--orientation <orientation>`: `portrait` or `landscape` (default: `portrait`)
- `-w,--width <length>`: Override paper width (e.g. `210mm`, `8.5in`)
- `-h,--height <length>`: Override paper height
- `-f,--format <uti>`: Output format (default: `public.pdf`)
- `-o,--output <output>`: Output file or URL (default: stdout)
- `--sandboxed`: Disallows paper-muncher to access local files and the network.
- `--timeout <duration>`: Adds a timeout to the document generation, when reached exits with a failure code.
- `-v,--verbose`: Makes paper-muncher be more talkative, it might yap about how its day's going

**Examples:**

```sh
paper-muncher article.html -o out.pdf
paper-muncher article.html -o out.pdf --paper Letter --orientation landscape
paper-muncher article.html -o https://example.com/doc.pdf --output-mime application/pdf
```

*NOTE: `<scale>`, `<density>`, `<orientation>`, `<length>`, `<duration>` all use [CSS unit synthax](https://developer.mozilla.org/en-US/docs/Web/CSS/CSS_Values_and_Units#units)*

