# Usage

**Paper-Muncher** is a document rendering tool that converts web documents into high-quality **PDFs** or **rasterized images** using the Vaev layout engine. It supports HTTP and local I/O, and CSS units for layout configuration.

## Commands

---

### `print`

```
paper-munch print <input> -o <output> [options]
```

Renders a web document to a print-ready file (typically PDF).

**Options:**

- `--scale <scale>`: Logical resolution (default: `1x`)
- `--density <density>`: Output pixel density (default: `1x`)
- `--paper <name>`: Paper stock (default: `A4`)
- `--orientation <orientation>`: `portrait` or `landscape` (default: `portrait`)
- `--width <length>`: Override paper width (e.g. `210mm`, `8.5in`)
- `--height <length>`: Override paper height
- `--output-mime <mime>`: Output format (default: `application/pdf`)
- `-o <output>`: Output file or URL (default: stdout)

**Examples:**

```sh
paper-munch print article.html -o out.pdf
paper-munch print article.html -o out.pdf --paper Letter --orientation landscape
paper-munch print article.html -o https://example.com/doc.pdf --output-mime application/pdf
```

---

### `render`

```
paper-munch render <input> -o <output> [options]
```

Renders a web document to a raster image (BMP, PNG, etc.).

**Options:**

- `--scale <scale>`: CSS resolution (default: `96dpi`)
- `--density <density>`: Pixel density (default: `96dpi`)
- `--width <length>`: Viewport width (default: `800px`)
- `--height <length>`: Viewport height (default: `600px`)
- `--output-mime <mime>`: Output format (default: `image/bmp`)
- `--wireframe`: Show wireframe overlay of the layout
- `-o <output>`: Output file or URL (default: stdout)

**Examples:**

```sh
paper-munch render page.html -o out.bmp
paper-munch render page.html -o out.png --width 1024px --height 768px --density 192dpi
paper-munch render page.html -o out.png --wireframe
```

