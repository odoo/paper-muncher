# REPORT ENGINE F.A.Q

**A Bold Case for Odoo's New PDF Generation Tool and Browser Engine**

**"Can't we just fix wkhtmltopdf?"**

The challenges with wkhtmltopdf run deep, tangled in its networking code and a legacy C++ architecture that's resistant to change.  Fixing it would demand rare expertise in both QT and WebKit (massive codebases!) and carry high risk due to a lack of tests.  Multiple attempts at fixing it have fallen short – it's time for a fresh approach.

**"Can't we just use Chrome?"**

While Chrome excels at small documents, it struggles with larger ones, consuming exponentially more memory and time. Its limited support for page media specs and header/footer customization makes it a poor fit for report generation. When dealing with large documents, Chrome's memory consumption skyrockets, leading to crashes and hangs.  This is inherent to its design, optimized for interactive performance, not static media. Relying on Chrome would mean depending on hacks and workarounds that could break with every update.

**"Can't we just use Weasyprint?"**

Weasyprint's strong support for paged media and decent handling of static media in HTML/CSS is commendable.  However, its Python implementation makes it inherently slow.  The good news?  It's relatively small (~70k lines) and open source, so we can contribute and fix issues.

**"Can't we build your own?"**

Absolutely!  We can reimagine Weasyprint in a lower-level language like C++ for blazing-fast performance.  We've waited long enough for a better solution; it's time to take the reins. If Chrome Headless were a viable alternative, we would have switched years ago.

**Deep Integration and Open Source: It's the Odoo Way**

Building our own opens exciting doors:

* **Drop-in Replacement:** Seamlessly integrate with existing Odoo workflows.
* **Client-side PDF Generation:**  Enable the same code and results on both server and client using WASM.
* **Powerful Debugging Tools:**  Replace black boxes with transparent tools for page layout troubleshooting.
* **Community Contribution:**  Sharing this technology benefits the broader open-source community and enhances Odoo's reputation.

**"Outside of Odoo, what would be the benefits?"**

The open-source landscape lacks a truly exceptional PDF rendering engine. Weasyprint is too slow and struggles with HTML/CSS specs, wkhtmltopdf is outdated, Chrome is memory-intensive and falters with large documents, and the rest are either non-existent or poorly maintained. 

Moreover, all mainstream browser engines are either built by Google, utilize Google's engine, or receive funding from Google.  An independent alternative is crucial for a healthy web ecosystem.

**"How are you going to make this thing fast?"**

We're embracing cutting-edge ideas and technologies:

* **Data-Oriented Design**
* **Modern C++ Features:** Coroutines, modules, concepts
* **LibUring**
* **Streamlined Pipeline:** Easier optimization.
* **Smaller Objects:** Faster allocation.
* **Efficient Image Handling:** Extract metadata without full decoding for faster PDF generation.

**"But it's impossible to build a browser engine from scratch!"**

That's simply not true.  The web is well-defined, well-tested, and has a wealth of implementation experience to draw from (Blink, WebKit, Servo, etc.). We don't need to implement every feature, only what's relevant for our specific report generation needs.  A small, focused team can achieve incredible things.

**"There's a whole lot of new specs every year. It would take a whole team to implement and maintain the engine."**

Most new web specs focus on turning the web into an application platform.  We're laser-focused on document generation, an area where web evolution has slowed. We can prioritize features that matter most to us.

**"Why are you using C++? Are you dumb???"**

We're choosing pragmatism over ideals. C++ offers:

* **Scalability:**  Ideal for large projects.
* **Excellent Tooling:**  clangd, clang-tidy, clang-format, GCC, MSVC, Visual Studio, Clion, gdb, lldb, valgrind, etc.
* **Strong Typing:**  Enhances code safety and maintainability.
* **Familiarity:**  Easy for developers experienced in C-like languages (Java, C#, etc.) to adopt.
* **Safe Subset:**  We're using a carefully chosen subset of C++ to avoid common pitfalls.

**"What about the crabs? 🦀"** (Rust)

While Rust is intriguing, I have limited experience with it.  For this proof of concept, I need to move quickly without the learning curve of a new language.

**"Why are you building everything from scratch? Are you dum dum??"**

Building on top of an existing general-purpose browser engine would likely repeat wkhtmltopdf's mistakes.  These engines are complex, memory-hungry, and difficult to integrate deeply.  Building from scratch offers:

* **Tighter Integration**
* **Lower Overhead**
* **Fun and Learning**
* **Proof of Concept:**  We'll only deploy a polished, production-ready implementation.
* **Odoo's Track Record:**  We have a history of successful innovation (Owl, Spreadsheet, ORM, HOOT, etc.).

**"But no one at Odoo can maintain C++??"**

We're committed to writing clean, well-documented code with clear links to relevant specs. Any skilled programmer should be able to grasp the project's architecture quickly. It's a straightforward pipeline with data flowing in a single direction.

**"What is the scope of the POC?"**

The primary goal is to generate a substantial general ledger at least 100x faster than wkhtmltopdf, without compromising any steps in the HTML/CSS parsing, styling, and layout process.

**"What are the least features we need to make the first release?"**

* **HTML Parsing & DOM Construction**
* **CSS Parsing & Style Calculation** (At least CSS 2.2 for wkhtmltopdf compatibility)
* **Layout Engine**
* **Basic Text Rendering**
* **PDF Output**

**"What can we borrow from the knowledge/code of wkhtmltopdf to speed up the project?"**

We can leverage insights into wkhtmltopdf's layout quirks to anticipate and address similar challenges.  Direct code reuse is unlikely due to its architecture and potential licensing issues.

**"Can multiple report engines be supported simultaneously? When Owl was introduced, we used both the old and new framework simultaneously and improved Owl progressively."**

Yes, we can achieve this by:

* **Abstracting the Rendering Layer:** Create a standard interface in Odoo to interact with different rendering engines.
* **Progressive Rollout:** Initially offer both the new engine and wkhtmltopdf, gradually phasing out the latter as the new engine matures.
