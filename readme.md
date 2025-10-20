<br/>
<br/>

<p align="center">
<img src="doc/assets/logo-light.png#gh-light-mode-only" width="280" />
<img src="doc/assets/logo-dark.png#gh-dark-mode-only" width="280" />
</p>
<p align="center">
    Munch the web into crisp documents
</p>
<p align="center">
<a href="https://odoo.github.io/paper-muncher/">Website</a> -
<a href="https://discord.gg/4GC9nQnAvY">Discord</a> -
<a href="https://odoo.github.io/paper-muncher/usage.html">Documentation</a>
</p>

<br/>
<br/>

# Status

Paper-Muncher is now in early alpha. We're currently focused on improving stability and ensuring compliance. While it's still early days, the project is functional enough to try out, and we're actively looking for feedback. The direction may still evolve, but you can already get a feel for where we're headed. Let us know what you think on [Discord](https://discord.gg/4GC9nQnAvY).

# Installation

1. **Download** the prebuilt package for your distribution from the [nightly releases](https://github.com/odoo/paper-muncher/releases/tag/nightly).
2. **Install** the package using your system's package manager.
3. **Add** `/opt/paper-muncher/bin` to your `PATH` environment variable.
4. **Verify** the installation by running `paper-muncher --help` to see available options.
5. **Learn more** by visiting the [official documentation](https://odoo.github.io/paper-muncher/usage.html).

# Basic usage

```bash
paper-muncher index.html -o output.pdf
```

# Introduction

In the beginning there was the Web, a sprawling chaos of information, a cacophony of code. And from this discord, a document tyrant arose: wkhtmltopdf. It ruled with an iron fist, its rendering slow, its output clunky. The chosen ones, the PDF wranglers, toiled in misery. But I… I bore witness.

I saw the crashes, the memory leaks, the endless command-line wrangling. The pleas for a simpler path went unanswered. The tyrant wkhtmltopdf cared not for their suffering. In their despair, they craved a savior. A champion. A… Paper Muncher.

From the ashes of frustration, a new tool rose. Forged in the fires of programmer ingenuity, the Paper Muncher arrived. It devoured webpages whole, spitting forth crisp, clean PDFs with ease. No more command-line incantations, no more cryptic errors. Just pure, unadulterated PDF conversion.

The reign of wkhtmltopdf is over. The Paper Muncher has come. Let its name be etched in the annals of document creation, a beacon of hope for the weary PDF wranglers.  Prepare to be Munched!

# Building

> **⚠ Warning**<br> Paper Muncher is currently in the early stages of development and is not yet ready for use. Here be dragons! 🐉

```sh
# Clone the repository
git clone https://github.com/odoo/paper-muncher

# Build and install the project
cd paper-muncher
./ck tools setup
./ck install --release --prefix=$HOME/.local paper-muncher

# Add the binary to your PATH, add this to your .bashrc to make it permanent
export PATH=$PATH:$HOME/.local/bin

# Render a webpage to PDF
paper-muncher index.html -o output.pdf

# For more options, run
paper-muncher --help
```

## Contributing

We welcome contributions to the Paper Muncher project! If you have ideas, suggestions, or bug reports, please open an issue on our GitHub repository. If you're interested in contributing code, please fork the repository and submit a pull request.

## License

The paper muncher document generation tool and its core components are licensed under the **GNU Lesser General Public License v3.0 or later**.

The full text of the license can be accessed via [this link](https://www.gnu.org/licenses/lgpl-3.0-standalone.html) and is also included in the [license.txt](license.txt) file of this software package.
