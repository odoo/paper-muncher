<br/>
<br/>

<p align="center">
<img src="doc/assets/logo-light.png#gh-light-mode-only" width="280" />
<img src="doc/assets/logo-dark.png#gh-dark-mode-only" width="280" />
</p>
<p align="center">
    Munch the web into crisp documents
</p>

<br/>
<br/>

# Status

Paper-Muncher is now in early alpha. We're currently focused on improving stability and ensuring compliance. While it's still early days, the project is functional enough to try out, and we're actively looking for feedback. The direction may still evolve, but you can already get a feel for where we're headed. Let us know what you think in the github issues!

# Introduction

In the beginning there was the Web, a sprawling chaos of information, a cacophony of code. And from this discord, a document tyrant arose: wkhtmltopdf. It ruled with an iron fist, its rendering slow, its output clunky. The chosen ones, the PDF wranglers, toiled in misery. But I… I bore witness.

I saw the crashes, the memory leaks, the endless command-line wrangling. The pleas for a simpler path went unanswered. The tyrant wkhtmltopdf cared not for their suffering. In their despair, they craved a savior. A champion. A… Paper Muncher.

From the ashes of frustration, a new tool rose. Forged in the fires of programmer ingenuity, the Paper Muncher arrived. It devoured webpages whole, spitting forth crisp, clean PDFs with ease. No more command-line incantations, no more cryptic errors. Just pure, unadulterated PDF conversion.

The reign of wkhtmltopdf is over. The Paper Muncher has come. Let its name be etched in the annals of document creation, a beacon of hope for the weary PDF wranglers.  Prepare to be Munched!

# Installation

> **⚠ Warning**<br> Paper Muncher is currently in the early stages of development and is not yet ready for use. Here be dragons! 🐉

```sh
# Clone the repository
git clone https://github.com/odoo/paper-muncher

# Build and install the project
cd paper-muncher
./ck package install --release --prefix=$HOME/.local

# Add the binary to your PATH, add this to your .bashrc to make it permanent
export PATH=$PATH:$HOME/.local/bin

# Render a webpage to PDF
paper-muncher print index.html -o output.pdf

# For more options, run
paper-muncher --help
```

## License

The paper muncher document generation tool and its core components are licensed under the **GNU Lesser General Public License v3.0 or later**.

The full text of the license can be accessed via [this link](https://www.gnu.org/licenses/lgpl-3.0-standalone.html) and is also included in the [license.txt](license.txt) file of this software package.
