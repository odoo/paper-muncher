# Usage

**Paper-Muncher** is a document rendering tool that converts web pages (HTML, XHTML, SVG, or Markdown) into printable or
viewable documents such as **PDFs** or **rasterized images** using the Vaev layout engine. It supports CSS units for
layout configuration.

```
paper-muncher <inputs> [options...]
```

Renders one or more web documents to a print-ready file (typically PDF).

**Common Options:**

- `-h, --help`: Show the help message and exit
- `-u, --usage`: Show the usage message and exit
- `-v, --version`: Show version information and exit

**Runtime Options:**

- `--sandboxed`: Disallow access to local files and the network
- `--verbose`: Enable verbose logging, it might yap about how its day's going
- `--quiet`: Suppress all logging except fatal errors

**Input/Output Options:**

- `-o, --output <output>`: Output file (default: stdout)
- `--batch <mode>`: How to handle multiple input documents (default: `concat`). With multiple inputs, batch mode
  `separate` writes one output file per input, named after the source file
- `-f, --format <format>`: Override the output format (default: inferred from the output file extension)
- `--density <density>`: Pixel density of the output document, in CSS resolution units (e.g. `96dpi`)

**Paper Options:**

- `--paper <name>`: Paper size of the output pages (default: `A4`). Use `--paper list` to display all supported standard
  paper sizes
- `--orientation <orientation>`: `portrait` or `landscape` (default: `portrait`)
- `--margins <margins>`: Page margins, in CSS units or as a named preset (default: `default`)
- `--background <color>`: Background color of the output document (default: white for HTML, transparent for SVG)

**Viewport Options:**

- `--width <length>`: Viewport width, in CSS units (e.g. `800px`)
- `--height <length>`: Viewport height, in CSS units (e.g. `600px`)
- `--scale <scale>`: Scale factor applied to the input document (e.g. `1x`)

*Explicit `--width` and `--height` values take precedence over the `--paper` dimensions.*

**Document Decoration:**

- `--header <document>`: Document to render as the page header
- `--header-size <size>`: Height of the page header (default: `auto`)
- `--footer <document>`: Document to render as the page footer
- `--footer-size <size>`: Height of the page footer (default: `auto`)

*Headers and footers repeat on every page, above and below the main content, within the page margins.*

**Document Flow:**

- `--flow <mode>`: How content flows across pages (default: `auto`)
    - `auto`: Paginate when producing a PDF, continuous otherwise
    - `paginated`: Content exceeding the viewport is split across additional pages
    - `continuous`: The viewport grows to fit all content on a single page

**Document Extend:**

- `--extend <mode>`: How content overflowing the initial viewport is handled (default: `crop`)
    - `crop`: Overflowing content is clipped to the container
    - `fit`: The container is resized to fit the content

**Developer Options:**

- `--debug <flags>`: Toggle or list debug flags by name or wildcard (e.g. `karm-*=on` or `list`)
- `--feature <flags>`: Toggle or list features by name or wildcard (e.g. `karm-*=on` or `list`)

**Supported Formats:**

- Input: HTML, XHTML, SVG, Markdown
- Output: PDF or image (BMP, PNG, JPEG, TGA, QOI, SVG)

**Examples:**

```sh
paper-muncher article.html -o out.pdf
paper-muncher article.html -o out.pdf --paper Letter --orientation landscape
paper-muncher article.md -o out.png --width 800px --extend fit
paper-muncher ch1.html ch2.html ch3.html -o book.pdf
paper-muncher page.html -o out.pdf --header header.html --footer footer.html
```

*NOTE: `<scale>`, `<density>`, `<length>`, `<margins>`, `<size>` all
use [CSS unit syntax](https://developer.mozilla.org/en-US/docs/Web/CSS/CSS_Values_and_Units#units)*