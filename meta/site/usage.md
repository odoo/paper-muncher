# Usage

**Paper-Muncher** is a next-generation document generation tool designed for rendering documents to different formats, including PDFs and images. While there is no official way to install it yet, users who wish to use it will need to build it from the source code. 

## Available Commands

1. **Printing to PDF:**

   ```
   paper-muncher print <input> -o <output> [options]
   ```

   This command renders the specified document to a PDF file. It accepts several options for inspecting various internal states during rendering:

   - `-s, --dump-style`: Dumps the stylesheet that was applied during rendering.
   - `-d, --dump-dom`: Dumps the DOM tree of the document.
   - `-l, --dump-layout`: Dumps the layout tree after the document is laid out.
   - `-p, --dump-paint`: Dumps the paint tree, showing how the document would be rendered visually.

   **Examples:**
   - Basic usage:
     ```
     paper-muncher print document.html -o output.pdf
     ```
   - Inspecting layout during PDF generation:
     ```
     paper-muncher print document.html -o output.pdf --dump-layout
     ```

2. **Rendering to Image:**

   ```
   paper-muncher render <input> -o <output> [options]
   ```

   This command renders the specified document to an image format such as PNG. It accepts additional options for setting the output dimensions and inspecting various internal states:

   - `-w, --width`: Sets the width of the output image. Default is 800 pixels.
   - `-h, --height`: Sets the height of the output image. Default is 600 pixels.
   - `-s, --dump-style`: Dumps the stylesheet that was applied during rendering.
   - `-d, --dump-dom`: Dumps the DOM tree of the document.
   - `-l, --dump-layout`: Dumps the layout tree after the document is laid out.
   - `-p, --dump-paint`: Dumps the paint tree, showing how the document would be rendered visually.

   **Examples:**
   - Basic usage:
     ```
     paper-muncher render document.html -o output.png
     ```
   - Custom image dimensions:
     ```
     paper-muncher render document.html -o output.png --width 1024 --height 768
     ```

3. **CSS Commands:**

   Paper-Muncher provides tools to inspect and debug CSS files:

   - **Dump Stylesheet:**
     ```
     paper-muncher css dump-stylesheet <input>
     ```
     Dumps the complete stylesheet of the input file.

   - **Dump Style Syntax Tree (SST):**
     ```
     paper-muncher css dump-sst <input>
     ```
     Dumps the parsed syntax tree of the stylesheet.

   - **Dump CSS Tokens:**
     ```
     paper-muncher css dump-tokens <input>
     ```
     Displays individual CSS tokens parsed from the input file.

4. **Style Commands:**

   - **List Style Properties:**
     ```
     paper-muncher style list-props
     ```
     Lists all the style properties that are recognized and supported by Paper-Muncher.

5. **Markup Commands:**

   Commands to debug and inspect HTML or XML-based documents:

   - **Dump DOM:**
     ```
     paper-muncher markup dump-dom <input>
     ```
     Dumps the DOM tree of the input document.

   - **Dump Markup Tokens:**
     ```
     paper-muncher markup dump-tokens <input>
     ```
     Displays individual tokens parsed from the markup input.

6. **Inspector Mode:**

   ```
   paper-muncher inspect <input>
   ```

   Launches a UI-based inspector for visual debugging of the document. This allows you to see the rendered result, inspect individual elements, and debug layout and styling issues interactively.
